---
title: 'Per-skin zone map pack round-trip and Mappings jump'
type: 'feature'
created: '2026-11-16'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-designer-zones-panel/io-and-live-bridge.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
baseline_commit: 'dfdccc10'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Zone maps edit and push live, but Save / Save as / reload still drop them, and authors cannot jump from a Zones MCS object name into Colour Mappings filtered on that object.

**Approach:** Persist each Designer skin’s used-cell zone map in the pack JSON (field `zoneMap`, same shape as PushPayload), reload with soft entry filtering, and wire MCS object-name activation to switch to Mappings with the existing object macro-filter.

## Boundaries & Constraints

**Always:**
- Ownership per Designer skin (same document unit as palette / mappings).
- Pack field name `zoneMap`; distinct from `colourSetLabels` and `mappings`; serialize used MCS cells only: `(object, zone) → colourSet`.
- Missing / void `zoneMap` on load ⇒ empty map (registry defaults, same as Reset).
- Soft pack load: skip unknown object/zone/colourSet strings; skip unused registry cells; drop entries outside `colourSetCount(object)`; do not fail the whole skin for those.
- Present `zoneMap` that is not an array ⇒ fail that skin/import (same shape rule as push serde).
- Round-trip: Save / Save as → reload restores author used-cell assignments.
- Dirty pack: matrix / Reset already bump Layer → `onChanged` → dirty; keep that cadence (no second dirty path).
- CAP-4: activating an MCS object-name control switches to Colour Mappings and applies the object macro-filter for that object; no Zone column on Mappings rows.
- PushPayload / Look-for-zone overlay path stays as story 1 (do not rebuild a second bridge).
- ASCII-only UI display strings (project display-string rule).

**Never:**
- Omit `zoneMap` from PushPayload (that is the `colourSetLabels` pattern — wrong here).
- Zone column on every Mappings row; replace Colour set.
- Editable exceptions; Module/Instance cascade; GUI click-inspector.
- Story 1/2 redo; cosmetic Zones polish; PR / GitHub push.
- Release packaging of live overlay; second preview/IPC stack.
- Refactor Colour Mappings beyond the filter/jump needed for CAP-4.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Save with edits | Used cells remapped on a Dev skin | Pack skin object includes `zoneMap` array of used entries only | N/A |
| Reload missing map | Skin JSON without `zoneMap` | Empty map ⇒ registry defaults in UI / push | N/A |
| Soft load junk | Unknown object/zone/cs, unused cell, cs outside count | Those entries dropped; valid used cells kept | No hard fail for soft drops |
| Bad shape | `zoneMap` present but not array | Import / skin parse fails | Error string; prior layer policy unchanged |
| Round-trip | setZoneMap → exportToJson → import/replace | Same used-cell assignments restored | N/A |
| CAP-4 jump | Click MCS object name (e.g. Slider) on Zones | Right surface = Mappings; object combo filter = that object | Read-only: still navigates/filters |
| CAP-4 no Zone col | After jump | Mappings columns unchanged (no Zone) | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/Dev/DevSkinJson.cpp` -- `skinToVar` / `skinFromVar`: write/read per-skin `zoneMap` (pack path). Push helpers already serialize payload `zoneMap` — do not regress.
- `Source/GUI/Skins/Dev/DevSkinJsonMapping.cpp/.h` -- Reuse `zoneMapEntryToVar`; add or extend pack-oriented soft load (skip unknown / unused / out-of-range) vs push path that still hard-errors on unknown strings. Keep helpers shared where safe.
- `Source/GUI/Skins/Dev/DevSkinTypes.h` -- `NamedSkin::zoneMap` already exists; no model change expected.
- `Source/GUI/Skins/Dev/DevSkinLayer.cpp` -- `setZoneMap` + `onChanged` already dirty the session; verify only.
- `Tests/Unit/DevSkinLayerJsonTests.cpp` (and/or focused new suite if cleaner) -- Pack round-trip + missing map + soft-drop pins. Push round-trip already covered in overlay / matrix tests — leave push strictness alone.
- `Tools/SkinDesigner/SkinDesignerZonesPanel.*` (+ Matrix) -- Make MCS `rowLabels_` activatable; fire callback with object type/name (not UCS/Exceptions names unless already free).
- `Tools/SkinDesigner/SkinDesignerMappingsPanel.*` (+ ActionRow) -- Public API to set object macro-filter (mirror `revealPastilleMappings` / combo path); rebuild rows + combo selection.
- `Tools/SkinDesigner/SkinDesignerMainComponentSkins.cpp` / MainComponent wire -- On Zones object jump: `setRightWorkSurface(kMappings)` + apply object filter (same spirit as `enterHeldColour` → Mappings).
- Continuity: stories `1-…` and `2-…` under this spec folder; `io-and-live-bridge.md` Skin JSON section.

**Reuse:** existing push `zoneMap` entry shape; Layer `zoneMap` + dirty via `onChanged`; Mappings `objectFilterName_` / `objectMacroFilterShowsGroup`.

**Do not change:** Mappings column layout; Hold/bind gesture; Look-for-zone apply core; PushPayload omit rules for `colourSetLabels`.

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/Dev/DevSkinJson.cpp` (+ Mapping helpers) -- Persist and soft-load per-skin `zoneMap` in pack skin JSON; keep push serde behaviour.
- [x] `Tests/Unit/DevSkinZoneMapPackJsonTests.cpp` (+ CAP-4 filter/column pins) -- Unit-pin pack round-trip, missing⇒empty, soft drops, bad-shape fail; object macro filter + no Zone column.
- [x] `Tools/SkinDesigner/SkinDesignerMappingsPanel.*` -- Expose object macro-filter setter usable from Main (combo stays in sync).
- [x] `Tools/SkinDesigner/SkinDesignerZonesPanel.*` + MainComponent wire -- MCS object-name activation → switch to Mappings + filter that object.
- [x] `python3 Scripts/quality/lint_touched.py` + SkinDesigner/Tests Debug build -- Clean Code gate + compile.

