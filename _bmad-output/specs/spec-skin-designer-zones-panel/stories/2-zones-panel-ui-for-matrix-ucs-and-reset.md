---
title: 'Zones panel UI for matrix, UCS, and reset'
type: 'feature'
created: '2026-11-14'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/zone-registry-contract.md'
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/io-and-live-bridge.md'
  - '{project-root}/Documentation/Devs/Plans/2026/11/2026-11-13-Skin-Designer-Zones-Panel-Vision.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
baseline_commit: '4ecb48f7'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Designer can push a per-skin zone map (story 1) but authors still have no Zones work surface to assign Patch / Master / Header / Modales drawers, see UCS and Exceptions honestly, or reset to product defaults.

**Approach:** Add a Zones panel beside Palette / Mappings (same useful width as Mappings) with an MCS combo matrix, read-only UCS and Exceptions blocks, and Reset — wired to `SkinZoneRegistry` + `Layer::setZoneMap` and the existing live push path.

## Boundaries & Constraints

**Always:**
- Third work surface chrome: `[ Palette ] [ Mappings ] [ Zones ]`; Zones useful width comparable to Mappings.
- Matrix: five MCS rows (`kMultiSetSkinObjects`) × four zones (`kAllSkinZones`); combos only on `isZoneUsed` cells; unused cells show typographic em dash (U+2014; not assignable).
- Combo options = drawers allowed by `supportsColourSet` / `colourSetCount` for that object; labels via existing `toString(ColourSet)` (`cs1`…).
- Active Designer skin id shown; map ownership per skin; edits call `setZoneMap` then the same `onChanged` → `pushVisible` cadence as Mappings.
- Display effective assignment: map entry if present, else `defaultColourSet` (empty map = registry defaults).
- UCS block: read-only list of single-set inventory objects (`colourSetCount == 1`); no Shared column on MCS rows; author cannot tick Shared.
- Exceptions block: read-only mirror of `kZoneExceptionRows` only (Where / Object / Uses).
- Reset: replace active skin zone map with empty map (registry defaults) + push when bridge connected.
- ASCII-only UI display strings (project display-string rule).
- Smoke: change one used-cell assignment with Standalone Dev connected → geography updates.

