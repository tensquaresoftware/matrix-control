---
organization: Ten Square Software
project: Matrix-Control
title: Story U-0b — Factory Dimension Registry & Strict Decoupling
author: BMad Agent
status: done
baseline_commit: 49c9130cd994d1500053628e620606b7c0e5fed6
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md
  - implementation-artifacts/u-0-figma-intake-and-design-reconciliation.md
  - implementation-artifacts/u-ids-widget-id-harmonization.md
  - Source/GUI/Factories/WidgetFactory.h
  - Source/GUI/Looks/WidgetLooks.h
  - Source/Shared/Definitions/PluginDescriptors.h
created: 2026-06-07
updated: 2026-06-07
---

# Story U-0b: Factory Dimension Registry & Strict Decoupling

Status: done

<!-- Requires U-0 Design* headers + U-IDs harmonized widget IDs. Enables U-2…U-9 layout audits. Strict Factory model P18/P19 from brainstorming. -->

## Story

As a developer,
I want the Factory to be the sole consumer of the design plan and inject dimensions at construction,
so that widgets and panels stay generic, Shared descriptors contain no pixel fields, and `resized()` never re-reads the SSOT.

## Acceptance Criteria

### AC 1 — Runtime dimension structs (mirror Looks)

1. Create `Source/GUI/Layout/WidgetDimensions.h` and `PanelDimensions.h`:
   - Plain structs holding **int design px @ 100 %** (or nested sub-structs per widget/panel type).
   - Mirror pattern of `Source/GUI/Looks/WidgetLooks.h` — data only, no JUCE dependency required beyond optional forward decl if needed.
2. Minimum struct coverage for current brownfield consumers:
   - **Widgets:** `ButtonDimensions`, `SliderDimensions`, `ComboBoxDimensions`, `ParameterCellDimensions`, `ModuleHeaderDimensions`, `ModulationBusCellDimensions`, section/module header widths, separator dimensions, display bands (envelope/track/patch name).
   - **Panels:** `BodyPanelDimensions`, `PatchEditPanelDimensions`, `SharedPanelDimensions`, `PatchManagerPanelDimensions`, per-module Patch Manager dims, `MasterEditPanelDimensions`, `HeaderPanelDimensions`, `FooterPanelDimensions`, **`PluginEditorDimensions`** (root GUI: width, height, header/body/footer breakdown, padding).
3. Factory method(s) populate these from `DesignAtoms` / `DesignRecipes` / `DesignPanels` constexpr — single assembly point.

### AC 2 — WidgetFactory as sole Design* consumer (widgets)

1. Extend `WidgetFactory` (or dedicated `DimensionFactory` used only by `WidgetFactory` if file size demands — prefer single Factory entry point):
   - `createStandaloneButton(widgetId, skin, height)` → resolve **width** from registry (see AC 3), not `desc->buttonWidth`.
   - Existing `createIntParameterSlider` / `createChoiceParameterComboBox` overloads unchanged in signature; internal paths may use registry for default widths where applicable.
2. **`grep '#include.*Design'`** in `Source/Shared/` → **zero hits**.
3. **`grep PluginDesignDimensions`** in `Source/GUI/Widgets/` → **zero hits** after migration.

### AC 3 — WidgetDimensionRegistry (Path A)

1. New GUI-only module e.g. `Source/GUI/Layout/WidgetDimensionRegistry.h` (+ `.cpp` if needed):
   - **Hybrid resolution:** rule-based defaults + explicit overrides map keyed by harmonized **`widgetId`** string (`PluginIDs::*`).
   - Default rules examples:
     - `*Init`, `*Copy`, `*Paste` suffix → `DesignAtoms` I/C/P button widths (20)
     - `bankUtilitySelectBank*` → bank select width (36)
     - `bankUtilityLockBank` → lock bank width (76)
     - Patch Mutator action buttons → per DesignAtoms mutator widths
   - Explicit map entries for exceptions not covered by rules.
