---
organization: Ten Square Software
project: Matrix-Control
title: Story U-IDs — Widget ID Harmonization
author: BMad Agent
status: done
baseline_commit: 51e115060f91e7267e694bc4f67362e52c897011
sources:
  - planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md
  - planning-artifacts/epics.md
  - brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md
  - implementation-artifacts/u-ids-widget-id-harmonization-inventory.md
  - Source/Shared/Definitions/PluginIDs.h
  - project-context.md
created: 2026-06-07
updated: 2026-06-09
---

# Story U-IDs: Widget ID Harmonization

Status: done

<!-- First Epic U story. Blocks U-0 (Design* reconciliation) and U-0b (Factory registry). No legacy ID aliases — plugin not public. -->

## Story

As a maintainer,
I want all widget and parameter IDs to follow a single module-scoped naming convention,
so that Shared descriptors stay pixel-free, code reads unambiguously at a glance, and the upcoming Factory dimension registry (U-0b) can resolve button widths by `widgetId` alone.

## Acceptance Criteria

### AC 1 — Naming convention documented

1. **Grammar** (also in epic § Architecture dimensions & IDs):
   - Standalone widgets: `{moduleScope}{actionVerb}{optionalTarget}` (e.g. `bankUtilityLockBank`, `internalPatchesInit`, `patchMutatorEnableDco1`).
   - APVTS parameters: `{moduleScope}{semanticName}` (e.g. `miscBankLockEnable`, `vcfVcaVca1Volume`, `patchEditPatchName`).
   - APVTS group IDs and Mode IDs (`masterMode`, `patchMode`): **unchanged**.
2. **Canonical rename tables:** `implementation-artifacts/u-ids-widget-id-harmonization-inventory.md` — **74 renames**, **159 IDs reviewed**. Story is done when every row in §2–§5 of that file is applied.

### AC 2 — `PluginIDs.h` updated

1. Apply all string value changes from inventory §2–§5.
2. Update C++ symbol names where inventory specifies (e.g. `kUnlockBank` → `kLockBank`, `kEnableDco1` for mutator toggles, Settings `kGuiScaleId` → `kGuiScale`).
3. **Product semantics:** `unlockBank` → `bankUtilityLockBank` (button locks current bank — future behaviour story; this story is ID + display name only).

### AC 3 — `PluginDisplayNames.h` aligned

1. Every renamed `widgetId` / `parameterId` has a matching display-name entry updated if the key path changed.
2. **BANK LOCK:** Bank Utility unlock button display name → **BANK LOCK** (replaces UNLOCK / unlock wording). ID is `bankUtilityLockBank`.
3. Display strings for unchanged IDs (e.g. Init/Copy/Paste labels on modules) remain semantically correct.

### AC 4 — Descriptors and layout builders

1. `PluginDescriptors.h`, `PluginDescriptors.cpp`, `PluginDescriptorsPatchEdit.cpp`, `PluginDescriptorsPatchManager.cpp`, `PluginDescriptorsMasterEdit.cpp`, `PluginDescriptorsMatrixModulation.cpp` — all `widgetId` / `parameterId` fields use new strings via `PluginIDs::*` constants (never literal old strings).
2. **`buttonWidth` + `PluginDesignDimensions` includes in Descriptors:** **out of scope** for U-IDs (removed in U-0b). Do not expand scope; only rename IDs.
3. `Core/Factories/ApvtsLayoutBuilder.cpp` — parameter/group IDs consistent with `PluginIDs.h`.

### AC 5 — GUI and Core references

