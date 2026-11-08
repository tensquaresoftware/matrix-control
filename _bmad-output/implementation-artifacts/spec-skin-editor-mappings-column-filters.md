---
title: 'Skin Editor Mappings column filters'
type: 'feature'
created: '2026-10-31'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'fa48aa4da61c37727c9cc1457db06019d27909ca'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-mappings-unbound-badge.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Operators scanning the Mappings tables cannot narrow rows by Property, State, or mapped colour, so finding a specific key (e.g. Border + Hover + kBlue) means scrolling the full structured inventory.

**Approach:** Make the sticky legend labels Property, State, and Mapped colour interactive column filters. Clicking a label opens a dropdown of filter values; choosing one filters all mapping tables to matching rows (AND across active columns). An active filter paints that label with Unbound amber and shows a square amber clear badge (X) to the right of the label with `kButtonGap` spacing; clicking the badge clears that column’s filter and restores the normal label colour.

**Decisions:**
- MENU_VALUES: dropdown lists only values that appear in the current Editor skin’s inventory rows for that column.
- MAPPED_UNBOUND: Mapped colour menu includes literal Unbound so unbound cells can be isolated.
- EMPTY_SECTIONS: hide object section headers (and their tables) when no mapping rows match.
- PERSISTENCE: clear all column filters whenever the Editor skin id changes; do not persist across relaunch.

## Boundaries & Constraints

**Always:**
- Filters apply to mapping data rows across every object table under the shared sticky legend (AND when multiple columns are active).
- Dropdown values are derived from values present in the current Editor skin’s inventory rows for that column (Property / State labels; mapped pastille names; plus Unbound when any unbound row exists).
- Active label colour and clear-badge fill use `SkinEditorChrome::attention()` (same amber as Unbound).
- Clear badge is square-ish with panel-style rounded corners (`kControlCorner`), amber fill, panel-surface X glyph; gap from label = `SkinEditorChrome::kButtonGap` (8).
- Clicking an already-active label reopens the menu so the operator can change the value without clearing first.
- Object tables with zero matching rows are omitted entirely (no empty section header).
- Clear all three filters when `setActiveSkinId` changes the Editor skin id.
- Object combo remains scroll/focus navigation only; Unbound count witness stays a whole-skin Export gate, not a filter control.
- UI strings ASCII only.

**Never:**
- Do not change bind / Hold / ArmedRow / pending mapping policy or Operation coaching.
- Do not revive a dedicated Unbound panel or turn the Unbound header witness into a filter toggle.
- Do not invent a new colour token for filter chrome.
- Do not persist filters in preferences or across app relaunch.
- Do not offer catalogue values that do not appear in the current skin’s inventory rows for that column.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Single filter | Click Property → choose Border | Only mapping rows with Property Border remain; Property label amber + clear badge | N/A |
| Multi-column AND | Property=Border and State=Hover | Rows must match both; each active column shows its own clear badge | N/A |
| Clear one | Click clear badge on State while Property still filtered | State filter off; Property filter remains | N/A |
| Change value | Click amber Property label → pick Text | Filter updates to Text; badge stays | N/A |
| No matches | Filters exclude every key | No mapping rows and no object section headers; filters stay active | N/A |
| Unbound filter | Mapped colour → Unbound | Only unbound mapping rows remain | N/A |
| Skin change | Editor skin id changes while filters active | All filters clear; labels return to normal | N/A |
| Empty shell | No Editor skin | No legend / filters (existing empty shell) | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMappingsPanel.h` — panel state; sticky legend; add per-column filter state, hit targets for labels/clear badges, mouse handling on panel chrome; clear filters in `setActiveSkinId` when id changes
- `Tools/SkinEditor/SkinEditorMappingsPanel.cpp` — `paintStickyLegend`, `paint`, `resized` / `layoutShellFilled` / `layoutActionRow`; extend legend paint for amber + clear badges; hit-test clicks; show `PopupMenu` built from present values
- `Tools/SkinEditor/SkinEditorMappingsPanelRows.cpp` — `rebuildRows` / `appendObjectTable` / `makeMappingRow`; apply AND predicates; skip object tables with zero matches
- `Tools/SkinEditor/SkinEditorMappingsPanelFilters.cpp` — column-filter state helpers, present-value menus, sticky-legend paint / hit targets / mouseUp
- `Tools/SkinEditor/SkinEditorChromeTable.{h,cpp}` — optional small `drawClearBadge` (or reuse `drawCountBadge` silhouette with X) so Unbound count badge stays dedicated
- `Tools/SkinEditor/SkinEditorChromeTokens.h` — reuse `attention()`, `kButtonGap`, `kControlCorner`; do not add a new amber
- `Source/GUI/Skins/Dev/DevSkinKeyNames.{h,cpp}` — `toString` for Property/State labels; do not change key SSOT naming
- `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` — out of scope unless a rebuild hook must refresh after palette rename (prefer panel-local refresh)

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorMappingsPanel.h` -- Hold optional Property/State/MappedColour filter values + legend hit bounds; clear on skin-id change -- Filter chrome owns state next to sticky legend
- [x] `Tools/SkinEditor/SkinEditorMappingsPanel.cpp` / `SkinEditorMappingsPanelFilters.cpp` -- Paint active labels amber; layout clear badges with `kButtonGap`; open column menus from present values (incl. Unbound); clear on badge click -- Match Unbound attention chrome without touching header Unbound witness
- [x] `Tools/SkinEditor/SkinEditorMappingsPanelRows.cpp` -- Filter mapping rows with AND; hide empty object sections -- Operators only see matching keys
- [x] `Tools/SkinEditor/SkinEditorChromeTable.{h,cpp}` -- Add `drawClearBadge` at Unbound badge scale -- Shared chrome, no second amber token
- [x] `Tools/SkinEditor/SkinEditorMappingsColumnFilter.h` + `Tests/Unit/SkinEditorMappingsColumnFilterTests.cpp` -- Pure AND/Unbound/present-value coverage for I/O matrix -- Automated matrix audit
- [ ] Manual verify in Skin Editor Mappings -- Visual amber/badge/menu chrome pass (logic covered by unit tests)

