---
title: 'Skin Designer Undo/Redo history'
type: 'feature'
created: '2026-11-12'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '03da3a247936d3ca502bbbed5e24e54e3b099a3b'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Designer Session Undo/Redo buttons and Cmd/Ctrl+Z shortcuts are placeholders, so Designer edits cannot be walked back.

**Approach:** Add a document history stack for Designer skins (snapshot restore via `Layer::replaceAll`), wire Session buttons plus standard OS shortcuts, coalesce colour-drag gestures into one step, and clear history on Load / external JSON reload.

## Boundaries & Constraints

**Always:**
- Undo/Redo covers Designer document mutations (palette, mappings, link groups, ColourSet labels, New/Duplicate/Rename/Delete Designer skins).
- One colour-picker drag (or begin/end nudge session) = one history step.
- Shortcuts: macOS Cmd+Z / Cmd+Shift+Z; Windows/Linux Ctrl+Z / Ctrl+Shift+Z (JUCE `commandModifier`).
- Disable Undo/Redo while Compare is on (Designer is read-only).
- Clear history after Load and after JSON Sync disk reload that replaces Designer skins.
- Save / Save as do not clear history; they record a clean snapshot marker. Undo/Redo that returns to that marker marks the document clean again (Save inactive).
- While a Hex/R/G/B `TextEditor` or modal rename field has focus, Cmd/Ctrl+Z / Shift+Z leave document history alone (text/local undo wins). Document shortcuts apply when no such text field is focused.
- Selection of Product skin, JSON Sync mode, and Compare toggle are not undoable.
- Reuse `TSS::classifyEditorialUndoRedoShortcut`; do not wire plugin `UndoManager` / APVTS / Core editorial MIDI paths.
- English UI strings; ASCII-only chrome copy; `lint_touched.py` on touched Skin Designer / tests.

**Never:**
- Per-mutator `UndoableAction` explosion or ValueTree rewrite of `DevSkin::Layer`.
- Ctrl+Y as Redo (out of requested shortcuts).
- Undoing across a Load / disk-reload baseline.
- Changing Session button layout or Compare placement.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Edit then Undo | One Designer mutation | Previous document restored; Redo enabled; live preview refresh/push | N/A |
| Colour drag | Many ARGB updates in one gesture | Single Undo step undoes whole drag | N/A |
| Redo after Undo then new edit | Undo, then another edit | Future stack cleared; new step pushed | N/A |
| Compare on | `compareOn` true | Undo/Redo buttons and shortcuts inactive | Status optional; no-op |
| Load / JSON reload | Replace Designer skins | History cleared; buttons disabled until next edit | N/A |
| Undo back to last Save | History returns to clean marker | `designerDirty_` false; Save inactive | N/A |
| Shortcut with text field focus | Hex/RGB or modal rename focused | Document Undo/Redo not invoked; text keeps the key | N/A |
| Undo empty | No past steps | No-op; Undo stays disabled | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinDesigner/SkinDesignerMainComponentActions.cpp` -- `onUndo` / `onRedo` placeholders to implement; New/Duplicate/delete/rename/pastille actions.
- `Tools/SkinDesigner/SkinDesignerMainComponentSession.cpp` -- `keyPressed` Z shortcuts; `markDesignerDirty/Clean`; `runWithoutLayerChangeNotify`; `refreshSessionButtonsEnabled`.
- `Tools/SkinDesigner/SkinDesignerMainComponentSkins.cpp` -- buttons wired but hard-disabled at setup.
- `Tools/SkinDesigner/SkinDesignerMainComponent.cpp` -- `layer_.onChanged` dirty + preview coalesce; `colourGestureActive_`.
- `Tools/SkinDesigner/SkinDesignerMainComponentFiles.cpp` / `ImportWatch.cpp` -- Load / reload replace paths (history clear).
- `Tools/SkinDesigner/SkinDesignerColourPicker.cpp` -- gesture begin/end; EscapeKeyForwarder pattern; Hex/RGB editors.
- `Source/GUI/Skins/Dev/DevSkinLayer.h` -- `skins()`, `activeSkinId()`, `replaceAll`, `onChanged` / `bump`.
- `Source/GUI/Helpers/EditorialUndoRedoShortcuts.h` -- shortcut classifier to reuse.
- New: `Tools/SkinDesigner/SkinDesignerHistory.{h,cpp}` -- past/future snapshot stacks + capture/restore API; register in `CMakeLists.txt`.
- Tests: new unit tests for history push/undo/redo/clear/gesture coalesce (no GUI MainComponent harness required).

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinDesigner/SkinDesignerHistory.{h,cpp}` + CMake -- snapshot stack (`NamedSkin` vector + active/designer ids); cap ~100; push/undo/redo/clear/canUndo/canRedo.
- [x] `Tools/SkinDesigner/SkinDesignerMainComponent*` -- capture before edits (onChanged previous-snapshot + colour gesture + silent New/Duplicate); restore via `replaceAll` under `runWithoutLayerChangeNotify`; refresh UI/bridge; enable buttons; clear on Load/reload; disable under Compare.
- [x] Dirty marker -- track clean snapshot at Save / Save as / Load / reload; set dirty from history current vs clean marker after edits and Undo/Redo.
- [x] Shortcuts -- switch to `classifyEditorialUndoRedoShortcut`; skip document Undo/Redo when a text field has focus; keep Escape mapping-mode behavior.
- [x] `Tests/Unit/*SkinDesignerHistory*` -- unit-cover matrix rows (edit/undo/redo, clear, gesture coalesce, empty stack, clean marker).
- [x] Verify build + `lint_touched.py` on touched paths.

