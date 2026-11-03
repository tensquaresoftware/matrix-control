---
title: 'Skin Editor Build B — Mappings ergonomics'
type: 'feature'
created: '2026-11-03'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '7907e93156c4f068d9135e153d299e35a679ae80'
context:
  - '{project-root}/_local/Documents/Recap-Skin-Editor-Mappings-Ergonomie-et-Link.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Mapping authors still fight the Mappings chrome: every header click opens a filter (no sort), Object only scrolls/highlights a table, Colour set shows cryptic `kN` with a cramped "Set" header, and Hold sits too close to Object.

**Approach:** Make left-click headers sort (one active sort, after filters) and right-click open the existing filter menu with amber clear + sort pictos; turn Object into a single-table macro-filter with `- All objects -` and a yellow reset; rename/widen Colour set with shared free labels `[kN] …` persisted as editor-only skin metadata; push Hold right for air. No Link column.

## Decisions (planning)

- **Sort scope:** one panel-wide sort column + direction (shared sticky legend); applied independently to each visible object table's filtered rows. Left-click cycles that column A→Z ↔ Z→A; switching column replaces the previous sort.
- **Sort pictos:** active = yellow square + down (A→Z) or up (Z→A); inactive = grey square + subtle dual arrow (match existing chrome greys / attention yellow).
- **Filter gesture:** right-click (or platform popup-menu click) on header opens current PopupMenu filter; left-click never opens filter. Per-column clear badge stays when that filter is active (gap after sort picto).
- **Object macro-filter:** combo item 1 = `- All objects -`; other items = object types in current display order. Selection shows only that object's table. Yellow clear badge right of combo when not All → resets to All. Remove focus outline / scroll-into-place jump when a single object is selected (and drop scroll-as-primary behaviour entirely).
- **Colour set labels:** four shared strings on the NamedSkin (indices `k1`…`k4`); display `[k1]` badge + label text (badge-only if empty). Double-click Colour set cell edits that index's shared label (does not remap pastille). JSON optional object `colourSetLabels` on skin via `skinToVar`/`skinFromVar`; omit empty keys; never copy into `PushPayload`. No `schemaVersion` bump.
- **Layout:** rename legend "Set" → "Colour set"; widen Colour set column (multiples of 5); increase `kMappingsObjectToHoldGap`; grow Mappings panel / window tokens if needed so tables stay readable. ASCII-only UI strings.
- **Label length:** hard max **16** characters at edit time (`setInputRestrictions`); no silent mid-display truncation. Keep full Build B scope in one spec (token overrun accepted).

## Boundaries & Constraints

**Always:**
- Technical ColourSet identity stays `k1`…`k4` in code, mapping JSON, and runtime.
- Labels are document-global per index (not per row / per object type).
- Editor metadata must not break runtime import or Standalone push (whitelist / payload omission).
- Reuse existing column-filter AND model, amber clear badge, and scroll-anchor preserve on rebuild.
- Skin Editor Clean Code gate (`lint_touched.py` includes `Tools/SkinEditor/`).

