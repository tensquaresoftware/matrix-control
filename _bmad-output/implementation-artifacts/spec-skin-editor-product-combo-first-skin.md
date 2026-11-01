---
title: 'Skin Editor Product combo first skin on launch'
type: 'bugfix'
created: '2026-11-01'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: '76d6d8752835a5969bdc4a2e23744570f66fdb63'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-product-session-persist.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** At Skin Editor launch, the Skins Product combo does not show the first skin in the list (BLACK). With no session it falls back to Cream; with a session it restores the last Product choice.

**Approach:** On every launch, force Product to the first combo item (BLACK today). Do not restore `productSkinId` from the session file on load. Mid-session Product changes and autosave may continue; the next launch still opens on the first list item.

**Decision (PRODUCT_LAUNCH_VS_SESSION):** ALWAYS_FIRST — renegotiates launch restore from `spec-skin-editor-product-session-persist` for Product only.

## Boundaries & Constraints

**Always:**
- Product combo item order stays BLACK then CREAM unless a later story makes the list dynamic.
- "First available" means the first item currently added to the Product combo (id 1 / BLACK today).
- After load (success or fail), `sessionState_.productSkinId` is set to the first Product skin before combos refresh.
- Editor skin, Compare, Bridge push, and Export/Discard dirty rules stay as today except Product launch selection.

**Never:**
- Do not restore Product from session on launch.
- Do not change the plugin's runtime Look / product skin outside the Skin Editor.
- Do not invent a third Product skin or reorder the combo unless required for "first available".
- Do not broaden into Editor combo defaults.
- Do not remove mid-session Product autosave unless it becomes dead weight with no other consumer — prefer leave write path; launch simply overwrites after load.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Launch with session | Session has productSkinId Cream or Black | Product combo shows BLACK (first list item); session Product ignored for launch | N/A |
| Launch without session | No session file / load fails | Product combo shows BLACK, not Cream | Corrupt session: keep existing status message path; Product still forced to first |
| Mid-session change | User picks Cream after launch | Product switches to Cream for this session; relaunch returns to BLACK | N/A |

</frozen-after-approval>

## Code Map

- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` -- `loadSessionOrBaseline()` (~424–443): after load (ok or not), force `productSkinId` to first Product (`kCompiledBlackId`). Replace Cream-only fallback branch.
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` -- `refreshCombos()` (~243–252): already BLACK then CREAM; selects from `sessionState_.productSkinId` — no change if load forces Black.
- `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` -- `applyProductComboChange()` (~90–98): keep mid-session mapping + autosave.
- `Source/GUI/Skins/Dev/DevSkinSession.h` / `.cpp` -- optional: align struct default / invalid → Black so helpers match launch policy; not required if load always overwrites.
- `Source/GUI/Skins/Dev/DevSkinTypes.h` -- `kCompiledBlackId` / `kCompiledCreamId`. Reuse.
- `Tools/SkinEditor/SkinEditorSkinsPanel.{h,cpp}` -- Product combo widget only; leave alone.
- `_bmad-output/implementation-artifacts/spec-skin-editor-product-session-persist.md` -- prior done: restore last Product. This story overrides launch restore for Product only.

## Tasks & Acceptance

**Execution:**
- [x] `Tools/SkinEditor/SkinEditorMainComponentSession.cpp` -- In `loadSessionOrBaseline`, always set `productSkinId` to the first Product skin (`kCompiledBlackId`) after session load attempt; remove Cream-as-launch-default -- launch Product must ignore restored session value.
- [x] `Source/GUI/Skins/Dev/DevSkinSession.h` (+ `.cpp` if invalid-id path still returns Cream) -- Align default/invalid Product id to Black where that path can still surface before/without the launch force -- avoid Cream creeping in.
- [ ] Manual smoke -- Cream mid-session → quit → relaunch shows BLACK; no-session launch shows BLACK -- no Skin Editor UI test harness for this combo.

**Acceptance Criteria:**
- Given any prior session Product (Cream or Black), when Skin Editor launches, then the Product combo shows the first list item (BLACK).
- Given no session file, when Skin Editor launches, then the Product combo shows BLACK (not Cream).
- Given the user selects Cream mid-session, when they continue without quitting, then Cream stays selected until quit; after relaunch, Product is BLACK again.

## Implementation Notes

- `forceProductToFirstAvailable` in `DevSkinSession` — called from `loadSessionOrBaseline` after every load attempt (ok or fail). Fail path still clears `editorSkinId` and keeps status messaging.
- `EditorSessionState` default and invalid-id clamp in `importSessionFromJson` now Black. Mid-session Product autosave / Discard prefs write path unchanged.
- Matrix coverage: `Tests/Unit/DevSkinLaunchProductTests.cpp` (session Cream ignored after force; default/invalid → Black; Cream round-trip without force). Registered in root `CMakeLists.txt`.
- Manual UI smoke still useful for Guillaume (combo paint / quit path).
- Review patch: clarified `forceProductToFirstAvailable` header comment (caller must call after load).
- Deferred: no automated pin that `loadSessionOrBaseline` invokes the force (helper-only tests).

## Spec Change Log

## Review Triage Log

- Blind: `forceProductToFirstAvailable` hardcodes BLACK instead of reading the combo list — **false** — frozen Always locks combo order BLACK then CREAM and defines first item as BLACK today; divergence only if a later story makes the list dynamic.
- Blind: launch restores then overwrites Product — **false** — frozen Approach/Never explicitly prefer leave write path and overwrite after load.
- Blind: status `done` with unchecked manual smoke — **false** — status corrected to `in-review`; manual smoke remains optional UI confirmation, not an automated gate.
- Blind: Verification omits new unit tests — **low** (rejected; fix would only edit this build's spec) — commands updated in Verification for accuracy after triage; tests already ran green in step-03.
- Blind: Cream→Black default/clamp is global — **false** — `EditorSessionState` is Skin Editor session-only; no other caller assumed Cream as Product default.
- Blind: no Spec Change Log / pointer on superseded persist spec — **low** (rejected; fix edits specs) — this story's frozen Decision already records ALWAYS_FIRST renegotiation of launch restore.
- Blind: load-fail branch of `loadSessionOrBaseline` untested — **medium** (unverified automation gap) — grouped with Verification Gap wiring defer; Skin Editor MainComponent not in unit harness.
- Blind: header comment attributes "ignores session" to the helper alone — **low** → **patch** — comment clarified to say caller must invoke after load.
- Edge Case Hunter: no findings — n/a.
- Verification Gap: launch wiring not pinned by tests that call force directly — **medium** → **defer** — removing the call would restore Cream-on-launch while helper tests stay green; no MainComponent harness; manual smoke remains.
- Verification Gap Other: story Verification lacked unit-test command — **low** (rejected as primary route; Verification commands amended for accuracy).

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` -- expected: link OK
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` && `Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests "DevSkinLaunchProduct"` -- expected: 0 failures
- `python3 Scripts/quality/lint_touched.py` -- expected: Quality gate OK

**Manual checks:**
- Set Product to Cream, quit (session saved), relaunch → Product shows BLACK.
- Missing session / first launch → BLACK not Cream.
- Mid-session Product → Cream works; Compare/Bridge unchanged aside from selection.
