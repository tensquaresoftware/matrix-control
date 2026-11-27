---
title: '12-4 Matrix Mod reorder undo and keyboard shortcuts'
type: 'feature'
created: '2026-08-24'
status: 'done'
baseline_commit: 'f98e7605c64398f842fbc32a9405111f8257afee'
review_loop_iteration: 0
context:
  - _bmad-output/specs/spec-undo-redo-patch-editing/SPEC.md
  - _bmad-output/specs/spec-undo-redo-patch-editing/undo-policy.md
  - _bmad-output/specs/spec-undo-redo-patch-editing/keyboard-shortcuts.md
  - _bmad-output/implementation-artifacts/spec-12-1-spike-undomanager-on-apvts-with-one-slider-and-midi-proof.md
  - _bmad-output/implementation-artifacts/spec-12-2-bulk-editorial-transactions-for-init-and-paste-module.md
  - _bmad-output/implementation-artifacts/spec-12-3-interactive-display-gesture-transactions.md
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Matrix Mod bus drag-reorder already swaps bus contents and emits SysEx, but the swap is not grouped into an editorial undo transaction — users cannot revert a reorder with undo. Editorial undo also has no keyboard entry point: `PluginEditor::keyPressed` handles Escape only, and slider arrow keys still lack per-focus-session grouping promised in `undo-policy.md`. The user manual still states there is no Undo/Redo.

**Approach:** Open one `beginNewTransaction` before a completed Matrix Mod reorder and ensure bus APVTS writes record under that transaction (mirror story 12-2 bulk `getParameterAsValue().setValue` pattern). Add Core `performEditorialUndo` / `performEditorialRedo` seams guarded by `kCompareActive`, wire Cmd/Ctrl+Z and Shift variant in `PluginEditor` with focus/overlay rules from `keyboard-shortcuts.md`. Group slider arrow edits in one transaction per focus session in `TSS::Slider`. Update `manuel-utilisateur.md` §7.

## Boundaries & Constraints

**Always:**
- Core owns undo/redo execution; GUI forwards shortcuts only — no `UndoManager` calls from `Source/Core/` upward.
- One `beginNewTransaction` per **completed** bus reorder (source ≠ target); no transaction when drag ends on same bus or invalid indices.
- Keep existing `suppressMatrixModParameterSysEx_` wrap in `swapMatrixModBusContents`; undo/redo rely on normal APVTS listener → SysEx dispatch when suppress is off.
- Shortcuts: macOS ⌘Z / ⌘⇧Z; Windows/Linux Ctrl+Z / Ctrl+Shift+Z via `ModifierKeys::commandModifier` + `KeyPress::zKey`.
- Skip patch undo when `kCompareActive` is true, when `isEscapeBlockedByOverlay()` is true (Settings, About, master-init confirm, bank-transfer progress), or when keyboard focus is in an active text editor (`PatchNameDisplay` edit session, Settings text fields).
- Slider arrow keys: first arrow keypress after focus opens one transaction; further arrow keypresses until `focusLost` stay in the same transaction; mouse drag grouping unchanged (existing `ScopedDragNotification` + attachment).
- English-only source; `lint_touched.py` on touched C++ under `Source/` and `Tests/`.

**Ask First:**
- If `pushBusToApvts` cannot record undo with current `setValueNotifyingHost` helpers, confirm switching bus push to `getParameterAsValue().setValue` (scoped to reorder path or shared push helpers) before broader mapper refactors.

