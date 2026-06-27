---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.4 — APVTS PATCH Parameter to SysEx (0x06)
author: BMad Agent
status: done
baseline_commit: 9202ad248bcc4969fb1e7bcc9933c63bcc2d3a02
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md
  - implementation-artifacts/1-3-apvtspatchmapper-round-trip.md
  - _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - project-context.md
created: 2026-06-04
---

# Story 2.4: APVTS PATCH Parameter to SysEx (0x06)

Status: done

## Story

As a sound designer,
I want PATCH slider/combo changes to send Remote Parameter Edit SysEx,
so that live editing works with DAW automation throttled by the queue (FR-10, FR-48).

## Acceptance Criteria

1. `SysExEncoder::encodeRemoteParameterEdit(parameterNumber, value)` builds `F0 10 06 06 <param> <value> F7` per Oberheim MD §06H (7 bytes, 7-bit data).
2. `Core::PatchParameterSysExDispatcher` in `Source/Core/MIDI/PatchParameterSysExDispatcher.{h,cpp}` — namespace `Core`, zero GUI. Maps `parameterId` → `PluginDescriptors` int/choice descriptor (from `ApvtsPatchMapper::build*Descriptors()`). Skips `sysExId == kNoSysExId`. Reads packed byte from `PatchModel` at `sysExOffset` after buffer sync. Enqueues via `EditorPath::enqueueSysEx`.
3. `PluginProcessor::valueTreePropertyChanged` — after `apvtsPatchMapper_->apvtsToBuffer()` for patch params, calls `patchParameterSysExDispatcher_->dispatch(parameterId)`. **Not** on `valueTreeRedirected` (bulk state replace — full patch 0x01 is later). **Not** GUI panels.
4. Matrix Mod bus parameters (`kNoSysExId`, Story 2.6) and patch name (`PatchNameSyncer`) do **not** emit 0x06.
5. Unit tests: `SysExEncoderTests` — remote edit byte layout; `PatchParameterSysExDispatcherTests` — int 6-bit, int 7-bit signed, choice → correct queue SysEx; unknown / `kNoSysExId` id → no enqueue.
6. `CMakeLists.txt` — new `.cpp` in plugin + test targets; all existing tests pass.

## Tasks / Subtasks

- [x] **Add `encodeRemoteParameterEdit` to `SysExEncoder`** (AC: #1)
  - [x] Header + implementation; 7-bit clamp on param number only; value = packed buffer byte (signed fields may be >127)

- [x] **Create `PatchParameterSysExDispatcher`** (AC: #2)
  - [x] Descriptor maps at construction
  - [x] `dispatch(parameterId)` — lookup, skip, packed byte, enqueue

- [x] **Wire `PluginProcessor`** (AC: #3, #4)
  - [x] Member + construction
  - [x] Call after patch `apvtsToBuffer` in `valueTreePropertyChanged` only

- [x] **Unit tests + CMake** (AC: #5, #6)
  - [x] `PatchParameterSysExDispatcherTests.cpp`
  - [x] Extend `SysExEncoderTests`
  - [x] Build and run full test suite

## Dev Notes

### Message format (Oberheim MD)

`F0H 10H 06H 06H <parameter> <value> F7H` — parameter = Matrix-1000 parameter number (`descriptor.sysExId`); value = packed edit-buffer byte at `sysExOffset` (6-bit, 7-bit signed, choice index — already encoded in buffer by `PatchModel` / `PackedFieldCodec`).

### AD-4 routing

PATCH `AudioParameter` → 0x06 only. MASTER → Story 2.5 (0x03). Matrix Mod → Story 2.6 (0x0B). Full buffer → 0x01 (later).

### Prior art

- Story 2.3: `EditorPath::enqueueSysEx` — use this, not `MidiSender`.
- Story 1.3: `ApvtsPatchMapper` unchanged; dispatch is additive in `PluginProcessor`.
- `apvtsToBuffer()` still syncs all patch params per change (O(n) deferred); 0x06 sends **one** param per event.

## Dev Agent Record

### Agent Model Used

claude-sonnet-4-6

### Debug Log References

- Initial test failure: `value & 0x7F` in encoder stripped signed packed bytes (e.g. -5 → 251 became 123). Fixed by passing packed byte through unchanged.

### Completion Notes List

- Added `SysExEncoder::encodeRemoteParameterEdit` (7-byte Oberheim 0x06 frame).
- Added `PatchParameterSysExDispatcher` with descriptor maps; `MidiManager::enqueueRemoteParameterEdit` bridges to `EditorPath`.
- `PluginProcessor` dispatches one 0x06 per patch parameter change on message thread; no dispatch on `valueTreeRedirected`.
- Matrix Mod and patch name paths unchanged (out of `patchParameterIds_` / `kNoSysExId`).
- All unit tests pass; VST3 builds and installs.

### File List

- Source/Core/MIDI/SysEx/SysExEncoder.h (modified)
- Source/Core/MIDI/SysEx/SysExEncoder.cpp (modified)
- Source/Core/MIDI/PatchParameterSysExDispatcher.h (new)
- Source/Core/MIDI/PatchParameterSysExDispatcher.cpp (new)
- Source/Core/MIDI/MidiManager.h (modified)
- Source/Core/MIDI/MidiManager.cpp (modified)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/SysExEncoderTests.cpp (modified)
- Tests/Unit/PatchParameterSysExDispatcherTests.cpp (new)
- CMakeLists.txt (modified)
- _bmad-output/implementation-artifacts/sprint-status.yaml (modified)

### Change Log

- 2026-06-04: Story 2.4 implemented — APVTS PATCH → 0x06 via `PatchParameterSysExDispatcher` and `EditorPath` queue.

### Review Findings

- [x] [Review][Decision] **AC #5 — test `kNoSysExId`** — Résolu (B) : aucun descripteur PatchEdit n’utilise `kNoSysExId` ; skip défensif + `testUnknownParameterNoEnqueue` ; commentaire dans les tests (pas de seam dédiée).

- [x] [Review][Patch] **Masquage param 0x7F non testé** [`Tests/Unit/SysExEncoderTests.cpp`]
- [x] [Review][Patch] **Octet value >127 non testé dans l’encodeur** [`Tests/Unit/SysExEncoderTests.cpp`]
- [x] [Review][Patch] **Tests dispatcher fragiles (`intDescs.front()` / `[1]`)** [`Tests/Unit/PatchParameterSysExDispatcherTests.cpp`]

- [x] [Review][Defer] **`MidiManager::enqueueRemoteParameterEdit` sans tests** [`Source/Core/MIDI/MidiManager.cpp:172`] — deferred, pre-existing (`MidiManagerTests` stub)
- [x] [Review][Defer] **Pas de test E2E `PluginProcessor` → queue** — deferred, hors AC story (couverture unitaire dispatcher + encoder)
- [x] [Review][Defer] **`apvtsToBuffer()` O(n) à chaque tweak patch** [`Source/Core/PluginProcessor.cpp:501`] — deferred, pattern accepté stories 1.3 / 2.4 dev notes