**Acceptance Criteria:**
- Given a Dev skin with a non-default used-cell zone map, when exporting then reloading the Designer pack, then those used-cell assignments are restored and missing `zoneMap` yields registry defaults.
- Given pack entries that are unknown, unused, or outside `colourSetCount`, when loading, then they are dropped without rejecting valid siblings; a non-array `zoneMap` fails parse.
- Given Zones MCS object-name activation, when the control fires, then Colour Mappings is shown filtered to that object and no Zone column appears.
- Given matrix or Reset edits, when the Layer bumps, then pack dirty follows the same path as mapping edits (no separate dirty pipe).

### Review Findings

- [x] [Review][Patch] Ignore popup/non-primary click on MCS object-name jump [`Tools/SkinDesigner/SkinDesignerZonesPanel.cpp:206`]
- [x] [Review][Defer] CAP-4 Zones → Mappings jump has no panel harness — deferred: Designer GUI / manual-smoke pattern (reconfirmed independent review; already in deferred-work from Build)
- [x] [Review][Defer] Companion `io-and-live-bridge.md` still soft-pedals pack `zoneMap` name / soft-vs-push — deferred: companion hygiene (reconfirmed; already deferred from Build)
- [x] [Review][Defer] Story 2 Design Notes still say MCS names are labels only — deferred: stale sibling-story hygiene (reconfirmed; already deferred from Build)

**Rejected**
- false — void/undefined `zoneMapFromVar*` leaves stale `out`: both pack `skinFromVar` and push `pushPayloadFromVar` do `out = {}` before read.
- false — `revealObjectMappings` drops filter when name missing from combo: no such early-return; MCS `toString` names are always in the object combo.
- false — Skin label / Reset hit-area overlap on narrow table: badge width clamps to `resetX - prefixW`; Reset stays at `tableW - resetW`.
- false — Story-2 polish bundled in commit violates Never: intentional fold called out in review scope; do not redo story 1/2.
- false — Always ASCII vs em dash on unused MCS cells: story 3 adds no PluginDisplayNames/footer strings; em dash is story-2 surface (do not reopen).
- false — Soft Always omits non-object skip / incomplete Review Findings checklist: fix would edit the spec under review.
- low — Incomplete pack pins (empty array, void prop, all-junk-only, duplicates) and push unknown-zone/cs pins: everyday path covered; extra pins expand suite without product defect.
- low — Weak no-Zone-column geometry pin / misleading `revealPastilleMappings` symmetry comment / missing CAP-4 cursor-underline affordance: cosmetic or UX beyond CAP-4 success; everyday left-click path works.

