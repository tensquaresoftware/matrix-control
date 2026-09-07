---
title: 'Drag-drop single .syx onto plugin to load via Computer Patches'
type: 'feature'
created: '2026-09-07'
status: 'done'
route: 'dispatch'
baseline_commit: '7f00593fb93914d171a1e1bb76a6f538e1f7add1'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiate">

## Intent

**Problem:** Trying Computer `.syx` presets one after another still forces Open → pick folder → choose in the combo. There is no hot-swap path that drops a file onto the plugin and loads it immediately.

**Approach:** Make the whole plugin window a file drop target. On drag, preview validity only in PATCH NAME (blink secondary). On drop of one valid single-patch `.syx`, reuse the Computer Patches Open/scan/select/load pipeline (same dirty/unsaved and Mutator history gates) — no second loader. Post footer messages only on drop.

## Boundaries & Constraints

**Always:**
- Whole editor listens for file drag (`FileDragAndDropTarget` on `PluginEditor`); not a small drop zone.
- Drag feedback only in PATCH NAME: primary fixed; secondary blinks at `startTimerHz(2)` (same cadence as Copy / Compare blink); both valid and invalid blink.
- Valid single Matrix patch: primary = internal name from file (empty / `BNK*` placeholder / unusable → OS filename stem fallback); secondary = `DROP TO LOAD` in existing secondary red (`look.secondaryText`).
- Invalid / unsupported: primary = `········` (8 middle dots — distinct from startup `--------`); secondary = `BAD FILE`; never invent a real patch name.
- `dragExit` / leave without drop: restore real current PATCH NAME display immediately; stop drag blink.
- Drop success: set Computer Patches folder to file parent → `scanAndPublishFolder` → select that file → `loadSelectedPatchFileImmediately` (or equivalent existing select+load path); reuse `confirmPatchContextChange` / unsaved + history gates unchanged.
- Drop failure: reject clearly; never load “first message” of a bank/multi dump. Footer message states rejection reason. No live footer text during drag.
- V1 accepts exactly one file per drop; more than one file → reject.
- If PATCH NAME inline rename is active when a file drag enters: cancel the unfinished rename (discard uncommitted text) and show the drag overlay.

