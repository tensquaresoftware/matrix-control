---
title: '12-5 Mutator load policy dirty tracker and Core tests'
type: 'feature'
created: '2026-08-24'
status: 'done'
baseline_commit: '7e900a6a3a2d304246e7001d23b81b6141fece02'
review_loop_iteration: 0
context:
  - _bmad-output/specs/spec-undo-redo-patch-editing/SPEC.md
  - _bmad-output/specs/spec-undo-redo-patch-editing/undo-policy.md
  - _bmad-output/implementation-artifacts/spec-12-4-matrix-mod-reorder-undo-and-keyboard-shortcuts.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Editorial undo records edits (stories 12.1–12.4), but checkpoint events never clear the stack — `clearUndoHistory` has zero production call sites. After load, full paste, or MUTATE/RETRY, users can still undo into pre-checkpoint state. `DirtyPatchTracker` snapshots are missing on full paste and mutate/retry paths, so the footer dirty indicator can disagree with undo/redo. CAP-6 and `undo-policy.md` require stack clears, snapshot alignment, and dirty coherence after undo/redo.

**Approach:** Add a Core `establishEditorialCheckpoint` seam (clear undo stack + refresh dirty snapshot after APVTS sync) wired at every checkpoint in `undo-policy.md`. Call it from existing load/save snapshot sites and add it to full paste and `PatchMutatorEngine::pushResultToEditorAndSynth` (MUTATE/RETRY only — not `onPatchLoaded`, which resets mutator history). Keep Compare guard (already in `isEditorialUndoRedoEnabled`). Add Core unit tests for stack policy and dirty matrix; extend `manuel-utilisateur.md` §7 with checkpoint-clear behavior.

## Boundaries & Constraints

**Always:**
- Checkpoint = `undoManager_.clearUndoHistory()` then `DirtyPatchTracker::captureSnapshot` on APVTS-synced `PatchModel` (same order as `captureCleanSnapshot` today).
- Checkpoint events: synth/device load, computer `.syx` load, internal navigation to another patch, full patch paste, MUTATE, RETRY (`undo-policy.md` §Checkpoint events).
- Undo/redo must not modify the dirty snapshot — only checkpoints refresh it.
- MUTATE/RETRY checkpoint clears editorial stack and snapshot but does **not** call `onPatchLoaded` / `resetSessionForPatchLoad` (mutator history must survive).
- Compare active (`kCompareActive`) continues to block `performEditorialUndo/Redo` — no regression.
- Core owns checkpoint + dirty logic; GUI unchanged except manual doc.
- English-only source; `lint_touched.py` on touched C++ under `Source/` and `Tests/`.

**Ask First:**
- If a single `ActionExecutionHooks::onEditorialCheckpoint` callback cannot reach all sites without circular init order issues, confirm splitting into Processor method + hook before duplicating clear/snapshot logic in handlers.

**Never:**
- Mix Patch Mutator history navigation into the editorial undo stack.
- Clear mutator history on MUTATE/RETRY (only editorial checkpoint).
- Undo/Redo menu or toolbar (v1 keyboard only).
- French in source; structural rewrite of user manual beyond §7 checkpoint paragraph.
- Host DAW undo delegation.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Device/synth load | Successful dump → `applySuccessfulDeviceDump` | Stack empty (`!canUndo()`); snapshot matches loaded patch; `syncApvtsAndIsDirty` false | No-op if tracker null |
| Computer file load | `applyLoadedPatchToApvtsAndSynth` | Same as device load | — |
| Internal full paste | `applyPastedPatchToEditorAndSynth` after paste buffer applied | Stack cleared; snapshot refreshed; mutator session reset via existing `onPatchLoaded` | — |
| MUTATE | `PatchMutatorEngine::mutate` completes | Stack cleared; snapshot matches mutated patch; mutator history retained | — |
| RETRY | `PatchMutatorEngine::retry` completes | Same as MUTATE | — |
| Edit → undo to clean | Tweak one param from checkpoint baseline | One undo restores baseline; `syncApvtsAndIsDirty` false | — |
| Edit → undo → redo | Tweak param 5→10, undo, redo | After undo dirty false; after redo dirty true (diverged from snapshot) | — |
| Load after edits | Slider edit then load other patch | Pre-load undo entries gone; cannot redo pre-load edit | — |
| Compare + Cmd+Z | `kCompareActive` true after edit | `performEditorialUndo` false; stack unchanged | Already covered — regression test |