2. **Guard:** unit test or compile-time test (preferred) asserting every `StandaloneWidgetDescriptor` in `PluginDescriptors*` has a resolvable width via registry (iterate IDs from descriptors or maintain static manifest cross-checked in test).
3. Remove `std::optional<int> buttonWidth` from `StandaloneWidgetDescriptor` in `PluginDescriptors.h`.
4. Remove all `.buttonWidth = …` and `#include PluginDesignDimensions.h` from:
   - `PluginDescriptors.cpp`
   - `PluginDescriptorsPatchEdit.cpp`
   - `PluginDescriptorsPatchManager.cpp`
   - `PluginDescriptorsMasterEdit.cpp`
   - `PluginDescriptorsMatrixModulation.cpp`
5. Update `WidgetFactory.cpp` — delete `desc->buttonWidth.value_or(…)` fallback; use registry.
6. Update `ApvtsValidator` if it validates `buttonWidth` presence — remove pixel validation from Shared layer.

### AC 4 — Panel Factory injection

1. Introduce panel construction path where Factory (or panel factory helper colocated with `WidgetFactory`) builds `*PanelDimensions` and passes to panel ctors.
2. Migrate **all** panel/widget files currently `#include PluginDesignDimensions.h` (25 GUI files per brownfield grep):

| Area | Files (minimum) |
|------|-----------------|
| Shell | `PluginEditor.h/.cpp`, `MainComponent.cpp`, `BodyPanel.cpp`, `SharedPanel.cpp` |
| Patch Edit | `PatchEditPanel`, Top/Bottom/Displays sub-panels |
| Shared | `MatrixModulationPanel`, `PatchManagerPanel`, 4× module panels |
| Master Edit | `MasterEditPanel` |
| Reusable | `BaseModulePanel.cpp` |
| Widgets | `ModuleHeader.cpp`, `ParameterCell.cpp` |

3. Panel/widget ctors accept const ref or struct of injected dimensions; store as members; **`resized()` uses members + `ScaledLayout`** — no Design* / PluginDesignDimensions include.
4. **`grep PluginDesignDimensions`** in `Source/` → **zero hits** (except optional deleted shim file removed entirely).
5. **`grep DesignAtoms|DesignRecipes|DesignPanels`** outside `Source/GUI/Factories/`, `Source/GUI/Layout/Design/`, and registry → **zero hits**.

### AC 5 — PluginEditor root dimensions

1. Add `WidgetFactory::getRootGuiDimensions()` (or static helper on Factory instance) returning `PluginEditorDimensions`.
2. `PluginEditor` ctor receives dimensions from Factory — **remove** `#include PluginDesignDimensions.h` from `PluginEditor.h`.
3. `setSize` / layout in `PluginEditor` uses injected root width/height (scaled via existing `ScaledLayout` + `uiScale_` pattern).

### AC 6 — Remove legacy SSOT path

1. Delete `Source/Shared/Definitions/PluginDesignDimensions.h` and any U-0 compatibility shim.
2. Update docs references in `_bmad-output/project-context.md` if SSOT path mentioned (one-line pointer to `GUI/Layout/Design/`).

### AC 7 — Build, tests, smoke

1. Standalone Debug + unit tests compile and pass.
2. Registry guard test passes (all standalone widget IDs resolvable).
3. Smoke: launch app @ 100 % UI scale — layout visually unchanged vs pre-U-0b (regression intent; pixel-perfect sign-off remains U-2…U-9).
4. Completion Notes: paste grep proof for AC 2, 4, 5.

## Tasks / Subtasks