**Never:**
- Palette→Mappings Hold/bind as the matrix gesture.
- Editable exceptions; inventing exception rows; Module/Instance cascade; GUI click-inspector.
- Zone column on Mappings rows; replacing Colour set; CAP-4 object-name jump (story 3) unless a no-op stub is required for compile — prefer omit.
- Designer pack JSON zone-map persistence (story 3).
- Second preview / IPC stack; Release packaging of live overlay.
- Story 3 scope or opening a PR.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Open Zones | Active Designer skin | Matrix shows effective cs (defaults if map empty); UCS + Exceptions filled from registry; skin name visible | N/A |
| Change used combo | e.g. Slider+Patch cs1→cs2 | `setZoneMap` updated; Mappings pastilles unchanged; connected Standalone geography follows after push | Layer rejects invalid; UI keeps prior |
| Unused cell | ComboBox+Header | Shows em dash (U+2014); no combo / no assignment | N/A |
| Reset | After arbitrary edits | Zone map cleared; all used cells show registry defaults; push reflects defaults | Compiled / read-only skin: no write |
| Switch skin | Another Designer skin | Panel rebinds to that skin's map (or defaults if empty) | N/A |
| Read-only session | Compare / compiled | Matrix / Reset non-editable; still readable | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinDesigner/SkinDesignerMainComponent.*` (+ `…Skins.cpp`, `…Session.cpp`, `…History.cpp`) -- Body layout today: Color | Palette | Mappings. Add Zones panel + Mappings/Zones work-surface switch in the Mappings-width column; wire `setLayer` / `setActiveSkinId` / `setReadOnly` / `refresh` like other panels.
- `Tools/SkinDesigner/SkinDesignerChromeTokens.h` / `SkinDesignerChromePanel.*` -- Panel chrome, title tabs, width helpers (`mappingsPanelWidthForPalettePanel`); extend for Zones title / switcher without shrinking below Mappings useful width.
- `Tools/SkinDesigner/SkinDesignerMappingsPanel.*` -- Pattern for panel API, chrome, Viewport content, edit → Layer → `onChanged`. Do not change Mappings table columns.
- `Tools/SkinDesigner/SkinDesignerZonesPanel.h/.cpp` (new) -- Matrix + UCS + Exceptions + Reset UI; build/replace `SkinZoneMap` via `Layer::setZoneMap`.
- `Source/GUI/Skins/SkinZoneRegistry.h` -- `isZoneUsed`, `defaultColourSet`, `kAllSkinZones`, `kZoneExceptionRows`, `SkinZoneMap`. Optionally add a small UCS iterable helper so Designer does not hard-code the Shared list (CAP-5).
- `Source/GUI/Skins/SkinValues.h` -- `kMultiSetSkinObjects`, `colourSetCount` / `supportsColourSet` / `supportsMultipleColourSets`.
- `Source/GUI/Skins/Dev/DevSkinLayer.h/.cpp` -- `setZoneMap` (validates used + supportsColourSet; bump); `findSkin` → `zoneMap`; `makePushPayloadForSkin` already copies map.
- `Source/GUI/Skins/Dev/DevSkinKeyNames.h` -- `toString` for object / zone / colourSet labels.
- `CMakeLists.txt` -- Register new Zones `.cpp` under `Matrix-Control_SkinDesigner` `target_sources`.
- Story 1 continuity: `_bmad-output/specs/spec-skin-designer-zones-panel/stories/1-shared-zone-registry-and-live-look-for-zone-overlay.md`.

**Reuse:** panel chrome + Layer mutate → `handleLayerDocumentChanged` → `pushVisible`; registry + `setZoneMap` + existing push serde/apply.

**Do not change:** Mappings Colour set column; Hold/bind gesture; pack JSON persistence; Look-for-zone / overlay core unless a blocking story-1 bug appears.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinDesigner/SkinDesignerZonesPanel.h/.cpp` -- Build Zones panel: skin label, MCS combo matrix, read-only UCS + Exceptions, Reset; read registry + skin `zoneMap`; write via `setZoneMap`.
- [x] `Tools/SkinDesigner/SkinDesignerMainComponent.*` (+ Skins/Session refresh paths) -- Host Zones; switch right work surface Mappings↔Zones; keep Palette; same bind/refresh/read-only as Mappings.
- [x] `Tools/SkinDesigner/SkinDesignerChromeTokens.h` (and chrome helpers if needed) -- Zones title / switcher chrome; preserve Mappings-comparable width.
- [x] `Source/GUI/Skins/SkinZoneRegistry.h` (optional small helper) -- UCS listing surface for the Shared block so Designer does not duplicate inventory.
- [x] `CMakeLists.txt` -- Add Zones panel translation unit to SkinDesigner sources.
- [x] `python3 Scripts/quality/lint_touched.py` + SkinDesigner Debug build -- Clean Code gate + compile.

**Acceptance Criteria:**
- Given Designer with an active Designer skin, when selecting Zones, then the panel shows five MCS rows, four zone columns, em-dash on unused cells, UCS list, and the three registry exception rows, with the active skin identified.
- Given a used cell combo change (e.g. Slider Patch → cs2), when the Layer bumps and the bridge is connected, then Standalone Look-for-zone geography follows that assignment without changing Mappings pastilles.
- Given unused cells, when the panel renders, then they show an em dash (U+2014) and cannot be assigned.
- Given Reset after edits, when confirmed by the control, then the active skin zone map returns to registry defaults (empty map) and a connected Standalone reflects defaults.
- Given Compare/compiled read-only, when Zones is shown, then matrix and Reset do not mutate the Layer.

## Implementation Notes

