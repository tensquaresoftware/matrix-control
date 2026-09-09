---
title: 'PATCH NAME display modes contract'
type: 'feature'
created: '2026-09-09'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '0983e4640e2311e98bfe59ac5c047e23225f0463'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/deferred-work.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-name-inline-edit-and-mutator-dual-line.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** PATCH NAME already supports single-line name, inline caret edit, Mutator secondary (`Mxx` / `Mxx-Ryy` / `INITIAL`), and a blinking drag-drop overlay — but there is no clean reusable contract for a “name required” presentation (empty L1 + caret, blinking L2 info). That gap blocks a safe STORE-after-INIT flow without greying STORE or using a footer-only hack.

**Approach:** Refactor `PatchNameDisplay` (+ panel paint consumers) around one explicit presentation contract covering the five product modes, rewire existing Mutator / drag / inline-edit paths onto it without visual regression, and make the name-required mode demonstrable for the later STORE gate.

## Boundaries & Constraints

**Always:**
- Single presentation API on `PatchNameDisplay` (no footer-only name-required; no scattered one-off paint branches outside the contract).
- Keep product height `kPatchNameDisplay` (72 design px); dual-line stays inside that box.
- Reuse existing Matrix 8-char charset / uppercase / illegal-char footer paths.
- UI strings English; new literals only via `PluginDisplayNames`.
- Mode priority while competing: drag overlay (transient) wins over name-required; name-required suppresses Mutator/Compare secondary while active; clearing a higher mode restores the lower one.
- Name-required L2 blinks at the same cadence as drag secondary (2 Hz).
- Existing empty-start double-click rename (clear buffer, caret at 0) remains the normal edit path outside name-required.
- Normal rename empty/all-spaces commit still keeps previous name (`PatchNameEditRules`).
- Name-required L2 copy SSOT: `NAME REQUIRED`.
- This Build ships UI modes + harness demo only; STORE-after-INIT gate is immediate follow-up (remind at Build close; resume Settings Init review / deferred-work).
- Name-required cancel/commit mirrors normal rename UX, with pending-STORE semantics owned by a later consumer:
  - Cancel (Escape / blur / outside) exits name-required, restores the model name display, and fires cancel so a pending STORE is aborted (no store SysEx).
  - Empty/all-spaces Return keeps previous name via `PatchNameEditRules` and counts as unsuccessful for pending STORE (clear mode + abort — never unlock STORE on kept `* INIT *`).
  - Successful Matrix-legal non-empty commit clears mode and signals success (STORE may complete in follow-up).
  - Re-click STORE while a legal name is still required re-arms name-required.
- This Build implements the UI + those outcome signals only; STORE pending-action wiring is follow-up.