- [x] **Runtime structs** (AC: #1)
  - [x] `WidgetDimensions.h`, `PanelDimensions.h`
  - [x] Factory builder fn(s) from Design*

- [x] **WidgetDimensionRegistry** (AC: #3)
  - [x] Rules + explicit map keyed on post-U-IDs `widgetId`s
  - [x] Guard test covering all standalone descriptors
  - [x] Wire `WidgetFactory::createStandaloneButton`

- [x] **Descriptor decoupling** (AC: #3)
  - [x] Remove `buttonWidth` field + all assignments
  - [x] Remove Design includes from Shared descriptor TUs

- [x] **Panel/widget migration** (AC: #4)
  - [x] Shell: PluginEditor, MainComponent, BodyPanel, SharedPanel
  - [x] Patch Edit / Matrix Mod / Patch Manager / Master Edit panels
  - [x] ModuleHeader, ParameterCell, BaseModulePanel

- [x] **Root GUI API** (AC: #5)
  - [x] `getRootGuiDimensions()`
  - [x] PluginEditor injection

- [x] **Legacy removal** (AC: #6)
  - [x] Delete PluginDesignDimensions.h + shim (already absent post-U-0; removed ModulationBusCellDimensions.h duplicate)
  - [x] project-context.md pointer

- [x] **Verify** (AC: #7)

### Review Findings

- [x] [Review][Defer] `patchMutatorEnable*` descriptor type vs registry width — Descriptors declare enable widgets as `kButton`; UI builds `TSS::Toggle`; registry returns `BW::kInit` for guard test only. Dead path (never `createStandaloneButton`). Proper fix = add `kToggle` descriptor type in a future taxonomy story (option A).

- [x] [Review][Patch] Registry maps Compare to Mutate constant — fixed: uses `BW::kPatchMutatorCompare`.

- [x] [Review][Patch] Registry maps Export to Clear constant — fixed: uses `BW::kPatchMutatorExport`.

- [x] [Review][Patch] Guard test checks resolvability only, not width correctness — fixed: explicit width assertions against Design atoms.

- [x] [Review][Patch] `PluginEditor::getDesignWidth/Height` bypass injected dimensions — fixed: removed dead static accessors.

- [x] [Review][Patch] `SectionHeaderDimensions` declared but unused — fixed: removed dead struct.

- [x] [Review][Defer] `WidgetFactory` default slider overload still reads `Design*` — `WidgetFactory.cpp:63-64`; AC 2.1 explicitly permits unchanged overload signature; full slider registry migration deferred.

- [x] [Review][Defer] Dual Design* read paths (DimensionFactory + Registry) — Path A architecture from brainstorming; registry intentionally reads `DesignAtoms` for button width resolution; consolidating into runtime structs is a future hardening pass.

- [x] [Review][Defer] Suffix/prefix registry rules fragile — `endsWith("Init")`, `startsWith("bankUtilitySelectBank")`; accepted trade-off per AC 3 Path A; explicit map expansion deferred until ID taxonomy changes.

- [x] [Review][Defer] No `DimensionFactory` unit tests — ~360-line assembly monolith untested; AC 7 requires registry guard only; dimension struct coherence tests deferred.

## Dev Notes

### Depends on

| Story | Must be **done** before U-0b |
|-------|------------------------------|
| **U-0** | Design* headers + DesignChecks passing |
| **U-IDs** | Harmonized `widgetId` strings for registry keys |

### Current brownfield (read before coding)

**WidgetFactory today** (`WidgetFactory.cpp` ~L98–104):

```cpp
validator.getStandaloneWidgetDescriptorOrThrow(desc, widgetId);
const auto buttonWidth = desc->buttonWidth.value_or(PluginDesignDimensions::Widgets::Widths::Button::kInit);
return createButtonFromDescriptor(..., buttonWidth, height);
```

**StandaloneWidgetDescriptor** (`PluginDescriptors.h` ~L90–96): `widgetId`, `displayNameKey`, `buttonWidth` optional.

**~80+ `.buttonWidth =`** assignments across 5 Descriptor `.cpp` files — all removed; widths move to registry.

### Registry design (brainstorming Path A)

- **Shared stays domain-only:** IDs, display names, APVTS metadata — zero pixels.
- **Registry lives GUI-side** — only Factory + registry include Design*.
- Prefer **rule-first** (Init/Copy/Paste, selectBank pattern) to limit 33+ explicit entries; explicit map for mutator/compare/export/clear variants.

### Panel injection strategy

Recommended order (minimize broken intermediate builds):

1. Define `PanelDimensions` structs + Factory fill from `DesignPanels.h`
2. Shell top-down: `PluginEditor` → `MainComponent` → `BodyPanel` → column panels
3. Leaf widgets last: `ModuleHeader`, `ParameterCell`
4. Each migrated file: remove SSOT include in same commit hunk as ctor change

**Do not** change layout algorithms beyond replacing SSOT reads with member fields (no U-5 originX fix here).

### ApvtsValidator / WidgetFactoryValidator

- Grep `buttonWidth` in `Source/Core/Factories/` — update validators to stop expecting pixel fields on descriptors.

### Out of scope

| Item | Story |
|------|-------|
| Figma value reconciliation | U-0 |
| ID renames | U-IDs |
| Pixel-perfect UAT per zone | U-2…U-9 |
| `BodyPanel` originX removal | U-5 |
| TestComponent | U-1 |

### References

- [Source: Source/GUI/Factories/WidgetFactory.h]
- [Source: Source/GUI/Looks/WidgetLooks.h] — struct mirror pattern
- [Source: Source/Shared/Definitions/PluginDescriptors.h]
- [Source: brainstorming session § 2. Descriptors — Path A]
- [Source: implementation-artifacts/u-0-figma-intake-and-design-reconciliation.md]

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Debug Log References

- Fixed sed damage in PluginDescriptorsMatrixModulation.cpp (bus standalone closing braces)
- Registry rule added for `patchMutatorEnable*` → kInit width

### Completion Notes List

- `DimensionFactory` is the single assembly point from Design* → `GuiLayoutDimensions`
- `WidgetDimensionRegistry` resolves all standalone button widths (rules + ID-specific overrides)
- Removed `buttonWidth` from `StandaloneWidgetDescriptor`; zero Design includes in Shared/
- All panel/widget `resized()` paths use injected member dimensions
- Build: Standalone Debug OK; all unit tests pass including `WidgetDimensionRegistryTests`
- Grep proof (AC 2/4/5): Shared Design includes = 0; PluginDesignDimensions = 0; Design* outside Factory/Design/Registry = 0; panel Design.h = 0

### File List

- CMakeLists.txt
- Tests/Unit/WidgetDimensionRegistryTests.cpp
- Source/GUI/Factories/DimensionFactory.h
- Source/GUI/Factories/DimensionFactory.cpp
- Source/GUI/Factories/WidgetFactory.h
- Source/GUI/Factories/WidgetFactory.cpp
- Source/GUI/Factories/WidgetFactoryValidator.h
- Source/GUI/Factories/WidgetFactoryValidator.cpp
- Source/GUI/Layout/WidgetDimensions.h
- Source/GUI/Layout/PanelDimensions.h
- Source/GUI/Layout/WidgetDimensionRegistry.h
- Source/GUI/Layout/WidgetDimensionRegistry.cpp
- Source/GUI/PluginEditor.h
- Source/GUI/PluginEditor.cpp
- Source/GUI/MainComponent.h
- Source/GUI/MainComponent.cpp
- Source/GUI/Panels/** (BodyPanel tree, BaseModulePanel, all module panels)
- Source/GUI/Widgets/ModuleHeader.h/.cpp
- Source/GUI/Widgets/ParameterCell.h/.cpp
- Source/GUI/Widgets/ModulationBusCell.h
- Source/GUI/Widgets/ModulationBusCellDimensions.h (deleted)
- Source/Shared/Definitions/PluginDescriptors.h
- Source/Shared/Definitions/PluginDescriptors*.cpp (5 files)
- _bmad-output/project-context.md

## Change Log

- 2026-06-09: Code review — 5 patches applied, 1 decision deferred (patchMutatorEnable taxonomy), story → done
