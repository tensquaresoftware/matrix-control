---
title: 'Plugin HARDWARE LATENCY slider monochrome greys'
type: 'feature'
created: '2026-10-06'
status: 'done'
route: 'dispatch'
baseline_commit: '22af806169dd75bdaa653d05186d6e92df71d38c'
review_loop_iteration: 0
context:
  - '{project-root}/Source/GUI/Skins/ColourChart.h'
  - '{project-root}/Source/GUI/Skins/SkinColoursWidgetsSelection.h'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In the plugin Settings DEVICE tab, HARDWARE LATENCY still uses the shared green slider look. That green accent will stop being the Matrix-Control default once Dual-tone skins introduce blue Patch and orange Master sliders, so a Settings control should already match the monochrome Settings combo language.

**Approach:** Keep the shared green slider chart for Patch / Master / header controls. Add a ButtonLike-style monochrome colour set for sliders (existing ColourChart greys only — no new greys), wire a look builder parallel to ComboBox ButtonLike, and apply it only to the plugin HARDWARE LATENCY slider.

**Decisions:**
- Monochrome enabled mapping (ButtonLike-aligned): track=`kBlack`, focus=`kDarkGrey3`, value bar=`kDarkGrey5`, text=`kLightGrey2`. Disabled roles keep shared slider disabled tokens / ButtonLike-disabled where a role is missing. No new ColourChart greys.

## Boundaries & Constraints

**Always:**
- Reuse existing `ColourChart` greys (same family as Settings ComboBox / PopupMenu ButtonLike).
- HARDWARE LATENCY remains plugin-only; standalone DEVICE layout unchanged.
- Green `Widgets::Slider` chart stays the default for all other sliders.

**Never:**
- Do not retint the shared green slider SkinColourIds globally.
- Do not add new grey tokens to `ColourChart`.
- Do not restyle Patch, Master, input-gain, or other non-Settings sliders in this change.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Plugin DEVICE open | HARDWARE LATENCY visible | Slider track / fill / value text / focus border use monochrome greys | N/A |
| Skin change Black/Cream | Open Settings DEVICE in plugin | Latency slider stays monochrome; other sliders stay green | N/A |
| Standalone DEVICE | No HARDWARE LATENCY row | No new slider; no colour path exercised | N/A |
| Disabled latency slider | Control disabled (if reachable) | Disabled greys / common disabled tokens still readable | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/ColourChart.h` — existing greys only; do not extend.
- `Source/GUI/Skins/SkinColoursWidgetsControls.h` — `Widgets::Slider` green chart (leave); add nested `Slider::ButtonLike` (or equivalent) grey `ColourElement`s.
- `Source/GUI/Skins/SkinColoursWidgetsSelection.h` — reference ComboBox/PopupMenu `ButtonLike` token roles for parity.
- `Source/GUI/Skins/SkinValues.h` / `Skin.cpp` / `Skin.h` / `ISkin` as needed — register ButtonLike slider colour IDs and load them in `initializeSliderColours`.
- `Source/GUI/Looks/LookBuilders.h` / `LookBuilders.cpp` — keep `sliderLookFromSkin` green; add `sliderLookButtonLikeFromSkin` that fills `SliderLook` from ButtonLike IDs; editor text/caret stay high-contrast on dark plate.
- `Source/GUI/Settings/SettingsPanelSetup.cpp` — create + skin-refresh HARDWARE LATENCY with the ButtonLike slider look only.
- Do not change: `ParameterCell`, `ModulationBusCell`, `HeaderPanel` input gain, `WidgetFactory` default slider path.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinColoursWidgetsControls.h` (+ SkinValues / Skin init as needed) -- Add Slider ButtonLike grey chart and SkinColourIds using existing greys -- SSOT for monochrome slider colours.
- [x] `Source/GUI/Looks/LookBuilders.cpp` -- Add ButtonLike slider look builder; leave green builder untouched -- callers opt in explicitly.
- [x] `Source/GUI/Settings/SettingsPanelSetup.cpp` -- Wire HARDWARE LATENCY create + skin refresh to ButtonLike look -- plugin Settings only.
- [x] `Tests/Unit/SliderButtonLikeColourTests.cpp` (+ CMakeLists) -- Pin ButtonLike greys, green defaults, plugin-only latency gate -- I/O matrix coverage.
- [x] Quality gate -- `python3 Scripts/quality/lint_touched.py` on touched C++ -- project DoD.

