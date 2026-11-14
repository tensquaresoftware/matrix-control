---
title: 'Shared zone registry and live Look-for-zone overlay'
type: 'feature'
created: '2026-11-13'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/zone-registry-contract.md'
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/io-and-live-bridge.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/thin-bridge.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
baseline_commit: 'b4aae48f'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Product geography (Patch / Master / Header / Modales → drawer) still lives only as frozen helpers and scattered exception call sites. The Skin Designer cannot push a per-skin zone map, so third-party authors cannot rewire geography live into connected Standalone.

**Approach:** Expose one shared zone registry (occupancy, defaults, named exception rows) and extend the existing Designer→Standalone Dev bridge so a zone map on `PushPayload` drives Look-for-zone for used MCS cells when a Dev overlay is active.

## Boundaries & Constraints

**Always:**
- Registry content matches `zone-registry-contract.md` (occupancy table, default matrix, four exception rows).
- `cs1…csN` stay technical drawer ids; zone names are geography only.
- Zone map on `PushPayload` (must not be omitted like `colourSetLabels`).
- Used MCS cells only in the map; unused cells are not assignable.
- Look-for-zone with no Dev overlay keeps today’s registry defaults (Release/compiled path unchanged).
- Exceptions remain registry/call-site behaviour in this story (read-only; not overlay-editable).
- After a successful Dev push, `updateSkin()` / look rebuild path must pick up the new zone→cs resolution (same refresh cadence as mapping pushes).
- Unit tests pin registry occupancy/defaults/exceptions and overlay-vs-default resolution; extend existing push JSON round-trip coverage for the zone map field.

