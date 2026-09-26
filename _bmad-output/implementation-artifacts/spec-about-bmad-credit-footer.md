---
title: 'About modal BMad credit footer'
type: 'feature'
created: '2026-09-26'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The About modal has no BMad credit line. Guillaume wants the same closing gesture as Luthier: a discreet separator, then an italic sentence with a clickable BMad link.

**Approach:** Add a bottom credit row to About — horizontal separator, italic phrase `Yet another project successfully completed with BMad!`, link word `BMad` to `https://github.com/bmad-code-org/bmad-method` — and grow the panel design height so the footer fits without clipping.

</frozen-after-approval>

## Implementation Notes

- Route: oneshot (small About-only UI credit; no irreversible side effects).
- Copy SSOT in `PluginDisplayNames::About`: prefix / `BMad` / `!` / URL; ASCII punctuation (no space before `!`).
- Painted italic prefix+suffix via synthetic `Font::italicised()` (skin has no italic face); orange `HyperlinkButton` for `BMad` (`ColourChart::kOrange`); separator uses `kHorizontalSeparatorLine` + `ScaledDrawing` 1px stroke.
- `AboutPanel::kDesignHeight` 232 → 264 for separator + balanced gaps + credit row + bottom padding; contextual help bound for the BMad link.
- Extracted `paintBmadCredit` to keep `paint` under the GUI function-length gate.
- Review patch: added `kGapAfterCreditSeparator_` so space below the hairline matches the closing rhythm (was tight against the text).
- Manual check: open About at 100%/200%, click BMad → GitHub URL, Escape still closes.

## Review Triage Log

- Gap above separator vs hairline-to-text — medium — patched: added equal gap after separator band; height set to 264 from stack arithmetic.
- Hand-picked height without bottom padding — medium — patched: height now matches padding + blocks + credit stack (264).
- Orange hardcoded vs skin token — false — About already hardcodes chart colours for links (white); orange matches Luthier accent per intent.
- Paint/HyperlinkButton seam or underline mismatch — false — same HyperlinkButton + GlyphArrangement pattern as email/GitHub/LinkedIn rows.
- Credit overflow if too wide — low rejected — fixed short English phrase fits 440 design width; no wrap policy needed.
- Spec missing Luthier property checklist — low rejected — frozen Intent + Implementation Notes cover the gesture; formal parity matrix not required for oneshot.
- Spec still in-progress / no verification note — medium — patched: status done; manual verification bullets added.
- About docs/story 7.10 out of date — defer — recorded in deferred-work.md.
- Spec missing trailing newline — low — patched on finalize.
