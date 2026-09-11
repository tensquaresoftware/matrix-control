---
title: 'Slider edit caret geometry'
type: 'bugfix'
created: '2026-09-11'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'bceea321bfb75d5f4e3f0d8653c960cb371e80c6'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-numberbox-edit-caret-geometry.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-direct-slider-value-entry.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiate">

## Intent

**Problem:** During slider double-click value edit, the blinking caret nearly fills the full dark-green slider height at most UI Scales instead of matching the inset track / value-bar band. From about 125% UI Scale upward, a fixed white bar plus a thin black blink line can appear together (double-caret look).

**Approach:** Match NumberBox’s custom-caret spirit (disable stock TextEditor caret; paint one white blinking caret on the Slider) without any Slider↔NumberBox class coupling. Vertical reference is the same scale-aware inset used for the idle value bar / disabled rail (`logicalInsetPixelsFromDesign(kValueBarPadding_)` → `reduced(insetPerSide)`), not the full track plate and not NumberBox’s `2T` / `H−4T` border formula. Caret thickness matches the CurrentPatchNumber visual family (design thickness 2, snapped scale-aware) but is owned and computed on the Slider side and injected as a Slider dimension — never read from NumberBox. Keep current edit behaviour (bold white text, full edit plate, idle value bar and text hidden).

**Decisions:**
- Caret thickness: same visual family as CurrentPatchNumber (design 2, scale-aware snap) — option A.
- Ownership: Slider recalculates / injects that thickness itself (local helper or Look/config injection). No include, call, or type dependency between `Slider` and `NumberBox`. Shared layout math via `ScaledDrawing` is fine; widget-to-widget coupling is not.

## Boundaries & Constraints

**Always:**
- Vertical caret band = live slider height minus `2 × insetPerSide` (same inset as idle value bar / disabled rail); top/bottom void must read clearly against the dark-green plate at tested UI Scales.
- One visible blinking caret only (no stock JUCE caret underneath or beside the custom paint).
- Thickness is scale-aware, owned by Slider (injected dimension / local ScaledDrawing call), visually matching design-2 snapped control border thickness without consulting NumberBox.
- Horizontal placement follows the TextEditor insertion point (empty clear-to-type centred field; typed digits follow caret X), without reopening edit gestures.
- NumberBox / CurrentPatchNumber remain visually unchanged unless an obvious cross-regression appears.

**Never:**
- Couple `Slider` to `NumberBox` (no includes, callbacks, shared widget helpers that force one class to know the other).
- Reopen slider edit gestures (double-click, Enter / Escape, Cmd/Ctrl-click reset) or NumberBox chrome / gestures.
- Keep the prior NumberBox-era “slider = thickness-only + stock JUCE vertical” contract for this work — that vertical choice is overturned here.
- Use unscaled design height for caret geometry (known NumberBox pitfall).
- Broad idle slider paint refactors beyond what the caret needs; PatchNameDisplay and other non-slider custom carets; automated caret-geometry unit tests (manual UI Scale smoke unless Build later decides otherwise).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Empty edit @ 100% | Double-click slider, empty clear-to-type | Single white blinking caret; height ≈ value-bar band; visible top/bottom void vs plate | N/A |
| Typed digits | Enter digits during edit | Same vertical band; X follows insertion; still one caret | N/A |
| High UI Scale | Edit at ≥125% (also 150%) | No white+black double caret; thickness stable | N/A |
| Low UI Scale | Edit at &lt;100% | Same vertical inset rule; caret still visible if band height &gt; 0 | Skip paint if computed height ≤ 0 |
| Cancel / commit | Escape or Enter | Editor dismisses; idle paint unchanged | Existing parse / cancel paths unchanged |

</frozen-after-approval>

## Code Map

