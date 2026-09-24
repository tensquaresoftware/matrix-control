---
organization: Ten Square Software
project: Matrix-Control
title: Spec — ComboBox Reclick to Close
author: BMad Agent
type: feature
created: '2026-09-24'
status: done
route: oneshot
baseline_commit: '6e576918b49828aa920c389c222f17e7ea2aaf21'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After opening a ComboBox popup (e.g. Header MIDI FROM), Escape and click-outside dismiss it, but a second click on the closed control dismisses then immediately reopens — so the cursor still over the combo cannot cancel without moving or using Escape.

**Approach:** Treat a left-click on the closed control while its popup is open as dismiss-only (no selection change, no reopen), for all TSS ComboBox-family hosts that share this modal popup path (flat Standard/ButtonLike and Hierarchical).

</frozen-after-approval>

## Implementation Notes

- Root cause: modal outside-click dismisses via `inputAttemptWhenModal`, then JUCE delivers `mouseDown` to the host; hosts always called `showPopup()`.
- Fix: `dismissFromOutsideClick` arms `suppressNextPopupOpen` only when a left-click is over the host bounds; host `mouseDown` consumes the flag first (before enablement / button checks) and skips reopen.
- Touched: `IPopupMenuHost`, `PopupMenuModalHelpers`, `PopupMenuBase`, `HierarchicalPopupMenu`, `ComboBox`, `HierarchicalComboBox`, `ComboBoxClosedControlHelper`.
- Skipped `callAsync` auto-clear of the suppress flag — posting clear can race ahead of the re-click `mouseDown` and reopen the menu.
- Header logo popup out of scope (not ComboBox-family).
- Manual verify: open Header MIDI FROM → re-click closed control → stays closed, selection unchanged; Escape / click-elsewhere still dismiss; next intentional click reopens.

## Review Triage Log

- BH: right/middle click arms flag but only left consumes — **medium/patch** — arm only when left button down; consume suppress before button/enablement checks.
- BH: disabled early-return before consume leaves sticky flag — **medium/patch** — same consume-first ordering.
- BH: main-mouse hit-test fragile for multi-pointer/touch — **low/reject** — not a realistic Matrix-Control host path; left+bounds check is enough for desktop plugin use.
- BH: no timeout if flag armed without matching mouseDown — **maybe-false/defer** — with left-only arm + consume-first, sticky path is rare; async clear rejected (can race and reopen). Manual UAT if reported.
- BH: empty Implementation Notes / no AC in oneshot spec — **low/patch** — filled Implementation Notes; oneshot route intentionally omits full AC section.
- BH: one-line arm helper is pointless indirection — **false** — pairs with `consumeSuppressNextPopupOpen` for shared ComboBox / Hierarchical contract.
- BH: no automated popup open/close tests — **defer** — see deferred-work.md.

### Review Findings

- [x] [Review][Patch] Edge re-click can miss suppress when screen position is rounded to int before host bounds check [`Source/GUI/Widgets/PopupMenuModalHelpers.h:43-48`] — fixed: float screen position vs `getScreenBounds().toFloat()`
- [x] [Review][Defer] Sticky suppress if host mouseDown never arrives after arm [`Source/GUI/Widgets/PopupMenuModalHelpers.h` / host mouseDown] — deferred: intentional tradeoff (async clear rejected for reopen race); rare; manual UAT if reported
- [x] [Review][Defer] No automated coverage for ComboBox popup open/close / re-click dismiss — deferred: already in deferred-work.md; reconfirmed by Verification Gap layer

#### Rejected

- BH: suppress only checked in mouseDown / keyboard or programmatic open can leave arm set — **false** — ComboBox-family opens via host mouseDown; Escape and non-host outside click never arm; protocol is consume-on-mouseDown by design
- BH: no clear on notifyPopupClosed / Escape / teardown — **false** as defect — clearing in notifyPopupClosed would wipe the arm before the follow-up mouseDown and reintroduce reopen; Escape never arms
- BH: HierarchicalComboBox mouseDown missing enablement early-return / uneven contract — **false** — consume-first is present; enablement is gated in showPopup/canShowPopupMenu (no sticky from disabled early-return)
- BH: getMainMouseSource / multi-pointer / touch hit-test fragile — **false** — not a realistic Matrix-Control desktop host path (same reject as Build triage)
- BH: full host screen bounds with no inset / padding click suppresses — **false** — host bounds are the closed control; that is the re-click intent
- BH: armSuppressNextPopupOpen is pointless indirection — **false** — pairs with consume helper for shared host contract
- BH: arm/consume living in ComboBoxClosedControlHelper splits ownership — **false** — host one-shot state vs modal dismiss helpers; no demonstrated harm
- BH: nothing asserts selection unchanged on re-click — **false** as separate code defect — dismiss path does not commit selection; assertion gap is the same deferred GUI harness item
- BH: Hierarchical path lacks in-code verify cue vs flat MIDI FROM UAT — **low/reject** — both hosts share the helper; no everyday developer harm worth new comments/harness notes
- ECH: arm when screen point in host bounds but another component is hit — **maybe-false** folded into sticky defer — overlapping non-host at host screen rect is not a normal Header/combo layout; would need sticky repro to act
- ECH: non-main MouseInputSource drives modal dismiss — **false** — same multi-pointer reject as above
- ECH: clear suppress at showPopup entry — **false** as fix for sticky — does not repair a missed mouseDown (consume still eats the open click); clear-on-close would break the feature
- AA: no acceptance findings — n/a
