---
title: 'Skin Editor colour picker fine control'
type: 'feature'
created: '2026-10-31'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context: []
baseline_commit: '8edb0b8da4a5f448e540907142a7e968e0f8623d'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Dragging the COLOR panel SV handle feels laggy because each mouse move rebuilds Skin Editor panels and pushes a full live skin over the bridge. The SV square alone is also a poor tool for tiny saturation or brightness nudges when calibrating Look colours against the live Matrix-Control UI.

**Approach:** Keep Hue + SV + #/R/G/B, add Saturation and Brightness horizontal sliders under those fields, and make continuous colour edits keep the picker handle sticky while coalescing expensive refresh/push work so live preview stays usable.

**Decisions:**
- Fine controls: Hue + SV + #/R/G/B + Saturation and Brightness sliders only (no numeric S/B fields).
- Live preview while dragging: coalesce `refreshAll` + `pushVisible` during continuous colour edits (~30–50 ms), flush on gesture end; local handle/HSB stay immediate.
- Third-axis label: **Brightness** (not Lightness) — matches existing HSV/HSB SV well (`val_` / JUCE brightness), not PhotoCraft HSL Lightness.
- Layout under #/R/G/B (habitual vertical gaps, compact rows, then bottom content pad):
  - `Saturation:` + gap + horizontal slider
  - `Brightness:` + gap + horizontal slider
  - Gap before COLOR panel bottom edge
- Grow COLOR preferred height (+ window min height) just enough for those two rows; keep rows compact.

## Boundaries & Constraints

**Always:**
- COLOR remains the only place to edit a selected pastille ARGB while mapping/editing modes work as today.
- When the bridge is connected, Matrix-Control preview stays nearly live via coalesced refresh/push (flush at gesture end).
- Window min height and `preferredHeightForWidth` stay consistent so COLOR does not clip.
- ASCII-only UI copy in Skin Editor labels (no fancy Unicode dashes). Sliders drive absolute HSV saturation / brightness (0..1), same axes as the SV square — not relative HSL image adjustments.

**Never:**
- Do not add extra colour models (CMYK, Lab, HSL-as-second-model, colour temperature).
- Do not label the brightness axis Lightness (would imply HSL and disagree with the SV well).
- Do not change DevSkin bump/revision semantics, bridge payload schema, or product/read-only edit guards.
- Do not overload the existing 400 ms session-autosave timer for colour coalescing — use a dedicated mechanism.
- Do not globally AffineTransform-scale Skin Editor UI.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| SV / Hue drag | Editing enabled, pastille selected | Handle tracks pointer fluidly; colour model updates; refresh/push coalesced then flushed on end | Ignore when editing disabled / no active colour |
| Saturation or Brightness slider | Same | Only that HSV axis changes; SV handle and #/RGB stay in sync; same coalesce policy | Clamp 0..1; ignore when disabled |
| Same ARGB again | Drag settles on identical packed ARGB | `setPastilleArgb` early-out — no bump / no push storm | N/A |
| Bridge disconnected | Colour edits while disconnected | Local Skin Editor state updates; no push attempt | Existing status path unchanged |
| Clear / no selection | `clearActiveColour` | Wells idle; S/B sliders disabled like #/RGB | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorColourPicker.*` — SV/Hue/#/RGB; every drag notifies; extend S/B rows + height helper
- `Tools/SkinEditor/SkinEditorMainComponent.cpp` — `onChanged` → `refreshAll` + `pushVisible`; COLOR layout
- `Tools/SkinEditor/SkinEditorMainComponentActions.cpp` — `onColourChanged` → `setPastilleArgb`
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` — autosave timer (do not reuse for colour coalesce)
- `Tools/SkinEditor/SkinEditorMainWindow.cpp` — min height from COLOR preferred height
- `Source/GUI/Skins/Dev/DevSkinLayerPastille.cpp` — `setPastilleArgb` + same-ARGB early-out
- Reuse HSB state + early-out; do not change bridge protocol or edit guards

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorColourPicker.*` -- Add compact `Saturation:` / `Brightness:` horizontal slider rows under #/RGB with habitual gaps; sync with SV/Hue/#/RGB; keep handle updates cheap during drag -- axis-locked fine tune
- [x] `Tools/SkinEditor/SkinEditorMainComponent.*` (+ window min height) -- Coalesce colour-driven refresh/push (~30–50 ms, flush on gesture end); grow COLOR preferred height -- kill lag, keep Look judgment
- [x] Existing DevSkin coverage -- Same-ARGB early-out still holds; no new bridge protocol tests -- regression safety