</frozen-after-approval>

## Code Map

- `Source/Core/PluginProcessor.h` / `PluginProcessorValueTree.cpp` — add `establishEditorialCheckpoint()` (or equivalent): `undoManager_.clearUndoHistory()` + delegate snapshot capture; reuse `isEditorialUndoRedoEnabled` / `performEditorialUndo/Redo` (L262–295) — read-only Compare guard.
- `Source/Core/PluginProcessorConstruction.cpp:115-132` — wire new `ActionExecutionHooks::onEditorialCheckpoint` lambda to Processor checkpoint; keep `beginEditorialTransaction` (L127).
- `Source/Core/Actions/ActionExecutionHooks.h` — add `onEditorialCheckpoint` callback (message thread).
- `Source/Core/Actions/PatchManagerActionHandler.cpp:232-244` — `captureCleanSnapshot`: call checkpoint hook after sync (or fold into `establishEditorialCheckpoint` and replace direct `captureSnapshot` calls).
- `Source/Core/Actions/PatchManagerActionHandlerDeviceLoad.cpp:155` — device load already calls `captureCleanSnapshot` — ensure stack clear piggybacks.
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp:228,290` — computer load + SAVE success snapshot sites.
- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp:308-323` — `applyPastedPatchToEditorAndSynth`: **missing** snapshot + stack clear today; calls `onPatchLoaded` only.
- `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp:356,443` — INIT/STORE snapshot sites.
- `Source/Core/Actions/PatchManagerActionHandlerBankImportWrite.cpp:225` — bank import snapshot site.
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp:308-332` — `pushResultToEditorAndSynth`: add editorial checkpoint after `pushPatchModelToApvtsWithSuppress`; **do not** invoke `onPatchLoaded`.
- `Source/Core/Services/DirtyPatchTracker.h` / `.cpp` — read-only; `captureSnapshot`, `syncApvtsAndIsDirty` (CAP-6 contract).
- `Source/Core/PluginProcessorGates.cpp:133-169` — `syncApvtsAndIsDirty` consumers for unsaved-edit gates — read-only.
- `Tests/Unit/DirtyPatchTrackerTests.cpp` — snapshot/sync unit tests; no undo integration yet.
- `Tests/Unit/UndoManagerEditorialUndoApiTests.cpp` — Compare guard pattern for harness reuse.
- `Tests/Unit/UndoManagerModuleBulkTestSupport.h` — ProcessorPathHarness / UndoManager wiring reference.
- `Tests/Unit/UndoManagerModuleBulkTests.cpp` — bulk undo harness pattern for checkpoint tests.
- `Documentation/Users/manuel-utilisateur.md:236-248` — §7: add checkpoint clears (load/paste/mutate reset undo baseline).
- `_bmad-output/specs/spec-undo-redo-patch-editing/undo-policy.md:48-73,92-101` — canonical checkpoint + dirty matrix (CAP-6).

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Actions/ActionExecutionHooks.h` — add `onEditorialCheckpoint` hook — uniform checkpoint seam for handlers and mutator.
- [x] `Source/Core/PluginProcessor.h` / `PluginProcessorValueTree.cpp` — implement `establishEditorialCheckpoint` (clear stack + snapshot) — Core ownership of CAP-6 checkpoint.
- [x] `Source/Core/PluginProcessorConstruction.cpp` — wire hook to Processor method — init-order-safe callback.
- [x] `Source/Core/Actions/PatchManagerActionHandler.cpp` — route `captureCleanSnapshot` through checkpoint (stack clear + snapshot) — single baseline helper.
- [x] `Source/Core/Actions/PatchManagerActionHandlerInternalPatches.cpp` — ensure `applyPastedPatchToEditorAndSynth` triggers checkpoint — full paste gap.
- [x] `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` — checkpoint after `pushResultToEditorAndSynth` APVTS push — MUTATE/RETRY baseline without `onPatchLoaded`.
- [x] `Tests/Unit/UndoManagerCheckpointPolicyTests.cpp` — stack clear on load/paste/mutate harness paths — CAP-6 policy proof without GUI.
- [x] `Tests/Unit/UndoManagerDirtyStateTests.cpp` — edit→undo→clean and edit→undo→redo dirty matrix — CAP-6 dirty coherence.
- [x] `Tests/CMakeLists.txt` — register new test files.
- [x] `Documentation/Users/manuel-utilisateur.md` — §7 checkpoint paragraph (load/paste/mutate clear undo history) — deferred from 12-4.
- [x] `python3 Scripts/quality/lint_touched.py` — zero findings on touched C++.

