---
title: 'Skin Editor Palette icon action buttons'
type: 'feature'
created: '2026-10-30'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The Skin Editor Palette column is wider than colour names need, because the New / Duplicate / Del text buttons (especially Duplicate) set the panel floor width.

**Approach:** Replace those three Palette actions with square rounded icon buttons drawn as Path pictos (+, overlapping rounded rects, simplified trash), with full English tooltips. Shrink `kPalettePanelWidth` to the icon-row floor so the window narrows; keep Mapped colour aligned by shrinking that column by the same amount via the existing shared paint-width token (Mappings panel total width unchanged; Property/State absorb the freed span). Skins-panel text buttons stay as they are. No external icon bank.

</frozen-after-approval>

## Implementation Notes

- Path icons live in `SkinEditorChromeIcons.h`; LookAndFeel `drawButtonText` routes Palette buttons by component ID.
- Square actions: side = `kButtonHeight` (24). Panel width floors on chrome text + icons via `paletteListPaintFloor()` → `kPalettePanelWidth` 183 → 124; `kPaletteListPaintWidth` 155 → 96 (Mapped colour follows).
- Tooltips + `setTitle` for New / Duplicate / Delete. Empty button text. Skins panel unchanged.
- Elide section headers, empty hint, pastille names, and Mapped colour legend when paint budget is tight.
- Layout tests lock width equality to runtime floor; lint + SkinEditorChromeLayout tests green.

## Review Triage Log

- high — Chrome strings clip at icon-only 88 px paint width — patched: floor on Product/Editor/Mapped colour measures + elide safety net.
- medium — Long pastille names overflow after swatch — patched: `elideToWidth` on colour names.
- medium — Empty TextButton accessibility names — patched: `setTitle` alongside tooltips.
- false — Window does not shrink by full palette delta — by design `windowContentWidth = max(body, title+SESSION)`; Mapped colour still tracks palette paint width.
- false — `layoutActionButtons` no proportional shrink — panel width floors on icon preferred width by construction.
- false — Oneshot spec thin vs dispatch template — expected for `route: oneshot`.
- low rejected — Extra icon unit tests / rename `measurePaletteActionButtonWidth` / history comment rewrite — cosmetic vs oneshot scope.
- low patched — Plus stroke caps vs trash PathStroke — unified rounded stroke; unused Tokens include removed from Icons header.
