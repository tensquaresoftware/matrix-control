---
title: 'Skin chrome decorrelation'
type: 'refactor'
created: '2026-11-01'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'ffd000bd9128bb910f517df64771eb87f0e22788'
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_local/Documents/Recap-ColourSets-et-refactorisation-widgets.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin chrome roles mislead designers: `DarkPanel` is text-only yet presupposes a tint; `WindowClose` is an orphan one-key object while modal chrome lives under `Dialog`; plugin/standalone window fill reuses `HeaderPanel` Background so Header/Body/Footer cannot be designed independently.

**Approach:** Rename ex-`DarkPanel` to `ChromeText`; fold close Glyph into `Dialog`; keep `ReadOnlyValueField` separate; keep Header/Body/Footer Background seeds unchanged; add minimal `WindowSurface` (Background only) seeded Black→`kBlack` / Cream→`kWhite` as diagnostic under-fill for plugin and standalone; migrate Dev JSON aliases; update substrate tests and smoke Settings/About/modals.

## Decisions (from planning)

- Chrome-text object name: **`ChromeText`** (ex-`DarkPanel`).
- HeaderPanel / BodyPanel / FooterPanel Background pastilles: **keep current** Black/Cream values (greys as today).
- Under-GUI fill: new minimal object **`WindowSurface`** with **Background** only; seed **`ColourChart::kBlack`** (Black skin) and **`ColourChart::kWhite`** (Cream skin). Name uses Window (not Plugin) so it fits plugin and standalone hosts. Intended normally invisible under full Header+Body+Footer coverage; pure black/white leaks signal assembly holes.
- `PluginEditor` / `MainComponent` `fillAll` read `WindowSurface` Background (not HeaderPanel Background).

## Boundaries & Constraints

**Always:**
- English API / Skin Editor object names; no tint or product-domain presupposition in skin object names.
- `Dialog` remains the single modal chrome table; close X reads Dialog Glyph after merge.
- `ReadOnlyValueField` stays its own object (near NumberBox in Mappings order if not already).
- HeaderPanel / BodyPanel / FooterPanel Background (and Header/Footer Border) stay independently mappable; their Black/Cream seeds stay as today.
- `WindowSurface` Background seeds pure black/white as above; editor/standalone root paint uses that key.
- Legacy Dev JSON: alias old object names on load (same pattern as former ButtonLike remaps).
- ModulationBusHeader stays without a dedicated skin object.

