---
title: 'Skin Designer blank start, draft session, Editor→Designer rename'
type: 'feature'
created: '2026-11-11'
status: 'done'
route: 'dispatch'
review_loop_iteration: 1
baseline_commit: 'f5e5cca23dddee2ad6452556a6af4b54733854a5'
context:
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
  - '{project-root}/CONVENTIONS.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** The Skin Editor restores previous Designer skins on launch and mixes Import/Export language, while the product name and code still say "Editor". Guillaume wants a blank Designer workspace, a Trials-style multi-skin JSON document, and a full Editor→Designer rename including tool paths.

**Approach:** Rename to Matrix-Control Skin Designer (hard cut on session/window files and bundle id). Launch loads prefs only. Session bar: Load → Save → Save as → JSON Sync → Undo/Redo → Compare/Push. One multi-skin JSON pack is the document of record; Unbound stays visible for BMad when hardcoding into Matrix-Control (no Export gate in the Designer).

**Decisions (renegotiated):**
- **Trials pack** is the workspace document (all Designer skins + activeSkinId).
- **No Export button** — completeness is checked when BMad implements skins into Matrix-Control from the JSON.
- **Load + Save + Save as** (order: Load, Save, Save as). Save overwrites the current watched path when known; otherwise Save opens a chooser. Save as always opens a chooser.
- Dirty cleared only by successful Save / Save as / Load (not by a separate Export).
- Load **replaces** Designer skins (not append).

## Boundaries & Constraints

**Always:**
- App display name and suggested pack filenames use `Matrix-Control Skin Designer`.
- Skins panel label `Designer:`; Palette group `Designer colours`; user-facing "Editor skin/work" → "Designer …".
- Blank launch: compiled Product skins only; `designerSkinId` empty; keep prefs `lastSkinJsonDirectory` + `jsonSyncMode` only; no JSON watch until Load/Save/Save as.
- Load replaces Designer skins from JSON; arms JSON watch; no Unbound gate; accepts multi-skin packs (and legacy single-skin files).
- Save / Save as write multi-skin Dev JSON via `exportToJson` (`schemaVersion`, `activeSkinId`, all Designer `skins`); Unbound/pending allowed; mark clean; arm/refresh watch; ensure `.json` extension; default stem `Matrix-Control Skin Designer - {workspace}.json`.
- Session bar order: Load (gap) Save (gap) Save as (sep) JSON Sync (sep) Undo (gap) Redo (sep) Compare (gap) Push.
- Quit dirty: Save / Discard / Cancel.
- JSON Sync reload uses the same replace/refuse rules as Load (`replaceDesignerSkinsFromJson`).
- Full rename: `Tools/SkinDesigner/`, CMake target/option/artefacts, DevSkin session document/keys/paths, Standalone receiver, tests, quality scripts, project rules that name the tool.
- Hard cut: `skin-designer-session.json`, document `skinDesignerSession`, window state `skin-designer-window.state`; no read of old session; BUNDLE_ID → `…SkinDesigner`.
- Rename Dropbox `… Skin Editor - Trials.json` / Product Skins → Skin Designer prefix (content unchanged).
- ASCII-only UI copy; English in code; `lint_touched.py` includes `Tools/SkinDesigner/`.

