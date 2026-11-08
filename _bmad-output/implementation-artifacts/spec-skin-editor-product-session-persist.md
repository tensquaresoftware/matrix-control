---
title: 'Skin Editor Product session persist across Discard'
type: 'bugfix'
created: '2026-10-30'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-skin-editor-ui-panels.md'
  - '{project-root}/_bmad/custom/ascii-display-strings.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Quitting the Skin Editor after choosing Product Black (especially via Discard on the unsaved modal, or after Product-only changes that incorrectly mark the session dirty) deletes or skips the session file, so the next launch falls back to Cream.

**Approach:** Keep one session store (`skin-editor-session.json`). Split “Editor dirty → Export/Discard/Cancel modal” from “session preference needs autosave” (Product, and other non-export prefs). Discard abandons unexported Editor skins but rewrites the session with the current Product preference kept; Product-only changes never open the quit modal; flush pending autosave on quit so a last Product choice is not lost inside the debounce window.

</frozen-after-approval>

## Implementation Notes

Agent decisions (oneshot):
- Quit gate uses `editorDirty_` only (renamed from `sessionDirty_`). Layer `onChanged` marks editor dirty + schedules autosave.
- Product combo + Editor combo: schedule autosave only (Editor combo suppresses `onChanged` around `selectSkin` so selection is not treated as edits).
- Compare toggle: runtime-only (`compareOn` never persisted) — no editor dirty and no autosave schedule.
- Discard: no `deleteFile()`. Preserve `productSkinId` + `lastSkinJsonDirectory`, `resetToCompiledBaseline()`, clear `editorSkinId`, rewrite via `saveSessionFile`. On rewrite failure: restore layer from disk, keep editor dirty, cancel quit. On success: `pushVisible()` if Bridge connected.
- `flushPendingSessionAutosave()` on quit gate entry and in destructor; `runWithoutLayerChangeNotify` shared helper for load / Discard / Editor combo.
- Cream remains default only when there is truly no session file.
- Touched: `SkinEditorMainComponent.{h,cpp}`, `SkinEditorMainComponentSession.cpp`, `SkinEditorMainComponentActions.cpp`. No `DevSkinSession` schema change.
- Commit skipped (user asked commit/PR only on explicit request).

## Review Triage Log

- Blind Hunter: loadSessionOrBaseline arms onChanged timer — **high**, patched (suppress + stopTimer).
- Blind Hunter: Discard succeeds even if rewrite fails — **high**, patched (bool + restore + cancel quit).
- Blind Hunter: Discard omits pushVisible when connected — **medium**, patched.
- Blind Hunter: Compare should schedule autosave per early notes — **false** (compareOn not persisted; notes corrected).
- Blind Hunter: oneshot spec lacks ACs — **false** (oneshot form).
- Blind Hunter: no automated test for dirty split — **defer** (manual smoke sufficient for this bugfix).
- Blind Hunter: duplicated onChanged nulling / early-return risk — **low**, patched (shared helper).
- Blind Hunter: destructor stopTimer without flush — **medium**, patched.
- Blind Hunter: corrupt-session relies on accidental timer — **defer** (pre-existing; load suppress removes accidental rewrite; explicit rewrite not in scope).
- Blind Hunter: unused public isEditorDirty — **low**, patched (removed).

## Spec Change Log

## Design Notes

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_SkinEditor` — expected: link OK
- `python3 Scripts/quality/lint_touched.py` — expected: Quality gate OK

**Manual checks:**
- Product → Black → wait ~500 ms or quit immediately → quit with no Editor edits → no Unsaved modal → relaunch → Black
- Edit Editor skin → quit → Discard → relaunch → Product still Black/Cream as left; Editor skins cleared to baseline
- First launch / missing session file → Cream
