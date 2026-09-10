---
title: 'Direct slider value entry'
type: 'feature'
created: '2026-09-11'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'ff479a26d12fc3ccd8e6f18d55fdbdd4635f06f5'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Sliders only change by dragging or by resetting. Users cannot type a precise value the way they already can on the Internal Patches CurrentPatchNumber NumberBox. The product needs one clear grammar for “enter a value” versus “reset to default.”

**Approach:** Double-click a slider to type a value (same gesture as CurrentPatchNumber). Cmd (macOS) / Ctrl (Windows, Linux) + click resets to the descriptor default. NumberBox behavior and red edit chrome stay unchanged. While a slider is editing: hide value text and the value bar / track fill so only the full dark-green slider background remains; show a centered blinking white caret and white bold typed digits at the **same font size** as the normal value text (bold + white only — no size change); Enter commits the edit and restores normal green value painting; Escape or focus-lost / click elsewhere cancels like NumberBox. Remove the idle Return-key reset (Return must not reset when not editing; while editing, Enter still commits). Disable JUCE `setDoubleClickReturnValue` on attached sliders. Cmd/Ctrl+click must reset without starting a value drag.

## Boundaries & Constraints

**Always:**
- Double-click = open inline numeric entry on enabled sliders (gesture-aligned with CurrentPatchNumber).
- Cmd/Ctrl+click = reset to descriptor default (only reset gesture on sliders; undoable like today’s reset).
- Edit chrome: full dark-green background only (no value text, no value bar / track fill); white blinking caret; white bold digits at the same font size as idle value text (no size change); never NumberBox red.
- While editing, Enter commits (snap to legal range/step); Escape or focus-lost / click elsewhere cancels and restores the previous value.
- When not editing, Return/Enter must not reset (that idle reset is removed).
- Empty or illegal Enter keeps the previous value and closes the editor (NumberBox spirit).
- Preserve undoable APVTS notifications for commit and reset.
- English-only strings; prefer existing `SliderLook` colours (map edit text/caret if missing).

**Never:**
- Do not use NumberBox-red edit fill on sliders.
- Do not change CurrentPatchNumber / NumberBox gestures or chrome.
- Do not use Return/Enter as a reset gesture (idle reset removed; Enter only commits while editing).
- Do not leave JUCE double-click-return enabled on parameter-attached sliders.
- Do not start a drag from a Cmd/Ctrl+click reset.
- Do not make parameter labels clickable; do not add ComboBox reset in this ship.
- Do not invent rotary widgets; do not change MIDI / SysEx semantics.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Open editor | Enabled slider; double-click | Value text + value bar hidden; full dark-green plate; white caret centered | N/A |
| Commit | Editor open; legal number; Enter | Snapped value applied; normal green paint restored; notify like drag end | N/A |
| Cancel | Editor open; Escape or focus lost / click elsewhere | Previous value kept; normal paint restored | N/A |
| Illegal / empty Enter | Editor open; empty or non-numeric | Previous value kept; editor closes | Soft ignore |
| Reset | Enabled; Cmd/Ctrl+click; not editing | Value → descriptor default; no drag session | N/A |
| Reset while editing | Editor open; Cmd/Ctrl+click | Cancel edit (restore previous), then reset that slider if still the target | Consistent cancel-then-reset |
| Plain click while editing | Click elsewhere or on self without Cmd/Ctrl | Cancel edit; do not begin drag on the canceling click | Prefer cancel-only |
| Disabled | Disabled; double-click or Cmd/Ctrl+click | No change | N/A |
| Return idle | Focused slider; not editing; Return | No reset (gesture removed) | N/A |

**Decisions locked:**
- Grammar: double-click edit; Cmd/Ctrl+click reset.
- Scope: sliders only for new behavior; NumberBox unchanged.
- Edit plate: dark-green full background; hide value text and value bar/track fill; white caret + white bold digits at same font size as idle text.
- Idle Return reset removed (Enter commits only while editing); disable attachment double-click-return; Cmd/Ctrl+click must not drag.

</frozen-after-approval>

## Code Map