**Never:**
- Undo/Redo menu items or toolbar buttons (v1 keyboard only).
- `DirtyPatchTracker` coherence or stack-clear on load/mutate (story 12-5).
- Changing Matrix Mod reorder geometry, highlight colours, or SysEx coalesce semantics (story 2.10).
- Host DAW undo delegation.
- French in source or user-manual structural rewrite beyond §7 undo content.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Matrix Mod reorder undo | Bus 0 triplet (4, -15, 9); bus 7 triplet (1, 22, 3); drag-reorder 0→7 | One undo transaction; one `undo()` restores original bus 0 and bus 7 contents in APVTS + model | No-op when source == target |
| Matrix Mod reorder redo | After undo above | `redo()` restores post-swap layout | N/A |
| SysEx after reorder undo | Device path enabled; reorder then `undo()` | 0x06 Matrix Mod messages for affected buses via existing dispatch | Suppress off during undo restore |
| Cmd+Z with stack | Prior slider drag recorded | `performEditorialUndo()` runs; values restored | Silent no-op when `!canUndo()` |
| Compare active | `kCompareActive` true | Shortcut ignored; stack unchanged | N/A |
| Settings text focus | Settings open; text field focused | Native text behavior; no patch undo | N/A |
| Slider arrows | Focus slider at 10; Up×3 to 13; focus lost | One transaction; one undo restores 10 | N/A |
| Non-editorial property | Footer message after edit | `setProperty(..., nullptr)` — stack size unchanged | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/PluginProcessorValueTree.cpp:83-91` — `swapMatrixModBusContents`: add `undoManager_.beginNewTransaction` before swap; keep suppress wrap.
- `Source/Core/MIDI/MatrixModBusReorderService.cpp:22-73` — read-only reorder logic; writes go through `pushBusToApvts`.
- `Source/Core/Models/ApvtsPatchMapper.cpp:36-48,81-93` — `pushBusToApvts` uses `pushIntToApvts` / `pushChoiceToApvts` with `setValueNotifyingHost` today; module bulk uses `getParameterAsValue().setValue` in `pushModuleToApvts` — align bus push for undo recording under open transaction.
- `Source/GUI/PluginEditorUiConstruction.cpp:38-42` — bus reorder handler calls `swapMatrixModBusContents`; read-only wiring.
- `Source/GUI/Panels/.../MatrixModulationPanel.cpp:168-180` — `finishBusReorderDrag` invokes handler only when source ≠ target.
- `Source/GUI/PluginEditor.cpp:113-137` — extend `keyPressed` for undo/redo after Escape branch.
- `Source/GUI/PluginEditorWindows.cpp:50-61` — `isEscapeBlockedByOverlay` reuse for undo blocking overlays.
- `Source/GUI/PluginEditor.h` — declare helper to detect text-field focus stealing shortcuts (mirror `keyboard-shortcuts.md`).
- `Source/Core/PluginProcessor.h:96-97,402` — `getUndoManager()` / `undoManager_`; add `canPerformEditorialUndo/Redo` + `performEditorialUndo/Redo` checking `MutatorState::kCompareActive`.
- `Source/GUI/Widgets/Slider.cpp:235-277` — `focusGained` / `focusLost` / `keyPressed`: arrow-session transaction via `apvts.getUndoManager()` (wire APVTS or undo callback from `ParameterCell` / `ModulationBusCell`).
- `Source/GUI/Widgets/ParameterCell.cpp:59` — `SliderAttachment` path for patch sliders; pass undo seam to `TSS::Slider`.
- `Source/GUI/Widgets/ModulationBusCell.cpp:77` — amount slider attachment; same arrow grouping if slider gains focus.
- `Source/GUI/Helpers/CompareLockBinder.cpp` — read-only; existing Compare lock pattern for controls.
- `Tests/Unit/MatrixModBusReorderServiceTests.cpp:289-327` — `ProcessorPathHarness.swapMatrixModBusContents` mirror for undo proof.
- `Tests/Unit/UndoManagerApvtsSpikeTests.cpp:83-97` — reference `beginNewTransaction` + gesture simulation.
- `Tests/Unit/UndoManagerModuleBulkTestSupport.h` — harness patterns for UndoManager + SysEx queue assertions.
- `Documentation/Users/manuel-utilisateur.md:236` — replace no-undo paragraph; document shortcuts and grouping.
- `_bmad-output/specs/spec-undo-redo-patch-editing/keyboard-shortcuts.md` — canonical focus rules.
- `_bmad-output/specs/spec-undo-redo-patch-editing/undo-policy.md:20-23,92` — arrow grouping + Matrix Mod reorder matrix.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/PluginProcessor.h` / `.cpp` — add editorial undo/redo API guarded by Compare — Core seam for GUI (CAP-2/CAP-3).
- [x] `Source/Core/PluginProcessorValueTree.cpp` — `beginNewTransaction("Matrix Mod reorder")` before `swapBusContents` — CAP-4 reorder grouping.
- [x] `Source/Core/Models/ApvtsPatchMapper.cpp` — ensure `pushBusToApvts` records undo under open transaction (`getParameterAsValue().setValue` or equivalent) — CAP-1/CAP-4.
- [x] `Source/GUI/PluginEditor.cpp` (+ `.h` if needed) — Cmd/Ctrl+Z / Shift+Z with overlay and text-focus guards — CAP-3.
- [x] `Source/GUI/Widgets/Slider.cpp` (+ `.h`) — arrow-key focus-session transaction; optional APVTS/undo callback from attachments — CAP-4 arrows.
- [x] `Source/GUI/Widgets/ParameterCell.cpp` / `ModulationBusCell.cpp` — pass undo seam into sliders — wires patch + Matrix Mod amount sliders.
- [x] `Tests/Unit/UndoManagerMatrixModReorderTests.cpp` — reorder swap + single undo/redo + SysEx after undo — CAP-4/CAP-5 without GUI.
- [x] `Tests/Unit/UndoManagerSliderArrowTests.cpp` — simulated arrow session grouping — CAP-4 arrows without GUI.
- [x] `Tests/CMakeLists.txt` — register new test files.
- [x] `Documentation/Users/manuel-utilisateur.md` — §7 undo/redo shortcuts and grouping — SPEC constraint.
- [x] `python3 Scripts/quality/lint_touched.py` — zero findings on touched C++.