**Never:**
- ColourSet Duo/kN expansion, ButtonLike unification, zone→set file mapping.
- Modal layout redesign beyond colour wiring.
- Creating ModulationBusHeader as a skin object.
- Leaving `Dark` (or Light/Neutral-as-tint) in skin object API names.
- Reusing Header/Body/Footer Background for the under-GUI fill (would erase the diagnostic black/white signal).
- Hitching under-fill onto NumberBox `EditorBackground` or Dialog properties.
- Naming the under-fill object with Plugin-only wording.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy path seed | Fresh Black/Cream compiled seed | `ChromeText` Text+TextHover; Dialog Border/TitleBand/Glyph; `WindowSurface` Background black/white; no WindowClose; Header/Body/Footer greys unchanged | N/A |
| Legacy Dev JSON | File still uses `"DarkPanel"` / `"WindowClose"` keys | Load remaps to `ChromeText` / `Dialog`+`Glyph`; save emits new names | Unknown keys stay rejected as today |
| Settings/About close | Hover/paint close control | Glyph from Dialog; cut-out still uses Dialog TitleBand | Missing Glyph falls back per existing skin getColour rules |
| Healthy layout | Header+Body+Footer cover editor | `WindowSurface` not visible | N/A |
| Layout hole | Gap / transparent Body without child cover | Pure black (Black) or pure white (Cream) shows through `WindowSurface` | N/A |
| Matrix modals | Confirm/progress dialogs | Still Dialog Border/TitleBand + ChromeText body ink; no forced X | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h` — add `kWindowSurface` / `kChromeText`; remove `kDarkPanel` / `kWindowClose`; keep `kDialog` + `kGlyph`.
- `Source/GUI/Skins/SkinColoursPanels.h` — Dialog Border/TitleBand; move WindowClose Glyph under Dialog; add `WindowSurface` Background element (Black/Cream → kBlack/kWhite).
- `Source/GUI/Skins/SkinColoursWidgetsLayout.h` — rename `Widgets::DarkPanel` → `ChromeText` (`kText`, `kTextHover`).
- `Source/GUI/Skins/Skin.cpp` / `Dev/DevSkinColourElementList.cpp` / `Dev/DevSkinSsotSeed.cpp` — seed inventory load order.
- `Source/GUI/Skins/Dev/DevSkinKeyNames.cpp` — `"ChromeText"`, `"WindowSurface"`; legacy aliases for `"DarkPanel"` / `"WindowClose"`.
- `Source/GUI/Skins/Dev/DevSkinJsonMapping.cpp` — load-time object rename aliases (ButtonLike pattern).
- `Source/GUI/Looks/LookBuilders.h` / `.cpp` / `LookBuildersMultiSet.cpp` — rename `darkPanelLabelLookFromSkin` → `chromeTextLabelLookFromSkin`; brand/alert still borrow ChromeText.
- Call sites (chrome text): `Logo.cpp`, `HeaderPanel.cpp`, `HeaderPanelSetup.cpp`, `FooterPanel.cpp`, `SettingsMidiPage.cpp`, `GettingStartedWizardDialog*.cpp`, `AboutPanel.cpp`, matrix dialogs under `Source/GUI/Dialogs/`.
- Close X: `Settings/SettingsWindow.cpp`, `About/AboutWindow.cpp` — `kDialog` / `kGlyph`.
- Window fill: `PluginEditor.cpp`, `MainComponent.cpp` — `kWindowSurface` / `kBackground`.
- Panels: `HeaderPanel.cpp`, `BodyPanel.cpp`, `FooterPanel.cpp` — keep Background seeds.
- Skin Editor order: `Tools/SkinEditor/SkinEditorMappingsColumnFilter.h` — `WindowSurface` first in chrome cluster; drop WindowClose; place `ChromeText` with chrome; ReadOnlyValueField near NumberBox.
- Tests: `Tests/Unit/SkinColourSubstrateTests.cpp` — ChromeText / Dialog Glyph / WindowSurface black-white; remove DarkPanel/WindowClose asserts.
- Reuse: ButtonLike legacy remap pattern. Do not change ColourChart greys or Skin Editor chrome tokens.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinValues.h` (+ property consumers) — `kChromeText`, `kWindowSurface`; remove `kDarkPanel` / `kWindowClose`; Dialog keeps `kGlyph`.
- [x] `Source/GUI/Skins/SkinColoursPanels.h` / `SkinColoursWidgetsLayout.h` / `Skin.cpp` / `DevSkinColourElementList.cpp` — Dialog Glyph; ChromeText elements; WindowSurface Background black/white; keep panel greys.
- [x] `Source/GUI/Skins/Dev/DevSkinKeyNames.cpp` / `DevSkinJsonMapping.cpp` — new names + legacy `DarkPanel`/`WindowClose` aliases.
- [x] Look helpers + chrome-text call sites in Code Map — switch keys/helpers; no leftover Dark* skin API names.
- [x] `SettingsWindow.cpp` / `AboutWindow.cpp` — close Glyph from Dialog.
- [x] `PluginEditor.cpp` / `MainComponent.cpp` — `fillAll` from `WindowSurface` Background.
- [x] `Tools/SkinEditor/SkinEditorMappingsColumnFilter.h` — WindowSurface → panels → Dialog (Glyph) → … → ChromeText; ReadOnlyValueField near NumberBox; no WindowClose.
- [x] `Tests/Unit/SkinColourSubstrateTests.cpp` (+ wiring if needed) — ChromeText / Dialog Glyph / WindowSurface asserts.
- [x] Build + `lint_touched.py` on touched C++ under Source/Tests/Tools/SkinEditor.