**Never:**
- Rename `PluginEditor`, `juce::TextEditor`, MIDI `EditorPath`, or skin tokens `EditorBackground` / `EditorText` / `EditorSelectionBackground`.
- Rewrite `_bmad-output/` historical story/spec prose (except this new spec).
- Change Matrix-Control plugin runtime skin behaviour for end users.
- Soft-migrate old `skin-editor-session.json` / `skinEditorSession`.
- Global blind replace of the substring `Editor`.
- Import-style append on Load; Designer Session Export button / Unbound write gate for saving the pack.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Blank launch | Old or missing session on disk | Prefs-only restore; no Designer skins; no watch prompt | Ignore unrecognized/old session document |
| Load Trials JSON | Multi-skin schema v2 file | Designer skins replaced by file; watch armed; combo shows loaded skins | Alert on parse/refuse/unreadable; layer unchanged |
| Save (path known) | Dirty workspace, watched file set | Pack overwritten; dirty cleared; watch refreshed | Write failure alert |
| Save as | Chooser path | Pack written; watch armed on new path; dirty cleared | Chooser cancel = no-op |
| Load after save | Previously saved pack | Designer skins replaced; active skin restored when id exists | Alert on parse/refuse; layer unchanged |
| JSON Sync reload | Watched file emptied of Designer skins | Reload refused; layer unchanged (same as Load) | Alert |
| Quit dirty | Unsaved Designer edits | Offer Save / Discard / Cancel | Cancel keeps app open |
| Rename cut | First launch after rename | New empty designer session path; orphan old session ignored | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/` → `git mv` to `Tools/SkinDesigner/` + rename `SkinEditor*` → `SkinDesigner*` (classes/namespaces/includes).
- `Tools/SkinDesigner/SkinDesignerMainComponent*` — blank `loadSessionOrBaseline`; Session layout/wiring; Load / Save as draft / Export; quit gate copy; dirty helpers `markDesigner*`.
- `Tools/SkinDesigner/SkinDesignerSkinsPanel.*` — `Designer:` label, combo/nav/tooltips.
- `Tools/SkinDesigner/SkinDesignerPalettePanel.cpp` — `Designer colours` / empty hint.
- `Tools/SkinDesigner/SkinDesignerChromeFonts.h` — `sessionWellPreferredWidth` for new order + `Save as draft`.
- `Tools/SkinDesigner/SkinDesignerUiHelpers.h` — default draft/export filename prefix; no-skin hint if needed.
- `Tools/SkinDesigner/Main.cpp` + CMake `PRODUCT_NAME` — `Matrix-Control Skin Designer`.
- `Source/GUI/Skins/Dev/DevSkinSession.{h,cpp}` — prefs-only load; `DesignerSessionState`; `designerSkinId`; `skin-designer-session.json`; document `skinDesignerSession`.
- `Source/GUI/Skins/Dev/DevSkinJson.{h,cpp}` + `DevSkinLayer.*` — rename Editor-skin APIs to Designer; keep `exportToJson` for drafts; keep single-skin export for Export.
- `Source/Standalone/SkinEditorReceiverStandalone.*` → `SkinDesignerReceiver*`; `MatrixControlStandaloneApp.cpp` host member.
- `CMakeLists.txt`, `CMakeUserPresets.json`, `.vscode/tasks.json` + `launch.json` — target `Matrix-Control_SkinDesigner`, option `MATRIX_BUILD_SKIN_DESIGNER`, artefact paths.
- `Scripts/quality/lint_touched.py`, `check_skin_editor_packaging.py` (+ tests) → designer names; gate path `Tools/SkinDesigner/`.
- `Tests/Unit/SkinEditor*Tests.cpp` → `SkinDesigner*`; DevSkin session/import-watch tests for prefs-only + document id.
- Project mentions: `.cursorrules`, `CONVENTIONS.md`, `.cursor/rules/matrix-control-project.mdc`, `clean-code.mdc`, `_bmad-output/project-context.md`.
- User file: `/Users/Guillaume/Library/CloudStorage/Dropbox/Dev/Projects/JUCE/Matrix-Control/Skins/Matrix-Control Skin Editor - Trials.json` → `… Skin Designer - Trials.json` (also rename sibling `Product Skins.json` for the same prefix).
- **Do not touch:** `PluginEditor*`, MIDI `EditorPath*`, `SkinColourProperty` Editor* tokens, color-picker `*Editor_` TextEditor fields.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/` → `Tools/SkinDesigner/` — `git mv` folder + files; rewrite includes/namespaces/classes `SkinEditor*` → `SkinDesigner*`.
- [x] `Source/Standalone/SkinEditorReceiverStandalone.*` + callers — rename to SkinDesigner receiver symbols.
- [x] `Source/GUI/Skins/Dev/DevSkinSession.{h,cpp}` — Designer session names/paths; prefs-only loader; hard-cut document id.
- [x] `Source/GUI/Skins/Dev/DevSkinJson.{h,cpp}` + `DevSkinLayer.{h,cpp}` — Designer skin API names; keep draft=`exportToJson`, export=single-skin.
- [x] `Tools/SkinDesigner/SkinDesignerMainComponent*` — blank launch; Load replace; Save as draft; Session order; quit gate; Import→Load naming.
- [x] `Tools/SkinDesigner/SkinDesignerSkinsPanel.*` + `SkinDesignerPalettePanel.cpp` + `SkinDesignerUiHelpers.h` + `SkinDesignerChromeFonts.h` — Designer labels + well width.
- [x] `CMakeLists.txt` + presets + `.vscode/*` + packaging scripts/tests — SkinDesigner target/option/PRODUCT_NAME/BUNDLE_ID/artefacts.
- [x] `Scripts/quality/lint_touched.py` + project rules/docs naming the tool — point at Skin Designer / `Tools/SkinDesigner/`.
- [x] `Tests/Unit/*` SkinEditor→SkinDesigner + session prefs-only / document hard-cut coverage.
- [x] Dropbox `Skins/Matrix-Control Skin Editor - *.json` — rename Trials + Product Skins to Skin Designer prefix (content unchanged).
- [x] Build SkinDesigner target + run touched unit tests + `lint_touched.py`.

