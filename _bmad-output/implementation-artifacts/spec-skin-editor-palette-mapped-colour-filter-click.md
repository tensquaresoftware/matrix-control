---
title: 'Skin Editor Palette count click → Mapped colour filter'
type: 'feature'
created: '2026-10-31'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-palette-mapping-counts.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-mappings-column-filters.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Operators who see a mapping count on a Palette colour cannot jump from that digit to the Mappings rows that use it — they must open Mapped colour and pick the name by hand.

**Approach:** Clicking a non-zero mapping count on a Palette colour row activates the Mappings Mapped colour column filter for that pastille name. Clicking a count on another colour updates the same filter to the new name. Leave other column filters (Property / State) unchanged. Do not enter mapping Hold/Arm/bind from a count click.

</frozen-after-approval>

## Implementation Notes

- Agent decisions: count-column hit only (non-zero digits); updates Mapped colour filter only (Property/State unchanged); no Hold/Arm/bind via `onPastilleSelected`; no toggle-clear on re-click; no focus steal.
- Public `SkinEditorMappingsPanel::setMappedColourFilter` wraps private `setColumnFilter(kMappedColour, …)`; same-value set is a no-op (avoids clearing Mappings row highlight).
- Palette: `onMappingCountClicked`; geometry SSOT in `SkinEditorPalettePanelGeometry.h`; clicks in `SkinEditorPalettePanelClicks.cpp`.
- Wire: `SkinEditorMainComponent` → `mappingsPanel_.setMappedColourFilter(name)`.
- Build `Matrix-Control_SkinEditor` OK; `lint_touched.py` OK.

## Review Triage Log

- Blind: missing row component falls through to Hold/Arm — **medium / patch**: list-relative hit + fallback without requiring realized row component.
- Blind: same-filter re-click always `refresh()` clears Mappings selection — **medium / patch**: `setColumnFilter` no-op when value unchanged.
- Blind: no scroll-to-first-match / Object combo jump — **false**: frozen Approach is filter activation only, not scroll navigation.
- Blind: duplicated paint/hit geometry constants across TUs — **medium / patch**: `SkinEditorPalettePanelGeometry.h`.
- Blind: ListBox selection + Dup/Del without mapping-mode sync — **low / rejected**: intentional (filter without Hold/Arm); name/swatch click still arms normally.
- Blind: whole count column clickable, no cursor/tooltip — **low / rejected**: matches existing digit affordance; cursor polish not requested.
- Blind: thin oneshot spec / AND empty-table case undocumented — **false**: oneshot omits Boundaries/AC by route; AND with Property/State is existing filter semantics.