**Acceptance Criteria:**
- Given a Designer colour or mapping edit, when Undo then Redo, then the document and live preview match before/after that step.
- Given a colour drag, when Undo once, then the whole drag reverts as one step.
- Given Compare on, when Undo/Redo button or shortcut, then nothing changes and controls stay disabled.
- Given Load or JSON Sync reload that replaces skins, when Undo, then there is no prior history.
- Given Undo back to the last Save snapshot, when history matches that marker, then Save is inactive again.
- Given Hex/RGB or modal rename focus, when Cmd/Ctrl+Z is pressed, then document history does not run.

## Implementation Notes

- Added `SkinDesignerHistory::{documentHistoryAllowed,shouldDeferDocumentUndoRedoToTextField}` for Compare/text-field matrix coverage; MainComponent delegates to them.
- Multi-write user actions (mapping + link peers) coalesce via `openChangeBurst` until the next message-loop idle (same gesture API as colour drag).
- Delete skin records history after silent delete + `refreshAll` so the next Designer selection is inside the snapshot.
- Verification used `Matrix-Control_Tests --category SkinDesignerHistory` (no ctest target); build dir `Builds/macOS/ARM/Debug`.
- COLOR well/slider: live `onChanged` during a press does not record history; one `recordDocumentChange` runs on gesture end so discrete SV clicks are separate Undo steps and a drag stays one step.
- Undo/Redo keeps the selected pastille (and Held/Editing colour mode); only ArmedRow bind mode is cleared. `refreshAll` re-syncs the SV well to the restored ARGB so COLOR does not go empty while the pastille swatch updates.

## Spec Change Log

### Review Findings

- [x] [Review][Patch] Delete confirm still says it cannot be undone [Tools/SkinDesigner/SkinDesignerMainComponentActions.cpp:85]
- [x] [Review][Patch] Undo/Redo Session buttons stay enabled mid colour press while handlers no-op [Tools/SkinDesigner/SkinDesignerMainComponentSession.cpp:289]
- [x] [Review][Patch] JSON Sync reload leaves colour-gesture flags live so gesture end can re-record after history clear [Tools/SkinDesigner/SkinDesignerMainComponentImportWatch.cpp:323]
- [x] [Review][Patch] Save during an open colour gesture can mark the pre-press snapshot clean while disk already has live colours [Tools/SkinDesigner/SkinDesignerMainComponentFiles.cpp:186]
- [x] [Review][Defer] Clean marker can fall off the undo cap so full undo never clears dirty [Tools/SkinDesigner/SkinDesignerHistory.cpp:28] — deferred: already recorded 2026-11-12; rare deep-history case
- [x] [Review][Defer] No MainComponent harness for record/restore/burst/colour/Delete wiring [Tests/Unit/SkinDesignerHistoryTests.cpp] — deferred: already recorded 2026-11-12; spec allowed History-only tests
- [x] [Review][Defer] Load/Save/reload dirty+history reset wiring unverified at MainComponent [Tools/SkinDesigner/SkinDesignerMainComponentFiles.cpp] — deferred: already recorded 2026-11-12; same harness gap
- [x] [Review][Defer] Compare/text-focus/keyPressed Session gates only helper-tested [Tools/SkinDesigner/SkinDesignerMainComponentSession.cpp] — deferred: already recorded 2026-11-12; manual smoke remains