**Never:**
- Zones panel UI, Mappings Colour set changes, Zone column on Mappings rows.
- Editable exceptions, Module/Instance cascade, GUI click-inspector.
- Shipping live zone overlay into Release plugin packaging.
- Designer pack JSON persistence round-trip (story 3) — in-memory / push only here is enough if Layer can hold a map for tests and later UI.
- Second preview stack or new IPC; extend the existing Dev bridge only.
- Inventing exception rows the plugin does not apply.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| No overlay | Compiled / cleared Dev overlay | `Look*ForZone` equals registry defaults | N/A |
| Overlay remap used cell | Push map Slider+Patch→cs2 | Patch sliders resolve Slider cs2 colours after skin refresh | N/A |
| Unused cell in payload | e.g. ComboBox+Header assigned | Push rejected; overlay unchanged | Machine-readable apply error |
| Invalid cs for object | Led→cs3 | Push rejected; overlay unchanged | Same as above |
| Missing zone map on push | Legacy payload without field | Treat as registry defaults (empty overlay map) | N/A |
| Exception sites | Computer Patches / logo / Header Undo | Still follow exception / UCS rules; not rewritten by MCS zone map | N/A |
| Clear overlay | Disconnect / clearDevSkinOverlay | Zone resolution returns to registry defaults | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/SkinValues.h` -- Today’s `SkinZone`, `zoneColourSet`, `kMultiSetSkinObjects`, `colourSetCount`, `kMonoMenuExceptionZone`. Extend or sibling registry for `isZoneUsed` + iterable exception rows; keep default matrix behaviour for used cells.
- `Source/GUI/Looks/LookBuilders.h` -- `sliderLookForZone` / `comboBoxLookForZone` / `popupMenuLookForZone` call `zoneColourSet` today; must resolve through overlay-aware helper when Dev overlay map is active.
- `Source/GUI/Skins/Dev/DevSkinTypes.h` -- `NamedSkin`, `PushPayload` (palette + mappings only). Add per-skin / payload zone map (used cells only).
- `Source/GUI/Skins/Dev/DevSkinLayer.cpp` -- `makePushPayloadForSkin` copies palette/mappings; must include zone map from the skin.
- `Source/GUI/Skins/Dev/DevSkinJson.cpp` -- `pushPayloadToVar` / `FromVar`; add zone map serde for push (pack file persistence can wait for story 3).
- `Source/GUI/Skins/Dev/DevSkinApplier.cpp` -- `applyPushToSkin` validates mappings then paints Skin colours; install/validate zone overlay as part of apply (atomic with colour apply).
- `Source/GUI/Skins/Dev/DevSkinStandalonePaintState.cpp` + `Source/GUI/PluginEditorSkinScale.cpp` -- `applyPush` / `applyDevSkinPush` then `updateSkin()`; `clearOverlayToCompiled` / `clearDevSkinOverlay` must clear zone overlay with colours.
- Exception call sites (do not invent new ones): `ComputerPatchesPanel.cpp`, `HeaderLogoPopupMenu.cpp`, Header Undo/Redo/Panic Button UCS — display/registry only this story.
- Tests: `Tests/Unit/SkinColourWiringCoverageTests.cpp` (matrix pins), `Tests/Unit/DevSkinMatrixTests.cpp` (push round-trip / apply). Add focused registry + overlay resolution tests; register new `.cpp` in CMake if needed.
- Parent contract: `_bmad-output/specs/spec-skin-designer-zones-panel/SPEC.md` CAP-5 / CAP-6 + companions.

**Reuse:** existing Dev bridge, `Look*ForZone`, `zoneColourSet` defaults, inventory helpers.

**Do not change:** `Tools/SkinDesigner/*` UI panels (story 2+), Mappings Colour set column, Release packaging gates for the bridge.

## Tasks & Acceptance

**Execution:**
- [x] Shared registry surface (`SkinValues.h` and/or sibling header/cpp) -- occupancy + default matrix + named exception rows per companion -- single read API for Designer (later) and tests.
- [x] Overlay-aware zone→cs resolution used by `LookBuilders` Look-for-zone helpers -- Dev overlay map wins for used MCS cells; else registry defaults.
- [x] `PushPayload` + `NamedSkin` + `makePushPayloadForSkin` + push JSON serde -- carry / validate zone map on the live pipe.
- [x] `applyPushToSkin` + Standalone/`PluginEditor` clear paths -- install and clear overlay atomically with Dev skin apply/clear.
- [x] Unit tests -- pin registry tables; overlay remap; reject unused/invalid; missing map = defaults; push JSON round-trip includes zone map.
- [x] CMake -- register any new translation units in plugin/tests source lists as required.

**Acceptance Criteria:**
- Given the registry, when querying occupancy, then Slider has four used zones; Combo/Popup omit Header; Led/Peak omit Patch/Master; exception rows match the companion table.
- Given no Dev overlay, when `Look*ForZone` runs, then ColourSets match today’s `zoneColourSet` defaults for used cells.
- Given a Dev push with Slider Patch→cs2 (and valid palette/mappings), when Standalone/`PluginEditor` applies and refreshes looks, then Patch slider looks resolve Slider cs2.
- Given a push that assigns an unused cell or an out-of-range cs, when apply runs, then it fails without installing a partial overlay.
- Given clear Dev overlay / compiled selection, when Look-for-zone runs, then resolution is registry defaults again.
- Given push JSON round-trip of a payload with a zone map, when decoded and applied, then the same used-cell assignments are present.

## Implementation Notes

- Added `SkinZoneRegistry.h`, `SkinZoneOverlay.h/.cpp`; Look-for-zone uses `resolveZoneColourSet`.
- Exception call sites keep `zoneColourSet` / `kMonoMenuExceptionZone` so mono menus ignore MCS Modales remaps.
- Follow-up in same Build: Header / Settings Led+Peak `setSkin` paths call `resolveZoneColourSet` + `setColourSet` so geography remaps refresh on Dev push (not only Slider/Combo/Popup Looks).
- Verification: `Matrix-Control_Tests --category SkinZoneRegistryOverlay` 0 failures; `lint_touched.py` OK; Standalone Debug build OK.
- Review patches: SkinDesigner CMake link for SkinZoneOverlay; atomic PluginEditor Dev push; clearDevSkinOverlay always clears zone overlay; compiled apply clears overlay; zoneMapFromVar clears out; exception-under-overlay look assertions.

## Spec Change Log

## Review Triage Log

- high — Blind: SkinZoneOverlay missing from MATRIX_SKIN_DEV_LAYER_SOURCES — verified; patched (added to Dev layer list).
- high — Blind+Edge: PluginEditor applyDevSkinPush destroys prior overlay before failed apply — verified; patched (temp unique_ptr, swap on success only).
- high — Edge: clearDevSkinOverlay early-return skips clearSkinZoneOverlay — verified; patched (always clear zone overlay first).
- medium — Edge: applyPushToSkin compiled kind leaves zone overlay — verified; patched (clearSkinZoneOverlay on compiled path).
- medium — Blind+VG: exception sites vs Look-for-zone under Modales remap untested — verified gap; patched (test asserts LookFromSkin+zoneColourSet stays cs3 while LookForZone follows remap).
- medium — Blind: zoneMapFromVar does not clear out — verified; patched (out.clear() before parse).
- medium — VG: Header/Settings Led+Peak setColourSet refresh untested in panel harness — verified gap; deferred (panels not in unit suite; resolveZoneColourSet pinned).
- medium — VG: PluginEditor clear paths not unit-tested — verified gap; deferred (StandalonePaintState clear covered; editor is thin Dev mirror).
- low — Blind: shared invalid_zone_map code for unused vs out-of-range — real but clients still get ok=false; rejected (message distinguishes; separate codes not required by AC).
- low — Blind: incomplete default-matrix pins / Peak remap test — partial Peak/Led resolve covered; rejected as everyday-use unlikely with current AC focus.
- false — Blind: SkinZoneOverlay in PLUGIN_SOURCES ships Release overlay — LookBuilders always need resolveZoneColourSet; Dev-only population remains gated; no live Release pipe.
- false — Blind: exception where strings ASCII vs companion em dash — ASCII matches project display-string rules; companion prose may use typographic dash.
- false — Blind: hasSkinZoneOverlay true with empty map — intentional Dev-session active with registry defaults.
- maybe-false — Blind: multi-instance global overlay cross-talk — same pattern as other Dev session state; deferred note only if hosts host multiple editors.
- defer — Blind: UCS iterable listing / kZones vs kAllSkinZones static assert / DevSkinMatrixTests zoneMap pin — not blocking V1; recorded in deferred-work.

## Design Notes

**Overlay holder (agent decision):** Keep a Dev-session zone overlay (set on successful push, cleared with Dev overlay clear) consulted by Look-for-zone helpers — avoid threading a map through every call site and avoid Release `ISkin` API churn. Message-thread / UI-thread only, same as today’s Dev apply.

**Persistence split:** In-memory `NamedSkin` zone map + push serde in this story; Designer pack JSON round-trip remains story 3. Layer setter(s) for tests/UI prep are allowed.

**Validation:** Invalid zone map entries fail the whole push (same atomic spirit as unknown pastille), rather than silently dropping on the live pipe.

**Static assert note:** Existing `SkinZone`/`ColourSet` order assert documents Slider default casting; free remapping via overlay must not assume zone≡cs at runtime for non-default maps.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` (or project’s usual unit-test target) -- build + run registry/overlay/push tests green.
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++ under `Source/` and `Tests/`.

**Manual checks (if no CLI):**
- Optional smoke once a Layer setter exists: push a remapped zone map into Standalone Dev and confirm Patch vs Master slider drawers swap without Designer Zones UI (story 2).

### Review Findings

- [x] [Review][Patch] Settings Led/Peak stay on registry drawers when Settings first opens under an active Dev zone remap — fixed: construct with `resolveZoneColourSet`; always `setSkin` after attach on open
- [x] [Review][Patch] `duplicateSkin(..., copyMappings=false)` keeps source `zoneMap` while clearing mappings/palette — fixed: clear `zoneMap` with mappings wipe + unit pin
- [x] [Review][Defer] Header/Settings Led+Peak `setColourSet` and PluginEditor clear paths lack panel/editor harnesses [`Tests/Unit/SkinZoneRegistryOverlayTests.cpp`] — deferred: pre-existing suite gap; resolve + StandalonePaintState covered; already in deferred-work
- [x] [Review][Defer] Exception mono-menu call sites not pinned beyond helper pattern [`ComputerPatchesPanel.cpp`, `HeaderLogoPopupMenu.cpp`] — deferred: helper contract tested; panels not in Matrix-Control_Tests
- [x] [Review][Defer] Process-wide zone overlay singleton may cross-talk across editors [`SkinZoneOverlay.cpp:13-17`] — deferred: maybe-false; same Dev-session pattern as colour overlay; settle if hosts ship multi-editor Dev
- [x] [Review][Defer] Companion docs not updated for live `zoneMap` field / Code Map still cites older test files — deferred: agent-context / companion edit outside this code patch
- [x] [Review][Defer] Optional registry hygiene (`kZones` vs `kAllSkinZones`, UCS iterable, DevSkinMatrixTests zoneMap) — deferred: already recorded; not blocking ACs

#### Rejected

- false — Empty active overlay (`hasSkinZoneOverlay` true, empty map) vs cleared: intentional Dev-session defaults.
- false — Release links `SkinZoneOverlay` / always routes Look-for-zone through `resolveZoneColourSet`: population remains Dev-gated; no live Release pipe.
- false — Duplicate-symbol risk from listing overlay in PLUGIN_SOURCES + MATRIX_SKIN_DEV_LAYER_SOURCES: no target concatenates both lists today.
- false — `zoneMapFromVar` skips occupancy validation: `validateZoneMap` in apply rejects unused/out-of-range before install.
- false — `zoneMapFromVar` void path leaves dirty `out`: sole caller `pushPayloadFromVar` does `out = {}` first; live push never retains stale map.
- false — Other Led/Peak hosts missed: only Header + Settings construct those widgets.
- low — Malformed zoneMap JSON paths under-tested: serde already errors on non-array / bad entry / unknown strings; everyday push path covered.
- low — Looser `assertExceptionRows` / missing Peak resolve pin: partial coverage enough for current ACs; Peak panel harness already deferred.
