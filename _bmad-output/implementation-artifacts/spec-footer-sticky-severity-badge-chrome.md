---
title: 'Footer sticky severity badge — permanent close square chrome'
type: 'refactor'
created: '2026-09-28'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
baseline_commit: 'c77771067e9cc953591096001cc332dd1e3c458a'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Sticky INFO / WARNING / ERROR badges still use severity pictos that swap to a close cross on hover, with a full-badge hit-zone — hard to discover and visually noisy for an 80s angular chrome.

**Approach:** Replace that chrome with a permanent close square (severity fill, dark cross always visible) + 1/2 px vertical separator + unchanged severity text badge. Hit-zone and hand cursor are the close square only; hover lightens that square’s fill only. Clear still goes through `ExceptionPropagator::clearMessage`. KEEP / AUTO CLEAR and HELP pause stay untouched. Scope is sticky severity only — not DEVICE, not furtive HELP.

</frozen-after-approval>

## Implementation Notes

- Route oneshot: permanent close square (side = badge height) + discrete 1/2 px separator + severity text badge; hit-zone = close square only.
- Removed severity pictos, inset square invert chrome, and hover glyph swap; kept `makeSeverityCloseCrossShape` geometry.
- Hover: close-square fill stays severity colour; cross is slightly smaller/thicker; hover paints cross in `kButtonTextHover` (white), matching button text hover.
- Removed ColourChart hover-fill tokens and `stickySeverityCloseHoverFill`.
- Pure helpers + StickyInfoMessagePolicyTests updated (geometry, separator thickness, square narrower than full chrome); auto-clear policy tests unchanged and green.
- Build macos-debug-arm64 OK; lint_touched OK. No Settings / KEEP-AUTO CLEAR changes.
- Review patches: fixed hover-fill doc comment; wire separator base to design constant; assert hit square width < badge width.

## Review Triage Log

- Doc comment claimed rest fill for `stickySeverityCloseHoverFill` — **high/medium→patch**: comment fixed to describe hover tokens.
- `BadgeChromeMode::SeverityIcon` naming leftover — **low rejected**: rename churn without smoke value; behaviour is correct.
- Design separator/close tokens not all in PanelDimensions — **medium→patch** for separator base via DesignPanels; close side already equals scaled badge height (asserted equal in DesignChecks).
- Dual SkinColourId rest vs ColourChart hover — **defer**: skins currently share chart values; SkinColourId hover plumbing out of scope.
- Unit test did not lock hit square narrower than full chrome — **medium→patch**: assert added.
- Done INFO MESSAGE spec still describes old chrome — **defer**: historical done artifact; chrome clauses obsolete, KEEP/AUTO CLEAR still valid.
- Oneshot missing smoke AC / still in-progress — **false** for code: oneshot presents smoke to human; status set `done` after review.
- ERROR 0.8 alpha only in paint path — **false**: mirrors `getSeverityColour` rest path; helper returns opaque chart tokens by design.

## Review Findings

### Decision-needed

- [x] [Review][Decision] Hover cross geometry vs colour-only — **resolved → rejected**: Guillaume (2026-09-28): smaller/thicker cross is the permanent rest+hover glyph; hover intentionally changes colour only (`kButtonTextHover`). No geometry swap on hover. Notes wording can be clarified later; not a code defect.

### Patch

- [x] [Review][Patch] Wire separator design token into runtime ladder (or lock with test) [`Source/GUI/Helpers/FooterSeverityBadge.h:23`]
- [x] [Review][Patch] Remove dead `BadgeDetailPaintArgs::stickySeverity` after picto removal [`Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.h:80`]
- [x] [Review][Patch] Remove unused `StickyInfoMessagePolicy.h` include from `FooterSeverityBadge.h` [`Source/GUI/Helpers/FooterSeverityBadge.h:5`]
- [x] [Review][Patch] Fill close-strip gutters when square is height-clamped narrower than badge [`Source/GUI/Panels/MainComponent/FooterPanel/FooterPanelStickyMessage.cpp:66`]
- [x] [Review][Patch] Correct stale deferred-work ColourChart hover-fill entry (shipped hover is white cross via SkinColourId, not ColourChart fills) [`_bmad-output/implementation-artifacts/deferred-work.md:2090`]
- [x] [Review][Patch] Rename test `separatorThicknessAndHoverFills` — no hover-fill asserts remain [`Tests/Unit/StickyInfoMessagePolicyTests.cpp:150`]

### Defer

- [x] [Review][Defer] Hit-zone “square only” not locked by FooterPanel wiring test [`Source/GUI/Panels/MainComponent/FooterPanel/FooterPanelStickyMessage.cpp:282`] — deferred: CONVENTIONS forbid GUI component / paint unit tests; pure geometry helpers covered; product contract relies on smoke.

### Rejected

- Frozen Intent “hover lightens fill” / “1/2 px” vs shipped smoke contract — **false** as code defect: human smoke renegotiated fill-stable + white cross + 1/2/3/4 ladder; fix would be editing frozen Intent (out of review patch scope).
- Acceptance Auditor “separator exceeds 1/2 px” — **false**: discrete ladder is the current contract (impl notes + tests + user smoke).
- `BadgeChromeMode::SeverityIcon` / `paintSeverityIconBadgeChrome` naming leftover — **low rejected**: misleading but behaviour correct; rename churn excluded by review brief.
- Spec Review Triage still mentions removed `stickySeverityCloseHoverFill` — **rejected**: fix is editing the spec under review.
- Missing written smoke checklist in Implementation Notes — **rejected**: oneshot already smoke-validated by human; fix is spec edit.
- Narrow-band missing separator-collapse assert — **low rejected**: rare clamp path; everyday harm unlikely; more test complexity than direct value.
- Duplicate “old INFO MESSAGE spec chrome clauses” — **rejected**: already deferred in deferred-work from prior review.
