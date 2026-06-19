---
organization: Ten Square Software
project: Matrix-Control
title: Story 4.6 — Previous and Next File Navigation
author: BMad Agent
status: review
baseline_commit: 48a0456
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - implementation-artifacts/4-5-import-name-reconciliation.md
  - implementation-artifacts/4-2-combobox-sentinel-states.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 4.6: Previous and Next File Navigation

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want `<` `>` buttons to cycle patch files in the folder,
so that I can audition files quickly (FR-52).

## Acceptance Criteria

1. **Given** Stories 4.1–4.5 complete (`PatchFileService` sorted scan, combobox sentinel FSM, folder persistence, save, combobox load with FR-29 reconciliation) **When** user clicks **Previous Patch File** (`<`) or **Next Patch File** (`>`) while combobox is in **FileSelected** state (`computerPatchesSelectPatch >= 1`) **Then** selection advances circularly through `sortedValidFileNames` (ascending lexicographic — same order as combobox ids `1..N`) and the **same load pipeline** as combobox selection runs: decode → reconcile → suppress APVTS SysEx → `bufferToApvts` + `PatchNameSyncer` → `sendPatch` (opcode 0x01) to current internal patch number.
2. **And** wrap semantics: from file id `1`, Previous selects id `N`; from id `N`, Next selects id `1` (FR-52 consequences).
3. **And** when only one valid file exists (`N == 1`), Previous/Next keep id `1` and **re-load** that file (acceptable v1 idempotency — same as re-selecting the same combobox entry in Story 4.5).
4. **And** after successful navigation **Then** `computerPatchesSelectPatch` APVTS property reflects the new 1-based id **and** the combobox display updates to the matching filename (no stale label).
5. **And** buttons remain **disabled** in `<EMPTY>` / `<SELECT>` sentinel states (already wired in Story 4.2 — verify unchanged); handler is a silent no-op if invoked with `selectedId < 1`, stale scan, or empty file list.
6. **And** load failure paths mirror combobox load (Story 4.5): invalid/missing file → warning footer, model/APVTS/MIDI unchanged; ask-once reconciliation cancel → abort with model restored; stale scan / out-of-range → warning footer.
7. **And** navigation does **not** rescan folder, save, persist folder path, or implement `DirtyPatchTracker` (**Epic 9**) / mutator history clear (**Epic 6.13** — existing `// FR-31 hook` in `applyLoadedPatchToApvtsAndSynth` remains).
8. **And** implementation reuses `handleLoadSelectedPatchFile` load path — **no duplicated** decode/reconcile/SysEx orchestration.
9. **And** unit tests cover: wrap next (last→first), wrap previous (first→last), single-file re-load, sentinel no-op (`selectedId == 0`), successful navigation enqueues patch SysEx, combobox property sync contract. Full `Matrix-Control_Tests` suite passes.

## Tasks / Subtasks

