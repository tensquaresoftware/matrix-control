---
title: 'Unify disabled control look (buttons, toggles, sliders, comboboxes)'
type: 'feature'
created: '2026-07-17'
status: 'done'
baseline_commit: '872ac447388b0b0edde7b8ef96872d4e9c2bfcfe'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Disabled (grayed) controls do not share one visual language. Buttons such as PASTE and COMPARE look lighter/uglier than others because disabled skin colours and alpha fades stack, and toggles barely honor disabled colours at all.

**Approach:** Adopt one skin-token convention for disabled buttons/toggles (including ButtonLike comboboxes), sliders, and standard comboboxes, paint from those tokens, and stop stacking alpha gray on controls that already use disabled/inactive paint.

## Boundaries & Constraints

**Always:**
- Buttons and toggles (and ComboBox `Style::ButtonLike`): disabled border = same colour as that control’s enabled border; background = `ColourChart::kDarkGrey2`; text (and ButtonLike arrow) = `ColourChart::kDarkGrey4`.
- Sliders: disabled background/fond = `kDarkGrey3`; track = `kDarkGrey2`; value bar and value text = `kDarkGrey4`. Keep showing the value bar when disabled (today it is suppressed).
- Standard comboboxes (including HierarchicalComboBox closed state): disabled background = `kDarkGrey3`; text and arrow = `kDarkGrey4`.
- `inactiveAppearance` must paint with the same disabled convention as `!isEnabled()`.
- When a control already paints via disabled/inactive skin colours, do not also apply `GrayedControlHelper::applyGrayedAppearance` (or equivalent `setAlpha` gray) on that same control — tokens alone define the look.
- Keep cream/black skin variants coherent: disabled greys are shared chart tokens (same on both variants unless an existing enabled border already differs by variant).

**Ask First:**
- Changing Compare-mode whole-panel dimming (`CompareLockBinder` alpha on entire panels) beyond removing per-control double-gray.
- Changing Bank Utility bank buttons that gray via alpha while staying `setEnabled(true)` and never use disabled paint — only if required for visual parity.

**Never:**
- Redesign enabled (active) colours, hover/click states, or green on/off toggle enabled look.
- Change enablement logic (when PASTE/COMPARE/etc. become interactive).
- French UI strings in source; new IDs/names stay English.
- Popup menu open-list styling, NumberBox, displays, LEDs, About/Settings chrome.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Button disabled / inactive | `!isEnabled()` or `inactiveAppearance` | BG Grey2, border = enabled border, text Grey4; no extra alpha | N/A |
| Toggle disabled | `!isEnabled()` | Same button/toggle convention (not green + alpha only) | N/A |
| ButtonLike combo disabled | Computer Patches / Header ButtonLike, disabled | Same as disabled button (BG Grey2, enabled border, text+arrow Grey4) | N/A |
| Standard combo disabled | History / green-style combo disabled | BG Grey3, text+arrow Grey4 | N/A |
| Slider disabled | Parameter/mutator slider disabled | Fond Grey3, track Grey2, value bar + text Grey4; value bar still drawn | N/A |
| No double gray | PASTE inactive or Mutator lock using disabled paint | Skin colours only — not skin + 0.5 alpha | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/ColourChart.h` -- `kDarkGrey2`…`kDarkGrey4` palette SSOT
- `Source/GUI/Skins/SkinColoursCommon.h` -- shared disabled defaults (today Grey3/4/5); update or stop using where per-widget convention differs
- `Source/GUI/Skins/SkinColoursWidgetsControls.h` -- Button/Slider/Toggle disabled ColourElements
- `Source/GUI/Skins/SkinColoursWidgetsSelection.h` -- Combo Standard + ButtonLike disabled ColourElements
- `Source/GUI/Skins/SkinValues.h` / `Skin.cpp` / `ISkinColours.h` -- IDs, init, getters (add slider background/track split if needed)
- `Source/GUI/Looks/WidgetLooks.h` / `LookBuilders.cpp` -- `ButtonLook`, `SliderLook`, `ToggleLook`, `ComboBoxLook` wiring
- `Source/GUI/Widgets/Button.cpp` -- disabled/inactive paint; border must match enabled
- `Source/GUI/Widgets/Toggle.cpp` -- add disabled paint path (today ignores `isEnabled()`)
- `Source/GUI/Widgets/Slider.cpp` -- track/value layers when disabled; stop suppressing value bar
- `Source/GUI/Widgets/ComboBoxControlPainter.cpp` -- Standard vs ButtonLike closed-state colours
- `Source/GUI/Helpers/GrayedControlHelper.*` -- alpha helper; call sites that double-gray
- Call sites to reconcile (no double alpha): e.g. ModuleHeader PASTE, MatrixModulationPanel PASTE, PatchMutatorPanel lock/COMPARE paths, Header keyboard combo if applicable

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinColoursWidgetsControls.h` (+ Common if still shared) -- set Button/Toggle/Slider disabled tokens to the new greys; Toggle gains disabled BG/text/border elements -- convention SSOT
- [x] `Source/GUI/Skins/SkinColoursWidgetsSelection.h` -- Standard combo text/triangle → Grey4; ButtonLike → button convention (BG Grey2, border = enabled border token, text/triangle Grey4)
- [x] `Source/GUI/Skins/SkinValues.h`, `Skin.cpp`, `ISkinColours.h` as needed -- wire any new slider/toggle colour IDs and getters
- [x] `Source/GUI/Looks/WidgetLooks.h`, `LookBuilders.cpp` -- map new colours into looks (ToggleLook disabled fields; SliderLook background vs track if split)
- [x] `Source/GUI/Widgets/Button.cpp` -- disabled border uses enabled border colour (token or paint path)
- [x] `Source/GUI/Widgets/Toggle.cpp` -- paint disabled state from ToggleLook
- [x] `Source/GUI/Widgets/Slider.cpp` -- disabled fond/track/value layers; draw value bar when disabled
- [x] `Source/GUI/Widgets/ComboBoxControlPainter.cpp` -- ButtonLike disabled border = enabled border colour
- [x] Grayed call sites (ModuleHeader, MatrixModulationPanel, PatchMutatorPanel, others found in implement) -- remove alpha overlay when control already uses disabled/inactive paint
- [ ] Manual GUI check (TestButtons / TestToggles / TestComboBoxes / Patch Mutator PASTE+COMPARE + Computer Patches combo) -- confirm homogeneity

