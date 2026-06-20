---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.11 — Mutator EXPORT Layout
author: BMad Agent
status: review
baseline_commit: 781a091
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/6-3-mutationnaming-and-display-names.md
  - implementation-artifacts/6-4-mutate-action.md
  - implementation-artifacts/6-10-historydefrag-on-limit.md
  - implementation-artifacts/4-1-patchfileservice-folder-scan.md
  - implementation-artifacts/4-4-save-with-filename-injection.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.11: Mutator EXPORT Layout

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want EXPORT to write `Initial.syx` and an `Mxx/` folder structure,
so that mutations are saved for hardware or archive (FR-33).

## Acceptance Criteria

1. **Given** Story 6.2 (`MutationHistoryStore`) and Story 6.3 (`MutationNaming`) **When** `PatchFileService::exportMutatorHistory` is implemented **Then** it writes the FR-33 layout to a user-chosen destination folder:
   - **`Initial.syx`** at export root — from `store.getInitialSnapshot()` when `store.hasInitialSnapshot()` (pre-mutation name bytes preserved per D-085; **not** renamed to `"Initial"`).
   - For each root index in `store.getSortedRootIndices()` (ascending numeric order):
     - Subfolder **`Mxx/`** where `xx` = `MutationNaming::formatRootLabel(rootIndex)` without extension (e.g. `M05/`).
     - **`Mxx/Mxx.syx`** — root entry `result` buffer (`getEntry(rootIndex, kRootOnly)`).
     - **`Mxx/Mxx-Ryy.syx`** for each retry in `store.getSortedRetryIndices(rootIndex)` — retry entry `result` buffer.
   - Filenames use `MutationNaming::formatExportStem(rootIndex, retryIndex)` + `PatchFileNameSanitizer::ensureSyxExtension` (self-describing stems per D-085).
   - Patch name bytes 0–7 in each written file match the corresponding display name (`M05`, `M05-R02`, etc.) — store already holds post-naming buffers (6.3); **do not** re-run `applyPatchName` at export time unless a test proves a gap.

2. **And** file I/O reuses existing `PatchFileService::savePatchSysExFile` (temp-file write + decode validation round-trip per Story 4.4) — **no parallel SysEx encoder path**.

3. **And** `PatchMutatorEngine::exportHistory(const juce::File& destinationFolder)` replaces the stub at `PatchMutatorEngine.cpp:415-419`:
   - **Blocked** when `historyStore_.isEmpty()` → `{ success: false, footerMessage: kEmptyHistoryFooterMessage, footerSeverity: warning }` (FR-60).
   - **Blocked** when destination is not a writable directory → `{ success: false, footerMessage: kExportFolderNotWritableFooterMessage, footerSeverity: warning }`.
   - Delegates to `patchFileService_->exportMutatorHistory(destinationFolder, historyStore_, *sysExEncoder_)`.
   - On service failure → `{ success: false, footerMessage: service error or kExportFailedFooterMessage, footerSeverity: warning }`.
   - On success → `{ success: true, footerMessage: kExportCompleteFooterMessage (includes file count), footerSeverity: info }`.
   - **Does not** mutate history, selection, compare state, or enqueue SysEx (read-only action; D-027 manual export only).

4. **And** engine constructor gains non-owning dependencies required for export:
   - `PatchFileService* patchFileService_`
   - `SysExEncoder* sysExEncoder_`
   - Null pointers → defensive `success: false` from `exportHistory` (no crash).
   - Update `PatchMutatorEngineTests::EngineHarness` accordingly; **`PluginProcessor` wiring deferred to Story 7.4** (processor already owns `patchFileService_` and `midiManager->getSysExEncoder()`).

5. **And** gap indices export correctly — roots at `0, 5, 99` produce folders `M00/`, `M05/`, `M99/` (not contiguous folder renumbering).