**Never:**
- Link column, link groups, or cross-object link policy (Build C).
- ProgressBar / NumberBox caret inventaire (Build A — already done; do not rework).
- Replace `kN` with zone names in the model; per-row or per-object labels; Skin Studio web.
- Store labels only in session JSON (they must travel with Export/Import skin docs).
- Bump `schemaVersion` solely for optional labels.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Sort after filter | Filter State=On; left-click Property | Visible On rows sorted A→Z (then Z→A on second click); hidden rows stay out | N/A |
| Filter via right-click | Right-click Mapped colour header | Existing filter menu; left-click does not open it | N/A |
| Object single | Choose Slider in Object combo | Only Slider table visible; no focus ring / scroll jump; yellow Object clear shown | N/A |
| Object reset | Click Object clear badge | Combo → `- All objects -`; all tables visible again | N/A |
| Label edit | Double-click Colour set cell on `k2` row; type up to 16 chars | All `k2` rows show `[k2] <text>`; pastille mapping unchanged | Input hard-capped at 16; excess keystrokes rejected |
| Label empty | Clear label text for `k1` | Display badge `[k1]` only; JSON omits `k1` key | N/A |
| Persist / push | Export/import skin; push to Standalone | Labels round-trip in editor skin JSON; push payload has no labels; plugin paints as today | Unknown extra keys ignored on read; labels must not fail import |
| Hold air | Default action row | Larger gap Object → Hold; no Link column | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMappingsPanelFilters.cpp` — sticky legend paint ("Colour set"), hit targets for sort/filter/clear; split left-click sort vs right-click `showColumnFilterMenu`; sort/filter pictos.
- `Tools/SkinEditor/SkinEditorMappingsPanel.cpp` / `.h` — row paint for `[kN]` badge + label; double-click label edit; remove/gate `paintFocusedObjectOutline` when single-object filter; sort state on panel.
- `Tools/SkinEditor/SkinEditorMappingsPanelRows.cpp` — build filtered then sorted rows; stop `scrollFocusedObjectIntoPlace` as Object primary behaviour.
- `Tools/SkinEditor/SkinEditorMappingsPanelActionRow.cpp` — Object combo `- All objects -`, visibility filter, yellow reset badge; Hold gap via tokens.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` (+ `SkinEditorChromeTable.*` if shared draw helpers) — Colour set width, Hold gap, panel/window widths, sort picto sizes/colours.
- `Tools/SkinEditor/SkinEditorMappingsColumnFilter.h` (+ tests) — keep filter predicates; add sort helpers if pure; ProgressBar order untouched.
- `Source/GUI/Skins/Dev/` (`DevSkinTypes` / `NamedSkin`, `DevSkinJson` `skinToVar`/`skinFromVar`) — optional `colourSetLabels` map; never add to `PushPayload` / `applyPushToSkin`.
- Do **not** change: Link; runtime ColourSet enum; mapping key JSON; Build A ProgressBar/caret; filter AND semantics beyond gesture split.

## Tasks & Acceptance

**Execution:**
- [x] `SkinEditorChromeTokens.h` (+ table chrome helpers) — widen Colour set, Hold gap, panel/window if needed; sort/filter picto metrics — layout SSOT.
- [x] `SkinEditorMappingsPanelFilters.cpp` + panel header handlers — left sort / right filter; legend "Colour set"; pictos + clear gap — chrome contract.
- [x] `SkinEditorMappingsPanelRows.cpp` + panel state — filter-then-sort; Object macro-filter visibility; drop single-object focus ring / scroll jump — Object behaviour.
- [x] `SkinEditorMappingsPanelActionRow.cpp` — `- All objects -`, Object clear badge, Hold spacing — action row.
- [x] `SkinEditorMappingsPanel.cpp` / `.h` — `[kN]` badge+label paint; double-click shared label edit with char limit — Colour set UX.
- [x] DevSkin `NamedSkin` + `skinToVar`/`skinFromVar` (+ unit test) — persist `colourSetLabels`; push path unchanged — editor metadata.
- [x] `SkinEditorMappingsColumnFilter*` tests / new sort-label tests as needed + `lint_touched.py` — quality gate.

**Acceptance Criteria:**
- Given Mappings open, when left-clicking a column header twice, then that column sorts A→Z then Z→A with yellow sort picto; other columns show grey dual-arrow; only one sort active.
- Given any header, when right-clicking, then the existing filter menu opens; when a filter is active, the amber clear badge appears after the sort picto.
- Given Object = Slider, when viewing Mappings, then only the Slider table is shown, no focus outline, and Object clear resets to `- All objects -`.
- Given a skin, when double-clicking Colour set on a `k3` row and setting a label, then every `k3` cell shows `[k3] <label>`; Export/Import keeps it; Standalone push ignores it.
- Given the action row, when measuring layout, then Hold is farther from Object than today and no Link column exists.

