---
title: 'Skin Editor palette Add: unique placeholders and pending swatch'
type: 'feature'
created: '2026-10-27'
status: 'done'
route: 'dispatch'
review_loop_iteration: 1
context: []
baseline_commit: '7ef69a48f302871b72b6668f4c22819c00a10bb3'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Palette Add always seeds the same ARGB, so a second pastille is blocked as a colour duplicate even when the name differs. Authors cannot quickly create several placeholder colours to rename and calibrate later, and after a break those placeholders do not stand out from calibrated swatches.

**Approach:** On Add (no name dialog), create pastilles immediately with a unique auto name (`Temp N`) and a unique opaque ARGB that does not collide with the palette. Persist a pending/uninitialized flag so pending colours paint as an empty circle (1 px stroke in Skin Editor logo grey `textBrand()`, same height as colour squares) until the author first calibrates the colour; then show the normal filled swatch. Authors rename Editor colours later by double-clicking the colour **name** (not the swatch).

**Decisions:**
- ADD_UX = IMMEDIATE — Add creates pastille with auto name + unique ARGB + pending; no name dialog.
- STROKE_COLOUR = LOGO_GREY — pending circle stroke uses Skin Editor title brand grey (`textBrand()` / `0xffb9b9b9`).
- RENAME_UX = DOUBLE_CLICK_NAME — double-click on the pastille name text opens rename (`askText`); Product pastilles stay non-renamable; rename does not clear pending.

## Boundaries & Constraints

**Always:**
- Keep ARGB uniqueness (exact uint32) across the skin palette; never allow two pastilles to share the same ARGB.
- Keep case-insensitive unique names.
- Persist pending state with the pastille (session + export/import JSON), defaulting missing JSON to not-pending.
- Clear pending only when ARGB actually changes via the colour editor path (same-ARGB re-apply must leave pending unchanged and must not dirty/bump).
- Pending swatch: empty circle, stroke 1 px in `textBrand()` grey, same height as existing colour squares (`kColourSwatchSize`).
- Opaque seed ARGB only (alpha `0xFF`); do not create transparent placeholders or reuse `kTransparent`.
- Add creates immediately (no name dialog).
- Double-click on Editor pastille **name** renames via dialog; rewrite mapping string refs to the new name; pending unchanged.
- After Add, select the new pastille for the colour picker without treating the selection as a user pastille-click (do not enter HeldColour / do not bind ArmedRow).

**Never:**
- Do not allow duplicate ARGB just because names differ.
- Do not force `#00000000` / transparent as the placeholder colour.
- Do not change Product pastille delete/readonly rules or compiled Black/Cream editability.
- Do not allow renaming Product pastilles.
- Do not redesign the colour picker chrome beyond pending paint + Add seeding behaviour.
- Do not show a name dialog on Add.
- Do not treat double-click on the swatch as rename (name text only).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Rapid Add | Editor skin editable; author triggers Add repeatedly | Each Add yields a new pastille with unique name + unique opaque ARGB + pending=true; picker shows it without mapping-mode side effects | Alert on failure; null skin same feedback style |
| Pending paint | Pastille pending=true | Swatch is empty circle (1 px stroke, logo grey), not filled square | N/A |
| First calibration | Author changes that pastille's ARGB to a new value | pending cleared; swatch becomes normal filled colour | Uniqueness still enforced on new ARGB |
| Same-ARGB set | Colour path calls set with unchanged ARGB | pending stays true; no bump/dirty | N/A |
| Rename only | Double-click name on pending Editor pastille; new unique name | Name updates; mappings updated; pending stays true | Duplicate/empty/product/compiled failures alert |
| Load old JSON | Pastille object without pending field | pending=false; existing filled swatch | Ignore unknown fields as today |
| Export/import | Skin JSON with pending true/false | Round-trips pending; imported pending still paints as circle until calibrated | Import refusal rules unchanged |

</frozen-after-approval>

## Code Map

