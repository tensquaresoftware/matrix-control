---
organization: Ten Square Software
project: Matrix-Control
title: Story 5.2 — Module Copy Paste Enable and Gray Rules
author: BMad Agent
status: done
baseline_commit: 53f70a236e63c57ddc0bb18d0ca44c6e9d8cb5d1
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/sprint-change-proposal-2026-06-18.md
  - implementation-artifacts/5-1-clipboardservice-compatibility-matrix.md
  - project-context.md
created: 2026-06-18
updated: 2026-06-18
---

# Story 5.2: Module Copy Paste Enable and Gray Rules

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want Paste enabled only on compatible targets,
so that UI reflects clipboard state (FR-35, D-033).

## Acceptance Criteria

1. **Given** Story 5.1 (`Core::ClipboardService` with query API) **When** the plugin starts or clipboard content/mode changes **Then** Core publishes boolean APVTS mirror properties (`*PasteEnabled`) per paste target reflecting `ClipboardService::canPaste*` results (D-033: Core pushes enabled state; GUI listens).
2. **And** graying matrix per FR-35 / addendum § Clipboard:

   | Clipboard mode (last Copy) | PATCH EDIT module Paste (7 modules) | Internal Patches Paste | Matrix Modulation Paste |
   |---|---|---|---|
   | **Empty** (no copy yet) | all grayed | grayed (unless ROM already blocks) | grayed |
   | **Module** | enabled only on compatible module(s) per 5.1 matrix | grayed | grayed |
   | **FullPatch** | all grayed | enabled when `canPasteFullPatch()` **and** ROM/bank rules allow (FR-23) | grayed |
   | **MatrixModulation** | all grayed | grayed | enabled when `canPasteMatrixModulation()` |

3. **And** Copy buttons remain **always enabled** on PATCH EDIT modules and Internal Patches (FR-35); only Paste is gated.
4. **And** last Copy defines clipboard mode until replaced — copying a module after full-patch copy switches to Module mode and updates all `*PasteEnabled` mirrors accordingly (last Copy wins — D-033).
5. **And** minimal **Copy** property handlers exist in `PluginProcessor` so gray rules are exercisable before Story 7.2: PATCH module Copy stamps → `clipboardService_->copyModule`; Internal Patches Copy → `copyFullPatch`; Matrix Modulation Copy property handler ready for Story 5.3 IDs (or add IDs now if cleaner). **Do not** implement Paste execution, SysEx burst, or `ActionDispatcher` routing in this story (Stories 7.2 / 7.3).
6. **And** GUI wires Paste graying: `ModuleHeader` paste buttons and `InternalPatchesPanel` paste button listen to `*PasteEnabled` properties and call `setEnabled`. ROM gating on Internal Patches Paste **AND**s with clipboard property (existing `updatePasteStoreEnabled` pattern).
7. **And** `matrixModulationPasteEnabled` property is published even though the Matrix Mod Paste **button** arrives in Story 5.3 — unit tests cover matrix-mod gray rules without GUI.
8. **And** unit tests cover paste-enabled resolution for representative mode transitions (module→module compat, full-patch→module switch, matrix-mod isolation). Full `Matrix-Control_Tests` suite passes.

## Tasks / Subtasks