**Never:**
- Grey STORE; footer-only “clear name” UX; wide Patch Manager refactor; change Init filenames / App Support Init folder; reopen DELETE / SAVE AS INIT review; French UI/source strings; grow Patch Name module height unless forced and called out.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Idle 1-line | Name set, no secondary, not editing | Centred primary only | N/A |
| Idle Mutator 2-line | History/Compare secondary non-empty | L1 name + stable L2 (no blink) | N/A |
| Inline edit | Double-click (editable) | Empty buffer + blinking caret; Escape cancel; Return/outside commit via edit rules | Illegal char → ignore + footer |
| Drag overlay | Valid/invalid `.syx` drag | Fixed primary + blinking L2; cancels edit; APVTS sync paused | BAD FILE strings unchanged |
| Name required enter | Consumer enables mode | L1 empty + caret; L2 blinking `NAME REQUIRED`; ready to type | N/A |
| Name required cancel | Escape / blur / outside cancel | Exit mode; show model name again; cancel callback (pending STORE abort — follow-up) | N/A |
| Name required empty Return | All spaces / empty + Enter | Keep previous name via edit rules; clear name-required; unsuccessful for pending STORE (abort — follow-up) | Must not unlock STORE on `* INIT *` |
| Name required success | Matrix-legal non-empty + Enter | Commit name; clear name-required; success callback (STORE may proceed — follow-up) | Illegal char → ignore + footer |
| Name required vs Mutator | Both would apply | Name-required presentation wins; Mutator L2 hidden until cleared | N/A |
| Name required vs drag | Drag starts while name-required | Drag overlay wins; after clear, name-required restores if still armed | N/A |
| Regression harness | TestComponent PatchNameDisplay page | Can show each of the five modes without production STORE wiring | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Widgets/PatchNameDisplay.{h,cpp,Paint.cpp}` — presentation contract, layout, caret, drag blink, paint; today flags (`editing_`, `dragOverlayActive_`, `secondaryLabel_`) not a named mode API
- `Source/GUI/Panels/.../Modules/PatchNameDisplayPanel.{h,cpp}` — APVTS sync, `computeSecondaryLabel`, drag apply/clear, edit commit handlers; consumer entry for name-required arm/clear
- `Source/GUI/PluginEditorFileDragDrop.cpp` — drag overlay via panel; must keep winning over other presentations
- `Source/GUI/PluginEditorUiConstruction.cpp` — editability + rename commit wiring
- `Source/Shared/Definitions/PluginDisplayNames.h` (`PatchNameModule`, `DragDropOverlay`) — add name-required L2 SSOT; keep `kInitPatchName` / `INITIAL` / drag strings
- `Source/Core/Services/PatchNameEditRules.{h,cpp}` + `Tests/Unit/PatchNameEditRulesTests.cpp` — normal empty-commit keep-previous; extend only if name-required needs a distinct pure rule
- `Source/GUI/Tests/TestPatchNameDisplays.{h,cpp}` (+ TestComponent page) — demonstrate all five modes
- `Source/Core/Services/PatchNameDisplayMode.h` — Settings musical vs hardware **name source** (unrelated); do not overload for layout modes
- Continuity: `spec-patch-name-inline-edit-and-mutator-dual-line.md` (done); deferred STORE note in `deferred-work.md` (2026-09-09); parent why in `spec-system-init-syx-filenames.md`
- STORE follow-up touchpoints (only if Q3=B): `PatchManagerActionHandlerInternalPatches` / `handleInternalPatchStore`, `refuseSaveIfInitSentinelActive` / `isInitPatchNameSentinel`

## Tasks & Acceptance

**Execution:**
- [x] `PluginDisplayNames.h` -- add name-required L2 (and any tiny related) SSOT constants -- English product copy in one place
- [x] `PatchNameDisplay.{h,cpp,Paint.cpp}` -- introduce clear presentation API covering idle 1-line, edit, Mutator secondary, drag overlay, name-required; keep blink/caret/layout behaviour; no visual regression for existing paths
- [x] `PatchNameDisplayPanel.{h,cpp}` -- map secondary + drag + new name-required arm/clear onto the widget contract; preserve APVTS sync gates
- [x] `PluginEditorFileDragDrop.cpp` / editor bindings -- confirm drag still cancels edit and overlays correctly after API change (minimal touch)
- [x] `PatchNameEditRules` (+ unit tests) -- keep normal empty-commit keep-previous; add only a tiny pure helper if name-required needs “successful vs unsuccessful for pending store” distinction without GUI
- [x] `TestPatchNameDisplays` (+ TestComponent if needed) -- scenario controls or fixed demos for all five modes so name-required is demonstrable without STORE
- [x] Append deferred-work (or Implementation Notes) reminder: wire STORE-after-INIT pending-action + name-required arm/cancel/success next; resume Settings Init review
- [x] Run build preset + `lint_touched.py` on touched C++

**Acceptance Criteria:**
- Given no secondary and not editing, when a patch name is shown, then the widget is single-line centred (mode 1).
- Given Mutator history or Compare, when secondary is set, then L1 is the user name and L2 is stable `Mxx` / `Mxx-Ryy` / `INITIAL` (mode 3).
- Given editable name, when double-click rename runs, then empty L1 + caret edit works as today including Escape / illegal char footer / empty-commit keep-previous (mode 2).
- Given a `.syx` drag, when overlay applies, then existing primary + blinking L2 strings still appear and clear correctly (mode 4).
- Given name-required is armed, when the widget paints, then L1 is empty with blinking caret and L2 shows blinking `NAME REQUIRED` (mode 5), demonstrable in TestComponent/harness.
- Given name-required and Mutator secondary both active, when painted, then name-required presentation wins until cleared.
- Given drag starts during name-required, when overlay clears, then name-required restores if still armed.
- Given name-required cancel, when Escape (or equivalent cancel) fires, then mode clears and cancel is observable for a future pending-STORE abort.
- Given name-required empty Return, when committed, then previous name is kept and outcome is unsuccessful for a future pending STORE (mode clears; no “success” signal).
- When this Build ends, then a clear reminder exists to wire STORE-after-INIT next (deferred-work / notes).

## Implementation Notes

- Presentation priority resolved in paint: drag overlay > name-required > Mutator/Compare secondary > idle.
- Widget API: `armNameRequired` / `clearNameRequired` / `onNameRequiredOutcome(bool success)`; panel mirrors arm/clear + outcome handler for the STORE follow-up.
- Name-required blur/outside-click cancels (unlike normal rename, which commits). Empty Return keeps previous name and reports `success=false`.
- Split input handlers into `PatchNameDisplayInput.cpp` to stay under the ~400-line GUI file budget.
- TestComponent PatchNameDisplay page: mode combo cycles presentations + “name required → drag → restore”; on-screen name-required outcome readout.
- Review patches: drag-safe cancel/clear, arm interrupts normal rename cleanly, panel single outcome forwarder + editable refresh, `resolveNameRequiredCommitEndState` unit-tested.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| BH: `clearNameRequired` no `notifyNameRequiredOutcome(false)` | medium | Verified: clears arm/edit without abort signal. Route **patch** (group A). |
| BH: drag restore wipes `editBuffer_` | false | Name-required contract is empty L1 + caret; restore re-enters that session by design. |
| BH: caret shares 2 Hz L2 timer | low | Real but cosmetic; dual timers add complexity → rejected. |
| BH: “Matrix-legal” vs trim-only predicate | low | Comment oversell; soften wording with commit-helper patch (group B). |
| BH: no automated outcome/commit combo tests | medium | Pre-verified VG gap; route **patch** extract pure commit end-state (group B). |
| BH: harness modes paint-only, no outcome demo | medium | Verified: combo exclusive modes, no outcome label. Route **patch** (group C). |
| BH: Verification Manual checks incomplete | false | Fix would edit this Build’s spec → rejected. |
| BH: Code Map stale / Input.cpp missing | false | Fix would edit this Build’s spec → rejected. |
| BH: empty Spec Change Log / Triage on done | false | Process hygiene; triage filled here; premature `done` corrected to `in-review`. |
| BH: overlapping deferred-work STORE notes | low | Two next-step notes; append clarifying defer (group D). |
| BH: sibling Settings Init Review Findings stale | defer | Pre-existing review pause text; refresh when resuming that review (group D). |
| BH: Drag I/O row says “cancels edit” | false | Fix would edit frozen/spec matrix → rejected. |
| BH: PluginEditorFileDragDrop marked done, no diff | false | Drag still calls panel overlay APIs; widget-level priority handles name-required; no editor change required. |
| BH: no mega-enum “five modes” API | false | Design Notes explicitly prefer arm/clear + existing setters over a mega-enum. |
| BH: double-register outcome lambda | low | Verified duplicate wire in `armNameRequired` + `setNameRequiredOutcomeHandler`. Route **patch** (group A). |
| EC: armNameRequired during normal rename | medium | Verified: `enterNameRequiredEditSession` without ending prior edit → missed `onEditEnded`. Route **patch** (group A). |
| EC: cancelEdit while drag suspended name-required | medium | Verified: `endEditSessionVisuals`/`stopTimer` while overlay still active kills drag blink. Route **patch** (group A). |
| EC: editable not refreshed after name-required exit | medium | Verified: panel skips `setEditable` while armed; cancel/commit don’t `syncFromApvtsState`. Route **patch** (group A). |
| VG: commitEdit gate only via pure helper | medium | Pre-verified; disposition patch. Route **patch** (group B). |
| VG: cancel outcome no automated/harness check | defer | No STORE consumer yet; close when wiring STORE (group D). |
| VG: drag-over-name-required restore not in harness | medium | Pre-verified exclusive modes; route **patch** (group C). |
| VG other: `clearNameRequired` vs cancel signal | medium | Same as BH clearNameRequired; group A. |

## Design Notes

Presentation stack (agent default): **drag overlay > name-required > Mutator/Compare secondary > idle name**. Prefer a small explicit API (e.g. arm/clear name-required + existing secondary/overlay setters) over a mega-enum that forces every caller to restate full state — but paint must resolve one coherent presentation, not ad-hoc special cases.

Do not confuse `Core::PatchNameDisplay` Settings mode (musical vs hardware name **source**) with this widget presentation contract — keep names distinct in code comments/API.

Golden visual: name-required looks like edit’s empty L1 caret row plus drag-like blinking L2, without drag’s fixed primary (`********` / patch name).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- build succeeds
- `ctest --preset macos-debug-arm64 -R PatchName` (or equivalent unit filter) -- edit-rules / new pure tests pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks:**
- TestComponent → PatchNameDisplay: cycle idle, Mutator-like secondary, edit, drag overlay strings, name-required
- Standalone smoke: Mutator secondary + `.syx` drag overlay unchanged vs pre-change behaviour
