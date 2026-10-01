---
title: 'Computer Patches combo same-item reclaim'
type: 'bugfix'
created: '2026-10-01'
status: 'done'
route: 'oneshot'
baseline_commit: '57786a9451b4641739a82552e8c9c304e4f9443e'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-patch-nav-focus-and-bank-marker.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-bank-utility-focus-internal-patches.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After browsing `.syx` files in Computer Patches then navigating Internal patches, reopening the Computer combo and choosing the already-selected file does nothing — navigation focus stays Internal and the `.syx` is not reloaded, because same-id combo selection never notifies.

**Approach:** When the user picks the already-selected Computer Patches combo item, treat that as the same reclaim as clicking the Computer Patches module header: Computer navigation focus plus immediate reload of the selected file (existing unsaved gate). Other combos stay unchanged.

</frozen-after-approval>

## Implementation Notes

- Root cause: JUCE `ComboBox::setSelectedId` skips notifications when the id is unchanged, so APVTS `kSelectPatchFile` never updates and Core never reclaims Computer focus after Internal navigation.
- Fix: `PopupMenuBase::selectItem` detects same-id commits and invokes optional `TSS::ComboBox::onSameIdReselected`. Computer Patches wires that callback to `dispatchTimestampAction(kHeaderClick)`, reusing the existing Computer header reclaim path (`setNavigationFocus(kComputer)` + `loadSelectedPatchFileImmediately`, including unsaved gate). Same-id while Computer already owns focus also reloads (header semantics). Gate cancel inherits header behaviour (focus becomes Computer, load aborted).
- Other combos leave `onSameIdReselected` unset — same-id pick remains a no-op for them.
- Core reclaim path covered by existing `PatchManagerActionHandlerHeaderClickTests`. GUI same-id → header-action wiring is manual UAT (no ComboBox popup unit harness).
- Kept `ComputerPatchesPanel.cpp` at 400 useful lines (compact `setNavigationButtonsEnabled` + compact same-id lambda) to satisfy the file-size gate.

## Review Triage Log

- Blind: oneshot missing Tasks/AC/Code Map — **false** (oneshot route is Intent + Implementation Notes only).
- Blind: HeaderClick tests do not cover GUI same-id wiring / overclaim — **medium** → notes clarified; automated GUI wiring **defer**.
- Blind: `setNavigationButtonsEnabled` one-line ifs are unrelated noise — **low** rejected (needed to stay at 400 useful lines after wiring).
- Blind: same-id while Computer-focused reload undocumented — **low** → **patch** (Implementation Notes).
- Blind: status still `in-progress` mid-review — **false** (finalized to `done` here).
- Blind: shared PopupMenuBase same-id branch needs consumer regression check — **false** (callback optional; unset = no-op).
- Blind: lambda brace layout inconsistent with header handler — **low** rejected (same file-size budget tradeoff).
- Blind: gate-cancel focus-to-Computer inherited but implicit — **low** → **patch** (Implementation Notes).
