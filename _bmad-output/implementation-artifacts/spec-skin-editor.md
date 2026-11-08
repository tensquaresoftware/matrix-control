---
title: 'Matrix-Control Skin Editor V1'
type: 'feature'
created: '2026-10-23'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '0cfd6da28c68bc8cf44efb1454dd94d590cee780'
context:
  - '{project-root}/_bmad-output/specs/spec-skin-editor/SPEC.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/thin-bridge.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/dev-runtime-layer.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/json-bounds.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/cmake-packaging.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/editor-surfaces.md'
  - '{project-root}/_bmad-output/specs/spec-skin-editor/code-map.md'
  - '{project-root}/_bmad-output/specs/spec-skins-colour-model-ssot/SPEC.md'
  - '{project-root}/CONVENTIONS.md'
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Inventing skins still means hand-editing C++ after Skin SSOT shipped. Operators need a Dev author tool that edits a runtime layer and judges truth on real Standalone paint, with portable JSON — without a web shell or holding multi-OS alpha hostage.

**Approach:** Same-repo Dev CMake target for a native JUCE **Matrix-Control Skin Editor** that owns a Dev runtime layer (named skins, palette, mappings, Unbound text bind), pushes one-way to a gated Standalone Dev receiver for live repaint, and mandatory JSON export/import of that layer only. Cursor Dev UX: distinct targets/apps plus a **compound launch** that starts Skin Editor and Standalone Dev together (editor-alone launch kept for offline).

## Boundaries & Constraints

**Always:**
- Branch onto delivered Skin SSOT (`ColourChart`, structured object × property × state, Black/Cream, Looks); C++ remains product SSOT; editor = Dev runtime layer; JSON = save/load of that layer only.
- Separate Standalone JUCE editor app; thin one-way push (identity + palette + mappings) → Standalone Dev apply/repaint; minimal ack/errors; visual truth = Standalone Dev paint only.
- Black/Cream compiled read-only; mutations require Duplicate to a free-text named Dev skin; Dev may add pastille names beyond ColourChart.
- Unbound = editor text list + bind only (no overlay on product paint).
- Editor V1 done = Dev CRUD + live paint + actionable Unbound + JSON round-trip.
- Editor + receiver gated out of Release user packaging and AU/VST3; alpha ships on Black/Cream without this tool.
- Cursor: compound launch config for Skin Editor + Standalone Dev; keep separate single-target launches; do not have the editor process spawn Matrix-Control.

**Never:**
- Reopen Skin SSOT model gate / Skin Studio Lot C / web shell / product GUI preview in the editor.
- Auto-promote JSON → Release C++; IPC into DAW-hosted plugins; shared undo / bidirectional IDE.
- Third colour system; local shrink/hex on product user GUI path; in-layout edit panel on Release MainComponent; in-place edit of compiled BC.
- Contributors / Dev distribution / five shipped skins as V1 requirements; commit/push/PR unless explicitly asked.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy invent | Duplicate Cream → named Dev; edit mapping; push while Standalone Dev up | Ack; Standalone Dev repaints from layer | N/A |
| Offline edit | Standalone Dev down; edit Dev layer | Editor usable; disconnected status; no fake preview | Show disconnected; queue push until connect |
| Push-on-connect | Connect with active Dev skin | Prefer push active skin so surfaces match | On apply error: show code + message; keep editor layer |
| Unbound bind | Bind unbound key to pastille; push | Mapping added; row leaves Unbound; paint updates | Invalid pastille: reject bind with message |
| BC mutation attempt | Edit while Black/Cream selected | Blocked until Duplicate | Clear UI cue (read-only) |
| JSON round-trip | Export → quit → fresh session → import | Same names/palette/mappings restored; push if connected | Malformed JSON: refuse import; keep prior layer |
| Disconnect / quit Standalone | Receiver gone mid-session | Editor keeps layer; Standalone falls back to compiled BC | Editor shows disconnected |
| Release build | Release preset / AU/VST3 | No editor binary; no Dev receiver | Configure/build must not link receiver into user formats |

</frozen-after-approval>

## Code Map

**Reuse (do not fork colour model):**
- `Source/GUI/Skins/SkinValues.h` — `SkinObjectType` × `SkinColourProperty` × `SkinColourState` / `SkinColourKey`
- `Source/GUI/Skins/ColourChart.h` — closed pastilles
- `Source/GUI/Skins/Skin.h` / `Skin.cpp` / `SkinColours*.h` / `ISkin*.h` — maps + resolution
- `Source/GUI/Looks/` + LookBuilders / `WidgetFactory` — injection path
- `Source/GUI/PluginEditorSkinScale.cpp` — current Black/Cream switch (`applySkinFromItemId`); extend carefully for Dev overlay in Standalone Dev only

