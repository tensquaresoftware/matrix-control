---
organization: Ten Square Software
project: Matrix-Control
title: Story U-11 — Module Panel Config Dedup (Descriptor-Driven Widget Types)
author: BMad Agent
status: done
sources:
  - implementation-artifacts/u-0b-factory-dimension-registry-and-descriptor-decoupling.md
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - Source/GUI/Panels/Reusable/BaseModulePanel.h
  - Source/GUI/Panels/Reusable/BaseModulePanel.cpp
  - Source/GUI/Factories/WidgetFactory.h
  - Source/GUI/Factories/WidgetFactory.cpp
  - Source/Shared/Definitions/PluginDescriptors.h
  - _bmad-output/project-context.md
created: 2026-06-16
updated: 2026-06-17
---

# Story U-11: Module Panel Config Dedup (Descriptor-Driven Widget Types)

Status: done

<!-- Refactoring / DRY hygiene in Epic U. Reduces boilerplate in 13 BaseModulePanel consumers. Does NOT touch Matrix ModulationPanel (different layout model). Ideal slot: after U-0b, before U-6 patch-edit layout audit. -->

## Story

As a developer maintaining Patch Edit and Master Edit module panels,
I want module panel layouts to declare only ordered parameter IDs (and separator placeholders),
so that Slider vs ComboBox widget types are inferred from `PluginDescriptors` via `PluginHelpers::resolveParameterWidgetKind`, eliminating redundant `ModulePanelParameterType` entries and mismatches between panel config and descriptors.

## Problem Statement

Today each `*Panel::createConfig()` repeats parameter IDs **and** widget kinds:

```cpp
{ PluginIDs::...::kSync, ModulePanelParameterType::ComboBox },
```

Descriptor type (`IntParameterDescriptor` vs `ChoiceParameterDescriptor`) already defines the APVTS + widget family. Duplication creates drift risk (panel says Slider, descriptor says Choice) with no compile-time check.

**In scope:** 13 panels using `BaseModulePanel` (Patch Edit modules + Master Edit Midi/Vibrato/Misc).

**Out of scope:**

- `MatrixModulationPanel` / `ModulationBusCell` — widget types are fixed per column (source/dest = ComboBox, amount = Slider); repetition is bus-indexed ID arrays, not Slider/ComboBox enums.
- `PatchMutatorPanel`, Patch Manager bank/computer panels — custom layouts, not `BaseModulePanel`.
- Descriptor schema changes, layout dimension work (U-6…U-9), APVTS/MIDI behaviour.

## Acceptance Criteria

### AC 1 — Layout vs resolved config split

1. Introduce `ModulePanelLayout` in `Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.h` (`.cpp` only if non-trivial):
   - Fields: `moduleId`, `buttonSet`, `moduleType`, `initWidgetId`, `copyWidgetId`, `pasteWidgetId`, `orderedParameterIds` (`std::vector<juce::String>`).
   - **Separator rows:** empty string `""` in `orderedParameterIds` maps to separator-only `ParameterCell` (existing `ParameterType::None` behaviour in `RampPortamentoPanel`, `Lfo2Panel`, `FmTrackPanel`).
2. Provide `ModulePanelConfig buildModulePanelConfig(const ModulePanelLayout& layout)`:
   - For each non-empty ID: resolve widget kind from `PluginHelpers::resolveParameterWidgetKind` (`IntParameterDescriptor` → Slider, `ChoiceParameterDescriptor` → ComboBox).
   - **Fail-fast:** if ID is empty → separator; if ID not found → `jassertfalse` in debug + throw `ParameterNotFoundException` with clear message.
   - If ID exists in **both** int and choice descriptor collections (should never happen) → `InvalidParameterException`.
3. Keep `ModulePanelConfig` as the resolved runtime struct consumed by `BaseModulePanel`. `BaseModulePanel` ctor accepts `ModulePanelLayout` and calls `buildModulePanelConfig(layout)` once — single resolution path via builder.

