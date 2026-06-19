---
organization: Ten Square Software
project: Matrix-Control
title: Story 4.1 — PatchFileService Folder Scan
author: BMad Agent
status: done
baseline_commit: a1547e6
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/3-4-master-module-init-confirmation-path.md
  - implementation-artifacts/5-1-clipboardservice-compatibility-matrix.md
  - implementation-artifacts/7-3-patchmanageractionhandler-bank-and-internal.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 4.1: PatchFileService Folder Scan

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want to open a folder and scan valid `.syx` files,
so that I can browse my patch library (FR-25).

## Acceptance Criteria

1. **Given** Epic 2 complete (SysEx parser/decoder, `MidiManager` queue paths) and brownfield `PatchManagerActionHandler` scaffold (Story 7.3) **When** user clicks **OPEN** in Computer Patches **Then** a native folder picker appears; on confirm, `Core::PatchFileService` scans the folder and returns a structured result — **no patch load**, **no SysEx enqueue**, **no APVTS parameter mutation** (scan-only per FR-25 / D-024).
2. **And** scan iterates **non-recursive** `*.syx` files (case-insensitive extension) in the chosen folder; each file is read into memory and validated with `SysExDecoder::validatePatchSysExMessage` — **patch dumps only** (`MessageType::kPatch`); `Master 1.syx`-style master dumps and corrupt files count as **invalid**, are skipped without throw/crash, and do not abort the scan.
3. **And** scan produces **sorted ascending** valid filenames (lexicographic on `juce::File::getFileName()` — same order FR-52 / Story 4.6 will use); invalid count is tracked separately; empty folder → `validCount == 0`, `invalidCount == 0`.
4. **And** footer reports scan outcome via existing APVTS contract (`uiMessageText`, `uiMessageSeverity`):
   - Valid folder with files: **info** severity, message includes valid + invalid counts (e.g. `"12 valid, 3 invalid"` — exact copy in `PluginDisplayNames`).
   - Empty folder: **info**, `"0 files in folder"` (D-024).
   - Missing / non-directory / unreadable path: **warning**, `"Folder not found"` (D-024).
5. **And** `PatchFileService` lives in `Source/Core/Services/PatchFileService.{h,cpp}`, namespace `Core`, **zero GUI dependencies** (`Core ↛ GUI`). Validation reuses `SysExDecoder` — **no duplicate SysEx structure tables**.
6. **And** `PatchManagerActionHandler` handles `computerPatchesOpenFolder`: invokes injected folder picker → `PatchFileService::scanFolder` → footer propagation; other Computer Patches action IDs remain explicit no-ops (`Epic 4` stories 4.2–4.6).
7. **And** folder picker is **injected** at handler/processor boundary (same pattern as `MutatorActionHandler` `ExportFolderPicker` in Story 7.4) — `FileChooser` runs on message thread from `PluginEditor` or processor callback; Core never includes `juce_gui_basics` file chooser.
8. **And** `PluginProcessor` owns `std::unique_ptr<PatchFileService>` (composition root AD-2); service retains **last scan snapshot** (folder path, sorted valid filenames, counts) for Stories 4.2–4.6 — exposed via const getters, not APVTS yet.
9. **And** `Tests/Unit/PatchFileServiceTests.cpp` covers: all committed patch fixtures validate; `Master 1.syx` rejected; empty temp dir; temp dir with one valid + one invalid (truncate/garbage) file; nonexistent folder; sorted order with `Patch 5.syx` / `Patch 66.syx` / `Patch 71.syx` / `Patch 808.syx`. Register `.cpp` in plugin + `Matrix-Control_Tests` `CMakeLists.txt`; full test suite passes.

## Tasks / Subtasks

