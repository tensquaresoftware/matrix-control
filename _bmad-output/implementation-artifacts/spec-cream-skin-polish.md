---
organization: Ten Square Software
project: Matrix-Control
title: Cream Skin Polish
author: BMad Agent
status: done
baseline_commit: '82f93bb3dfb304764449f6459df5b04bfef0ea6d'
review_loop_iteration: 0
created: '2026-07-17'
updated: '2026-07-17'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Cream skin still uses light header titles on a cream body, light-grey “paper” buttons instead of Black chrome, and a footer that drifts from Black because identity text shares `kLabelText` with body labels while some footer/message and ButtonLike popup colours still diverge.

**Approach:** Remap Cream `creamVariant` values so section/module/group header text matches Cream `LabelText`; copy Black button and ButtonLike (combo + popup) colours into Cream; make Header and Footer chrome paint identically in both skins, including a dedicated footer identity colour so body labels can stay Cream-dark without breaking the dark footer.

## Boundaries & Constraints

**Always:**
- Change only Cream side (`creamVariant`) unless a new skin colour ID is required for footer identity/none text.
- Header titles in scope: `SectionHeader`, `ModuleHeader`, `GroupLabel` text → Cream `LabelText` (`ColourChart::kDarkGrey1`). `ModulationBusHeader` follows via existing `ModuleHeader` colour IDs.
- Keep blue/orange header lines and `GroupLabel` line colours unchanged.
- `Button` and ComboBox/PopupMenu `ButtonLike` Cream states (enabled, hover, clicked, disabled where they currently differ) must equal their Black variants.
- Header and Footer must look identical in Black and Cream (backgrounds, buttons, ButtonLike combos/popups in header, footer messages, footer identity/device text).
- Body panel stays Cream; body `LabelText` stays Cream `kDarkGrey1`.

**Ask First:**
- Any Cream remapping beyond the widgets listed above (Standard ComboBox/PopupMenu, toggles, sliders, LEDs, separators).
- Changing Black variants or introducing a second body-label colour.

**Never:**
- Do not change layout, fonts, typography sizes, or widget geometry.
- Do not restyle the Cream body background or body parameter labels away from `kDarkGrey1`.
- Do not invent a parallel skin system; stay in existing `ColourElement` / `SkinColourId` tables.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Cream headers | Cream skin; section/module/modulation-bus/group titles | Text colour equals Cream `LabelText` (`kDarkGrey1`); lines unchanged | N/A |
| Cream buttons | Cream skin; TextButton states | Each Button ColourElement creamVariant equals blackVariant | N/A |
| Header ButtonLike | Cream skin; MIDI port combos + logo popup | ComboBox + PopupMenu ButtonLike creamVariants equal Black | N/A |
| Footer parity | Same footer content in Black then Cream | Background, identity text, Info/Success/Warning/Error colours match Black exactly | N/A |
| Body labels preserved | Cream skin; module parameter labels | Still `kDarkGrey1`; body background still `kCream` | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinColoursWidgetsLayout.h` -- Section/Module/Group header text; FooterPanel message colours; DarkPanelText for header brand + footer identity
- `Source/GUI/Skins/SkinColoursWidgetsControls.h` -- Button creamVariants → copy Black
- `Source/GUI/Skins/SkinColoursWidgetsSelection.h` -- ComboBox ButtonLike + PopupMenu ButtonLike creamVariants → copy Black
- `Source/GUI/Skins/SkinValues.h` -- add `SkinColourId::kDarkPanelText`
- `Source/GUI/Skins/Skin.cpp` -- wire `kDarkPanelText` into `colours_`
- `Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp` -- identity + severity None/default use `kDarkPanelText`, not `kLabelText`
- `Source/GUI/Looks/LookBuilders.cpp` -- brand label uses `kDarkPanelText`; ModulationBusHeader still shares ModuleHeader IDs
- `Source/GUI/Widgets/Logo.cpp` -- idle brand colour uses `kDarkPanelText`
- `Source/GUI/Skins/SkinColoursPanels.h` -- Header/Footer backgrounds already identical; Body stays Cream (no edit)

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinColoursWidgetsLayout.h` -- Set Cream `SectionHeader`/`ModuleHeader`/`GroupLabel` text to `kDarkGrey1`; align Footer Info/Success cream to Black; add DarkPanelText (both variants `kLightGrey2`)
- [x] `Source/GUI/Skins/SkinValues.h` + `Skin.cpp` + `FooterPanel.cpp` + `Logo.cpp` + `LookBuilders.cpp` -- Register and use `kDarkPanelText` for footer identity/None and header brand (stop using `kLabelText` on dark chrome)
- [x] `Source/GUI/Skins/SkinColoursWidgetsControls.h` -- Copy Button blackVariants into creamVariants wherever they differ
- [x] `Source/GUI/Skins/SkinColoursWidgetsSelection.h` -- Copy ComboBox ButtonLike and PopupMenu ButtonLike blackVariants into creamVariants wherever they differ
- [x] Visual pass -- Build succeeded; manual Cream↔Black check deferred to human review

