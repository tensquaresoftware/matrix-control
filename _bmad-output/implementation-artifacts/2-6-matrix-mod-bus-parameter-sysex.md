---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.6 — Matrix Mod Bus Parameter SysEx
author: BMad Agent
status: done
baseline_commit: 2e21abf
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - implementation-artifacts/2-4-apvts-patch-parameter-to-sysex-0x06.md
  - implementation-artifacts/2-5-apvts-master-parameter-to-full-master-sysex-0x03.md
  - _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - project-context.md
created: 2026-06-05
---

# Story 2.6: Matrix Mod Bus Parameter SysEx

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a sound designer,
I want Matrix Mod bus edits to update SOURCE, AMOUNT, and DESTINATION together,
so that one bus change sends one coherent SysEx block (FR-14, PRD §9 #3).

## Acceptance Criteria

1. **Opcode decision (PRD §9 #3):** Use Oberheim **0x0B** — Remote Parameter Edit (Matrix Modulation). Message: `F0 10 06 0B <bus> <source> <amount> <destination> F7` (8 bytes). **Do not** send three separate 0x06 messages — FR-14 requires one block. Document in Dev Notes: 0x06×3 alternative rejected per Oberheim MD §0BH and D-044; Matrix-6/6R expected compatible (same PATCH buffer layout) — flag **SM-1 manual verify** on hardware before closing epic.
2. **`SysExEncoder::encodeMatrixModBusEdit(bus, source, amount, destination)`** builds the 8-byte frame. `bus` = 0–9; `source`/`destination` = packed buffer bytes at bus offsets (Oberheim Table 2/3 codes, choice index 0 = NONE/delete); `amount` = packed signed byte (pass through unchanged — same rule as Story 2.4 for signed fields).
3. **`Core::MatrixModBusParameterSysExDispatcher`** in `Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.{h,cpp}` — namespace `Core`, zero GUI. Maps any of the 30 Matrix Mod `parameterId`s → bus index 0–9. On dispatch, reads **all three** fields (source, amount, destination) from `PatchModel` at offsets from `PluginDescriptors::MatrixModulationSection::kModulationBus*Parameters[bus]` and enqueues one 0x0B via `EditorPath`.
4. **`ApvtsPatchMapper` extended** to include Matrix Mod bus int/choice descriptors (bytes 104–133). **Critical gap today:** Matrix Mod params are **not** synced to `PatchModel` — without this, dispatch reads stale buffer bytes.
5. **`PluginProcessor::valueTreePropertyChanged`** — for Matrix Mod params: after `apvtsPatchMapper_->apvtsToBuffer()`, call `matrixModBusParameterSysExDispatcher_->dispatch(parameterId)` instead of `patchParameterSysExDispatcher_` (which correctly skips `kNoSysExId` today but does not send 0x0B). **Not** on `valueTreeRedirected`. **Not** GUI panels.
6. PATCH edit params continue 0x06 only; MASTER continues 0x03 only (Stories 2.4–2.5 unchanged).
7. Unit tests: `SysExEncoderTests` — 0x0B byte layout, signed amount >127; `MatrixModBusParameterSysExDispatcherTests` — change source / amount / destination on same bus → identical 8-byte queue message with all three fields; bus 3 → `bus` byte 3; unknown id → no enqueue.
8. `CMakeLists.txt` — new `.cpp` in plugin + test targets; all existing tests pass.

## Tasks / Subtasks

- [x] **Add `encodeMatrixModBusEdit` to `SysExEncoder`** (AC: #1, #2)
  - [x] Declare in `SysExEncoder.h`; implement in `SysExEncoder.cpp`
  - [x] Add `kMatrixModBusMessageLength = 8` to `SysExConstants.h` if helpful
  - [x] Use `SysExConstants::Opcode::kRemoteParameterEditMatrix` (already `0x0B`)

- [x] **Extend `ApvtsPatchMapper` for Matrix Mod** (AC: #4)
  - [x] Append `MatrixModulationSection::kModulationBusIntParameters[bus]` and `kModulationBusChoiceParameters[bus]` for buses 0–9 in `buildIntDescriptors()` / `buildChoiceDescriptors()`
  - [x] Verify round-trip: APVTS tweak → `apvtsToBuffer()` → bytes 104–133 updated (spot-check bus 0 in unit test or reuse `PatchModel` test pattern)

- [x] **Create `MatrixModBusParameterSysExDispatcher`** (AC: #3)
  - [x] `parameterId → busIndex` map at construction (from descriptors)
  - [x] `dispatch(parameterId)` — resolve bus, read source/amount/destination packed bytes from `PatchModel`, enqueue

- [x] **Add `MidiManager::enqueueMatrixModBusEdit`** (AC: #3)
  - [x] Mirror `enqueueRemoteParameterEdit` pattern — encode + `editorPath_.enqueueSysEx`
  - [x] Declare in `MidiManager.h`

- [x] **Wire `PluginProcessor`** (AC: #5, #6)
  - [x] `matrixModParameterIds_` set + `buildMatrixModParameterIdSet()`
  - [x] Member + construction with `MidiManager::enqueueMatrixModBusEdit` callback
  - [x] Branch in `valueTreePropertyChanged`: Matrix Mod → matrix mod dispatcher; else patch dispatcher

- [x] **Unit tests + CMake** (AC: #7, #8)
  - [x] `MatrixModBusParameterSysExDispatcherTests.cpp`
  - [x] Extend `SysExEncoderTests`
  - [x] Build and run full test suite

## Dev Notes

### Message format (Oberheim MD §0BH)

```
F0H 10H 06H 0BH <bus> <source> <amount> <destination> F7H
```

| Field | Source in code |
|---|---|
| `<bus>` | Bus index 0–9 |
| `<source>` | `PatchModel` byte at `kModulationBusChoiceParameters[bus][0].sysExOffset` (offset 104+3×bus) |
| `<amount>` | `PatchModel` byte at `kModulationBusIntParameters[bus][0].sysExOffset` (offset 105+3×bus), 7-bit signed |
| `<destination>` | `PatchModel` byte at `kModulationBusChoiceParameters[bus][1].sysExOffset` (offset 106+3×bus) |

**Choice index ↔ Oberheim code:** APVTS choice index equals Oberheim Table 2/3 code (index 0 = NONE/unused = 0; source 1–20; destination 1–32). Descriptors in `PluginDescriptorsMatrixModulation.cpp` already order choices to match Tables 2/3. **Do not** add a parallel translation table.

**Amount encoding:** `PackedFieldCodec::encodeField` stores signed amount as 8-bit two's complement in the packed buffer. Pass the raw packed byte to SysEx unchanged (Story 2.4 lesson: do not mask with `& 0x7F`).

### Opcode spike resolution (PRD §9 #3)

| Option | Decision |
|---|---|
| **0x0B block** (Oberheim MD §0BH) | **Use this** — official opcode for Matrix Mod bus edit |
| **0x06 × 3** (three Remote Parameter Edits) | **Reject** — violates FR-14 “together”; not documented in Oberheim MD for bus edits |
| **Matrix-6/6R** | Same 134-byte PATCH buffer; `oberheim-matrix-6-6r-midi-sysex-implementation.md` Tables 2/3 match. SM-1 hardware verify recommended; no code fork in v1 |

### AD-4 routing (architecture)

| Change source | Outbound |
|---|---|
| PATCH edit param (`sysExId` set) | 0x06 (Story 2.4) |
| MASTER param | 0x03 (Story 2.5) |
| Matrix Mod bus param (`kNoSysExId`) | **0x0B** (this story) |
| Full buffer replace | 0x01 (later) |

### Critical brownfield gap (must fix)

`ApvtsPatchMapper::build*Descriptors()` currently includes **only** `PatchEditSection` modules — **not** Matrix Mod. Matrix Mod APVTS params exist (`ApvtsFactory` / `ApvtsLayoutBuilder`) but never sync to `PatchModel`. Story 2.6 **must** extend the mapper before dispatch is meaningful.

`PluginDescriptors` Matrix Mod entries use `sysExId = kNoSysExId` intentionally — they are excluded from `PatchParameterSysExDispatcher` (Story 2.4 AC #4). Do **not** assign fake `sysExId` values.

### `PluginProcessor` dispatch branching (preserve Stories 2.4–2.5)

Current `valueTreePropertyChanged` (simplified):

```cpp
if (patchParameterIds_.count(parameterId) > 0) {
    apvtsPatchMapper_->apvtsToBuffer();
    patchParameterSysExDispatcher_->dispatch(parameterId);
}
if (masterParameterIds_.count(parameterId) > 0) { ... }
```

**Target after 2.6:**

```cpp
if (patchParameterIds_.count(parameterId) > 0) {
    apvtsPatchMapper_->apvtsToBuffer();
    if (matrixModParameterIds_.count(parameterId) > 0)
        matrixModBusParameterSysExDispatcher_->dispatch(parameterId);
    else
        patchParameterSysExDispatcher_->dispatch(parameterId);
}
```

Matrix Mod ids become members of **both** `patchParameterIds_` (via extended mapper) and `matrixModParameterIds_` (explicit set).

### Prior art — copy these patterns exactly

| Pattern | Reference |
|---|---|
| Dispatcher class + enqueue callback | `PatchParameterSysExDispatcher`, `MasterParameterSysExDispatcher` |
| `MidiManager` thin encode+enqueue | `enqueueRemoteParameterEdit` |
| Unit test queue capture | `MasterParameterSysExDispatcherTests` — `EditorPath` + `MidiOutboundQueue` |
| Signed packed byte in SysEx | Story 2.4 debug log — no `& 0x7F` on value byte |

### Files to touch

| File | Action |
|---|---|
| `Source/Core/MIDI/SysEx/SysExEncoder.h/.cpp` | ADD `encodeMatrixModBusEdit` |
| `Source/Core/MIDI/SysEx/SysExConstants.h` | OPTIONAL `kMatrixModBusMessageLength` |
| `Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.h/.cpp` | NEW |
| `Source/Core/MIDI/MidiManager.h/.cpp` | ADD `enqueueMatrixModBusEdit` |
| `Source/Core/Models/ApvtsPatchMapper.cpp` | EXTEND descriptors |
| `Source/Core/PluginProcessor.h/.cpp` | Wire dispatcher + id sets |
| `Tests/Unit/MatrixModBusParameterSysExDispatcherTests.cpp` | NEW |
| `Tests/Unit/SysExEncoderTests.cpp` | EXTEND |
| `CMakeLists.txt` | Register new sources |

### Out of scope

- Matrix Mod init defaults (Story 3.3)
- Bus reorder SysEx burst (Story 2.10)
- GUI / `ModulationBusCell` changes
- `valueTreeRedirected` full-patch 0x01 send
- Hardware SM-1 validation (document only)

### Project Structure Notes

- Dispatcher lives in `Source/Core/MIDI/` alongside `PatchParameterSysExDispatcher` and `MasterParameterSysExDispatcher`.
- Descriptor data stays in `PluginDescriptorsMatrixModulation.cpp` — no duplicate offset tables.
- Core ↛ GUI invariant preserved.

### References

- [Source: _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md#0BH]
- [Source: planning-artifacts/epics.md#Story 2.6]
- [Source: planning-artifacts/architecture/.../architecture.md#AD-4]
- [Source: planning-artifacts/prds/.../addendum.md#Outbound SysEx strategy (D-044)]
- [Source: implementation-artifacts/2-4-apvts-patch-parameter-to-sysex-0x06.md]
- [Source: implementation-artifacts/2-5-apvts-master-parameter-to-full-master-sysex-0x03.md]
- [Source: Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp]
- [Source: Source/Core/MIDI/SysEx/SysExConstants.h — `kRemoteParameterEditMatrix`]

## Previous Story Intelligence (2.5)

- `MasterParameterSysExDispatcher` pattern: known-id set at construction, `dispatch(parameterId)` reads model buffer, enqueue callback to `MidiManager`.
- `PluginProcessor` dispatches on `valueTreePropertyChanged` only — never `valueTreeRedirected`.
- `PatchParameterSysExDispatcher` unchanged when adding new dispatch path — branch, don't modify skip logic.
- Review deferred: full test-suite pass evidenced locally before merge; choice test byte-compare parity optional.
- Baseline commit `2e21abf` — Story 2.5 done.

## Git Intelligence Summary

Recent Epic 2 commits establish the dispatcher + `PluginProcessor` wiring template:

| Commit | Pattern to reuse |
|---|---|
| `2e21abf` Story 2.5 | `MasterParameterSysExDispatcher` + `sendMaster` callback |
| `d1fca4f` Story 2.4 | `PatchParameterSysExDispatcher` + `enqueueRemoteParameterEdit` |
| `9202ad2` Story 2.3 | `EditorPath::enqueueSysEx` — always enqueue, never direct `MidiSender` |

## Latest Technical Information

- **JUCE 8.0.12** — no API changes relevant to this story.
- **`SysExConstants::Opcode::kRemoteParameterEditMatrix`** (`0x0B`) already defined — encoder was the missing piece.
- **No new dependencies.**

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Debug Log References

- Story AC cited "8-byte frame"; Oberheim wire format is 9 bytes total (`F0`…`F7` inclusive). `kMatrixModBusMessageLength = 9` matches `encodeMatrixModBusEdit` output and unit tests.

### Completion Notes List

- Added `SysExEncoder::encodeMatrixModBusEdit` — opcode `0x0B`, bus/source/amount/destination passed through unchanged (signed amount not masked).
- Extended `ApvtsPatchMapper` with all 10 Matrix Mod buses (bytes 104–133); bus 0 round-trip covered in `ApvtsPatchMapperTests`.
- Created `MatrixModBusParameterSysExDispatcher` — maps 30 parameter IDs to bus 0–9, reads full bus triplet from `PatchModel`, enqueues via callback.
- Added `MidiManager::enqueueMatrixModBusEdit` (bus > 9 guard).
- Wired `PluginProcessor`: `matrixModParameterIds_` + branch in `valueTreePropertyChanged`; PATCH 0x06 and MASTER 0x03 paths unchanged.
- Unit tests: `MatrixModBusParameterSysExDispatcherTests` (identical message for source/amount/destination, bus 3, unknown id); extended `SysExEncoderTests` for 0x0B layout and signed amount.
- Full test suite + `Matrix-Control_Standalone` build pass on macOS ARM.

### File List

- Source/Core/MIDI/SysEx/SysExConstants.h
- Source/Core/MIDI/SysEx/SysExEncoder.h
- Source/Core/MIDI/SysEx/SysExEncoder.cpp
- Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.h
- Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.cpp
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/Models/ApvtsPatchMapper.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Tests/Unit/MatrixModBusParameterSysExDispatcherTests.cpp
- Tests/Unit/SysExEncoderTests.cpp
- Tests/Unit/ApvtsPatchMapperTests.cpp
- CMakeLists.txt

### Change Log

- 2026-06-05 — Story 2.6: Matrix Mod bus 0x0B SysEx path (encoder, dispatcher, mapper extension, PluginProcessor wiring, unit tests).

### Review Findings

- [x] [Review][Patch] `MidiManager::enqueueMatrixModBusEdit` uses literal `9` instead of `Matrix1000Limits::kModulationBusCount` [`MidiManager.cpp:188`] — fixed

- [x] [Review][Defer] `readPackedByte` returns `0` on invalid offset via `PackedFieldCodec::safeOffset` — pre-existing `PatchParameterSysExDispatcher` pattern [`MatrixModBusParameterSysExDispatcher.cpp:44-47`] — deferred, pre-existing
- [x] [Review][Defer] `syncIntToBuffer` / `syncChoiceToBuffer` skip update when `rawValue == nullptr` — stale buffer field possible on dispatch [`ApvtsPatchMapper.cpp:38-47`] — deferred, pre-existing
- [x] [Review][Defer] Unknown `parameterId` → silent return (no log/assert release) — matches `PatchParameterSysExDispatcher` [`MatrixModBusParameterSysExDispatcher.cpp:25-27`] — deferred, pre-existing
- [x] [Review][Defer] Dual routing sets (`patchParameterIds_` + `matrixModParameterIds_` + dispatcher map) — maintenance burden if descriptors diverge [`PluginProcessor.cpp:632-643`] — deferred, established 2.4/2.5 pattern
- [x] [Review][Defer] `ApvtsPatchMapper` Matrix Mod test is one-way (`apvtsToBuffer` only); no `bufferToApvts` round-trip for choice params [`ApvtsPatchMapperTests.cpp:175-211`] — deferred, story task accepts bus-0 spot-check
- [x] [Review][Defer] No SysEx coalescing/debounce on rapid Matrix Mod edits — redundant 0x0B bursts possible [`PluginProcessor.cpp:518-525`] — deferred, out of scope / future perf story

---

**Story context engine analysis completed — comprehensive developer guide created.**