Rejected:
- Spec Design Notes / Code Map / Verification still describe pre-ship design — rejected: fix would edit the spec under review
- Frozen I/O matrix omits New/Duplicate/Delete and change-burst — rejected: fix would edit the spec under review
- openChangeBurst stale callAsync can endGesture on a newer burst — false: same-message close+reopen still ends after the message; coalescing window is unchanged
- Colour net-zero press still records a history step — false: `setPastilleArgb` returns without `bump` when ARGB unchanged, so `colourGestureMadeEdit_` stays false
- `History::recordChange` never ignores an identical snapshot — false for product paths: layer mutators no-op without `onChanged`; remaining case is intentional API
- `assertCommitOnEndDiscretePresses` overclaims COLOR gesture coverage — low: History-level discrete commits are intentional; MainComponent colour path is already deferred
- History header overstates nudge coalescing via `beginGesture` — low: nudge uses colour begin/end; everyday impact negligible

## Review Triage Log

- Blind: burst async endGesture vs colour drag ownership — medium — patched: `closeChangeBurst` on colour begin / reset / apply / markClean / adopt
- Blind: reset leaves changeBurstOpen_ — medium — patched (same `closeChangeBurst`)
- Blind: no-op onChanged pushes steps — false — `setPastilleArgb` returns without bump when ARGB unchanged; other mutators fail or bump only on real change
- Blind: unbounded snapshot memory/CPU — low — deferred: snapshot approach is intentional; revisit if packs grow painful
- Blind: restore order refreshAll then selectSkin — false — `refreshAll` uses `designerSkinId`; trailing silent `selectSkin` + `syncHistoryCurrentSelection` align layer active id
- Blind: Undo/Redo buttons not refreshed after record — false — `onChanged` non-gesture path calls `applyColourPreviewRefresh` → `refreshAll` → `refreshSessionButtonsEnabled`; gesture path refreshes on coalesce flush
- Blind: silent write paths may skip history — false after audit — silent paths either `finishSilentDocumentEdit` / `recordDocumentChange` / `resetHistory` / selection-only `syncCurrentSelection`
- Blind: no status Undone/Redone; Load clears quietly — low — rejected: cosmetic; Load clear is specified
- Blind: EditorialUndoRedoShortcuts .cpp missing / TextEditor test needs GUI init / History.h pulls gui_basics — false / low — header-only helper; tests passed headless; gui_basics only for focus policy helpers
- Blind: extra History unit gaps (cap/clean/gesture) — low — deferred: core matrix covered; optional hardening later
- Edge: Undo mid colour drag — medium — patched: ignore Undo/Redo while `colourGestureActive_`
- Edge: stale async after burst then colour — medium — patched (`closeChangeBurst` before colour begin)
- Edge: Save/markClean during coalesce overwrites clean — medium — patched: `closeChangeBurst` before `markClean`
- Edge: stale selectedPastille_ after restore — false — `syncPastilleSelectionAfterRefresh` clears missing names
- Edge: no-op layer bump — false — same as Blind no-op
- Edge: per-tick full copy — low — deferred with snapshot memory note
- Edge: shortcut swallowed when Compare/empty — false — matches plugin editorial key handling (consume, no-op)
- Edge: buttons stale after record — false — same refreshAll path
- Edge: construction buttons enabled before first refresh — low — rejected: first `refreshAll` soon after setup
- Edge: Undo restores designerSkinId (selection side effect) — medium — patched: `syncCurrentSelection` on silent Designer select so meta stays current; restore of id with document remains intentional (e.g. Delete)
- Edge: Compare shortcut consume claim — false — same as swallow finding
- Edge: clean marker lost via cap/coalesce — low/medium — coalesce case patched via markClean; cap eviction deferred as rare edge
- VG: MainComponent record/burst/Delete/New/Duplicate unverified — medium — deferred: spec allowed no MainComponent harness
- VG: Save/Load/reload dirty+reset wiring unverified — medium — deferred: same
- VG: keyPressed/Compare/text only helper-tested — medium — deferred: same; manual smoke remains
- VG other: silent select left stale designerSkinId in current — medium — patched via `syncCurrentSelection`

## Design Notes

Prefer whole-document snapshots over `juce::UndoManager`: `NamedSkin` is copyable, `replaceAll` restores, panels mutate `Layer` directly so one `onChanged` hook covers them. Capture the pre-edit snapshot (keep a "baseline" after each committed change; push it on the next change). Colour: push once at gesture begin, suppress further pushes while `colourGestureActive_`. After restore: clear mapping mode, refresh panels, `pushVisible` if connected, bump revision via `replaceAll` (do not roll revision backward).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinDesigner Matrix-Control_Tests` -- expected: build OK
- `ctest --test-dir Builds/macOS/Debug --output-on-failure -R SkinDesignerHistory` (or final test name) -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Session Undo/Redo enable after edit; Cmd/Ctrl+Z / Shift+Z on macOS; Compare disables; Load clears history; text-field policy smoke.
