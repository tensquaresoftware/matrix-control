---
title: 'Skin Editor Mappings Unbound count badge'
type: 'feature'
created: '2026-10-30'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '5e6dd3fcabf669c5db9247638bcb9d219e630192'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-chrome-session-skins.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After the dedicated Unbound panel was removed, operators no longer see how many keys remain unbound while mapping; yellow "Unbound" cells alone do not give a total, so Export's Unbound = 0 gate stays opaque until they try Export.

**Approach:** In the Mappings header, keep Hold colour in place, shorten the Object combo as needed, and show the word "Unbound" (same amber as table cells, left edge aligned with the Mapped colour column's Unbound text) followed by a filled count badge (amber fill, panel-surface glyph, panel-style rounded corners) with `Layer::unboundCount` for the active Editor skin.

</frozen-after-approval>

## Implementation Notes

Agent decisions (planning):
- Hold colour stays in Mappings (user choice); Unbound label + badge occupy mapped-column alignment — Hold moves left of that column so Object combo ends before Hold.
- Restyle orphan `drawCountBadge` to attention fill + `surfacePanel` text; use `kControlCorner` (panel corners) instead of capsule pill radius.
- Count = whole-skin `unboundCount` (matches Export gate), including when Object filter is narrowed; show when shell filled even if count is 0.
- Do not revive Unbound panel; do not move Hold into SESSION.

Agent decisions (oneshot):
- Main feeds `setUnboundCount(layer.unboundCount(editorSkinId))` so Compare Product still shows the Editor total (empty id → 0).
- `refresh()` calls panel `repaint()` as well as `content_.repaint()` because the witness is painted on the panel chrome.
- Moved bind/focus helpers into `SkinEditorMappingsPanelRows.cpp` to stay under the useful-line gate after adding the witness.
- Touched: `SkinEditorMappingsPanel.{h,cpp}`, `SkinEditorMappingsPanelRows.cpp`, `SkinEditorChromeTable.{h,cpp}`, `SkinEditorMainComponentSession.cpp`.

## Review Triage Log

- Blind: panel paint stale after refresh (only content repaint) — **high**, patched (`repaint()` on panel).
- Blind: Compare Product badge used panel/product skin count vs Editor Export gate — **medium**, patched (`setUnboundCount` from editorSkinId).
- Blind: drawCountBadge width/clamp DRY still deferred — **low** → defer (pre-existing orphan cleanup; tracked again).
- Blind: bind/focus move to Rows.cpp widens review surface — **false** (required for file-size gate; behaviour unchanged).
- Blind: no Export-gate tooltip on Unbound witness — **low** → defer (mock has no tooltip; coaching optional).
- Blind: spec still in-progress / docs call badge orphan — **false** (finalize marks done; docs are historical).
- Blind: drawCountBadge now amber but named generic — **false** (single caller; colours match Unbound intent).
- Blind: kControlCorner vs kChromeBadgeCorner optical match — **false** (user asked panel-style corners).
- Blind: MappingsPanel.cpp still above soft total lines — **false** (useful lines 377; gate OK).
