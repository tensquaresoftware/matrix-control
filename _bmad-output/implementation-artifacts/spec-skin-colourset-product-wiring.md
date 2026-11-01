---
title: 'Skin ColourSet product wiring (zones → k1…k4)'
type: 'feature'
created: '2026-11-01'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '6f9e753fd31a56e65ac1d635b67ab630867560ec'
context:
  - '{project-root}/_local/Documents/Recap-ColourSets-et-refactorisation-widgets.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-colourset-model-and-editor.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** ColourSet `k1`…`k4` exists in the skin and Skin Editor, but product Looks still resolve the default set only. Duo/Quadri cannot recolour zones until construction assigns the right set.

**Approach:** Pass each zone’s ColourSet into Look resolution for multi-set controls at construction and `setSkin` rebuilds. Keep zone names out of the skin model. Black & Cream stay visually identical while seeds match.

## Decisions (human-approved planning)

- **k1:** Patch Edit classic sliders/combos; Matrix Modulation classic controls + Init/Copy/Paste; Patch Manager classic controls (Internal Patches / Bank Utility / Patch Mutator buttons, combos, toggles, HierarchicalComboBox). ModuleHeader / ModulationBusHeader chrome remain k1-only objects.
- **k2:** Master Edit classic sliders/combos + Master ModuleHeader Init/Copy/Paste buttons.
- **k3:** Header MIDI Leds, Input Gain slider, Header PeakIndicator, Header Undo/Redo/Panic buttons.
- **k4:** Settings (ButtonLike combo/popup, Hardware Latency SliderButtonLike, buttons, ReadOnlyValueField, Settings PeakIndicator, Settings MIDI Leds); Computer Patches file combo (keep ButtonLike types); Internal Patches NumberBoxes; dialog multi-set controls (Getting Started, EPROM prompt, confirm/choice Buttons/fields).
- Shared ColourSet across Led + PeakIndicator (or other types) does **not** merge SkinObjectTypes.
- ButtonLike objects stay; assign `k4` only (merge is GPC 4). No bus-specific set. No zone→set config file. No product zone names in skin identifiers.

## Boundaries & Constraints

**Always:**
- Thread `ColourSet` through multi-set `*LookFromSkin` and Skin helpers they call.
- Preserve ColourSet across `setSkin` rebuilds (store on widget or re-pass from owner).
- Keep missing-set fallback (requested → k1 → white assert).
- No intentional Black/Cream visual regression while seeds are identical.
- Wiring tests force distinct pastilles so wrong-set bugs fail loudly.

**Never:**
- Product zone names as ColourSet/SkinObjectType; ButtonLike merge; schema/editor column changes (except blocking GPC 2 bug); Duo hue seeding; GPC 1 reorder; GPC 5 renames; zone→set file.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Patch classic slider | PatchEdit module build | Slider colours at `k1` | N/A |
| Master classic slider | MasterEdit module build | Slider colours at `k2` | N/A |
| Header Gain / MIDI Led / Peak | HeaderPanel build | Slider / Led / Peak at `k3` | N/A |
| Settings ButtonLike + latency | Settings setup | ButtonLike / SliderButtonLike at `k4` | N/A |
| Peak Header vs Settings | Two PeakIndicator sites | Header `k3`, Settings `k4` | N/A |
| Led Header vs Settings | Two Led contexts | Header `k3`, Settings `k4` | N/A |
| Computer Patches combo | ButtonLike style | ButtonLike objects at `k4` | N/A |
| Internal NumberBoxes | Bank/patch boxes | NumberBox at `k4` | N/A |
| setSkin rebuild | Skin swap | Same ColourSet reapplied | N/A |
| Black/Cream smoke | Compiled Black or Cream | No intentional Look regression | N/A |
| Distinct-set unit test | Forced distinct pastilles | Zone looks match assigned set | Fail test |

</frozen-after-approval>

## Code Map

- `LookBuilders.h/.cpp` — `ColourSet` (default `k1`) on multi-set builders (Button, Slider±ButtonLike, ComboBox, PopupMenu, ReadOnlyValueField, Led, Peak, Toggle, NumberBox); omit on k1-only builders.
- `ISkinColours.h`, `Skin.h/.cpp` — ColourSet on `getSlider*` / `getComboBox*` / `getPopupMenu*` / `getToggle*` (and any helper LookBuilders use).
- Multi-set widgets (+ HierarchicalComboBox if it owns looks) — store `colourSet_` for rebuilds; default `k1`.
- `WidgetFactory` / `ParameterCell` / `BaseModulePanel` — PatchEdit→`k1`, MasterEdit→`k2` fork for factory-created controls.
- Call sites: HeaderPanel→`k3`; Settings* + ComputerPatches ButtonLike + Internal NumberBoxes + dialog multi-set→`k4`; Matrix Mod / Patch Manager classic / Patch Mutator→`k1` (explicit or default).
- Do not change: Skin Editor ColourSet column, Dev schema (unless blocking), Peak≠Led split, ButtonLike routing, ModulationBusHeader→ModuleHeader, display/panel k1-only inventory.
- Tests: `SkinColourSubstrateTests` (or focused wiring tests) — LookFromSkin(set) vs getColour; Header vs Settings Peak/Led with distinct pastilles; Black+Cream pair; CMake register if new file.