6. **And** unit tests cover service + engine without GUI:
   - **`PatchFileServiceTests`** (extend) — layout, name-byte round-trip, gap indices, `Initial.syx`, folder-not-writable.
   - **`PatchMutatorEngineTests`** (extend) — empty-history block, success footer, delegation to service.
   - Full `Matrix-Control_Tests` target remains green.

7. **And** this story delivers **`exportMutatorHistory` + engine `exportHistory()` + tests only**:
   - **No** folder-picker GUI (**7.4** injects picker callback → engine).
   - **No** `MutatorActionHandler` EXPORT wiring (**7.4**).
   - **No** EXPORT button `setEnabled` mirrors (**6.12** / FR-60).
   - **No** auto-export on MUTATE/RETRY (**D-027** — grep confirms no export calls in `mutate()`/`retry()`).
   - **No** clear-on-patch-load changes (**6.13**).

## Tasks / Subtasks

- [x] **Add `exportMutatorHistory` to `PatchFileService`** (AC: #1, #2, #5)
  - [x] `PatchFileExportResult` struct: `success`, `filesWritten`, `errorMessage`
  - [x] `exportMutatorHistory(folder, store, encoder)` — iterate store per algorithm below
  - [x] Create `Mxx/` subdirectories with `juce::File::createDirectory()`
  - [x] Write `Initial.syx` when `hasInitialSnapshot()`

- [x] **Extend `PatchMutatorEngine` for export** (AC: #3, #4)
  - [x] Add `PatchFileService*`, `SysExEncoder*` to constructor + members
  - [x] Replace `exportHistory` stub
  - [x] Add footer constants: `kExportCompleteFooterMessage`, `kExportFolderNotWritableFooterMessage`, `kExportFailedFooterMessage`

- [x] **Unit tests** (AC: #6)
  - [x] Extend `Tests/Unit/PatchFileServiceTests.cpp`
  - [x] Extend `Tests/Unit/PatchMutatorEngineTests.cpp` (`EngineHarness` + export cases)

- [x] **CMake + self-review** (AC: #7)
  - [x] No new `.cpp` if logic fits existing files; no GUI includes in service
  - [x] Grep — `mutate()` / `retry()` do not call export
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

## Dev Notes

### What Story 6.11 IS — and what it is NOT

| In scope (6.11) | Out of scope |
|---|---|
| `PatchFileService::exportMutatorHistory` (FR-33) | Folder-picker dialog (**7.4**) |
| `PatchMutatorEngine::exportHistory(folder)` | `MutatorActionHandler::handleExport` (**7.4**) |
| FR-33 folder layout + name-byte fidelity | EXPORT button enabled mirrors (**6.12**) |
| Reuse `savePatchSysExFile` encode/validate path | Auto-export on mutation (**D-027**) |
| Engine/service unit tests | `PluginProcessor` engine construction (**7.4**) |
| Engine ctor injection for file service + encoder | Partial-export cleanup policy (fail-fast is sufficient v1) |

**Critical boundary:** EXPORT is a **manual checkpoint** (D-027). User picks folder in GUI layer (7.4); Core writes layout. Handler must call **engine only** — never `PatchFileService` directly (7.4 AC #5).

### Export layout (implement exactly)

Per addendum § EXPORT and D-085:

```text
ExportFolder/
  Initial.syx                 ← initial snapshot (original patch name in bytes 0–7)
  M00/
    M00.syx                   ← root entry result
    M00-R00.syx               ← retry entries
    M00-R01.syx
  M05/
    M05.syx
    M05-R00.syx
```

**Folder names** = `MutationNaming::formatRootLabel(rootIndex)` → `"M05"` (not `"M05.syx"`).

**File names** = `PatchFileNameSanitizer::ensureSyxExtension(MutationNaming::formatExportStem(...))`.

**Initial.syx** filename is literal `"Initial.syx"`; buffer bytes 0–7 stay the **pre-mutation** patch name (never passed through `MutationNaming::applyPatchName`).

### `exportMutatorHistory` algorithm (implement exactly)

```
exportMutatorHistory(folder, store, encoder):
  if store.isEmpty():
    return { success: false, errorMessage: "History empty" }

  if !folder.isDirectory() || !folder.hasWriteAccess():
    return { success: false, errorMessage: "Folder not writable" }

  filesWritten = 0

  if store.hasInitialSnapshot():
    initialFile = folder.getChildFile("Initial.syx")
    save = savePatchSysExFile(initialFile, store.getInitialSnapshot().data(), encoder)
    if !save.success: return fail with save.errorMessage
    ++filesWritten

  for rootIndex in store.getSortedRootIndices():
    rootLabel = formatRootLabel(rootIndex)           // "M05"
    rootDir = folder.getChildFile(rootLabel)
    if !rootDir.createDirectory(): return fail

    rootEntry = store.getEntry(rootIndex, kRootOnly)
    if rootEntry:
      rootFile = rootDir.getChildFile(ensureSyxExtension(rootLabel))
      save rootEntry->result buffer
      if fail: return fail
      ++filesWritten

    for retryIndex in store.getSortedRetryIndices(rootIndex):
      stem = formatExportStem(rootIndex, retryIndex)  // "M05-R02"
      retryFile = rootDir.getChildFile(ensureSyxExtension(stem))
      save retryEntry->result buffer
      if fail: return fail
      ++filesWritten

  return { success: true, filesWritten }
```

**Fail-fast:** first `savePatchSysExFile` failure aborts; prior files may remain on disk (document in tests; no rollback required v1).

**Skip missing root entry defensively:** if `getEntry(rootIndex, kRootOnly)` returns nullopt, skip root `.syx` but still process retries (should not happen with consistent store — add test only if engine can create such state).

### `exportHistory()` algorithm (implement exactly)

```
exportHistory(destinationFolder):
  if historyStore_.isEmpty():
    return blocked (kEmptyHistoryFooterMessage, warning)

  if patchFileService_ == nullptr || sysExEncoder_ == nullptr:
    return { success: false }

  if !destinationFolder.isDirectory() || !destinationFolder.hasWriteAccess():
    return blocked (kExportFolderNotWritableFooterMessage, warning)

  const auto exportResult = patchFileService_->exportMutatorHistory(
      destinationFolder, historyStore_, *sysExEncoder_)

  if !exportResult.success:
    return blocked (exportResult.errorMessage or kExportFailedFooterMessage, warning)

  result.success = true
  result.footerMessage = formatExportComplete(exportResult.filesWritten)
  result.footerSeverity = info
  return result
```

**Do not** call `forceExitCompare()`, `syncHistoryUiProperties`, or `auditionAfterHistoryMutation()` — export is read-only.

Suggested footer strings (English, anonymous namespace in `PatchMutatorEngine.cpp`):

- `kExportCompleteFooterMessage` — e.g. `"Exported N mutation file(s)."` (substitute count)
- `kExportFolderNotWritableFooterMessage` — e.g. `"Export folder is not writable."`
- `kExportFailedFooterMessage` — e.g. `"Mutation export failed."`

### Existing code to extend (do not rewrite)

**Stub to replace:**

```415:419:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
MutatorActionResult PatchMutatorEngine::exportHistory(const juce::File&)
{
    MutatorActionResult result;
    return result;
}
```

**Reuse save path** (`PatchFileService.cpp:45-109`):

- Temp file `*_write.tmp` → validate via `SysExDecoder` → atomic `moveFileTo` target.
- Parent directory must exist and be writable (subfolder creation is caller responsibility before `savePatchSysExFile`).

**Store iteration API** (already complete from 6.2):

```57:58:Source/Core/Services/PatchMutator/MutationHistoryStore.h
        juce::Array<int> getSortedRootIndices() const;
        juce::Array<int> getSortedRetryIndices(int rootIndex) const;
```

**Naming helpers** (6.3 — `formatExportStem` == `formatPatchName`):

```40:43:Source/Core/Services/PatchMutator/MutationNaming.cpp
    juce::String MutationNaming::formatExportStem(int rootIndex, int retryIndex)
    {
        return formatPatchName(rootIndex, retryIndex);
    }
```

**Initial snapshot contract** (6.2, 6.8):

- Captured on first successful `mutate()` (`PatchMutatorEngine.cpp:131-132`).
- `clear()` does **not** clear initial snapshot — export after CLEAR still writes `Initial.syx` if snapshot exists and history re-populated; if history empty, export blocked regardless.

### File structure (this story)

```
Source/Core/Services/
├── PatchFileService.h              (UPDATE — exportMutatorHistory decl + PatchFileExportResult)
├── PatchFileService.cpp            (UPDATE — export implementation)
└── PatchMutator/
    ├── PatchMutatorEngine.h        (UPDATE — ctor params, forward decls)
    └── PatchMutatorEngine.cpp      (UPDATE — exportHistory impl)

Tests/Unit/
├── PatchFileServiceTests.cpp       (UPDATE — export layout tests)
└── PatchMutatorEngineTests.cpp     (UPDATE — EngineHarness ctor + export tests)
```

No new source files expected unless export logic exceeds ~15-line helpers (then extract `MutatorExportWriter` only if needed — prefer private methods on `PatchFileService`).

### CMake registration

No new `.cpp` expected — **no CMake change** unless a new file is introduced.

If tests-only changes: verify `PatchFileService.cpp` and `PatchMutatorEngine.cpp` already in plugin + test targets (they are).

### Testing requirements

**PatchFileServiceTests** — use temp directory pattern from existing tests (`createTempScanDir()`):

| Test | Setup | Assert |
|---|---|---|
| `exportMutatorHistory_emptyStore_fails` | Empty store | `!success` |
| `exportMutatorHistory_writesInitialAndRoot` | Snapshot + root M00 | `Initial.syx` exists; `M00/M00.syx` exists; `filesWritten >= 2` |
| `exportMutatorHistory_writesRetries` | Root M00 + R00, R01 | `M00/M00-R00.syx`, `M00/M00-R01.syx` exist |
| `exportMutatorHistory_gapIndices` | Roots at 0, 5, 99 | Folders `M00/`, `M05/`, `M99/` (not `M01/`, `M02/`) |
| `exportMutatorHistory_nameBytesMatch` | Named entries | Load each `.syx` → decode → `getName()` matches `formatPatchName` |
| `exportMutatorHistory_initialKeepsOriginalName` | Snapshot with name `"MY PATCH"` | `Initial.syx` decoded name unchanged (not `M00`) |
| `exportMutatorHistory_nonWritableFolder_fails` | Invalid parent path | `!success`, error message set |
| `exportMutatorHistory_roundTripValidates` | Any export | All files pass `scanFolder` validCount |

**PatchMutatorEngineTests** — extend `EngineHarness`:

```cpp
// Add to EngineHarness:
SysExDecoder decoder;
PatchFileService patchFileService { decoder };
// Pass &patchFileService, &midiManager.getSysExEncoder() to engine ctor
```

| Test | Setup | Assert |
|---|---|---|
| `export_emptyHistory_blocked` | Empty store | `!success`, `kEmptyHistoryFooterMessage`, 0 files |
| `export_success_footer` | After `mutate()` | `success`, info severity, footer mentions file count |
| `export_nonWritableFolder_blocked` | Fake/nonexistent folder | `!success`, warning footer |
| `export_doesNotMutateStore` | History count N | Same `rootCount()` after export |
| `export_noSysEx` | After export | `countPatchSysExMessages == 0` |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **AD-6** — EXPORT delegated to `PatchFileService::exportMutatorHistory(folder, store)` per architecture.md § PatchMutatorEngine.
- **AD-7** — Initial snapshot RAM-only; written only on manual EXPORT.
- **Core ↛ GUI** — no `juce_gui_basics`, no folder picker in Core.
- **D-027** — manual EXPORT only; no hooks in `mutate()`/`retry()`.
- **D-085** — self-describing `Mxx-Ryy.syx` filenames; 0-based indices in folder names.
- **FR-33** — layout + name-byte match + no auto-export.
- **FR-60** — EXPORT blocked when history empty (enabled mirrors in 6.12).
- English source/comments only.
- Functions ≤ 15 lines — extract `writeRootFolder`, `writeInitialSnapshot` helpers on `PatchFileService`.

### Previous story intelligence (6.10 — done)

- Footer constant pattern in anonymous namespace — follow for export messages.
- `kEmptyHistoryFooterMessage` already defined — reuse for blocked export.
- Engine orchestration pattern: guard → delegate to service → footer on result.
- Review lesson: test exact footer strings where AC specifies them.

### Previous story intelligence (6.3 — done)

- Store holds **post-naming** buffers — export reads `result` arrays directly.
- `formatExportStem` + `ensureSyxExtension` for filenames — do not invent alternate naming.
- Initial snapshot **excluded** from `MutationNaming` — never rename for export.

### Previous story intelligence (6.2 — done)

- `getSortedRootIndices()` returns numeric ascending order — drives folder iteration order.
- `clear()` preserves initial snapshot — export may still write `Initial.syx` after CLEAR if user mutates again; blocked when `isEmpty()`.
- `getEntry` returns `std::optional<MutationEntry>` with `result` buffer array.

### Previous story intelligence (4.4 — done)

- `savePatchSysExFile` is the **only** sanctioned disk write for patch SysEx.
- Write failure messages: `"Folder not writable"`, `"Encode failed"`, `"Validation failed"`, `"Write failed"` — propagate to export result.

### Git intelligence

| Commit | Relevance |
|---|---|
| `781a091` | Story 6.10 — engine orchestration + footer patterns; `exportHistory` stub still empty |
| `8b2d916` | Story 6.9 — `kEmptyHistoryFooterMessage`, blocked empty-history pattern |
| `87a2a97` | Story 6.8 — initial snapshot capture on first mutate |
| `9a9eaef` | Story 6.7 — export must not enqueue SysEx |
| Earlier 6.2–6.4 | Store + naming + engine API surface |

**Brownfield:** `PatchFileService` has scan/save/load only — **no** `exportMutatorHistory` yet. `PatchMutatorEngine::exportHistory` returns `{ success: false }`. `PluginProcessor` does not construct `PatchMutatorEngine` yet (7.4).

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `juce::File::createDirectory()`, `getChildFile()`; no new dependencies.
- No web research required — FR-33, addendum layout, and D-085/D-027 fully specify behaviour.
- `SysExEncoder` passed by reference (same pattern as `PatchManagerActionHandler` save path).

### Story 7.4 handoff (EXPORT wiring)

```cpp
// Composition root (PluginProcessor or editor):
auto folder = exportFolderPicker();  // GUI — FileChooser
if (folder == juce::File())
    return;  // user cancelled — no footer

auto result = patchMutatorEngine_->exportHistory(folder);
propagateFooterMessage(result.footerMessage, result.footerSeverity);
```

Engine ctor in 7.4:

```cpp
patchMutatorEngine_ = std::make_unique<PatchMutatorEngine>(
    patchModel_.get(), ..., patchFileService_.get(), &midiManager->getSysExEncoder());
```

Handler **must not** call `patchFileService_->exportMutatorHistory` directly.

### Story 6.12 handoff (awareness)

`refreshActionEnabledMirrors` will disable EXPORT when `rootCount() == 0` (FR-60). Engine blocked path already returns footer — 6.12 mirrors only.

### Story 6.13 handoff (awareness)

Patch load clears history (not initial snapshot policy TBD in 6.13). Export after load with empty history remains blocked.

### Project context reference

- `project-context.md` — Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — no French in source; English footers
- FR-33 manual export layout; FR-60 EXPORT requires non-empty history
- D-027 no auto-export; D-085 naming; AD-6/AD-7 persistence boundaries

### References

- [epics.md — Story 6.11]
- [prd.md — FR-33, FR-60]
- [addendum.md — § EXPORT layout]
- [.decision-log.md — D-027, D-085]
- [architecture.md — AD-6 EXPORT delegation, AD-7]
- [6-2-mutationhistorystore-two-level-mr.md — store API, initial snapshot]
- [6-3-mutationnaming-and-display-names.md — formatExportStem, post-naming buffers]
- [6-4-mutate-action.md — exportHistory stub ownership]
- [6-10-historydefrag-on-limit.md — engine orchestration pattern]
- [4-4-save-with-filename-injection.md — savePatchSysExFile pipeline]
- [7-4-mutatoractionhandler.md — folder picker + engine-only export]
- [PatchFileService.cpp — savePatchSysExFile]
- [PatchMutatorEngine.cpp — exportHistory stub, initial snapshot capture]
- [MutationNaming.cpp — formatExportStem]
- [MutationHistoryStore.h — iteration API]

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Debug Log References

### Completion Notes List

- Implemented `PatchFileService::exportMutatorHistory` with FR-33 layout: `Initial.syx` at root, `Mxx/` folders with self-describing `.syx` stems via `MutationNaming` + `PatchFileNameSanitizer`, reusing `savePatchSysExFile` encode/validate path.
- Replaced `PatchMutatorEngine::exportHistory` stub: empty-history and non-writable-folder guards, null dependency defense, delegation to service, info/warning footers.
- Extended engine constructor with optional `PatchFileService*` and `SysExEncoder*` (defaults `nullptr`; `PluginProcessor` wiring deferred to 7.4).
- Added 8 `PatchFileServiceTests` and 5 `PatchMutatorEngineTests` export cases; `Matrix-Control_Tests` green.

### File List

- Source/Core/Services/PatchFileService.h
- Source/Core/Services/PatchFileService.cpp
- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Tests/Unit/PatchFileServiceTests.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp

### Review Findings

- [x] [Review][Patch] `writeRootFolder`, `exportMutatorHistory`, `exportHistory` exceed 15-line architecture limit [PatchFileService.cpp:135-230, PatchMutatorEngine.cpp:428-467]
- [x] [Review][Patch] `exportMutatorHistory_roundTripValidates` should use `scanFolder` per test plan [PatchFileServiceTests.cpp:515-536]
- [x] [Review][Patch] `export_emptyHistory_blocked` should verify 0 files written [PatchMutatorEngineTests.cpp:1610-1625]
- [x] [Review][Patch] `export_success_footer` should assert exact footer message with file count [PatchMutatorEngineTests.cpp:1648-1649]
- [x] [Review][Defer] Partial export fail-fast leaves prior files on disk (v1 spec) — deferred, pre-existing policy
- [x] [Review][Defer] Re-export overwrite behavior undefined for v1 — deferred, out of scope
- [x] [Review][Defer] Non-writable folder tests use nonexistent path, not read-only directory — deferred, test hardening
- [x] [Review][Defer] No tests for mid-batch export failure / orphan output — deferred, v1 fail-fast policy

## Change Log

- 2026-06-20 — Story 6.11 created: PatchFileService exportMutatorHistory, engine exportHistory(), FR-33 layout, injection contract, test plan; picker/handler/processor wiring deferred to 7.4.
- 2026-06-20 — Code review: refactored export helpers to ≤15-line functions; strengthened scanFolder and footer assertions in tests.
