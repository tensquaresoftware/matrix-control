---
title: 'Fix logo popup missing in VST3'
type: 'bugfix'
created: '2026-11-17'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '86855e71cbd01cc929fa0d189c084b3201523745'
context:
  - '{project-root}/Source/GUI/Widgets/HeaderLogoPopupMenu.cpp'
  - '{project-root}/Source/GUI/Widgets/PopupMenuOutsideDismissWatcher.cpp'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-product-tones-skins-menu.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Clicking the header logo opens the Skin / UI Scale menu in Standalone, but the same click in VST3 never shows the menu.

**Approach:** Keep Standalone behaviour, and make the logo menu open and stay visible under embedded VST3 hosting by fixing the post-TONES flyout layout/dismiss interaction that only hurts the delayed logo-open path in plugin peers.

</frozen-after-approval>

## Implementation Notes

- Root cause: post-TONES `layoutFamilyFlyout` clamped the **full expanded** host into the parent. A tall TONES flyout set Y to 0, sliding the menu under the logo cursor in short VST3 editors; combined with delayed post-mouseup open + outside-dismiss watcher, the menu could vanish immediately.
- Shipped: clamp X/Y using **main panel** size only (flyout may clip); 250 ms post-`arm` dismiss grace; defer discoverability flyout one message tick (skip if user already opened a family or menu is no longer modal); parent `repaint` after show.
- Review patches: skip async flyout if `familyFlyout_` already set; main-only X clamp; grace keeps `previousLeftDown_` false while held so post-grace outside hold still dismisses; comment wording.
- Build: `windows-debug` `Matrix-Control_VST3` + `Matrix-Control_Standalone` green; `lint_touched.py` OK. Tests target not enabled in this preset (`MATRIX_BUILD_TESTS=OFF`).
- Manual gate: reload the Debug VST3 in the DAW (not a stale install), click logo with a TONES look active, confirm menu stays open; Standalone non-regression.
- Follow-up (still missing in DAW after first ship): Logo no longer uses a 200 ms Timer after mouseUp. Open path now matches ComboBox — `mouseDown` + `MessageManager::callAsync` — so `PopupMenuOutsideDismissWatcher` can arm with `waitingForOpenRelease_` while the press may still be down. Opaque host + full-bounds background fill; `toFront(true)`; dismiss-before-reopen in `showLogoPopup`.

## Review Triage Log

- Async discoverability flyout overrides early hover family — medium / patched: skip when `familyFlyout_ != nullptr` or not modal.
- X clamp still used full expanded width — medium / patched: clamp with `mainPopupWidth_`.
- `closeFamilyFlyout` leaves shifted X — false after main-only X clamp (main position no longer shoved for flyout width).
- Standalone timing/grace must be VST3-gated — false: Intent keeps Standalone working; shared settle path is acceptable.
- Grace comment tied to logo 200 ms delay — low / patched: comment now says post-show settle.
- Outside press held through grace never dismisses — medium / patched: do not set `previousLeftDown_` true during grace while down.
- No automated clamp/grace/mount tests — deferred (GUI mount pattern).
- Spec still planned/in-progress — false for code; finalized here.
- Tall TONES clip + keyboard fallback restatement — deferred (existing TONES V1 accept).
- Async lambda missing modal check — low / patched: `isCurrentlyModal()` guard.