- `Source/GUI/Skins/Dev/DevSkinTypes.h` -- `Pastille { name, argb }`; add `pending` (default false)
- `Source/GUI/Skins/Dev/DevSkinJson.cpp` -- round-trip `pending`; missing → false
- `Source/GUI/Skins/Dev/DevSkinLayer.h` / pastille CRUD (keep file-size gate: `DevSkinLayerPastille.cpp` if needed) -- `addPastille(..., pending)`; `setPastilleArgb` clear pending + bump only when ARGB changes; reject other-pastille ARGB collisions; new `renamePastille` (unique name, block product/compiled, rewrite `mappings` values)
- `Tools/SkinEditor/SkinEditorMainComponentActions.cpp` -- immediate Add with `suggestUniqueTempPastilleName` / `suggestUniqueOpaqueArgb` + pending; alert if skin null; select without mapping click side effects; `onRenamePastille` via `askText`
- `Tools/SkinEditor/SkinEditorUiHelpers.h` -- Temp N + unique opaque ARGB helpers
- `Tools/SkinEditor/SkinEditorPalettePanel.*` -- pending swatch paint; `listBoxItemDoubleClicked` + name-area hit-test (swatch excluded); callback for rename
- `Tools/SkinEditor/SkinEditorChromeTable.*` -- `paintPendingColourSwatch` (1 px, `textBrand()`)
- `Tools/SkinEditor/SkinEditorMappingsPanel.*` / mapping witness -- pending circle when mapped/witness colour is pending
- `Tools/SkinEditor/SkinEditorMainComponentMapping.cpp` -- do not treat programmatic Add selection as `pastilleClick`
- `Tests/Unit/DevSkinPendingPastilleTests.cpp` -- pending seeds via helpers composition; clear only on ARGB change; rename keeps pending + updates mappings; JSON/legacy; ColourChart/`duplicateSkin` seeds `pending == false`
- Reuse: `SkinEditorUi::askText`, `Layer::renameDevSkin` pattern, `isProductPastilleName`
- Do not change: Product catalog / `kTransparent` special role beyond uniqueness rules

## Tasks & Acceptance

**Execution:**
- [x] `Source/GUI/Skins/Dev/DevSkinTypes.h` + `DevSkinJson.cpp` -- `pending` field + JSON round-trip (default false)
- [x] `Source/GUI/Skins/Dev/DevSkinLayer*` -- add with pending; setArgb clear/bump only on real ARGB change + collision reject; `renamePastille` with mapping rewrite
- [x] `Tools/SkinEditor/SkinEditorUiHelpers.h` + `SkinEditorMainComponentActions.cpp` -- immediate Add (`Temp N` + unique opaque ARGB + pending); null-skin alert; post-Add select without mapping side effects; rename action via `askText`
- [x] `Tools/SkinEditor/SkinEditorPalettePanel.*` + Chrome table -- pending circle paint; double-click **name** (not swatch) → rename callback
- [x] `Tools/SkinEditor/SkinEditorMappingsPanel.*` + mapping witness -- pending circle consistency
- [x] `Tests/Unit/DevSkinPendingPastilleTests.cpp` (+ CMake) -- helpers composition, pending rules, rename+mappings, legacy JSON, product seeds not pending

**Acceptance Criteria:**
- Given an editable Editor skin, when the author clicks Add repeatedly, then each pastille is `Temp N` with unique opaque ARGB, pending circle, and no HeldColour/ArmedRow side effect from that Add.
- Given a pending pastille, when the author double-clicks its **name** and renames uniquely, then the name and mapping refs update and it stays pending; when they change its colour to a new ARGB, then pending clears.
- Given a same-ARGB colour re-apply, when set runs, then pending stays true and the document is not dirtied by that no-op.
- Given older skin JSON without pending, when loaded, then colours behave as calibrated (filled swatches).

## Implementation Notes

- Prior implementation was reverted after review intent_gap (no rename path). Re-derived including RENAME_UX and review patches.
- Pastille CRUD in `DevSkinLayerPastille.cpp`; tests in `DevSkinPendingPastilleTests.cpp`.
- Add sets `selectedPastille_` before `addPastille` so quiet `refreshAll` selection avoids mapping side effects; `selectPastille` uses `suppressSelectionNotify_`.
- Verified: DevSkinPendingPastille 0 failures; lint_touched OK; Skin Editor + Tests build.

## Spec Change Log

- review_loop 1 / intent_gap: Human chose RENAME_UX = DOUBLE_CLICK_NAME (double-click pastille name). Frozen intent + matrix/tasks updated. Prior code reverted before re-derive. KEEP: pending flag + JSON; unique Temp N + unique opaque ARGB; 1 px `textBrand()` empty circle; clear pending only on real ARGB change; Add without name dialog; no Product rename; avoid Add→mapping side effects; helper/composition tests.