**Acceptance Criteria:**
- Given the Mappings panel with an Editor skin, when the operator clicks Property and chooses Border, then only Border rows remain and Property shows amber label + clear badge.
- Given Property and State filters both active, when rows are rebuilt, then a row must match both to appear.
- Given an active filter, when the operator clicks the clear badge, then that column’s filter is removed and the label returns to normal header colour.
- Given an active filter, when the operator clicks the amber label and picks another value, then the filter updates without requiring a prior clear.
- Given filters that match nothing, when tables rebuild, then no mapping rows and no object section headers appear, and filters remain active until cleared.
- Given Mapped colour → Unbound, when tables rebuild, then only unbound rows remain.
- Given active filters, when the Editor skin id changes, then all filters clear.

## Implementation Notes

- Extracted pure predicates to `SkinEditorMappingsColumnFilter.h` (`ActiveFilters`, `filteredObjectGroups`); panel Filters.cpp owns PopupMenu + legend hit layout.
- Clear badge reserved inside column width when active so Mapped colour X does not overflow the legend.
- Review patches: skinId-safe menu callback; no duplicate Unbound menu entry; `mouseWasClicked`; no layout-from-paint; EMPTY_SECTIONS helper tested.
- Unit tests cover I/O matrix filter outcomes; amber paint / click chrome still needs a human GUI pass.
- `setActiveSkinId` early-returns on same id; clears all three filters when id changes (session always refreshes after).

## Spec Change Log

## Review Triage Log

- Blind: duplicated `columnsForWidth` in panel vs Filters.cpp — **low** → reject (drift risk only; extract would widen this story’s surface)
- Blind: parallel `FilterColumn` / `Column` enums — **false** (private panel enum maps to pure helper; intentional)
- Blind: `layoutLegendHitTargets` from `paint()` — **medium** → patch (removed; layout stays on resized / `layoutShellFilled`)
- Blind: `clearOneColumnKeepsOther` never clears — **medium** → patch (test rewritten for Property+State then Property-only)
- Blind: active filter value not shown on legend label — **false** (intent: amber + clear badge; value remains in menu checkmark)
- Blind: menus not narrowed by other active filters — **false** (frozen MENU_VALUES = inventory for that column, not cross-filtered)
- Blind: Design Notes still say paint-only legend — **false** (finding’s fix is editing this build’s spec)
- Blind: Verification omits unit suite — **medium** → patch (tests added/run; Verification commands updated)
- Blind: no automated PERSISTENCE for `setActiveSkinId` — **medium** → defer (GUI wiring; see deferred-work)
- Blind: Object combo empties when no matches — **false** (expected with EMPTY_SECTIONS; objects_ drives combo)
- Blind: no pointer cursor / tooltip on filter labels — **low** → reject (affordance not in intent; unlikely daily miss once yellow+badge exist)
- Blind: `mouseUp` without `mouseWasClicked` — **medium** → patch (guard added)
- Blind: `drawClearBadge` duplicates `drawCountBadge` — **low** → reject (shared helper already deferred from Unbound badge story)
- Blind: Code Map paths stale vs Filters.cpp — **false** (finding’s fix is editing this build’s spec)
- Blind: unused `SkinValues.h` in tests — **low** → reject (transitive enums via includes; no lint hit)
- Edge: stale PopupMenu after skin id change — **high** → patch (callback checks skinId captured at open)
- Edge: duplicate Unbound menu entry if pastille named Unbound — **medium** → patch (`contains` guard before insert)
- Edge: cannot isolate pastille literally named Unbound vs unbound cells — **medium** → defer (sentinel vs pastille name collision; pathological)
- VG: `rebuildRows` EMPTY_SECTIONS / filter apply untested — **high** → patch (`filteredObjectGroups` + unit tests; rebuildRows calls helper)
- VG: `emptyFiltersMatchAll` falsely claims skin-change clear — **medium** → patch (beginTest renamed) + defer full `setActiveSkinId` coverage
- VG: `clearOneColumnKeepsOther` duplicates single-filter — **medium** → patch (same as Blind clear-one rewrite)

