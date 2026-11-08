---
title: 'Skin Editor launch sync for last Import JSON'
type: 'feature'
created: '2026-11-08'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** On relaunch, Skin Editor restores Editor skins from the session file and re-arms the last Import path using the *current* disk fingerprint as baseline. If that JSON changed between sessions, the watch stays quiet while the UI shows stale session skins — a silent desync.

**Approach:** Persist the last accepted disk fingerprint with the Import path. On launch, if the path still exists and the live fingerprint differs from the stored one, show the same Reload / Discard choice as the in-session external-change prompt (do not silently accept disk or wipe session-only edits). Matching or missing legacy fingerprint → arm watch as today.

</frozen-after-approval>

## Implementation Notes

- Agent decisions: store `lastImportedJsonFingerprint` in session JSON next to `lastImportedJsonPath`; update it whenever the watch baseline is accepted (`arm`, fingerprint refresh after Discard/Export, successful Reload). Empty stored fingerprint (old sessions) → treat as match and arm without prompt. Missing file → clear watch path. Launch dialog copy may mention “since last session” while keeping title/buttons ASCII English. Reuse `reloadWatchedImportJsonFromDisk` / Discard paths.
- Code Map (oneshot): `DevSkinSession.h/.cpp`; `SkinEditorMainComponentImportWatch.cpp` (+ header decls); `SkinEditorMainComponentSession.cpp` `loadSessionOrBaseline`.
- Implemented: `reconcileWatchedImportJsonOnLaunch()`; session persist/load of fingerprint; launch dialog title `JSON Out of Sync`; content-only fingerprint (no mtime); flush stale missing-path from session disk; unreadable live read keeps stored baseline without silent accept.
- Commit deferred to human request (project rule: no proactive commit).

## Review Triage Log

- Missing-file clear not persisted — medium, patched: reconcile flushes session when disk still lists a dropped path.
- Unreadable live fingerprint silently arms — medium, patched: keep stored baseline and watch without accepting.
- Launch prompt without 2-poll debounce — medium, deferred: same immediate prompt as intentional launch UX; Dropbox mid-write race rare at cold start.
- Esc/close acts as Discard — medium, deferred: shared AlertWindow result handling; pre-existing live-watch behaviour.
- Launch title/status unfinished — medium, patched: `JSON Out of Sync` + launch Discard status string.
- mtime-only false diverge — medium, patched: fingerprint is size|hash only.
- Autosave on every matching launch arm — low, rejected: harmless rewrite; arm already autosaved for path persistence.
- Reload status ignores deleted skins — low/defer: pre-existing Reload status helper; not launch-specific.
- Failed Reload accepts disk fingerprint — medium, deferred: pre-existing refuse path; would need launch-specific retry policy.
- No unit tests — medium, deferred: same gate as external-json-reload oneshot.

### Review Findings

- [x] [Review][Defer] Esc/close on JSON dialog acts as Discard — deferred: confirmed product choice (Esc/close = Discard / accept disk fingerprint)
- [x] [Review][Patch] Failed Reload must not accept the disk fingerprint [Tools/SkinEditor/SkinEditorMainComponentImportWatch.cpp:reloadWatchedImportJsonFromDisk]
- [x] [Review][Patch] Palette long labels can clip under equal-width cells [Tools/SkinEditor/SkinEditorPalettePanel.cpp:layoutActionButtons]
- [x] [Review][Patch] Session round-trip tests omit lastImportedJsonPath / fingerprint [Tests/Unit/DevSkinLayerJsonTests.cpp:assertSessionRoundTrip]
- [x] [Review][Patch] Natural editor-skin sort helpers have no unit tests [Tools/SkinEditor/SkinEditorUiHelpers.h:compareEditorSkinNamesNatural]
- [x] [Review][Patch] Inverse Palette-from-remainder helper untested [Tools/SkinEditor/SkinEditorChromeTokens.h:palettePanelWidthFromBodyRemainder]
- [x] [Review][Patch] Editor Skins Dup/Ren/Del lack tooltips [Tools/SkinEditor/SkinEditorSkinsPanel.cpp]
- [x] [Review][Patch] Product/Editor nav tooltips are identical [Tools/SkinEditor/SkinEditorSkinsPanel.cpp:wireNav]
- [x] [Review][Defer] Reload status omits removed Editor skins — deferred: known debt; collectChangedEditorSkins only diffs surviving/new skins
- [x] [Review][Defer] Permanently missing watched JSON stays silent — deferred: transient rename ignored by design; permanent-delete UX later
- [x] [Review][Defer] Import-watch / launch-reconcile orchestration untested — deferred: GUI/timer/dialog; close session-field gap first
- [x] [Review][Defer] Frozen Intent still says native modal — deferred: product chose themed AlertWindow; update Intent wording when renegotiating frozen text
- [x] [Review][Defer] Launch prompt has no 2-poll debounce — deferred: intentional cold-start UX (already recorded)
