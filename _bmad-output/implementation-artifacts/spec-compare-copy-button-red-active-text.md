---
title: 'Red text for Compare lit blink and Copy press'
type: 'feature'
created: '2026-07-17'
status: 'done'
baseline_commit: '7a2304c99781691a4c9ba1a2122920991f495e31'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** While Compare is active, the button blinks by alpha only, so the “lit” phase still shows grey label text. Copy buttons (COPY / C) already flash red on mouse-down via `textClicked`, but that active-red language is not shared with Compare’s sustained lit state.

**Approach:** Give Button a true On text colour (`ColourChart::kRed`, same as click), drive Compare’s toggle state from `compareActive` so lit blink phases paint On text, and keep Copy on the existing press-red path so both share the same red active look.

## Boundaries & Constraints

**Always:**
- Button On text and Button clicked text both resolve to `ColourChart::kRed` on black and cream skins.
- While `patchMutatorCompareActive` is true, Compare paints as toggle On (red text) and keeps the existing 2 Hz alpha blink; when false, toggle Off and alpha fully opaque.
- COPY / C keep momentary press feedback (red via `textClicked`); no new latch unless Ask First.

**Ask First:**
- Adding a post-click hold/flash timer on Copy beyond mouse-down red.
- Changing Button On background or border (today On matches Off).

**Never:**
- Change Compare blink rate, enablement, or lock behaviour.
- Change Toggle widget green On colours.
- French UI strings; redesign hover/disabled looks.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Compare lit | Compare active, blink alpha ≈ 1 | Label text red (On) | N/A |
| Compare dim | Compare active, blink alpha ≈ 0.35 | Same On colours, faded by alpha | N/A |
| Compare off | Compare inactive | Grey Off text, alpha 1 | N/A |
| Copy press | Mouse down on COPY or C | Red text via clicked look | N/A |
| Copy release | Mouse up | Grey Off text again | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinColoursWidgetsControls.h` -- add `Widgets::Button::kTextOn` (red); `kTextClicked` already red
- `Source/GUI/Skins/Skin.cpp` -- map `kButtonTextOn` to `kTextOn` (today wrongly mirrors Off `kText`)
- `Source/GUI/Looks/LookBuilders.cpp` -- already maps `textOn` from `kButtonTextOn`; no API change expected
- `Source/GUI/Widgets/Button.cpp` -- paints `textOn` when `getToggleState()`; no paint change expected
- `Source/GUI/Panels/.../PatchMutatorPanel.cpp` -- `refreshCompareUiState`: `compareButton_->setToggleState(compareActive, dontSendNotification)`; keep alpha timer
- Copy call sites (`ModuleHeader`, `InternalPatchesPanel`, `MatrixModulationPanel`) -- verify only; press already uses `textClicked`

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinColoursWidgetsControls.h` -- add Button `kTextOn` = `ColourChart::kRed` (both variants) -- On text SSOT
- [x] `Source/GUI/Skins/Skin.cpp` -- wire `kButtonTextOn` to `Widgets::Button::kTextOn` -- stop mirroring Off text
- [x] `Source/GUI/Panels/.../PatchMutatorPanel.cpp` -- set Compare `toggleState` from `compareActive` in `refreshCompareUiState` (and ensure Off when inactive) -- lit blink uses On text
- [ ] Manual GUI check -- Compare blink lit/dim/off + COPY/C press on black and cream

**Acceptance Criteria:**
- Given Compare is active, when the blink is in the lit (full alpha) phase, then the Compare label text is red.
- Given Compare is active, when the blink is in the dim phase, then the label still uses On colours but is visibly faded by alpha.
- Given Compare becomes inactive, when the UI refreshes, then Compare text returns to Off grey and alpha is 1.
- Given the user presses COPY or a module C button, when the button is down, then its text is the same red as Compare’s lit On text.
- Given cream and black skins, when checking both, then On and clicked Button text remain `ColourChart::kRed`.

## Spec Change Log

## Design Notes

Prefer skin `textOn` + Compare `setToggleState` over a one-off `setLook` override: `Button::getTextColour` already branches on toggle state, and Copy’s press path already shares `kRed` via `textClicked`. Do not introduce a Copy flash timer unless Guillaume asks — press-red already matches the requested active colour.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Standalone` -- expected: build succeeds

**Manual checks (if no CLI):**
- Activate Compare with history present: lit phases show red “COMPARE”; dim phases fade; exit Compare restores grey.
- Click Internal Patches COPY and a module header C: text red only while pressed, same red as Compare lit.
- Spot-check cream skin for the same red.

## Suggested Review Order

**Skin On text (red SSOT)**

- New Button On text token shared with click red via `ColourChart::kRed`.
  [`SkinColoursWidgetsControls.h:29`](../../Source/GUI/Skins/SkinColoursWidgetsControls.h#L29)

- Wire `kButtonTextOn` to the new On token (was Off grey).
  [`Skin.cpp:230`](../../Source/GUI/Skins/Skin.cpp#L230)

**Compare lit state**

- Drive Compare toggle from `compareActive` so blink lit phases use On text.
  [`PatchMutatorPanel.cpp:578`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L578)

**Hover during On**

- Keep On text red under hover (review patch after Blind Hunter).
  [`Button.cpp:99`](../../Source/GUI/Widgets/Button.cpp#L99)
