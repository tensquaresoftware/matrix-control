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

**Approach:** On Mappings `refresh()` (filter apply/clear and other rebuilds), keep the same object under the view top by restoring scroll from that object's section Y plus the prior offset (absolute Y alone jumps when filters rewrite groups above). Do not call Object-combo scroll-into-place on filter changes.

</frozen-after-approval>

## Implementation Notes

- First pass (absolute view-Y, Palette-style) failed smoke: filters that remove/add object groups above the watched table still moved it out of view.
- Second pass: `captureViewScrollAnchor` / `restoreViewScrollAnchor` pin to the object at the view top (`sectionY + offset`), with absolute Y fallback when that object disappears from the filtered set.
- Object combo remains the only intentional jump-to-table (`scrollFocusedObjectIntoPlace`).
- Touched: `SkinEditorMappingsPanel.cpp` / `.h`.
- Manual GUI: select Object or scroll a table into view → apply/clear Property or Mapped colour filter → the same table should stay under the sticky legend; empty “no matches” may clamp to 0.

## Review Triage Log

- Blind: absolute Y can lose the watched table when filters rewrite groups above it — **medium** → defer (out of frozen absolute-Y Approach; see deferred-work)
- Blind: Problem vs Approach tradeoff not recorded in frozen block — **false** (Approach deliberately chooses absolute Y / no Object re-pin)
- Blind: Implementation Notes implied filter path called scrollFocusedObjectIntoPlace — **low** → patch (notes corrected)
- Blind: no acceptance / manual checks in oneshot body — **low** → patch (manual GUI line added to Implementation Notes)
- Blind: status still in-progress while notes claim finish — **false** (finalize sets done)
- Blind: Mappings forces X=0 vs Palette keeps X — **false** (Mappings viewport is vertical-only; intentional)
- Blind: focused Object chrome can stay off-screen after filter — **medium** → defer (same root as absolute-Y residual; grouped)
- Blind: no automated verification for scroll-after-filter — **low** → reject (manual GUI noted; Component scroll harness not justified for this oneshot)