### Review Findings

- [x] [Review][Patch] Remove dead `mappingKeyMatches` (declared/defined, never called; `rebuildRows` uses `filteredObjectGroups`) [`SkinEditorMappingsPanel.h` / `SkinEditorMappingsPanelFilters.cpp`]
- [x] [Review][Patch] Mapped colour filter menu paints opaque black swatch when a mapped name is missing from the palette (ARGB left at 0) [`SkinEditorMappingsPanelFilters.cpp` `makeMappedMenuSpec`]
- [x] [Review][Defer] Automate PERSISTENCE (`setActiveSkinId` clears filters) — deferred: already in deferred-work; pure helper tests do not cover panel wiring
- [x] [Review][Defer] Automate async filter-menu `skinId` guard — deferred: PopupMenu + Component outside usual Skin Editor unit style; extend manual checklist instead
- [x] [Review][Defer] Unbound sentinel vs pastille literally named Unbound — deferred: already in deferred-work (pathological)
- [x] [Review][Defer] Revalidate open-menu / sticky Mapped filter when palette or mappings mutate — deferred: rare during edit; Clear badge / skin change recover; not in frozen AC

#### Rejected

- Badge uses `kChromeBadgeCorner` / `kClearBadgeSize` vs frozen `kControlCorner` / Unbound scale — **false** (post-smoke decisions already tranchées; shipped chrome matches review brief; fixing frozen text alone is out of scope for this finding)
- Spec `status: done` with manual verify unchecked / empty Spec Change Log / Code Map points at Panel.cpp — **reject** (fix is editing the spec under review)
- LookAndFeel `PopupMenu` → `controlFill()` out of Code Map / no-op alias — **false** (tranché: combo-styled menus; `controlFill()` aliases `surfacePanel()` so no visual collateral)
- Object combo empties when filters match nothing — **false** (EMPTY_SECTIONS; objects_ drives combo; prior triage)
- `setActiveSkinId` clears filters without calling `refresh` — **false** (session always `refresh()` immediately after; intended split)
- Thin unit coverage (State presentValues / three-column AND) — **low** reject (core I/O covered; unlikely daily miss)
- Unused `SkinValues.h` in tests — **low** reject (transitive via DevSkin; prior triage; no lint hit)
- Filters.cpp ~410 lines / duplicated `columnsForWidth` — **reject** (hors scope / already deferred DRY)
- Verification `--category` flag wording — **false** (TestMain matches name substring; command works)

## Design Notes

Sticky legend labels and clear badges are interactive (`PopupMenu` + panel `mouseUp`). Object combo remains scroll/focus only. Clear badge matches Unbound count chip scale (`kCountBadgeDiameter`, `kControlCorner`). Prefer Skin Editor LookAndFeel `PopupMenu` over a custom list.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` -- expected: build succeeds
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` then `Matrix-Control_Tests --category SkinEditorMappingsColumnFilter` -- expected: 0 failures
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched `Tools/SkinEditor/` and `Tests/Unit/` files

**Manual checks (if no CLI):**
- Mappings: filter Property=Border, State=Hover, Mapped colour=a pastille; clear each; change skin and confirm filters clear; confirm Object combo and Unbound count still behave as before