- `Source/GUI/Widgets/Slider.cpp` / `Slider.h` — paint track / value bar / text; `mouseDoubleClick` reset; Return reset in `keyPressed`; focus border; add edit session + Cmd/Ctrl+click reset; suppress drag on modifier click
- `Source/GUI/Widgets/NumberBox.cpp` / `NumberBox.h` — reference for Enter/Escape/focus-lost and clear-to-type; do not copy red fill
- `Source/GUI/Helpers/ApvtsUndoableParameterAttachments.cpp` — disable `setDoubleClickReturnValue` (or equivalent) so attachments do not reset on double-click
- `Source/GUI/Looks/WidgetLooks.h` / `LookBuilders.cpp` — `SliderLook` track/value/text/focusBorder; add edit text/caret if needed
- `Source/GUI/Factories/WidgetFactory.cpp` — descriptor `defaultValue` for reset target
- `Source/GUI/Widgets/ParameterCell.cpp` — labels remain passive
- Prior: `_bmad-output/implementation-artifacts/spec-numberbox-edit-border-and-font.md` — editor bold/white digit lessons only

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Widgets/Slider.{h,cpp}` -- Inline editor (dark-green plate, white caret/digits); double-click open; Cmd/Ctrl+click reset without drag; remove Return reset; Enter/Escape/focus-lost -- core UX
- [x] `Source/GUI/Looks/WidgetLooks.h` / `LookBuilders.cpp` -- Map slider edit text/caret colours if not already present -- skin consistency
- [x] `Source/GUI/Helpers/ApvtsUndoableParameterAttachments.cpp` -- Disable JUCE double-click-return on attached sliders -- prevent reset under editor
- [x] `Tests/` -- Cover open/commit/cancel/illegal/Cmd-Ctrl reset / no Return reset from the I/O matrix -- regression
- [ ] Manual UAT -- Patch Edit, Matrix Modulation amount, Header gain; CurrentPatchNumber still red-edits -- homogeneity check

**Acceptance Criteria:**
- Given an enabled slider, when the user double-clicks, then value text and value bar disappear, the full dark-green background remains, and a white caret is ready for input with typed digits in white bold at the same font size as idle value text.
- Given an open slider editor, when the user types a legal number and presses Enter, then the snapped value is applied and normal green slider painting returns.
- Given an open slider editor, when the user presses Escape or clicks elsewhere, then the previous value remains and edit mode ends.
- Given an enabled slider not being edited, when the user Cmd/Ctrl+clicks, then the value resets to the descriptor default and no drag starts.
- Given a focused slider not being edited, when the user presses Return, then the value does not reset.
- Given CurrentPatchNumber, when the user double-clicks, then existing red NumberBox edit behavior is unchanged.

## Implementation Notes

- Split editor session into `Source/GUI/Widgets/SliderEditing.cpp` to keep useful-line budgets under the quality gate.
- `SliderLook::editorText` / `editorCaret` reuse `SkinColourId::kNumberBoxEditorText` (white) — no new slider skin tokens.
- Widget ctor and `ApvtsUndoableSliderAttachment` both call `setDoubleClickReturnValue(false, …)`.
- `resetToDefaultValue()` uses `ScopedDragNotification` so Cmd/Ctrl+click reset is one undoable APVTS gesture (same pattern as Enter commit).
- Unit coverage: `Tests/Unit/SliderValueEntryTests.cpp` (gesture matrix) + `Tests/Unit/SliderValueEntryApvtsTests.cpp` (APVTS/undo) + `Tests/Unit/SliderValueEntryTestSupport.h`.
- Headless runs assert on `grabKeyboardFocus` without a desktop peer (same pattern as existing slider arrow tests); assertions do not fail the runner.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| Plain click on open editor does not cancel (Blind/Edge) | false | Frozen Approach cancels like NumberBox via Escape / focus-lost / click elsewhere. Cancelling on `SliderEditField::mouseDown` would close the editor on the first field click and break typing. `Slider::mouseDown` still cancel-without-drag if the parent receives the event. |
| Double-click opens empty drag/undo gestures before editor (Blind) | medium (defer) | Pre-existing: `mouseDown` always starts `ScopedDragNotification`; attachment begins an editorial transaction even when value does not change. Not introduced by the editor itself. |
| Enter commit / Cmd reset lack APVTS+undo asserts (Blind + Verification Gap) | medium | Widget-only tests; `dontSendNotification` or dropping the drag session would still pass. Spec requires undoable APVTS notifications. |
| Out-of-range Enter snap untested (Blind) | low | `handleEditorReturn` snaps via `jlimit`/`snapToLegalValue`; no test commits above max. |
| Bipolar / negative typed entry untested (Blind + Verification Gap) | medium | All harnesses use DCO1 Frequency (min 0); `editorAllowedCharacters` minus branch never exercised. |
| Open-editor chrome / clear-to-type / font size untested (Blind) | low | Covered by unchecked Manual UAT task; unit tests cannot assert paint reliably. |
| `enablementChanged` mid-edit close untested (Blind + Verification Gap) | medium | Code closes editor on disable; `testDisabledIgnoresGestures` disables before open only. |
| Attachment test does not prove double-click opens without parameter reset (Blind) | low | Only asserts `isDoubleClickReturnEnabled()==false`; overlaps undo/commit gap. |
| `formatValue_` / display text vs typed raw numbers (Blind) | false | Spec asks for numeric entry; display formatters remain paint-only. |
| Focus not restored to Slider after Escape/focus-lost (Blind) | false | NumberBox `hideEditor` also does not `grabKeyboardFocus` on the parent. |
| Missing descriptor `jassert` then dereference in tests (Edge) | false | Same fixture pattern as `UndoManagerSliderArrowTests`; descriptor is always present in this tree. |
| Edit colours from skin untested (Verification Gap) | low (defer) | Spec already scopes chrome to Manual UAT; optional LookBuilders assert is follow-up. |

## Design Notes

**Gesture vs colour:** Double-click means “type a value” on Slider and NumberBox. Edit chrome keeps each control’s identity (NumberBox red vs slider dark-green plate).

**Contrast:** Editing shows only the full dark-green background under white caret/digits — no partial value bar under the text.

**Attachment:** Both widget-level double-click reset and JUCE `setDoubleClickReturnValue` must be removed/disabled so double-click only opens the editor.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- plugin and tests build
- Targeted tests for touched widgets -- pass
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++

**Manual checks (if no CLI):**
- Standalone: mid/low/high values, commit, Escape, click-away, Cmd/Ctrl+click reset, confirm Return idle does nothing; NumberBox still red-edits.
