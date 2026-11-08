---
title: 'Skin Editor Build C — Mappings Link'
type: 'feature'
created: '2026-11-03'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '895b8e82261870d80c5e2c656e9306d5194470d6'
context:
  - '{project-root}/_local/Documents/Recap-Skin-Editor-Mappings-Ergonomie-et-Link.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-mappings-ergonomics.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Mapping authors still repaint the same pastille across related rows by hand; Build B left Link out of scope, so there is no calm, persistent way to treat those rows as one edit group.

**Approach:** Add a Link column right of Colour set with calm-by-default bidirectional link groups (reveal on selection, yellow chain chrome), Cmd/Ctrl multi-select, editor-only skin JSON persistence (ignored at runtime / push), and mapping propagation to the whole group on every bind regardless of Hold.

## Decisions (planning)

- Groups are bidirectional; multiple distinct groups per skin; a row belongs to at most one group (joining A leaves B).
- Visual calm: no row link pictos when selection is empty; selecting a member reveals group highlight + yellow chains; hover shows grey chain affordance on candidate rows when a linkable context exists.
- Header Link: square + chain (grey/yellow per recap §6.5); Unbound-style count badge = size of the current selection's group (or selected free-row count when creating / merging).
- Unlink: yellow chain click removes that row; last member dissolves the group; yellow header click unlinks the whole highlighted group.
- Multi-select Cmd (macOS) / Ctrl (Windows/Linux) then header or one selected row's chain creates / merges; mixed groups merge into one; winning pastille = already-mapped over Unbound, else last-clicked / activated-chain row wins and overwrites; no Text/Set business guards at V1.
- Hold stays independent: group propagation runs on every successful mapping write to a linked row; Hold only keeps the gesture armed for the next click.
- Persist as editor-only NamedSkin metadata via `skinToVar`/`skinFromVar` (same family as `colourSetLabels`); omit when empty; never Put in `PushPayload` / `applyPushToSkin`; no `schemaVersion` bump; legacy skins without links load clean.
- Widen Mappings panel / window tokens as needed; reuse Build B yellow/grey chrome squares.
- Spec size: keep the full Build C Link scope in one spec (token overrun accepted).
- Object ≠ `- All objects -`: Link create / extend / unlink allowed only among rows of the visible object table; no cross-object add or merge while other tables are hidden. Existing cross-object groups remain in the document; revealing them for edit requires returning to All objects (or selecting a visible member — peers in hidden tables stay linked but are not shown).

## Boundaries & Constraints

**Always:**
- Member identity = full `SkinColourKey` (object + property + state + colourSet).
- Export product value = resulting mappings; link groups are an editing accelerator that round-trip with the skin document / session.
- When Object macro-filter is not `- All objects -`, Link actions are limited to the visible table only (no cross-object create/extend).
- Skin Editor Clean Code gate (`lint_touched.py` includes `Tools/SkinEditor/`).
- ASCII-only UI chrome strings.