## Implementation Notes

- Review loop 1 patches: removed dead `scrollFocusedObjectIntoPlace`; fixed stale width comments; ASCII allow-list at edit time (shared with Layer/JSON); clamp `focusedObject_` after rebuild; show label-edit errors; help-label ellipsis (later removed in Build B+polish review); `objectFilterNameFromComboSelection` helper + tests; DevSkin label tests for ASCII strip, import cap, compiled reject.
- Review Build B + polish (2026-11-03): removed Colour set help ellipsis; replaced dead `kMappingsObjectToHoldGap` with `mappingsHoldLeft`; shared `sanitizeColourSetLabel` (strip then cap); import ASCII + order tests; `isColourSetColumn` hit helper + test. Kept State width 75. Deferred: panel mouse harness; focusedObject_ identity rebind.
- Deferred: panel-level mouse harness for left-sort / right-filter wiring (helper-only unit tests remain).

## Spec Change Log

## Review Triage Log

- `false` — placeLegendColumn leaves stale clear hits — `layoutLegendHitTargets` calls `clearLegendHitTargets()` first, so inactive clears stay empty.
- `false` — layoutGlobalClearBounds leaves stale global clear — same clear-then-set pattern; early return keeps empty bounds from clearLegendHitTargets.
- `false` — Colour set sort should use help labels — column keys are technical `kN`; help labels are shared annotations, not row-unique sort keys.
- `false` — clearAllColumnFilters must also reset sort/Object — name and prior behaviour are column filters only; not required by frozen intent.
- `false` — reject finding that asks to edit this build's spec status/Design Notes wording.
- `low` (rejected) — duplicated ASCII filter before shared allow-list helper — fixed as part of patch bundle via shared charset.
- `medium` — unused `scrollFocusedObjectIntoPlace` — confirmed no callers; deleted (patch).
- `low` — stale `// 446`/`// 476` and Set wording in tokens — confirmed; comments updated (patch).
- `medium` — label edit caps length but strips non-ASCII only on save — confirmed; ASCII allow-list at `setInputRestrictions` (patch).
- `medium` — `focusedObject_` can index wrong/out-of-range table after rebuild — confirmed; clamp to -1 when out of range (patch).
- `low` — silent `setColourSetLabel` failure after dialog OK — confirmed; show Result message (patch).
- `low` — help label `drawText` without ellipsis — confirmed; ellipsis enabled (patch).
- `medium` — Object combo id 1→empty filter untested — confirmed; helper + SortErgo test (patch).
- `medium` — ASCII strip / import >16 / compiled reject untested — confirmed; DevSkinColourSetLabelsTests extended (patch).
- `medium` (deferred) — headerGesture unit test does not wire panel mouseUp — true gap; no Mappings panel harness in Tests/; manual smoke remains.

### Review Findings

