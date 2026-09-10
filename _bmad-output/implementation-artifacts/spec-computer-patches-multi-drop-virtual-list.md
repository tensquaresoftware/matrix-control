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
- Drag overlay (no heavy inventory): lightweight accept when selection has any directory and/or any `.syx` path → L1 `PATCHES...` (ASCII dots, 10 chars, fixed, no counter) + blinking L2 `DROP TO LOAD`; single plain `.syx` keeps today’s assess + name preview; otherwise junk overlay — singular `BAD FILE` for one unloadable item, plural `BAD FILES` for two or more.
- Drop single real folder: non-recursive scan (same as OPEN); combo = valid only; load first sorted; PATCH NAME = that patch; remember folder as real `kFolderPath`; first-load footer combines scan tally + Loaded (`formatFirstLoadAfterScanMessage`); later in-list loads stay Loaded only (see `spec-computer-patches-footer-first-load.md`).
- Drop multi files / multi folders / mix: build in-memory virtual list of absolute paths of valid `.syx` only (each selected folder: non-recursive scan; merge with directly selected `.syx`); combo = that list only (not parent folder full scan); load first valid; PATCH NAME same; do not treat any parent as the virtual “folder” for OPEN.
- Combo order: same open-list sort as `scanFolder` (basename via `MutationNaming::compareOpenListFileNames`); equal basenames → stable tie-break by full path.
- Homonyms: show identical display names; no artificial folder suffixes; combo item IDs stay unique (1-based index).
- OPEN uses last real folder only: last OPEN, successful single-`.syx` drop (parent), or successful single-folder drop — never virtual-list parents, never Desktop default.
- Reload / new session: virtual list gone; fall back to last real folder.
- SAVE writes the current computer-file origin path (session origin, not “whatever the combo row still shows”); Save As uses a real disk location, preferring last real folder when relevant.
- Virtual list + Save As (input vs output): Save As must **not** mutate the drop-built virtual list or select the export as a list row. If the export path is not already a list entry, clear combo selection (`<SELECT>`, same as OPEN + Save As outside the opened folder). Update origin path to the Save As file so the next SAVE overwrites that export until the user re-enters list browsing (choose a combo row, then Prev/Next or COMPUTER PATCHES title reload). Plain SAVE on a list origin may still rewrite that list entry’s absolute path when the on-disk identity of the same slot changes.
- Footer: scan-only / empty paths still use `formatScanSummary` (and existing empty/reject strings); first successful load after OPEN/drop uses combined `formatFirstLoadAfterScanMessage`; later loads use Loaded/Saved/reconciliation; enrich Loaded/Saved with a readable location (prefer useful end: folder + file); ASCII `...` only; retire/adapt `kDropRejectedMultiFile` (no longer “one .syx at a time”).

**Never:**
- Persist virtual list across sessions; recursive subfolder scan; homonym decorations; PATCH NAME height/typo changes beyond overlay needs; Bank Utility / bank-dump drop; parallel loader bypassing Computer Patches gates; heavy per-`fileDragMove` validation of thousands of files; treat Save As as a browsing/list mutation while a virtual list is active.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| Drag single valid `.syx` | One patch file | Existing overlay (name + `DROP TO LOAD`) | N/A |
| Drag folder / multi / mix with ≥1 dir or `.syx` | Finder selection | `PATCHES...` + `DROP TO LOAD` blink; no deep scan | N/A |
| Drag junk only (1 item) | No dir / no `.syx` | `********` + `BAD FILE` | N/A |
| Drag junk only (≥2 items) | No dir / no `.syx` | `********` + `BAD FILES` | N/A |
| Drop one folder (valid files) | Non-recursive `.syx` | Real folder mode; combo sorted valids; load first; `kFolderPath` = folder; combined first-load footer (scan tally + Loaded) | Empty / all invalid → existing reject / empty footers; no load |
| Drop multi `.syx` / folders / mix | Valid after merge | Virtual list mode; combo = merged valids only; load first; `kFolderPath` unchanged | Zero valid → drop reject (existing family); no virtual list install |
| Drop single `.syx` | Valid file | Existing parent-folder scan + select that file; real folder remembered | Existing reject kinds |
| OPEN after virtual list | Virtual list active | Opens last real folder (not list parents); replaces list with that folder scan | Missing folder → existing folder-not-found |
| Prev/Next / combo in virtual list | Absolute paths in session | Navigate list; load origin files; SAVE to current origin | Stale/missing file → existing load failure footers |
| Save As while virtual list active | Export path not in list | Write export; footer Saved at export path; origin → export path; **list unchanged**; combo → `<SELECT>` (selection 0) | Picker cancel → no write / no origin change |
| SAVE after that Save As (no list re-entry) | Origin = export path | Overwrites export path; list still unchanged; selection stays 0 | Existing save failures |
| Combo / title reload / Prev/Next after Save As | Re-enter list browsing | Choose a list row (then Prev/Next or title reload as usual); origin returns to that list path; later SAVE follows list again | Existing load / dirty gates |
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
- Given a folder of `.syx` dropped on the editor, when drop completes, then Computer Patches shows only valid patches, loads the first sorted, remembers that folder for OPEN, and shows a combined first-load footer (scan tally + Loaded).
- Given multiple `.syx` and/or folders dropped together, when drop completes, then the combo is exactly the merged valid absolute paths (sorted), the first loads, and OPEN still opens the previous real folder unchanged by the virtual list.
- Given a virtual list with two same-named files from different folders, when browsing the combo, then both labels match the real filename stem and each entry loads its own file.
- Given drag of a multi/folder selection that includes a directory or `.syx`, when hovering, then PATCH NAME shows `PATCHES...` / `DROP TO LOAD` without scanning the whole tree on every move.
- Given SAVE on a virtually listed patch, when save succeeds, then the origin file is overwritten and the footer reports a readable location (not a raw untruncated macOS absolute path).
- Given junk-only drag of two or more unloadable items, when hovering, then PATCH NAME shows `BAD FILES` (singular `BAD FILE` when exactly one).
- Given Save As while a virtual list is active to a path not in that list, when export succeeds, then the drop list stays as built, the combo shows `<SELECT>`, the footer shows the export path, and the next SAVE overwrites that export until the user chooses a list row again (then Prev/Next or title reload as usual).