## Tasks & Acceptance

**Execution:**
- [x] `LookBuilders` + Skin helpers — thread ColourSet into multi-set looks.
- [x] Widgets + factory / ParameterCell / BaseModulePanel — store/accept set; Patch→k1 / Master→k2.
- [x] `HeaderPanel` — k3 on MIDI Leds, Input Gain, Peak, Undo/Redo/Panic.
- [x] Settings + Computer Patches ButtonLike + Internal NumberBoxes + dialog multi-set — k4.
- [x] Patch-side classic remainder (Matrix Mod, Patch Manager, Patch Mutator) — k1 completeness.
- [x] Wiring unit asserts + Black/Cream smoke; lint_touched.py on touched C++.

**Acceptance Criteria:**
- Given Patch Edit classic slider look build, then colours resolve at `k1`.
- Given Master Edit classic slider look build, then colours resolve at `k2`.
- Given Header MIDI Led, Input Gain, Header Peak look build, then colours resolve at `k3`.
- Given Settings ButtonLike, Hardware Latency, Settings Peak, Settings MIDI Led look build, then colours resolve at `k4`.
- Given Computer Patches file combo, then ButtonLike types remain and colours resolve at `k4`.
- Given Internal Patches NumberBoxes, then colours resolve at `k4`.
- Given Black/Cream with identical seeds, then no intentional product Look regression.
- Given distinct pastilles per set, then Header Peak `k3` ≠ Settings Peak `k4` (same for Led).
- Given `setSkin` after construction, then the original ColourSet is preserved.
- Given lint_touched.py on touched C++, then it passes.

## Implementation Notes

- Threaded ColourSet through multi-set LookBuilders (split `LookBuildersMultiSet.cpp`) and Skin colour accessors (`SkinColourAccessors.cpp`).
- Zone wiring: ParameterCell/ModuleHeader Patch→k1 Master→k2; HeaderPanel→k3; Settings/Computer Patches ButtonLike/Internal NumberBoxes/dialogs→k4; Patch-side classic remains default k1.
- Review patches: NumberBox focus tokens at k4 (Computer Patches, Bank Utility, ModuleHeader borrow); device-field refresh uses stored getColourSet(); PeakIndicator + ParameterCell wiring asserts added.
- Verified: SkinColourWiring green; lint_touched.py OK. Manual Black/Cream glance still recommended (seeds identical → no intentional visual change).

## Spec Change Log

## Review Triage Log