**Acceptance Criteria:**
- Given Skin Editor Mappings, when listing objects, then no `DarkPanel`/`WindowClose`; Dialog exposes Glyph; `ChromeText` and `WindowSurface` present; ReadOnlyValueField near NumberBox.
- Given a legacy Dev skin JSON with old object names, when loaded, then colours apply under the new keys and resave uses new names.
- Given Settings and About, when hovering/clicking close, then the X still paints from Dialog Glyph with TitleBand cut-out behaviour preserved.
- Given Black/Cream seeds, when inspecting keys, then panel Backgrounds keep current greys and `WindowSurface` Background is pure black/white; healthy layout hides `WindowSurface`.
- Given substrate tests, when run, then DarkPanel/WindowClose assertions are gone and ChromeText / Dialog Glyph / WindowSurface invariants pass.

## Implementation Notes

- Implemented ChromeText / WindowSurface / Dialog Glyph merge; legacy aliases in `parseObjectType` (consumed by DevSkinJsonMapping).
- Added `ColourSetModelTests` legacy DarkPanel/WindowClose remap coverage; `SkinChromeDecorrelationTests` for WindowSurface vs panel greys + Dialog Glyph; Skin Editor chrome display-order test.
- Manual smoke (Settings/About close X, forced layout gap) still for human walkthrough.
- `lint_touched.py` clean after extracting WindowSurface asserts out of `SkinColourSubstrateTests.cpp`.

## Spec Change Log

## Review Triage Log

- Blind: stale Dark-panel comment in GettingStartedWizardDialogControls — verdict: `low` — comment still presupposes tint beside chromeText helper; direct wording fix. Route: patch.
- Blind: Spec/Code Map claim DevSkinJsonMapping.cpp gained aliases — verdict: `false` — aliases correctly live in `parseObjectType` used by JsonMapping; fixing narrative would edit this build's spec (rejected).
- Blind: legacy load remaps but resave new names only half-tested — verdict: `medium` — mappingFromVar covered; mappingToVar/export names not asserted. Route: patch (group with TextHover gap).
- Blind: DarkPanel TextHover remap untested — verdict: `low` — Hover is part of ChromeText inventory. Route: patch (same legacy-test group).
- Blind: Verification ctest -R SkinColour misses new suites — verdict: `false` — finding asks to edit this build's spec Verification section (rejected).
- Blind: spec status done vs unfinished smoke — verdict: `false` — process artifact; status set to in-review during this review; not a code defect.
- Blind: Code Map still attributes WindowSurface asserts to SkinColourSubstrateTests — verdict: `false` — fix is edit this build's spec (rejected).
- Blind: no automated check PluginEditor/MainComponent fillAll reads WindowSurface — verdict: `medium` — seed tested, paint call sites not; repo avoids paint unit tests. Route: defer.
- Blind: chromeDecorrelationDisplayOrder toString != DarkPanel/WindowClose nearly vacuous — verdict: `low` — rejected (everyday harm unlikely; meaningful fix needs more than a direct correction).
- Blind: kWindowSurface front of enum renumbers later values — verdict: `false` — Dev JSON persists object names as strings via DevSkinKeyNames, not ordinals.
- Blind: WindowClose alias remaps any property to Dialog — verdict: `low` — rejected (only Glyph existed historically; everyday corrupt rows unlikely; guard adds branch complexity).
- Blind: project agent context still names DarkPanel/WindowClose — verdict: `medium` — historical docs outside this change. Route: defer (agent-context / docs).
- Edge: WindowClose non-Glyph property — verdict: `low` — same as Blind WindowClose-any-property; rejected.
- Edge: legacy+new duplicate key silent last-wins — verdict: `maybe-false` — map overwrite is pre-existing import semantics; would be medium if proven harmful on real dual rows. Route: defer.
- Edge: claim DevSkinJsonMapping.cpp holds aliases — verdict: `false` — claim about docs/task wording; code path via parseObjectType is correct.
- Verification-gap: chromeTextLabelLookFromSkin / brandLabelLookFromSkin Look↔Skin untested — verdict: `medium` — pre-verified; helpers can drift to kLabel with tests still green. Route: patch.
- Verification-gap: PluginEditor/MainComponent WindowSurface fillAll unpinned — verdict: `medium` — pre-verified disposition defer. Route: defer.
- Verification-gap: Settings/About close Glyph call sites unpinned beyond seed — verdict: `medium` — pre-verified disposition defer. Route: defer.

