---
title: 'Combo dismiss on window chrome click'
type: 'bugfix'
created: '2026-10-02'
status: 'done'
route: 'oneshot'
baseline_commit: 'ebf37a7e5fabc768e6fceb8c1207841b353f2330'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-combobox-reclick-to-close.md'
  - '{project-root}/_bmad-output/implementation-artifacts/u-13-combobox-popup-infrastructure-dedup.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** An open ComboBox-family popup dismisses when clicking elsewhere in the plugin GUI, but stays open when clicking the standalone (or host floating) native title bar / window chrome — same gap in VST3/AU.

**Approach:** While a TSS modal popup is open, also dismiss on a left-button press whose screen position is outside the popup, including presses that never reach JUCE components (native title bar / host chrome), without changing in-GUI modal dismiss or re-click-to-close behaviour.

</frozen-after-approval>

## Implementation Notes

- Root cause: combo popups dismiss only via JUCE `inputAttemptWhenModal`, which never runs for native title-bar / host-chrome clicks (no component mouse event).
- Fix: `PopupMenuOutsideDismissWatcher` polls OS left-button state at 60 Hz after `arm()` (called right after `enterModalState`); if the press is outside the popup screen bounds while modal, call the same dismiss path as outside-click (`dismissFromOutsideClick` / HeaderLogo `closePopup`).
- `arm()` waits for the opening button release so the click that opened the menu cannot dismiss it.
- Wired on `PopupMenuBase` (flat Standard/ButtonLike via MultiColumn/Scrollable `show`), `HierarchicalPopupMenu`, and `HeaderLogoPopupMenu`.
- Mac uses `NSEvent pressedMouseButtons` via objc runtime; Windows/Linux use `ComponentPeer::getCurrentModifiersRealtime()`.
- Screen hit-test uses `MouseInputSource::getScreenPosition()` (float), matching `dismissFromOutsideClick`.
- Kept native title bar (`setUsingNativeTitleBar`); did not touch PluginEditor focus helpers.

## Review Triage Log

- Brief-click miss at 30 Hz rising-edge — medium — patched: level-triggered after open-release arm + `kPollHz_ = 60`.
- Watcher races ahead of `inputAttemptWhenModal` — false — message thread; after modal exit JUCE already delivers the click to the underlying control (same as stock modal dismiss).
- Int vs float screen position mismatch — low — patched: `getScreenPosition()`.
- Poll is blunt second outside-click system — false — host chrome is outside peer content; peer-only gate would miss the reported bug.
- Missing oneshot AC/UAT matrix — low rejected — oneshot omits that section; manual smoke covers title bar / chrome.
- Windows `GetAsyncKeyState` noted as required like Mac — low — patched: non-Mac uses realtime modifiers.
- Mac `objc_getClass` fallback weak — low rejected — rare; would need ObjC++/.mm for a stronger path.
- Magic poll rate — low — patched: named `kPollHz_`.
- Timer started before `enterModalState` — medium — patched: `arm()` after modal enter.
- Cross-app outside press dismisses — defer — see deferred-work.md.
- No injectable test seam / automated chrome dismiss tests — defer — see deferred-work.md.
- HeaderLogo uses `closePopup` not `dismissFromOutsideClick` — false — correct for logo teardown (no combo host suppress).
