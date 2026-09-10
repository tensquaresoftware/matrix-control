---
title: 'Computer Patches multi-drop and virtual list'
type: 'feature'
created: '2026-09-10'
status: 'done'
route: 'dispatch'
baseline_commit: 'd578daed4fbe06493c5d0a2692f3ee1d19fa51cc'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/spec-syx-drag-drop-load.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Computer Patches drag-drop only accepts one `.syx`. Dropping a folder, several files, or a Finder mix of folders and files is rejected, so building an on-the-fly browsing list still requires OPEN.

**Approach:** Extend drag-drop so a folder, multi-`.syx` selection, multi-folder selection, or mix builds the Computer Patches combo (valid patches only), loads the first sorted valid patch, and keeps OPEN / SAVE / footer / PATCH NAME behaviour coherent via real-folder mode vs in-session virtual list.

## Boundaries & Constraints

**Always:**
- Drag overlay (no heavy inventory): lightweight accept when selection has any directory and/or any `.syx` path → L1 `PATCHES...` (ASCII dots, 10 chars, fixed, no counter) + blinking L2 `DROP TO LOAD`; single plain `.syx` keeps today’s assess + name preview; otherwise existing BAD FILE overlay.
- Drop single real folder: non-recursive scan (same as OPEN); combo = valid only; load first sorted; PATCH NAME = that patch; remember folder as real `kFolderPath`; footer `formatScanSummary` then Loaded.
- Drop multi files / multi folders / mix: build in-memory virtual list of absolute paths of valid `.syx` only (each selected folder: non-recursive scan; merge with directly selected `.syx`); combo = that list only (not parent folder full scan); load first valid; PATCH NAME same; do not treat any parent as the virtual “folder” for OPEN.
- Combo order: same open-list sort as `scanFolder` (basename via `MutationNaming::compareOpenListFileNames`); equal basenames → stable tie-break by full path.
- Homonyms: show identical display names; no artificial folder suffixes; combo item IDs stay unique (1-based index).
- OPEN uses last real folder only: last OPEN, successful single-`.syx` drop (parent), or successful single-folder drop — never virtual-list parents, never Desktop default.
- Reload / new session: virtual list gone; fall back to last real folder.
- SAVE writes the current patch’s origin file; Save As uses real disk location, preferring last real folder when relevant.
- Footer: reuse `formatScanSummary`, Loaded/Saved/reconciliation, `Drop rejected: …`; enrich Loaded/Saved with a readable location (prefer useful end: folder + file); ASCII `...` only; retire/adapt `kDropRejectedMultiFile` (no longer “one .syx at a time”).

**Never:**
- Persist virtual list across sessions; recursive subfolder scan; homonym decorations; PATCH NAME height/typo changes beyond overlay needs; Bank Utility / bank-dump drop; parallel loader bypassing Computer Patches gates; heavy per-`fileDragMove` validation of thousands of files.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Drag single valid `.syx` | One patch file | Existing overlay (name + `DROP TO LOAD`) | N/A |
| Drag folder / multi / mix with ≥1 dir or `.syx` | Finder selection | `PATCHES...` + `DROP TO LOAD` blink; no deep scan | N/A |
| Drag junk only | No dir / no `.syx` | `********` + `BAD FILE` | N/A |
| Drop one folder (valid files) | Non-recursive `.syx` | Real folder mode; combo sorted valids; load first; `kFolderPath` = folder; scan + Loaded footers | Empty / all invalid → existing reject / empty footers; no load |
| Drop multi `.syx` / folders / mix | Valid after merge | Virtual list mode; combo = merged valids only; load first; `kFolderPath` unchanged | Zero valid → drop reject (existing family); no virtual list install |
| Drop single `.syx` | Valid file | Existing parent-folder scan + select that file; real folder remembered | Existing reject kinds |
| OPEN after virtual list | Virtual list active | Opens last real folder (not list parents); replaces list with that folder scan | Missing folder → existing folder-not-found |
| Prev/Next / combo in virtual list | Absolute paths in session | Navigate list; load origin files; SAVE to current origin | Stale/missing file → existing load failure footers |
| Homonyms in virtual list | Same basename, different dirs | Identical combo / PATCH NAME labels; distinct indices | N/A |
| Session reload | Had virtual list | List cleared; rescan last real folder if any | N/A |

</frozen-after-approval>

## Code Map