**Acceptance Criteria:**
- Given APVTS with UndoManager and distinct bus 0 / bus 7 triplets, when `swapMatrixModBusContents(0, 7)` runs, then `getUndoDescriptions().size()` increases by 1 and one `undo()` restores pre-swap bus contents in APVTS and PatchModel.
- Given the same harness after undo, when `redo()` runs, then bus contents match post-swap state.
- Given Compare active (`kCompareActive` true), when `performEditorialUndo()` is invoked, then it returns false and the undo stack is unchanged.
- Given `PluginEditor` focus rules (no overlay, no text field focus), when user presses platform undo shortcut after a recorded edit, then `performEditorialUndo()` runs and restores the prior value.
- Given slider at 10 with keyboard focus, when three increment arrow keypresses occur then focus is lost, then exactly one undo transaction exists and one `undo()` restores 10.
- Given `manuel-utilisateur.md` §7, when read after implementation, then it documents Undo/Redo shortcuts and does not state that undo is unavailable.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` — expected: build succeeds.
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests && ./Builds/macOS/Debug/Matrix-Control_Tests` — expected: all unit tests pass including new undo tests.
- `python3 Scripts/quality/lint_touched.py` — expected: zero findings on touched C++.

**Manual checks:**
- Standalone: drag-reorder Matrix Mod bus, Cmd+Z restores layout; Cmd+Shift+Z reapplies; Compare active blocks shortcuts.
- Focus slider, arrow keys several times, blur focus, Cmd+Z once reverts entire arrow session.

### Review Findings

- [x] [Review][Patch] Consume Cmd/Ctrl+Z even when editorial undo is blocked or unavailable — always return `true` from `tryHandleEditorialUndoRedoKey` when the shortcut is recognized. [`Source/GUI/PluginEditor.cpp:172-183`]

- [x] [Review][Patch] Matrix Mod init/paste missing editorial transaction wrapper — added `hooks_.beginEditorialTransaction` in `handleMatrixModInit` and `pasteMatrixModulationSection`. [`Source/Core/Actions/ModuleActionHandler.cpp`]

- [x] [Review][Patch] `beginNewTransaction` before invalid bus index guard — validate bus indices before opening reorder transaction. [`Source/Core/PluginProcessorValueTree.cpp`]

- [x] [Review][Patch] Undo shortcut uses `KeyPress::zKey` instead of `getTextCharacter()` for layout-safe matching. [`Source/GUI/PluginEditor.cpp:172-183`]

- [x] [Review][Patch] Editorial undo API tests — added `UndoManagerEditorialUndoApiTests.cpp` covering Compare guard and undo/redo restore.

- [x] [Review][Patch] Slider arrow undo tests — `TSS::Slider` key path + gesture-session undo tests in `UndoManagerSliderArrowTests.cpp`; arrow sessions use `ScopedDragNotification` in `Slider.cpp`.

- [x] [Review][Defer] No automated `PluginEditor::keyPressed` undo/redo test — AC4 is covered by manual verification in spec; GUI harness cost deferred. [`Source/GUI/PluginEditor.cpp:172-183`] — deferred, manual AC4 path accepted for v1

- [x] [Review][Defer] Envelope/TrackGenerator binding `getUndoManager()` → `apvts_.undoManager` — minor supporting fix outside listed tasks; no behavior change intended for 12-4. [`EnvelopeDisplayApvtsBinding.cpp`, `TrackGeneratorDisplayApvtsBinding.cpp`] — deferred, out-of-scope cleanup

- [x] [Review][Defer] Manual §7 omits undo-policy extras (checkpoint clears, stack depth, dirty state) — AC6 satisfied; fuller manual alignment deferred to story 12-5 stack-clear work. [`Documentation/Users/manuel-utilisateur.md`] — deferred, story 12-5 scope