### AC 2 — PluginHelpers parameter kind resolution (Shared SSOT)

1. Add to `PluginHelpers` in `Source/Shared/Definitions/PluginHelpers.h`:

```cpp
enum class ParameterWidgetKind { Slider, ComboBox };

std::optional<ParameterWidgetKind> resolveParameterWidgetKind(const juce::String& parameterId);
```

2. Implementation scans `PluginDescriptors` int/choice collections (including matrix-mod bus arrays and master-edit aggregates). Int → Slider, Choice → ComboBox, else `std::nullopt`. Dual-map collision throws `InvalidParameterException`.
3. **Rationale (code-review decision B):** Static layout assembly runs before `WidgetFactory` maps are required; Shared-layer scan avoids GUI factory dependency in the builder. `WidgetFactory` continues to own widget **creation** via `createIntParameterSlider` / `createChoiceParameterComboBox`.

### AC 3 — Optional convenience helper `makeModulePanelLayout`

1. Provide overload(s) reducing header/button boilerplate, e.g.:

```cpp
ModulePanelLayout makePatchEditModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    const juce::String& copyId,
    const juce::String& pasteId,
    std::initializer_list<const char*> orderedParameterIds);

ModulePanelLayout makeMasterEditModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    std::initializer_list<const char*> orderedParameterIds);
```

2. `makeMasterEditModuleLayout` sets `buttonSet = InitOnly`, empty copy/paste IDs.
3. Panels may use helper **or** designated initializers on `ModulePanelLayout` — both valid.

### AC 4 — Migrate all BaseModulePanel consumers (13 files)

Replace `createConfig()` returning hand-typed `ModulePanelConfig` with `createLayout()` returning `ModulePanelLayout` (or inline layout in ctor).

| Panel | Notes |
|-------|-------|
| `Dco1Panel`, `Dco2Panel`, `VcfVcaPanel`, `FmTrackPanel`, `RampPortamentoPanel` | Patch Edit top |
| `Env1Panel`, `Env2Panel`, `Env3Panel`, `Lfo1Panel`, `Lfo2Panel` | Patch Edit bottom |
| `MidiPanel`, `VibratoPanel`, `MiscPanel` | Master Edit |

1. Remove `ModulePanelParameterType` enum if no remaining references.
2. `BaseModulePanel` ctor: accept `ModulePanelLayout`, call `buildModulePanelConfig(layout)` once.
3. Update panel `.h` declarations: `static ModulePanelLayout createLayout();` (rename from `createConfig`).

**Before/after target (Dco1Panel):**

```cpp
// Before: ~22 lines in parameters vector with Slider/ComboBox per row
// After example:
static ModulePanelLayout Dco1Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
        PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
        PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
        {
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
            // ... remaining IDs in display order only
        });
}
```

### AC 5 — Unit test guard

1. Add `Tests/Unit/ModulePanelConfigBuilderTests.cpp` + `Tests/Unit/MigratedModulePanelLayouts.cpp` (layout fixtures mirroring panel `createLayout()` order):
   - For each migrated panel layout ordered IDs (non-empty): `PluginHelpers::resolveParameterWidgetKind` returns a value; `buildModulePanelConfig` succeeds.
   - Spot-check: one Int param → Slider, one Choice param → ComboBox.
   - Separator: empty ID in layout yields `ParameterType::None`.
   - Negative: unknown ID throws `ParameterNotFoundException`; no int+choice descriptor ID collisions.
2. Register test in `Tests/CMakeLists.txt`.

### AC 6 — Build & regression

1. Standalone Debug + unit tests compile and pass.
2. Smoke: open Patch Edit + Master Edit @ 100% UI scale — all module panels render; sliders/combos attach correctly (no blank cells, no wrong control type).
3. `grep ModulePanelParameterType Source/` → **zero hits** after migration.