1. Grep each **old** ID string from inventory §2–§5 across `Source/` and `Tests/` → **zero hits** after merge.
2. Update call sites that pass widget ID strings to `WidgetFactory`, APVTS attachments, panel factories, and handlers. Known touch areas (non-exhaustive — grep is authority):
   - `Source/GUI/Panels/.../PatchManager*` (BankUtility, InternalPatches, ComputerPatches, PatchMutator)
   - `Source/GUI/Panels/.../MasterEdit*`, `PatchEdit*`, `MatrixModulationPanel`
   - `Source/GUI/Widgets/ModulationBusCell.cpp`
   - `Source/GUI/PluginEditor.cpp`, `SettingsPanel.cpp`
   - `Source/Core/PluginProcessor.cpp`, `PatchNameSyncer.cpp`
   - `PatchEditDisplaysPanel` / `InteractiveDisplayApvtsSync` (`patchName` → `patchEditPatchName`)
3. **Member renames encouraged** where they mirror old semantics (e.g. `unlockBankButton_` → `lockBankButton_`, `initPatchButton_` stays or becomes consistent with new factory calls) — match surrounding panel style; not mandatory if only factory ID string changes.

### AC 6 — Tests and build

1. Update any unit tests referencing old parameter or widget ID strings.
2. Standalone Debug + unit test target build cleanly.
3. Smoke: app launches; Patch Edit module I/C/P buttons still construct; Patch Manager panels instantiate without APVTS layout errors.

### AC 7 — Verification checklist (Completion Notes)

1. Paste grep summary: no old IDs in `Source/` / `Tests/`.
2. Confirm **74/74** renames applied per inventory.
3. Note any intentional deviation (must be none unless owner approves in story review).

## Tasks / Subtasks