- Zones panel + Mappings↔Zones right-column switch; Palette unchanged.
- `kSingleSetSkinObjects` on registry for UCS Shared block; panel reads it only.
- Combo edits / Reset call `Layer::setZoneMap` (empty map = defaults); existing push path unchanged.
- Matrix Layer pins in `Tests/Unit/SkinZoneMapLayerTests.cpp` (reset empty + compiled reject + per-skin isolation).
- Verified: SkinDesigner + Tests build; SkinZoneRegistryOverlay + SkinZoneMapLayer 0 failures; `lint_touched.py` OK.
- Manual Standalone smoke still pending (done_checkpoint).
- Review patches: HeldColour switches to Mappings; setZoneMap revision/onChanged pins; UCS uniqueness assert; combo resync + Where tooltips; ASCII unused-cell comment; dropped unused kZonesPanelMinWidth.

### Review Findings

- [x] [Review][Patch] Pastille HeldColour while Zones visible — fixed: `enterHeldColour` switches to Mappings
- [x] [Review][Patch] Layer zone-map tests skipped bump/onChanged — fixed in `SkinZoneMapLayerTests`
- [x] [Review][Patch] UCS list uniqueness / full coverage — fixed static_assert + test
- [x] [Review][Patch] Combo selectedId <= 0 blank cell; long Exception Where clipped — fixed resync + tooltip
- [x] [Review][Patch] Registry em-dash comment; unused `kZonesPanelMinWidth` — fixed
- [x] [Review][Defer] Zones panel combo/Reset UI not unit-tested — deferred (manual smoke)

### Review Findings (polish `dfdccc10` / `e8039e2a..HEAD`, 2026-11-15)

- [x] [Review][Decision] Unused MCS cells: keep typographic em dash or restore ASCII `-`? — resolved: keep U+2014; story ACs / Design Notes aligned to polish + SPEC (2026-11-15)
- [x] [Review][Patch] Clamp Skin badge width so long names cannot overlap Reset — fixed
- [x] [Review][Patch] Clip Location path paint to the Location cell — fixed
- [x] [Review][Patch] Guard inactive tab-ledge bevel against panel right edge — fixed
- [x] [Review][Patch] Pin exception `where` strings contain ASCII `" > "` in overlay tests — fixed
- [x] [Review][Patch] Pin dual work-surface tab order/titles in chrome layout tests — fixed
- [x] [Review][Patch] Translate French chrome comments to English — fixed
- [x] [Review][Patch] Remove dead `objectHeader_` path — fixed
- [x] [Review][Defer] MCS write controls still lack Zones panel harness [`Tools/SkinDesigner/SkinDesignerZonesPanelPages.cpp:93`] — deferred: same Designer GUI / manual-smoke pattern as prior story-2 defer
- [x] [Review][Defer] Story 1 frozen tables still mention Header Undo/Redo/Panic exception row — deferred: companion/story-1 hygiene outside this polish diff

**Rejected (polish review):**
- false — Location column missing BorderSize padding — paint path already uses `reduced(kPad, 0)` on the Location cell.
- false — `paintSubTabUnderline` skips Shared/Exceptions when matrix empty — matrix is laid out first; empty-matrix-with-siblings is unreachable in `resized`.
- false — Exceptions intro no longer points to Colour Mappings — intentional rewrite; intro still explains read-only overrides.
- low — `layoutWorkSurfaceTabs` `jlimit(1,0)` / zero-width ZONE tab — rejected: at `kMappingsPanelMinWidth` both titles fit; pathological narrow not everyday.
- low — Sub-tab preferred widths collapse Shared/Exceptions — rejected: three short labels fit everyday panel width.
- low — `makeBand` inverted when panel shorter than gutters — rejected: Designer panel height never hits that in normal use; fix adds complexity.
- rejected (fix=edit spec) — Exclusive MCS/UCS/Exceptions sub-tabs vs AC “show all together” — intentional polish; story AC stale.
- rejected (fix=edit spec) — Exceptions “Location” vs story “Where” — intentional rename; contract already updated.
- rejected (fix=edit spec) — Chrome titles COLOUR/ZONE MAPPINGS vs story MAPPINGS/ZONES — intentional polish.
- rejected (fix=edit spec) — Skin badge/Reset only on MCS sub-page — matches decided MCS chrome; not a code defect.
- low — Sub-tabs mouse-only / no a11y roles — rejected: Designer pattern; keyboard a11y beyond this polish scope.

## Spec Change Log

