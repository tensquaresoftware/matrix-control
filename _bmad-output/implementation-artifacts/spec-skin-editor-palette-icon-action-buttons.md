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

**Approach (final, after visual reject of Path icons):** Keep text action buttons; abbreviate Duplicate to Dup (New / Dup / Del). Shrink `kPalettePanelWidth` to the preferred text-row + chrome floor; Mapped colour stays aligned via the shared paint-width token. Skins-panel text buttons unchanged.

</frozen-after-approval>

## Implementation Notes

- 2026-10-31: Reverted Path icon buttons (`SkinEditorChromeIcons.h` removed). Labels: New / Dup / Del; Dup/Del keep full-word tooltips.
- `kPalettePanelWidth` 183 → 156; `kPaletteListPaintWidth` 155 → 128 (Mapped colour follows). Floor via `paletteListPaintFloor()`.
- Elide safety on tight Palette / Mapped colour chrome strings retained from the icon pass.
- Layout tests + lint green.

## Review Triage Log

- high — Chrome strings clip at icon-only 88 px paint width — patched then superseded by text Dup approach (floor 156 / paint 128).
- medium — Long pastille names overflow after swatch — patched: `elideToWidth` on colour names (retained).
- medium — Empty TextButton accessibility names — n/a after revert to labeled text buttons.
- false — Window does not shrink by full palette delta — by design `windowContentWidth = max(body, title+SESSION)`.
- false — Oneshot spec thin vs dispatch template — expected for `route: oneshot`.