**Acceptance Criteria:**
- Given Cream skin, when Section/Module/ModulationBus/Group header text paints, then colour equals Cream `LabelText` (`kDarkGrey1`) and underline colours are unchanged.
- Given Cream skin, when any TextButton state paints, then mapped Button colour IDs match Black exactly.
- Given Cream skin, when Header ButtonLike combos or their popups (ports, logo) paint, then ComboBox/PopupMenu ButtonLike colours match Black exactly.
- Given identical footer content, when switching Black↔Cream, then footer background, identity text, and severity message colours are visually and token-identical.
- Given Cream skin, when body parameter labels and body background paint, then they remain `kDarkGrey1` and `kCream` respectively.

## Spec Change Log

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds after colour/ID wiring changes

**Manual checks:**
- Cream: headers/group labels readable dark on cream; buttons look like Black chrome on cream body.
- Cream Header: port combos + logo menu match Black chrome; header field labels readable light on dark chrome.
- Cream Footer vs Black Footer: same background and text colours for identity and each severity.
- Cream body labels still dark grey; body still cream.

## Suggested Review Order

**Dark chrome text token**

- Shared light text for Header/Footer/dialog chrome, decoupled from body labels
  [`SkinColoursWidgetsLayout.h:93`](../../Source/GUI/Skins/SkinColoursWidgetsLayout.h#L93)

- New colour ID registration
  [`SkinValues.h:13`](../../Source/GUI/Skins/SkinValues.h#L13)

- Wire token into skin map
  [`Skin.cpp:191`](../../Source/GUI/Skins/Skin.cpp#L191)

- Look builder for chrome labels
  [`LookBuilders.cpp:62`](../../Source/GUI/Looks/LookBuilders.cpp#L62)

**Header / Footer consumers**

- Header field labels use dark-panel look
  [`HeaderPanel.cpp:69`](../../Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp#L69)

- Footer identity no longer uses body LabelText
  [`FooterPanel.cpp:50`](../../Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp#L50)

- Brand logo idle colour follows dark-panel token
  [`LookBuilders.cpp:70`](../../Source/GUI/Looks/LookBuilders.cpp#L70)

**Cream body titles**

- Section/Module/Group header Cream text → label dark grey
  [`SkinColoursWidgetsLayout.h:11`](../../Source/GUI/Skins/SkinColoursWidgetsLayout.h#L11)

**Black chrome on Cream**

- Button creamVariants equal Black
  [`SkinColoursWidgetsControls.h:11`](../../Source/GUI/Skins/SkinColoursWidgetsControls.h#L11)

- ComboBox / PopupMenu ButtonLike creamVariants equal Black
  [`SkinColoursWidgetsSelection.h:58`](../../Source/GUI/Skins/SkinColoursWidgetsSelection.h#L58)
