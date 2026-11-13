---
title: 'Ship product CREAM/CHROMA from trial CREAM/CHROMA 3'
type: 'feature'
created: '2026-11-13'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'f639bc2c65374aa8a32a728f2f17f8b2fe503dbd'
context:
  - '{project-root}/Source/GUI/Skins/ColourChart.h'
  - '{project-root}/Source/GUI/Skins/Skin.h'
  - '{project-root}/Source/GUI/Skins/SkinColoursCommon.h'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** CREAM/CHROMA is still a greyed placeholder in the product while trial CREAM/CHROMA 3 is validated (cream chrome + Soft blue/orange/yellow accents). Soft* trial names must become standard Blue/Orange/Yellow chart steps without changing RGB of already-shipping skins.

**Approach:** Expand ColourChart with cream Soft RGB under Blue/Orange/Yellow names; relocate BLACK/CHROMA (and shared) chart RGB that collide onto free indices with identical hex values; add ColourVariant CreamChroma as creamVariant base plus accent overrides from the trial Soft mappings; enable logo/settings digit 5 with a new stable prefs id.

## Boundaries & Constraints

**Always:**
- Trial SSOT for cream Soft accents: Dropbox Trials skin `CREAM/CHROMA 3` Soft* ARGB (final values at approval time).
- Existing shipping skin RGB unchanged (BLACK/GREEN, BLACK/CHROMA, BLACK/MINIMAL, CREAM/GREEN, CREAM/MINIMAL) — only constant *names* / which chart symbol a seed references may change.
- `Orange500` remains footer WARNING `#D44818` for all variants.
- CreamMinimal prefs id stays `5`; CreamChroma gets new id `6` (digit 5 → id 6, digit 6 → id 5).