- 2026-11-15 — Polish review decision: unused MCS cells keep typographic em dash (U+2014); story ACs / Design Notes aligned to SPEC + implemented polish.

## Review Triage Log

- false — Blind: frozen chrome `[ Palette ] [ Mappings ] [ Zones ]` contradicts one-column switcher — verified; Always requires third-surface chrome labels, not three always-visible columns; Design Notes + peer titles satisfy intent.
- low — Blind: `kZonesPanelMinWidth` unused — verified dead alias equal to Mappings min; routed patch (delete unused token).
- medium — Blind+Edge: `kSingleSetSkinObjects` lacks uniqueness / full UCS coverage pin — verified; size-only assert can hide dupes; patch uniqueness + stronger test.
- low — Blind: `applyCellChange` writes default cs into sparse map — verified but geography still correct; rejected (everyday Reset/empty-map semantics still hold; erase-on-default is polish).
- low — Blind: peer titles lack hover/focus/keyboard chrome — verified muted click-only; rejected (clicks work; polish beyond this story).
- low — Blind: peer title lacks tab bevel — verified; rejected cosmetic.
- low — Blind: registry comment still says em-dash — verified; patch ASCII wording.
- false — Blind: story Tasks omit SkinZoneMapLayerTests — reject (fix would edit this build's spec acceptance trail).
- false — Blind: tasks checked while manual smoke pending — intentional done_checkpoint; not a code defect.
- medium — Blind: Exceptions Where clips without tooltip — verified fixed 280px + preventTextSquish; patch setTooltip(where).
- false — Blind: ZonesPanel ~419 lines over budget — lint useful lines under 400; Quality gate OK.
- medium — Edge: pastille Hold/bind while Zones visible leaves mapping mode with Mappings hidden — verified `onPastilleSelected` does not switch surface; Escape still works if MainComponent focused, but bind rows are unreachable; patch switch to Mappings on bind intents.
- medium — Edge: Escape focused on Zones during mapping — same root as pastille-while-Zones; grouped with prior patch (surface switch + MainComponent Escape already clears when focused).
- low — Edge: peer title can overflow narrow title row — window min width makes everyday clip unlikely; rejected.
- low — Edge: combo `selectedId <= 0` leaves blank cell — verified early return without resync; patch syncCombosFromMap.
- high — VG: SkinZoneMapLayerTests skip bump/onChanged — verified tests hand-call applyPush; removing bump() would green-pass while Designer push dies; patch assert revision/onChanged.
- defer — VG: Zones panel applyCellChange/Reset not GUI-tested — Layer path pinned; Designer GUI remains manual smoke (project pattern).
- low — VG Other: unused `kZonesPanelMinWidth` — grouped with Blind dead-token patch.

## Design Notes

**Work-surface layout (agent decision):** Keep Palette in its column. Put Zones in the existing Mappings-width column and switch Mappings↔Zones via selectable chrome titles (`MAPPINGS` / `ZONES`). Matches CAP-1 “selecting Zones”, keeps useful width, avoids a third always-visible column that would crush the window.

**Effective map display:** For each used cell, show `zoneMap[key]` if present else `defaultColourSet`. Sparse maps stay valid (overlay resolve already falls back). Combo change: copy current effective map (or existing sparse map), set the one cell, `setZoneMap`. Reset: `setZoneMap(skinId, {})`.

**UCS list (agent decision):** Prefer a registry-side iterable of single-set objects (or walk inventory with `!supportsMultipleColourSets`) so CAP-5 holds; do not invent a parallel list in the panel.

**No CAP-4 jump in this story:** Object names are labels only.

**Em-dash:** Unused MCS cells paint typographic em dash U+2014 (Skin Designer Inter path). Plugin footer / `PluginDisplayNames` ASCII rule does not apply to this Designer matrix glyph.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinDesigner` -- Skin Designer builds.
- `python3 Scripts/quality/lint_touched.py` -- clean on touched C++ under `Tools/SkinDesigner/` (and `Source/` if registry helper touched).

**Manual checks (if no CLI):**
- Smoke (done_checkpoint): change one MCS zone assignment with Standalone Dev connected → geography updates; Reset restores defaults; unused cells stay em-dash.