**Acceptance Criteria:**
- Given a prior Skin Editor session on disk, when Skin Designer launches, then no Designer skins are loaded and no JSON-sync launch prompt appears.
- Given blank workspace, when user Loads the Trials file, then its Designer skins appear and JSON Sync can watch that path.
- Given several Designer trials with Unbound > 0, when user chooses Save as draft, then one multi-skin JSON pack is written and dirty clears; Export stays disabled/blocked for incomplete current skin.
- Given Unbound = 0, when user Exports, then single-skin ready JSON is written as today.
- Given the Session well, when laid out, then control order matches Load, Save as draft, Export, JSON Sync, Undo, Redo, Compare, Push with the specified gaps/separators.
- Given the built app, when opened, then product name is Matrix-Control Skin Designer and Skins/Palette show Designer wording.
- Given repo search for tool paths, when checked, then `Tools/SkinDesigner` and CMake SkinDesigner names are used; false friends (PluginEditor, TextEditor, EditorBackground) remain.
- Given the Dropbox Skins folder, when listed, then Trials (and Product Skins) use the Skin Designer filename prefix.

## Implementation Notes

- Implemented via dispatch subagent; parent added quit-dirty prompt policy helpers in `SkinDesignerUiHelpers.h` + unit coverage for matrix row Quit dirty.
- File I/O lives in `SkinDesignerMainComponentFiles.cpp` (size gate). `replaceDesignerSkinsFromJson` leaves layer untouched on refuse.
- Beyond-spec (kept): Load confirms when dirty; empty workspace quits without prompt; default pack stem falls back to `… Trials.json` when no watched path.
- Post-review patch: removed Export; Load/Save/Save as; SafePointer + fileDialogOpen_; reload uses replaceDesignerSkinsFromJson; .json extension helper; clearer Load missing-file alert.
- `appendDesignerSkinsFromJson` retained for tests; app Load uses replace.
- Verification (after patch): SkinDesigner + Tests build OK; DevSkinWorkspaceFiles / ChromeLayout = 0 failures; `lint_touched.py` OK.

## Spec Change Log

- 2026-11-11 — Review vs Desktop GPC `Save as draft - Spec.md`: locked multi-skin Trials pack for Save as draft; rejected GPC single-skin + `(draft)` + Import append; kept Unbound split and dirty semantics from GPC; recorded Load replace + Save as draft label/order from this chat.
- 2026-11-11 — Intent renegotiation after review: drop Export; Load + Save + Save as; dirty cleared only by Save/Save as/Load; Unbound checked later by BMad when hardcoding into Matrix-Control; JSON Sync reload matches Load refuse rules.

## Review Triage Log