- `Source/GUI/PluginEditorFileDragDrop.cpp` — Replace `files.size() != 1` drag BAD FILE / drop multi reject; classify lightweight accept vs single-file assess; dispatch drop to folder / multi / single entry points; keep no heavy move validation.
- `Source/GUI/Panels/.../PatchNameDisplayPanel.cpp` — `applyDragOverlay`: support fixed primary `PATCHES...` for multi/folder accept (do not write overlay into APVTS).
- `Source/Shared/Definitions/PluginDisplayNames.h` — Add `DragDropOverlay` / Computer Patches `PATCHES...`; adapt/remove `kDropRejectedMultiFile`; enrich `formatLoadSuccess` / `formatSaveSuccess` (readable location); keep `formatScanSummary`.
- `Source/GUI/Helpers/TextFitHelpers.h` + `Source/GUI/Panels/.../FooterPanel.cpp` — Prefer path-style / end-useful truncation for Loaded/Saved location (today footer keeps start, cuts end).
- `Source/Core/Services/PatchFileService.{h,cpp}` — Reuse `assessSinglePatchSyxFile`, non-recursive `scanFolder` / `findSyxFiles`, `MutationNaming::compareOpenListFileNames`; add merge/sort of absolute valid paths + invalid counts for multi selection; extend last-scan cache so combo can bind to absolute files (virtual) without inventing a fake folder.
- `Source/Core/Actions/PatchManagerActionHandlerComputerBrowser.cpp` — Fork `prepareDroppedComputerPatchSelection` / new multi+folder drop prepare: real folder vs virtual list; `fileAtComputerPatchesIndex` must resolve absolute path in virtual mode; OPEN/`kFolderPath` only on real-folder outcomes; cancel restore must restore mode + list/folder.
- `Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp` — SAVE/Save As / `publishLoadFooters` use origin file + enriched location; do not write into a virtual “folder”.
- `Source/GUI/Panels/.../ComputerPatchesPanel.cpp` — Combo still 1-based ids; display stems (homonyms allowed); rebuild from scan revision whether folder or virtual.
- `Source/Core/PluginProcessorGates.cpp` / `PluginProcessor.h` — Widen drop API from single `File` to selection list if needed; keep dirty/Mutator gates.
- `Tests/Unit/PatchManagerActionHandlerDropLoadTests.cpp`, `PatchFileServiceTests.cpp` (+ assess tests if needed) — Cover folder drop, multi merge, virtual vs `kFolderPath`, sort+path tie-break, zero-valid reject; GUI blink remains manual.
- Do **not** change: SysEx single-patch contract, Bank Import/Export orchestration, recursive scans, Core→GUI dependency direction.

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/PatchFileService.{h,cpp}` — Add selection merge (non-recursive per folder + direct files), absolute-path sort with basename comparator + path tie-break, publish into last-scan virtual or folder mode — SSOT for combo list
- [x] `Source/Core/Actions/PatchManagerActionHandler*.{h,cpp}` (+ Computer browser/load-save) — Drop prepare for folder / multi / single; virtual vs real `kFolderPath`; resolve load/save/Prev/Next by absolute path in virtual mode; OPEN ignores virtual parents; cancel restore both modes — product behaviour
- [x] `Source/Shared/Definitions/PluginDisplayNames.h` — `PATCHES...`; retire multi-one-file reject copy; Loaded/Saved location formatting — English UI SSOT
- [x] `Source/GUI/PluginEditorFileDragDrop.cpp` + `PatchNameDisplayPanel.*` — Lightweight drag accept + `PATCHES...` overlay; drop dispatch without multi hard-reject — entry point
- [x] `Source/GUI/Panels/.../FooterPanel.cpp` (+ `TextFitHelpers` if needed) — End-useful / path-aware truncation for enriched Loaded/Saved — readable footer
- [x] `Source/GUI/Panels/.../ComputerPatchesPanel.cpp` — Homonym-safe display from stems; ids = indices — combo contract
- [x] `Tests/Unit/PatchManagerActionHandlerDropLoadTests.cpp` + `PatchFileServiceTests.cpp` — Lock I/O matrix Core cases (folder, multi, mix, empty, OPEN after virtual, sort tie-break) — regression net
- [x] `CMakeLists.txt` / `Tests/CMakeLists.txt` — Register new sources if split — green build

**Acceptance Criteria:**
- Given a folder of `.syx` dropped on the editor, when drop completes, then Computer Patches shows only valid patches, loads the first sorted, remembers that folder for OPEN, and shows scan + Loaded footers.
- Given multiple `.syx` and/or folders dropped together, when drop completes, then the combo is exactly the merged valid absolute paths (sorted), the first loads, and OPEN still opens the previous real folder unchanged by the virtual list.
- Given a virtual list with two same-named files from different folders, when browsing the combo, then both labels match the real filename stem and each entry loads its own file.
- Given drag of a multi/folder selection that includes a directory or `.syx`, when hovering, then PATCH NAME shows `PATCHES...` / `DROP TO LOAD` without scanning the whole tree on every move.
- Given SAVE on a virtually listed patch, when save succeeds, then the origin file is overwritten and the footer reports a readable location (not a raw untruncated macOS absolute path).

## Implementation Notes

- Added Core coverage for session rescan clearing virtual list, Prev/Next on absolute virtual paths, and SAVE overwrite + readable footer while virtual mode stays active.
- Drag overlay rows remain manual (GUI); Core matrix rows covered by DropLoad + PatchFileService tests (0 failures).
- Review patches: virtual SAVE rewrites absolute list paths and selects by full path only; cancel restore republishes scan footer; tests for virtual gate-cancel, empty-folder drop, and post-SAVE path identity.

## Spec Change Log

## Review Triage Log

| Finding | Verdict | Evidence |
|---------|---------|----------|
| Virtual SAVE leaves stale absolute paths after case-fold rename | high | Confirmed: `rescanAndSelectSavedFile` skips folder rescan in virtual mode; `sortedValidFiles` keep pre-save paths while `knownSyxFullPath_` updates — later load/Prev/Next can miss the file on case-sensitive volumes. |
| Virtual basename fallback can select wrong homonym after path miss | medium | Confirmed: `selectSavedFileInCurrentScan` falls back to `indexOfFileNameIgnoreCase` when path miss; shared root with stale-path SAVE. |
| Edge: post-save no longer refreshes virtual browser list (deletion) | high | Same defect as stale virtual SAVE paths. |
| Cancel restore virtual skips `propagateScanResult` | low | Confirmed: `installVirtualFileList` + `bumpScanRevision` only; footer may stay stale vs folder restore via `scanAndPublishFolder`. |
| Edge claim: cancel restore misses virtual footer parity | low | Same as missing `propagateScanResult` on virtual restore. |
| No gate-cancel test for virtual snapshot restore | medium | Verification-gap pre-verified: cancel tests only cover folder-mode single `.syx`. |
| Empty/all-invalid single-folder drop untested | medium | Verification-gap pre-verified: only multi zero-valid covered. |
| Empty-folder drop messaging asymmetric vs multi reject | false | Spec/matrix require OPEN-like empty footers and keeping the folder for single-folder drop; multi zero-valid uses `kDropRejectedNoValid` + prior restore by design. |
| ComputerPatchesPanel marked done but absent from diff | false | Combo already binds 1-based ids to `sortedValidFileNames` stems; virtual mode publishes the same names — no panel change required. |
| Code Map TextFitHelpers unused; only FooterPanel flip | false | Spec allowed FooterPanel path-style truncate; `middle=true` for Loaded/Saved is the intended implementation. |
| Unreadable dirs skipped without invalidCount | low | Real but rare; rejected (everyday use unlikely; richer messaging adds complexity beyond a direct fix). |
| `formatReadablePatchLocation` lacks root/deep-path unit tests | low | Rejected — cosmetic verification gap unlikely in everyday use. |
| Drag accept heuristic duplicated Editor vs Core | maybe-false | Drift possible but no demonstrated user-facing defect; defer as tracking if needed — routed with manual drag defer. |
| CMake indentation under MATRIX_BUILD_TESTS | low | Rejected — harmless to CMake. |
| deferred-work.md still mentions kDropRejectedMultiFile | false | Tracking hygiene, not a product defect from this change. |
| Spec Change Log / Review Triage empty mid-review | false | Process state during review; logs filled here. |
| Blank-line-only diff noise | low | Rejected — no behavioral impact. |
| Drag overlay accept/reject has no automated check | medium | Verification-gap disposition `defer` — matches spec manual GUI note. |

## Design Notes

**Modes:** Real folder = today’s `kFolderPath` + `scan.folder` + basenames. Virtual list = session absolute `File` array in the scan cache (or sibling field) with a clear mode flag; combo ids remain 1-based indices; `fileAtComputerPatchesIndex` returns that `File` directly.

**Drag heuristic (agent):** Accept overlay if any path `isDirectory()` or extension equals `.syx` (case-insensitive). Do not call `assessSinglePatchSyxFile` on every child during move. Single non-directory `.syx` keeps full assess + name preview. Drop performs validation/merge once.

**Sort:** Reuse `MutationNaming::compareOpenListFileNames` on basename (same as `scanFolder`, including INITIAL / mutator ordering). Tie-break equal basenames with case-insensitive full path so order is stable.

**Footer location:** Prefer `Loaded <leafFolder>/<file.syx>` (and same for Saved). If paint width still clips, use path-style middle truncate (`TextFitHelpers` `middle=true`) for these success lines so folder+file stay readable. ASCII `...` only.

**Cancel / dirty gates:** Keep `confirmPatchContextChange` and browser snapshot restore; snapshot must include virtual vs folder mode and list contents when virtual.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --test-dir Builds/macOS/...` (or project unit-test target with `MATRIX_BUILD_TESTS=ON`) focusing `PatchManagerActionHandlerDropLoad` / `PatchFileService` -- expected: new + existing cases pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Finder: drop one folder; multi `.syx`; multi folders; mix files+folders; junk-only drag → BAD FILE; OPEN after virtual list; SAVE / Save As; homonym labels; footer Loaded/Saved readability.
