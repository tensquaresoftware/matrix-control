---
title: 'Unify ComboBox / Slider / PopupMenu ButtonLike into classic types'
type: 'refactor'
created: '2026-11-01'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '0a565bd7cbcbaf1c84bfadd0af415faf16624f8d'
context:
  - '{project-root}/_local/Documents/Recap-ColourSets-et-refactorisation-widgets.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-colourset-model-and-editor.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-colourset-product-wiring.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** ComboBox, Slider, and PopupMenu still exist twice in Mappings (`*ButtonLike` vs classic). The real differences are ColourSet and construction behaviour, so authors cannot mix “mono” colours with multi-column or scroll without fake object types.

**Approach:** Keep one skin object per widget family. Resolve colours only via ColourSet (product former-ButtonLike sites stay on `k4`). Express popup/scroll/chrome behaviour as construction options. Delete `*ButtonLike` SkinObjectTypes, dual Look fields, and `isButtonLike` colour routing after seeds and Dev keys migrate.

## Decisions (human-approved planning)

- Keep this spec as one full dispatch (do not split Slider vs ComboBox/PopupMenu).
- Replace `ComboBox::Style::{Standard,ButtonLike}` with explicit construction options: popup mode `MultiColumn` | `Scrollable`, plus existing setters (port-sentinel chrome, content-sized width, popup-only mark, vertical placement). Drop the ButtonLike name from the widget API and painters. Colours come only from ColourSet / unified Look slots. Closed-face border metrics that today follow ButtonLike stay tied to `Scrollable` (or an equivalent chrome flag), not to a skin object.

## Boundaries & Constraints

**Always:**
- One Mappings table each for `ComboBox`, `Slider`, `PopupMenu` with ColourSet column; no `*ButtonLike` rows.
- Migrate compiled Black/Cream ButtonLike pastilles onto the classic object at the same ColourSet index (product sites keep `k4`).
- Preserve today’s behaviours: multi-column vs scrollable popup, port-sentinel chrome, content-sized popup width, popup-only mark, first-item / Settings separator chrome, closed-face border thickness quirks where they exist today.
- Preserve ColourSet across `setSkin`; Looks take a single colour slot set filled from the requested ColourSet.
- Skin Editor / DevSkin lists, filters, and JSON key names drop ButtonLike; old Dev JSON remaps on load.
- Widget API has no `Style::ButtonLike` / `ComboBoxControlStyle::ButtonLike`; former sites pass `PopupMode::Scrollable` (and related setters).
- Black/Cream stay visually equivalent after migration; wiring tests keep distinct-pastille asserts.
- Include PopupMenu (combos depend on it). Run `lint_touched.py` on touched C++.