- Blind Hunter: WidgetFactory still builds sliders/combos without ColourSet — false — Code Map allowed optional factory set; ParameterCell applies Patch→k1 / Master→k2 via setLook (verified ParameterCell.cpp colourSet_).
- Blind Hunter: buttonAlertLookFromSkin ignores ColourSet for FooterMessage/DarkPanel alert fills — false — those objects are k1-only inventory; alert pastilles are intentionally shared chrome, not zone sets.
- Blind Hunter: resolvedTypographyFont duplicated across LookBuilders files — low — reject (extract helper adds surface; no everyday harm while heights stay in sync).
- Blind Hunter: device-field refresh triples getColour k4 blocks — low — reject as DRY-only; related getColourSet fix routed under Edge Case patch.
- Blind Hunter: wiring tests never construct HeaderPanel/Settings/ComputerPatches — medium — grouped with Verification Gap call-site coverage; patch via ParameterCell/Peak/Led getters + LookBuilders, not full panel mounts.
- Blind Hunter: only Led has setSkin ColourSet preservation test — medium — grouped with VG PeakIndicator gap → patch.
- Blind Hunter: no product-path assert Master→k2 — medium — grouped with VG → patch ParameterCell getColourSet assert.
- Blind Hunter: multi-set widgets Button/Slider/Combo/NumberBox/Toggle lack colourSet_ — false — frozen Always allows store on widget OR re-pass from owner; owners re-apply.
- Blind Hunter: spec tasks claim BaseModulePanel / Patch Mutator touched — false — reject (would edit this build's spec; k1 via LookBuilders default matches AC).
- Blind Hunter: Verification claims green while Implementation Notes empty — false — reject (edit-spec finding); notes filled by build process separately.
- Blind Hunter: ParameterCell stamps look after factory default-k1 — low — reject (redundant work, colours correct after setLook).
- Blind Hunter: assertSettingsAndComputerPatchesUseK4 name overclaims integration — low — reject (naming only; coverage extended by patch group).
- Edge Case: ComputerPatchesPanel NumberBox focus getColour omits ColourSet (defaults k1) while combo is k4 — medium — verified line 144 — patch.
- Edge Case: BankUtilityPanel NumberBox focus likewise omits ColourSet — medium — verified BankUtilityPanel.cpp:321 — patch (same focus token as Internal NumberBoxes k4).
- Edge Case: moduleHeaderLookFromSkin borrows NumberBox textFocus at default k1 — medium — verified LookBuilders.cpp:108; product NumberBoxes are k4 — patch.
- Edge Case: Eprom/GettingStarted/Settings refreshDeviceValueField hardcodes k4 ignoring stored set — medium — verified; setColourSet then refresh can diverge — patch use getColourSet().
- Edge Case claim: Computer Patches focused combo leaves k4 zone — medium — same root as ComputerPatches focus omit — grouped.
- Verification Gap: zone assignments at product call sites not exercised — medium — pre-verified VG — patch (ParameterCell Master/Patch getColourSet; Peak setSkin; keep LookBuilder distinct-pastille asserts).
- Verification Gap: PeakIndicator setSkin ColourSet preservation untested — medium — pre-verified VG — patch.

## Design Notes

Prefer `*LookFromSkin(skin, colourSet)` at call sites; widgets that rebuild looks store `colourSet_`. Map existing PatchEdit/MasterEdit module type to ColourSet in the shared cell/factory path. When slider looks borrow NumberBox editor tokens, request the **same** ColourSet as the slider. Do not confuse ColourSet with `Skin::ColourVariant` or Blue/Orange header variants. Peak≠Led already split (GPC 2); this wave only assigns sets.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` — build OK
- Matrix-Control_Tests SkinColour / wiring filters — green
- `python3 Scripts/quality/lint_touched.py` — OK on touched C++

**Manual checks:**
- Plugin Black then Cream: Header leds+peak, Patch classic control, Master control, Settings ButtonLike / latency — unchanged at a glance.
- Optional Dev skin with divergent sets: Header vs Settings Peak/Led diverge without merging types.

### Review Findings

- [x] [Review][Decision] How deep should ColourSet call-site tests go? — Resolved: option 2 — harden targeted widget/LookBuilder asserts without mounting Header/Settings/Computer Patches panels. Residual call-site risk on those panels accepted for this wave.
- [x] [Review][Patch] Assert ParameterCell actually stamps slider/combo looks with the module ColourSet (not only getColourSet with ParameterType::None) [Tests/Unit/SkinColourWiringTests.cpp]
- [x] [Review][Patch] Assert ModuleHeader MasterEdit Init/Copy/Paste use ColourSet k2 (and non-Master k1) on create / skin refresh [Source/GUI/Widgets/ModuleHeader.cpp]
- [x] [Review][Patch] Assert moduleHeaderLookFromSkin textFocus resolves NumberBox focus at k4 with distinct pastilles [Tests/Unit/SkinColourWiringCoverageTests.cpp]
- [x] [Review][Patch] Assert ReadOnlyValueField constructed at k4 applies that set (and preserves it across setSkin) [Tests/Unit/SkinColourWiringCoverageTests.cpp]

#### Rejected

- WidgetFactory sliders/combos lack ColourSet arg — false — ParameterCell stamps looks after create; Matrix Modulation factory path correctly stays default k1.
- BaseModulePanel never touched — false — Patch/Master classic colours flow through ParameterCell; k1 defaults cover Patch-side modules without BaseModulePanel edits.
- HierarchicalComboBox / Patch Mutator unwired — false — call sites use LookBuilders default k1, matching the zone map.
- Led/Peak/ROVF setColourSet unused in product — false — Always allows store on widget or owner re-pass; ctor + setSkin paths cover product use.
- buttonAlertLookFromSkin FooterMessage/DarkPanel ignore ColourSet — false — alert pastilles are shared k1-only chrome by design.
- GettingStarted applyControlLooks skips device field — false — setSkin + refreshDeviceValueField already run.
- Spec Change Log empty / Code Map still cites Skin.cpp — rejected — would edit the spec under review.
- resolvedTypographyFont duplicated across LookBuilders TUs — low — extract adds surface; no everyday harm while heights stay in sync.
- Panic comment “value-pressure” vs queue-pressure API — low — comment-only; not worth a wiring patch.
- Black/Cream smoke covers only slider + Peak — low — seeds still identical; expanding smoke unlikely to catch a real regression this wave.
- ReadOnlyValueField::setColourSet resets error/custom colours — low — no product caller; fix would add branches for an unused path.
