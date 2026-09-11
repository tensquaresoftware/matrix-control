---
title: 'NumberBox edit caret geometry'
type: 'bugfix'
created: '2026-09-11'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
baseline_commit: '49cdd3c73e69fc5743f8ac369f47325566d04ec1'
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** During CurrentPatchNumber edit, the blinking white caret does not follow a clear vertical inset relative to the NumberBox border across UI Scales. Stock JUCE `CaretComponent` forces a 2 px width and lives in the TextEditor text holder, so scale-aware centering fails.

**Approach:** Keep the existing NumberBox border thickness `T` (`borderStrokeThickness()` / design 2 + snapped stroke) as the single scale token. While editing: red fill covers the full inner plate with no black ring under the grey border; border paint stays as today. Draw a custom white blinking caret with width `T`, top inset `2T`, height `H - 4T` (red void `T` above and below the caret inside the border). Horizontally: centre when the field is empty (clear-to-type); follow the insertion point when there is text. Do not reopen NumberBox gestures or change the red edit chrome. For sliders only: set blinking caret thickness to the same `T` formula without changing track height, edit plate, or gestures.

</frozen-after-approval>

## Implementation Notes

- Reused NumberBox border thickness via shared `ScaledDrawing::snappedControlBorderThickness` (design 2, kRound) — single T token for NumberBox border + caret and slider caret width.
- NumberBox: disabled stock TextEditor caret; parent paints red plate + border; transparent editor; custom caret in `paintOverChildren` with `y = 2T`, `height = H - 4T`, width `T`; blink timer 500 ms; horizontal X from TextEditor caret centre, clamped to keep T void from left/right border.
- Slider: `ScaledWidthCaretComponent` + editor LookAndFeel so caret width = same T; vertical JUCE placement unchanged; clip paint to parent to avoid bleed.
- Review patches: TextEditor-based X (not GlyphArrangement), horizontal T void clamp, slider caret parent clip.
- Smoke-test fix: caret height used `NumberBox::getHeight()` which returned unscaled design `height_` (correct only at 100% UI Scale). Now uses `getLocalBounds().getHeight()`; renamed accessor to `getDesignHeight()`.
- Did not reopen NumberBox gestures or red chrome fill colour.

## Review Triage Log

- Horizontal clamp allowed caret on grey border — `medium` / patched: clamp X to `[T, W - 2T]`.
- Empty vs typed X jump from centre-vs-left placement — `medium` / patched: centre width-T on TextEditor insertion X.
- GlyphArrangement vs TextEditor layout drift — `medium` / patched: use `getCaretRectangle()` for X.
- Bare LookAndFeel_V4 on slider editor — `false`: editor colours set on the component; only createCaretComponent needed.
- Slider vs NumberBox horizontal placement differ — `false`: slider thickness-only; NumberBox uses full geometry rule.
- Blink 500 ms vs JUCE ~380 ms — `low` / rejected: cadence not in Intent; not worth coupling timers.
- Spec thin on acceptance checks — `false` for oneshot code path; smoke checklist is manual.
- Caret vanishes when `H ≤ 4T` — `defer` (unverified at product sizes): CurrentPatchNumber height at tested scales keeps `H > 4T`; prove with extreme scale/height combo.
- Raw `2.0f`/`4.0f` multipliers — `low` / rejected: matches Intent formula literally.
- Duplicate caret notify on type — `low` / rejected: harmless blink restart.
- No unit tests for caret geometry — `defer`: visual UI Scale smoke is the acceptance path for this paint detail.
- Unclipped slider caret bleed — `medium` / patched: intersect paint with parent local bounds.

### Review Findings

- [x] [Review][Patch] Misleading editCaretX comment claims a full horizontal T red void while clamp is flush to the inner border edge [`Source/GUI/Widgets/NumberBox.cpp:325`] — fixed: comment matches clamp / Intent (no horizontal T void)
- [x] [Review][Defer] NumberBox edit caret may vanish when H ≤ 4T [`Source/GUI/Widgets/NumberBox.cpp:186-189`] — deferred: already in deferred-work.md; not hit at CurrentPatchNumber for 50–150%
- [x] [Review][Defer] No automated tests for NumberBox/slider edit caret geometry vs UI Scale — deferred: already in deferred-work.md; Manual UAT / UI Scale smoke is the acceptance path (CONVENTIONS §8.5)

#### Rejected

- Intent empty-vs-typed X vs always following TextEditor caret — `false`: centred empty field + `getCaretRectangle()` already matches Intent behaviour
- W < 3T inverted `jlimit` range — `false`: product editable NumberBox (CurrentPatchNumber) keeps W ≫ 3T across 50–200% scales; unreachable path
- Slider `jmax(1.0f, T)` breaks T parity — `false`: at product scales snapped T ≥ 1; floor never changes painted width
- NumberBox caret uses `editorText` not `editorCaret` — `false`: NumberBoxLook has no caret token; slider maps both to the same skin colour
- Two caret stacks should share a helper — `false`: Intent keeps slider thickness-only vs NumberBox full geometry on purpose
- Unused public `getDesignHeight()` — `low` / rejected: documents design vs live bounds after the shadowing bug; no caller harm
- Collapse `notifyEditCaretChanged` into `restartEditCaretBlink` — `low` / rejected: pure alias, no user impact
- Double blink restart on type (`onTextChange` + caret callback) — `low` / rejected: harmless (same as prior triage)
- Incomplete `ScaledDrawing` SSOT comment — `low` / rejected: comment-only, not worth a change pass
- Spec missing smoke checklist / cross-links to deferrals — rejected: fix would edit the spec under review
- Implementation Notes “T void” overclaim vs clamp — rejected as a standalone fix: would edit the spec; code comment covered by the Patch above
- Verification-gap re-raises of H ≤ 4T and missing caret tests — rejected as duplicates of existing deferred-work entries