**Acceptance Criteria:**
- Given APVTS with UndoManager and a recorded slider edit, when `establishEditorialCheckpoint` runs (simulated load), then `undoManager_.canUndo()` is false.
- Given full patch paste via `applyPastedPatchToEditorAndSynth`, when paste completes, then stack is empty and `syncApvtsAndIsDirty` returns false.
- Given MUTATE or RETRY pushing a new patch buffer, when `pushResultToEditorAndSynth` completes, then stack is empty, snapshot matches live buffer, and mutator history is non-empty (history not reset).
- Given checkpoint baseline and one param tweak (+5), when `performEditorialUndo` runs, then `syncApvtsAndIsDirty` is false.
- Given tweak undone, when `performEditorialRedo` runs, then `syncApvtsAndIsDirty` is true.
- Given `kCompareActive` true, when `performEditorialUndo` is called, then it returns false (regression).
- Given `manuel-utilisateur.md` §7 after edit, when read, then checkpoint clears (load, paste, mutate) are documented alongside existing shortcut/grouping text.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` — expected: build succeeds.
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests && ./Builds/macOS/Debug/Matrix-Control_Tests` — expected: all unit tests pass including new checkpoint/dirty tests.
- `python3 Scripts/quality/lint_touched.py` — expected: zero findings on touched C++.

**Manual checks:**
- Standalone: edit param, load another patch — Cmd+Z does not restore pre-load edit.
- MUTATE, tweak slider, Cmd+Z reverts tweak only; mutator history combo still populated.
- Footer dirty: edit → undo until clean → indicator matches (no false dirty).

### Review Findings

- [x] [Review][Decision] Should bank import slot apply clear editorial undo? — **Resolved: keep checkpoint** (same UX as load when current slot is replaced).
- [x] [Review][Decision] Should full Internal Patches INIT clear editorial undo? — **Resolved: keep checkpoint** (full INIT = new baseline).
- [x] [Review][Patch] Editorial checkpoint fires on Compare and M/R history navigation [`PatchMutatorEngine.cpp`] — **Fixed:** checkpoint only after MUTATE/RETRY in `PatchMutatorEngineActions.cpp`.
- [x] [Review][Patch] No test for full-patch paste checkpoint (AC #2) [`UndoManagerCheckpointPolicyTests.cpp`] — **Fixed:** `fullPaste_clearsUndoAndRefreshesSnapshot`.
- [x] [Review][Patch] Wire `onEditorialCheckpoint` in shared PatchManager test harness [`PatchManagerActionHandlerTestSupport.cpp`] — **Fixed:** mirrors production clear + `syncDirtySnapshotFromApvts`.
- [x] [Review][Patch] Checkpoint policy test duplicates Processor seam [`UndoManagerCheckpointPolicyTests.cpp`] — **Fixed:** uses HandlerHarness + production checkpoint delegation.
- [x] [Review][Patch] CMake test source list indentation [`CMakeLists.txt:552`] — **Fixed.**
- [x] [Review][Patch] Duplicate `#include "Core/Init/InitDefaults.h"` [`UndoManagerCheckpointPolicyTests.cpp`] — **Fixed.**
- [x] [Review][Defer] Load-path integration tests (device/computer dump success → `!canUndo`) — deferred, pre-existing harness gap; harness-only checkpoint test partially covers policy.
- [x] [Review][Defer] Manual §7 beyond checkpoint paragraph (navigation, SAVE vs checkpoint, dirty↔undo footer) — deferred, story scope was §7 baseline paragraph only (AC #6).
