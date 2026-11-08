---
title: 'Skin Editor UI panels and session'
type: 'feature'
created: '2026-10-23'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '4e5f2f8ee8a8b366c41f8f3fae374dcd6adb7cc4'
context:
  - '{project-root}/_bmad-output/specs/spec-skin-editor/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/editor-surfaces.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/json-bounds.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Skin Editor V1 wastes the separate window (tabs + bottom action bar), lacks Skin Studio–grade Color tools, and has no overnight resume for incomplete Editor skins. Export must stay a complete-skin handoff for BMad-assisted product integration, not a draft dump.

**Approach:** Rebuild the editor chrome as a guttered multi-panel layout (Skins strip + Color | Palette | Mappings | Unbound), with Product skin / Editor skin workflow, Compare, Skin Studio–spirit panel contents, auto-push retained plus normal Push, Export of the current Editor skin only (blocked while Unbound > 0), Import appending an Editor skin, silent session autosave for resume, and a dirty quit modal (Export / Discard / Cancel).

## Boundaries & Constraints

**Always:**
- Labels: **Product skin** / **Editor skin** (not Dev skin in UI copy).
- Layout: full-width Skins band; four equal columns Color | Palette | Mappings | Unbound; chrome greys + Photocraft gutters per Skin Studio (`surface-app` / `surface-panel` / `surface-gutter` spirit — trust Studio capture, not Excel thickness).
- Actions live in the panel they belong to; Push stays normal-size in Skins; auto-push on Editor-skin layer changes when connected (V1 behavior kept).
- Product skins = compiled Black/Cream, read-only verify; Duplicate creates Editor skins; Rename / Delete / New apply to Editor skins; Compare toggles Standalone paint + panels between Product and Editor (Product view: Mappings read-only, Unbound empty).
- Color: SV square + vertical hue + editable `#` (6 hex) + R/G/B decimals; edits selected Palette pastille only; no current-colour preview swatch.
- Palette: Product colours (non-deletable) above Dev/new colours (`+ Add` / `Delete…`); click loads Color; hex edits write into the active **Editor** skin layer.
- Mappings / Unbound: Studio spirit (Object + State filters; Property → pastille rows; Unbound list + count badge on panel title; bind via Palette id only, never free hex).
- Export = current Editor skin JSON only; disabled while that skin has Unbound > 0. Import = append new Editor skin entry. Session autosave = silent full editor session resume (incomplete OK). Quit dirty → Export / Discard / Cancel.
- Keep thin bridge, `MATRIX_BUILD_SKIN_EDITOR` gates, compiled Skin SSOT untouched as Release truth.