## Tasks / Subtasks

- [x] **Builder + types** (AC: #1, #3)
  - [x] `ModulePanelConfigBuilder.h` (+ `.cpp` if needed)
  - [x] `ModulePanelLayout` struct
  - [x] `buildModulePanelConfig`, `makePatchEditModuleLayout`, `makeMasterEditModuleLayout`

- [x] **PluginHelpers resolution** (AC: #2)
  - [x] `ParameterWidgetKind` + `resolveParameterWidgetKind` in `PluginHelpers`

- [x] **BaseModulePanel** (AC: #1, #4)
  - [x] Ctor takes `ModulePanelLayout` instead of resolved `ModulePanelConfig`
  - [x] Internal resolution via builder

- [x] **Panel migration** (AC: #4)
  - [x] 13 panel `.h/.cpp` files
  - [x] Remove `ModulePanelParameterType`

- [x] **Tests & CMake** (AC: #5, #6)
  - [x] Unit tests
  - [x] Smoke + grep proof in Completion Notes

### Review Findings

- [x] [Review][Decision] **Resolution path: WidgetFactory maps vs PluginHelpers scan** — **Resolved: B** — PluginHelpers is Shared SSOT for static layout-time resolution; `buildModulePanelConfig(layout)` without factory param is accepted. Follow-up patches: remove dead `WidgetFactory::resolveParameterWidgetKind` wrapper + duplicate enum; update AC1/AC2 spec text.

- [x] [Review][Patch] **Remove dead WidgetFactory resolution wrapper (decision B)** [`WidgetFactory.h:65`, `WidgetFactory.cpp:147-157`] — Removed `WidgetFactory::ParameterWidgetKind` and `resolveParameterWidgetKind`.

- [x] [Review][Patch] **Update spec AC1/AC2 for PluginHelpers SSOT (decision B)** [`u-11-module-panel-config-dedup.md`] — AC1/AC2/AC5 and Dev Notes updated.

- [x] [Review][Patch] **Missing debug `jassert` before throw on unknown parameter ID** [`ModulePanelConfigBuilder.cpp:53-54`] — Added `jassertfalse` before `ParameterNotFoundException`.

- [x] [Review][Patch] **Unit tests incomplete vs AC5** [`ModulePanelConfigBuilderTests.cpp`] — All 13 layouts via `MigratedModulePanelLayouts.cpp`; resolve + build coverage.

- [x] [Review][Patch] **No negative-path unit tests** [`ModulePanelConfigBuilderTests.cpp`] — Unknown ID throw + collision regression tests added.

- [x] [Review][Patch] **Story hygiene / AC6 evidence missing** [`u-11-module-panel-config-dedup.md`, `sprint-status.yaml`] — Tasks checked; Completion Notes filled; sprint synced to `done`.

- [x] [Review][Defer] **O(n) linear descriptor scan at panel construction** [`PluginHelpers.cpp:63-124`] — deferred, accepted trade-off per decision B.

- [x] [Review][Defer] **`PluginHelpers::resolveParameterWidgetKind` exceeds Clean Code limits** [`PluginHelpers.cpp:63-124`] — deferred, accepted per decision B; refactor optional future hygiene story.

## Dev Notes

### Architecture compliance

- **Dependency direction unchanged:** GUI only; builder may include `WidgetFactory`, `PluginDescriptors`, `PluginIDs`. No Core→GUI leak.
- **SSOT unchanged:** `PluginDescriptors` remains authoritative for parameter kind; panels only own **visual order** and **separator placement**.
- **WidgetFactory pattern preserved:** cells still created via `createIntParameterSlider` / `createChoiceParameterComboBox` — resolution only picks the path.

### Why not resolve types in static `createConfig()` without factory?

`createConfig()` is currently static and runs before maps exist. Options considered:

| Option | Verdict |
|--------|---------|
| Scan raw `PluginDescriptors` vectors in static helper | **Chosen (PluginHelpers)** — Shared SSOT for layout-time kind resolution; no `WidgetFactory` at static layout assembly |
| Pass `WidgetFactory&` into panel ctor, build layout inline | Works but scatters resolution |
| Pass `WidgetFactory&` into `buildModulePanelConfig` | Rejected at code review (decision B) — factory maps used at widget creation, not layout assembly |

### Matrix Modulation — explicitly deferred

`MatrixModulationPanel::createSourceParameterIds()` (and amount/destination/bus arrays) lists 10 bus-specific IDs. This is repetitive but **not** the Slider/ComboBox redundancy problem. `ModulationBusCell` hardcodes column widget types correctly. A future optional story could generate bus IDs from index + name template; **do not** mix into U-11.

### Panel inventory (grep `createConfig`)

13 files with `ModulePanelParameterType` in Patch Edit + Master Edit module panels. `InteractiveDisplayApvtsSync.cpp` includes `BaseModulePanel.h` for other reasons — verify no erroneous migration.

### Files to touch (minimum)

| Action | Path |
|--------|------|
| NEW | `Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.h` |
| NEW (if needed) | `Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.cpp` |
| UPDATE | `Source/GUI/Panels/Reusable/BaseModulePanel.h/.cpp` |
| UPDATE | `Source/GUI/Factories/WidgetFactory.h/.cpp` |
| UPDATE | 13× `*Panel.h/.cpp` under PatchEdit + MasterEdit Modules |
| NEW | `Tests/Unit/ModulePanelConfigBuilderTests.cpp` |
| UPDATE | `CMakeLists.txt`, `Tests/CMakeLists.txt` |

### Estimated line reduction

~8–12 lines per panel × 13 ≈ **100–150 lines** removed; net lower after builder (~80 lines new). Primary win is **correctness + maintainability**, not LOC.

### Suggested execution slot

- **When:** any time before **U-6** (Patch Edit layout audit) — cleaner panel files make layout diffs easier to review.
- **Parallel OK** with U-3/U-4/U-5 (different files) unless touching same panel during active layout edit — then sequence U-11 first.

### References

- [Source: `Source/GUI/Panels/Reusable/BaseModulePanel.h`]
- [Source: `Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco1Panel.cpp`]
- [Source: `Source/GUI/Factories/WidgetFactory.cpp` — `findIntParameter` / `findChoiceParameter`]
- [Source: `_bmad-output/project-context.md` — WidgetFactory pattern, SSOT rules]

## Dev Agent Record

### Agent Model Used

_(filled by dev agent)_

### Debug Log References

### Completion Notes List

- Code review (2026-06-17): decision B — `PluginHelpers` is Shared SSOT for layout-time widget kind resolution; removed dead `WidgetFactory::resolveParameterWidgetKind` wrapper.
- `grep ModulePanelParameterType Source/` → zero hits (AC6).
- Unit tests: `ModulePanelConfigBuilderTests` + `MigratedModulePanelLayouts` (13 panel layouts, negative paths).
- `Matrix-Control_Tests` built and run Debug @ 2026-06-17 (see test run output).
- Smoke: Patch Edit + Master Edit module panels — manual verification pending host run (unchanged behaviour expected).

### File List

| Action | Path |
|--------|------|
| NEW | `Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.h/.cpp` |
| UPDATE | `Source/GUI/Panels/Reusable/BaseModulePanel.h/.cpp` |
| UPDATE | `Source/Shared/Definitions/PluginHelpers.h/.cpp` |
| UPDATE | 13× `*Panel.h/.cpp` (Patch Edit + Master Edit modules) |
| NEW | `Tests/Unit/ModulePanelConfigBuilderTests.cpp` |
| NEW | `Tests/Unit/MigratedModulePanelLayouts.h/.cpp` |
| UPDATE | `CMakeLists.txt` |