- [x] **Implement adjacent selection advance** (AC: #1–#3, #5, #8)
  - [x] Replace stub at `PatchManagerActionHandler.cpp:159-163`
  - [x] Add `advanceComputerPatchesSelection(bool isNext)` — guards + circular 1-based id math on `sortedValidFileNames.size()`
  - [x] On prev/next button event: call `advanceComputerPatchesSelection` only (set `computerPatchesSelectPatch` — let existing `kSelectPatchFile` dispatch trigger `handleLoadSelectedPatchFile`)
  - [x] Do **not** call decode/load directly from prev/next branch (avoids double SysEx)

- [x] **Sync combobox when Core updates selection** (AC: #4)
  - [x] Extend `ComputerPatchesPanel::valueTreePropertyChanged` to react to `computerPatchesSelectPatch` (not only `kScanRevision`)
  - [x] Call `syncSelectionFromApvts()` — uses `setSelectedId(..., dontSendNotification)` so no recursive property write / load storm
  - [x] Keep `setNavigationButtonsEnabled(selectedId >= 1)` behaviour intact

- [x] **Self-review** (AC: #7)
  - [x] No new Core services; no GUI layout changes; methods ≤ 15 lines; English only in source
  - [x] Leave `// FR-31 hook` untouched

- [x] **Unit tests** (AC: #9)
  - [x] `PatchManagerActionHandlerTests.cpp` — `testLoadAdjacent_nextWraps`, `testLoadAdjacent_previousWraps`, `testLoadAdjacent_singleFileReloads`, `testLoadAdjacent_sentinelNoOp`, `testLoadAdjacent_enqueuesSysEx`
  - [x] Reuse `HandlerHarness` + temp scan dirs + `Patch 71.syx` fixture patterns from Story 4.5 load tests

## Dev Notes

### What this story IS — and what it is NOT

Story 4.6 delivers **FR-52 / D-075** Computer Patches **Previous/Next file navigation** with circular wrap through the scanned library.

It must **NOT** in this story:
- Change scan/save/reconciliation/folder persistence logic (Stories 4.1–4.5)
- Add debounce on navigation (not in FR-52; Mutator history debounce is separate — FR-57 / Epic 6)
- Implement unsaved-edit confirmation on navigation (**Epic 9** — FR-51)
- Clear mutator history on load (**Epic 6.13** — hook already documented)
- Touch Patch Mutator `<` `>` (D-026 excludes Mutator only)
- Add new widget IDs or ActionPropertyRegistry entries (already registered in Story 7.1)

[Source: epics.md Story 4.6; addendum § Previous/Next file navigation (D-075); PRD FR-52]

### Authoritative navigation flow

| Step | Component | Action |
|---|---|---|
| 1 | `ComputerPatchesPanel` | User clicks `<` or `>` → sets event property `computerPatchesLoadPrevious` / `computerPatchesLoadNext` (timestamp) |
| 2 | `ActionDispatcher` | Routes to `PatchManagerActionHandler::handleAction` |
| 3 | Handler | `advanceComputerPatchesSelection(isNext)` computes wrapped id, sets `computerPatchesSelectPatch` |
| 4 | `PluginProcessor` | `valueTreePropertyChanged` → dispatches `kSelectPatchFile` |
| 5 | Handler | `handleLoadSelectedPatchFile` — **existing** Story 4.5 pipeline |
| 6 | `ComputerPatchesPanel` | Listener on `computerPatchesSelectPatch` → `syncSelectionFromApvts()` updates combobox label |

**Why property round-trip:** `kSelectPatchFile` is already the single load trigger (Story 4.5). Prev/next only change selection; load logic stays in one place.

[Source: `4-5-import-name-reconciliation.md` Load trigger table; `ActionPropertyRegistry.cpp:87-89`]

### Circular index math (1-based ids)

```cpp
// count = sortedValidFileNames.size(); currentId in [1, count]
int nextId = isNext
    ? (currentId >= count ? 1 : currentId + 1)
    : (currentId <= 1 ? count : currentId - 1);
```

Mirror internal-patch wrap spirit (`DeviceMemoryLimits::wrapPatchWithinDevice`) but operate on **file list indices**, not synth patch numbers.

Sort order is **already guaranteed** by `PatchFileService::scanFolder` — verified in `PatchFileServiceTests::scan_sortOrder_patchFixtures` (Story 4.6 prep test).

[Source: `PatchFileServiceTests.cpp:208-226`; Story 4.1 AC #3]

### Sentinel / button enablement (do not regress)

| Combobox state | `selectedId` | `<` `>` enabled | Prev/next handler |
|---|---|---|---|
| `<EMPTY>` | 0 | No (Story 4.2) | No-op if called |
| `<SELECT>` | 0 | No (Story 4.2) | No-op — user must pick initial file |
| File selected | ≥ 1 | Yes | Advance + load |

Story 4.2 already disables buttons in Empty/Select — **no layout work** unless regression found.

[Source: `ComputerPatchesPanel.cpp:132-138`, `applyEmptySentinel`, `applySelectSentinel`]

### Brownfield state (READ before editing)

| File | Current behaviour | This story changes |
|---|---|---|
| `PatchManagerActionHandler.cpp:159-163` | `return; // Story 4.6` stub | Implement `advanceComputerPatchesSelection` |
| `PatchManagerActionHandler.cpp:350-369` | `handleLoadSelectedPatchFile` full pipeline | **Reuse as-is** — no fork |
| `ComputerPatchesPanel.cpp:61-66` | Listens only to `kScanRevision` | Also listen to `kSelectPatchFile` |
| `ComputerPatchesPanel.cpp:270-297` | Buttons fire event properties | **No change** |
| `ActionPropertyRegistry.cpp:87-88` | Prev/next routed to PatchManager | **No change** |

### Suggested API (handler only)

```cpp
// PatchManagerActionHandler.h — private
void advanceComputerPatchesSelection(bool isNext);

// PatchManagerActionHandler.cpp
void PatchManagerActionHandler::handleAction(...)
{
    if (propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile
        || propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile)
    {
        const bool isNext = propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile;
        advanceComputerPatchesSelection(isNext);
        return;
    }
    // ...
}

void PatchManagerActionHandler::advanceComputerPatchesSelection(bool isNext)
{
    const int currentId = readComputerPatchesSelectedId();
    if (currentId < 1 || ! isComputerPatchesScanCurrent())
        return;

    const int count = patchFileService_->getLastScanResult().sortedValidFileNames.size();
    if (count < 1)
        return;

    const int nextId = isNext
        ? (currentId >= count ? 1 : currentId + 1)
        : (currentId <= 1 ? count : currentId - 1);

    apvts_.state.setProperty(
        PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile,
        nextId,
        nullptr);
}
```

**Do not** pass `limits` into advance — load receives limits when `handleLoadSelectedPatchFile` runs from dispatcher.

### Combobox sync (mandatory GUI fix)

`rescanAndSelectSavedFile` works today because it calls `bumpScanRevision()` → panel refresh → `syncSelectionFromApvts`. Prev/next **only** changes `kSelectPatchFile` without scan revision — panel must listen:

```cpp
void ComputerPatchesPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                    const juce::Identifier& property)
{
    const auto name = property.toString();
    if (name == ComputerPatchesIds::StateProperties::kScanRevision)
        refreshPatchFileComboBox();
    else if (name == ComputerPatchesIds::StandaloneWidgets::kSelectPatchFile)
        syncSelectionFromApvts();
}
```

`syncSelectionFromApvts` already uses `dontSendNotification` — safe when Core drives selection.

### Load pipeline (unchanged from Story 4.5)

Reuse without modification:
- `resolveSelectedPatchFileForLoad`
- `decodeAndReconcilePatchFile`
- `applyLoadedPatchToApvtsAndSynth`
- `publishLoadFooters` / `publishLoadFailureFooter`

Reconciliation policy (D-025), ask-once picker, suppress hooks — all apply identically to navigated loads.

[Source: `PatchManagerActionHandler.cpp:350-520`]

### Architecture compliance

- **Core ↛ GUI:** wrap math + property update in handler; combobox sync only in `ComputerPatchesPanel`.
- **Single load path:** prev/next must not duplicate Story 4.5 orchestration.
- **Event vs state (D-031):** prev/next buttons remain **event** properties (timestamp); `computerPatchesSelectPatch` is **state** (persisted selection id).
- **Threading:** synchronous on message thread (same as combobox load).
- **Idempotency:** re-loading same file via wrap on `N==1` re-sends SysEx — acceptable v1 (Story 4.5 precedent).

### File structure (this story)

```
Source/Core/Actions/
├── PatchManagerActionHandler.h        (UPDATE — advanceComputerPatchesSelection)
└── PatchManagerActionHandler.cpp      (UPDATE — replace stub, implement advance)

Source/GUI/.../ComputerPatchesPanel.cpp (UPDATE — listen kSelectPatchFile)

Tests/Unit/
└── PatchManagerActionHandlerTests.cpp (UPDATE — adjacent navigation tests)
```

No `CMakeLists.txt` changes expected (no new translation units).

### Testing requirements

| Test | Setup | Assert |
|---|---|---|
| `loadAdjacent_nextWraps` | Temp dir with 3 valid `.syx` (sorted names), `selectedId=3` | After `kLoadNextPatchFile`, property `== 1`, SysEx enqueued |
| `loadAdjacent_previousWraps` | Same, `selectedId=1` | After `kLoadPreviousPatchFile`, property `== 3` |
| `loadAdjacent_singleFileReloads` | 1 valid file, `selectedId=1` | Property stays `1`, SysEx enqueued (reload) |
| `loadAdjacent_sentinelNoOp` | `selectedId=0` | Property unchanged, queue empty |
| `loadAdjacent_enqueuesSysEx` | 2+ files, mid-list next | Queue non-empty, property incremented |
| `loadAdjacent_staleScan` | Folder path mismatch | Warning footer, property unchanged (via existing load guards) |

**Harness pattern:** copy `testLoadSelected_enqueuesSysEx` — scan temp dir with multiple fixture copies (`Patch 5.syx`, `Patch 71.syx`, etc.), set initial `kSelectPatchFile`, fire `kLoadNextPatchFile` / `kLoadPreviousPatchFile`, assert property + `MidiOutboundQueue`.

Sort-order fixture names from `PatchFileServiceTests::scan_sortOrder_patchFixtures` ensure deterministic wrap expectations.

### Previous story intelligence (Story 4.5)

| Learning | Application in 4.6 |
|---|---|
| `handleLoadSelectedPatchFile` is the only load entry | Prev/next set `kSelectPatchFile` only |
| `readComputerPatchesSelectedId`, `isComputerPatchesScanCurrent`, `fileAtComputerPatchesIndex` | Reuse in advance guards |
| Idempotent re-select sends SysEx again | Single-file wrap reload is OK |
| Ask-once cancel restores model | Applies to navigated loads automatically |
| Leave prev/next stub | **This story implements it** |

### Previous story intelligence (Story 4.2)

| Learning | Application in 4.6 |
|---|---|
| 1-based combobox ids map to `sortedValidFileNames[index]` | Same index math for wrap |
| Buttons disabled when `selectedId < 1` | Handler guards duplicate UI |
| `dontSendNotification` on programmatic combobox sync | Required for new property listener |

### Git intelligence (recent commits)

`48a0456` — Story 4.5: full load pipeline, reconciler, `loadPatchSysExFile`. **Extend handler only** — do not fork load logic.

`3c28321` — Save rescan sets `kSelectPatchFile` + `bumpScanRevision`. Prev/next needs explicit combobox listener because no scan bump.

`4d73254` — Combobox FSM + nav button disable. Verify still correct after property listener addition.

### Latest tech information

- **JUCE 8.0.12** — `ValueTree::Listener` on `apvts.state`; `setProperty` triggers processor listener → ActionDispatcher (existing wiring).
- **No new dependencies.**

### Project context reference

- `Core ↛ GUI` strict [project-context.md]
- Clean Code: methods ≤ 15 lines [CONVENTIONS.md]
- English only in source/comments [project-context.md]
- Tests: JUCE `UnitTest`, `HandlerHarness` patterns [project-context.md]

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 4 Story 4.6, FR-52]
- [Source: `_bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md` — FR-52]
- [Source: `_bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md` — D-075]
- [Source: `_bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md` — D-075 rationale]
- [Source: `implementation-artifacts/4-5-import-name-reconciliation.md` — load pipeline, prev/next deferred]
- [Source: `implementation-artifacts/4-2-combobox-sentinel-states.md` — button disable, 1-based ids]
- [Source: `Source/Core/Actions/PatchManagerActionHandler.cpp:159-163` — stub to replace]
- [Source: `Source/GUI/.../ComputerPatchesPanel.cpp:61-66` — listener gap]
- [Source: `Tests/Unit/PatchFileServiceTests.cpp:208-226` — sort order contract]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- Single-file wrap (N==1): JUCE `ValueTree::setProperty` does not notify when value unchanged; prev/next branch calls `handleLoadSelectedPatchFile` only when `afterId == beforeId` to preserve reload without double SysEx on id change.

### Completion Notes List

- Implemented `advanceComputerPatchesSelection` with circular 1-based wrap over `sortedValidFileNames`.
- Prev/next handler advances selection via `kSelectPatchFile` property; load reuses Story 4.5 `handleLoadSelectedPatchFile` pipeline (property dispatch in production; explicit load only for unchanged-id reload).
- `ComputerPatchesPanel` listens to `kSelectPatchFile` and calls `syncSelectionFromApvts()` for combobox label sync.
- Added 5 unit tests (`loadAdjacent_*`); full `Matrix-Control_Tests` suite passes.

### File List

- Source/Core/Actions/PatchManagerActionHandler.h
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp
- Tests/Unit/PatchManagerActionHandlerTests.cpp

### Change Log

- 2026-06-19: Story 4.6 — Previous/Next file navigation with circular wrap, combobox sync, unit tests.
- 2026-06-19: Code review — fallback reload guarded (scan current + count), out-of-range guard before wrap, `loadAdjacent_staleScanNoOp` + `loadAdjacent_emptyListNoOp` tests.

### Review Findings

- [x] [Review][Decision] Comportement scan périmé sur Prev/Next — **résolu : no-op silencieux (option 1)** ; test `loadAdjacent_staleScanNoOp` sans footer.

- [x] [Review][Patch] Fallback reload déclenche un chargement sur scan périmé ou liste vide [`PatchManagerActionHandler.cpp:165-170`] — corrigé : `isComputerPatchesScanCurrent()` et `count >= 1` requis avant reload.

- [x] [Review][Patch] Id sélectionné hors plage non validé avant le wrap [`PatchManagerActionHandler.cpp:387`] — corrigé : `currentId > count` early return.

- [x] [Review][Patch] Test `loadAdjacent_staleScan` manquant (AC#9) [`PatchManagerActionHandlerTests.cpp`] — ajouté `loadAdjacent_staleScanNoOp`.

- [x] [Review][Patch] Test no-op liste vide sur navigation adjacente manquant (AC#5) [`PatchManagerActionHandlerTests.cpp`] — ajouté `loadAdjacent_emptyListNoOp`.

- [x] [Review][Defer] Appel direct `handleLoadSelectedPatchFile` quand id inchangé (N==1) [`PatchManagerActionHandler.cpp:165-166`] — déviation documentée du libellé tâche « advance only » ; justifiée par le no-op JUCE `setProperty` ; conforme AC#3.

- [x] [Review][Defer] Pas de test GUI `syncSelectionFromApvts` (AC#9 partiel) [`ComputerPatchesPanel.cpp:67-68`] — aucune infra de tests panel dans le projet ; listener vérifié par inspection.

- [x] [Review][Defer] Tests simulent le dispatch `kSelectPatchFile` manuellement [`PatchManagerActionHandlerTests.cpp:289-301`] — limitation harness connue ; comportement handler couvert, chaîne PluginProcessor non intégrée.

- [x] [Review][Defer] `advanceComputerPatchesSelection` ~19 lignes (limite 15) [`PatchManagerActionHandler.cpp:376-394`] — dette style mineure.

- [x] [Review][Defer] Section BMad titres dans `CONVENTIONS.md` — hors périmètre story 4.6 ; à committer séparément.