**Never:**
- Tabs as primary IA; bottom global action bar; web/Skin Studio shell; product GUI preview inside the editor.
- Auto-duplicate all Product skins at launch; soft Export with Unbound remaining.
- Mutate ColourChart / Black / Cream C++ SSOT from the editor; ship editor in Release/AU/VST3.
- Force-translate UI labels; non-ASCII display punctuation in `PluginDisplayNames`-style copy (ASCII only).

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Edit while connected | Change pastille/mapping on Editor skin | Auto-push; Standalone repaints | Status shows ack/error |
| Compare ON (Product) | Toggle Compare with Product selected | Standalone shows Product; Mappings RO; Unbound empty; no Editor mutations | N/A |
| Export complete | Editor skin Unbound = 0 | Write single-skin JSON | N/A |
| Export blocked | Editor skin Unbound > 0 | Export control disabled | Operator uses Unbound list |
| Import | Valid single-skin (or multi-skin legacy) JSON | Append Editor skin(s); select imported | Malformed → status error; no wipe of session |
| Autosave resume | Relaunch after unclean quit mid-edit | Session restored including incomplete skins | Corrupt session → baseline + message |
| Quit dirty | Close with unsaved Editor edits | Modal: Export / Discard / Cancel | Cancel keeps editor open |
| Offline edit | Standalone down | Editor usable; disconnected status; queue/pending push per V1 | No fake preview |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMainComponent.*` -- Replace tab+list+bottom-bar UI with Skins band + four panels; keep `Layer` + `BridgeClient` ownership, `onChanged` auto-push, status labels.
- `Tools/SkinEditor/Main.cpp` -- Quit gate / dirty modal; window chrome; do not hardcode unrelated product version concerns beyond existing app shell.
- `Tools/SkinEditor/` (new panel/components as needed) -- Color picker (SV+hue+#RGB), Palette lists, Mappings, Unbound; register new `.cpp` on `Matrix-Control_SkinEditor` in `CMakeLists.txt`.
- `Source/GUI/Skins/Dev/DevSkinLayer.*` -- Add Dev-only pastille delete; support Compare selection / push of Product vs Editor; keep `duplicateSkin` / CRUD.
- `Source/GUI/Skins/Dev/DevSkinJson.*` -- Add **single Editor skin** export; change Import to **append** Editor skin(s) (do not `replaceAll`-wipe session); keep schemaVersion discipline; session file may reuse layer JSON shape or a thin wrapper — implementer choice.
- `Source/GUI/Skins/Dev/DevSkinBridgeClient.*` -- Reuse; no protocol redesign.
- `Source/Shared/ProjectPaths.*` -- Session autosave location via product Application Support (`makeProductPropertiesFileOptions` / `getApplicationDataDirectory`); Skin Editor–scoped filename.
- `Tests/Unit/DevSkinLayerJsonTests.cpp` (+ matrix tests if needed) -- Single-skin export, append import, pastille delete, Unbound export gate helpers.
- `Source/GUI/Tests/TestColourPickerButton.*` -- Reference only (JUCE `ColourSelector`); do not couple editor to Tests target.
- Do **not** change: `ColourChart` / Skin SSOT maps, Release packaging OFF path, AU/VST3 receiver absence, product Looks.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/*` + `CMakeLists.txt` -- Multi-panel chrome (gutters/greys), Skins band (Product/Editor combos, Duplicate/Rename/Delete/New, Compare, Push, Export/Import, connection status) -- replace tabs/bottom bar
- [x] `Tools/SkinEditor/*` -- Color panel (SV+hue+#RGB) wired to selected pastille on active Editor skin
- [x] `Tools/SkinEditor/*` -- Palette panel (Product vs new colours, Add/Delete with referenced confirm) -- Studio row spirit
- [x] `Tools/SkinEditor/*` -- Mappings + Unbound panels (Object/State, bind path, count badge, Compare RO/empty rules)
- [x] `Source/GUI/Skins/Dev/DevSkinLayer.*` -- `deletePastille` (Dev-only names); Compare/push selection helpers as needed
- [x] `Source/GUI/Skins/Dev/DevSkinJson.*` -- Single-skin export; append import; session load/save helpers
- [x] `Tools/SkinEditor/Main.cpp` (+ component) -- Silent session autosave + dirty quit modal (Export/Discard/Cancel)
- [x] `Tests/Unit/DevSkinLayerJsonTests.cpp` -- Cover single-skin export, append import, delete pastille, export-blocked when unbound
- [x] `Scripts/quality/lint_touched.py` -- Clean on touched C++

**Acceptance Criteria:**
- Given Skin Editor open, when viewing the window, then Skins strip + four guttered panels are visible without tabs or a global bottom action bar.
- Given connected Standalone Dev, when editing an Editor skin pastille, then Standalone repaints without requiring Push; Push still forces a send.
- Given Product skin selected and Compare on, when inspecting Mappings/Unbound and Standalone, then Product paint shows, Mappings are read-only, Unbound is empty.
- Given Editor skin with Unbound > 0, when Export is considered, then Export is disabled; when Unbound = 0, Export writes JSON for that Editor skin only.
- Given Import of a skin JSON, when confirmed, then a new Editor skin appears without wiping other Editor skins.
- Given dirty Editor work and app quit, when choosing Discard vs Cancel vs Export, then Discard loses edits, Cancel stays, Export follows Export rules then can quit if successful.
- Given mid-work relaunch, when opening Skin Editor, then the autosaved session restores including incomplete Editor skins.

## Implementation Notes

- Implemented multi-panel Skin Editor UI, single-skin export/append import, session autosave (`DevSkinSession`), Compare push via `makePushPayloadForSkin`, dirty quit gate.
- Matrix coverage extended: session round-trip, corrupt→baseline (`importSessionFromJsonOrReset`), Compare product payload, quit Export gate = `isEditorSkinExportAllowed`.
- Manual compound-launch checks still required (live paint, Compare on Standalone, quit modal UX, overnight resume).
- `SkinEditorMainComponent.cpp` remains large; panels extracted but shell still dense.

## Spec Change Log

## Review Triage Log

- high — `deletePastille` clears mappings before verifying pastille exists — verified DevSkinLayer.cpp:191–205; missing name returns fail after mapping erase without bump/rollback.
- high — Discard quit leaves autosave timer armed — verified tryQuitWithDirtyGate Discard deletes session file then markClean without stopTimer; timerCallback can rewrite file before process exit.
- medium — Unclean quit within 400ms debounce loses latest edits — verified markDirty starts 400ms timer; quit/dirty paths do not flush autosaveSession first.
- medium — Multi-skin append can leave partial imports — verified appendEditorSkinsFromJson loop returns mid-way after earlier appendDevSkin succeeded.
- medium — Toolbar Export success does not markClean — verified onExportJson success path vs quit Export markClean.
- medium — saveSessionFile can write empty file if exportSessionToJson empty — verified replaceWithText without empty check.
- medium — selectedPastille_ not cleared on Editor skin switch/delete — verified comboChanged/onDelete paths.
- medium — Editor skin Delete has no confirm — verified onDelete(); referenced pastille delete has confirm.
- medium — Mappings/Unbound refresh keeps selected row across rebuild — verified refresh() without deselectAllRows.
- medium — Session import does not require document=skinEditorSession — verified importSessionFromJson accepts any layer JSON.
- medium — quitExportGate test only rechecks Layer gate — verified assertQuitExportGateUsesExportAllowed; pre-verified gap.
- medium — deletePastille test does not assert palette removal — verified assertDeletePastille; pre-verified gap.
- medium — append remint / multi-skin append untested — verified assertAppendImportKeepsExisting only; pre-verified gap.
- low — cleanRevision_ written never read — verified markClean; reject complexity (delete dead field is fine as patch if bundled).
- low — allColourPropertyNames unused — verified KeyNames; patch delete dead API.
- low — surfaceGutter unused — gutters are layout gaps over surfaceApp; false (visual intent met via kGutter spacing).
- low — isSessionDirty public unused — false for users; cosmetic.
- low — Compare without setClickingTogglesState — cosmetic chrome; reject (manual setToggleState works).
- low — duplicate askPastille helpers — defer (DRY later).
- low — SkinEditorMainComponent still oversized — defer.
- false — textEditorTextChanged empty vs live sync — focus-lost + Return commit hex/RGB; mid-keystroke apply would push invalid hex; Design Note read as commit sync not per-keystroke.
- false — createEmptyDevSkin seeds Cream — intentional New = Duplicate Cream baseline; not a blank empty palette.
- false — spec still in-progress / empty notes — stale reviewer snapshot; status is in-review with Implementation Notes filled.
- maybe-false — dirty after autosave still prompts quit — intentional: session file is resume aid; Export handoff is separate; dirty tracks unsaved Editor work vs last clean export/load. No patch.
- maybe-false — exportCurrentEditorSkinToFile empty-json vs write failure message — need clearer error; low harm; include in export path patch if easy.
- medium — Task claimed session helpers in DevSkinJson but live in DevSkinSession — documentation drift only; false for runtime; no code patch.

## Design Notes

- UI copy English ASCII: Product skin, Editor skin, Duplicate, Compare, Push, Export, Import, Unbound badge.
- Session ≠ Export: session may be incomplete; Export remains the BMad handoff artifact and stays Unbound-gated.
- Legacy multi-skin export files: append all Dev skins as Editor entries (no silent replace).
- Color: Photoshop SV square + hue strip; no H/S/V sliders; sync `#`/RGB live into selected pastille.
- Prefer splitting oversized `SkinEditorMainComponent` into panel components under `Tools/SkinEditor/` rather than growing one file past project size norms.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64` with `MATRIX_BUILD_SKIN_EDITOR=ON` -- configures Skin Editor target
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` -- build succeeds
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` (or project test target with DevSkin tests) -- DevSkin JSON/layer tests pass
- `python3 Scripts/quality/lint_touched.py` on touched C++ -- clean

**Manual checks:**
- Compound launch Skin Editor + Standalone Dev: edit colour → live paint; Compare toggles Product vs Editor; Export disabled with Unbound; quit modal; relaunch restores session.