**Standalone / CMake precedents:**
- `CMakeLists.txt` — `juce_add_plugin(Matrix-Control)`; `Matrix-Control_Standalone` extras under `Source/Standalone/`; `MATRIX_BUILD_TESTS` gated console apps; `ExportMatrixModals` always-on console
- `CMakeUserPresets.json` — Debug/Release binaryDirs under `Builds/`
- `.vscode/launch.json` / tasks — Standalone launch pattern; add Skin Editor + **compound** config
- Artefacts: `Builds/.../Matrix-Control_artefacts/$CONFIG/Standalone/Matrix-Control.app`

**New (expected):**
- Dev-layer types + JSON I/O (shared Dev sources; editor + Standalone Dev)
- Thin bridge client (editor) / receiver (Standalone Dev, compile-gated)
- New option-gated `juce_add_gui_app` (or equivalent) Skin Editor target + UI surfaces per `editor-surfaces.md`
- CMake option e.g. `MATRIX_BUILD_SKIN_EDITOR` (exact name implementer choice) default OFF for Release user path

**Do not change:** Skin SSOT tables as Release SSOT; Orbitron ≠ PatchNameDisplay; product user path colour shortcuts.

## Tasks & Acceptance

**Execution:**
- [x] Dev-layer types — seed from Skin SSOT; Duplicate BC → named Dev; free-text names; Dev-only pastilles; Unbound inventory — `dev-runtime-layer.md`
- [x] Standalone Dev receiver — apply payload + repaint; Dev compile gate; absent AU/VST3/Release — `thin-bridge.md`, `cmake-packaging.md`
- [x] Skin Editor CMake target + app shell — connection status; no product preview — `cmake-packaging.md`
- [x] Thin bridge client — localhost one-way push + minimal ack/errors; prefer push-on-connect of active skin — `thin-bridge.md`
- [x] Editor surfaces — skin list CRUD, palette, mappings, Unbound bind, JSON export/import — `editor-surfaces.md`, `json-bounds.md`
- [x] Cursor Dev UX — launch config for Skin Editor alone + compound “Skin Editor + Standalone Dev”; optional build task; distinct `.app` name — `.vscode/launch.json`
- [x] Packaging proof — Release / plugin formats omit editor + receiver — `cmake-packaging.md`
- [x] Tests / checks — unit-test Dev-layer JSON round-trip and Unbound bind edge cases where feasible; `Scripts/quality/lint_touched.py` on touched C++

**Acceptance Criteria:**
- Given Dev builds of Skin Editor + Standalone Dev, when operator Duplicates Cream (or Black) to a named Dev skin, rebinds a mapping (and clears an Unbound row if present), and pushes, then Standalone Dev repaints live without a product preview in the editor and without mutating compiled SSOT.
- Given an exported Dev-layer JSON, when the editor is restarted and that file is imported, then palette + mappings + Dev skin names match the export (round-trip); a subsequent push updates Standalone Dev when connected.
- Given Standalone Dev is down, when the operator edits the Dev layer, then the editor stays usable offline with disconnected status and no fake preview.
- Given a Release user / AU / VST3 build path, when packaging runs, then Skin Editor binary and Dev receiver are absent.
- Given Cursor compound launch, when that config runs, then Skin Editor and Standalone Dev both start; editor-alone launch remains available.

## Implementation Notes

- baseline_commit originally `0cfd6da2`; branch later reset onto `main` at hotfix merge `f8e599bd` so Skin Editor work sits on current main.
- `MATRIX_BUILD_SKIN_EDITOR` ON in Debug presets / OFF in Release; target `Matrix-Control_SkinEditor`; bridge TCP port `39217`.
- Matrix audit: `DevSkinMatrix` + `DevSkinLayerJson` unit tests + `Scripts/quality/check_skin_editor_packaging.py`; live chrome paint remains a manual check.
- Cursor: `Debug: Skin Editor`, compound `Debug: Skin Editor + Standalone Dev`.

## Spec Change Log

## Review Triage Log

