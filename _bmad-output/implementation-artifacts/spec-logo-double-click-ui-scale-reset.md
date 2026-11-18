---
title: 'Restore logo double-click UI Scale reset'
type: 'bugfix'
created: '2026-11-18'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-logo-popup-vst3.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Double-clicking the header logo no longer resets UI Scale to 100%. The VST3 logo-menu open fix replaced the post-mouseUp delay with an immediate async open, so the menu races ahead of the double-click reset.

**Approach:** Restore a short post-mouseUp delay that still opens after the click is fully released (keeps Ableton VST3 safe), so a real double-click can cancel the pending menu open and reset UI Scale to 100% again.

</frozen-after-approval>

## Implementation Notes

- Root cause: commit `2d6b67f9` removed `Logo` `Timer(200)` and opened via `callAsync` on first `mouseUp`; that tick usually fires before the second click, so cancel cannot prevent the menu.
- Pre-fix path: `mouseUp` → `startTimer(200)`; `mouseDoubleClick` → `stopTimer()` + `onUiScaleReset`; `timerCallback` → `onPopupRequested`.
- Changed only `Logo.h` / `Logo.cpp`. Left `HeaderPanel` wiring, `HeaderLogoPopupMenu`, and `PopupMenuOutsideDismissWatcher` alone (dismiss-before-reopen stays).
- Restored `juce::Timer` on `Logo`. Kept the VST3-era `mouseWasClicked()` gate. Open delay uses `juce::MouseEvent::getDoubleClickTimeout()` (not hard-coded 200) so a slow-but-valid double-click still cancels before open.
- Review patches: `stopTimer()` on Alt no-op and on `getNumberOfClicks() > 1` early return (do not leave a pending open armed).
- Lint: `Scripts/quality/lint_touched.py` OK. Build: `macos-debug-arm64` `Matrix-Control_Standalone` green.
- Manual smoke: single-click opens menu after the delay; double-click within the JUCE timeout resets to 100% without menu; Ableton VST3 logo menu still opens and stays (reload Debug VST3, not a stale install).

## Review Triage Log

- 200 ms shorter than JUCE double-click timeout (400 ms) — medium / patched: delay now `MouseEvent::getDoubleClickTimeout()`.
- Alt early-return does not `stopTimer()` — medium / patched.
- `getNumberOfClicks() > 1` skips `stopTimer()` — medium / patched.
- No left-button guard on open arming — deferred (pre-existing).
- `!mouseWasClicked()` leaves prior timer running — false / rejected: this path never starts a timer; a prior armed timer is cancelled by double-click / modifiers / multi-click.
- Spec under-documents kept `mouseWasClicked()` gate — low / rejected: recorded in Implementation Notes instead.
- Sibling `spec-logo-popup-vst3.md` still describes `callAsync` open — deferred (done-spec hygiene).
- Manual smoke missing slow double-click / Ableton gate — low / rejected: Implementation Notes smoke list updated.
