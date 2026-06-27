---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.10 — Matrix Mod Bus Reorder SysEx
author: BMad Agent
status: done
baseline_commit: f732a11
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md
  - implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
  - implementation-artifacts/manual-uat-epic2-midi-audio-hardware.md
  - _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md
  - project-context.md
created: 2026-06-06
---

# Story 2.10: Matrix Mod Bus Reorder SysEx

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a sound designer,
I want bus content reorder to send SysEx for affected buses,
so that permuted Source/Amount/Destination values reach the synth (FR-50, SysEx side).

## Acceptance Criteria

1. **Core reorder primitive (FR-50, D-073):** New `Core::MatrixModBusReorderService` in `Source/Core/MIDI/MatrixModBusReorderService.{h,cpp}` — zero GUI. Exposes **swap** of two bus **contents** by index `0…9`: Source + Amount + Destination triplets permute between rows; **bus indices and UI row labels do not change**.
2. **PatchModel + APVTS coherence:** After swap, `PatchModel` bytes `104–133` reflect the permutation and APVTS parameters for **both** affected buses match the model. Updates must **not** emit spurious per-parameter 0x0B via `PluginProcessor::valueTreePropertyChanged` (suppress or silent push — see Dev Notes).
3. **SysEx burst (FR-14):** On completed swap, enqueue **exactly one 0x0B per affected bus** (typically **2** messages for a two-row swap) via existing `MidiManager::enqueueMatrixModBusEdit` / `EditorPath`. Each message uses the **fixed hardware bus index** as `<bus>` byte with the **post-swap** source/amount/destination from `PatchModel`. **Do not** renumber buses.
4. **Inactive buses:** Swapping two NONE/0%/NONE rows is valid — same code path; both 0x0B messages carry zero triplets.
5. **Edge cases:** `fromBus == toBus` → no-op (no model change, no SysEx). Out-of-range indices → no-op. Message-thread only (document in header); future Story 7.9 calls from GUI drag handler on message thread.
6. **Public facade:** `PluginProcessor::swapMatrixModBusContents(int fromBus, int toBus)` delegates to the service — thin entry point for Epic 7 handler; no GUI wiring in this story.
7. **Delay profile:** Multi-message burst respects AD-3 queue + Story 2.2 inter-message delay (consumer applies `SysExInterMessageDelay` per dequeue — **do not** bypass queue with direct `MidiSender`).
8. **Unit tests:** `MatrixModBusReorderServiceTests` — swap active buses (model + queue), swap inactive buses, identical indices no-op, invalid index no-op, APVTS values match post-swap model for both buses. All existing tests pass; `CMakeLists.txt` updated.

## Tasks / Subtasks

