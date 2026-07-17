---
title: 'Computer Patches — auto-select first after Open; no startup rescan'
type: 'feature'
created: '2026-07-17'
status: 'done'
baseline_commit: '1d8d394fe116770381d9878c03a9ae73a1679844'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** After Open picks a folder with valid `.syx` files, Computer Patches shows `<SELECT>` with Previous/Next disabled, forcing a second manual combo pick. Separately, restarting the plugin rescans the last folder and repopulates the combo, which is unwanted until the user explicitly Opens again.

**Approach:** On a successful Open scan with at least one valid file, select and load the first sorted valid patch (combo id `1`) through the existing load pipeline. Keep persisting `kFolderPath` across sessions only as the Open dialog’s starting directory (with fallback if missing). Do not rescan or populate the Computer Patches combo at plugin startup / host state restore — combo and Previous/Next stay grayed (`<EMPTY!>`) until the user Opens.

## Boundaries & Constraints

**Always:**
- Reuse `handleLoadSelectedPatchFile` (same decode / reconcile / mutator history gate / SysEx path as manual combo load).
- First file = index `0` of `sortedValidFileNames` → APVTS id `1`.
- If Open leaves selection already at `1`, still load (JUCE `setProperty` no-op when unchanged — mirror prev/next N==1 pattern).
- Empty / unusable / `validCount == 0` after Open → no auto-load; `<EMPTY!>` + existing scan footer.
- Persist `kFolderPath` across sessions; Open’s folder picker starts in that directory when it still exists, otherwise OS default / empty start.
- On processor construction and `setStateInformation` restore: do **not** call startup folder rescan; leave scan cache empty / unpublished so the combo shows `<EMPTY!>`, is disabled, and Previous/Next stay disabled. Selection must not appear populated from a prior session’s file list.

**Ask First:**
- Changing sentinel strings or removing `<SELECT>` from the codebase entirely (beyond “not shown after a successful Open with valid files”).
- Deleting `rescanPersistedComputerPatchesFolder` entirely if tests still need it as an explicit helper (prefer stop calling it from production startup paths; keep helper if useful for unit tests).

**Never:**
- DirtyPatchTracker / FR-51 unsaved-edit modal (Epic 9 — deferred; Cancel / Continue decision already recorded in `deferred-work.md`).
- New save UI inside Open.
- Changing Prev/Next circular navigation rules.
- Auto-load when scan fails or yields zero valid files.
- Auto-scan or auto-load of Computer Patches files at plugin launch or DAW state restore.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Happy Open | Folder with ≥1 valid `.syx` | `kSelectPatchFile == 1`; first file loaded; combo shows stem; Prev/Next enabled | Load failures use existing footers; selection may still be `1` |
| Empty / invalid folder | Unusable path or `validCount == 0` | No load; `<EMPTY!>`; scan footer as today | N/A |
| Re-Open same folder while id already `1` | Prior selection was first file | Still runs load once (direct call if property unchanged) | Same as manual re-select |
| Open cancelled | Folder picker returns non-directory | Folder path + selection unchanged; no scan bump | N/A |
| Plugin / host session start | Persisted `kFolderPath` present | Path kept in state; **no** rescan; combo `<EMPTY!>` grayed; Prev/Next disabled; no file list | N/A |
| Open after restart | Persisted path still a directory | Folder picker opens at that path; after confirm, scan + auto-load first as Happy Open | If path missing, picker falls back to default start location |

</frozen-after-approval>

## Code Map

- `Source/Core/PluginProcessor.cpp` -- remove startup rescan; call `resetComputerPatchesBrowserAfterSessionLoad` from `setStateInformation`
- `Source/Core/Actions/PatchManagerActionHandler.cpp` -- Open post-scan select+load; `resetComputerPatchesBrowserAfterSessionLoad`
- `Source/Core/Actions/PatchManagerActionHandler.h` -- Open takes limits; public session-load reset
- `Source/GUI/PluginEditor.cpp` -- Open `FileChooser` initial directory from persisted `kFolderPath` when usable
- `Source/GUI/Panels/.../ComputerPatchesPanel.cpp` -- unchanged; EmptySentinel via empty scan + `kScanRevision` / `valueTreeRedirected`
- `Tests/Unit/PatchManagerActionHandlerTests.cpp` -- Open auto-load / empty / reload-first; session-load reset replaces startup-rescan AC
## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/PluginProcessor.cpp` -- Stop calling `rescanPersistedComputerPatchesFolder` from constructor and `setStateInformation` -- startup must not populate Computer Patches list
- [x] `Source/Core/Actions/PatchManagerActionHandler.{h,cpp}` -- After successful Open scan with `validCount > 0`, set `kSelectPatchFile` to `1` (or call `handleLoadSelectedPatchFile` when already `1`) so load runs once -- auto-select + auto-load
- [x] `Source/GUI/PluginEditor.cpp` -- Seed Open folder chooser with persisted `kFolderPath` when that path is still a directory; otherwise default start -- session path reuse without scanning
- [x] `Tests/Unit/PatchManagerActionHandlerTests.cpp` -- Assert Open auto-load / empty no-load / already-id-`1` reload; update or replace `rescanPersistedFolder_scansOnStartup` expectations for the new “no startup rescan” contract; keep path persistence round-trip -- locks I/O matrix
- [x] `Source/GUI/.../ComputerPatchesPanel.cpp` -- Only if needed: idle/refresh stays `<EMPTY!>` + disabled nav until an Open-driven scan -- no phantom list from prior session