**Never:**
- Do not change Soft* trial JSON as the product source of truth after ship (product seeds win).
- Do not add a fourth ColourElement column unless overlay approach fails review — prefer cream base + accent override table.
- Do not alter Skin Designer title-bar behaviour in this spec.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Select CREAM/CHROMA | Logo row or Cmd/Ctrl+5 | Skin applies; prefs store id 6; Patch/Master/Header accents blue/orange/yellow | N/A |
| Restore CreamMinimal prefs | Saved skin id 5 | Still CREAM/MINIMAL (not Chroma) | N/A |
| BLACK/CHROMA after chart rename | Open BLACK/CHROMA | Identical RGB to pre-change | N/A |
| CREAM/GREEN after chart rename | Open CREAM/GREEN | Identical RGB (greens/cafe/reds unchanged) | N/A |
| Unknown/legacy skin id | Corrupt/missing id | Fall back to default BLACK/GREEN | Normalize like existing appearance guard |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/ColourChart.h` — expand Blue/Orange/Yellow ladders; relocate colliding BLACK/CHROMA RGB to free indices (same hex).
- `Source/GUI/Skins/SkinColours*.h` — retarget blackChromaVariant (and any cream) references that used relocated symbols; creamChroma accents via Blue/Orange/Yellow Soft RGB names.
- `Source/GUI/Skins/SkinColoursCommon.h` / `Skin.h` / `Skin.cpp` — `ColourVariant::CreamChroma`; init = creamVariant then Soft→chart accent overlay (or dedicated override list).
- `Source/GUI/Skins/Dev/DevSkin*.{h,cpp}` — `kCompiledCreamChroma`, seed/export/applier/layer parity with other compiled skins.
- `Source/Shared/Definitions/PluginIDs.h` — `SkinVariants::kCreamChroma = 6` (keep `kCreamMinimal = 5`).
- `Source/GUI/Skins/Skin.h` `SkinComboBoxItemId` — add `kCreamChroma = 6`.
- `Source/GUI/Widgets/HeaderLogoPopupMenuItems.cpp` — enable CREAM/CHROMA row with real item id.
- `Source/GUI/Helpers/EditorChromeShortcuts.h` — digit 5 → `kCreamChroma`.
- `Source/GUI/PluginEditor*.cpp` / Settings / Getting Started — create/hold/apply CreamChroma skin; allowlist id 6.
- `Source/GUI/Skins/Dev/DevSkinPaletteCatalog*` / gated colour scan — allow new chart hex if required.
- Tests: `SkinColour*`, `SliderZoneSeed*`, `EditorChromeShortcut*`, `GettingStartedSettingsContract*`, DevSkin compiled inventory — extend for CreamChroma + renamed chart symbols.
- Trial reference (read-only): `…/Skins/Matrix-Control Skin Designer - Trials.json` skin `CREAM/CHROMA 3`.

### Chart remap (agent decision — RGB preserved for shipping skins)

**Blue** — Soft cream RGB take Blue100/200/400/500/600/700/800. Former BLACK/CHROMA `Blue400/600/800` move to `Blue450/650/750` (same RGB). `Blue950` unchanged.

**Orange** — Soft take Orange100/200/400/600/700/800; SoftOrange500 → `Orange550` (do not touch warning `Orange500`). Former BLACK `Orange400/700/800` → `Orange350/650/750`.

**Yellow** — Soft take Yellow100/200/400/500/600/700/800. Former BLACK `Yellow400/700/800` → `Yellow300/650/850`. `Yellow950` unchanged.

## Tasks & Acceptance

**Execution:**
- [x] `ColourChart.h` — apply remap + Soft RGB under standard names; document ladders in comments.
- [x] `SkinColours*.h` — update references for relocated BLACK/CHROMA symbols; set CreamChroma accent keys to Soft-derived chart names (greens → Blue/Orange/Yellow by colour set).
- [x] `Skin.{h,cpp}` + DevSkin compiled CreamChroma wiring — variant, seed, applier, layer, standalone paint.
- [x] PluginIDs / combo / logo / shortcuts / editor / settings / getting-started — ship id 6, enable digit 5.
- [x] Unit tests + gated colour allowlist if needed — prove BLACK/CHROMA & CREAM/GREEN RGB stable; CreamChroma Soft accents match trial hex.
- [ ] Build Standalone Debug — smoke selectable CREAM/CHROMA. (Build OK; interactive smoke not run — Computer Use lacked Screen Recording permission.)

**Acceptance Criteria:**
- Given the plugin, when the user chooses CREAM/CHROMA (logo or digit 5), then Patch/Master/Header controls use the validated Soft blue/orange/yellow look on cream chrome (reds, Cafe500 borders, BrandCream modals/logo menu).
- Given a prefs file with skin id 5, when the plugin loads, then CREAM/MINIMAL still applies.
- Given BLACK/CHROMA and CREAM/GREEN, when opened after the chart remap, then every previously shipping RGB is unchanged.
- Given Soft trial names, when reading product seeds, then only Blue/Orange/Yellow chart symbols appear (no Soft*).

## Implementation Notes

- Accent overlay lives in `Source/GUI/Skins/SkinColoursCreamChroma.h` (`kAccentOverrides`, 49 keys = every key where trial CREAM/CHROMA 3 differs from compiled CREAM/GREEN). `Skin` and the compiled Designer seed both apply creamVariant then the overlay.
- Trial rows on colour sets outside the object inventory (Toggle/ProgressBar cs2-cs3, Led/PeakIndicator cs3) are dropped by the Designer JSON loader, so they never reached the validated trial look and are intentionally not shipped.
- Chart remap verified by resolving all 197 compiled elements (3 columns) before/after: byte-identical.
- `PluginIDs::Settings::SkinVariants::isValid` replaces three hand-written id allowlists (Settings restore/combo, Getting Started).
- Skin Designer Product combo now lists six skins in logo order (CREAM/CHROMA = 5, CREAM/MINIMAL = 6; combo ids are session-only, prefs use skin id strings).

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence / route |
|---------|---------|------------------|
| README still says five skins | low | Doc lag; reject as out of everyday path for this ship — defer README follow-up |
| Interactive Standalone smoke unchecked | false | Not a code defect; human smoke remains the gate |
| Spec Change Log empty | false | Reject: would only edit this build's spec |
| Prefs id 5/6 restore untested | medium | Partial cover via isValid + id constants; restore path uses isValid — accept residual |
| Logo row enablement/id untested | medium | patch: assert SkinComboBoxItemId == PluginIDs; defer full menu extract |
| Enum order Minimal before Chroma | false | Explicit switches; order not product-visible |
| Dual id 6 without assert | medium | patch: equality asserts in GettingStartedSettingsContractTests |
| Master/Header accent asymmetry | false | Matches validated trial inventory (Designer drops unsupported colour sets) |
| Caret/disabled not Soft-overridden | low | Reject: rare visual; cream disabled already BrandCream/Cafe |
| Designer JSON pastille rename rebind | medium | defer: documented risk for old Trials files |
| Getting Started own combo list | low | Reject: small surface; Settings already shares populate helper where it matters |
| Orange500 warning not asserted | low | patch: SoftTrialRgb locks Orange500 |
| VG: product combo mapping untested | medium | patch: DevSkinWorkspaceFilesTests round-trip |
| VG: launch size-only check | medium | patch: findSkin(kCompiledCreamChromaId) |
| VG: logo menu private GUI | medium | defer: needs extractable skin-row table |
| Edge Case Hunter empty | — | no findings |

## Suggested Review Order

1. ColourChart remap + SkinColours blackChroma retargets (RGB lock).
2. CreamChroma accent wiring vs trial Soft maps.
3. Prefs id 6 / digit 5 / menu enablement.
4. Tests proving non-regression of shipping skins.
