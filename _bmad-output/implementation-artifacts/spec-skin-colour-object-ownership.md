---
title: 'Skin colours — each object owns its painted tokens'
type: 'refactor'
created: '2026-11-10'
status: 'done'
route: 'dispatch'
review_loop_iteration: 1
context: []
baseline_commit: '24a1cb1847adfb2cdb904774b606411154c41c63'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Several Matrix-Control paints still read another Skin Editor object's pastille (ModuleHeader←NumberBox Focus, Slider editors←NumberBox, NumberBox borders←Button, plus Settings/Dialog/About/Footer/Modulation/Ramp borrows). Authors changing one mapping get surprise colour changes elsewhere.

**Approach:** Give each painted owner its own Skin Editor properties with generic reusable names (Focus, Hover, Emphasis, EditorText, Caret, Border, Error, …), wire LookBuilders/paint to those keys only, seed defaults to keep today's look, and stop wrong-owner cross-object `getColour`. Keep BodyPanel / ChromeText / HorizontalSeparator as intentional shared chrome SSOT.

**Decisions (2026-11-10):**
- Keep the full ownership pass (widget LookBuilders + Settings tabs, Dialog chrome that wrongly borrows Button, About link hover, Footer sticky close, ModulationBus highlights, Ramp “M” badge, ProgressBar phase labels). Spec size accepted above the soft 1600-token comfort band.
- Shared chrome stays: dialogs/settings may keep reading BodyPanel, ChromeText, and HorizontalSeparator. Do not duplicate those into every dialog object.
- Alert button hover/clicked label ink owns a dedicated Button pastille (`Button / SecondaryText / Hover`), seeded from the former ChromeText Text Hover (White / White / Cafe900). Do not reuse `Button / Text / Hover` (Cream Black) for alert hover — that would change Cream panic hover text. Idle alert fill/text stay `Button / Background|Text / Error`.

## Boundaries & Constraints

**Always:**
- Keep generic state/property vocabulary (`Focus`, `Hover`, `Emphasis`, `Disabled`, `EditorText`, `Caret`, `Error`).
- Seed new ColourElements so Black / BlackDuo / Cream look unchanged until an author remaps.
- Register every new ColourElement in `DevSkinColourElementList`.
- Update unit tests that assert the old borrows.
- Prefer existing `SkinColourProperty` / `SkinColourState` enums; add enum values only when unavoidable.

