---
title: 'Zones UCS/MCS + product skins BLACK/DUO and CREAM'
type: 'feature'
created: '2026-11-08'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '404e03b0c896d1abbe8d2a36b3fba19df0d5e5b3'
context:
  - '{project-root}/_local/Documents/Preparation-assainissement-zones-tiroirs-Black-Duo.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Product Looks still treat most controls as four shared drawers (`k1`…`k4`), so Patch / Master / Header / Modales wiring and the compiled Black / Cream skins do not match the frozen UCS/MCS zone model or the validated Skin Editor trials.

**Approach:** Shrink per-object colour-set inventory to the frozen matrix, rewire call sites (zone → CS + hard exceptions), and replace compiled product skins with faithful ports of trial `BLACK/DUO 2` and `CREAM 1`, exposed as **BLACK/DUO** and **CREAM**.

## Boundaries & Constraints

**Always:**
- Zones named only Patch, Master, Header, Modales — never a product zone named Shared or Footer MCS.
- UCS objects store/resolve a single technical set (`cs1` / `k1`); MCS counts: Slider 4; ComboBox + PopupMenu 3; Led + PeakIndicator 2.
- Runtime: UCS → `k1`; else zone → matrix CS; §6 exceptions override geography.
- Port trials `dev:blackduobalanced2` → product BLACK/DUO and `dev:cream-2` → product CREAM (palette + mappings). Remap trial indices when inventory shrinks (e.g. BLACK/DUO Led/Peak yellow at trial `cs3` → product Led/Peak `cs1`; mono → product `cs2`).
- Keep ButtonStyle, combo unification, Led ≠ Peak types, and multi-set LookBuilders — sanitize inventory/wiring, do not rip out ColourSet.
- ASCII product labels: `BLACK/DUO`, `CREAM` (no em dash).
- Keep Getting Started Matrix monochrome chrome on the Black/BLACK/DUO compiled skin for dark-plate contrast (existing behaviour).
- Document temporary Skin Editor / inventory drift; do not block plugin delivery on Editor pro.