- [x] [Review][Decision] Ellipsis on Colour set help labels → **resolved: remove ellipsis** (option 2) — becomes patch below.
- [x] [Review][Decision] State column 75 px (−40) → **resolved: keep 75** (option 1) — no code change.
- [x] [Review][Decision] Commit polish before merge/PR → **resolved: after patches** (option 3) — one polish+fix commit once patches land; not a code patch.
- [x] [Review][Patch] Remove Colour set help-label ellipsis (`drawText` useEllipses=false) [`Tools/SkinEditor/SkinEditorMappingsPanelRows.cpp`] — applied 2026-11-03.
- [x] [Review][Patch] Retire unused `kMappingsObjectToHoldGap` / pin real Hold layout via `mappingsHoldLeft` [`Tools/SkinEditor/SkinEditorChromeTokens.h`] [`SkinEditorMappingsPanelActionRow.cpp`] [`SkinEditorChromeLayoutTests.cpp`] — applied 2026-11-03.
- [x] [Review][Patch] Strip non-ASCII before 16-char cap via shared `sanitizeColourSetLabel` [`DevSkinTypes.h`] [`DevSkinJsonMapping.cpp`] [`DevSkinLayer.cpp`] — applied 2026-11-03.
- [x] [Review][Patch] Unit-test import path ASCII strip + strip-before-cap order [`Tests/Unit/DevSkinColourSetLabelsTests.cpp`] — applied 2026-11-03.
- [x] [Review][Patch] Extract + unit-test `isColourSetColumn` [`SkinEditorMappingModePolicy.h`] [`SkinEditorMappingModePolicyTests.cpp`] — applied 2026-11-03.
- [x] [Review][Defer] Panel mouse harness for left-sort / right-filter [`Tools/SkinEditor/SkinEditorMappingsPanelFilters.cpp`] — deferred: already recorded 2026-11-03; helper-only `headerGestureForClick` tests; manual header smoke remains.
- [x] [Review][Defer] `focusedObject_` index can point at a different object after column filters reshape `objects_` [`Tools/SkinEditor/SkinEditorMappingsPanelRows.cpp:124`] — deferred: pre-existing identity drift; Build B only clamps out-of-range to -1; rebind-by-name would be a separate fix.

#### Rejected

- `false` — Global clear must also reset Object macro-filter — `clearAllColumnFilters` / gutter clear are column-only by prior design; Object has its own yellow clear.
- `false` — Colour set sort should follow help labels — technical `kN` keys are the sort identity; help labels are shared annotations (prior triage).
- `false` — Import of all-non-ASCII label must error — silent sanitize to empty matches Layer set-path; omission of empty keys is intended.
- `false` — Spec/Design Notes Hold-gap wording must be edited in this review — rejected: fix-the-spec findings are out of band; layout contract is handled via the Hold patch above.
- `low` (rejected) — Duplicated trim/cap/ASCII loops in Layer vs `colourSetLabelsFromVar` — shared allow-list already; factoring adds surface for little everyday harm.
- `low` (rejected) — No unit assert that all-empty labels omit the whole `colourSetLabels` object — `colourSetLabelsToVar` already returns void when `!any`; per-key omit + push omit covered.
- `low` (rejected) — `ColumnLayout` duplicated across Mappings cpp files — pre-existing WET; not worth a shared refactor in this review pass.
- `low` (rejected) — Object combo can select a type with zero visible rows / no empty-state — edge UX only; not an AC.
- `low` (rejected) — Colour set label edit discoverability (double-click only) — matches AC; affordance polish is optional later.

## Design Notes


**Label JSON (agent):** on each skin object in editor JSON, optional `"colourSetLabels": { "k1": "Patch", "k4": "Modales" }` — only non-empty ASCII labels. Round-trip in session via skins array; Export/Import same. Runtime `importFromJson` / push path never reads this field.

**Sort apply order:** for each object table: collect keys → apply column filters → sort by active column (string compare on display labels / colour name) → paint. Object combo filters which tables exist, not which rows inside a table.

**Gesture split:** use `mods.isPopupMenuClick()` (or equivalent) for filter; plain left for sort. Do not break clear-badge hits or Unbound/Hold flows.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched Skin Editor / DevSkin / Tests paths
- Relevant unit tests (column filter + new label/sort helpers) via project test preset -- expected: pass
- `cmake --build --preset macos-debug-arm64` (Skin Editor + plugin as needed) -- expected: success

**Manual checks:**
- Sort pictos + right-click filters on Property / State / Colour set / Mapped colour
- Object All vs single + clear badge; no focus ring in single mode
- Double-click label shared across tables; empty → badge only; reload session / re-import skin keeps labels
- Push to Standalone: product UI unchanged; no import error from skins with labels
- Hold spacing visually aired; window still fits fixed body columns
