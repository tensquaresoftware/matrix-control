---
title: 'Skin ColourSet model + Skin Editor Mappings column'
type: 'feature'
created: '2026-11-01'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '281f98ae286c57d796cd80c27f69f5ca7b33cb7d'
context:
  - '{project-root}/_local/Documents/Recap-ColourSets-et-refactorisation-widgets.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** One object type can only carry a single colour mapping game today, so multi-zone skins (Duo and beyond) either fake extra object types or cannot exist. Skin authors need several colour games on the same Mappings grid without exploding table count. PeakIndicator also shares Led pastilles, so authors cannot edit peak and MIDI leds as separate tables.

**Approach:** Add a first-class `ColourSet` dimension (`k1`…`k4`) to the skin colour key and Skin Editor Mappings. Split `PeakIndicator` into its own skin object (seeded with today’s Led hues) so Mappings shows two tables. Seed Black & Cream so product Look stays visually identical. Do not wire product zones→sets yet, and do not merge ButtonLike objects.

## Decisions (human-approved planning)

- **First-wave count:** `ColourSet::k1`…`k4` (four sets). No Set5+.
- **Multi-set inventory / Mappings rows:** control-like objects — `Slider`, `SliderButtonLike`, `ComboBox`, `ComboBoxButtonLike`, `PopupMenu`, `PopupMenuButtonLike`, `NumberBox`, `Button`, `Toggle`, `Led`, `PeakIndicator`, `ReadOnlyValueField`. Each gets `k1`…`k4` rows. Panels, section/module headers, labels, separators, dialogs, WindowClose, FooterMessage, and displays (`EnvelopeDisplay`, `TrackGeneratorDisplay`, `PatchNameDisplay`) stay `k1`-only rows.
- **Led / PeakIndicator split:** introduce `SkinObjectType::kPeakIndicator` with its own ColourElements (same Black/Cream ARGB as current shared Led rows at seed). `PeakIndicator` widget resolves colours from that object; `Led` keeps MIDI leds. Skin Editor shows distinct Led and PeakIndicator tables. No product zone→set wiring yet.
- **Product intent note (wiring is GPC 3, recorded for inventory fit):** Patch-side green controls → future `k1` (blue); Master sliders/combos → `k2` (orange); Header Input Gain + MIDI Leds + Header PeakIndicator → `k3` (yellow); Settings / ButtonLike / buttons / Hardware Latency / Settings PeakIndicator / NumberBox / ReadOnlyValueField → `k4` (mono dark/light). Toggle, EnvelopeDisplay, TrackGeneratorDisplay are Patch-only hues (no Master orange).
- **Black/Cream seed:** for every multi-set inventory key, seed all four sets with that object’s current Black/Cream pastille (identical hues). PeakIndicator seed copies today’s Led hues. `k1`-only objects keep a single seeded mapping. Runtime fallback: requested set → `k1` → white assert.

## Boundaries & Constraints

**Always:**
- Code name `ColourSet` with values `k1`…`k4` in this wave; never Patch/Master/Settings/Header as set names.
- Same Mappings table per object + new ColourSet column (window width may grow).
- Missing-set lookup: requested set → `k1` → existing white/`jassertfalse` path.
- Old Dev JSON without `colourSet` remains readable (default `k1`).
- Black/Cream compiled + product Look: no visible regression after this ship (seeded sets identical; PeakIndicator still paints like today’s shared Led hues).
- Infographists edit colours of existing sets only; they do not create sets.
- Multi-set inventory limited to the control-like list in Decisions; displays stay `k1`-only.
- Led and PeakIndicator are separate skin objects and separate Mappings tables.