**Never:**
- Put scroll / multi-column / separator / port-sentinel options into skin JSON.
- Keep `Style::ButtonLike` as a colour or skin-object alias.
- ColourSet k5+, Duo final hues, GPC 5 panel/DarkPanel work, zone→set config file.
- Redesign Settings layout/spacing/typography unrelated to this merge.
- Merge unrelated widget types that merely share a ColourSet.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Settings combo | `PopupMode::Scrollable` + `k4` | Classic ComboBox/PopupMenu colours at `k4`; scrollable behaviour unchanged | N/A |
| Patch classic combo | `PopupMode::MultiColumn` + `k1` | Classic colours at `k1`; multi-column unchanged | N/A |
| Hardware Latency | Settings slider + `k4` | `sliderLookFromSkin(..., k4)` matches former SliderButtonLike | N/A |
| Computer Patches | Scrollable + related setters + `k4` | Same scroll/chrome options; colours via ComboBox/PopupMenu `k4` | N/A |
| Multi-col + k4 | MultiColumn + `k4` Look | Allowed (colours from set; layout from construction) | N/A |
| Skin Editor tables | Open Mappings | No `*ButtonLike` object tables | N/A |
| Old Dev JSON | Keys `ComboBoxButtonLike` etc. | Load remaps to classic object + ColourSet; next save writes new keys | Missing set → existing k1 fallback |
| Black/Cream smoke | Compiled seeds | No intentional visual regression | N/A |
| Focus / disabled / scrollbar | Former ButtonLike paths | Same focus, disabled, scrollbar painting via unified Look slots | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h` — remove `kSliderButtonLike` / `kComboBoxButtonLike` / `kPopupMenuButtonLike` from enum + `kMultiSetSkinObjects`.
- `Source/GUI/Skins/SkinColoursWidgetsSelection.h`, `SkinColoursWidgetsControls.h`, `Skin.cpp` — fold ButtonLike ColourElements into classic object rows (per ColourSet); stop seeding ButtonLike namespaces.
- `Source/GUI/Skins/Skin.h`, `ISkinColours.h`, `SkinColourAccessors.cpp` — drop `isButtonLike` parameters; resolve via object + ColourSet only.
- `Source/GUI/Skins/Dev/DevSkinKeyNames.cpp`, Dev element lists, Skin Editor filters/tables (`Tools/SkinEditor/*`, `SkinEditorMappingsColumnFilter.h`) — remove ButtonLike names; load-time remap for old JSON keys.
- `Source/GUI/Looks/WidgetLooks.h`, `LookBuilders.h`, `LookBuilders*.cpp` — single colour slots on ComboBoxLook/PopupMenuLook; delete `sliderLookButtonLikeFromSkin` and `buttonLike*` fields; `*LookFromSkin(skin, colourSet)` only.
- `Source/GUI/Widgets/ComboBox.*`, `ComboBoxControlPainter.*`, `PopupMenuBase.*`, `PopupMenuRenderer.*`, `ScrollablePopupMenu.*`, `MultiColumnPopupMenu.*` — remove `Style`/`ComboBoxControlStyle::ButtonLike`; add `PopupMode::{MultiColumn,Scrollable}` (name may vary slightly); colours from unified Look; scroll vs multi-column and closed-face chrome from construction.
- Call sites: `SettingsPanelSetup.cpp`, `SettingsMidiPage.cpp`, `SettingsAudioPage.cpp`, `ComputerPatchesPanel.cpp`, `GettingStartedWizardDialogControls.cpp`, `EpromTypePromptDialog.cpp`, `HeaderLogoPopupMenu.cpp` — classic Look builders + ColourSet (`k4` where today ButtonLike); migrate `Style::ButtonLike` → `PopupMode::Scrollable` (+ setters).
- Tests: `SliderButtonLikeColourTests.cpp` (rename/retarget), `SkinColourSubstrateTests.cpp`, `SkinColourWiringTests.cpp` / Coverage / Support, ComboBox popup tests, `NumberBoxDisabledBackgroundColourTests.cpp`, `Source/GUI/Tests/TestComboBoxes.cpp` — assert classic object + ColourSet; no ButtonLike types or Style enum.
- Do not change: zone→set map from GPC 3, Peak≠Led, panel/DarkPanel names, Duo hue seeds.

## Tasks & Acceptance

**Execution:**
- [x] Skin model + seeds — remove `*ButtonLike` types; migrate Black/Cream pastilles onto classic objects per ColourSet; drop `isButtonLike` accessors.
- [x] ComboBox API + painters/popups — replace Style with `PopupMode` (+ existing setters); unified Look slots; delete dual ButtonLike Look paths and `sliderLookButtonLikeFromSkin`.
- [x] Product call sites + Header logo popup — classic builders with existing ColourSets; migrate former ButtonLike sites to Scrollable mode.
- [x] DevSkin + Skin Editor — lists/filters/JSON keys; load remap for legacy ButtonLike keys.
- [x] Unit/GUI tests + CMake — retarget ButtonLike/Style tests; distinct-pastille wiring still green; `lint_touched.py` on touched C++.

**Acceptance Criteria:**
- Given Skin Editor Mappings, when listing objects, then no `SliderButtonLike` / `ComboBoxButtonLike` / `PopupMenuButtonLike` tables exist.
- Given Settings / Computer Patches / dialogs former-ButtonLike combos, when shown, then colours resolve from `ComboBox`/`PopupMenu` at `k4` and scroll/chrome behaviour matches today via `PopupMode::Scrollable` (no Style::ButtonLike).
- Given Patch classic multi-column combo at `k1`, when opened, then layout and colours match today’s classic path via `PopupMode::MultiColumn`.
- Given Hardware Latency slider, when Look builds with `k4`, then colours match today’s SliderButtonLike.
- Given a Look built once for a ColourSet, when painting closed combo and popup (focus, disabled, scrollbar), then only unified Look slots are used (no `buttonLike*` fields).
- Given source under `Source/GUI/Widgets`, when searching ButtonLike style symbols, then `ComboBox::Style` and `ComboBoxControlStyle::ButtonLike` are gone.
- Given old Dev JSON with `*ButtonLike` keys, when loaded, then mappings attach to classic objects without data loss of ARGB/ColourSet.
- Given Black or Cream compiled skin, when smoking Settings + Patch combos/sliders, then no intentional visual regression.
- Given distinct pastilles per ColourSet in wiring tests, when asserting zones, then former-ButtonLike sites still read `k4` on classic objects.
- Given `python3 Scripts/quality/lint_touched.py` on touched C++, then it passes.

## Implementation Notes

- Removed `*ButtonLike` SkinObjectTypes; former ButtonLike ARGB overlays classic Slider/ComboBox/PopupMenu at ColourSet `k4` only (k1–k3 keep classic hues).
- ComboBox API: `PopupMode::{MultiColumn,Scrollable}`; closed-face thick border via `ComboBoxClosedChrome::AlwaysThickBorder` when Scrollable. No Style/ButtonLike in widgets.
- Looks: single colour slots; deleted `sliderLookButtonLikeFromSkin` and dual `buttonLike*` fields; product former-ButtonLike sites use classic builders + `k4`.
- DevSkin load remaps legacy `*ButtonLike` object key strings to classic types; Skin Editor filter/lists have no ButtonLike tables.
- Tests: replaced `SliderButtonLikeColourTests` with `SliderColourSetK4Tests`; ColourSetModel legacy remap + wiring coverage Black/Cream k4 carry; lint_touched OK.
- Verified suites green: SliderColourSetK4, ColourSetModel, SkinColourWiring/Coverage, SkinColourSubstrate, NumberBoxDisabledBackgroundColour, ComboBox popup suites, SkinEditorMappingsColumnFilter.
- Manual Black/Cream glance of Settings / Computer Patches / Patch SYNC·TRACK INPUT still recommended (seeds designed for visual parity).
- Review patches: legacy *ButtonLike JSON force ColourSet k4; stronger Combo/Popup k4 + DevSkin compiled overlay asserts; dialog `popupMode` rename; filter test title nine.

## Spec Change Log

## Review Triage Log

- Blind Hunter: legacy *ButtonLike remap leaves colourSet unchanged (can overwrite classic k1–k3) — medium — verified DevSkinKeyNames/mappingFromVar; route patch (force k4 on legacy object names).
- Blind Hunter: overlayFormerButtonLikeAtK4 duplicated Skin.cpp vs DevSkinSsotSeed — low — real duplication; factoring adds shared surface beyond trivial patch — defer.
- Blind Hunter: only SliderK4 fully pinned; ComboBox/PopupMenu k4 slots under-asserted — medium — verified assertBlackCreamSeedLooksAfterUnify bg/text only — patch.
- Blind Hunter: wiring coverage never asserts PopupMenu k4 paint beyond bg/text — medium — same root as Combo/Popup k4 asserts — grouped patch.
- Blind Hunter: ColourSetModel remap test only ComboBoxButtonLike+k4 — medium — same root as legacy remap — grouped patch.
- Blind Hunter: Scrollable closed face ignores focusBorder / unused ComboBoxK4::kFocusBorder for Settings — false — AlwaysThickBorder is intentional per frozen PopupMode decision; FocusBorder still used by MultiColumn+k4.
- Blind Hunter: dialog locals still named comboStyle — low — verified Eprom/GettingStarted — patch rename to popupMode.
- Blind Hunter: spec status in-progress vs tasks done — false — status is in-review at triage time; diff snapshot stale.
- Blind Hunter: empty Spec Change Log / Review Triage Log — false — reject (asks to edit this build's process sections as a finding).
- Blind Hunter: PopupMode couples layout and closed chrome — false — frozen Decisions explicitly tie thick border to Scrollable.
- Blind Hunter: paintSettingsZone helper omits PopupMenu k4 retints — medium — same root as PopupMenu k4 coverage — grouped patch.
- Blind Hunter: TestComboBoxes lacks PopupMode behaviour asserts — low — reject (GUI harness smoke; construction path covered by call sites + unit looks).
- Edge Case: legacy *ButtonLike with colourSet not k4 — medium — verified — same root as Blind remap — grouped patch.
- Edge Case: classic + *ButtonLike same slot last-write-wins — medium — verified map insert; force-k4 mitigates k1 wipe; residual same-key collision at k4 deferred.
- Edge Case claim: load without ARGB/ColourSet loss falsified by collapse — medium — same remap root — grouped patch.
- Verification Gap: ComboBox/PopupMenu k4 overlay slots beyond bg/text unpinned — medium — pre-verified — patch.
- Verification Gap: HeaderLogoPopupMenu k4 has no consumer test — medium — defer (matches GPC 3 pattern: Look builders + seed asserts, no panel/menu mounts; one-liner call site).
- Verification Gap: DevSkin compiled-seed overlay never executed by tests — medium — pre-verified makeCompiledSkin path — patch.
- Verification Gap Other: SkinEditorMappingsColumnFilterTests title still says twelve — low — patch rename to nine.

## Design Notes

- Today `Style::ButtonLike` couples (1) alternate colour object, (2) scrollable popup, (3) closed-face border metrics. After merge, (1) becomes ColourSet only; (2)+(3) become `PopupMode::Scrollable` (and related setters). PopupMenu must merge in the same change because ScrollablePopupMenu reads ButtonLike Look slots.
- Seed rule: for each ButtonLike ColourElement, write its Black/Cream ARGB into the classic object at the same ColourSet; then delete ButtonLike elements. Product wiring already passes `k4` at former-ButtonLike sites — keep that.
- Construction inventory to preserve: `PopupMode::{MultiColumn,Scrollable}`, `setUsesPortSentinelPopupChrome`, `setUsesContentSizedPopupWidth`, `setPopupOnlyMarkedItem`, `setPopupVerticalPlacement`, popup layout dimensions, first-item separator / port-sentinel row chrome. Completing any missed flag is in-scope at implement time.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-ControlTests` (or project unit-test target) -- expected: touched colour/combo/slider tests green
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched `Source/`, `Tests/`, `Tools/SkinEditor/` C++

**Manual checks (if no CLI):**
- Black + Cream: Settings combos/latency, Computer Patches combo, Patch SYNC / TRACK INPUT popups — colours and scroll/multi-column behaviour look unchanged.
