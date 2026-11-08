---
title: 'Skin Editor Mappings global filter reset'
type: 'feature'
created: '2026-11-01'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-mappings-column-filters.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Operators who set several Mappings column filters must clear each amber X separately; there is no one-click way to drop every active filter.

**Approach:** Add a global clear control on the Mappings action row: same amber square + X as the per-column clear badges, placed to the right of the Unbound count witness, with its right edge flush to the sticky legend / dark header right edge (`tableX + tableW`). Clicking it clears every active column filter and refreshes the tables. The control appears only when at least one column filter is active (same show/hide pattern as per-column clears).

**Decisions:**
- VISIBILITY: show only when any Property / State / Set / Mapped colour filter is non-empty.
- LOOK: reuse `SkinEditorChrome::drawClearBadge` — no new colour or badge size.
- ALIGN: clear-all badge right edge = sticky legend right edge; leave a small gap after the Unbound count badge so the two never overlap.
- SCOPE: does not change Unbound Export-gate semantics, bind/Hold, or filter persistence rules (skin-id change still clears filters as today).

</frozen-after-approval>

## Implementation Notes

- Reuse `clearAllColumnFilters()` then `refresh()` for the click path.
- Layout in `layoutActionRow`; paint in panel `paint`; hit-test in panel `mouseUp` (same paint-only pattern as legend clears).
- No tooltip (per-column clears have none).
- Added `anyColumnFilterActive()`; `globalClearBounds_` right-aligned to `tableX + tableW`, shown only when a filter is active and there is `kButtonGap` after the Unbound count badge.
- Files: `SkinEditorMappingsPanel.h` / `.cpp` / `SkinEditorMappingsPanelFilters.cpp`.
- Lint LIGHT: both panel cpp files already near / over the 400-line useful-size band; this change stays within that existing pressure (no new split this story).

## Review Triage Log

- Blind: tight layout can omit global clear while filters stay active — **low** → reject (mapped column width keeps Unbound far left of the table right in normal Skin Editor sizes; per-column clears remain)
- Blind: amber X ambiguous next to Unbound / no tooltip — **false** (frozen LOOK + same paint-only pattern as per-column clears)
- Blind: `clearAllColumnFilters` lacks refresh vs `clearColumnFilter` — **low** → reject (pre-existing API; `setActiveSkinId` already relies on caller refresh; click path pairs clear + refresh)
- Blind: no automated coverage for global clear — **defer** (GUI hit/layout; see deferred-work)
- Blind: four filter members duplicated in clear/any helpers — **false** (mirrors existing `clearAllColumnFilters` list)
- Blind: `setUnboundCount` skips relayout — **low** → reject (pre-existing; clear sits at table right so digit-width growth does not move it)
- Blind: header comment still says sticky-legend-only filters — **low** → reject (comment polish only)
- Blind: no menu/keyboard clear-all — **false** (out of frozen intent)
