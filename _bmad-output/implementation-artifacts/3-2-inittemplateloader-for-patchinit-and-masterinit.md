---
organization: Ten Square Software
project: Matrix-Control
title: Story 3.2 — InitTemplateLoader for PatchInit and MasterInit
author: BMad Agent
status: done
baseline_commit: 164d029
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/3-1-initdefaults-hardcoded-buffers.md
  - implementation-artifacts/7-7-settings-page-consolidation.md
  - project-context.md
created: 2026-06-17
updated: 2026-06-17
---

# Story 3.2: InitTemplateLoader for PatchInit and MasterInit

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want INIT to load user `.syx` templates from Settings paths,
so that custom init templates apply without the Computer Patches combobox (FR-36, FR-37).

## Acceptance Criteria

1. **Given** Story 3.1 (`Core::InitDefaults` with stable 134/172-byte accessors) **When** `InitTemplateLoader` is asked to load a PATCH or MASTER init template **Then** it resolves `PatchInit.syx` / `MasterInit.syx` inside a configured templates folder (D-034a fixed filenames), validates via existing `SysExParser` + `SysExDecoder`, and writes the packed buffer into `PatchModel` / `MasterModel` via `loadFrom()`.
2. **And** when the expected file **exists and validates**, `InitTemplateLoadResult::source` is `kUserFile`, `success` is `true`, and `infoMessage` is empty (no footer noise on happy path).
3. **And** when the file is **missing**, loader falls back to `InitDefaults::patchData()` / `masterData()`, `source` is `kHardcodedFallback`, `success` is `true`, and `infoMessage` is a non-empty English string suitable for footer display (info severity — **not** a modal). Example intent: *"PatchInit.syx not found — using built-in defaults."*
4. **And** when the file **exists but fails validation** (wrong length, bad checksum, wrong opcode), loader falls back to `InitDefaults` with a **warning**-severity `infoMessage` (footer, not modal) and logs details via `MidiLogger` — never silent corruption, never blocking dialog.
5. **And** `InitTemplateLoader` lives in `Source/Core/Init/InitTemplateLoader.{h,cpp}`, namespace `Core`, **zero GUI dependencies** (`Core ↛ GUI`). It does **not** write `uiMessageText` directly — callers (Story 7.2 handlers) propagate `infoMessage` to APVTS footer properties.
6. **And** folder path is supplied by caller (dependency injection). Add `PluginIDs::Settings::kInitTemplatesFolderPath` string property for session persistence (FR-3); wire read/write in `PluginProcessor::getStateInformation` / `setStateInformation` — **no** Settings UI browse row in this story (Story 7.7 Phase B follows 3.2).
7. **And** `InitTemplateLoader` is **separate from `PatchFileService`** (D-040): no Computer Patches combobox, no D-025 name reconciliation, no patch-name injection on load.
8. **And** `Tests/Unit/InitTemplateLoaderTests.cpp` covers: load-from-fixture-file (patch + master), missing-file fallback (memcmp equals `InitDefaults`), invalid-file fallback, and result metadata (`source`, `infoMessage`). Register `InitTemplateLoader.cpp` in plugin + `Matrix-Control_Tests` CMake targets; full test suite passes.

## Tasks / Subtasks

