---
title: 'Skin Editor Mappings column intents and Operation copy'
type: 'feature'
created: '2026-10-27'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '12855f64598452361e0a1f49989b80ec1c08028e'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-hold-mapping-chrome.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** In Skin Editor Mappings, a whole-row click only arms or binds a target. The Mapped colour swatch looks editable but does not load that colour into COLOR / SV, and the OPERATION witness names what was clicked without saying what to do next for the bind gesture.

**Approach:** Split Mappings row clicks by column — Property/State keeps the bind/arm gesture; Mapped colour selects the referenced palette pastille and loads COLOR/SV for direct edit (palette remains SSOT). Rewrite OPERATION witness copy so each active mode states the held or armed item and the expected next click.

## Decisions (locked)

- Keep the full spec (column split + OPERATION copy in one delivery).
- Mapped colour click = **edit only**: select pastille + load COLOR/SV; clear mapping mode to Idle (no HeldColour from this click); no OPERATION bind coaching for that gesture.
- OPERATION copy (ASCII): HeldColour → prefix `Hold: {pastilleName}`, next-step `Click a Property or State cell to map`; ArmedRow → prefix `Target: {object} > {property} > {state}`, next-step `Click a palette colour to map`. Pure Mapped-colour edit does not show an Edit OPERATION witness.
- Unbound Mapped colour click = **no-op** (no mode change, no SV load).

## Boundaries & Constraints

**Always:**
- Keep Idle / HeldColour / ArmedRow policy (Hold batch direction, Esc → Idle, no dual sticky rewrite) from hold-mapping chrome.
- Palette ARGB remains SSOT; mappings stay name references only.
- Mapped colour click that resolves a pastille: select that pastille in Palette, sync COLOR/SV, clear mapping mode to Idle; edit writes via existing pastille ARGB path.
- Property or State click: existing row bind/arm path (`SkinEditorMappingModePolicy` row intents).
- OPERATION witness must name the active item and the expected next step for HeldColour and ArmedRow using the locked copy above (ASCII hyphen punctuation).
- Stay in `Tools/SkinEditor/` (+ existing Skin Editor unit tests if policy/copy helpers need coverage); pass `lint_touched.py` on touched Skin Editor C++.

**Never:**
- Change DevSkin JSON / mapping schema; plugin Look; replace Hold/Esc collision rules; instructional spam on the bottom status bar; French UI strings; enter HeldColour from a Mapped colour click; invent colour edit for Unbound Mapped cells.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Edit mapped colour | Any mode; click Mapped colour cell with a bound pastille | Mapping mode → Idle; pastille selected in Palette; COLOR/SV shows that colour; editable when Editor skin allows; OPERATION witness cleared | Unresolved name: no SV load; no crash |
| Arm via Property/State | Click Property or State cell | Same as today's whole-row activation (bind if HeldColour, else arm / re-arm); OPERATION uses locked ArmedRow or post-bind rules | Read-only: ignore |
| HeldColour witness | Pastille held (via Palette click), awaiting rows | `Hold: {name}` + `Click a Property or State cell to map` (+ swatch as today) | N/A |
| ArmedRow witness | Row armed, awaiting pastille | `Target: {object} > {property} > {state}` + `Click a palette colour to map` | N/A |
| Held + Mapped colour | HeldColour; click Mapped colour on a bound row | Clear to Idle; select that row's pastille + SV; do not bind/rewrite via leftover arm | N/A |
| Unbound Mapped cell | Click Mapped colour when cell is Unbound | No-op: no mode change, no SV load | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMappingsPanel.cpp` — `Content::mouseUp` / `tryBindRow` / `columnsForWidth`: add x hit-test → Mapped vs Property/State column; callback must distinguish edit-mapped vs activate-row.
- `Tools/SkinEditor/SkinEditorMappingsPanel.h` — extend row activation API (enum or dual callbacks).
- `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` — Mapped path → resolve pastille from editor skin mapping, clear mapping mode to Idle, select pastille + `syncColourPickerToSelection` (do not `enterHeldColour`); Property/State → existing `onMappingRowActivated`; `fillHeldColourWitness` / `fillArmedRowWitness` — locked next-step copy.
- `Tools/SkinEditor/SkinEditorMappingModePolicy.h` + `Tests/Unit/SkinEditorMappingModePolicyTests.cpp` — only if new intents are needed; prefer not to break existing row/pastille intents.
- `Tools/SkinEditor/SkinEditorMainComponentActions.cpp` — reuse `onColourChanged` / pastille ARGB write for SV edits.
- `Tools/SkinEditor/SkinEditorPalettePanel.*` — selection highlight when Mapped colour selects a pastille.
- Do **not** change: `DevSkin` palette/mapping schema, plugin FooterPanel, bottom status bar role (connection / last push / version).

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorMappingsPanel.{h,cpp}` — column hit-test + distinct callbacks for Mapped colour vs Property/State — split edit vs bind gestures; Unbound Mapped = no-op at panel or main.
- [x] `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` (+ `.h` if needed) — Mapped → Idle + pastille select + SV sync; rewrite Held/Armed OPERATION witness with locked copy.
- [x] `Tools/SkinEditor/SkinEditorPalettePanel.*` — ensure programmatic pastille selection updates highlight when driven from Mapped colour.
- [x] `Tools/SkinEditor/SkinEditorMappingModePolicy.*` + unit tests — only if new intents required; otherwise leave policy SSOT intact.
- [x] Run `python3 Scripts/quality/lint_touched.py` on touched Skin Editor / Tests files; build Skin Editor; run mapping policy tests if touched.

