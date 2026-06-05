---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.5 — APVTS MASTER Parameter to Full Master SysEx (0x03)
author: BMad Agent
status: done
baseline_commit: d1fca4fbd26853a5e787923d578352de141f0a09
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/2-4-apvts-patch-parameter-to-sysex-0x06.md
  - implementation-artifacts/1-4-apvtsmastermapper-round-trip.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md
  - project-context.md
created: 2026-06-05
---

# Story 2.5: APVTS MASTER Parameter to Full Master SysEx (0x03)

Status: done

## Story

As a sound designer,
I want each MASTER change to send a complete master SysEx,
so that master edits are reliable (FR-16, FR-48).

## Acceptance Criteria

1. `SysExEncoder::encodeMasterSysEx(version, packedData)` already builds the full 351-byte `F0 10 06 03 <version> … F7` frame — no encoder changes required unless tests expose a gap.
2. `Core::MasterParameterSysExDispatcher` in `Source/Core/MIDI/MasterParameterSysExDispatcher.{h,cpp}` — namespace `Core`, zero GUI. On any known MASTER `parameterId`, enqueues full master buffer from `MasterModel` via `EditorPath` (through `MidiManager::sendMaster`).
3. `PluginProcessor::valueTreePropertyChanged` — after `apvtsMasterMapper_->apvtsToBuffer()` for master params, calls `masterParameterSysExDispatcher_->dispatch(parameterId)`. **Not** on `valueTreeRedirected` (bulk state replace — full master load is later). **Not** GUI panels.
4. PATCH parameters continue to emit 0x06 only (Story 2.4); Matrix Mod unchanged (Story 2.6).
5. Unit tests: `MasterParameterSysExDispatcherTests` — master int/choice change → 351-byte message, opcode `0x03`, version byte `0x03`; unknown id → no enqueue.
6. `CMakeLists.txt` — new `.cpp` in plugin + test targets; all existing tests pass.

## Tasks / Subtasks

- [x] **Create `MasterParameterSysExDispatcher`** (AC: #2)
  - [x] Master parameter ID set from `ApvtsMasterMapper` descriptors
  - [x] `dispatch(parameterId)` — lookup, encode full buffer, enqueue

- [x] **Wire `PluginProcessor`** (AC: #3, #4)
  - [x] Member + construction with `MidiManager::sendMaster` callback
  - [x] Call after master `apvtsToBuffer` in `valueTreePropertyChanged` only

- [x] **Unit tests + CMake** (AC: #5, #6)
  - [x] `MasterParameterSysExDispatcherTests.cpp`
  - [x] Build and run full test suite

## Dev Notes

### Message format (Oberheim MD)

Full master dump: `F0H 10H 06H 03H <version> <344 nibbles> <checksum> F7H` — 351 bytes total. Version byte = `0x03` (Matrix-1000 master format, byte 4 in reference `Master 1.syx`).

### AD-4 routing

MASTER `AudioParameter` → full 0x03 on **every** change (entire 172-byte packed buffer). PATCH → 0x06 (Story 2.4). Matrix Mod → Story 2.6.

### Prior art

- Story 2.4: `PatchParameterSysExDispatcher` + `MidiManager::enqueueRemoteParameterEdit` pattern.
- Story 2.3: `MidiManager::sendMaster` already enqueues via `EditorPath::enqueueSysEx`.
- Story 1.4: `ApvtsMasterMapper::apvtsToBuffer()` syncs all master params per change (O(n) deferred, same as patch).

## Dev Agent Record

### Agent Model Used

composer-2.5-fast

### Debug Log References

- No encoder changes needed; `encodeMasterSysEx` already correct from Story 1.2.

### Completion Notes List

- Added `MasterParameterSysExDispatcher` — known master `parameterId` → full 172-byte buffer → `MidiManager::sendMaster(0x03, …)` → `EditorPath`.
- `PluginProcessor` dispatches one 0x03 per master parameter change on message thread; no dispatch on `valueTreeRedirected`.
- PATCH 0x06 path unchanged (Story 2.4).
- All unit tests pass; VST3 builds and installs.

### File List

- Source/Core/MIDI/MasterParameterSysExDispatcher.h (new)
- Source/Core/MIDI/MasterParameterSysExDispatcher.cpp (new)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/MasterParameterSysExDispatcherTests.cpp (new)
- CMakeLists.txt (modified)
- _bmad-output/implementation-artifacts/sprint-status.yaml (modified)
- _bmad-output/implementation-artifacts/2-5-apvts-master-parameter-to-full-master-sysex-0x03.md (new)

### Change Log

- 2026-06-05: Story 2.5 implemented — APVTS MASTER → full 0x03 via `MasterParameterSysExDispatcher` and `EditorPath` queue.

### Review Findings

- [x] [Review][Patch] CMakeLists test target indentation regression [`CMakeLists.txt:259`]
- [x] [Review][Defer] Choice unit test omits full payload byte-compare (int test has it) [`MasterParameterSysExDispatcherTests.cpp:91-123`] — deferred, test parity nice-to-have
- [x] [Review][Defer] Full test-suite pass not evidenced in changeset — deferred, run Matrix-Control_Tests locally before merge
