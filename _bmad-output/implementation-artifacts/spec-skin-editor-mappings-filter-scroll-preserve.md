---
title: 'Skin Editor Mappings filter scroll preserve'
type: 'bugfix'
created: '2026-11-01'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In Skin Editor Mappings, after the operator selects an Object or scrolls to keep a mapping table in view, changing or clearing a column filter often jumps the list scroll so the watched table leaves the viewport.

**Approach:** On Mappings `refresh()` (filter apply/clear and other rebuilds), preserve the viewport vertical position the same way Palette already does — restore prior view Y after layout, clamped to the new content height — without auto-scrolling to the focused object on filter changes.

</frozen-after-approval>

## Implementation Notes

- Mirrored Palette absolute view-Y preserve in `SkinEditorMappingsPanel::refresh()`: capture `viewport_.getViewPositionY()` before rebuild; after `layoutContent()`, restore with clamp to new max Y. Failure mode was viewport clamp on content resize (not `scrollFocusedObjectIntoPlace` on filter paths — that was never called from `refresh()`).
- Object combo remains the only intentional jump-to-table.
- Touched: `Tools/SkinEditor/SkinEditorMappingsPanel.cpp` only.
- Manual GUI: select Object or scroll a table into view → apply/clear Property or Mapped colour filter → scroll Y should stay put (or clamp only if content shrinks past prior Y); empty “no matches” may clamp to 0.

## Review Triage Log

- Blind: absolute Y can lose the watched table when filters rewrite groups above it — **medium** → defer (out of frozen absolute-Y Approach; see deferred-work)
- Blind: Problem vs Approach tradeoff not recorded in frozen block — **false** (Approach deliberately chooses absolute Y / no Object re-pin)
- Blind: Implementation Notes implied filter path called scrollFocusedObjectIntoPlace — **low** → patch (notes corrected)
- Blind: no acceptance / manual checks in oneshot body — **low** → patch (manual GUI line added to Implementation Notes)
- Blind: status still in-progress while notes claim finish — **false** (finalize sets done)
- Blind: Mappings forces X=0 vs Palette keeps X — **false** (Mappings viewport is vertical-only; intentional)
- Blind: focused Object chrome can stay off-screen after filter — **medium** → defer (same root as absolute-Y residual; grouped)
- Blind: no automated verification for scroll-after-filter — **low** → reject (manual GUI noted; Component scroll harness not justified for this oneshot)