- [x] **Add paste-enabled property IDs** (AC: #1)
  - [x] `PluginIDs.h` — `kPasteEnabled` constants adjacent to each existing Paste widget ID:
    - PATCH EDIT: `dco1PasteEnabled` … `lfo2PasteEnabled` (7)
    - Internal Patches: `internalPatchesPasteEnabled`
    - Matrix Modulation: `matrixModulationPasteEnabled` (button in 5.3; property now)
  - [x] Initialize defaults `false` in `PluginProcessor` startup (same pattern as `deviceDetected`, port IDs)

- [x] **Core paste-enabled resolver** (AC: #1, #2, #8)
  - [x] `Source/Core/Services/ClipboardPasteEnabledResolver.{h,cpp}` — pure function(s) mapping `const ClipboardService&` → struct of bools per target (no GUI, no APVTS)
  - [x] `PatchModuleKind patchModuleKindFromWidgetId(const juce::String& copyOrPasteWidgetId)` helper (7 PATCH modules)
  - [x] Unit tests: `Tests/Unit/ClipboardPasteEnabledResolverTests.cpp`

- [x] **PluginProcessor integration** (AC: #1, #4, #5)
  - [x] Member `std::unique_ptr<Core::ClipboardService> clipboardService_`
  - [x] `refreshClipboardPasteEnabledProperties()` — reads resolver, `apvts.state.setProperty` each `*PasteEnabled`
  - [x] `handleClipboardCopyPropertyChange(const juce::String& propertyId)` — sync `apvtsPatchMapper` → `PatchModel`, dispatch copy, call refresh
  - [x] Hook from `valueTreePropertyChanged` (after existing init handlers)
  - [x] Call `refreshClipboardPasteEnabledProperties()` once at end of processor construction (initial all-false)

- [x] **GUI gray wiring** (AC: #3, #6)
  - [x] `ModuleHeader` — for `InitCopyPaste` button set: register `ValueTree::Listener` on `apvts_->state`; listen for matching `*PasteEnabled`; `pasteButton_->setEnabled(bool)`; Copy stays enabled
  - [x] Pass `pasteEnabledPropertyId` into `WithActionsSpec` (from `ModulePanelConfigBuilder` / `PluginIDs`)
  - [x] `InternalPatchesPanel` — listen for `internalPatchesPasteEnabled`; combine with ROM in `updatePasteStoreEnabled`

- [x] **Self-review** (AC: #5) — no Paste execution; no SysEx; no `ActionDispatcher`; Copy handlers ≤ 15 lines each; GUI does not call `ClipboardService` directly (Core ↛ GUI inverted: GUI reads properties only)

### Review Findings

- [x] [Review][Patch] Restaurer `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InternalPatchesPanel)` — supprimé par erreur lors de l'ajout de `clipboardPasteEnabled_` [`InternalPatchesPanel.h:80`]

- [x] [Review][Patch] Recalculer les propriétés `*PasteEnabled` après restauration d'état DAW — `getStateInformation` sérialise ces booléens ; au reload le presse-papiers est vide mais les valeurs sauvegardées peuvent rester `true` [`PluginProcessor.cpp:467-475`, `valueTreeRedirected:1540`]

- [x] [Review][Patch] `PasteEnabledPropertyListener` doit resynchroniser sur `valueTreeRedirected` — callback vide aujourd'hui ; le bouton Paste peut rester dans un état obsolète après `replaceState` [`ModuleHeader.cpp:44`]

- [x] [Review][Patch] `InternalPatchesPanel::valueTreeRedirected` doit relire `internalPatchesPasteEnabled` — même risque d'état obsolète pour le bouton Paste Internal [`InternalPatchesPanel.h:40`]

- [x] [Review][Defer] `patchModuleKindFromWidgetId` mappe aussi les IDs Paste — le guard `endsWith("Copy")` protège aujourd'hui ; piège pour Story 7.2 [`ClipboardPasteEnabledResolver.cpp:53-58`, `PluginProcessor.cpp:1449`] — deferred, Story 7.2 handoff

- [x] [Review][Defer] `refreshDeviceLimits()` appelé sur changement clipboard — effet correct mais recalcul ROM inutile [`InternalPatchesPanel.cpp:324`] — deferred, hygiène perf

- [x] [Review][Defer] Liste des 9 IDs `*PasteEnabled` dupliquée dans `initialize*` et `refresh*` — risque d'oubli à l'ajout d'une cible [`PluginProcessor.cpp:1380-1418`] — deferred, maintenabilité

- [x] [Review][Defer] Pas de propriétés `*PasteEnabled` MASTER — hors scope 5.2 (pas de boutons C/P MASTER) ; FR-35 MASTER grisé quand boutons existent (7.x) — deferred, scope story

## Dev Notes

### What Story 5.2 IS — and what it is NOT

Story 5.2 connects **Story 5.1 query API** to **visible Paste button state** via APVTS mirror properties (D-033). It adds **Copy** handlers in the processor so users can exercise graying in the running app before Epic 7.

It must **NOT** in this story:
- Execute Paste (model mutation + APVTS push + SysEx) — Story **7.2** / **7.3**
- Add Matrix Modulation section C/P **buttons** or descriptor entries — Story **5.3**
- Add `ActionDispatcher` / `ModuleActionHandler` — Stories **7.1** / **7.2**
- Modify `ClipboardService` compatibility logic — Story **5.1** (done)
- Add MASTER module Copy/Paste buttons — MASTER panels use `InitOnly` today (`makeMasterEditModuleLayout`); FR-35 matrix-mod gray rule for MASTER Paste applies when MASTER C/P exist (future 7.x). **No MASTER paste buttons to wire in 5.2.**

[Source: D-033; FR-35; `5-1-clipboardservice-compatibility-matrix.md` § cross-story map]

### Epic 5 cross-story map

| Story | Delivers | Relation to 5.2 |
|---|---|---|
| **5.1 (done)** | `ClipboardService` + `canPaste*` API | Resolver consumes query methods |
| **5.2 (this)** | `*PasteEnabled` properties + Copy handlers + GUI gray | Paste mirrors + minimal Copy wiring |
| **5.3** | Matrix Mod I/C/P GUI + Copy/Paste descriptors | Consumes `matrixModulationPasteEnabled`; adds Copy button |
| **7.2** | Paste execution + SysEx | Calls `clipboardService_->paste*`; must call `refreshClipboardPasteEnabledProperties()` if clipboard could change |
| **7.3** | Internal Patches Paste execution + ROM | Uses `canPasteFullPatch()` before paste |

### Authoritative graying rules (FR-35 + addendum + D-095)

```
Module Copy      → Paste on compatible PATCH modules only
Full-patch Copy  → all PATCH module Paste grayed; Internal Patches Paste active (subject to ROM FR-23)
Matrix-mod Copy  → all PATCH module Paste grayed; Internal Patches Paste grayed; Matrix Mod Paste only
Last Copy wins   → mode + all mirrors recomputed
```

No footer message on incompatible Paste (implicit UX — user manual EN/FR).

### APVTS property contract (D-033)

| Property | Type | Writer | Reader |
|---|---|---|---|
| `dco1PasteEnabled` … `lfo2PasteEnabled` | `bool` | `PluginProcessor::refreshClipboardPasteEnabledProperties` | `ModuleHeader` |
| `internalPatchesPasteEnabled` | `bool` | same | `InternalPatchesPanel` (AND ROM) |
| `matrixModulationPasteEnabled` | `bool` | same | `MatrixModulationPanel` (Story 5.3) |

Copy trigger properties (existing, timestamp pattern) remain unchanged: `dco1Copy`, `internalPatchesCopy`, etc.

**Do not** persist clipboard snapshots in plugin state — clipboard is session RAM only (same as 5.1 in-memory service).

### Recommended resolver shape

```cpp
// Source/Core/Services/ClipboardPasteEnabledResolver.h
namespace Core
{
    struct ClipboardPasteEnabledState
    {
        bool dco1 = false;
        bool dco2 = false;
        bool env1 = false;
        bool env2 = false;
        bool env3 = false;
        bool lfo1 = false;
        bool lfo2 = false;
        bool internalPatches = false;
        bool matrixModulation = false;
    };

    ClipboardPasteEnabledState resolvePasteEnabled(const ClipboardService& clipboard);
    std::optional<PatchModuleKind> patchModuleKindFromWidgetId(const juce::String& widgetId);
}
```

Keep resolver **pure** — testable without processor/APVTS.

### PluginProcessor copy handler pattern

Follow `handleMatrixModInitPropertyChange` / `handleMasterModuleInitPropertyChange` (Story 3.3 / 3.4):

1. Detect Copy property ID in `valueTreePropertyChanged` → `handleClipboardCopyPropertyChange`
2. `apvtsPatchMapper_->apvtsToBuffer()` before reading `PatchModel`
3. Dispatch:
   - PATCH `*Copy` → `clipboardService_->copyModule(kind, *patchModel_)`
   - `internalPatchesCopy` → `clipboardService_->copyFullPatch(*patchModel_)`
   - `matrixModulationCopy` → `clipboardService_->copyMatrixModulation(*patchModel_)` (property ID from 5.3 or add in 5.2)
4. `refreshClipboardPasteEnabledProperties()`

**No paste handlers** in 5.2.

### GUI wiring — ModuleHeader (UPDATE file)

Current state (`ModuleHeader.cpp`):
- `createCopyPasteButtons` stamps timestamp on `copyWidgetId` / `pasteWidgetId`
- Paste button always enabled today
- `apvts_` pointer already stored for Init confirmation

Changes:
- Add `pasteEnabledPropertyId_` to `WithActionsSpec`
- Implement `ValueTree::Listener` (or small nested helper) when `buttonSet_ == InitCopyPaste`
- On `pasteEnabledPropertyId_` change → `pasteButton_->setEnabled(static_cast<bool>(newValue))`
- Initial sync in constructor after buttons created
- Remove listener in destructor

Copy button: **never** disabled by clipboard state (FR-35).

### GUI wiring — InternalPatchesPanel (UPDATE file)

Current state (`InternalPatchesPanel.cpp`):
- `updatePasteStoreEnabled(limits, currentBank)` sets Paste/Store from ROM (`limits.isPasteStoreAllowed`)
- Does not know about clipboard

Changes:
- Track `clipboardPasteEnabled_` from `internalPatchesPasteEnabled` property
- `updatePasteStoreEnabled` → `pastePatchButton_->setEnabled(allowPasteStore && clipboardPasteEnabled_)`
- Listen in `valueTreePropertyChanged` for `internalPatchesPasteEnabled`
- Initial read in constructor after `apvts_.state.addListener`

### MASTER module Paste (out of scope for GUI)

`ModulePanelConfigBuilder::makeMasterEditModuleLayout` uses `InitOnly` — no Copy/Paste widgets on MIDI/Vibrato/Misc headers today. Matrix-mod mode graying for MASTER Paste is specified in FR-35 for when those buttons exist; **5.2 does not add MASTER `*PasteEnabled` properties** unless MASTER C/P buttons are added in the same story (they are not). Document in 7.2 handoff.

### Files to touch (expected)

| File | Action |
|---|---|
| `Source/Core/Services/ClipboardPasteEnabledResolver.h` | **NEW** |
| `Source/Core/Services/ClipboardPasteEnabledResolver.cpp` | **NEW** |
| `Source/Core/PluginProcessor.h` | **UPDATE** — clipboard service, handlers |
| `Source/Core/PluginProcessor.cpp` | **UPDATE** — copy + refresh |
| `Source/Shared/Definitions/PluginIDs.h` | **UPDATE** — `kPasteEnabled` IDs |
| `Source/GUI/Widgets/ModuleHeader.h` | **UPDATE** — listener + spec field |
| `Source/GUI/Widgets/ModuleHeader.cpp` | **UPDATE** — paste gray |
| `Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.cpp` | **UPDATE** — pass pasteEnabled ID |
| `Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.h` | **UPDATE** |
| `Source/GUI/Panels/MainComponent/.../InternalPatchesPanel.cpp` | **UPDATE** — AND ROM + clipboard |
| `Source/GUI/Panels/MainComponent/.../InternalPatchesPanel.h` | **UPDATE** if needed |
| `Tests/Unit/ClipboardPasteEnabledResolverTests.cpp` | **NEW** |
| `CMakeLists.txt` | **UPDATE** — register new `.cpp` in plugin + tests |

**Do not modify** `ClipboardService.{h,cpp}` except if a trivial `PatchModuleKind` lookup helper fits better there than resolver (prefer resolver to keep 5.1 frozen).

### Testing requirements

Per NFR-1, D-033, architecture AD-8:

- **AAA** structure; no hardware
- `ClipboardPasteEnabledResolverTests` (suggested cases):
  1. `emptyClipboard_allPasteDisabled`
  2. `moduleCopy_dco1_enablesDco2Only`
  3. `moduleCopy_env1_enablesEnv2Env3`
  4. `fullPatchCopy_disablesAllModulePaste_enablesInternal`
  5. `matrixModCopy_disablesModuleAndInternal_enablesMatrixMod`
  6. `modeSwitch_fullPatchThenDco1Copy_updatesMirrors` (mirrors 5.1 `modeReplacement` test at resolver layer)
- Optional processor integration test deferred — resolver unit tests satisfy AC #8
- Manual smoke: run plugin, Copy DCO1 → only DCO2 Paste enabled; Copy Internal patch → only Internal Paste enabled (on RAM bank)

Register: `static ClipboardPasteEnabledResolverTests clipboardPasteEnabledResolverTests;`

### Architecture compliance

| Invariant | Application |
|---|---|
| `Core ↛ GUI` | GUI reads APVTS bool properties only; never `#include ClipboardService.h` in GUI |
| D-033 | Core writes `pasteEnabled`; GUI grays |
| Descriptor SSOT | Widget/copy IDs from `PluginIDs`; module kind mapping centralized in resolver |
| No SysEx in story | Copy reads model; no outbound MIDI |
| Brownfield | Extend `ModuleHeader` listener pattern; do not refactor all panels to `BaseModulePanel` |

### Previous story intelligence (5.1)

| Learning | Application |
|---|---|
| `ClipboardService` query API complete | Resolver calls `canPasteModule`, `canPasteFullPatch`, `canPasteMatrixModulation`, `hasContent` |
| `getSourceModuleKind()` returns `nullopt` when mode ≠ Module | Resolver must not assume module source in FullPatch/MatrixMod modes |
| Processor wiring deferred in 5.1 | **5.2 owns** processor `ClipboardService` lifetime + copy property handlers |
| 10 unit tests green in `ClipboardServiceTests.cpp` | Do not duplicate matrix tests — test resolver mapping only |
| Review deferrals: message-thread, no partial-paste UX | Paste enabled mirrors match `canPaste*` exactly; no extra UX |

### Git intelligence

- `53f70a2` — **Add ClipboardService for Story 5.1** — service + tests landed; processor not yet wired
- `60ec966` / Epic 3 — init handlers in processor (`handleMatrixModInitPropertyChange`, `handleMasterModuleInitPropertyChange`) are the copy-handler template
- `dcf721d` — `DeviceMemoryLimits` + Internal Patches ROM gating already in GUI — AND with clipboard property

### Latest tech information

- **JUCE 8.0.12** — `ValueTree::Listener` on `apvts.state` for non-parameter properties; `setProperty` with `nullptr` undo manager (existing pattern)
- **C++17** — `std::optional` for widget→module mapping
- No new dependencies

### Project context reference

- `_bmad-output/project-context.md` — Core/GUI boundary, test pyramid, English source
- `CONVENTIONS.md` §8.5 — AAA tests
- `Documentation/Development/software-development-quality-principles.md`

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` § Story 5.2]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md` FR-35, FR-45]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md` § Clipboard compatibility matrix]
- [Source: `_bmad-output/planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md` D-033, D-060, D-095]
- [Source: `_bmad-output/planning-artifacts/sprint-change-proposal-2026-06-18.md` § Implementation sequence]
- [Source: `implementation-artifacts/5-1-clipboardservice-compatibility-matrix.md`]
- [Source: `Source/Core/Services/ClipboardService.h` — query API]
- [Source: `Source/GUI/Widgets/ModuleHeader.cpp` — current C/P stamp pattern]
- [Source: `Source/GUI/Panels/.../InternalPatchesPanel.cpp` — ROM paste gating]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Resolver unit tests initially assumed source-module Paste disabled; aligned with `ClipboardService::canPaste*` (same-family targets include source module).

### Completion Notes List

- Added `ClipboardPasteEnabledResolver` (pure Core) mapping `ClipboardService::canPaste*` to per-target bools; 7 resolver unit tests pass.
- `PluginProcessor` owns `ClipboardService`, publishes 9 `*PasteEnabled` APVTS properties (D-033), handles Copy property stamps (module / full patch / matrix-mod IDs).
- `ModuleHeader` listens to `pasteEnabledPropertyId` via nested `ValueTree::Listener`; Copy buttons never grayed.
- `InternalPatchesPanel` ANDs `internalPatchesPasteEnabled` with existing ROM gating.
- Added `matrixModulationCopy` / `matrixModulationPaste` IDs for Story 5.3; `matrixModulationPasteEnabled` published now.
- `Matrix-Control_Tests` and `Matrix-Control_Standalone` build green (macOS ARM Debug).

### File List

- Source/Core/Services/ClipboardPasteEnabledResolver.h (NEW)
- Source/Core/Services/ClipboardPasteEnabledResolver.cpp (NEW)
- Source/Core/PluginProcessor.h (UPDATE)
- Source/Core/PluginProcessor.cpp (UPDATE)
- Source/Shared/Definitions/PluginIDs.h (UPDATE)
- Source/GUI/Widgets/ModuleHeader.h (UPDATE)
- Source/GUI/Widgets/ModuleHeader.cpp (UPDATE)
- Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.h (UPDATE)
- Source/GUI/Panels/Reusable/ModulePanelConfigBuilder.cpp (UPDATE)
- Source/GUI/Panels/Reusable/BaseModulePanel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.h (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco1Panel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco2Panel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env1Panel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env2Panel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env3Panel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Lfo1Panel.cpp (UPDATE)
- Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Lfo2Panel.cpp (UPDATE)
- Tests/Unit/ClipboardPasteEnabledResolverTests.cpp (NEW)
- Tests/Unit/MigratedModulePanelLayouts.cpp (UPDATE)
- CMakeLists.txt (UPDATE)

### Change Log

- 2026-06-18: Story 5.2 — paste-enabled APVTS mirrors, Copy handlers, GUI gray wiring (D-033 / FR-35).