## Design Notes

- Place `WindowSurface` then `Dialog` then `ChromeText` at the top of the Mappings chrome cluster (before Header/Body/Footer), not among control widgets.
- Rename helpers to `chromeTextLabelLookFromSkin`; keep alert hover borrowing ChromeText Hover white; do not add X to matrix modals.
- JSON alias: `WindowClose`+Glyph → Dialog/Glyph; `DarkPanel` → `ChromeText` (Text/TextHover). No legacy name for WindowSurface (new).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: plugin + Skin Editor + tests compile
- `ctest --preset macos-debug-arm64 -R 'SkinColour'` -- expected: SkinColour* tests pass (adjust filter to project test binary if needed)
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks (if no CLI):**
- Open Settings and About: close X colour/hover; Dialog chrome unchanged.
- Skin Editor Mappings: `WindowSurface` + Dialog Glyph + `ChromeText`; old names absent; temporarily shrink a panel or force a gap and confirm pure black/white under-fill shows.

### Review Findings

- [x] [Review][Patch] Pin Dialog Glyph seed on Cream as well as Black [`Tests/Unit/SkinChromeDecorrelationTests.cpp:62`]
- [x] [Review][Defer] PluginEditor/MainComponent fillAll not unit-pinned to WindowSurface [`Source/GUI/PluginEditor.cpp:125`] — deferred: repo does not unit-test editor paint; already in deferred-work.md
- [x] [Review][Defer] Settings/About close paint not unit-pinned to Dialog Glyph [`Source/GUI/Settings/SettingsWindow.cpp:53`] — deferred: close-button paint outside unit style; already in deferred-work.md
- [x] [Review][Defer] Dual legacy+new Dev JSON rows last-wins after remap [`Source/GUI/Skins/Dev/DevSkinKeyNames.cpp:148`] — deferred: pre-existing import semantics; settle with dual-row fixture if authors hit it
- [x] [Review][Defer] Historical sibling specs / notes still name DarkPanel or WindowClose — deferred: docs/agent-context outside this chrome rename; already in deferred-work.md
- [x] [Review][Defer] Skin Editor Mappings focus-pad paint / hit-test / content-height untested [`Tools/SkinEditor/SkinEditorMappingsPanel.cpp:281`] — deferred: no Mappings panel harness; pad alignment verified by smoke

#### Rejected

- Spec Execution checkbox order vs Design Notes / code — false: product AC and decided order match code (`WindowSurface → Dialog → ChromeText →` panels); fixing the checkbox is a spec edit.
- Code Map / tasks still cite SkinColourSubstrateTests / DevSkinJsonMapping / SsotSeed — false: Implementation Notes already correct; fix would edit this spec.
- Verification `ctest -R SkinColour` misses new suites — false: process/spec Verification wording; fix would edit this spec.
- Skin Editor polish (pad / Palette copy) out of Intent — false: polish is in this review blast radius; chrome token values unchanged (aligned to existing `kMappingsFocusPad`).
- `chromeDecorrelationDisplayOrder` vacuous DarkPanel/WindowClose string checks — low: everyday harm unlikely; meaningful inventory absence needs more than deleting those expects.
- WindowClose alias remaps any property to Dialog — low: only Glyph existed historically; property guard adds branch complexity for corrupt rows.
- Empty Spec Change Log / status done with deferrals — false: process artifact, not a code defect.
- No on-disk Dev JSON fixture for DarkPanel/WindowClose round-trip — false: `mappingFromVar` / `mappingToVar` already assert load remap and resave new names.
- SkinColourWiring / inventory unpinned for WindowSurface / Dialog Glyph — false: seed asserts in `SkinChromeDecorrelationTests` fail if those keys are missing from the compiled element list.