**Acceptance Criteria:**
- Given an editable pastille and connected bridge, when dragging in the SV well, then the handle tracks the pointer without the current heavy lag, and Matrix-Control reflects the colour via coalesced nearly-live preview (flushed at gesture end).
- Given a chosen hue, when adjusting Saturation or Brightness sliders, then only that HSV axis changes and #/R/G/B plus the SV handle stay consistent.
- Given COLOR panel width at minimum window height, then #/RGB and both slider rows are fully visible with habitual gaps and bottom pad.

## Implementation Notes

- 2026-10-31: Added Saturation/Brightness LinearHorizontal sliders under #/RGB; SV/Hue drag captures target so handle stays sticky outside the well; `onColourGestureBegan/Ended` + dedicated 40 ms `BlinkTimer` coalesce `refreshAll`/`pushVisible` (autosave timer untouched); skip picker ARGB re-sync while gesture active; `preferredHeightForWidth` (+ window min) grows by two compact rows.
- Build: `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` OK; `python3 Scripts/quality/lint_touched.py` OK.
- Spec Verification listed target `MatrixControlSkinEditor`; CMake target name is `Matrix-Control_SkinEditor`.
- Updated `Tests/Unit/SkinEditorChromeLayoutTests.cpp` expected COLOR height for two axis rows; `Matrix-Control_Tests SkinEditorChromeLayout` → 0 failures.
- Review patches: end SV/Hue (+ slider) gesture on clear/disable; wheel/atomic S/B nudges wrap began/ended; `sliderDragEnded` gated; alpha comment restored; `SkinEditorColourPickerTests` + CMake link; Verification target → `Matrix-Control_SkinEditor`.

## Spec Change Log

## Review Triage Log

- false — Blind: Saturation: may clip under Brightness-measured width — both labels are 11 ASCII chars; Brightness: is the wider glyph set under the panel font, so measuring that string is the intended shared width.
- low (rejected everyday) — Blind: sliders stop at SV right edge — usable fine control remains; full-width under hue is polish, not required by frozen layout wording.
- medium — Blind/Edge: Escape / clearActiveColour / disable mid-drag leaves colourGestureActive_ / dragTarget_ until mouseUp — verified: clearMappingModeToIdle clears picker without ending gesture.
- medium — Blind/Edge/VG claim: wheel or non-drag slider valueChanged bypasses began/ended — verified: only dragStarted sets gesture; valueChanged alone notifies without coalesce wrap.
- low — Blind/Edge: sliderDragEnded lacks started guards — verified; spurious flush is one extra refresh; fix with the same gesture-depth wrap as wheel.
- false (spec-edit reject) — Blind/VG: Verification still names MatrixControlSkinEditor — true but finding whose fix is editing the spec is rejected from route; still corrected in Verification as housekeeping.
- medium (VG pre-verified patch) — No automated check that S/B sliders update colour / HSV axis — demonstration holds; height-only tests stay green if sliderValueChanged is gutted.
- defer — VG: coalesce / sticky sync untested at MainComponent — needs GUI/timer harness; manual AC remains; extract later if regresses.
- medium (VG pre-verified patch) — Height test does not assert S/B slider rows exist after resized — verified; preferredHeight can grow with empty space.
- false — Blind: alpha comment removal breaks invariant — alpha still preserved in fromHSV / hex / RGB paths; comment loss is docs-only (restored opportunistically with patches).
- low (rejected everyday) — Edge: non-colour onChanged during colourGestureActive_ delayed — colour drag is the dominant bump source; rare collision not worth branching the coalesce gate now.

## Design Notes

Lag is not SV math: each notify bumps the layer then full `refreshAll` + JSON push. Keep local HSB/handle immediate; coalesce the expensive path.

PhotoCraft Hue/Saturation "Lightness" is an HSL relative adjuster (black → colour → white track, centered at 0). Our COLOR panel already uses an HSV SV square; the vertical axis is brightness/value. Label **Brightness** so the slider and the SV well mean the same thing.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` -- expected: build succeeds
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched C++

**Manual checks (if no CLI):**
- SV drag: handle sticky; connected bridge updates Look colours nearly live.
- After a teal hue, small Saturation/Brightness nudges stay easy; RGB/# coherent; layout under #/RGB with habitual gaps.