- GPC vs current-spec conflict (single-skin draft vs Trials pack) — high — settled by human: option 2 Trials pack.
- GPC 3-build split vs one lot — medium — kept one lot per human “même lot” + Designer rename scope.
- GPC Import append vs Load replace — medium — kept replace for blank-start document model.
- Blind: Export markDesignerClean clears whole multi-skin dirty — high — verified: onExportJson calls markDesignerClean after single-skin write; conflicts with Trials-pack dirty model; frozen still says Export clears dirty → intent_gap.
- Edge: same Export dirty claim — high — same evidence; grouped with above.
- Blind/Edge: JSON Sync reload still uses importFromJson (unguarded) vs Load replaceDesignerSkinsFromJson — high — verified in ImportWatch reloadWatchedImportJsonFromDisk; empty watched file can wipe workspace → patch (pending after intent_gap).
- Blind/Edge: async FileChooser captures raw this — medium — verified browseForWorkspaceFile/onSaveDraft/onExportJson; Load confirm uses SafePointer → patch pending.
- Blind/Edge: Export/Save can overwrite watched opposite-kind file — medium — verified suggestedDraftFile returns watched path; exportCurrentDesignerSkinToFile re-baselines watch → patch/intent pending.
- Blind: dirty Load confirm has no Save-as-draft-first — low — true but beyond frozen; reject complexity for this lot.
- Blind: misleading Load error on missing file — medium — verified loadFileAsString empty path → patch pending.
- Blind: hard-cut leftovers / old .app / BUNDLE_ID — false — intentional hard cut in frozen; no defect.
- Blind: spec ctest pattern / AC gaps / Dropbox path bookkeeping — false — rejected (fix is edit this build's spec).
- Blind: pending-colour draft test gap / weak quit string asserts — low — deferred/reject; DevSkinWorkspaceFiles covers Unbound draft gate.
- Blind: appendDesignerSkinsFromJson dead / comments stale / Undo placeholders — low — defer or reject (no everyday harm this lot).
- Edge: missing .json extension on save — medium — verified chooser can return stem without extension → patch pending.
- Edge: stacked Load dialogs — medium — no loadDialogOpen_ guard → patch pending.
- Edge: Compare-on saveDraft selectSkin divergence — maybe-false — needs runtime with Compare on; not settled from diff alone.
- Edge: silent corrupt session prefs — low — hard-cut prefs-only; reject adding notice complexity this lot.
- Edge: compiled skins replaced via replaceAll — false — existing Layer contract; not a new product defect.
- Edge: deletion of corrupt-session message / lastExportDirectory — low — intentional prefs hard cut; reject.
- Verification-gap: reload adoption of replaceDesignerSkinsFromJson — high — pre-verified; same as reload patch above.
- Verification-gap: MainComponent Load/Save/Export/quit wiring untested — medium — defer (GUI not in unit target; manual checks listed).
- Verification-gap other: legacy watch assertion / temp dir name / default filename untested — low — defer/reject.

## Design Notes

- **Load = document open (replace Designer skins)** — matches Import→Load and blank-start; append would duplicate Trials on re-Load.
- **Save as draft = Trials pack via `exportToJson`** — all Designer skins in one file so trial switching / compare / colour reuse stays inside the app; Export stays single-skin `exportDesignerSkinToJson`.
- **Older GPC** (single-skin Save + `(draft)` stem + 3 split builds + Import append) is partially superseded; useful leftovers: Unbound rules, dirty clean on save/export, shared schema.
- **Quit dirty** — Save as draft / Discard / Cancel only; Export stays on the Session bar with its Unbound gate.
- **Hard cut** — orphan `skin-editor-session.json` left on disk; Guillaume has Trials backup.
- Rename with whole-token maps (`SkinEditor`→`SkinDesigner`, `editorSkin`→`designerSkin`, …); never raw `Editor`→`Designer`.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64 && cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinDesigner` -- expected: build succeeds
- `ctest --preset macos-debug-arm64 -R 'SkinDesigner|DevSkinSession|DevSkinLayer|DevSkinLaunch' --output-on-failure` (or project-equivalent test invoke) -- expected: pass
- `python3 Scripts/quality/lint_touched.py` -- expected: pass on touched C++ under Source/Tests/Tools/SkinDesigner

**Manual checks:**
- Launch app → empty Designer combo / START coaching; Load Trials → skins present; Save as draft round-trip; Export still gated on Unbound; Session order visual check; macOS menu/Dock name Skin Designer.