- `Source/GUI/Widgets/SliderEditing.cpp` — `SliderEditField` currently `setCaretVisible(true)` + `SliderEditCaretLookAndFeel` → `ScaledWidthCaretComponent`; `showValueEditor` / `layoutEditor` / `applyEditorAppearance`; leave gestures, parse, Cmd/Ctrl reset alone.
- `Source/GUI/Widgets/Slider.cpp` / `Slider.h` — idle `insetPerSide` + value bar / disabled rail; add parent-side caret paint + blink (NumberBox pattern: `paintOverChildren`, timer, stock caret off). Reuse the same inset formula for caret Y/H.
- `Source/GUI/Widgets/ScaledWidthCaretComponent.h` — current thickness-only caret (full JUCE height, unclipped paint, parent clip); slider-only caller today — remove or stop using once stock caret is disabled.
- `Source/GUI/Widgets/NumberBox.cpp` / `NumberBox.h` — reference only: `setCaretVisible(false)`, `paintOverChildren` caret, blink 500 ms, X from `getCaretRectangle()`; do not edit unless cross-regression.
- `Source/GUI/Layout/ScaledDrawing.h` — `snappedControlBorderThickness`, `logicalInsetPixelsFromDesign`.
- `_bmad-output/implementation-artifacts/spec-numberbox-edit-caret-geometry.md` — prior “slider thickness-only” note overturned for vertical here.
- `_bmad-output/implementation-artifacts/spec-direct-slider-value-entry.md` — product edit behaviour to preserve.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Widgets/SliderEditing.cpp` / `Slider.h` / `Slider.cpp` -- Disable stock TextEditor caret; paint one blinking slider caret with value-bar insets for Y/H; inject Slider-owned design-2 snapped thickness (no NumberBox coupling); TextEditor-based X; drop LookAndFeel/`ScaledWidthCaretComponent` path if unused -- Fix full-height caret and double-caret artefact without touching edit gestures.
- [x] `Source/GUI/Widgets/ScaledWidthCaretComponent.h` -- Remove file (and includes) if no remaining callers after the switch -- Avoid a dead thickness-only caret stack.
- [ ] Manual UI Scale smoke (100%, 125%, 150%, one &lt;100%) -- empty + typed edit; Enter / Escape -- Confirm height, single blink, NumberBox unchanged.

**Acceptance Criteria:**
- Given a slider in double-click edit, when viewing the caret at 100% / 125% / 150% / one scale below 100%, then a single white caret blinks, its height matches the inset value-bar band (visible void above and below vs the plate), and no black parasitic line appears.
- Given digits typed in the edit field, when the caret moves, then vertical geometry stays on the inset band and X follows the insertion point.
- Given CurrentPatchNumber edit after this change, when comparing appearance, then NumberBox caret geometry and chrome are unchanged.
- Given Escape or Enter during slider edit, when the editor closes, then idle track / value bar / text behaviour is unchanged from today.

## Implementation Notes

- Disabled stock TextEditor caret (`setCaretVisible(false)`); removed `SliderEditCaretLookAndFeel` / `ScaledWidthCaretComponent`.
- Slider paints one white blinking caret in `paintOverChildren` (500 ms timer), thickness via local `sliderCaretThickness()` → `ScaledDrawing::snappedControlBorderThickness` (design 2); no NumberBox includes.
- Vertical band: `inset = logicalInsetPixelsFromDesign(kValueBarPadding_, …)`, `y = inset`, `height = H − 2×inset` using live `getLocalBounds().getHeight()`.
- Horizontal X from `TextEditor::getCaretRectangle()` centre, clamped to local width; caret/text change restarts blink.
- Deleted unused `ScaledWidthCaretComponent.h`.
- Build `macos-debug-arm64` + `lint_touched.py` pass; Manual UI Scale smoke still open.
- Review patch: `Slider::resized` now `repaint()`s after `layoutEditor()` so open-edit resize does not leave a stale caret until the next blink.

## Spec Change Log

## Review Triage Log

- `resized` without `repaint` during edit leaves stale caret — `medium` / patched: `layoutEditor()` + `repaint()` (NumberBox pattern).
- `setUiScale` does not restart blink — `false`: already `layoutEditor()` + `repaint()`; mid-off blink until next tick matches NumberBox; not a defect of this change.
- Duplicated inset math vs idle paint — `low` / rejected: same formula by Intent; extracting a shared helper adds surface without everyday user harm.
- Missing parent-clip after deleting ScaledWidthCaret — `false`: caret paints in `Slider::paintOverChildren` within the slider’s own bounds (full-bleed editor); old clip was for an unclipped child CaretComponent.
- `editCaretX` null returns `0` vs NumberBox `thickness` — `low` / rejected: dead path (`paintOverChildren` guards `editor_ != nullptr`); no user impact.
- Double blink restart on type (`onCaretOrTextChanged` + `onTextChange`) — `low` / rejected: harmless timer restart (same class as NumberBox triage).
- Stale Code Map present-tense / empty Spec Change Log — rejected: fix would edit this build’s spec under review.
- Companion `spec-numberbox-edit-caret-geometry.md` still documents slider ScaledWidthCaret stack — `defer`: documentation drift outside this frozen Intent; see deferred-work.md.
- Smoke axes omit `editorCaret` Look wiring — `false`: paint uses existing `look_.editorCaret`; colour wiring was already present for the prior caret path.
- No automated observer for caret visuals / open Manual smoke — `false` for requiring unit tests (frozen Never); Manual UI Scale smoke remains the intentional acceptance gate (already deferred for NumberBox/slider caret geometry).

## Design Notes

Prefer the NumberBox *pattern* (not NumberBox APIs) over stretching `ScaledWidthCaretComponent`: disable stock caret (`setCaretVisible(false)`), paint on `Slider` with a blink timer, derive X from `TextEditor::getCaretRectangle()`. That removes the dual-paint risk (JUCE 2 px caret bounds + unclipped thicker fill) seen especially at ≥125%.

Thickness ownership: Slider-local method (or Look/config injection) calling `ScaledDrawing::snappedControlBorderThickness` — same numeric family as CurrentPatchNumber, zero `NumberBox` includes. Do not factor a shared widget caret helper that both classes must depend on for this ticket.

Vertical formula (agent-owned, product-fixed by Intent):  
`inset = logicalInsetPixelsFromDesign(kValueBarPadding_, uiScale, displayScale)`  
`y = inset`, `height = H − 2×inset`, `width = sliderCaretThickness()`  
Use live `getLocalBounds().getHeight()`, never design `height_`.

Enabled idle track still fills the full plate; the caret aligns to the **inset band** (value bar / disabled rail), which is the visual “rail” height Guillaume called out.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds for touched GUI sources
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched C++ under `Source/`

**Manual checks (if no CLI):**
- Slider double-click edit at 100%, 125%, 150%, and one scale &lt;100%: single white blink; caret height ≈ value-bar band; visible top/bottom void.
- Empty then typed digits; Enter commits; Escape cancels.
- CurrentPatchNumber edit: caret / chrome look unchanged.