**Never:**
- Invent bank-/module-specific property names (`CurrentBank`, …).
- Change widget geometry, gestures, or mapping UX beyond colour ownership.
- Force every dialog to own Background/Text when BodyPanel/ChromeText is the agreed shared chrome.
- Auto-rewrite Dropbox Trials JSON beyond Product seed regeneration for new keys (Trials stay author-owned).
- Force-translate product labels.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Remap ModuleHeader Text/Hover only | Change that pastille | Clickable module titles follow; NumberBox Focus unchanged | N/A |
| Remap NumberBox Focus only | Change NumberBox Text/Focus | NumberBox focus ink changes; ModuleHeader titles do not | N/A |
| Remap NumberBox Caret only | Change NumberBox Caret | NumberBox caret only; Slider caret unchanged | N/A |
| Remap Slider Caret only | Change Slider Caret (MCS sets) | Slider edit caret only | N/A |
| Remap SliderK4 Caret/EditorText only | Change colour-set-4 Slider editor tokens | Dialog/modales zone sliders follow; Patch zone unchanged | N/A |
| Remap Button Border only | Change Button Border Off/On | Buttons only; NumberBox borders unchanged | N/A |
| Remap Dialog selected-tab tokens | Change Dialog owned selected-tab fill/text | Settings tabs follow; Button Text Off unchanged | N/A |
| Remap About link hover token | Change ChromeText Emphasis | About links follow; ModuleHeader LineOrange unused by About | N/A |
| Remap Button SecondaryText Hover only | Change that pastille | Alert hover/clicked text follows; normal Button Text Hover unchanged | N/A |
| Remap Button Text Hover only | Change normal button hover text | Normal buttons follow; alert hover text unchanged | N/A |
| Shared chrome remap | Change BodyPanel / ChromeText / HorizontalSeparator | Dialogs/settings that legitimately use them still follow | N/A |
| Legacy Editor JSON | Missing new keys | Unbound inventory rows; runtime uses compiled seeds until mapped | No crash |
| Alert Button | Panic/alert look | Button Error fill/text + Button SecondaryText Hover for hover/clicked text; not FooterMessage / ChromeText | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Looks/LookBuilders.cpp` -- ModuleHeader←NumberBox Text/Focus → ModuleHeader Text/Hover; add ModuleHeader Background/Text Emphasis for Ramp badge.
- `Source/GUI/Looks/LookBuildersMultiSet.cpp` -- Slider←NumberBox editors → Slider EditorText/Caret per colour set; NumberBox←Button borders → NumberBox Border Default/Disabled; Button alert←FooterMessage/ChromeText → Button Background/Text Error + Button SecondaryText Hover (explicit textHover/textClicked; do not inherit Button Text Hover).
- `Source/GUI/Looks/WidgetLooks.h` -- extend NumberBox/Slider/ModuleHeader looks with new fields.
- `Source/GUI/Skins/SkinColoursWidgetsLayout.h` -- ModuleHeader Text/Hover + Background/Text Emphasis; ChromeText Text Emphasis; FooterMessage Glyph Hover; Dialog Text Default/On; Label Background Clicked/Hover.
- `Source/GUI/Skins/SkinColoursWidgetsControls.h` -- Button Background/Text Error; Button SecondaryText Hover (alert hover ink); Slider EditorText/Caret on Slider + SliderK2/K3/K4; ProgressBar SecondaryText (+ Disabled).
- `Source/GUI/Skins/SkinColoursWidgetsDisplays.h` -- NumberBox Border Default/Disabled.
- `Source/GUI/Skins/SkinColoursPanels.h` -- Dialog Text tokens if housed with panels.
- `Source/GUI/Skins/Dev/DevSkinColourElementList.cpp` -- register all new elements (24 with SecondaryText Hover).
- `Source/GUI/Widgets/NumberBox.cpp`, `ModuleHeaderTitleClick.cpp`, `ModuleHeaderTitleColour.h` -- consume owner looks.
- `Source/GUI/Settings/SettingsTabRail.cpp` -- selected tab text → Dialog Text On (not Button Text Off); TitleBand may stay as selected fill.
- `Source/GUI/Dialogs/DialogMatrixHelpers.cpp` -- title text → Dialog Text; leave HeaderPanel Background content fill unchanged (deferred pre-existing borrow).
- `Source/GUI/About/AboutPanel.cpp` -- link hover → ChromeText Text Emphasis (orange seed).
- `Source/GUI/Widgets/ModulationBusCell.cpp` -- drag/drop highlights → Label Background Clicked / Hover.
- `Source/GUI/Panels/.../RampPortamentoPanel.cpp` -- “M” badge → ModuleHeader Background/Text Emphasis.
- `Source/GUI/Dialogs/BankTransferProgressDialog.cpp` -- phase labels → ProgressBar SecondaryText (+ Disabled), not ProgressBar Text (look-unchanged).
- `Source/GUI/Panels/.../FooterPanelStickyMessage.cpp` -- sticky close hover → FooterMessage Glyph Hover.
- `Tests/Unit/SkinColourWiringCoverageTests.cpp` -- owner seed parity; independence remaps including SliderK4 editor tokens vs Patch; alert SecondaryText Hover vs Button Text Hover; rename overclaiming About/badge test + assert textEmphasis seed.
- `Tests/Unit/SliderZoneSeedTests.cpp`, `NumberBoxDisabledBackgroundColourTests.cpp`, `SkinColourSubstrateTests.cpp`, `DevSkinMappingLinkGroupsTests.cpp` -- update borrow asserts.

**Reuse:** existing enums only (SecondaryText + Hover already exist); seed ARGBs from current borrow sources (alert hover ← ChromeText Text Hover).

**Do not change:** mapping gestures, DevSkin schema version, zone→colourSet matrix, Button Text/Emphasis + ComboBox Text/Focus already shipped, BodyPanel/ChromeText/HorizontalSeparator as shared chrome readers, HeaderPanel dialog content fill (deferred).

## Tasks & Acceptance

**Execution:**
- [x] `SkinColours*.h` + `DevSkinColourElementList.cpp` -- add owner ColourElements for full graph (except shared chrome) including `Button / SecondaryText / Hover`; seed from current borrows.
- [x] `LookBuilders*.cpp` / `WidgetLooks.h` -- wire owner keys; alert look sets textHover/textClicked from SecondaryText Hover; remove wrong-owner `getColour`.
- [x] Settings / Dialog / About / Footer sticky / ModulationBus / Ramp / ProgressBar phase sites -- consume owner or agreed shared-chrome keys only.
- [x] Unit tests -- owner-key asserts + independence checks (include SliderK4 editor remap vs Patch; alert hover vs Button Text Hover; ModuleHeader emphasis seed after LineOrange remap).
- [x] Product skins: rely on compiled `makeCompiledSkin` (no Dropbox Product JSON rewrite); leave Trials author-owned.

**Acceptance Criteria:**
- Given remap of NumberBox Text/Focus alone, when Push, then ModuleHeader clickable titles do not change.
- Given remap of NumberBox Caret alone, then Slider edit caret does not change.
- Given remap of SliderK4 Caret or EditorText alone, then modales/dialog zone slider editors follow and Patch zone does not.
- Given remap of Button Border Off/On alone, then NumberBox borders do not change.
- Given remap of Button Text Off alone, then Settings selected-tab text and Dialog overlay titles do not change.
- Given remap of ModuleHeader LineOrange alone, then About link hover and Ramp badge emphasis colours do not change.
- Given remap of Button SecondaryText Hover alone, then alert hover/clicked text follows and normal Button Text Hover does not.
- Given remap of Button Text Hover alone, then normal button hover text follows and alert hover text does not.
- Given remap of BodyPanel or ChromeText, then legitimate dialog/settings chrome still follows (shared SSOT kept).
- Given compiled Black/Cream with no Editor remaps, when plugin loads, then in-scope visuals match prior look (including Cream alert hover Cafe900).
- Given listed unit tests, when run, then 0 failures and no asserts of removed borrows.

## Implementation Notes

- 24 new ColourElements registered; seeds match former borrow sources (parity asserted for Black / BlackDuo / Cream).
- Alert hover/clicked text reads `Button / SecondaryText / Hover` (White / White / Cafe900); Cream panic hover stays Cafe900; normal `Button / Text / Hover` stays Black on Cream.
- ProgressBar phase labels use SecondaryText (+ Disabled); ModulationBus uses Label Background Clicked/Hover; Ramp badge uses ModuleHeader Emphasis; About links use ChromeText Emphasis.
- Product skins from compiled seeds; Trials unbound for new keys. HeaderPanel dialog content fill left unchanged (deferred).
- Verification: lint OK; SkinColourWiring / SliderZoneSeed / NumberBoxDisabled / SkinColourSubstrate → 0 failures.

## Spec Change Log

- 2026-11-10 review loop 1 — intent_gap: Cream alert hover Cafe900→Black when alert inherited Button Text Hover. Human chose option 2: dedicated Button pastille for alert hover/clicked text. Amended frozen Decisions + Alert matrix row + Code Map/Tasks/Design Notes for `Button / SecondaryText / Hover` seeded from ChromeText Text Hover. KEEP from prior pass: full ownership graph, ProgressBar SecondaryText for phase labels, Label Background Clicked/Hover for ModulationBus, ModuleHeader Emphasis for Ramp badge, ChromeText Emphasis for About, compiled Product seeds, HeaderPanel dialog fill deferred. Also fold prior review patch intent: SliderK4 editor remap independence test; fix About/badge test overclaim.

## Review Triage Log

- high — Blind+Edge+VG: Cream alert hover/clicked text Cafe900→Black — verified: `buttonAlertLookFromSkin` keeps `textHover` from `Button Text Hover` (Cream `kBlack`); former source was ChromeText Hover (Cream `kCafe900`). Contradicts frozen look-unchanged. Route: intent_gap. **Resolved (human option 2):** dedicated `Button / SecondaryText / Hover`.
- false — Blind: author Trials skins lack migration for new owner keys — frozen I/O says unbound rows fall back to compiled seeds; Trials stay author-owned; old borrow-source remaps no longer affecting new owners is the intended decoupling.
- medium — Blind+Edge+VG: paint-site ownership (Settings tab, Dialog title, Footer close, ModulationBus, ProgressBar phase, About, Ramp badge) only seed/look-checked — pre-verified gap; repo leaves GUI paint to manual smoke. Defer on next review pass if still open.
- low (rejected) — Blind: independence asserts hard-code ColourChart stops — reseeding updates those expects in the same change; everyday harm negligible.
- low (rejected) — Blind+Edge: `assertRemapNumberBoxLeavesModuleHeaderAndSlider` compares all zones to Patch-before — false failure only after future k2–k4 seed divergence; ownership claim still holds today.
- low (rejected) — Blind: DevSkinMappingLinkGroups / inventory miss asserting all 23 new keys — registration covered by compile + seed suite; inventory drift unlikely in everyday use without a wider harness.
- low (rejected) — Blind: questionable state names (ChromeText Emphasis as hover, Label Background Hover for bus only, Footer Glyph Hover without Default) — naming taste; no demonstrated wrong paint.
- false — Blind: spec Code Map / Design Notes / Verification text drift — rejected (fix would edit this build's spec).
- medium — Blind+Edge: Dialog content fill still reads HeaderPanel Background — verified pre-existing borrow left unchanged; not introduced by this story. Defer on next review pass.
- false — Blind: NumberBox Border / Button Error lookups with caller colourSet — UCS keys exist at k1; `Skin::getColour` falls back; behaviour correct.
- medium — VG: no remap of SliderK4 EditorText/Caret proving zone wiring — folded into Tasks for this re-derive.
- medium — VG: `assertRemapModuleHeaderLineOrangeLeavesBadgeAndAbout` never reaches About/badge paint — folded into Tasks (rename + textEmphasis seed assert).
- carried medium (deferred) — Blind+VG: seven paint-site rewires seed-checked only — still true; deferred-work entry added.
- carried medium (deferred) — Blind+Edge: Dialog content fill HeaderPanel Background — still true; deferred-work entry added.
- carried false — Blind: Trials/author migration gap — frozen unbound→compiled seeds remains intentional.
- carried low (rejected) — Blind: naming / Label Background Hover scope / Glyph Hover without Default — taste only.
- carried low (rejected) — Blind: zone-before Patch baseline / hard-coded chart expects / inventory not enumerating all new keys.
- false — Edge: unbound Slider k2–k4 editor keys inherit remapped k1 — compiled skins seed all four sets; unbound Trials keep compiled base until mapped; fallback only if higher-set key missing entirely.
- low (patched) — Edge: AboutPanel comment still mentioned Module header orange line after move to ChromeText Emphasis.
- low (patched) — Blind: remap NumberBox test remapped EditorText without asserting it.
- low (patched) — Blind: restore UCS fallback comment on NumberBox borders / Button alert Error+SecondaryText Hover keys.
- low (rejected) — VG: NumberBox enabled/disabled border paintPixel harness — identical default seeds; everyday risk low; paint harness beyond trivial patch.
- low (rejected) — Blind: shared-chrome test / Slider k1–k3 remap breadth / Verification filter omits ModuleHeaderTitleColourTests / spec text drift — reject or edit-spec only.
- false — Blind: missing changelog for author skins — frozen forbids auto-migration; Unbound+compiled seeds is the documented path.

## Design Notes

**Naming defaults:**
- ModuleHeader clickable title → `Text` / `Hover`.
- NumberBox borders → `Border` / `Default` + `Border` / `Disabled`.
- Slider → `EditorText` + `Caret` on MCS sets that edit values.
- Button alert idle → `Background` / `Error` + `Text` / `Error`.
- Button alert hover/clicked text → `SecondaryText` / `Hover` (Text/Hover stays normal button hover; Cream seed Cafe900).
- Dialog overlay title → `Dialog` / `Text` / `Default`.
- Settings selected tab text → `Dialog` / `Text` / `On`; fill stays `Dialog` / `TitleBand`.
- About orange link hover → `ChromeText` / `Text` / `Emphasis` seeded orange (Hover stays white for logo).
- Footer sticky close hover → `FooterMessage` / `Glyph` / `Hover`.
- ProgressBar phase enabled label → `ProgressBar` / `SecondaryText` (Text is emerald on Black — look-unchanged wins).
- ModulationBus highlights → `Label` / `Background` / `Clicked` + `Hover`.
- Ramp “M” badge → `ModuleHeader` / `Background` / `Emphasis` + `Text` / `Emphasis`.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py --base HEAD` -- expected: Quality gate OK
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests && ./Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests SkinColourWiring SliderZoneSeed NumberBoxDisabled SkinColourSubstrate` -- expected: 0 failures

**Manual checks:**
- Skin Editor shows new owner rows including Button SecondaryText Hover; Push; remap each new token alone and confirm only its owner changes; remap BodyPanel/ChromeText and confirm shared chrome still moves together; on Cream, panic button hover text stays Cafe900.
