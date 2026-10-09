---
title: 'Modal action-footer horizontal separator (Getting Started intro pilot)'
type: 'feature'
created: '2026-10-09'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '1cf72bb1a5514836a705e8be33f8943e6c73f730'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Matrix chrome modals that show a bottom action-button row currently rely on empty vertical space only. The action zone does not read as a distinct footer band, so body copy and buttons feel less separated than the rest of the chrome language (Settings rail rule, `HorizontalSeparator` lines).

**Approach:** Pilot a thin horizontal rule above the action buttons on the Getting Started **intro** step only. Match the body text column width (`textArea`), leave one blank-line of air above and below the stroke, then visually review before extending to other Matrix chrome dialogs that share the same button-row pattern.

## Boundaries & Constraints

**Always:**
- Pilot scope is Getting Started wizard **intro** (`Step::kIntro`) only.
- Rule width equals the modal body text column (`geometry.textArea` width / `bodyTextWidthFor`), not full dialog content width and not the button pack width.
- Leave one empty-line equivalent of vertical air above and below the stroke (use `DialogMatrixHelpers::measureLineStep` with the modal body font).
- Stroke thickness is 1 design pixel (same family as `HorizontalSeparator` / Settings rule at 100% scale).
- Rule colour is `SkinColourId::kHorizontalSeparatorLine` (`ColourChart::kDarkGrey5`). If it reads too light after visual review, switch to Settings rail grey (`ColourChart::kDarkGrey4`) in a follow-up tweak.

**Never:**
- Do not extend to Settings, About, or other modals in this change (follow-up after visual approval).
- Do not change intro copy, button labels, or wizard flow behaviour.
- Do not add a live `HorizontalSeparator` widget unless paint-only reuse of its colour/look is clearly simpler; prefer drawing in the existing dialog paint path for a one-step pilot.
- Do not apply AffineTransform scaling; use existing `uiScale` / `scaledDesign` helpers.
- Do not put French in source strings or comments.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Intro shows rule | Getting Started opens on intro with CONFIGURE LATER / CONTINUE | Thin H-rule sits between body copy and button row; width matches welcome text column; air above and below ≈ one body line each; colour matches HorizontalSeparator | N/A |
| Later GS steps unchanged | User advances past intro | No new footer rule on steps that were not part of the pilot | N/A |
| UI scale | Scale 100% / 125% / 150% / 200% | Rule thickness, width, and air scale with modal metrics; no clipping into title, text, or buttons | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Dialogs/DialogMatrixHelpers.h` / `.cpp` — shared Matrix chrome: `kGapBeforeButtons` (24), `computeModalGeometry` (`textArea`, `band`, `buttonRow`), `bodyTextWidthFor`, `measureLineStep`, `paintBodyText`, `layoutCentredButtonRow`. Prefer extending geometry/gap here only if the pilot clearly needs a shared band; otherwise keep pilot paint local.
- `Source/GUI/Dialogs/GettingStartedWizardDialog.cpp` — `paint` / `computeGeometry` / intro path; today paints chrome + body text only; buttons laid out in `resized` from `geometry.buttonRow`.
- `Source/GUI/Dialogs/GettingStartedWizardMetrics.h` — per-step control-band heights; intro has no control rows (gap is pure `kGapBeforeButtons`).
- `Source/GUI/Dialogs/GettingStartedWizardFlow.h` — intro buttons CONFIGURE LATER + CONTINUE.
- `Source/GUI/Settings/SettingsWindow.cpp` — vertical rule painted with `ColourChart::kDarkGrey4`.
- `Source/GUI/Widgets/HorizontalSeparator.cpp` + `SkinColoursWidgetsLayout.h` — 1px line via `SkinColourId::kHorizontalSeparatorLine` (`kDarkGrey5`).
- `Source/GUI/Skins/ColourChart.h` — `kDarkGrey4` / `kDarkGrey5`.
- Follow-up candidates (out of this pilot): `EpromTypePromptDialog`, `MatrixOrderedConfirmDialog`, `MatrixMutatorDeleteConfirmDialog`, `MasterInitConfirmDialog`, `MasterM1kmLoadChoiceDialog`, `MutatorHistoryDefragConfirmDialog`, `BankTransferProgressDialog`. Not candidates: `AboutWindow` / `SettingsWindow` (no bottom action-button chrome row).

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Dialogs/GettingStartedWizardDialog.cpp` (and helpers only if required) -- For intro only, reserve vertical space for air + 1px rule + air above the button row, paint the rule at `textArea` width using the chosen colour token, keep other steps visually unchanged -- Delivers the aesthetic pilot without touching unrelated dialogs.
- [ ] Manual visual check -- Open Getting Started intro at default UI scale and at least one higher scale; confirm rule width vs welcome text, air, and no collision with buttons -- Visual change has no automated UI test.