- [x] **Define `PatchFileService` API + result struct** (AC: #1, #5, #8)
  - [x] `PatchFolderScanResult` — `folder`, `folderUsable`, `validCount`, `invalidCount`, `sortedValidFileNames` (`juce::StringArray`), `footerMessage`, `footerSeverity` (`info` | `warning`)
  - [x] `PatchFileService.h` — ctor takes `SysExDecoder&`; `scanFolder(const juce::File&)` returns result + updates last-scan cache; const getters for last scan
  - [x] `kSyxExtension = ".syx"` constexpr; scan uses `juce::File::findChildFiles(juce::File::findFiles, false, "*.syx")`

- [x] **Implement scan + validation loop** (AC: #2, #3)
  - [x] Read file via `loadFileAsData`; on read failure → increment invalid, continue
  - [x] `decoder_.validatePatchSysExMessage(memoryBlock)` — true → append filename to valid list; false → invalid++
  - [x] Sort valid names before returning; build footer via `PluginDisplayNames` helpers

- [x] **Footer message constants** (AC: #4)
  - [x] Add `PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages` — `formatScanSummary(valid, invalid)`, `kEmptyFolder`, `kFolderNotFound`
  - [x] Optional thin helper `PatchFileServiceFooter::propagateScanResult(apvts, result)` mirroring `InitTemplateFooter::propagateMessage`

- [x] **Wire OPEN action** (AC: #6, #7)
  - [x] `using PatchFolderPicker = std::function<juce::File()>;` on `PatchManagerActionHandler` ctor
  - [x] `handleOpenPatchFolder()` — picker returns invalid/empty path → no-op (user cancelled); else scan + footer
  - [x] `PluginProcessor`: construct `patchFileService_`; pass picker lambda (initially returns invalid `File` until editor registers real chooser)
  - [x] `PluginEditor`: register folder picker on processor (async `FileChooser` `"Select patch folder"`, `canSelectDirectories=true`, `canSelectFiles=false`); call from message thread only

- [x] **Unit tests + CMake** (AC: #9)
  - [x] `Tests/Unit/PatchFileServiceTests.cpp` — temp dirs, fixture copies from `Tests/Fixtures/Patches/` and `Tests/Fixtures/Masters/`
  - [x] Register `Source/Core/Services/PatchFileService.cpp` in plugin + test targets

- [x] **Self-review** (AC: #5) — classes ≤ 200 lines; methods ≤ 15 lines; no GUI in Core; no load/save/persistence/combobox work in this story

### Review Findings

- [x] [Review][Decision] FileChooser synchrone vs `launchAsync` — **Resolved: keep synchronous `browseForDirectory()` for story 4.1** (option 1). `launchAsync` deferred to a future cross-cutting picker story.

- [x] [Review][Patch] Dangling `SysExDecoder` reference in unit tests [`Tests/Unit/PatchFileServiceTests.cpp:63-68`] — fixed: parser/decoder/service as test class members.

- [x] [Review][Patch] Case-insensitive `.syx` not enforced on case-sensitive filesystems [`Source/Core/Services/PatchFileService.cpp:54-64`] — fixed: enumerate all files, filter with `hasSyxExtension`; test added.

- [x] [Review][Patch] Unreadable directory may report `"0 files in folder"` instead of `"Folder not found"` [`Source/Core/Services/PatchFileService.cpp:15-22`] — fixed: `isFolderReadable` / `hasReadAccess` guard before scan.

- [x] [Review][Patch] `scanFolder` exceeds 15-line method limit [`Source/Core/Services/PatchFileService.cpp:15-47`] — fixed: extracted `scanReadableFolder` + `collectSyxScanResults`.

- [x] [Review][Defer] Synchronous full-folder scan blocks message thread [`Source/Core/Services/PatchFileService.cpp:15-47`] — deferred, acceptable v1 per story dev notes ("acceptable for v1 folder sizes").

- [x] [Review][Defer] `validatePatchSysExMessage` logs `decodePatchSysEx: validation failed` on every rejected file during bulk scan [`SysExDecoder.cpp`] — pre-existing decoder logging; noisy in scan context.

- [x] [Review][Defer] No `PatchManagerActionHandler` OPEN integration test with real temp-dir picker [`Tests/Unit/PatchManagerActionHandlerTests.cpp:198`] — optional per story spec.

- [x] [Review][Defer] `loadFileAsData` without size cap during scan [`Source/Core/Services/PatchFileService.cpp:67-72`] — out of scope v1; no spec requirement.

## Dev Notes

### What this story IS — and what it is NOT

Story 4.1 delivers the **Core folder scan service** and **OPEN → scan → footer** path only. It replaces the dev placeholder understanding that Computer Patches file ops were stubbed in Story 7.3 (AC #7 there).

It must **NOT** in this story:
- Implement combobox sentinel states `<EMPTY>` / `<SELECT>` or remove hardcoded factory names in `ComputerPatchesPanel` (**Story 4.2**)
- Persist last folder path or rescan on relaunch (**Story 4.3** — D-010 prefs)
- Load a selected `.syx` into `PatchModel`, push APVTS, or enqueue patch **0x01** SysEx (**Stories 4.5+**)
- Save / Save As / filename injection (**Story 4.4**)
- Previous/Next file navigation (**Story 4.6** — FR-52)
- Name reconciliation policy (**Story 4.5** — D-025)
- `DirtyPatchTracker` unsaved warning (**Epic 9**)
- Mutator `exportMutatorHistory` (**Epic 6** / FR-33)
- Use `InitTemplateLoader` for Computer Patches browse (D-034a — separate services per decision log)

[Source: D-024, D-058 step 4, FR-25 in `prd.md`; Story 7.3 AC #7 deferral]

### Epic 4 cross-story map

| Story | Delivers | Relation to 4.1 |
|---|---|---|
| **4.1 (this)** | `PatchFileService::scanFolder`, OPEN wiring, footer counts, last-scan cache | Foundation for all Computer Patches file ops |
| **4.2** | Combobox `<EMPTY>` / `<SELECT>` sentinels; bind combobox to `sortedValidFileNames` | Consumes last-scan cache; removes hardcoded names |
| **4.3** | Session XML persistence of folder path; relaunch rescan without load | Calls `scanFolder` on startup |
| **4.4** | Save/Save As + 8-char filename injection | Uses service folder + validation on write |
| **4.5** | Load + name reconciliation policy | Uses `decodePatchSysEx` + mapper |
| **4.6** | `<` `>` prev/next navigation | Uses sorted list from service |

### Brownfield state (READ before editing)

| File | Current behaviour | This story changes |
|---|---|---|
| `PatchManagerActionHandler.cpp:80-88` | Computer Patches IDs → `return; // Epic 4` | Implement **OPEN** only; others stay stub |
| `ComputerPatchesPanel.cpp:202-218` | Hardcoded 48 factory patch names in combobox | **Leave unchanged** — 4.2 replaces with service-driven list |
| `PluginProcessor.cpp` | No `PatchFileService` member | Add service + picker registration hook |
| `Source/Core/Services/` | `ClipboardService`, `DeviceTypeRegistry`, … — **no** `PatchFileService` yet | **NEW** |

### Validation contract (reuse — do not reinvent)

Patch Computer Patches library accepts **single patch SysEx files** only:

```132:141:Source/Core/MIDI/SysEx/SysExDecoder.cpp
bool SysExDecoder::validatePatchSysExMessage(const juce::MemoryBlock& sysEx) const
{
    auto validation = parser.validateSysEx(sysEx);
    if (!validation.isValid || validation.messageType != SysExParser::MessageType::kPatch)
    {
        MidiLogger::getInstance().logError("decodePatchSysEx: validation failed");
        return false;
    }
    return true;
}
```

- **Valid examples:** `Tests/Fixtures/Patches/Patch 71.syx` (134-byte packed payload after decode).
- **Invalid examples:** `Tests/Fixtures/Masters/Master 1.syx` (master opcode 0x03); empty file; truncated SysEx; random bytes.
- **Postel's law:** log via `MidiLogger` on validation failure is acceptable; scan must never propagate exception to UI thread.

[Source: `InitTemplateLoader.cpp` `loadSysExBytes` pattern for file read; NFR-1 `.syx` validation tests]

### `PatchFileService` — suggested API

```cpp
namespace Core
{
    struct PatchFolderScanResult
    {
        juce::File folder;
        bool folderUsable = false;
        int validCount = 0;
        int invalidCount = 0;
        juce::StringArray sortedValidFileNames;
        juce::String footerMessage;
        juce::String footerSeverity; // "info" | "warning"
    };

    class PatchFileService
    {
    public:
        explicit PatchFileService(SysExDecoder& decoder) noexcept;

        PatchFolderScanResult scanFolder(const juce::File& folder);
        const PatchFolderScanResult& getLastScanResult() const noexcept;

    private:
        SysExDecoder& decoder_;
        PatchFolderScanResult lastScan_;
    };
}
```

Keep scan loop in small private helpers (`countFile`, `appendValidName`) to satisfy Clean Code limits.

### Folder picker injection (mandatory — Core ↛ GUI)

Mirror Story 7.4 EXPORT pattern:

```cpp
using PatchFolderPicker = std::function<juce::File()>;

PatchManagerActionHandler(..., PatchFileService* patchFileService, PatchFolderPicker pickFolder, ...);

void PatchManagerActionHandler::handleOpenPatchFolder()
{
    if (patchFileService_ == nullptr || !pickFolder_)
        return;

    const juce::File folder = pickFolder_();
    if (!folder.isDirectory())
        return; // user cancelled

    const auto result = patchFileService_->scanFolder(folder);
    PatchFileServiceFooter::propagateScanResult(apvts_, result);
}
```

**PluginEditor** registers real picker (JUCE 8.0.12):

```cpp
processor.setPatchFolderPicker([safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
{
    if (safeThis == nullptr)
        return {};

    juce::File chosen;
    // FileChooser async or sync on message thread — must not block audio/MIDI threads
    return chosen;
});
```

Expose `setPatchFolderPicker` on `PluginProcessor` (or pass at editor construction after `createEditor`). Default no-op picker until editor attaches.

### Footer messages (D-024 — authoritative)

| Condition | Severity | Message (English — `PluginDisplayNames`) |
|---|---|---|
| Folder missing / not a directory | `warning` | `Folder not found` |
| Valid directory, zero `.syx` files | `info` | `0 files in folder` |
| Scan complete | `info` | `{valid} valid, {invalid} invalid` (both counts always shown) |

Use `ExceptionPropagator::clearMessage` when replacing a prior footer after successful scan with zero issues is **not** required — info scan summary should display. Clear stale warnings from unrelated actions if needed (mirror `MasterModuleInitService` success path).

[Source: addendum § Computer Patches combobox states; D-024 decision log]

### Architecture compliance

- **Path:** `Source/Core/Services/PatchFileService.{h,cpp}` — architecture doc says `src/` post-P-001; repo still uses `Source/` (Epic 0 done but tree kept `Source/` — follow existing `CMakeLists.txt`).
- **Composition root:** `PluginProcessor` owns service via `unique_ptr` (AD-2).
- **Dependency direction:** Service depends on `SysExDecoder` only (+ JUCE Core); handler depends on service pointer; no GUI headers in Core.
- **Separate from InitTemplateLoader:** Init templates (`PatchInit.syx`) use Settings folder + `InitTemplateLoader` (D-034a); Computer Patches OPEN uses independent user library folder.
- **Threading:** Scan is synchronous on message thread after folder pick — acceptable for v1 folder sizes; no `processBlock` / MIDI thread work.
- **Idempotency:** Re-scanning same folder replaces `lastScan_` entirely.

### File structure (this story)

```
Source/Core/Services/
├── PatchFileService.h          (NEW)
└── PatchFileService.cpp        (NEW)

Source/Core/Actions/
├── PatchManagerActionHandler.h (UPDATE — picker + service deps)
└── PatchManagerActionHandler.cpp (UPDATE — handleOpenPatchFolder)

Source/Core/
├── PluginProcessor.h/.cpp      (UPDATE — service member, picker setter)

Source/GUI/
└── PluginEditor.cpp            (UPDATE — register FileChooser picker)

Source/Shared/Definitions/
└── PluginDisplayNames.h        (UPDATE — Computer Patches footer format helpers)

Tests/Unit/
└── PatchFileServiceTests.cpp   (NEW)

CMakeLists.txt                  (UPDATE — register new .cpp)
```

### Testing requirements

Follow `InitTemplateLoaderTests` / `ClipboardServiceTests` — `juce::UnitTest`, AAA, temp dirs under `juce::File::tempDirectory`.

| Test | Assert |
|---|---|
| `scan_committedPatchFixtures_allValid` | Copy all 4 patch fixtures to temp dir → `validCount == 4`, `invalidCount == 0` |
| `scan_rejectsMasterDump` | Folder with `Master 1.syx` only → `validCount == 0`, `invalidCount == 1` |
| `scan_mixedValidInvalid` | One patch fixture + one 4-byte garbage file → counts 1/1 |
| `scan_emptyFolder` | Empty temp dir → `validCount == 0`, footer `0 files in folder` |
| `scan_nonexistentFolder` | `folderUsable == false`, warning footer |
| `scan_sortOrder` | Files named `Z.syx`, `A.syx`, `M.syx` → sorted `A`, `M`, `Z` |
| `scan_readFailure_countsInvalid` | Zero-byte `.syx` → invalid |

Optional handler test in `PatchManagerActionHandlerTests.cpp`: fake picker returns temp dir → assert footer properties set — only if low effort.

Manual smoke (after implementation):
1. Standalone → Computer Patches → OPEN → select folder with factory `.syx` → footer shows valid/invalid counts.
2. OPEN folder with only `Master 1.syx` → `0 valid, 1 invalid`.
3. Cancel folder dialog → no footer change, no crash.
4. Confirm combobox still shows legacy hardcoded list (expected until 4.2).

### Previous story intelligence (Epic 3 / Epic 7)

| Source | Learning for 4.1 |
|---|---|
| **Story 3.2** `InitTemplateLoader` | File read + `validatePatchSysExMessage` + graceful invalid handling — copy patterns, not the loader itself |
| **Story 3.4** `InitTemplateFooter` | Footer severity mapping (`info` vs `warning`) |
| **Story 5.1** `ClipboardService` | Service placement under `Source/Core/Services/`, CMake dual registration, zero GUI |
| **Story 7.3** | `PatchManagerActionHandler` Computer Patches stub at lines 80–88 — replace OPEN branch only |
| **Story 7.4** | Injected `std::function` picker for Core/GUI boundary |

### Git intelligence (recent commits)

Recent work focused on **ActionDispatcher** and **PatchManagerActionHandler** bank/internal paths (`a1547e6`, `577cd83`). Computer Patches file ops intentionally deferred with `// Epic 4` comment — this story is the first Epic 4 implementation. Follow handler extension + processor wiring patterns from 7.3 (constructor injection, nullable service pointers for tests).

### Latest tech information

- **JUCE 8.0.12** — `juce::FileChooser::launchAsync` preferred for non-blocking folder selection from editor; ensure completion callback runs on message thread.
- **Case-insensitive `.syx` filter** — `findChildFiles(..., "*.syx")` on macOS/Windows; optionally normalize extension check with `toLowerCase()`.
- **No new dependencies** — Core + existing SysEx stack only.

### Project context reference

- `Core ↛ GUI` strict [project-context.md § Architecture Invariants]
- Clean Code: methods ≤ 15 lines, classes ≤ 200 lines [CONVENTIONS.md]
- English only in source/comments [project-context.md § Agent Communication]
- Tests: JUCE `UnitTest`, fixtures under `Tests/Fixtures/` [project-context.md § Testing Strategy]
- Do not duplicate `PluginDescriptors` data

### References

- [Source: `_bmad-output/planning-artifacts/epics.md` — Epic 4 Story 4.1]
- [Source: `_bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md` — FR-25]
- [Source: `_bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md` — § Computer Patches combobox states (D-024)]
- [Source: `_bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md` — D-024, D-034a, D-058]
- [Source: `_bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md` — AD-2 PatchFileService, E4 mapping]
- [Source: `Source/Core/MIDI/SysEx/SysExDecoder.h` — validatePatchSysExMessage]
- [Source: `Source/Core/Actions/PatchManagerActionHandler.cpp:80-88` — Epic 4 stub]
- [Source: `Source/GUI/Panels/.../ComputerPatchesPanel.cpp:202-218` — hardcoded combobox placeholder]

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Debug Log References

- Made `SysExDecoder::validatePatchSysExMessage` public so `PatchFileService` can reuse validation without duplicating SysEx tables.
- Added `JUCE_MODAL_LOOPS_PERMITTED=1` to plugin target so `FileChooser::browseForDirectory()` works on the message thread (JUCE 8.0.12).

### Completion Notes List

- Implemented `Core::PatchFileService` with non-recursive `.syx` scan, patch-only validation, sorted valid filenames, and last-scan cache.
- Wired `computerPatchesOpenFolder` through `PatchManagerActionHandler` with injected folder picker; other Computer Patches actions remain Epic 4 stubs.
- Footer messages use `PluginDisplayNames::ComputerPatchesModule::FooterMessages` and `PatchFileServiceFooter::propagateScanResult`.
- `PluginProcessor` owns the service; `PluginEditor` registers native folder picker on attach.
- Added 8 unit tests in `PatchFileServiceTests.cpp`; full `Matrix-Control_Tests` suite passes; `Matrix-Control_Standalone` builds.

### File List

- `Source/Core/Services/PatchFileService.h` (NEW)
- `Source/Core/Services/PatchFileService.cpp` (NEW)
- `Source/Core/Services/PatchFileServiceFooter.h` (NEW)
- `Source/Core/Actions/PatchManagerActionHandler.h` (UPDATE)
- `Source/Core/Actions/PatchManagerActionHandler.cpp` (UPDATE)
- `Source/Core/PluginProcessor.h` (UPDATE)
- `Source/Core/PluginProcessor.cpp` (UPDATE)
- `Source/Core/MIDI/SysEx/SysExDecoder.h` (UPDATE — public validatePatchSysExMessage)
- `Source/GUI/PluginEditor.cpp` (UPDATE)
- `Source/Shared/Definitions/PluginDisplayNames.h` (UPDATE)
- `Tests/Unit/PatchFileServiceTests.cpp` (NEW)
- `Tests/Unit/PatchManagerActionHandlerTests.cpp` (UPDATE)
- `CMakeLists.txt` (UPDATE)

### Change Log

- 2026-06-19: Story 4.1 — PatchFileService folder scan, OPEN action wiring, footer propagation, unit tests.