- [x] **Inventory walk-through** (AC: #1, #2)
  - [x] Apply §2 standalone buttons (32) in `PluginIDs.h`
  - [x] Apply §3 standalone non-button (3)
  - [x] Apply §4 APVTS parameters (36)
  - [x] Apply §5 Settings (3)
  - [x] Update C++ constant identifiers per inventory notes

- [x] **Display names** (AC: #3)
  - [x] Sync `PluginDisplayNames.h` (all renamed paths)
  - [x] BANK LOCK label for `bankUtilityLockBank`

- [x] **Descriptors + APVTS layout** (AC: #4)
  - [x] `PluginDescriptors*.cpp` / `.h`
  - [x] `ApvtsLayoutBuilder.cpp`

- [x] **GUI panels & widgets** (AC: #5)
  - [x] Patch Manager modules (4)
  - [x] Remaining panels/widgets from grep of old strings
  - [x] Settings / PluginEditor if Settings IDs renamed

- [x] **Core** (AC: #5)
  - [x] `PluginProcessor.cpp` (Settings + `patchEditPatchName` if referenced)
  - [x] `PatchNameSyncer.cpp`, mappers as needed

- [x] **Tests + build** (AC: #6, #7)
  - [x] Fix test ID references
  - [x] Build + smoke
  - [x] Completion Notes with grep proof

## Dev Notes

### Why this story exists

Brainstorming session 2026-06-07 established strict Factory decoupling: Shared must not carry pixel dimensions (`buttonWidth` removed in U-0b). Harmonized **`widgetId`** strings let `WidgetDimensionRegistry` map IDs → `DesignAtoms` without embedding widths in Descriptors. Module-first IDs (`internalPatchesInit` vs `initPatch`) prevent cross-panel ambiguity.

### ID convention (quick reference)

| Pattern | Example |
|---------|---------|
| Module + action | `dco1Init` (unchanged), `bankUtilitySelectBank0` |
| Module + action + target | `patchMutatorEnableDco1` |
| Module + semantic param | `miscBankLockEnable`, `fmTrackPoint1` |
| Distinct button vs param | `bankUtilityLockBank` (button) ≠ `miscBankLockEnable` (Misc toggle) |

### Implementation order (mandatory)

1. `PluginIDs.h`
2. `PluginDisplayNames.h`
3. `PluginDescriptors*` + `ApvtsLayoutBuilder.cpp`
4. GUI + Core references
5. Tests

Single mechanical commit preferred; avoid half-renamed tree (APVTS will fail at runtime).

### Out of scope (later Epic U stories)

| Story | Scope |
|-------|--------|
| **U-0** | `DesignAtoms/Recipes/Panels/Checks.h` — replace `PluginDesignDimensions.h` |
| **U-0b** | Remove `buttonWidth` from Descriptors; `WidgetDimensionRegistry`; Factory `*Dimensions` injection |
| **U-2…U-9** | Layout audit |

### Project structure notes

- IDs live in `Source/Shared/Definitions/PluginIDs.h` — still Shared (domain identity), but **no geometry**.
- Do **not** move IDs to GUI; only naming changes in this story.
- After U-0, geometry moves from `Shared/Definitions/PluginDesignDimensions.h` to `Source/GUI/Layout/Design/` (not in U-IDs).

### Risk: APVTS state on dev machines

Renaming `parameterId` strings invalidates local saved plugin state / dev presets. **Accepted** — no migration layer (owner decision). Clear local prefs if parameters appear missing after merge.

### References

- [Source: implementation-artifacts/u-ids-widget-id-harmonization-inventory.md] — exhaustive old → new table
- [Source: planning-artifacts/epic-ui-scale-audit-pixel-perfect-layout.md § Architecture dimensions & IDs]
- [Source: brainstorming/brainstorming-session-2026-06-07-ui-dimensions-strategy.md § Decision Synthesis]
- [Source: Source/Shared/Definitions/PluginIDs.h] — SSOT for all ID strings
- [Source: Source/Core/Factories/ApvtsLayoutBuilder.cpp] — APVTS tree construction

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Debug Log References

None

### Completion Notes List

- **74/74 renames applied** per inventory §2–§5 in `PluginIDs.h` (SSOT). `PluginIDs.h` diff: 74 string values removed, 74 added (baseline `51e1150`).
- **Grep proof (AC7.1)** — quoted old ID strings across `Source/` + `Tests/` (74 inventory old values):
  ```
  Old ID string literals in Source/ + Tests/: 0 hits
  PASS — zero quoted old widget/parameter ID strings
  PluginIDs.h: 74 strings removed, 74 strings added
  Inventory rename rows: 74
  ```
  Note: inventory §5 lists `hardwareLatencyMsId` as the old identifier; baseline string value was `"hardwareLatencyMs"` (C++ symbol `kHardwareLatencyMsId`). Renamed to `"settingsHardwareLatencyMs"` / `kHardwareLatencyMs`.
- **C++ symbol renames:** `kUnlockBank`→`kLockBank`, Settings `kGuiScaleId`→`kGuiScale`, `kHardwareLatencyMsId`→`kHardwareLatencyMs`, `kTestWidgetId`→`kTestWidget`, Patch Mutator `kDco1`→`kEnableDco1` (and siblings).
- **Display:** `bankUtilityLockBank` label → **BANK LOCK**; `unlockBankButton_` → `lockBankButton_`.
- **Distinctness:** `bankUtilityLockBank` (button) ≠ `miscBankLockEnable` (Misc APVTS toggle).
- **Build:** Debug `Matrix-Control_Standalone` OK; `Matrix-Control_Tests` all pass (exit 0).
- **No intentional deviations.**

### Review Findings

- [x] [Review][Patch] Completion notes claimed 75/75 — inventory §2–§5 enumerates **74** rows (§2 buttons: 32, not 33). Corrected to 74/74 in story + inventory summary.
- [x] [Review][Patch] AC7.1 grep proof absent — pasted reproducible grep summary above (2026-06-09 code review).

### File List

- Source/Shared/Definitions/PluginIDs.h
- Source/Shared/Definitions/PluginDisplayNames.h
- Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp
- Source/Core/PluginProcessor.cpp
- Source/GUI/PluginEditor.cpp
- Source/GUI/Tests/TestComponent.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp

### Change Log

- 2026-06-09: Harmonized 74 widget/parameter ID strings to module-scoped grammar; propagated symbol renames; BANK LOCK display; build + tests green.
- 2026-06-09: Code review — corrected 74/74 count + AC7.1 grep proof; story → done.