## Implementation Notes

- Pack: `packZoneMapToVar` / `readPackZoneMapProperty` → `zoneMapFromVarSoft` (used cells only on write; soft-drop on load). Push keeps strict `zoneMapFromVar`.
- CAP-4: MCS `rowLabels_` mouse listener → `onMcsObjectJumpRequested` → `setRightWorkSurface(kMappings)` + `revealObjectMappings`.
- Tests: `DevSkinZoneMapPackJsonTests`; CAP-4 filter pin in `SkinDesignerMappingsColumnFilterTests`; no-Zone column pin in `SkinDesignerChromeLayoutTests`. Dirty cadence already pinned via `SkinZoneMapLayer` `onChanged`/revision.
- Manual smoke still recommended: Save → reload; click MCS object name → Mappings filtered.
- Review patches: soft load skips non-object array entries; `revealObjectMappings` clears column filters before applying object filter; push unknown-string hard-fail pinned.

## Spec Change Log

## Review Triage Log

- false — Blind: Always ASCII-only clashes with Designer em dash for CAP-4 — verified; story 3 adds no new footer/PluginDisplayNames strings; em dash is story-2/Designer matrix, not this jump.
- false — Blind: mappingsColumnsHaveNoZoneColumn too weak / missing empty-array·duplicate·error-string pack pins / no click affordance — verified as low cosmetic or UX beyond CAP-4 success; rejected (everyday use unlikely / fix adds complexity or only strengthens tests).
- medium — Blind+Edge: `zoneMapFromVarSoft` hard-fails whole import when a `zoneMap` array entry is not an object — verified at `DevSkinJsonMapping.cpp` soft loop; soft siblings rejected. Route: patch.
- medium — Blind: `revealObjectMappings` leaves mapped-colour / column filters active so CAP-4 can AND-stack and hide object rows — verified vs `revealPastilleMappings` clearing the object filter. Route: patch.
- medium — Verification Gap: push `zoneMapFromVar` unknown-string hard-fail unpinned after soft twin — verified; push still calls strict path but no test injects junk. Route: patch.
- defer — Verification Gap+Blind: CAP-4 click/wire path untested in Matrix-Control_Tests — real; Designer GUI / manual-smoke pattern (same as story 2 Zones harness).
- defer — Blind: companion `io-and-live-bridge.md` still vague on pack field name / soft vs push — companion hygiene outside this code patch.
- defer — Blind: story 2 Design Notes still say object names are labels only — stale companion/story hygiene beside CAP-4 wire.

## Design Notes

**Pack vs push:** Same JSON entry shape (`object` / `zone` / `colourSet`). Pack load is intentionally softer than push unknown-string errors (io Soft load). Push apply still validates occupancy via existing `validateZoneMap`.

**CAP-4 gesture:** Single click (or label mouseUp) on MCS row object name — not Hold/bind. UCS / Exceptions labels stay non-jumping in V1 (CAP-4 text is MCS object name).

**Filter API:** Prefer a small `revealObjectMappings(objectDisplayName)` (or equivalent) next to `revealPastilleMappings` rather than inventing a second filter stack.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` (or project equivalent) -- DevSkin JSON / zone map unit tests green.
- `python3 Scripts/quality/lint_touched.py` -- clean on touched `Source/`, `Tests/`, `Tools/SkinDesigner/`.

**Manual checks (if no CLI):**
- Save → quit/reload (or Load pack) restores zone assignments; click Slider (etc.) on Zones → Mappings filtered to Slider.
