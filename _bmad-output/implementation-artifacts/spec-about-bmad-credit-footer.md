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

### Review Findings

- [x] [Review][Patch] AboutTextLink opens URL on non-left mouse click [Source/GUI/About/AboutPanel.cpp:79] — fixed: left-button press tracked in mouseDown; mouseUp launches only after left press
- [x] [Review][Defer] Hover red `0xff9A131D` duplicated across AboutPanel / AboutWindow / SettingsWindow [Source/GUI/About/AboutPanel.cpp:16] — deferred: oneshot hors scope for close-button colour SSOT; third literal added here
- [x] [Review][Defer] No automated check that BMad credit paints / panel height fits [Source/GUI/About/AboutPanel.cpp:320] — deferred: project GUI/manual-test policy; Manual UAT covers About
- [x] [Review][Defer] No automated check that AboutTextLink click launches URLs (all four links) [Source/GUI/About/AboutPanel.cpp:79] — deferred: same GUI/manual-test policy; smoke covered click BMad

#### Rejected

- Spec / triage still describe orange HyperlinkButton — false: product intent renegotiated post-smoke (idle = label colour, custom AboutTextLink); fixing would only edit the spec under review
- Frozen Intent omits Email/GitHub/LinkedIn rewrite — false: intentional polish for shared hover red; already decided
- Acceptance: idle not orange / HyperlinkButton required — false: same renegotiated intent; code matches current product decisions
- Separator gaps 12 vs 8 not “equal” — low: smoke validated layout; 4 px design asymmetry not everyday defect; equalizing needs taste call beyond a mechanical patch
- AboutTextLink lacks keyboard / accessibility handler — low: About is mouse-first; Escape still closes; full a11y would add focus + key + handler complexity
- Disabled link still paints as active — false: About links are never disabled in this panel
- Tooltip URL dropped vs HyperlinkButton — false: ContextualHelpBinder already covers destination help in the footer
- Credit overflow if phrase wider than content — low: fixed short English copy; unlikely everyday; prior triage rejected same risk
- `layoutHyperlinkButtons` / `refreshHyperlinkAppearance` naming — low: cosmetic rename, not everyday harm
- `review_loop_iteration: 0` stale — rejected: fix is edit spec under review
- Manual checklist gaps in Implementation Notes — rejected: fix is edit spec under review
- Commit message claims orange hyperlink — false for code defect: narrative stale; UI matches renegotiated colours