## Review Triage Log

- high | intent_gap | Removing Add name dialog leaves no pastille-rename path in Skin Editor (no rename API/UI); authors are stuck with `Temp N` despite intent to customise names later. Evidence: grep finds no renamePastille/setPastilleName; former Ask dialog was the only naming step. Code reverted pending human decision. **Resolved:** DOUBLE_CLICK_NAME.
- medium | patch (bake into re-derive) | `setPastilleArgb` clears `pending` and bumps even when ARGB unchanged — clear pending + bump only when ARGB actually changes.
- medium | patch (bake into re-derive) | After Add, `selectPastille` fires `onPastilleSelected` → Idle enters HeldColour; ArmedRow auto-binds. Select for picker without mapping-policy click side effects.
- medium | patch (bake into re-derive) | Verification gap: helpers + `pending=true` composition under test.
- low | patch (bake into re-derive) | Assert ColourChart / duplicated compiled seeds have `pending == false`.
- low | defer | Pending swatch paint wiring has no unit test — GUI paint outside unit pyramid; keep manual check.
- low | false | Session save/load pending — session uses `exportToJson`; covered by JSON round-trip.
- low | maybe-false / reject | JSON non-bool `pending` coercion — unlikely everyday path.
- low | reject | ARGB space exhaustion fallback — not everyday.
- low | reject | Picker shows seed while list shows empty circle — intentional for calibration.
- low | defer | `selectPastille` may not scroll new row into view — polish.
- low | defer | Colour collision leaves picker on rejected colour — pre-existing UX shape.
- low | patch (bake into re-derive) | Null skin on Add returns silently — show failure feedback.
- medium | patch | `suggestUniqueOpaqueArgb` uses unbounded `for (;;)` — restore bounded random attempts + deterministic fallback so Add cannot hang.
- medium | patch | `renamePastille` does not reject `isProductPastilleName(newName)` — can create a product-named custom pastille if that chart name were absent from the palette; block target name too.
- medium | patch | No test that `setPastilleArgb` rejects another pastille's ARGB and leaves pending/ARGB unchanged.
- medium | patch | No test for rename empty name / case-insensitive name collision.
- low | patch | Three-arg `addPastille` default `pending=false` not asserted.
- low | patch | Unused `#include "DevSkinPaletteCatalog.h"` in `DevSkinLayerPastille.cpp`.
- low | reject | Accepting seeded ARGB without changing it cannot clear pending — intentional; placeholders are meant to be calibrated by a real colour change (nudge if keeping seed).
- low | defer | Rename discoverability (tooltip/hint) — polish after ship.
- low | defer | No automated test that quiet Add selection blocks mapping policy — GUI wiring; manual check.
- low | reject | Confusing uniqueness vs empty-circle conflict source — uniqueness is correct; no UI redesign in scope.
- low | reject | Header comment docs for setPastilleArgb contract — cosmetic.
- carried low | defer | Scroll new Add into view; paint unit tests; picker revert on collision.
- low | reject | Pending ellipse non-square — call sites use square `kColourSwatchSize`.
- low | reject | Double-click hit strip vs glyph bounds — swatch excluded per intent; padding in name column is acceptable.
- low | reject | JSON non-bool pending coerce; Temp N unbounded; ARGB full-space hang after bounded fix — not everyday.
- low | defer | `onAddPastille` always passes `pending=true` not unit-tested at action layer — GUI glue; helpers+explicit true covered in Layer tests.

## Design Notes

- Placeholder naming: `Temp 1`, `Temp 2`, … (next free, case-insensitive).
- Unique ARGB: opaque random with collision retry; no transparency.
- Pending circle: outline only, 1 px; thicken later only if visual QA says so.
- Rename: double-click name text only; Product blocked; pending unchanged.
- Deferred (not this re-derive): scroll-into-view on Add; picker revert on ARGB collision; paint unit tests.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor Matrix-Control_Tests` -- expected: build succeeds
- `./Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests DevSkinPendingPastille` -- expected: 0 failures
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++ under `Source/`, `Tests/`, `Tools/SkinEditor/`

**Manual checks (if no CLI):**
- Add several times → pending circles; no accidental mapping mode.
- Double-click name → rename; pending circle remains until colour changes.
- Edit colour → filled square; quit/reopen → pending still circles until calibrated.