## Implementation Notes

- Frozen footer wording renegotiated (2026-09-10 code review of `spec-computer-patches-footer-first-load.md`): first successful OPEN/drop load uses combined scan+Loaded; scan-only `formatScanSummary` remains for empty/unusable paths only.
- Added Core coverage for session rescan clearing virtual list, Prev/Next on absolute virtual paths, and SAVE overwrite + readable footer while virtual mode stays active.
- Drag overlay rows remain manual (GUI); Core matrix rows covered by DropLoad + PatchFileService tests (0 failures).
- Review patches: virtual SAVE rewrites absolute list paths and selects by full path only; cancel restore republishes scan footer; tests for virtual gate-cancel, empty-folder drop, and post-SAVE path identity.

## Spec Change Log

- 2026-09-10: Human renegotiation — junk drag overlay plural `BAD FILES` for ≥2 unloadable items (singular `BAD FILE` kept for one).
- 2026-09-10: Human renegotiation — virtual-list Save As is output-only: do not mutate drop list; clear combo to `<SELECT>` when export is outside the list (align with OPEN + Save As elsewhere); SAVE follows session origin until list browsing re-entered.
- 2026-09-10: Code-review patches — preserve virtual invalidCount on cancel restore; session reload rescans last real folder; DropLoadExtras UnitTest suite rename.
- 2026-09-10: Post-patch re-review — SAVE falls back to session origin when combo is `<SELECT>`; clear selection suppresses load settle; session restore uses `applyComputerPatchesBrowserAfterSessionLoad`; DropLoadReview locks overwrite + in-list invalidCount.

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

**Footer location:** Prefer full absolute disk path in Loaded/Saved (and reconciliation). Footer paint uses path-style middle truncate (`TextFitHelpers` `middle=true`) so distinct parents stay distinguishable when leaf folders share a name. ASCII `...` only.

**Cancel / dirty gates:** Keep `confirmPatchContextChange` and browser snapshot restore; snapshot must include virtual vs folder mode and list contents when virtual.