**Acceptance Criteria:**
- Given plugin Settings DEVICE, when HARDWARE LATENCY is shown, then its enabled colours are monochrome greys from the approved mapping, not green.
- Given any other slider (Patch, Master, header input gain), when skin is Black or Cream, then greens remain unchanged.
- Given ColourChart, when this change lands, then no new grey constants were added.

## Implementation Notes

- Added `Widgets::Slider::ButtonLike` chart (kBlack / kDarkGrey3 / kDarkGrey5 / kLightGrey2) + four SkinColourIds; green chart untouched.
- `sliderLookButtonLikeFromSkin` reuses shared slider disabled tokens; editor text/caret stay NumberBox white.
- HARDWARE LATENCY create + skin refresh use ButtonLike look only (`SettingsPanelSetup.cpp`).
- Unit test `SliderButtonLikeColourTests` pins chart greys, green defaults, shared disabled token, and Black/Cream `Skin` → look mapping.
- Test target links `Skin.cpp` / `LookBuilders.cpp` / `TypographyStyles.cpp` + `PluginFonts` for look asserts.
- Lint + `SliderButtonLikeColour` unit tests OK; SettingsPanel look wiring left as manual / deferred.

## Spec Change Log

## Review Triage Log

- Blind: test source missing from first review diff package — verdict: `false` — file existed on disk; packaging used tracked-only diff; later intent-add includes it.
- Blind: Verification section lists lint only — verdict: `false` / rejected — fix would only edit this build's spec; commands updated at finalize for accuracy.
- Blind: Code Map still says look-builder name TBD — verdict: `false` / rejected — non-frozen wording only; name settled in code as `sliderLookButtonLikeFromSkin`.
- Blind: Frozen text mentions ButtonLike-disabled where a role is missing, but no ButtonLike disabled chart — verdict: `false` — all `SliderLook` disabled roles are filled from shared slider disabled tokens; no missing role.
- Blind: chart test did not assert Skin/look mapping — verdict: `medium` → `patch` — extended test with Black/Cream Skin + `sliderLookButtonLikeFromSkin` asserts.
- Blind: frontmatter context still cites Selection.h — verdict: `false` / rejected — spec-only index; Code Map already points at Controls.h.
- Blind: look-builder comment overstated “green” disabled tokens — verdict: `low` → `patch` — comment rephrased to shared slider disabled tokens.
- Blind: empty Spec Change Log / Review Triage Log headings — verdict: `false` — empty until first review pass by design; this log fills triage.
- Edge Case Hunter: no findings.
- Verification Gap: HARDWARE LATENCY SettingsPanel wiring untested — verdict: `medium` → `defer` — GUI panel construction outside unit-test style; manual Settings DEVICE smoke.
- Verification Gap: Skin load + look builder untested — verdict: `medium` → `patch` — same Skin/look asserts as Blind patch.
- Verification Gap: `hardwareLatency_pluginOnly` only rechecked layout helper — verdict: `low` → `patch` — case removed; metrics stay in SettingsTabsShellTests.

## Design Notes

Green slider roles today: track `kGreen1`, focus `kGreen2`, value bar `kGreen3`, text `kGreen4`.
Approved grey mirror (aligned with ComboBox ButtonLike plate / border / triangle / text):
track → `kBlack`, focus → `kDarkGrey3`, value bar → `kDarkGrey5`, text → `kLightGrey2`.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: exit 0 on touched C++
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` then `Matrix-Control_Tests --test SliderButtonLikeColour` -- expected: 0 failures

**Manual checks (if no CLI):**
- Plugin: Settings > DEVICE > HARDWARE LATENCY looks monochrome next to ButtonLike combos; Patch/Master sliders still green. Standalone: no latency row.
