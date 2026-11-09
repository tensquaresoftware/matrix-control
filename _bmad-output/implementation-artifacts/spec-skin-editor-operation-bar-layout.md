---
title: 'Skin Editor — OPERATION bar beside logo (min 700, stretch)'
type: 'feature'
created: '2026-11-09'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-chrome-session-skins.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Editor still stacks OPERATION on its own row under the logo, which costs vertical space the body panels could use.

**Approach:** Put SESSION then OPERATION on the title row (no in-app logo). Give OPERATION a 700 px minimum width and let it grow when the window widens; drop the old under-logo OPERATION strip so the chrome is shorter. SESSION separators use a single button gap on each side.

</frozen-after-approval>

## Implementation Notes

Agent decisions (user-invisible):
- Title row L→R: SESSION at `sessionWellPreferredWidth()` → `kButtonGap` → OPERATION (≥700, stretches to the right edge).
- Keep `kTitleBarHeight` (54) and shared well height 36; vertical placement mirrors current SESSION (`kChromeStackGap` above + well + bottom gutter language).
- Remove the dedicated OPERATION band and one extra gutter from `resized` / `skinEditorMinContentHeight` (36 + 6 = 42 px saved).
- Fold `kOperationWellMinWidth = 700` into `windowContentWidth()` / title-row min so the constrainer respects logo + 700 + SESSION + edge gutters.
- Two-line logo both at `kAppTitleFont` 12 px (same size); geometrically centred (`kLogoTitleNudgeY = 0`).
- Body column stretch (Palette / Mappings) stays as today when the window widens; OPERATION also expands in the title mid-slot.
- Do not change mapping-witness / start-hint content layout inside the OPERATION pocket; only the well's outer bounds change.

Done:
- Added `kOperationWellMinWidth`; renamed title min helper to `windowTitleChromeMinWidth()`.
- `layoutTitleBar` places OPERATION between logo and SESSION; `layoutSessionWell` takes explicit well bounds.
- Dropped under-logo OPERATION strip from `resized` and `skinEditorMinContentHeight`.
- Updated `SkinEditorChromeLayoutTests` for new min width / min height stack.
- Quality gate OK; Skin Editor + chrome layout unit tests green.
- Review patches: refresh `kChromeStackGap` comment; rename `layoutOperationWell` param to `well`; lock exact title-chrome min-width sum in a dedicated unit test; correct saved-height note to 42 px.
- Follow-up: OPERATION min width 700; logo removed; SESSION separators use one `kButtonGap` each side; SESSION left / OPERATION right.

## Review Triage Log

- false — SESSION shrink-on-collision removed: window constrainer min width already includes logo + OPERATION 650 + SESSION preferred, so overlap is not reachable in normal resize.
- false — Spec `in-progress` vs Done notes: lifecycle finalized to `done` after review.
- false — Title label text vs `kAppTitleText` drift: same pre-existing dual source as before this change; not introduced here.
- false — Mapping witness unusable at OPERATION 650: intentional product floor from the request; pocket remains wider than title-tab + pad.
- false — Older `spec-skin-editor-chrome-session-skins` still says OPERATION under logo: historical done artifact; this freeform spec supersedes that chrome placement.
- low (patched) — Obsolete `kChromeStackGap` SESSION↔OPERATION comment: updated.
- low (patched) — `layoutOperationWell(... band)` vocabulary: parameter renamed to `well`.
- medium (patched) — Title-chrome min width not locked exactly: dedicated unit test asserts the full sum.
- low (patched) — Implementation Notes claimed ~48 px saved: corrected to 42 px (36 + 6).
- low (rejected) — Rename `kMappingBandHeight` constant: cosmetic alias still equals well height; wider rename deferred.
- defer — Extract shared `skinEditorMinContentHeight` helper for MainWindow + tests: pre-existing duplication pattern.
- defer — Broader rename of `kMappingBandHeight` / “band” comments across chrome docs: not required for the layout change.
