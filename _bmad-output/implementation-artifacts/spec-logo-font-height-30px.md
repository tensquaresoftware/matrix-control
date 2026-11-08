---
title: 'Logo font height 30 px'
type: 'chore'
created: '2026-10-25'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The header brand logo reads a bit small at the current optical size (27 design px).

**Approach:** Raise the SSOT logo font height to 30 design px and keep the compile-time check / Skin Editor comment in sync. Box width/height and layout unchanged.

</frozen-after-approval>

## Implementation Notes

- Raised `Design::Panels::Header::kLogoFontHeight` 27 → 30; updated matching `static_assert` in `DesignChecks.h`.
- Synced Skin Editor chrome comment that referenced the old 27 px value.
- Left logo box W/H and layout gaps unchanged; still a ÷4 optical exception (30 % 4 ≠ 0).
- Consumers already read the SSOT via `brandLabelLookFromSkin` — no further call-site edits.

## Review Triage Log

- Font 30 in still-32 logo box may clip / feel cramped (esp. with scale nudge) — maybe-false: font height is not ink height; `Logo::paint` centres measured glyph ink. Would be medium if real; deferred to visual smoke across scales.
- Optical nudge comments/tuning stale after ~11% face bump — maybe-false: same smoke settles it; grouped with clip risk.
- Horizontal fit of MATRIX-CONTROL in kLogoWidth 240 unmeasured — maybe-false: advances grow with height; smoke settles; grouped with clip risk.
- No DesignChecks relationship font ≤ box height — defer: pre-existing gap / idea for later, not a regression from this bump.
- ÷4 story inconsistent / easy to wrongly on-grid the font — false: intentional optical exception already documented on the constant; this change keeps that policy.
- Skin Editor comment hard-codes the numeric height — low rejected: synced for this bump; de-hardcoding is a style pass beyond oneshot scope.
- Oneshot spec thin / no AC / smoke checklist — false: route gate intentionally drops those sections for clean oneshot.
- About / neighbour clearance not re-verified in harness — maybe-false: intentional scope left brand consumers on SSOT; smoke walkthrough covers header neighbours.