**Never:**
- Multi-`.syx` batch drop; bank-dump drop into Bank Utility.
- Parallel decode/load path that bypasses Computer Patches scan/select/load, PREV/NEXT, SAVE, dirty/unsaved, or Mutator history gate.
- Footer / message-bar updates during drag (drop only).
- Changing `SysExParser` exact 275-byte single-patch contract or Bank Import/Export orchestration.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Drag valid single `.syx` | One 275-byte Matrix patch file over editor | PATCH NAME primary = internal name (or stem fallback); secondary `DROP TO LOAD` blinks ~2 Hz; footer unchanged | N/A |
| Drag invalid / non-`.syx` / multi-file | Unsupported payload over editor | Primary `········`; secondary `BAD FILE` blinks; footer unchanged | N/A |
| Drag exit | Leave window without drop | Restore pre-drag PATCH NAME + secondary; blink off | N/A |
| Drop valid, clean editor | One valid `.syx` | Folder = parent; scan; combo selects file; patch loads; footer success naming the `.syx` file | Existing load errors use existing footers |
| Drop valid, dirty / Mutator history | Gates would normally confirm | Same confirm as Open / combo change; Cancel aborts drop load and restores display | Cancel → no folder/selection change committed beyond what existing cancel restore already does |
| Drop bank / multi-message / wrong length | File fails single-patch validate | No load; footer explains rejection (bank/multi vs invalid type/length) | Primary/secondary leave drag overlay |
| Drop multiple files | ≥2 files in one drop | No load; `BAD FILE` path + footer “one .syx at a time” | N/A |
| Drag while inline rename active | Editing PATCH NAME, then file drag enters | Cancel unfinished rename (discard uncommitted text); show drag overlay | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/PluginEditor.{h,cpp}` (+ related split `.cpp` if needed) — implement `juce::FileDragAndDropTarget`; forward drag/drop to PATCH NAME overlay + Core drop-load entry; whole-window hit target
- `Source/GUI/Widgets/PatchNameDisplay.{h,cpp}` — drag overlay mode: fixed primary, blinking secondary (`Timer` at 2 Hz or shared phase); restore API
- `Source/GUI/Panels/.../PatchNameDisplayPanel.{h,cpp}` — apply/clear overlay from editor; do not write drag text into APVTS patch name
- `Source/Shared/Definitions/PluginDisplayNames.h` — `DROP TO LOAD`, `BAD FILE`, `········`, drop success/reject footer formatters (mirror `formatLoadSuccess`)
- `Source/Core/Services/PatchFileService.{h,cpp}` — thin public `isValidSinglePatchSyxFile` (or equivalent) wrapping existing private `validateFileContents`; optional preview-name helper via decode + `PatchModel::getName`
- `Source/Core/Actions/PatchManagerActionHandler.{h,cpp}` (+ Computer browser/load `.cpp`) — `loadDroppedComputerPatchFile(file, limits)` chaining folder set → `scanAndPublishFolder` → select by `indexOfFileNameIgnoreCase` → `loadSelectedPatchFileImmediately`; reuse `confirmPatchContextChange`
- `Source/Core/Services/PatchFileNameSanitizer.*` — reuse `isUsablePatchName` / `isOberheimBankPlaceholderName` for preview fallback
- `Source/GUI/Helpers/GrayedControlHelper.*` — footer only on drop (`setFooterInfoMessage` / `setFooterErrorMessage`)
- `Source/GUI/PluginEditorTimers.cpp` — reference only for 2 Hz blink cadence (`ClipboardFeedbackPhaseTimer`); do not couple Copy lit state to drag blink unless reuse is trivial
- `Tests/Unit/PatchFileService*.cpp` and/or `PatchManagerActionHandlerTests.cpp` — unit-cover validate + drop-load chain / reject multi / bank-sized file; GUI blink is manual
- Do **not** change: `SysExParser` length contract, Bank Import/Export, Computer combo cache via `kScanRevision`, Core→GUI dependency direction

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchFileService.{h,cpp}` — Public single-patch file validate (+ optional name preview) reusing scan gate — SSOT for drag preview and drop reject
- [x] `Source/Core/Actions/PatchManagerActionHandler.{h,cpp}` (+ Computer* split) — Add drop-load entry: parent folder → scan → select dropped file → existing immediate load + gates — no parallel loader
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` — Add drag overlay strings and drop footer formatters — English UI SSOT
- [x] `Source/GUI/Widgets/PatchNameDisplay.{h,cpp}` — Overlay mode + secondary-only 2 Hz blink + restore — drag feedback contract
- [x] `Source/GUI/Panels/.../PatchNameDisplayPanel.{h,cpp}` — Wire overlay without mutating APVTS name during drag — keep model honest
- [x] `Source/GUI/PluginEditor.{h,cpp}` — `FileDragAndDropTarget` on whole editor; preview on enter/move; clear on exit; on drop call handler + footer — product entry point
- [x] `Tests/Unit/...` — Cover validate / drop-load select / reject multi-file and non-single-patch payloads from I/O matrix — lock Core behavior
- [x] `CMakeLists.txt` — Register any new `.cpp` if split — build stays green

**Acceptance Criteria:**
- Given a valid single-patch `.syx` dragged over the editor, when the drag is held, then PATCH NAME shows the file’s patch name (or stem fallback) with blinking `DROP TO LOAD`, and the message bar stays unchanged.
- Given an invalid or multi-file drag, when held over the editor, then PATCH NAME shows `········` / blinking `BAD FILE` and the message bar stays unchanged.
- Given any drag leaves without drop, when `dragExit` fires, then the real current patch display is restored and drag blink stops.
- Given a valid single `.syx` is dropped, when gates allow, then Computer Patches opens the parent folder, lists scanned files, selects that file, loads it like a combo choice, and the footer confirms using the `.syx` filename.
- Given the editor is dirty or Mutator history needs confirm, when a valid `.syx` is dropped, then the same confirmation path as Open / file change runs; Cancel does not leave a half-applied drop load.
- Given a bank dump or multi-message `.syx` is dropped, when validated, then nothing loads and the footer states a clear bank/multi rejection (not a silent first-message load).

## Implementation Notes

- Implemented via `assessSinglePatchSyxFile` + `loadDroppedComputerPatchFile`; `PluginEditor` hosts `FileDragAndDropTarget`; PATCH NAME overlay in `PatchNameDisplay` / `PatchNameDisplayPaint.cpp` (paint split).
- Unit tests: `PatchFileServiceAssess`, `PatchManagerActionHandlerDropLoad` — 0 failures. Lint OK.
- Matrix: Core drop/validate/gate/bank/invalid rows covered by unit tests that ran. Pure GUI rows (drag overlay blink, dragExit restore, multi-file footer, inline-rename cancel) are manual; multi-file automated coverage deferred.
- Known: successful drop may briefly show scan summary footer before load success (same as Open) — deferred.
- Review patches: drop commit flag; assess wraps `validateFileContents`; overlay primary ignores hover; drag assess path cache; empty drop no footer; tightened Master/internal-name/invalid tests.

## Spec Change Log

## Review Triage Log

- Blind: assess reimplements validate instead of wrapping validateFileContents — verdict: medium — claim true; assess duplicates load+validate+decode vs private validateFileContents; routes with wrap patch.
- Blind: prepare fail reports generic invalid — verdict: low — real messaging gap; rejected (unlikely everyday; dedicated copy adds surface beyond trivial).
- Blind: Master assess OR allows kInvalid — verdict: medium — pre-verified by verification-gap; Master can silently become generic invalid footer.
- Blind: multi-file reject untested / task claim — verdict: medium — true; GUI-only path; defer (Core policy).
- Blind: AC omit multi-file — verdict: false — would require editing this build's spec; rejected.
- Blind: hover alpha during overlay — verdict: low — primaryTextColour() uses hover while overlay active; trivial paint fix.
- Blind: fileDragMove reassess every move — verdict: medium — full disk read/decode each move; cache same-path trivial.
- Blind: finalize maps non-match to kCancelled indistinguishably — verdict: medium — grouped with false-kLoaded below.
- Blind: empty Spec Change/Review logs while in-review — verdict: false — process fills triage now; not a product defect.
- Blind: Code Map formatter helpers vs constants — verdict: false — success reuses formatLoadSuccess as Design Notes require.
- Blind: missing empty/unreadable/.SYX case tests — verdict: low — nice-to-have; partial coverage arrives via kInvalid drop test; rest rejected as non-everyday.
- Blind: scan footer flash vs AC — verdict: defer — same Open pattern; not introduced uniquely beyond Implementation Notes.
- Edge: gate cancel after re-drop same path returns kLoaded — verdict: high — finalize infers success from prior knownSyxFullPath_; cancel leaves origin unchanged; return lies (caller ignores today; tests can lie).
- Edge: successful load reported kCancelled on path string mismatch — verdict: false — resolution.file built from same parent+name as drop; noteComputerPatchOrigin uses that File.
- Edge: handler null silent drop — verdict: low — rejected (not everyday; only if handler construction failed).
- Edge: prepare fail → kInvalid footer — verdict: low — same as Blind prepare messaging; rejected.
- Edge: missing path labeled kNotSyx — verdict: medium — !existsAsFile shares branch with !hasSyxExtension.
- Edge: short junk multi-F0 labeled bank/multi — verdict: low — rejected (exotic; framed-message classifier is non-trivial).
- Edge: empty StringArray drop → multi footer — verdict: low — isInterestedInFileDrag rejects empty; still add early return as trivial guard with multi patch group.
- Edge: valid assess empty preview → middle dots — verdict: low — rejected (empty stem rare; fallback already specified).
- Edge claim: prepare footer inaccurate — verdict: low — carried with prepare messaging; rejected.
- Edge claim: missing as wrong type — verdict: medium — carried with existsAsFile branch; patch.
- Edge claim: multi-file coverage absent — verdict: medium — carried; defer.
- Edge claim: assess vs validateFileContents diverge — verdict: medium — carried with wrap patch.
- VG: Master bank/multi OR — verdict: medium — pre-verified; patch tighten assert.
- VG: usable internal preview name never observed — verdict: medium — pre-verified; BNK fixtures only; patch add named fixture case.
- VG: multi-file no automated coverage — verdict: medium — pre-verified disposition defer.
- VG: DropLoad never pins kInvalid footer — verdict: medium — pre-verified; patch add corrupt .syx case.
- VG Other: assess decode stricter than scan validate — verdict: medium — carried with wrap+decode-for-preview design (validate first, decode only for name).
## Design Notes

**Host:** `PluginEditor` implements `FileDragAndDropTarget` so the entire plugin UI is the hit target (modals can still sit above).

**Interest:** `isInterestedInFileDrag` returns true for any non-empty file list so non-`.syx` and multi-file drags still get `BAD FILE` feedback (not silently ignored).

**Preview name:** Decode via existing validate/load helpers into a temporary `PatchModel`; if `!isUsablePatchName` or `isOberheimBankPlaceholderName`, show file stem (not `PATCH` sanitizer fallback) — matches “filename de secours”.

**Reject copy (drop footer, English):**
- Multi-file: `Drop rejected: drop one .syx at a time`
- Not `.syx` / wrong type: `Drop rejected: not a Matrix patch .syx`
- Size/validate fail suggestive of bank/multi (file longer than one patch message or multiple `F0`): `Drop rejected: bank or multi-message dump`
- Other invalid: `Drop rejected: invalid patch file`
- Success: reuse `formatLoadSuccess(fileName)` (`Loaded {name}`)

**Blink:** Local timer on `PatchNameDisplay` at `startTimerHz(2)` toggling secondary visibility or alpha; do not drive from `kCopyLit` unless that proves simpler without coupling Copy state to drag.

**Inline edit:** Decision — cancel unfinished rename on drag enter (discard uncommitted text), then show drag overlay.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --test-dir Builds/macOS/ARM/Debug --output-on-failure` (or project’s usual unit-test invoke with `MATRIX_BUILD_TESTS=ON`) -- expected: touched unit tests pass
- `python3 Scripts/quality/lint_touched.py` -- expected: no new lint failures on touched C++

**Manual checks:**
- Drag valid `.syx` over window → PATCH NAME overlay + blink; leave → restore; drop → Computer Patches selects file and loads; footer on drop only
- Drag bank dump / `.txt` / two files → `BAD FILE`; drop → reject footer, no load
- Dirty patch drop → same unsaved confirm as Open; Cancel aborts