**Acceptance Criteria:**
- Given a disabled or inactive text button (including PASTE), when painted, then background is `kDarkGrey2`, text is `kDarkGrey4`, border matches the enabled border, and component alpha is fully opaque.
- Given a disabled toggle, when painted, then it uses the same border/BG/text convention as disabled buttons (not green washed with alpha).
- Given a disabled ButtonLike combobox, when closed-state painted, then it matches the disabled button convention.
- Given a disabled standard combobox, when closed-state painted, then background is `kDarkGrey3` and text/arrow are `kDarkGrey4`.
- Given a disabled slider, when painted, then fond is `kDarkGrey3`, track is `kDarkGrey2`, value bar and value text are `kDarkGrey4`, and the value bar remains visible at the current value.
- Given Compare/Mutator lock that disables controls via skin paint, when applied, then those controls are not additionally alpha-faded.

## Spec Change Log

## Design Notes

**Slider layer mapping (enabled → disabled):** Today `drawTrack` fills the full bounds (`track*`) and `drawValueBar` fills the inset value. Disabled must present three greys: paint an outer fond (`kDarkGrey3`), an inset full-width track rail (`kDarkGrey2`), then the value fill (`kDarkGrey4`) plus text (`kDarkGrey4`). Prefer extending `SliderLook` / skin IDs over overloading one colour for two layers.

**Border = enabled:** Prefer pointing disabled border ColourElements at the same chart values as enabled border (Button / ButtonLike / Toggle), or have paint select `borderOff`/`borderOn` when disabled — avoid a separate greyer disabled border token.

**Alpha policy:** `GrayedControlHelper` remains valid for controls that stay enabled and only look unavailable without disabled paint (Ask First: Bank Utility). Do not leave PASTE (`inactiveAppearance` + alpha) or Mutator locked controls on both paths.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- Run unit tests for the preset if a fast target exists for GUI-unrelated regressions -- expected: pass (no new colour unit tests required unless a tiny Look token test is cheap)

**Manual checks:**
- Patch Mutator: disabled MUTATE/RETRY/DELETE/COMPARE/PASTE-like actions and locked sliders/toggles/history combo — one grey language, no washed-out double fade
- Computer Patches ButtonLike file combo when disabled/grayed
- Module header PASTE when paste disabled vs neighboring INIT/COPY
- Optional: GUI test harness buttons/toggles/combos forced disabled

## Suggested Review Order

**Skin tokens (convention SSOT)**

- Shared disabled greys: content Grey4, decoration/track Grey2
  [`SkinColoursCommon.h:22`](../../Source/GUI/Skins/SkinColoursCommon.h#L22)

- Button/toggle/slider disabled ColourElements
  [`SkinColoursWidgetsControls.h:53`](../../Source/GUI/Skins/SkinColoursWidgetsControls.h#L53)

- Standard + ButtonLike combo disabled tokens
  [`SkinColoursWidgetsSelection.h:37`](../../Source/GUI/Skins/SkinColoursWidgetsSelection.h#L37)

**Paint paths**

- Button disabled border uses enabled border colours
  [`Button.cpp:83`](../../Source/GUI/Widgets/Button.cpp#L83)

- Toggle now paints a real disabled state
  [`Toggle.cpp:44`](../../Source/GUI/Widgets/Toggle.cpp#L44)

- Slider: fond + rail + value bar when disabled
  [`Slider.cpp:78`](../../Source/GUI/Widgets/Slider.cpp#L78)

- ButtonLike combo border matches enabled border
  [`ComboBoxControlPainter.cpp:46`](../../Source/GUI/Widgets/ComboBoxControlPainter.cpp#L46)

**No double gray (alpha removal)**

- PASTE: inactiveAppearance only (no alpha stack)
  [`ModuleHeader.cpp:61`](../../Source/GUI/Widgets/ModuleHeader.cpp#L61)

- Mutator Compare lock: `setEnabled` only; blink alpha kept
  [`PatchMutatorPanel.cpp:601`](../../Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp#L601)

- Header plugin keyboard combo: disabled paint without alpha
  [`HeaderPanel.cpp:431`](../../Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp#L431)