- high — BridgeServer/Client use `InterprocessConnection(false,…)`; push/ack handlers can touch PluginEditor / UI off the message thread — verified in DevSkinBridgeServer.cpp / DevSkinBridgeClient.cpp ctors.
- high — Editor disconnect does not clear Dev overlay; `connectionLost` only resets the socket while Spec requires Standalone fall back to compiled BC — verified vs ReceiverController::stop-only clear.
- high — One-shot `attachIfNeeded` via `getActiveEditor()` with ignored `start()` failure can leave a session with no receiver — verified SkinEditorReceiverStandalone.cpp.
- medium — `BridgeSession::takePushOnConnect` returns pending but never clears `hasPending_` — verified DevSkinBridgeSession.cpp:24–28.
- medium — `MATRIX_SKIN_EDITOR_RECEIVER=1` applied to shared `Matrix-Control` so Debug AU/VST3 compile the receiver API surface — verified CMakeLists.txt ON block.
- medium — `connectionLost` `callAsync([this])` can use-after-free if Connection is destroyed first — verified BridgeServer.cpp:19–25.
- medium — `createConnectionObject` replaces `activeConnection_` without disconnecting prior client — verified BridgeServer.cpp:109–112.
- medium — `compiledBaseVariant` only checks compiled ids; Dev-from-Dev Black lineage defaults to Cream — verified DevSkinApplier.cpp + Layer::duplicateSkin sets derivedFrom to source id.
- medium — `pastilleFromVar` accepts weak hex via `getHexValue64` without length/digit validation — verified DevSkinJson.cpp.
- medium — Bridge wire JSON+MCSK frame has no round-trip unit test (verification-gap) — pre-verified.
- medium — Inventory “SSOT match” test compares curated list to itself (verification-gap) — pre-verified.
- medium — Packaging script not on normal CI path (verification-gap) — pre-verified; route: assert Release presets stay OFF + keep script.
- medium — `decodeMessage` trusts framed length with no max cap — verified protocol path claim.
- medium — `sendMessage` failure not surfaced to operator — verified send path claim.
- medium — JSON import allows duplicate skin ids — verified appendImportedDevSkins claim.
- medium — Bridge listen not bound to loopback while contract says local-only — verified `beginWaitingForSocket(kBridgePort)` alone.
- low — Editor target links `DevSkinBridgeServer.cpp` via shared source list — verified MATRIX_SKIN_DEV_LAYER_SOURCES; reject complexity if split is large? Keep as patch (exclude server from editor).
- low — Skin Editor `Main.cpp` hardcodes version `"1.0.0"` vs CMake `PROJECT_VERSION` — patch.
- low — Packaging script assumes Builds/ exists — patch mkdir.
- low — Empty pastille name treated as cancel in Set Mapping UI — reject (unlikely everyday; Layer already supports erase).
- low — Orphan `MATRIX_CONTROL_PROJECT_ROOT` on Standalone Dev launch — false (pre-existing Standalone launch pattern, not Skin Editor defect).
- low — `createEmptyDevSkin` unused / misnamed — reject (dead API, no user harm in V1).
- low — Auto-push on every onChanged without coalesce — maybe-false / defer (V1 acceptable; settle with manual rapid-edit soak).
- low — revision cast to int truncates — reject (revision stays small in V1).
- false — Codesign/notarization removal in baseline-scoped diff — already on `main` via hotfix merge; not Skin Editor working-tree intent.
- false/defer — README/docs missing Skin Editor discoverability — defer (helpful, not ship-blocker for V1 done).
- defer — Packaging script Ninja/JUCE_DIR hardness — defer (local Dev gate still works on project presets).

## Design Notes

- Transport: implementer choice within `thin-bridge.md` (prefer JUCE `InterprocessConnection` or localhost socket); local-only; not JSON-as-live-pipe.
- Target names: prefer clear product strings in UI (“Matrix-Control Skin Editor”) and distinct CMake/launch labels so Cursor target picker cannot confuse with `Matrix-Control_Standalone`.
- Dev overlay in Standalone: apply pushed layer onto resolution path used by Looks; on disconnect, fall back to compiled Black/Cream — do not widen shipped `ColourVariant` to N product skins as a V1 requirement.
- Compound launch is Cursor tooling only; editor must not spawn the product process.

## Verification

**Commands:**
- `cmake --preset macos-debug-arm64` with Skin Editor option ON — expected: configures Skin Editor + Standalone Dev receiver sources
- `cmake --build --preset macos-debug-arm64 --target <SkinEditorTarget>` and `--target Matrix-Control_Standalone` — expected: both binaries build
- Release / plugin configure with option OFF (or Release preset) — expected: no editor target artefacts; receiver not in AU/VST3/Release Standalone
- `python3 Scripts/quality/lint_touched.py` on touched C++ — expected: clean per project gate
- Unit tests for JSON round-trip / Unbound bind when added under `MATRIX_BUILD_TESTS` — expected: pass

**Manual checks:**
- Compound launch from Cursor starts both apps; Duplicate → edit → push → live paint; export → restart → import → same layer; Unbound bind clears row; BC remains read-only without Duplicate.