**Virtual Save As vs SAVE origin:** Drop list = browsing input; Save As = disk export. Do not append the export to the virtual combo. When the export is outside the list, clear selection to `<SELECT>` (same honesty as folder mode Save As outside the opened folder). Session origin (`knownSyxFullPath_` / patch-load context) tracks the last successful computer save/load path so SAVE matches the Saved/Loaded footer path without parsing footer text. Re-entering list browsing starts by choosing a combo row again; then Prev/Next or COMPUTER PATCHES title reload behave as usual.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64` -- expected: build succeeds
- `ctest --test-dir Builds/macOS/...` (or project unit-test target with `MATRIX_BUILD_TESTS=ON`) focusing `PatchManagerActionHandlerDropLoad` / `PatchFileService` -- expected: new + existing cases pass
- `python3 Scripts/quality/lint_touched.py` -- expected: clean on touched C++

**Manual checks:**
- Finder: drop one folder; multi `.syx`; multi folders; mix files+folders; junk-only drag → `BAD FILE` (1) / `BAD FILES` (≥2); OPEN after virtual list; SAVE / Save As (virtual: list unchanged, SAVE follows export until list re-entry); homonym labels; footer Loaded/Saved readability.

### Review Findings

- [x] [Review][Decision] Keep plural BAD FILES — resolved: keep plural; frozen Always / matrix / manual checks updated 2026-09-10.
- [x] [Review][Decision] Virtual Save As vs list — resolved: leave list unchanged; clear combo to `<SELECT>` when export outside list; SAVE follows session origin until list re-entered; spec updated 2026-09-10.
- [x] [Review][Patch] Virtual Save As outside list → keep list, selection 0 / `<SELECT>`; origin tracks export [Source/Core/Actions/PatchManagerActionHandlerComputerDrop.cpp]
- [x] [Review][Patch] Preserve invalidCount when cancel-restoring a virtual list [Source/Core/Services/PatchFileServiceSelection.cpp]
- [x] [Review][Patch] On session reload, rescan last real folder after clearing the virtual cache [Source/Core/PluginProcessorState.cpp]
- [x] [Review][Patch] Give DropLoadExtrasTests a distinct UnitTest suite name [Tests/Unit/PatchManagerActionHandlerDropLoadExtrasTests.cpp]
- [x] [Review][Defer] Sibling drag-drop spec still rejects multi-file — deferred: other-spec / SSOT hygiene (`spec-syx-drag-drop-load.md`)
- [x] [Review][Defer] PATCH NAME drag overlay / editor accept has no automated net — deferred: GUI convention; already in deferred-work; manual Finder
- [x] [Review][Defer] Footer Loaded/Saved middle-truncate untested in paint — deferred: GUI paint; spec Verification already manual
- [x] [Review][Defer] No DropLoad test for multi-folder-only selection — deferred: coverage gap; folder+file and multi-file covered
- [x] [Review][Defer] No automated lock that Save As default folder prefers last real folder while virtual — deferred: `resolveDefaultSaveFolder` already uses `kFolderPath`; coverage only

**Rejected**
- Reconciliation footer lost `resolvedName` — false: Design Notes require full path in Loaded/Saved/reconciliation; `(filename used)` / `(internal name used)` remain; winning name is on PATCH NAME.
- Spec Code Map still cites ComputerBrowser for drop helpers — rejected: fix would edit this spec under review.
- Spec Change Log empty / `review_loop_iteration: 0` — rejected: fix would edit this spec under review.
- Zero-valid merge installs virtual cache before reject — false: prepare restores prior browser; lasting UI matches “no virtual list”.
- Case-insensitive path dedupe can drop distinct files on case-sensitive volumes — low rejected: everyday macOS volumes are case-insensitive; richer dedupe adds complexity.
- Stale `kDropRejectedMultiFile` bullets in deferred-work — false: tracking hygiene, not a product defect from this change.

### Review Findings (post-patch re-review 2026-09-10)

- [x] [Review][Patch] SAVE after virtual Save As outside list must overwrite export via session origin (`knownSyxFullPath_`), not no-op on selection 0 — harden DropLoadReview assert (size/mtime/content) [Source/Core/Actions/PatchManagerActionHandlerComputerLoadSave.cpp:40-58] — fixed: origin fallback + truncate/rewrite lock
- [x] [Review][Patch] Wrap virtual Save As `clearComputerPatchesSelection` with load-suppress like other programmatic selection writes [Source/Core/Actions/PatchManagerActionHandlerComputerDrop.cpp:265] — fixed: suppress inside `clearComputerPatchesSelection`
- [x] [Review][Patch] Lock session reload rescan through `PluginProcessor::applyRestoredPluginState` (or thinnest path that executes that block); optionally start from a virtual list [Source/Core/PluginProcessorState.cpp:100-104] — fixed: `applyComputerPatchesBrowserAfterSessionLoad` + virtual→folder Browser test
- [x] [Review][Patch] Assert in-list virtual SAVE preserves `invalidCount` after rewrite [Tests/Unit/PatchManagerActionHandlerDropLoadReviewTests.cpp] — fixed
- [x] [Review][Defer] Full-path Loaded/Saved APVTS asserts still go through `formatReadablePatchLocation` (tautological) — deferred: Core string shape; paint truncate already deferred; cheap non-tautological assert later
- [x] [Review][Defer] Plural junk drag `BAD FILES` still has no automated overlay net — deferred: GUI convention; reconfirmed; manual Finder

**Rejected (post-patch re-review)**
- Frozen Always / AC / Code Map / Change Log footer-location wording drift vs Design Notes full absolute path — rejected: fix would edit this spec under review.
- Design Notes cancel snapshot omits `invalidCount` while code preserves it — rejected: fix would edit this spec under review.
- Spec Verification commands omit DropLoadReview suite — rejected: fix would edit this spec under review.
- Case-fold Save As can match a virtual list entry via ignore-case on case-sensitive volumes — low rejected: everyday macOS volumes are case-insensitive (same prior disposition).
- Empty drag file list shows singular BAD FILE — low rejected: empty Finder drag uncommon; clearing overlay is polish beyond a direct fix.
- `installVirtualFileList` default `invalidCount = 0` — low rejected: both production call sites pass the count explicitly.
- Save As outside clear without `bumpScanRevision` — false: combo tracks `kSelectPatchFile`; selection 0 updates UI without a scan bump.
- Save As outside must assert `invalidCount` unchanged — low rejected: outside path does not reinstall the list; cancel-restore already locks the count.