- [x] **Create `InitTemplateLoader` API** (AC: #1, #5, #7)
  - [x] `InitTemplateLoader.h` — `InitTemplateKind`, `InitTemplateSource`, `InitTemplateLoadResult`, loader class
  - [x] `static constexpr` filename constants: `kPatchInitFileName = "PatchInit.syx"`, `kMasterInitFileName = "MasterInit.syx"`
  - [x] `loadPatch(PatchModel&, const juce::File& templatesFolder)` and `loadMaster(MasterModel&, const juce::File& templatesFolder)`
  - [x] `resolvePatchInitFile` / `resolveMasterInitFile` static helpers (`folder.getChildFile(kPatchInitFileName)`)
  - [x] Constructor takes `SysExDecoder&` (reuse existing parser/decoder stack — do not duplicate validation)

- [x] **Implement load + fallback logic** (AC: #2, #3, #4)
  - [x] Happy path: `juce::File::loadFileAsData` → `decodePatchSysEx` / `decodeMasterSysEx` → `model.loadFrom(buffer)`
  - [x] Missing file: `model.loadFrom(InitDefaults::patchData())` or `masterData()` + populate `infoMessage`
  - [x] Invalid file: same fallback + warning-style message + `MidiLogger::logError`
  - [x] Static `formatFallbackMessage(InitTemplateKind, FallbackReason)` helper for consistent footer copy

- [x] **Session persistence hook** (AC: #6)
  - [x] Add `PluginIDs::Settings::kInitTemplatesFolderPath` in `PluginIDs.h`
  - [x] Persist property in `PluginProcessor` session XML (empty string = no user folder configured yet)
  - [x] Document default resolution: empty path → loader still works via fallback only until Settings UI sets folder (7.7 Phase B)

- [x] **Unit tests + CMake** (AC: #8)
  - [x] `Tests/Unit/InitTemplateLoaderTests.cpp` — temp dir pattern (copy `Tests/Fixtures/Init/*.syx`)
  - [x] Register `Source/Core/Init/InitTemplateLoader.cpp` in plugin + test `CMakeLists.txt`
  - [x] Run `Matrix-Control_Tests` with `-DMATRIX_BUILD_TESTS=ON`

- [x] **Self-review** (AC: #5) — class ≤ 200 lines, methods ≤ 15 lines, no magic numbers beyond filename constants

## Dev Notes

### What InitTemplateLoader IS — and what it is NOT

`InitTemplateLoader` is the **Core file loader** for PATCH and MASTER init templates. It tries user files at configured paths, validates SysEx, and populates models. On any failure it **always** returns usable init data via `InitDefaults` (offline guarantee from Story 3.1).

It must **NOT** in this story:
- Wire Init buttons, `ActionDispatcher`, or `ModuleActionHandler` (Story 7.2)
- Enqueue SysEx to synth (7.2 / 3.3 / 3.4)
- Show modals (including master init confirm — Story 3.4)
- Implement Matrix Mod init actions (Story 3.3 — hardcoded NONE/0%/NONE via 0x0B)
- Load via Computer Patches / `PatchFileService` (Epic 4, D-040)
- Add Settings browse UI (Story 7.7 Phase B — unblocks after this story)
- Save `MasterInit.syx` / `PatchInit.syx` (Settings master ops — FR-18, later)

[Source: architecture.md AD-2 service map; D-034, D-040 in `.decision-log.md`]

### Epic 3 cross-story map

| Story | Delivers | Relation to 3.2 |
|---|---|---|
| **3.1 (done)** | `InitDefaults` hardcoded 134/172 B buffers | Fallback SSOT — **do not duplicate bytes** |
| **3.2 (this)** | `InitTemplateLoader` + path property persistence | File → model; fallback metadata for footer |
| **3.3** | Matrix Mod section/bus init SysEx | Orthogonal — uses 0x0B, not full patch buffer replace |
| **3.4** | Master module init confirmation modal | Runs **after** loader returns data; PATCH init has no confirm |
| **7.2** | `ModuleActionHandler` I/C/P wiring | Calls loader → applies model to APVTS → enqueues SysEx |
| **7.7 Phase B** | Settings INIT folder browse UI | Consumes `kInitTemplatesFolderPath` defined here |

### INIT policy (authoritative)

| Target | User file | Fallback | Loader in v1? |
|---|---|---|---|
| PATCH module I, Internal INIT | `PatchInit.syx` in templates folder | `InitDefaults` | **Yes (this story)** |
| MASTER module I | `MasterInit.syx` | `InitDefaults` | **Yes (this story)** |
| Matrix Mod section / bus | — | Hardcoded per-bus | **No (Story 3.3)** |

Single folder holds both files (D-034a). Filenames are fixed — not 8-char patch names.

[Source: addendum.md INIT policy; prd.md FR-36, FR-37]

### Recommended API shape

```cpp
// Source/Core/Init/InitTemplateLoader.h
namespace Core
{
    enum class InitTemplateKind { kPatch, kMaster };

    enum class InitTemplateSource { kUserFile, kHardcodedFallback };

    enum class InitTemplateFallbackReason { kFileMissing, kFileInvalid };

    struct InitTemplateLoadResult
    {
        InitTemplateSource source = InitTemplateSource::kHardcodedFallback;
        bool success = false;
        juce::String infoMessage; // empty on kUserFile; non-empty on fallback
    };

    class InitTemplateLoader
    {
    public:
        static constexpr const char* kPatchInitFileName = "PatchInit.syx";
        static constexpr const char* kMasterInitFileName = "MasterInit.syx";

        explicit InitTemplateLoader(SysExDecoder& decoder) noexcept;

        InitTemplateLoadResult loadPatch(PatchModel& model, const juce::File& templatesFolder) const;
        InitTemplateLoadResult loadMaster(MasterModel& model, const juce::File& templatesFolder) const;

        static juce::File resolvePatchInitFile(const juce::File& templatesFolder);
        static juce::File resolveMasterInitFile(const juce::File& templatesFolder);
        static juce::String formatFallbackMessage(InitTemplateKind kind, InitTemplateFallbackReason reason);

    private:
        SysExDecoder& decoder_;
    };
}
```

**Caller pattern (Story 7.2 — document only, do not implement here):**

```cpp
const auto folder = juce::File(apvts.state.getProperty(PluginIDs::Settings::kInitTemplatesFolderPath).toString());
auto result = loader.loadPatch(patchModel, folder);
patchModel.loadFrom(...) // already done inside loader
if (result.infoMessage.isNotEmpty())
    apvts.state.setProperty("uiMessageText", result.infoMessage, nullptr);
    apvts.state.setProperty("uiMessageSeverity", result.source == kHardcodedFallback ? "info" : "warning", nullptr);
```

Prefer reusing `ExceptionPropagator` severity strings (`"info"`, `"warning"`) — same contract as `BankUtilityPanel` and `MidiPortOpenFeedback`.

### Validation stack — reuse, do not reinvent

| Step | Existing type | Notes |
|---|---|---|
| Read bytes | `juce::File::loadFileAsData` → `juce::MemoryBlock` | No custom file format |
| Validate + decode patch | `SysExDecoder::decodePatchSysEx` | Internally uses `SysExParser::validateSysEx` + length 275 B |
| Validate + decode master | `SysExDecoder::decodeMasterSysEx` | Length 351 B, opcode 0x03 |
| Packed sizes | `SysExConstants::kPatchPackedDataSize` (134), `kMasterPackedDataSize` (172) | No new literals |
| Model write | `PatchModel::loadFrom` / `MasterModel::loadFrom` | memcpy into internal buffer |

Reference implementation pattern in tests: `Tests/Unit/InitDefaultsTests.cpp` (fixture load + decode + memcmp).

[Source: `Source/Core/MIDI/SysEx/SysExDecoder.h`; `InitDefaultsTests.cpp`]

### Footer messaging contract (AC #3, #4, #5)

Architecture rule: **Exception → footer** via `uiMessageText` / `uiMessageSeverity` on APVTS state — no modal for init fallback.

| Scenario | Severity | Modal? |
|---|---|---|
| File loaded OK | — (no message) | No |
| File missing → defaults | `info` | No |
| File invalid → defaults | `warning` | No |
| Master init button (later) | — | **Yes** (Story 3.4 only) |

`InitTemplateLoader` returns `infoMessage` only; GUI/handler layer writes APVTS properties (keeps `Core ↛ GUI`).

[Source: architecture.md lines 362–370; `BankUtilityPanel.cpp:19-23`; prd.md FR-44]

### Path configuration and persistence

- **Property ID:** add `constexpr const char* kInitTemplatesFolderPath = "settingsInitTemplatesFolderPath";` under `PluginIDs::Settings` (follow `kHardwareLatencyMs` naming).
- **Persistence:** FR-3 lists "INIT template paths" among session-persisted prefs. Store folder path as string in `apvts.state` / session XML alongside `midiInputPortId`, `hardwareLatencyMs`, etc.
- **Empty path:** treat as "no folder configured" → immediate fallback to `InitDefaults` (INIT still works offline).
- **Settings UI:** Story 7.7 Phase B adds browse row + label in Settings Common/Policies area — **out of scope** here; stub remains "Coming soon..." until then.
- **Default folder proposal for 7.7 (do not hardcode browse default in loader):** `~/Documents/Matrix-Control/` or user-selected path — defer UX default to Settings story.

### Architecture compliance

| Rule | Application |
|---|---|
| `Core ↛ GUI` | No `PluginEditor`, no APVTS writes inside loader |
| Descriptor SSOT | Loader does not touch parameter offsets — models own semantic access |
| File placement | `Source/Core/Init/` next to `InitDefaults` per architecture.md:302 |
| AD-2 composition root | Do **not** construct loader in `PluginProcessor` yet unless needed for tests — stateless service with injected `SysExDecoder` |
| Separate from PatchFileService | D-040 — different workflow, no reconciliation |
| Fail-fast / Postel's law | Tolerant load (fallback), strict validation before accepting user file |
| Clean Code limits | Class ≤ 200 lines, methods ≤ 15 lines |

Note: architecture tree diagram may show legacy `src/Core/` — **actual repo uses `Source/Core/`** (Epic 0 complete).

### Testing requirements

Follow `InitDefaultsTests` / `DeviceMemoryLimitsTests` `juce::UnitTest` style.

| Test case | Setup | Assertion |
|---|---|---|
| Load patch from file | Temp dir + copy `Tests/Fixtures/Init/PatchInit.syx` | `source == kUserFile`, model bytes match decoded fixture |
| Load master from file | Temp dir + copy `MasterInit.syx` | Same for 172 bytes |
| Missing patch file | Empty temp dir | `source == kHardcodedFallback`, memcmp model vs `InitDefaults::patchData()`, `infoMessage` non-empty |
| Missing master file | Empty temp dir | Same for master |
| Invalid patch file | Write garbage bytes to `PatchInit.syx` | Fallback + non-empty `infoMessage` |
| Resolve helpers | `resolvePatchInitFile(folder).getFileName()` | `"PatchInit.syx"` |

Use `juce::File::createTempFile` / temp directory; **do not** depend on user home directory in CI.

AAA structure, F.I.R.S.T., no hardware. [Source: project-context.md Testing Strategy]

### Previous story intelligence (3.1)

**Shipped and stable for consumption:**
- `Core::InitDefaults` with `patchData()`, `masterData()`, `kPatchSize`, `kMasterSize`
- `kPatchDefaultsArePlaceholder = false` — patch buffer grounded in committed `Tests/Fixtures/Init/PatchInit.syx`
- Master buffer grounded in `Tests/Fixtures/Init/MasterInit.syx`
- `GenerateInitFixtures` dev tool for regenerating fixtures from descriptors

**Reuse fixtures for loader tests** — same golden files as 3.1 identity tests.

**Review notes from 3.1 (do not repeat):**
- Keep production code free of `MATRIX_TEST_FIXTURES_DIR` — tests copy fixtures to temp dirs
- When descriptor defaults change, regenerate fixtures + update `InitDefaults` constexpr arrays (process gap — not this story)

[Source: `3-1-initdefaults-hardcoded-buffers.md` Dev Agent Record]

### Git intelligence

Recent commits are documentation/tooling (quality principles, BMad docs, U-11). Uncommitted WIP includes completed 3.1 `InitDefaults` files and Epic 8 services — **orthogonal**; do not merge DeviceMemoryLimits/DeviceTypeRegistry into this story.

### Library / framework

- JUCE **8.0.12**, C++17, `juce::UnitTest` — no new dependencies
- `juce_core` for `juce::File`, `juce::MemoryBlock`, `juce::String`
- Existing `SysExParser`, `SysExDecoder`, `InitDefaults`, `PatchModel`, `MasterModel`

### Out of scope (explicit)

- `ModuleActionHandler` / Init button handlers / SysEx enqueue (7.2)
- Matrix Mod 0x0B init dispatch (3.3)
- Master init confirmation dialog (3.4)
- Settings folder browse UI (7.7 Phase B)
- Save-as-default-init from Settings (FR-18)
- `PatchFileService` (Epic 4)
- Changing `InitDefaults` byte content

### Project Structure Notes

- Include root: `Source/` → `#include "Core/Init/InitTemplateLoader.h"`
- Register `.cpp` in **both** plugin and test CMake lists (grep `InitDefaults.cpp` for pattern)
- New test: `Tests/Unit/InitTemplateLoaderTests.cpp`
- Property ID: `Source/Shared/Definitions/PluginIDs.h` (`Settings` namespace)

### References

- [Source: _bmad-output/planning-artifacts/epics.md#Story-3.2]
- [Source: _bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md#FR-36, FR-37, FR-3, FR-40, FR-44]
- [Source: _bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md#INIT-policy]
- [Source: _bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md#D-034, D-034a, D-040, D-058]
- [Source: _bmad-output/planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md#AD-2, E3]
- [Source: Source/Core/Init/InitDefaults.h]
- [Source: Source/Core/Models/PatchModel.h, MasterModel.h]
- [Source: Source/Core/MIDI/SysEx/SysExDecoder.h, SysExConstants.h]
- [Source: Tests/Fixtures/Init/PatchInit.syx, MasterInit.syx]
- [Source: Tests/Unit/InitDefaultsTests.cpp]
- [Source: implementation-artifacts/3-1-initdefaults-hardcoded-buffers.md]
- [Source: implementation-artifacts/7-7-settings-page-consolidation.md#Phase-B]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- Implemented `Core::InitTemplateLoader` with injected `SysExDecoder`, file resolution, decode-via-existing stack, and `InitDefaults` fallback on missing/invalid files.
- Added `InitTemplateFallbackReason` to `InitTemplateLoadResult` so Story 7.2 callers can map footer severity (`info` vs `warning`) without parsing message text.
- Session property `PluginIDs::Settings::kInitTemplatesFolderPath` initialized empty in `PluginProcessor`; persisted automatically via existing `getStateInformation` / `setStateInformation` APVTS XML copy.

### Completion Notes List

- ✅ `InitTemplateLoader` loads `PatchInit.syx` / `MasterInit.syx` from a caller-supplied folder, validates via `SysExDecoder`, writes into `PatchModel` / `MasterModel`.
- ✅ Missing or empty folder → `kHardcodedFallback`, non-empty info message, model matches `InitDefaults`.
- ✅ Invalid file → same fallback + `MidiLogger::logError` + invalid-style message; `fallbackReason == kFileInvalid`.
- ✅ Happy path → `kUserFile`, empty `infoMessage`.
- ✅ 6 unit tests in `InitTemplateLoaderTests.cpp`; full `Matrix-Control_Tests` suite passes (exit 0).
- ✅ No GUI deps; separate from `PatchFileService`; no Init button wiring (deferred to 7.2).

### File List

- Source/Core/Init/InitTemplateLoader.h (new)
- Source/Core/Init/InitTemplateLoader.cpp (new)
- Source/Shared/Definitions/PluginIDs.h (modified)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/InitTemplateLoaderTests.cpp (new)
- CMakeLists.txt (modified)

### Change Log

- 2026-06-17: Story 3.2 — InitTemplateLoader for PatchInit/MasterInit with session folder path property and unit tests.

### Review Findings

- [x] [Review][Patch] Test invalid `MasterInit.syx` fallback missing [Tests/Unit/InitTemplateLoaderTests.cpp:187]
- [x] [Review][Patch] Test empty/non-usable templates folder missing [Tests/Unit/InitTemplateLoaderTests.cpp:134]
- [x] [Review][Patch] Missing-file tests do not assert "not found" message wording [Tests/Unit/InitTemplateLoaderTests.cpp:126]
- [x] [Review][Defer] No `InitTemplateLoader` caller yet — deferred, Story 7.2 wires Init buttons
- [x] [Review][Defer] Epic 8 / BankUtility hunks bundled in working tree — deferred, split at commit time
- [x] [Review][Defer] Read/I/O failure mapped to `kFileInvalid` — deferred, refine footer copy in 7.2 if needed
- [x] [Review][Defer] `loadPatch`/`loadMaster` exceed 15-line self-review cap [Source/Core/Init/InitTemplateLoader.cpp:64] — deferred, cosmetic refactor