**Never:**
- Product zone→set wiring (GPC 3).
- ButtonLike unification / removing `*ButtonLike` object types (GPC 4).
- DarkPanel / WindowClose / window-background renames (GPC 5).
- Reordering of the full Mappings display list beyond inserting `PeakIndicator` next to `Led` (GPC 1 order otherwise stays).
- Editable zone→set mapping file (YAGNI).
- Real Duo hue differentiation (blue/orange/yellow skins).
- Widget construction options in the skin (scroll, multi-column, …).
- Pre-create empty Set5+.
- Names Accent / Neutral / Family / ColourFamily / Variant for this concept.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Default API call | `getColour(object, property, state)` (no set) | Resolves as `ColourSet::k1` | N/A |
| Missing set in map | Key exists for `k1`, request `k3` | Fallback: requested → `k1` → white assert | Soft fallback; no crash |
| Legacy Dev JSON | Mapping omits `colourSet` | Treated as `k1` on load | Reject only hard schema breaks |
| Compiled seed | Black or Cream create / SSOT seed | Multi-set objects: identical current hues on `k1`…`k4`; PeakIndicator = former Led ARGB; others: `k1` only | N/A |
| Peak vs Led paint | Header Peak + MIDI Led on Black/Cream | Same pixels as before split (shared hues at seed) | N/A |
| Skin Editor tables | Inventory open | Distinct Led and PeakIndicator object groups | N/A |
| Skin Editor Unbound | Inventory keys unbound for a set | Count includes those keys; Export gate still whole-skin | N/A |
| Filter ColourSet | Filter menu selects Set / Unbound colour | AND with Property/State/Mapped; Hold unchanged | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h` — `ColourSet` enum; extend `SkinColourKey` (+ compare / `skinColourKey`); add `SkinObjectType::kPeakIndicator`; 3-arg helpers default to `k1`.
- `Source/GUI/Skins/ISkinColours.h`, `Skin.h`, `Skin.cpp` — set-aware lookup + missing-set fallback; init maps for new object; leave `getComboBox*` / ButtonLike routing untouched (GPC 4).
- `Source/GUI/Skins/SkinColoursWidgetsDisplays.h` (+ list append in `DevSkinColourElementList`) — PeakIndicator ColourElements; stop documenting Led as shared with Peak.
- `Source/GUI/Looks/LookBuilders.h/.cpp`, `Widgets/PeakIndicator.*` — resolve Peak from `kPeakIndicator` (keep `LedLook` shape or thin alias if useful); Led path unchanged.
- `Source/GUI/Skins/Dev/DevSkinTypes.h`, `DevSkinJson.cpp`, `DevSkinKeyNames.*` — optional `colourSet` field; `"PeakIndicator"` name token; schema read compat.
- `Source/GUI/Skins/Dev/DevSkinInventory.*`, `DevSkinSsotSeed.*`, `DevSkinApplier.*` — structured keys × sets; compiled seed including Peak.
- `Tools/SkinEditor/SkinEditorMappingsColumnFilter.h` + panel/filters/rows — ColourSet column; insert `PeakIndicator` beside `Led` in `mappingsObjectDisplayOrder()` only.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` (+ chrome layout tests) — widen Mappings / window for ColourSet column.
- `Tests/Unit/SkinColourSubstrateTests.cpp`, `DevSkinLayerJsonTests.cpp`, `DevSkinMatrixTests.cpp`, `SkinEditorMappingsColumnFilterTests.cpp`, `SkinEditorChromeLayoutTests.cpp` — substrate, Peak≠Led keys, JSON compat, column filters, widths, display-order slot for Peak.
- Do **not** change: zone→set call sites; `ColourChart` token SSOT; Release JSON skin loading; wholesale GPC1 order reshuffle.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/SkinValues.h` (+ `ISkinColours` / `Skin`) — `ColourSet`, 4th key field, defaulted API, missing-set fallback — substrate contract.
- [x] `SkinValues` + `SkinColoursWidgetsDisplays` + `LookBuilders` + `PeakIndicator` + Dev list/names — add `kPeakIndicator`, seed from current Led ARGB, wire Peak paint — Led/Peak split.
- [x] `Source/GUI/Skins/Dev/*` (Types, Json, KeyNames, Inventory, SsotSeed, Applier, Layer as needed) — schema/read compat + seed Black/Cream × sets — Dev SSOT.
- [x] `Tools/SkinEditor/SkinEditorMappings*` + `SkinEditorChromeTokens.h` — ColourSet column, Peak table slot, filters/Hold/Unbound, width tokens — editor UX.
- [x] `Tests/Unit/SkinColour*` / `DevSkin*Json*` / `SkinEditorMappings*` / `SkinEditorChromeLayout*` — fallback, legacy JSON, Peak≠Led, column filters, widths — regression net.
- [x] Register any new test `.cpp` in `Tests/CMakeLists.txt` if added — build wiring.

**Acceptance Criteria:**
- Given Black or Cream compiled skin, when the product UI paints with default `getColour` calls, then pixels match pre-change Look (no Duo hues; Peak still matches former shared Led hues).
- Given a legacy Dev mapping without `colourSet`, when loaded, then it binds as `k1`.
- Given a request for an unmapped higher set when `k1` exists, when `getColour` runs, then fallback returns `k1` colour (not white).
- Given Skin Editor Mappings, when inventory is shown, then Led and PeakIndicator are separate object tables; multi-set controls show `k1`…`k4`; displays/panels show `k1` only; filters/Unbound/Hold still behave.
- Given empty ColourSet filter, when filtering Property/State/Mapped, then AND semantics unchanged.
- Given `lint_touched.py` on touched C++ under Source/Tests/Tools/SkinEditor, when run, then it passes.

## Implementation Notes

## Spec Change Log

## Review Triage Log

- Blind Hunter: schemaVersion stays 1 while export always writes colourSet — medium — verified (DevSkinTypes kSchemaVersion=1; mappingToVar always emits colourSet); old editors can collapse multi-set rows — route patch
- Blind Hunter: legacy import leaves k2–k4 Unbound and blocks Export — medium — verified (inventory expands sets; unboundCount gates export; import only stores present keys as k1) — route patch (fill missing higher sets from k1 on load)
- Blind Hunter + Edge Case Hunter: JSON/setMapping accept k2–k4 on k1-only objects — medium — verified (mappingFromVar has no supportsMultipleColourSets check) — route patch
- Blind Hunter + Verification Gap: inventory test only samples Slider / Envelope / Header — medium — pre-verified VG — route patch
- Blind Hunter: no test that explicit higher set wins over k1 — low — real coverage gap — route patch
- Blind Hunter + Verification Gap: unknown colourSet JSON (e.g. k5) untested — medium — pre-verified VG — route patch
- Blind Hunter: ColourSetModelTests uses mappings.at after soft expects — low — reject (everyday miss unlikely; crash only if prior expects already failed noisily)
- Blind Hunter: Peak/Led paint checked only for Black — low — reject as separate Cream mirror; VG differential-colour case supersedes
- Verification Gap: Peak/Led separation only proven with identical colours — medium — pre-verified — route patch
- Blind Hunter: ColourSet legend 70px truncates with clear badge — low — route patch (short legend "Set" per Design Notes)
- Blind Hunter: stale width comment still says 466 — low — route patch (comment only)
- Blind Hunter: task cites Tests/CMakeLists.txt vs root CMakeLists — false — reject (fix would edit this build's spec)
- Blind Hunter: status still in-progress / expected-only verification — false — reject (status already in-review; process noise)

## Design Notes

**Key shape:** `(object, property, state, colourSet)` with `colourSet` default `k1`. Product call sites keep working without set arguments until GPC 3.

**Fallback:** lookup exact key → if miss and set ≠ `k1`, retry same object/property/state at `k1` → else existing white/`jassertfalse` path.

**Seed:** for each multi-set compiled `ColourElement`, write mappings for `k1`…`k4` with the same Black/Cream pastille as today. PeakIndicator elements copy current Led ARGB. `k1`-only elements keep one mapping. ButtonLike objects keep their own hues on every set until GPC 4 merges types.

**Displays:** `EnvelopeDisplay` / `TrackGeneratorDisplay` stay `k1`-only so Duo can paint them Patch-blue without Master-orange rows.

**Led vs Peak:** separate objects and Mappings tables; Header yellow vs Settings grey for Peak is future ColourSet wiring on `PeakIndicator`, independent of MIDI `Led` hues.

**Editor:** ColourSet column after State; labels `k1`…`k4` (short “Set N” OK). Insert PeakIndicator next to Led in display order. Do not expose product zone names.

**Naming collisions:** keep `Skin::ColourVariant` (Black/Cream) and widget Blue/Orange header variants distinct from `ColourSet`.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build OK
- `./Builds/macOS/.../Matrix-Control_Tests --category SkinColourSubstrate` (and DevSkin / SkinEditorMappings / SkinEditorChrome categories as named in suite) -- expected: green
- `python3 Scripts/quality/lint_touched.py` -- expected: OK on touched C++

**Manual checks:**
- Open Skin Editor Mappings: ColourSet column visible; distinct Led and PeakIndicator tables; Black/Cream compiled skins show seeded sets bound; plugin Look Black/Cream unchanged at a glance (Header leds + peak, a green combo, a ButtonLike control).