- [x] **Extend `MatrixModBusParameterSysExDispatcher`** (AC: #3)
  - [x] Add `dispatchBus(int busIndex) const` — read source/amount/destination from `PatchModel` at bus offsets via existing descriptor tables; enqueue one 0x0B
  - [x] Reuse internally from existing `dispatch(parameterId)` to avoid duplicate read logic (optional refactor)

- [x] **Extend `ApvtsPatchMapper`** (AC: #2)
  - [x] Add `pushBusToApvts(int busIndex)` — push int + choice params for one bus only from `PatchModel` to APVTS
  - [x] Document silent-update strategy paired with processor suppress flag

- [x] **Create `MatrixModBusReorderService`** (AC: #1, #3, #4, #5)
  - [x] Constructor: `PatchModel&`, `ApvtsPatchMapper&`, enqueue callback (same signature as Story 2.6) or reference to dispatcher + `dispatchBus`
  - [x] `swapBusContents(int busA, int busB)` — swap 3 packed bytes per bus in model; push APVTS for both buses; call `dispatchBus` for each affected index in ascending order (0 before 9)

- [x] **Wire `PluginProcessor`** (AC: #2, #6)
  - [x] Member `matrixModBusReorderService_` + construction after dispatcher/mapper
  - [x] `suppressMatrixModParameterSysEx_` (bool or nested counter) checked in `valueTreePropertyChanged` before `matrixModBusParameterSysExDispatcher_->dispatch`
  - [x] `swapMatrixModBusContents(int fromBus, int toBus)` — set suppress, invoke service, clear suppress

- [x] **Unit tests + CMake** (AC: #8)
  - [x] `Tests/Unit/MatrixModBusReorderServiceTests.cpp`
  - [x] Register in `CMakeLists.txt` (plugin + test targets)

### Review Findings

- [x] [Review][Patch] `apvtsToBuffer()` during suppress corrupts swap [Source/Core/PluginProcessor.cpp:847-854] — fixed: skip `apvtsToBuffer()` when suppress active for Matrix Mod params.
- [x] [Review][Patch] Pas de test intégration chemin `PluginProcessor` [Tests/Unit/MatrixModBusReorderServiceTests.cpp] — added `testProcessorPathSwapWithApvtsListener` via `ProcessorPathHarness`.
- [x] [Review][Defer] `suppressMatrixModParameterSysEx_` bool vs compteur imbriqué [Source/Core/PluginProcessor.h:185] — deferred, pre-existing (story spec recommande bool ; réentrance multi-swap hors scope message-thread v1)

## Dev Notes

### Epic split (do not scope-creep)

| Layer | Story | Responsibility |
|---|---|---|
| **SysEx + Core swap** | **2.10 (this)** | `MatrixModBusReorderService`, model/APVTS permutation, 0x0B burst |
| **Drag UX + handler** | **7.9** | `ModulationBusCell` drag on bus number label, `ActionDispatcher` / handler wiring (UX-DR5) |
| **Init reset** | **3.3** | Section/per-bus init to NONE/0%/NONE |

This story delivers the **Core contract** Story 7.9 will call. **No** drag-and-drop, **no** `ModulationBusCell` changes, **no** ActionDispatcher.

### FR-50 semantics (v1 Core primitive)

PRD/addendum: user drags bus **number label**; labels **0–9 stay fixed**; only **values** move. UX-DR5 (swap vs move) is **not yet specified** in a UX doc — implement **swap** as the Core primitive:

- Drag from row A, drop on row B → **exchange** triplets between bus A and bus B.
- Story 7.9 may map drag UX to `swapMatrixModBusContents(from, to)` directly.
- **Move** (shift intermediate rows) is **out of scope** unless trivially composable; defer to 7.9/UX if needed.

### Packed buffer layout (reuse Story 2.6 — no duplicate offset table)

Per bus `b` (0–9), 3 consecutive bytes in `PatchModel`:

| Field | Descriptor | Offset |
|---|---|---|
| Source | `kModulationBusChoiceParameters[b][0]` | `104 + 3×b` |
| Amount | `kModulationBusIntParameters[b][0]` | `105 + 3×b` |
| Destination | `kModulationBusChoiceParameters[b][1]` | `106 + 3×b` |

Swap algorithm: read triplet A, read triplet B, write A→B, write B→A using `PatchModel::setValue` / `setChoiceIndex` with descriptors from `PluginDescriptors::MatrixModulationSection`.

### SysEx message (unchanged from Story 2.6)

```
F0 10 06 0B <bus> <source> <amount> <destination> F7
```

- `<bus>` = **hardware row index** (unchanged by reorder)
- `<source>` / `<destination>` = packed choice index (= Oberheim Table 2/3 code)
- `<amount>` = raw packed signed byte — **do not** mask with `& 0x7F` [Story 2.6 lesson]

After swap bus 2 ↔ bus 7: enqueue **two** messages — `bus=2` with bus 2's **new** triplet, `bus=7` with bus 7's **new** triplet.

### Suppressing duplicate 0x0B during APVTS push (critical)

`pushBusToApvts` updates 3 parameters per bus → 6 `valueTreePropertyChanged` callbacks if using `setValueNotifyingHost`. Without suppression, reorder would enqueue **6+2=8** messages instead of **2**.

**Required pattern:**

```cpp
void PluginProcessor::swapMatrixModBusContents(int fromBus, int toBus)
{
    suppressMatrixModParameterSysEx_ = true;
    matrixModBusReorderService_->swapBusContents(fromBus, toBus);
    suppressMatrixModParameterSysEx_ = false;
}
```

In `valueTreePropertyChanged`, skip `matrixModBusParameterSysExDispatcher_->dispatch` when suppress flag is set; PATCH 0x06 path unaffected (Matrix Mod uses `kNoSysExId`).

Alternative: silent APVTS write via internal parameter API — only if it avoids listener entirely **and** host automation still sees final state. Prefer suppress flag (explicit, matches future bulk operations).

### Brownfield baseline (verified @ `f732a11`)

| Component | State | File |
|---|---|---|
| `encodeMatrixModBusEdit` / 0x0B | Done (Story 2.6) | `SysExEncoder.cpp` |
| `MatrixModBusParameterSysExDispatcher` | Per-`parameterId` only | `MatrixModBusParameterSysExDispatcher.{h,cpp}` |
| `MidiManager::enqueueMatrixModBusEdit` | Done | `MidiManager.cpp:226-236` |
| `ApvtsPatchMapper` | Full-buffer sync only | `ApvtsPatchMapper.cpp` |
| `ModulationBusCell` | Static bus number label, **no drag** | `ModulationBusCell.{h,cpp}` |
| Reorder service | **Missing** | — |
| `PluginProcessor::swapMatrixModBusContents` | **Missing** | — |

### Prior art — copy these patterns

| Pattern | Reference |
|---|---|
| Bus triplet read + 0x0B enqueue | `MatrixModBusParameterSysExDispatcher::dispatch` |
| Descriptor-driven bus access | `MatrixModBusParameterSysExDispatcherTests::configureBus0` |
| Queue capture in tests | `MatrixModBusParameterSysExDispatcherTests` — `EditorPath` + `MidiOutboundQueue` |
| Thin processor facade | `setMidiInputPort`, `setKeyboardFromPort` (Story 2.9b) |
| CMake dual-target registration | Story 2.6 file list |

### Files to touch

| File | Action |
|---|---|
| `Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.h/.cpp` | ADD `dispatchBus(int)` |
| `Source/Core/MIDI/MatrixModBusReorderService.h/.cpp` | **NEW** |
| `Source/Core/Models/ApvtsPatchMapper.h/.cpp` | ADD `pushBusToApvts(int)` |
| `Source/Core/PluginProcessor.h/.cpp` | Wire service, suppress flag, public swap API |
| `Tests/Unit/MatrixModBusReorderServiceTests.cpp` | **NEW** |
| `CMakeLists.txt` | Register new `.cpp` |

### Out of scope

- GUI drag-and-drop (`ModulationBusCell`, UX-DR5) — Story 7.9
- `ActionDispatcher` / `ModuleActionHandler` — Stories 7.1 + 7.9
- Matrix Mod section/bus **init** SysEx — Story 3.3
- Full-patch 0x01 on reorder — use per-bus 0x0B only
- SysEx coalescing/debounce beyond queue delay — pre-existing deferral [Story 2.6 review]
- Hardware SM-1 validation — document manual step; optional after Epic 2 UAT

### Manual verification (post-dev, optional SM-1)

After Story 7.9 ships drag UX, or via temporary test hook calling `swapMatrixModBusContents(0, 1)`:

1. Configure bus 0 active, bus 1 inactive on hardware
2. Invoke swap
3. Confirm Matrix-1000 bus 0 shows former bus 1 contents and vice versa
4. Monitor MIDI: exactly **two** 0x0B messages, correct `<bus>` bytes 0 and 1

Until 7.9, unit tests are the acceptance gate for this story.

### Project Structure Notes

- Service lives in `Source/Core/MIDI/` alongside `MatrixModBusParameterSysExDispatcher` (architecture §4.4: "SysEx dispatch + bus reorder service").
- Descriptor data stays in `PluginDescriptorsMatrixModulation.cpp` — no duplicate offset tables.
- Core ↛ GUI invariant preserved.

### References

- [Source: planning-artifacts/epics.md#Story 2.10]
- [Source: planning-artifacts/prds/.../prd.md#FR-50]
- [Source: planning-artifacts/prds/.../addendum.md#Matrix Mod bus reorder (D-073)]
- [Source: planning-artifacts/prds/.../.decision-log.md#D-073]
- [Source: planning-artifacts/architecture/.../architecture.md#§4.4 Matrix Mod]
- [Source: implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md]
- [Source: _bmad-output/reference-docs/oberheim/oberheim-matrix-1000-midi-sysex-implementation.md#0BH]
- [Source: Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp]
- [Source: Source/Core/MIDI/MidiManager.cpp — enqueueMatrixModBusEdit]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- Full unit suite: `Builds/Tests/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` — exit 0

### Completion Notes List

- Added `MatrixModBusParameterSysExDispatcher::dispatchBus(int)`; `dispatch(parameterId)` delegates to it.
- Added `ApvtsPatchMapper::pushBusToApvts(int)` for per-bus APVTS sync (caller suppresses SysEx during reorder).
- Implemented `MatrixModBusReorderService::swapBusContents` — model triplet swap, APVTS push, two 0x0B in ascending bus order.
- Wired `PluginProcessor::swapMatrixModBusContents` with `suppressMatrixModParameterSysEx_` guard in `valueTreePropertyChanged`.
- Added `MatrixModBusReorderServiceTests` (5 cases: active swap, inactive swap, same-index no-op, invalid index no-op, APVTS coherence).

### File List

- Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.h
- Source/Core/MIDI/MatrixModBusParameterSysExDispatcher.cpp
- Source/Core/MIDI/MatrixModBusReorderService.h
- Source/Core/MIDI/MatrixModBusReorderService.cpp
- Source/Core/Models/ApvtsPatchMapper.h
- Source/Core/Models/ApvtsPatchMapper.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Tests/Unit/MatrixModBusReorderServiceTests.cpp
- CMakeLists.txt

### Change Log

- 2026-06-06: Story 2.10 — Matrix Mod bus reorder Core service, APVTS suppress path, 0x0B burst, unit tests.

## Previous Story Intelligence

**From Story 2.6 (done @ `b88c870`):**

- 0x0B is the **only** opcode for Matrix Mod bus edits — do not send 0x06×3 [FR-14].
- `MatrixModBusParameterSysExDispatcher` maps 30 parameter IDs → bus 0–9; extend with `dispatchBus` rather than new encoder.
- `ApvtsPatchMapper` already includes all 10 buses in descriptors — extend with per-bus push, not full rebuild.
- Review deferrals still apply: silent unknown-id return, no coalescing on rapid edits — reorder burst is bounded (≤2 messages per swap).

**From Story 2.9b (done):**

- Epic 2 MIDI stack + header routing validated for UAT; manual UAT guide explicitly lists 2-10 as next story.
- Processor facade pattern: GUI calls thin public methods; Core does work — mirror for `swapMatrixModBusContents`.

**From Story 2.9 (done):**

- `MidiManager::run()` dequeues via `SysExInterMessageDelay` — reorder messages automatically spaced; no extra delay logic in service.

## Git Intelligence Summary

Recent commits (newest first) relevant to this story:

| Commit | Relevance |
|---|---|
| `f732a11` | PRD v1.1 — no Matrix Mod reorder code changes |
| `68c13ce` | Story 7-7 Settings — unrelated to reorder |
| `b88c870` | Story 2.6 — **primary template** for 0x0B dispatch and mapper extension |
| `e033d68` | Story 2.9 — queue consumer; reorder burst uses same path |
| `8c3a830` | Story 2.2 — inter-message delay for multi-0x0B burst |

No reorder implementation exists in git — greenfield Core service on brownfield 0x0B path.

## Latest Technical Information

- **JUCE 8.0.12** — no API changes relevant to this story.
- **`SysExConstants::Opcode::kRemoteParameterEditMatrix`** (`0x0B`) and `kMatrixModBusMessageLength = 9` already defined.
- **Multi-message reorder:** addendum D-044 / FR-50 note — burst must respect active `SysExDelayProfile`; satisfied by existing queue consumer (Story 2.2), not by sleep in service.
- **No new dependencies.**

## Project Context Reference

- C++17, JUCE 8.0.12, Allman braces, 4-space indent, `k` prefix constants [project-context.md]
- `Source/` include root — paths like `"Core/MIDI/MatrixModBusReorderService.h"`
- Core ↛ GUI — service and tests only; no `#include` from `Source/GUI/`
- Epic 2 = MIDI transport; this story completes FR-50 **SysEx side** before Epic 7 drag UX

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **review**