**Acceptance Criteria:**
- Given a bound Mapped colour cell, when the operator clicks that column, then mapping mode is Idle, the referenced pastille appears in COLOR/SV and Palette selection, and that click alone does not write a mapping.
- Given a Property or State cell click, when Hold/Idle/Armed rules apply, then bind/arm behavior matches today's policy (no regression of Hold collision fixes).
- Given HeldColour, when OPERATION is visible, then it shows `Hold: {name}` and `Click a Property or State cell to map`.
- Given ArmedRow, when OPERATION is visible, then it shows `Target: {object} > {property} > {state}` and `Click a palette colour to map`.
- Given an Unbound Mapped colour cell, when clicked, then nothing changes (no mode, no SV).
- Given Esc, when any mapping mode was active, then Idle clears witness as today.

## Implementation Notes

- Column hit-test in `SkinEditorMappingsPanel::Content::mouseUp` via `SkinEditorMappingPolicy::isMappedColourColumn`; Mapped → `onMappedColourClicked`, Property/State → `onRowActivated`.
- Mapped edit: `boundPastilleNameFor` + `mappedColourClick` → Idle + `palettePanel_.selectPastille` + SV sync; never `enterHeldColour`.
- OPERATION copy SSOT in `SkinEditorMappingModePolicy.h` (`heldColourWitnessPrefix` / `armedRowWitnessPrefix` + next-step constants); layout yields prefix space so next-step stays readable.
- Reused existing `SkinEditorPalettePanel::selectPastille` (no panel API change).
- Matrix covered by extended `SkinEditorMappingModePolicyTests` (column, edit/no-op, Property/State policy, witness copy). Manual GUI checks still pending.
- Compare/read-only Mapped colour click still loads SV with editing disabled (same as read-only Palette select); Property/State remains ignored when read-only.

## Spec Change Log

## Review Triage Log

- false — Mapped colour clears Hold/Target on bound click: locked edit-only decision (mode → Idle); not a regression.
- medium/patch — Dual fake `mappedColourClick` args in panel vs main: consolidated; panel gates `item.unbound` only; main resolves pastille once — patched.
- false — Product vs Editor Hold prefix removed: locked copy is `Hold: {name}`.
- false — `selectPastille` re-enters HeldColour: `suppressSelectionNotify_` blocks `onPastilleSelected`.
- low/reject — Witness layout may still elide on very narrow wells: tooltips keep full text; further layout work is polish beyond a trivial patch.
- low/reject — Misleading test asserts + missing GUI harness coverage claims: misleading asserts removed; remaining harness gaps deferred below.
- low/reject — Keyboard path for Mapped-colour edit: mouse-driven table matches prior bind UX; a11y expansion out of everyday scope for this story.
- low/patch — Policy header JUCE comment stale: comment updated — patched.
- medium/patch — Press Property/State, release Mapped (or reverse) fires wrong gesture: require same column for press and release — patched.
- maybe-false/reject — Palette has no selectable row for resolved name: `boundPastilleNameFor` already requires palette membership; product rows still have a non-lower-half list entry.
- false — Unbound Mapped click silent under Hold: locked Unbound Mapped = no-op; Property/State remains the bind hit target.
- false — Read-only Mapped loads SV and clears mode: allowed (picker editing disabled); Property/State still ignored when read-only.
- defer — No panel/`mouseUp` test that Mapped x routes to edit and Property/State to bind (policy helpers only).
- defer — No component test that `onMappedColourClicked` clears mode and syncs Palette/SV.
- defer — No unit test for `layoutOperationWitness` prefixMax arithmetic.

## Design Notes

- Prior done spec `spec-skin-editor-hold-mapping-chrome.md` intentionally dropped instructional status strings; this story **reintroduces** next-step coaching in the OPERATION witness only (not the bottom status bar).
- Column split: Property + State share activate-row; Mapped colour alone is the edit hit target (swatch column span from `columnsForWidth`).
- Mapped colour edit clears bind chrome on purpose so OPERATION never coaches a bind while the operator is only retouching a colour.

## Verification

**Commands:**
- `python3 Scripts/quality/lint_touched.py` on touched Skin Editor / Tests paths -- expected: clean
- CMake build Skin Editor (macos-debug-arm64 preset or current) -- expected: success
- `ctest` / mapping policy unit tests if policy touched -- expected: 0 failures

**Manual checks:**
- Click Mapped colour → SV matches swatch; OPERATION empty; drag SV → palette pastille updates; other rows with same name follow.
- Click Property/State → arm; OPERATION next-step visible; click pastille → bind.
- Hold on: multi-row / multi-pastille still no stale rewrite; Esc clears.
- Unbound Mapped colour click → no change.