**Never:**
- Permanent forest of chain icons on every row.
- Business guards requiring same Property / same Colour set.
- Coupling Hold to whether the group remaps.
- Rework Build A ProgressBar/caret or Build B sort/filter/Object/labels except minimal width/neighbour integration for the new column.
- Skin Studio web; renaming `kN` to product zones in the model.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Calm rest | No selection | No row chain pictos; header grey inactive | N/A |
| Create pair | Select free A; click grey chain on B | A+B one group; yellow chains; group highlight; badge=2 | N/A |
| Dissolve | Unlink until one member left | Group removed; survivor free | N/A |
| Unlink all | Yellow header on highlighted group | Header grey; all members free | N/A |
| Multi create | Cmd/Ctrl multi free rows; header or chain | One new group; pastille align rules apply | N/A |
| Merge | Multi across groups / mixed | Single merged group; winner pastille overwrites | N/A |
| Propagate | Map pastille on one linked row; Hold off | Whole group gets that pastille; Hold idle after | N/A |
| Hold independent | Hold on; map linked row | Group remapped; gesture stays HeldColour | N/A |
| Persist / push | Export/import / session; push Standalone | Groups round-trip in editor JSON; push omits links; runtime ignores | Unknown keys ignored; empty links omit |
| Orphan keys | Skin JSON lists a key no longer in inventory | Drop orphan on load; keep valid members; dissolve if <2 left | N/A |
| Object≠All | Macro-filter single object | Link only among visible-table rows; no cross-object create/extend | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMappingsPanel.h` / `.cpp` — add Link column to `columnsForWidth`; extend selection to multi-select (Cmd/Ctrl toggle); row paint for calm/hover/yellow chains + group highlight; header hit for Link states; wire chain clicks.
- `Tools/SkinEditor/SkinEditorMappingsPanelFilters.cpp` — sticky legend Link header (square+chain, count badge); no sort/filter on Link.
- `Tools/SkinEditor/SkinEditorMappingsPanelRows.cpp` — rebuild must preserve selection keys; apply Object≠All Link gating; call into link model when binding.
- `Tools/SkinEditor/SkinEditorMappingsPanelActionRow.cpp` — Hold left may need re-anchor after Link width; leave Hold semantics alone.
- `Tools/SkinEditor/SkinEditorChromeTokens.h` (+ `SkinEditorChromeTable.*`) — Link column width; panel/window growth; chain/header metrics; reuse attention yellow / grey squares.
- `Tools/SkinEditor/SkinEditorMappingModePolicy.h` (+ MainComponent mapping) — after successful `applyMapping` / bind, remap all keys in the writer's link group; do not change Hold retain rules.
- `Source/GUI/Skins/Dev/DevSkinTypes.h` — NamedSkin editor field for link groups (e.g. vector of key-sets).
- `Source/GUI/Skins/Dev/DevSkinJson.cpp` / `DevSkinJsonMapping.cpp` / `DevSkinLayer.cpp` — serialize/sanitize like `colourSetLabels`; Layer mutators + `bump()`; never push.
- Pure helpers (new small header under Tools/SkinEditor or Shared testable unit) — group create/add/remove/merge/dissolve, winner pastille, header state machine from selection.
- Tests: extend `DevSkinColourSetLabelsTests` pattern for link JSON/push-omit; new pure-logic tests for group rules + header state; chrome layout width tests.
- Do **not** change: Build B sort/filter gestures; Colour set label model; ProgressBar inventory; runtime ColourSet enum.

## Tasks & Acceptance

**Execution:**
- [x] Pure link-group helpers (+ unit tests) — create/merge/remove/dissolve, header state, winner pastille — rules SSOT.
- [x] `DevSkinTypes` + JSON + Layer (+ persistence tests) — editor-only link groups round-trip; push omits; orphans dropped.
- [x] `SkinEditorChromeTokens` / table draw helpers (+ layout tests) — Link width, panel/window, chain/header chrome.
- [x] `SkinEditorMappingsPanel*` selection + paint + hits — multi-select, calm reveal, hover grey, yellow chains, header actions, Object≠All gate.
- [x] Mapping write path (`MappingModePolicy` / MainComponent) — propagate pastille to group peers; Hold retain unchanged.
- [x] `lint_touched.py` + relevant unit tests — quality gate.

**Acceptance Criteria:**
- Given no selection, when viewing Mappings, then no row chain pictos appear and Link header is grey inactive.
- Given free row A selected, when clicking grey chain on free row B, then A and B form one group with yellow chains and badge 2.
- Given a highlighted group, when clicking yellow Link header, then all members unlink.
- Given a linked row, when mapping a pastille with Hold unchecked, then every group member gets that pastille and Hold does not stay armed.
- Given Hold checked, when mapping a linked row, then the group remaps and the HeldColour gesture remains for the next click.
- Given Cmd/Ctrl multi-select across two groups, when activating Link header, then groups merge and the winner pastille overwrites.
- Given Export/Import or session reload, when reopening the skin, then link groups and Colour set labels are still present; Standalone push payload has neither.
- Given Object ≠ All objects, when attempting Link actions, then behaviour matches the frozen Object≠All policy.

### Review Findings

_(Passage 1 — Link model / persistence / propagation — code review 2026-11-04)_

- [x] [Review][Decision] Single free-row Link badge: hide vs muted "1" — resolved 2026-11-04: keep muted "1" (option 2); follow-up hide note superseded.
- [x] [Review][Decision] Property-only bind target (State cell no longer arms/binds) — resolved 2026-11-04: keep Property-only (option 2); accepted polish.
- [x] [Review][Patch] Joining a row that already belongs to another group merges both groups wholesale instead of moving that row (joining A leaves B) — patched 2026-11-04 (`addToContext` removes candidate first)
- [x] [Review][Patch] Multi-select then Link chain click only links the last selected key + candidate — patched 2026-11-04 (`handleLinkChainClick` uses `linkOrMerge` when selection ≥ 2)
- [x] [Review][Patch] alignGroupPastilles elects winner only from orderedKeys — patched 2026-11-04 (`winnerPastilleForGroup`)
- [x] [Review][Patch] No unit test that duplicateSkin(..., copyMappings=false) clears mappingLinkGroups — patched 2026-11-04
- [x] [Review][Patch] No unit test that sanitizeMappingLinkGroups enforces exclusive membership — patched 2026-11-04
- [x] [Review][Patch] HeldColour still enables Property bind hover when bind is blocked — patched 2026-11-04
- [x] [Review][Defer] No automated MainComponent applyMapping→propagate(+Hold) co-coverage — deferred: already in deferred-work.md (2026-11-03); Layer + MappingModePolicy + manual Recap §9 remain the gate
- [x] [Review][Defer] Non-transactional peer/align writes (partial remap / writer kept / mid-align abort) — deferred: already in deferred-work.md (2026-11-03); full rollback needs Layer API beyond this review slice
- [x] [Review][Defer] Panel create/merge does not assert alignGroupPastilles is invoked — deferred: harness does not drive panel orchestration; pure winnerPastille + Layer propagate remain the automated pins

**Rejected (passage 1):**
- `false` — CMake / panel chrome hunks missing from review package — artifact of chunked passage 1; sources exist on branch; passages 2–3 cover UI chrome.
- `false` — Shift-range contradicts Design Notes “no Shift-range in V1” — polish accepted in this review’s frozen intent; fixing would only edit Design Notes.
- `false` — Object≠All badge shows full group size while UnlinkAll peels visible keys only — matches frozen Object≠All (hidden peers stay linked; badge reflects true group size).
- `false` — Design Notes “dirty bump once” vs per-peer bump — prior triage rejected; behaviour correct for users.
- `low` (rejected) — mappingKeyFromVar comment / omitted colourSet defaulting to k1 — round-trip always writes colourSet; hand-edited JSON edge not everyday.
- `false` — Spec front-matter review_loop_iteration / status metadata — finding asks to edit the spec under review.

_(Passage 2 — Mappings UI: selection / Link chrome / Property hover / Unbound·Hold neighbourhood — code review 2026-11-05)_

- [x] [Review][Decision] Grey Link hover: whole candidate row vs Link column only — resolved 2026-11-05: keep Link-column-only hover (option 2); recap row wording superseded for this polish
- [x] [Review][Decision] Sticky Link legend: restore “Link” title (widen) vs keep chain+badge-only at 55px — resolved 2026-11-05: keep chain+badge-only (option 2); title omitted as accepted polish
- [x] [Review][Patch] Unbound label/badge click clears row selection — patched 2026-11-05 (`mouseUp` preserves `hitActionRowChrome`)
- [x] [Review][Patch] Property bind hover arms for whole mapping row — patched 2026-11-05 (Property column gate)
- [x] [Review][Patch] Incomplete selection/hover reset — patched 2026-11-05 (`clearSelectionAndHovers` + skin-null / early clear)
- [x] [Review][Patch] Cmd/Ctrl toggle never sets `selectionAnchorKey_` when starting from empty anchor — patched 2026-11-05 (reseed from selection back)
- [x] [Review][Patch] Enabling Property bind hover does not sample current mouse position — patched 2026-11-05
- [x] [Review][Patch] readOnly Link-column click neither links nor selects — patched 2026-11-05 (select under readOnly)
- [x] [Review][Patch] `drawLinkCountBadge` header comment still says em dash for idle; paint uses geometric bar — patched 2026-11-05
- [x] [Review][Patch] No unit asserts for `propertyBindHoverNeedsDarkText` light vs dark fills — patched 2026-11-05
- [x] [Review][Patch] Link grey-hover / `revealedKeys` helpers only pin empty-selection calm path — patched 2026-11-05
- [x] [Review][Patch] Unbound/Hold neighbourhood geometry unpinned — patched 2026-11-05
- [x] [Review][Defer] No automated Cmd/Shift selection / preserve-or-clear panel harness — deferred: same Skin Editor no-panel-harness gate as prior Mappings reviews; manual smoke remains
- [x] [Review][Defer] Hold checkbox attention-yellow chrome in `drawToggleButton` — deferred: passage 3 scope (Hold jaune / LookAndFeel), not passage 2 UI Link neighbourhood

**Rejected (passage 2):**
- `false` — Free multi-select has no row highlight — `revealedKeys` inserts free selected keys; `isKeyGroupRevealed` drives highlight.
- `false` — Revealed peers share the same highlight as the clicked member — intentional group reveal (recap §6.2).
- `false` — `globalClearBounds_` missing from `preservesRowSelectionForEvent` — sticky legend bounds already preserve; panel `handleLegendClearClick` consumes the chip.
- `false` — Object combo A→Z vs table order — combo already uses `mappingsObjectDisplayOrder()`.
- `false` / accepted polish — Unbound count chip fixed to `linkCountBadgeWidth()` may clip huge totals — same-width Unbound↔Link polish accepted for this lot.
- `false` — `propertyBindHoverNeedsDarkText` “never enforces a minimum ratio” — chooses the better of two inks by contrast; mid-fill residual is taste.
- `low` (rejected) — Link cut-slab 50 vs 51 px pad asymmetry — one-pixel legend air; not everyday harm worth layout churn.
- `low` (rejected) — Mapped sticky title elide vs `contentWidth` reserve mismatch — cosmetic legend text only.
- `false` — Shift+click on Link chain ranges instead of link/unlink — Link column hit-first is intentional; existence of Shift-range already accepted.

_(Passage 3 — polish: csN tokens / codesign POST_BUILD / Hold jaune / `drawToggleButton` quality gate — code review 2026-11-05)_

- [x] [Review][Decision] Legacy `kN` ColourSet wire tokens after `csN` rename — resolved 2026-11-05: hard cut (option 1); only `csN` valid; pre-rename JSON must be resaved / migrated manually; no `kN` parse aliases
- [x] [Review][Patch] Skin Editor ad-hoc re-sign incomplete on Apple artefact paths [`CMakeLists.txt`] — patched 2026-11-05: always re-sign build bundle; codesign+xattr after CopyToArtefactsDir and solo artefacts copy
- [x] [Review][Defer] Hold checked attention-yellow chrome has no automated paint pin [`SkinEditorLookAndFeel.cpp`] — deferred: project keeps LookAndFeel/`paint()` out of unit tests; manual Skin Editor smoke remains the gate (implementation itself fulfills passage-2 Hold-yellow defer)
- [x] [Review][Defer] Skin Editor POST_BUILD codesign/xattr not exercised by CI [`CMakeLists.txt` / workflows] — deferred: CI sets `USER_COPY_TO_ARTEFACTS_DIR=OFF`; local or optional smoke with copy enabled would settle it

**Rejected (passage 3):**
- `false` — `csN` rename “violates” frozen Link Code Map “do not change Colour set label model” — intentional out-of-band passage-3 polish (deferred-work / review scope); runtime enum stays `ColourSet::k1`…`k4`.
- `false` — checked Hold ignores hover/pressed fills — intentional solid attention chrome while on; only Skin Editor toggle is Hold.
- `false` / intentional — dual vocabulary enum `kN` vs wire/UI `csN` without table note — Build B already kept technical identity `k1`…`k4` in code; wire rename is display/JSON only.
- Rejected (edit other spec) — `spec-skin-editor-mappings-ergonomics.md` still documents `k1`…`k4` JSON tokens — fix would edit a different done spec, not this diff.
- `low` (rejected) — tests assert `csN` only with no explicit “reject `kN`” pin — subsumed by the legacy-alias decision; no separate everyday fix.
- Hold yellow + `drawToggleButton` helper extract — no defect; fulfills passage-2 defer and stays under function-length gate.

## Implementation Notes

- Build C (2026-11-03): Link column (70px) between Colour set and Mapped; panel/window tokens 561/591. Pure helpers in `SkinEditorMappingLinkGroups.h`; shared column geometry in `SkinEditorMappingsColumns.h`. Editor-only `mappingLinkGroups` JSON + Layer `setMappingLinkGroups` / `propagateMappingToLinkPeers`; push omit. Panel: Cmd/Ctrl multi-select, calm/hover/yellow chains, header create/merge/unlink, Object≠All gate. Hold unchanged. Verified: unit tests + `lint_touched.py --base` baseline + Skin Editor build.
- Matrix audit follow-up: added calm/unlink-all helper coverage and Layer `propagateMappingToLinkPeers` unit test (Hold retain stays covered by MappingModePolicy tests; applyMapping calls propagate then retain independently).
- Review patches: early-return on peer propagate failure; align group via first in-group ordered key; headerState under Object filter; prune selection + clear hover on rebuild/refresh; clear link groups when duplicateSkin drops mappings; delete unused sanitize; strengthen DevSkin/layout unit tests.
- Follow-up: Link header count badge — single free row keeps muted "1" (review 2026-11-04); group size or multi-select create/merge count (>= 2) unchanged. Property-only bind/arm (not State) accepted as polish.
- Passage 2 review (2026-11-05): Link hover stays column-scoped; sticky legend stays chain+badge without “Link” title. Patches: Unbound preserve selection, Property-column bind hover + enable-time sample, selection/hover reset helper, Cmd/Shift anchor reseed, readOnly Link select, idle-bar comment, chrome/hover/reveal unit pins. Deferred: panel selection harness; Hold yellow → passage 3.
- Passage 3 review (2026-11-05): `csN` wire/display rename; Hold checked attention-yellow + `drawToggleButton` helpers under quality gate. Legacy `kN`: hard cut accepted (no aliases). Codesign: always re-sign build bundle + codesign/xattr after every Apple artefacts copy (solo + CopyToArtefactsDir). Deferred: Hold paint pin; CI artefacts-copy codesign smoke.
- Manual smoke still recommended: Recap §9 Link+Hold checklist and Object≠All cross-object policy; Hold checked yellow; Dropbox Skin Editor after full ALL build (Finder seal).

## Spec Change Log

## Review Triage Log

- `false` — calm rest unreachable without Escape — Cmd/Ctrl toggle can deselect the last row; empty selection is reachable; Escape clearing was not required by frozen intent.
- `low` (rejected) — ActiveGrey paints same as inactive grey — recap only requires grey vs yellow; badge count + clickability mark create/merge readiness.
- `low` — unused `SkinEditorMappingLink::sanitize` duplicates DevSkin sanitize — dead code; delete helper (patch).
- `medium` — `applyMapping` retains Hold/armed after `propagateMappingToLinkPeers` failure — confirmed at MainComponentMapping.cpp:247-255; return early after alert (patch).
- `medium` — round-trip test asserts sizes not member keys — verification-gap; assert restored members equal a/b (patch).
- `false` — Code Map stale paths / AC matrix gaps — finding asks to edit this build's spec; rejected.
- `medium` — `headerState` ignores Object macro-filter — confirmed; `linkHeaderState` must use `keysForObjectFilter` (patch).
- `medium` — `alignGroupPastilles` uses `orderedKeys.back()` which may be outside the committed group under Object filter — confirmed; resolve group via first ordered key present in groups (patch).
- `medium` — `rebuildRows` never prunes `selectedKeys_` to visible rows — confirmed; prune on rebuild + clear hover on refresh (patch).
- `low` (rejected) — Design Notes "one dirty bump" vs per-peer bump — agent ideal; behaviour correct; batching adds complexity without user-facing harm.
- `medium` — `duplicateSkin(..., copyMappings=false)` keeps `mappingLinkGroups` after clearing mappings — confirmed; clear link groups with mappings (patch).
- `false` — hover chain from any column of row — intended; chain paints only in Link cell; recap is row hover.
- `medium` (deferred) — no automated `applyMapping` / Hold+Link co-coverage at MainComponent — verification-gap disposition defer; Layer + MappingModePolicy + manual Recap §9.
- `medium` — partial peer remap leaves mixed pastilles without rollback — real; full transactional rollback deferred; early-return on retain is the patch slice.
- `medium` — stale `hoveredLinkKey_` after scroll/refresh without mouseMove — clear hover in `refresh` (patch).
- `medium` (deferred) — mid-loop `writePastilleToKey` failure after group commit — already aborts loop; full rollback of group+align deferred.
- `medium` — propagate unit test never overwrites an already-mapped peer — verification-gap; extend assert (patch).
- `low` — beginTest name claims Hold but body never observes Hold — rename test (patch).
- `medium` — `SkinEditorMappingsColumns::forWidth` unpinned by tests — add layout/hit assert (patch).
- Edge claim "whole-group remap fails while Hold continues" — same root as peer-failure retain (patch).
- Edge claim "winner pastille skipped" — same root as alignGroupPastilles anchor (patch).

## Design Notes

**JSON (agent):** optional `"mappingLinkGroups": [ [ {object,property,state,colourSet}, … ], … ]` on the skin object — only groups with ≥2 valid keys; omit empty. Keys match existing mapping entry identity. Sanitize on read: drop orphans / invalid; dissolve undersized groups.

**Selection (agent):** replace single `selectedRow_` with an ordered set of `SkinColourKey` (last-clicked = winner anchor). Plain click replaces selection; Cmd/Ctrl toggles; no Shift-range in V1. Group reveal = all keys sharing the selected row's group (union when multi spans one group).

**Link column:** not sortable / not filterable; legend shows title + header chain control + count badge only.

**Propagation:** on successful bind/setMapping for key K, if K ∈ group G, apply the same pastille name to every other key in G (same write path / dirty bump once).

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched Skin Editor / DevSkin / Tests paths
- Unit tests for link helpers + DevSkin link JSON/push-omit + chrome layout -- expected: pass
- `cmake --build --preset macos-debug-arm64` (Skin Editor + tests as needed) -- expected: success

**Manual checks:**
- Recap §9 Link + Hold checklist items (calm tables, second group, dissolve, All objects cross-object link, Hold independent)
- Object≠All policy smoke
- Panel/window still usable after width growth
