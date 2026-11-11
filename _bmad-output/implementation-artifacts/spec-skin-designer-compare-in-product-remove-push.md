---
title: 'Skin Designer: move Compare beside Product Duplicate; remove Push button'
type: 'feature'
created: '2026-11-11'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Compare (Product vs Designer preview on Matrix-Control) lives in SESSION, far from the Product selector, so the gesture is hard to discover. The Push button is almost never needed because auto-push already covers connect, combo changes, edits, and load/import.

**Approach:** Move Compare to the Skins Product row immediately to the right of Duplicate (same blink/toggle/read-only semantics). Remove the Push button from chrome entirely; keep automatic `pushVisible()` and the LAST PUSH status badge. No new focus mechanism and no Push replacement/recovery UI in this change. SESSION ends with Undo/Redo after JSON Sync.

</frozen-after-approval>

## Implementation Notes

- Owned Compare in `SkinDesignerSkinsPanel` (callback `onCompare` + `compareButton()` for blink chrome); wired from `setupSkinsPanel()`.
- Removed SESSION `pushButton_` / `onPush()` / `sessionPushButtonWidth`; SESSION layout ends Undo/Redo; `sessionWellPreferredWidth` updated.
- Kept `pushVisible()`, LAST PUSH badge, Compare Product edit-lock + blink unchanged.
- Review patches: Compare tooltip; class comment; layout unit test for Duplicate+Compare row.
- Skipped Blind Hunter suggestions: SkinsPanel blink encapsulation API; rewriting historical done SESSION-order specs (deferred); in-app “push is automatic” cue (explicit non-goal).

## Review Triage Log

- Missing Compare tooltip after move — medium — patched (`setTooltip` on Compare).
- No layout test for Duplicate+Compare Product row — medium — patched (`skinsPanelProductDuplicateCompareFitOneRow`).
- Historical specs still document SESSION Compare/Push — medium — deferred (done artifacts; not this change’s chrome).
- Product row may clip Compare under tighter width — false — `kColorPanelWidth` layout + new unit test keep both buttons fully visible.
- Expose blink chrome via SkinsPanel API instead of `compareButton()` — low — rejected (extra API for no user gain).
- SkinsPanel header omits Compare — low — patched (class comment).
- Planning note said wire in Skins.cpp vs setupSkinsPanel — false — wiring in `setupSkinsPanel()` is correct and matches other Skins callbacks.
- Spec still in-progress / no checklist — false — process; finalized `done` after implement+review.
- No residual cue after removing Push — false — intentional; user deferred any recovery UI.