**Acceptance Criteria:**
- Given Getting Started intro is visible, when the dialog paints, then a 1px horizontal rule spans the body text column width between the welcome copy and the action buttons, with roughly one blank line of space above and below the rule.
- Given the user is on a Getting Started step other than intro, when that step paints, then no new footer separator from this change appears.
- Given UI scale changes, when intro is shown, then the rule and spacing remain aligned to scaled `textArea` / button metrics without clipping.

## Implementation Notes

- Intro-only: `bandHeightOverride` on `DialogMatrixHelpers::ModalGeometryArgs` sizes the band via shared `introFooterRuleMetrics()` (`measureLineStep` + snapped rule slot + `measureLineStep`). Paint uses the same helper and `SkinColourId::kHorizontalSeparatorLine` at `textArea` width. Other GS steps unchanged.
- Review patch: rule slot derived from snapped stroke thickness so slot and paint cannot diverge across UI/display scales.
- Metrics comments document that intro planning still uses `kGapBeforeButtons` while runtime uses the taller footer band.
- Manual visual check still pending (human): open Getting Started intro at default and higher UI scale.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| Snapped stroke thicker than `scaledDesign(1)` rule slot (Edge Case + Blind) | medium → patched | Confirmed: `snappedStrokeThicknessFromDesign` can exceed `round(1*uiScale)` (e.g. fractional scales on Retina). Fixed via shared `introFooterRuleMetrics`. Route: patch. |
| Dual lineStep measurement with different widths (Edge Case + Blind) | false | `bodyWidth` and `textArea` width use the same inset formula; values match. Still unified via helper for clarity. |
| Non-positive textArea width on intro paint (Edge Case) | false | `bodyTextWidthFor` returns `jmax(1, …)`; intro geometry always has a positive text column. |
| Metrics / contract tests still plan intro with 24 px gap (Verification Gap + Blind) | low → rejected | Planning floors already diverge from measured body height by design; intro still stays under Settings with a slightly taller runtime band. Comments updated; formula rewrite deferred to shared-helper follow-up. |
| Contract tests never call `bandHeightOverride` (Verification Gap) | low → rejected | Same as above; GUI paint/geometry of live dialog is manual per project conventions; no DialogMatrixHelpers.cpp in test link set. |
| Intro band can shrink below 24 px if lineStep is tiny (Blind) | maybe-false | Modal body font is 14 px; `2*lineStep+slot` is typically above 24. No observed shrink path; would need font collapse. |
| `TextModalLayoutArgs` omits `bandHeightOverride` (Blind) | false | Intro does not use `computeTextModalLayout`; unused path for this pilot. |
| Override discarded when `extraBandHeight > 0` (Blind) | defer | Intentional pilot limit; matters only when extending the rule to control-band steps. |
| Missing units wording on `bandHeightOverride` doc (Blind) | low → rejected | Cosmetic; adjacent `extraBandHeight` already documents pixel height; unlikely everyday harm. |

## Design Notes

Getting Started intro has no control band: `geometry.band` is currently the empty `kGapBeforeButtons` (24 design px) between `textArea` and `buttonRow`. One body line step is often near that size, so air+rule+air may need a slightly taller reserved gap than today's 24 px — expand the intro gap rather than squeezing the stroke into the old space.

When extending later, prefer a shared helper on `DialogMatrixHelpers` (paint + optional gap metric) used by every Matrix chrome dialog that calls `layoutCentredButtonRow`, rather than copy-pasting per dialog. Settings and About stay out: they close via the title-bar control, not a bottom action row.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: plugin target builds cleanly after the paint/geometry change.
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/`.

**Manual checks (if no CLI):**
- Launch Getting Started intro: rule colour/width/air match the approved decision; CONFIGURE LATER / CONTINUE still centred and clickable.
- Advance one step: no accidental rule from this pilot.