**Acceptance Criteria:**
- Given a folder with valid patches, when the user completes Open, then the first sorted valid file is selected and loaded, the combo shows that name (not `<SELECT>`), and Previous/Next are enabled.
- Given an empty or unusable folder, when Open completes, then nothing is loaded, `<EMPTY!>` remains, and Previous/Next stay disabled.
- Given Open is cancelled in the folder picker, when the action returns, then prior folder path and selection are unchanged.
- Given a previous session stored `kFolderPath`, when the plugin starts or host state is restored, then the path remains persisted, the Computer Patches combo shows grayed `<EMPTY!>`, Previous/Next are disabled, and no patch list is shown until the user Opens.
- Given a usable persisted folder path, when the user clicks Open, then the folder picker starts in that directory (fallback to default if missing); after the user confirms a folder, scan + first-file auto-load rules apply as above.

## Spec Change Log

## Design Notes

Prefer handler-owned post-scan selection over rewriting `applySelectSentinel`. Setting `kSelectPatchFile` to `1` after the scan cache is current reuses PluginProcessor dispatch into `handleLoadSelectedPatchFile`. When the property is already `1`, call load directly (same as Story 4.6 single-file prev/next).

Startup today calls `rescanPersistedComputerPatchesFolder` from `PluginProcessor` ctor and `setStateInformation` (Story 4.3). Remove those production calls so `PatchFileService` has no published scan until Open. Today’s Open `FileChooser` starts at `juce::File()` — wire initial directory from `kFolderPath` when `isDirectory()`.

Unit tests that primed a scan via `rescanPersistedComputerPatchesFolder()` may keep calling that helper explicitly; only the “auto on startup” production contract changes.

Mutator history clear on load continues via `onPatchLoaded`. FR-51 dirty-patch Cancel/Continue before Open remains Epic 9.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- build succeeds
- `ctest --preset macos-debug-arm64 -R PatchManagerActionHandler` (or equivalent project test filter) -- updated Open / persistence tests pass

**Manual checks (if no CLI):**
- Restart plugin after having opened a folder → combo `<EMPTY!>` grayed, Prev/Next grayed, path still remembered for Open dialog; Open → lands in prior folder → confirm → first patch loads with nav enabled.

## Suggested Review Order

**Open → auto-select first**

- After scan, set id `1` and load via existing path (or direct load if already `1`).
  [`PatchManagerActionHandler.cpp:362`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L362)

- Empty / unusable folder clears selection to `0` (no auto-load).
  [`PatchManagerActionHandler.cpp:378`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L378)

**No startup rescan; path-only persistence**

- Constructor no longer rescans the persisted folder.
  [`PluginProcessor.cpp:350`](../../Source/Core/PluginProcessor.cpp#L350)

- Clear in-memory scan before `replaceState` to avoid a stale list flash.
  [`PluginProcessor.cpp:628`](../../Source/Core/PluginProcessor.cpp#L628)

- After restore: clear selection, empty browser UI, keep `kFolderPath`.
  [`PatchManagerActionHandler.cpp:400`](../../Source/Core/Actions/PatchManagerActionHandler.cpp#L400)

- Open dialog starts in the persisted folder when it still exists.
  [`PluginEditor.cpp:81`](../../Source/GUI/PluginEditor.cpp#L81)

**Tests**

- Open auto-load, empty folder, reload-when-already-first, session reset.
  [`PatchManagerActionHandlerTests.cpp:771`](../../Tests/Unit/PatchManagerActionHandlerTests.cpp#L771)