**Never:**
- Cascade zone → module → instance; Skin Editor pro / GUI Zone column / click inspector.
- Product skins CREAM/DUO, BLACK/MONO, CREAM/MONO; trials BLACK/DUO 1 & 3 as product.
- Reintroduce four drawers “just in case” for UCS or for Combo/Led/Peak beyond the matrix.
- Preserve old mental model of Settings/`k4` as universal catch-all.
- Paint() GUI unit tests; Skin Editor watch / AlertWindow SE work unrelated to this goal.
- Commit / push / PR unless Guillaume asks later.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Patch classic slider/combo | Patch Edit / Matrix Mod / Patch Manager (non-exception) | Slider/Combo/Popup `k1` | N/A |
| Master classic slider/combo | Master Edit | Slider/Combo/Popup `k2` | N/A |
| Header Gain / MIDI Led / Peak | HeaderPanel | Slider `k3`; Led/Peak `k1` (Header CS of those types) | N/A |
| Header Undo/Redo/Panic | HeaderPanel buttons | Button UCS `k1` (not Header accent) | N/A |
| Computer Patches combo/popup | Patch Manager file browser | Combo/Popup `k3` mono (exception) | N/A |
| Logo Header popup | HeaderLogoPopupMenu | Popup `k3` mono (exception) | N/A |
| Settings / wizard / dialogs MCS | Modales geography | Slider `k4`; Combo/Popup `k3`; Led/Peak `k2`; UCS types `k1` | N/A |
| ProgressBar / ReadOnly / NumberBox / Toggle / Button | Any site | UCS `k1` only (no multi-set fan-out) | Missing set → existing fallback |
| Skin switch | Settings / Getting Started combo | Labels BLACK/DUO + CREAM; IDs stay 1/2 | N/A |
| Getting Started chrome | Wizard open with CREAM selected live | Wizard chrome still BLACK/DUO compiled skin | N/A |
| Inventory pin | Unit test / DevSkin keys | Multi-set object key counts match matrix; UCS objects only `k1` | Fail test |
| Trial port pin | Seed vs extracted trial colours for representative keys | BLACK/DUO and CREAM match remapped trial pastilles | Fail test |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h` -- `ColourSet`, `kMultiSetSkinObjects`, `supportsMultipleColourSets`; add/adjust per-object set count (Slider 4, Combo/Popup 3, Led/Peak 2; drop UCS types from multi-set).
- `Source/GUI/Skins/Skin.cpp`, `SkinColours*.h`, `Dev/DevSkinSsotSeed.cpp`, `Dev/DevSkinInventory.cpp`, `Dev/DevSkinJsonMapping.cpp` -- seed fan-out, inventory keys, JSON CS support must honour reduced counts; remove obsolete k4-only ButtonLike/ProgressBar overlay assumptions where UCS/matrix replace them.
- Trials JSON (Pause Dropbox) `dev:blackduobalanced2`, `dev:cream-2` -- SSOT look; port into compiled Black/Cream variants (display BLACK/DUO + CREAM).
- `Source/GUI/Looks/LookBuildersMultiSet.cpp` (+ mono LookBuilders / ProgressBar paint paths) -- keep ColourSet threading; callers pass matrix CS.
- Call sites: `ParameterCell` / `ModuleHeader` (Patch `k1`, Master `k2`); `HeaderPanel.cpp` (Slider `k3`, Led/Peak `k1`, Button UCS); `ComputerPatchesPanel.cpp` (Combo/Popup `k3`); `HeaderLogoPopupMenu.cpp` (Popup `k3`); Settings + `Source/GUI/Dialogs/**` (Modales matrix / UCS); Internal Patches NumberBox → UCS `k1`.
- `Source/GUI/PluginEditorSkinScale.cpp`, `PluginEditorUiConstruction.cpp`, `PluginEditorGettingStarted.cpp`, Settings skin combo -- product switch + GS chrome note.
- `Source/Shared/Definitions/PluginDisplayNames.h`, `PluginIDs.h` -- rename display `BLACK` → `BLACK/DUO`; keep IDs 1/2.
- Tests: `Tests/Unit/ColourSetModelTests.cpp`, `SkinColourWiringTests.cpp`, `SkinColourWiringCoverageTests.cpp`, `SkinColourSubstrateTests.cpp`, `DevSkinCompiledSeedOverlayTests.cpp`, `ProgressBarColourSetK4Tests.cpp`, `SliderColourSetK4Tests.cpp` -- retarget pins to matrix / UCS / remapped seeds (no paint tests).
- Continuity (done, do not re-open scope): `spec-skin-colourset-product-wiring.md`, `spec-skin-colourset-model-and-editor.md`.
- Out of scope north star: `_local/Documents/Vision-Skin-Editor-pro-cascade-tiroirs-zones.md`.

**Reuse:** existing `ColourSet` enum values `k1`…`k4` as technical CS indices; zone names stay out of skin identifiers; LookBuilders multi-set signatures.

**Do not change:** Skin Editor pro UI; cascade; unrelated SE watch/AlertWindow commits; artistic endless tweaking beyond trial port.

## Tasks & Acceptance

**Execution:**
- [x] Branch -- create `feature/zones-skins-black-duo-cream` from current `feature/skin-editor` HEAD -- isolate this product goal from later SE watch commits.
- [x] `Source/GUI/Skins/SkinValues.h` (+ inventory/JSON helpers) -- MCS/UCS inventory counts per frozen matrix -- stop advertising four sets on UCS and excess Combo/Led/Peak slots.
- [x] `Source/GUI/Skins/SkinColours*.h` + `Skin.cpp` / `DevSkinSsotSeed.cpp` -- replace Black/Cream compiled seeds from trials with remapping rules in Design Notes -- faithful product looks.
- [x] Call sites (Header, Patch Manager exceptions, Settings/Dialogs, ParameterCell/ModuleHeader) -- zone → CS + §6 exceptions; UCS always `k1` -- honest Patch/Master/Header/Modales colouring.
- [x] `PluginDisplayNames.h` / Settings + Getting Started skin combos / `PluginEditorSkinScale.cpp` -- show BLACK/DUO + CREAM; keep GS chrome on BLACK/DUO skin -- coherent product switch.
- [x] `Tests/Unit/*Colour*` (+ ProgressBar/Slider k4 tests as needed) -- pin inventory counts, wiring, and representative seed pastilles -- catch regressions without paint() tests.
- [x] Short note in Implementation Notes -- document Skin Editor temporary inventory drift -- pause assumed.

**Acceptance Criteria:**
- Given inventory helpers, when listing keys for Button/Toggle/NumberBox/ReadOnlyValueField/ProgressBar, then only `k1` exists; Combo/Popup stop at `k3`; Led/Peak stop at `k2`; Slider keeps `k1`…`k4`.
- Given Header Gain, MIDI Led, Peak, and Undo, when Looks resolve, then Gain uses Slider `k3`, Led/Peak use their `k1`, Undo uses Button `k1`.
- Given Computer Patches combo and logo popup, when opened, then Combo/Popup resolve `k3`.
- Given Settings Hardware Latency slider and Settings MIDI Led, when Looks resolve, then Slider `k4` and Led `k2`.
- Given product skin combo, when listing items, then labels are ASCII `BLACK/DUO` and `CREAM`.
- Given compiled BLACK/DUO and CREAM seeds, when comparing remapped trial representative keys (Slider Patch/Master/Header/Modales accents; Led Header on-colour; WindowSurface/Body/Header/Dialog chrome), then pastilles match the Pause trials after remapping.
- Given Getting Started opens while live skin is CREAM, when wizard chrome paints, then it still uses the BLACK/DUO compiled skin for Matrix monochrome chrome.

## Implementation Notes

Planning decisions (agent-owned):
- Work on dedicated branch `feature/zones-skins-black-duo-cream` from `404e03b0`.
- UCS Toggle seed = trial `cs1` (only Patch Mutator site; BLACK/DUO blue, CREAM green).
- UCS ProgressBar / modal mono = trial modal/mono drawer when sets diverge (`cs4` on CREAM); if a trial omits ProgressBar, keep compiled mono greys at UCS `k1`.
- Led/Peak BLACK/DUO: trial yellow `cs3` → product `k1`; grey mono → product `k2`. CREAM: green trial `cs1` → product `k1`; cafe trial `cs4` → product `k2`.
- Combo/Popup: drop trial `cs4`; product Modales/exceptions use `k3` (trial `cs3` mono on BLACK/DUO; cafe on CREAM).
- CREAM may keep identical greens across Slider/Combo `k1`…`k3` if the trial does — structure follows matrix; colours follow trial.
- Keep numeric skin IDs `kBlack=1`, `kCream=2`; only display strings change for BLACK/DUO.

Implementation outcome:
- Skin Editor inventory drift (temporary, pause assumed): the editor now lists only the shrunk inventory (UCS objects `k1`; Combo/Popup up to `k3`; Led/Peak up to `k2`). Its product combo and DevSkin compiled names still read `BLACK` / `Black` / `Cream`; no SE UI was changed.
- Older trial / session JSON carrying colour sets beyond an object's inventory now imports; those rows are silently dropped (no hard failure). Legacy `*ButtonLike` rows land on the object's last set (Slider `k4`, Combo/Popup `k3`).
- Compiled seeds are explicit per colour set (no `k1` fan-out, no former ButtonLike `k4` overlay); `Skin` builds its maps from the same element list as the DevSkin seed.
- Faithful trial port kept one likely trial slip: BLACK/DUO Slider Master (`k2`) disabled track = `kBlue950`.
- Header / Settings / Computer Patches call sites are not unit-built in the test target; tests pin the zone matrix, the exception zone, and the Looks resolved per zone instead.
- Getting Started Matrix chrome policy pinned via `Skin::kGettingStartedMatrixChromeVariant` (= Black / product BLACK/DUO) plus contract test; wizard still paints from `skinBlack_`.
- Review patches: `DevSkinColourElementList.cpp` always in `PLUGIN_SOURCES` (Release link); `SkinValues` static_asserts for zone/CS order and multi-set list; MCS higher-set completeness test.

## Spec Change Log

## Review Triage Log

- high — Blind: Release link misses `DevSkinColourElementList.cpp` when `MATRIX_BUILD_SKIN_EDITOR=OFF` — verified: file only in tests + SE-gated plugin sources; `Skin.cpp` calls `DevSkin::forEachCompiledColourElement`.
- medium — Blind: Verification only lists SE-on debug preset — real risk is the link gap above; covered by same patch / manual Release check; reject as standalone “edit Verification section” ask.
- medium — Blind+Edge: CREAM SliderK4 / ProgressBar text == valueBar (`kCafe800`) — verified in seeds; faithful trial port; deferred (usability vs fidelity).
- medium — Blind+Edge: CREAM disabled slider text == valueBarDisabled (`kCafe600`) — verified; deferred with trial fidelity.
- medium — Blind+Edge: CREAM ReadOnlyValueField `kTextError` == `kText` (`kCafe950`) — verified; deferred with trial fidelity.
- medium — Blind: no Pause trial JSON fixture in tests — pins use ColourChart from the same port; deferred (manual / future fixture), not blocking.
- low — Blind: Master disabled track `kBlue950` trial slip — already recorded in Implementation Notes; deferred.
- false — Blind: UCS builders still take ColourSet — intentional; spec keeps multi-set LookBuilders; missing set falls back to k1.
- low — Blind: `zoneColourSet` silent k1 for unsupported pairs + Slider `static_cast` enum order — unsupported pairs are dead product paths; enum-order risk real → patch `static_assert` only.
- low — Blind: `kMultiSetSkinObjects` can drift from `colourSetCount` — patch with static_assert loop.
- medium — Blind+VG: no per-(property,state) completeness across MCS sets — verified circular inventory; patch inventory test.
- defer — Blind+Edge: JSON import drops excess sets without warning — SE pause / documented; not product blocker.
- defer — Edge: JSON import does not role-remap old trial indices — SE pause; compiled seeds already remapped.
- maybe-false — Edge: ComboBoxButtonLike vs explicit cs3 order race — would need import fixture order proof; unverified medium → defer with settle note.
- false — Blind+Edge: SE product combo still “BLACK” — documented inventory drift; out of intent to polish SE UI.
- false — Blind: GS chrome unit test “circular” — contract pins `kGettingStartedMatrixChromeVariant`; full editor not in test target by design.
- medium — VG: Header/Settings/ComputerPatches call sites not in test target — verified; AC covered via helpers + manual smoke; defer (adding panels to tests non-trivial).
- false — Blind: call-site style underscore / missing Led LookForZone helper — cosmetic; no user harm.
- false — Blind: spec frontmatter status / Code Map stale entries — reject findings that require editing this build’s spec as the fix.
- false — Edge low: UCS ColourSet param enforcement — same as UCS builders false above.

## Design Notes

**Zone → CS (MCS):** Slider Patch/Master/Header/Modales = k1/k2/k3/k4. Combo+Popup Patch/Master/Modales = k1/k2/k3 (no Header combo). Led+Peak Header/Modales = k1/k2.

**Exceptions:** Computer Patches Combo+Popup k3; logo Popup k3; Header Undo/Redo/Panic Button UCS k1.

**Trial → product remapping (not naive index copy):** shrink drawers, then copy the *role* colour (Patch blue / Master orange / Header yellow / Modales mono on BLACK/DUO; CREAM greens + cafe mono) into the product index for that role.

**Suggested Review Order:** (1) `SkinValues` inventory counts + tests, (2) seed port remapping vs trial JSON, (3) Header + exception call sites, (4) Settings/Dialogs Modales matrix, (5) display names + GS chrome.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'Colour|Skin|ProgressBar|SliderColour'` (or project-equivalent unit filter) -- expected: updated pins pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under Source/ and Tests/

**Manual checks (if no CLI):**
- Smoke Standalone: switch BLACK/DUO ↔ CREAM; confirm Patch blue vs Master orange vs Header yellow accents on BLACK/DUO; CREAM cream/green/cafe chrome; Computer Patches + logo menus mono; Settings latency + leds; Bank Transfer ProgressBar; Getting Started chrome stays dark Matrix plate.

### Review Findings

- [x] [Review][Patch] Tighten JSON drop test to assert excess keys absent and supported seed untouched [`Tests/Unit/ColourSetModelTests.cpp:assertJsonDropsSetsBeyondInventory`]
- [x] [Review][Patch] Assert ParameterCell ComboBox/PopupMenu looks follow Patch vs Master zone [`Tests/Unit/SkinColourWiringTests.cpp` next to slider zone pin]
- [x] [Review][Patch] Pin ProgressBar Cream disabled seeds (not Black-only) [`Tests/Unit/ProgressBarUcsSeedTests.cpp:assertDisabledContract`]
- [x] [Review][Defer] CREAM Modales value-bar text contrast (cafe on cafe) — deferred: faithful trial port; already in deferred-work
- [x] [Review][Defer] CREAM ReadOnlyValueField error text matches normal text — deferred: trial fidelity; already in deferred-work
- [x] [Review][Defer] JSON import drops excess CS / no role-remap — deferred: SE pause; product seeds remapped
- [x] [Review][Defer] Header/Settings/Computer Patches/logo call sites not built in unit tests — deferred: helpers + smoke gate
- [x] [Review][Defer] No Pause trial JSON fixture in Tests — deferred: ColourChart pins from same port

#### Rejected

- false — ColourSetModel legacy expand asserts Slider tracks share k1: tests legacy JSON fan-out, not compiled multi-set seeds; suite green
- false — Product Skin depending on `DevSkinColourElementList`: intentional shared SSOT; always in `PLUGIN_SOURCES` for Release link
- false — Missing Led/Peak `*LookForZone` helpers: call sites use `zoneColourSet` correctly; cosmetic API gap only
- reject — Spec Verification still filters `SliderColour` / Code Map lists old test names: fix would be editing this spec under review
- low — `appendButtons` also registers ProgressBar tokens: naming taxonomy only; no user harm
- low — NumberBox `kEditorBackground` Black red sentinel vs Cream cream: pre-existing Black sentinel + Cream port; not an AC blocker
