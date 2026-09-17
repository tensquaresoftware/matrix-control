---
title: 'Darken furtive contextual HELP footer chrome to #888888'
type: 'chore'
created: '2026-09-17'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Furtive contextual HELP in the left footer (HELP badge fill and detail text) was too close to GUI label grey (`#B9B9B9`), so the overlay did not read as quieter chrome.

**Approach:** Paint HELP badge fill and HELP detail text with `#888888` (midway toward sticky INFO `#606060`). Do not change shared label greys or sticky / DEVICE footer message colours.

</frozen-after-approval>

## Implementation Notes

- Added `ColourChart::kContextualHelpChrome` as the hex SSOT for furtive HELP badge fill + detail; left `kLightGrey2` (`#B9B9B9`) unchanged so GUI labels stay as-is.
- Tuned to `#888888` (midway between prior HELP `#B0B0B0` and sticky INFO `#606060`); sticky INFO chrome stays `kFooterMessageInfo` / `kDarkGrey6` (`#606060`).
- `FooterPanel::paintContextualHelp` uses that constant for badge fill and detail text; badge glyph colour remains the footer background.
- Skipped wiring a new `SkinColourId` for this oneshot (smaller footprint); ColourChart already included by `FooterPanel.cpp`.
- Side effect: HELP no longer follows Cream `LabelText` (`kDarkGrey1`), which would have painted near-invisible chrome on the dark footer.
- Manual UAT: hover any HELP target → badge fill + detail `#888888`; sticky INFO / DEVICE / label greys unchanged.
- Commit skipped pending explicit request (dirty tree also has unrelated artifacts).

## Review Triage Log

- Spec still `in-progress` / empty notes at review time — `false` for code; process gap closed by this finalize.
- Frozen Intent omits constant / call-site names — `false`; oneshot Intent stays product-level; approach recorded in Implementation Notes.
- ColourChart literal instead of `SkinColourId` footer token — `low` rejected; intentional oneshot footprint; defer only if a second footer-only grey needs the same skin table path.
- Older Mutator HELP spec still cites `kLabelText` for chrome — `defer` (docs drift, not caused as a product bug by this paint change).
- Cream `kLabelText` invisibility avoided by hardcoding light chrome — `false` as defect; beneficial side effect noted above.
- Missing smoke checklist in slim oneshot spec — `low` rejected; manual check listed in Implementation Notes.
- 9/255 RGB delta may be subtle — `false`; hex was an explicit human choice.
- Constant sits between numbered light greys / one-off name — `low` rejected; name encodes HELP purpose.
- Missing `baseline_commit` oneshot metadata — `low` rejected; not required by current oneshot template.
- Constant comment omits sticky/DEVICE non-goals — `low` rejected; non-goals live in frozen Intent.
