---
organization: Ten Square Software
project: Matrix-Control
title: Story 3.3 — Matrix Mod Init Defaults
author: BMad Agent
status: review
baseline_commit: 5ba775c
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md
  - implementation-artifacts/2-10-matrix-mod-bus-reorder-sysex.md
  - implementation-artifacts/3-1-initdefaults-hardcoded-buffers.md
  - implementation-artifacts/3-2-inittemplateloader-for-patchinit-and-masterinit.md
  - project-context.md
created: 2026-06-17
updated: 2026-06-17
---

# Story 3.3: Matrix Mod Init Defaults

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want section and per-bus Matrix Mod init to reset to NONE/0%/NONE,
so that modulation matrix clears predictably (FR-15).

## Acceptance Criteria

1. **Given** Story 2.6 (`MatrixModBusParameterSysExDispatcher`, `encodeMatrixModBusEdit` 0x0B) and Story 2.10 (`pushBusToApvts`, `suppressMatrixModParameterSysEx_` pattern) **When** `Core::MatrixModInitService` resets one or all buses **Then** each targeted bus is set to hardcoded defaults: SOURCE choice index **0** (NONE), AMOUNT **0** (0%), DESTINATION choice index **0** (NONE) in `PatchModel` bytes `104–133`.
2. **And** after model reset, `ApvtsPatchMapper::pushBusToApvts` updates APVTS for affected buses while SysEx coalescing is suppressed (same flag pattern as `swapMatrixModBusContents`) — no spurious per-parameter 0x0B from `valueTreePropertyChanged`.
3. **And** after APVTS push, the service enqueues **exactly one 0x0B per affected bus** via `MatrixModBusParameterSysExDispatcher::dispatchBus` with post-reset values (`source=0`, `amount=0`, `destination=0`). Section init → **10** messages in ascending bus order `0..9`. Per-bus init → **1** message for that bus index.
4. **And** no user INIT file is read — Matrix Mod init is **hardcoded v1** per D-034 / FR-15. Do **not** route through `InitTemplateLoader` or `InitDefaults` patch buffer replace.
5. **And** `MatrixModInitService` lives in `Source/Core/Init/MatrixModInitService.{h,cpp}`, namespace `Core`, **zero GUI dependencies** (`Core ↛ GUI`).
6. **And** existing GUI stubs already stamp APVTS state properties on click (`matrixModulationInit`, `modulationBus0Init` … `modulationBus9Init`). `PluginProcessor::valueTreePropertyChanged` routes those property changes to the init service (brownfield handler like `handlePatchManagerPropertyChange`) until Story 7.2 migrates to `ModuleActionHandler`. **No** new GUI code in this story.
7. **And** `Tests/Unit/MatrixModInitServiceTests.cpp` covers: single-bus reset (model + queue), section init all 10 buses (model + 10 queue messages with correct `<bus>` bytes and zero triplets), invalid bus index no-op. Register `.cpp` in plugin + `Matrix-Control_Tests` CMake targets; full test suite passes.

## Tasks / Subtasks

- [x] **Create `MatrixModInitService` API** (AC: #1, #4, #5)
  - [x] `MatrixModInitService.h` — hardcoded default constants, `initBus(int busIndex)`, `initAllBuses()`
  - [x] Constructor takes `PatchModel&`, `ApvtsPatchMapper&`, `MatrixModBusParameterSysExDispatcher&` (mirror `MatrixModBusReorderService` deps)
  - [x] `static constexpr` defaults: `kDefaultSourceIndex = 0`, `kDefaultAmount = 0`, `kDefaultDestinationIndex = 0` — document alignment with `PluginDescriptorsMatrixModulation.cpp` defaultValue fields and FR-15

- [x] **Implement reset + SysEx burst** (AC: #2, #3)
  - [x] Private `resetBusInModel(int busIndex)` using descriptor tables from `PluginDescriptors::MatrixModulationSection` (same triplet read/write pattern as `MatrixModBusReorderService::swapBusContents`)
  - [x] `initBus`: reset model → `pushBusToApvts(bus)` → `dispatchBus(bus)`
  - [x] `initAllBuses`: loop buses `0..kModulationBusCount-1` — reset all in model first, then push each bus, then dispatch each bus in ascending order
  - [x] Validate bus index via `Matrix1000Limits::kModulationBusCount`; invalid index → no-op

- [x] **Wire `PluginProcessor` handler** (AC: #6)
  - [x] Add `handleMatrixModInitPropertyChange(const juce::String& propertyId)` + `parseMatrixModBusInitIndex(propertyId)` helper
  - [x] Map `kMatrixModulationInit` → `initAllBuses()`; `kBus0Init`…`kBus9Init` → `initBus(n)`
  - [x] Wrap service calls with `suppressMatrixModParameterSysEx_ = true/false` (identical to `swapMatrixModBusContents`)
  - [x] Construct `matrixModInitService_` in processor init alongside `matrixModBusReorderService_`
  - [x] Call handler from `valueTreePropertyChanged` after existing dispatch paths

- [x] **Unit tests + CMake** (AC: #7)
  - [x] `Tests/Unit/MatrixModInitServiceTests.cpp` — reuse harness pattern from `MatrixModBusReorderServiceTests` (`TestAudioProcessor`, `EditorPath`, queue capture)
  - [x] Assert packed bytes at offsets `104+3n`, `105+3n`, `106+3n` are `0` after init
  - [x] Assert queue message count and `encodeMatrixModBusEdit` byte layout (`F0 10 06 0B <bus> 00 00 00 F7`)
  - [x] Register `Source/Core/Init/MatrixModInitService.cpp` in plugin + test `CMakeLists.txt`

- [x] **Self-review** (AC: #5) — class ≤ 200 lines, methods ≤ 15 lines, no duplicate offset tables

## Dev Notes

### What MatrixModInitService IS — and what it is NOT

`MatrixModInitService` is the **Core reset service** for Matrix Modulation buses. It applies hardcoded NONE/0%/NONE defaults to `PatchModel`, syncs APVTS, and enqueues 0x0B SysEx per affected bus.

It must **NOT** in this story:
- Load `PatchInit.syx` or touch `InitTemplateLoader` / `InitDefaults` for Matrix Mod actions (D-034 — Matrix Mod has no user INIT file v1)
- Replace the full 134-byte patch buffer (0x01) — use per-bus 0x0B only, same as parameter edits and reorder
- Add confirmation dialog (Master init only — Story 3.4)
- Implement `ActionDispatcher` / `ModuleActionHandler` (Story 7.2 will refactor the processor handler into the hub)
- Change GUI panels or `ModulationBusCell` / `MatrixModulationPanel` (buttons already stamp timestamp properties)
- Implement PATCH/MASTER module I/C/P init (Stories 3.2 loader + 7.2 handler)

[Source: D-034 in `.decision-log.md`; FR-15 in `prd.md`]

### Epic 3 cross-story map

| Story | Delivers | Relation to 3.3 |
|---|---|---|
| **3.1 (done)** | `InitDefaults` patch buffer with cleared bytes 104–133 | Coherent full-patch INIT fallback only — **not** the runtime Matrix Mod init action path |
| **3.2 (done)** | `InitTemplateLoader` for PATCH/MASTER files | Orthogonal — no Matrix Mod file |
| **3.3 (this)** | `MatrixModInitService` + processor wiring | Hardcoded bus reset + 0x0B burst |
| **3.4** | Master module init confirmation modal | PATCH/Master init — not Matrix Mod |
| **7.2** | `ModuleActionHandler` | Will call this service; migrate handler out of `PluginProcessor` |

### INIT policy (authoritative — D-034)

| Target | User file | Fallback | v1 action |
|---|---|---|---|
| PATCH module I, Internal INIT | `PatchInit.syx` | `InitDefaults` | `InitTemplateLoader` (3.2) |
| MASTER module I | `MasterInit.syx` | `InitDefaults` | `InitTemplateLoader` (3.2) |
| **Matrix Mod section + per-bus** | **None** | **Hardcoded per bus** | **`MatrixModInitService` (this story)** |

### Hardcoded default values

| Field | APVTS value | Packed byte | Oberheim meaning |
|---|---|---|---|
| SOURCE | choice index `0` | `0` | NONE (Table 2 code 0) |
| AMOUNT | int `0` | `0` | 0% (signed byte 0) |
| DESTINATION | choice index `0` | `0` | NONE (Table 3 code 0) |

Choice index equals Oberheim Table 2/3 code (Story 2.6). Descriptors already use `defaultValue = 0` for all three fields per bus in `PluginDescriptorsMatrixModulation.cpp`.

Matrix Mod packed layout (bytes 104–133):

| Bus | Source | Amount | Destination |
|---|---|---|---|
| n | `104 + 3n` | `105 + 3n` | `106 + 3n` |

[Source: `2-6-matrix-mod-bus-parameter-sysex.md`; `3-1-initdefaults-hardcoded-buffers.md`]

### Recommended API shape

```cpp
// Source/Core/Init/MatrixModInitService.h
namespace Core
{
    class MatrixModInitService
    {
    public:
        static constexpr int kDefaultSourceIndex = 0;
        static constexpr int kDefaultAmount = 0;
        static constexpr int kDefaultDestinationIndex = 0;

        MatrixModInitService(PatchModel& patchModel,
                             ApvtsPatchMapper& apvtsPatchMapper,
                             const MatrixModBusParameterSysExDispatcher& sysExDispatcher);

        void initBus(int busIndex);
        void initAllBuses();

    private:
        void resetBusInModel(int busIndex);
        static bool isValidBusIndex(int busIndex) noexcept;

        PatchModel& patchModel_;
        ApvtsPatchMapper& apvtsPatchMapper_;
        const MatrixModBusParameterSysExDispatcher& sysExDispatcher_;
    };
}
```

**Processor wiring pattern (this story — 7.2 will extract):**

```cpp
void PluginProcessor::handleMatrixModInitPropertyChange(const juce::String& propertyId)
{
    using namespace PluginIDs::MatrixModulationSection;

    if (matrixModInitService_ == nullptr)
        return;

    suppressMatrixModParameterSysEx_ = true;

    if (propertyId == StandaloneWidgets::kMatrixModulationInit)
        matrixModInitService_->initAllBuses();
    else if (const int bus = parseMatrixModBusInitIndex(propertyId); bus >= 0)
        matrixModInitService_->initBus(bus);

    suppressMatrixModParameterSysEx_ = false;
}
```

Call from `valueTreePropertyChanged` — standalone widget properties resolve to their widget ID string (not `ApvtsTypes::kValue`), same as bank buttons.

### SysEx message (unchanged from Story 2.6)

```
F0 10 06 0B <bus> <source> <amount> <destination> F7
```

After init bus 3: single message with `<bus>=3`, all data bytes `0x00`.

After section init: **10** messages, `<bus>` = `0, 1, …, 9` in order. Queue inter-message delay (Story 2.2) applies automatically via `MidiManager` consumer.

**Do not** route through `MatrixModSysExCoalesceTimer` — call `dispatchBus` directly (init is an explicit burst, like reorder).

### Suppressing duplicate 0x0B during APVTS push (critical)

`pushBusToApvts` updates 3 parameters per bus via `setValueNotifyingHost` → triggers `valueTreePropertyChanged` → would enqueue via coalesce timer without suppression.

**Required:** wrap init service calls with `suppressMatrixModParameterSysEx_` exactly as `PluginProcessor::swapMatrixModBusContents` does today.

[Source: `2-10-matrix-mod-bus-reorder-sysex.md` §Suppressing duplicate 0x0B]

### Brownfield GUI stubs (already shipped — do not modify)

| UI control | Property stamped | File |
|---|---|---|
| Section "I" button | `matrixModulationInit` | `MatrixModulationPanel.cpp:241` |
| Per-bus "I" button | `modulationBus0Init` … `modulationBus9Init` | `ModulationBusCell.cpp:126-141` |

Pattern: `apvts_.state.setProperty(widgetId, juce::Time::getMilliseconds(), nullptr)` — timestamp value is irrelevant; property **change** is the trigger.

Widget IDs registered in `PluginDescriptorsMatrixModulation.cpp` and `ApvtsFactory::getAllStandaloneWidgets()`.

### Architecture compliance

| Rule | Application |
|---|---|
| `Core ↛ GUI` | Service has no editor/APVTS writes — processor passes suppress flag around mapper push |
| Descriptor SSOT | Bus offsets from `PluginDescriptors::MatrixModulationSection` only |
| File placement | `Source/Core/Init/` alongside `InitDefaults`, `InitTemplateLoader` (architecture E3) |
| AD-4 routing | Matrix Mod edits use 0x0B only — no 0x06×3, no full 0x01 |
| D-034 | No Matrix Mod INIT file — hardcoded constants in service |
| Clean Code limits | Class ≤ 200 lines, methods ≤ 15 lines |

### Testing requirements

Follow `MatrixModBusReorderServiceTests` harness (`TestAudioProcessor` + minimal APVTS layout for one bus or all buses).

| Test case | Setup | Assertion |
|---|---|---|
| `initBus` active bus | Pre-fill bus 2 with non-zero triplet | Model bytes 110–112 = 0; APVTS params match; queue has 1 message, bus byte = 2 |
| `initBus` invalid index | `busIndex = 99` | No model change, no enqueue |
| `initAllBuses` | Pre-fill multiple buses | All bytes 104–133 = 0; queue size = 10; bus bytes 0..9 ascending |
| SysEx payload | Capture queue | `memcmp` first 4 bytes `F0 10 06 0B`, triplet bytes all `00` |
| APVTS coherence | After init with suppress off | Raw parameter values = default indices |

AAA structure, F.I.R.S.T., no hardware. Optional SM-1 manual verify after Epic 2 UAT — not blocking.

### Previous story intelligence

**Story 2.6 (done):** `encodeMatrixModBusEdit`, `MatrixModBusParameterSysExDispatcher`, `dispatchBus(int)`, `MidiManager::enqueueMatrixModBusEdit`. Choice index = Oberheim code; amount byte unmasked.

**Story 2.10 (done):** `pushBusToApvts(int)`, `suppressMatrixModParameterSysEx_`, triplet read/write via descriptors. **Copy this service structure verbatim** — init is simpler (write constants, no swap).

**Story 3.1 (done):** Patch `InitDefaults` buffer clears bytes 104–133 — ensures full-patch INIT fallback is coherent, but Matrix Mod button actions must **not** call `InitDefaults` / full `loadFrom`.

**Story 3.2 (done):** `InitTemplateLoader` is PATCH/MASTER only. Review note: no caller wired yet (deferred 7.2) — Matrix Mod init should wire in 3.3 because GUI stubs already exist and FR-15 requires functional reset.

### Git intelligence

Recent commits:
- `5ba775c` — Init SysEx fixtures (`PatchInit.syx`, `MasterInit.syx`) committed; supports 3.1/3.2, orthogonal to 3.3
- `3a73663` — French code-review communication rule (agent-facing)
- Epic 3 `InitDefaults` + `InitTemplateLoader` implemented on branch (stories 3.1–3.2 done per sprint status)

No conflicting Matrix Mod init work in flight. Reuse uncommitted/readily available `MatrixModBusReorderService` as the structural template.

### Library / framework

- JUCE **8.0.12**, C++17, `juce::UnitTest` — no new dependencies
- Reuse `PatchModel`, `ApvtsPatchMapper`, `MatrixModBusParameterSysExDispatcher`, `SysExEncoder`, `Matrix1000Limits`

### Out of scope (explicit)

- `ActionDispatcher` / `ModuleActionHandler` refactor (7.1, 7.2)
- PATCH/MASTER module Init button handlers (7.2 + 3.2 loader)
- Master init confirmation dialog (3.4)
- Settings "Init all Master modules" (FR-18, 7.7)
- Full-patch 0x01 SysEx on section init — per-bus 0x0B only
- Changing `InitDefaults` byte content or `InitTemplateLoader`
- GUI styling / layout (Epic U)

### Project Structure Notes

- Include root: `Source/` → `#include "Core/Init/MatrixModInitService.h"`
- Register `.cpp` in **both** plugin and test CMake lists (grep `InitTemplateLoader.cpp` for pattern)
- New test: `Tests/Unit/MatrixModInitServiceTests.cpp`
- Property IDs: `Source/Shared/Definitions/PluginIDs.h` (`MatrixModulationSection` — **no new IDs**)

### References

- [Source: _bmad-output/planning-artifacts/epics.md#Story-3.3]
- [Source: _bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md#FR-15]
- [Source: _bmad-output/planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md#D-034]
- [Source: _bmad-output/planning-artifacts/architecture/.../architecture.md#E3, AD-4]
- [Source: implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md]
- [Source: implementation-artifacts/2-10-matrix-mod-bus-reorder-sysex.md]
- [Source: implementation-artifacts/3-1-initdefaults-hardcoded-buffers.md]
- [Source: implementation-artifacts/3-2-inittemplateloader-for-patchinit-and-masterinit.md]
- [Source: Source/Core/MIDI/MatrixModBusReorderService.cpp — triplet pattern]
- [Source: Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.h — dispatchBus]
- [Source: Source/Core/PluginProcessor.cpp — suppressMatrixModParameterSysEx_, swapMatrixModBusContents]
- [Source: Source/GUI/Widgets/ModulationBusCell.cpp — per-bus init stub]
- [Source: Source/GUI/Panels/.../MatrixModulationPanel.cpp — section init stub]
- [Source: Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp — defaults]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- Built with `cmake --preset default-macos-arm64`; full `Matrix-Control_Tests` suite passes (exit 0).

### Completion Notes List

- Added `Core::MatrixModInitService` — hardcoded NONE/0%/NONE reset per bus via descriptor SSOT; `initBus` / `initAllBuses` with model → APVTS push → `dispatchBus` burst.
- Wired `PluginProcessor::handleMatrixModInitPropertyChange` for section (`matrixModulationInit`) and per-bus (`modulationBusNInit`) stubs; suppresses coalesced 0x0B during APVTS push.
- Five unit tests cover single-bus, invalid index no-op, section init (10 messages), SysEx layout, APVTS coherence.

### File List

- Source/Core/Init/MatrixModInitService.h (new)
- Source/Core/Init/MatrixModInitService.cpp (new)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/MatrixModInitServiceTests.cpp (new)
- CMakeLists.txt (modified)

### Change Log

- 2026-06-17: Story 3.3 — Matrix Mod init defaults service, processor wiring, unit tests.

### Review Findings

- [x] [Review][Decision] Diff `PluginProcessor.cpp` mélange story 3-3 et d'autres travaux — **Résolu 2026-06-17 : option 1** — valider 3-3 isolément (service + handler Matrix Mod) ; le bundle patch manager / 8-5 sera revu sous sa propre story.

- [x] [Review][Patch] Test suppress manquant pour AC #2 [Tests/Unit/MatrixModInitServiceTests.cpp] — **Corrigé 2026-06-17** — `ProcessorPathHarness` + `testProcessorPathInitWithApvtsListener`.

- [x] [Review][Patch] Assertion partielle sur index bus invalide [Tests/Unit/MatrixModInitServiceTests.cpp:213-226] — **Corrigé 2026-06-17** — vérifie source, amount et destination.

- [x] [Review][Defer] Flag suppress sans RAII [PluginProcessor.cpp:1286-1293] — Même pattern que `swapMatrixModBusContents` (l.529-531) ; JUCE ne lance pas d'exception sur ce chemin ; refactor scope guard = dette transversale.

- [x] [Review][Defer] Chaîne if hardcodée `parseMatrixModBusInitIndex` [PluginProcessor.cpp:1255-1271] — Miroir de `parseBankButtonIndex` ; `kModulationBusCount` = 10 figé v1 ; `static_assert` ou boucle = amélioration future si variant multi-bus.

- [x] [Review][Defer] `sendProgramChange` dans `handlePatchNumberChange` [PluginProcessor.cpp:1180-1182] — Hors scope AC 3-3 ; appartient au bundle patch manager / story 8-5 dans le même diff.

- [x] [Review][Defer] Littéral `"deviceDetected"` [PluginProcessor.cpp:1128-1129] — Hors scope 3-3 ; constante nommée à traiter avec le travail device-type / 8-5.
