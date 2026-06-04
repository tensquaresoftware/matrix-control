---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.3 — Instrument Path and Editor Path Producers
author: BMad Agent
status: done
baseline_commit: 0e4a149b5c83141f6d7f230c9ce46d2b06955191
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - implementation-artifacts/2-2-sysexdelayprofile-and-inter-message-delay.md
  - project-context.md
created: 2026-06-04
---

# Story 2.3: Instrument Path and Editor Path Producers

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a performer,
I want keyboard/CC forwarded when armed and editor SysEx always active,
so that I can play and edit independently (FR-5, FR-6).

## Acceptance Criteria

1. `InstrumentMidiForwarder` lives in `Source/Core/Audio/InstrumentMidiForwarder.{h,cpp}`, namespace `Core`. Zero GUI dependencies. Accepts a `juce::MidiBuffer`, a `bool instrumentPathEnabled`, and a non-owning `MidiOutboundQueue&`. When `instrumentPathEnabled` is `false`, enqueues **nothing**. When `true`, iterates the buffer and enqueues **only** Note On/Off, Control Change, and Pitch Bend via `queue.enqueueRealtime()`. **Strips** Program Change, SysEx, channel pressure, and all other message types (no enqueue, no logging spam in hot path).
2. `EditorPath` lives in `Source/Core/MIDI/EditorPath.{h,cpp}`, namespace `Core`. Holds a non-owning `MidiOutboundQueue&`. Exposes **MIDI-thread-only** enqueue APIs: `enqueueSysEx(juce::MemoryBlock)` → `queue.enqueueSysEx()`; `enqueueProgramChange(int programNumber, int channel = 1)` → builds `juce::MidiMessage::programChange()` and `queue.enqueueRealtime()`. No `MidiSender` / `MidiOutput` calls inside `EditorPath`.
3. `PluginProcessor` owns `std::unique_ptr<Core::MidiOutboundQueue> outboundQueue_`, `std::unique_ptr<Core::InstrumentMidiForwarder> instrumentForwarder_`, and wires `MidiManager` with the same queue + `EditorPath` instance (single queue SSOT per AD-2/AD-3).
4. `processBlock()` calls `instrumentForwarder_->forward(midiMessages, instrumentPathEnabled)` where:
   - **Plugin (DAW):** `instrumentPathEnabled = (midiMessages.getNumEvents() > 0)` — disarmed/muted tracks deliver an empty host MIDI buffer → instrument path enqueues nothing (D-056).
   - **Standalone:** `instrumentPathEnabled` reads APVTS state property `keyboardFromEnabled` (`bool`, default `false` until Keyboard From UI exists in Epic 7/8). Document in code comment; no GUI work in this story.
5. `MidiManager::sendPatch`, `sendMaster`, and `sendProgramChange` encode as today but **enqueue through `EditorPath`** instead of calling `MidiSender` directly. Synchronous RPC paths (`sendSysExWithDelay`, `requestSysExData`, `performDeviceInquiry`) **remain direct** `MidiSender` + `SysExInterMessageDelay` until Story 2.9 (per deferred-work.md and Story 2.2 AC #8).
6. `MidiManager::run()` consumer loop **unchanged** (still stub `wait` only). Outbound messages **accumulate** in the queue until Story 2.9 drains them — acceptable; no current GUI callers depend on `sendPatch` reaching hardware yet.
7. Unit tests in `Tests/Unit/InstrumentMidiForwarderTests.cpp` and `Tests/Unit/EditorPathTests.cpp` verify: (a) disabled instrument path → queue empty after forward; (b) note/CC/PB forwarded when enabled; (c) PC + SysEx in buffer stripped when enabled; (d) `EditorPath` SysEx → `dequeue()` returns `kSysEx`; (e) `EditorPath` PC → `dequeue()` returns `kRealtime` program change; (f) editor enqueue works while instrument path disabled. All existing unit tests pass; VST3 builds cleanly.
8. New `.cpp` files added to `PLUGIN_SOURCES` and `Matrix-Control_Tests` `target_sources`; test files added to test target.

## Tasks / Subtasks

- [x] **Create `InstrumentMidiForwarder`** (AC: #1, #4)
  - [x] `forward(const juce::MidiBuffer&, bool enabled, MidiOutboundQueue&)` — static or instance method ≤ 15 lines body; helper `isAllowedInstrumentMessage(const juce::MidiMessage&)` 
  - [x] Header comment: allowed types + explicit strip list (FR-5)
  - [x] `jassert` or comment: never call from non-audio thread

- [x] **Create `EditorPath`** (AC: #2, #5)
  - [x] Constructor stores `MidiOutboundQueue&`
  - [x] `enqueueSysEx`, `enqueueProgramChange` — thin wrappers
  - [x] Header comment: MIDI thread / `MidiManager` API surface only

- [x] **Wire ownership in `PluginProcessor`** (AC: #3, #4)
  - [x] `outboundQueue_`, `instrumentForwarder_` members
  - [x] Construct after `midiManager`; pass queue ref into `MidiManager` ctor or setter
  - [x] `processBlock` forward call with arming logic above
  - [x] Init `keyboardFromEnabled` property on `apvts.state` in `initializeMidiPortProperties()` or dedicated helper

- [x] **Refactor `MidiManager` editor sends** (AC: #5, #6)
  - [x] Own or borrow `EditorPath editorPath_` + `MidiOutboundQueue& outboundQueue_`
  - [x] `sendPatch` / `sendMaster` → encode → `editorPath_.enqueueSysEx`
  - [x] `sendProgramChange` → `editorPath_.enqueueProgramChange`
  - [x] Leave `sendSysExWithDelay`, `requestSysExData`, inquiry on direct `midiSender`

- [x] **Write unit tests** (AC: #7)
  - [x] `InstrumentMidiForwarderTests` — filter matrix + enabled flag
  - [x] `EditorPathTests` — SysEx + PC classification on shared queue

- [x] **Update `CMakeLists.txt`** (AC: #8)
  - [x] Add `InstrumentMidiForwarder.cpp`, `EditorPath.cpp` to plugin + test targets
  - [x] Add both test `.cpp` files to test target

### Review Findings

- [x] [Review][Patch] Dead `MidiConnectionException` handlers on editor enqueue paths [`MidiManager.cpp:134-137,157-160,174-177`]
- [x] [Review][Patch] `sendProgramChange` missing `catch (const std::exception&)` parity with `sendPatch`/`sendMaster` [`MidiManager.cpp:167-177`]
- [x] [Review][Patch] Validate `keyboardFromEnabled` as bool before arming standalone path [`PluginProcessor.cpp:182`]
- [x] [Review][Patch] Clamp `programNumber` and `channel` in `EditorPath::enqueueProgramChange` [`EditorPath.cpp:15-17`]
- [x] [Review][Patch] Add unit tests stripping channel pressure and aftertouch [`InstrumentMidiForwarderTests.cpp`]
- [x] [Review][Defer] Mutex lock in `processBlock` via `enqueueRealtime` — inherited from Story 2.1 queue; consumer/threading in 2.9 [`MidiOutboundQueue.cpp:7-8`]
- [x] [Review][Defer] Editor SysEx without `SysExInterMessageDelay` until Story 2.9 consumer [`MidiManager.cpp:132`]
- [x] [Review][Defer] Queue growth with stub `MidiManager::run()` — explicit AC6 acceptance until 2.9 [`MidiManager.cpp:305-310`]
- [x] [Review][Defer] Audio-thread read of `keyboardFromEnabled` ValueTree — defer atomic/listener sync until Keyboard From UI (Epic 7/8) [`PluginProcessor.cpp:182`]

## Dev Notes

### Architecture Mandate — AD-3 Dual Producers (FR-5, FR-6)

From AD-3 and D-056:

| Path | Source thread | Allowed outbound | Arming |
|---|---|---|---|
| **Instrument** | Audio (`processBlock`) | Note On/Off, CC, Pitch Bend | Plugin: host MIDI buffer non-empty; Standalone: `keyboardFromEnabled` |
| **Editor** | MIDI thread (`MidiManager` APIs) | SysEx, Program Change | Always on — independent of track arming |

Both producers write to the **same** `MidiOutboundQueue` (Story 2.1). Consumer + `SysExInterMessageDelay` on dequeue is Story 2.9.

```
processBlock (audio) ──► InstrumentMidiForwarder ──► enqueueRealtime
MidiManager APIs (MIDI thread) ──► EditorPath ──► enqueueSysEx / enqueueRealtime(PC)
                              └──► MidiOutboundQueue ◄── (drain in 2.9)
```

### Current Codebase State (READ BEFORE EDITING)

| File | Today | Story 2.3 change |
|---|---|---|
| `PluginProcessor::processBlock` | Ignores `audioBuffer` and `midiMessages` | Call forwarder |
| `PluginProcessor` | No `MidiOutboundQueue` member | Add queue + forwarder |
| `MidiManager::sendPatch/Master/PC` | Direct `midiSender` / `sendSysExWithDelay` | Editor enqueue only (SysEx fire-and-forget) |
| `MidiManager::run` | Stub `wait(kMinSysExDelayMs)` | **No change** |
| `MidiOutboundQueue` | Exists, tested | **No ordering/mutex changes** |
| `Source/Core/Audio/` | **Missing** — create directory | New forwarder |

**Preserve:** `SysExInterMessageDelay` on synchronous RPC (`sendSysExWithDelay`) — shared global gap for inquiry/patch requests until 2.9 unifies all SysEx through consumer + gate.

### Instrument Message Filter — Exact Rules

```cpp
bool isAllowedInstrumentMessage(const juce::MidiMessage& msg) noexcept
{
    return msg.isNoteOnOrOff() || msg.isController() || msg.isPitchWheel();
}
```

**Strip (do not enqueue):** `isProgramChange()`, `isSysEx()`, `isAftertouch()`, `isChannelPressure()`, MTC, song position, etc.

**Deferred (deferred-work.md):** No `isSysEx()` reroute inside `enqueueRealtime` — instrument forwarder must never pass SysEx-shaped messages. Add unit test that injects PC + SysEx + NoteOn in one buffer → only NoteOn dequeued.

### `InstrumentMidiForwarder` Skeleton

```cpp
// InstrumentMidiForwarder.h
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace Core
{
    class InstrumentMidiForwarder
    {
    public:
        void forward(const juce::MidiBuffer& midiMessages,
                     bool instrumentPathEnabled,
                     MidiOutboundQueue& queue) const;
    };
}
```

Iterate with `MidiBuffer::Iterator`; for each message, if enabled && allowed → `queue.enqueueRealtime(msg)`.

### `EditorPath` Skeleton

```cpp
// EditorPath.h
#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace Core
{
    class EditorPath
    {
    public:
        explicit EditorPath(MidiOutboundQueue& queue) noexcept;

        void enqueueSysEx(juce::MemoryBlock sysEx);
        void enqueueProgramChange(int programNumber, int channel = 1);

    private:
        MidiOutboundQueue& queue_;
    };
}
```

Program Change uses `enqueueRealtime` per Story 2.1 design (realtime queue, still prioritized before SysEx on dequeue).

### `PluginProcessor` Wiring

Construction order (extend existing ctor):

```cpp
, outboundQueue_{ std::make_unique<Core::MidiOutboundQueue>() }
, instrumentForwarder_{ std::make_unique<Core::InstrumentMidiForwarder>() }
, midiManager(std::make_unique<MidiManager>(apvts, *outboundQueue_))  // signature change
```

`processBlock`:

```cpp
const bool instrumentPathEnabled = isStandaloneWrapper()
    ? apvts.state.getProperty("keyboardFromEnabled", false)
    : (midiMessages.getNumEvents() > 0);

instrumentForwarder_->forward(midiMessages, instrumentPathEnabled, *outboundQueue_);
```

**Audio thread:** no allocations beyond existing `juce::MidiMessage` copies in `enqueueRealtime` (fixed-size for notes/CC/PB).

### `MidiManager` Integration

Add members:

```cpp
Core::MidiOutboundQueue& outboundQueue_;
Core::EditorPath editorPath_;
```

`sendPatch` body change (conceptual):

```cpp
auto sysExMessage = sysExEncoder->encodePatchSysEx(patchNumber, packedData);
editorPath_.enqueueSysEx(sysExMessage);
// Do NOT call midiSender here — Story 2.9 drains queue
```

Keep exception handling and `updateErrorState` as today.

**Thread assumption:** `sendPatch` / `sendMaster` / `sendProgramChange` are only invoked from the MIDI thread or message thread that will later serialize through `MidiManager::run()`. If a future GUI caller appears, wrap with `MessageManager::callAsync` targeting MIDI thread (same pattern as Story 1.3 deferred threading note).

### Queue Consumer Gap (2.3 → 2.9)

Until Story 2.9:

- Enqueued editor/instrument messages **do not** reach the synth.
- Synchronous `requestCurrentPatch()` / Device Inquiry **still work** (direct `midiSender`).
- No regression for current brownfield flows that use RPC helpers.

Story 2.9 will: loop `while (auto msg = queue.dequeue())` in `run()`, send realtime via `MidiSender`, SysEx via `sendSysExWithDelay` or inline gate + `recordSysExSent`, remove duplicate direct sends.

### Test Pattern

Mirror `MidiOutboundQueueTests.cpp` — static `juce::UnitTest`, no `MinimalAudioProcessor`.

**InstrumentMidiForwarderTests:**

```cpp
Core::MidiOutboundQueue queue;
Core::InstrumentMidiForwarder forwarder;
juce::MidiBuffer buffer;
buffer.addEvent(juce::MidiMessage::programChange(1, 42), 0);
buffer.addEvent(juce::MidiMessage::noteOn(1, 60, 0.8f), 0);
forwarder.forward(buffer, true, queue);
auto msg = queue.dequeue();
expect(msg.has_value());
expect(msg->midiMessage.isNoteOn());
expect(queue.dequeue() == std::nullopt);  // PC stripped
```

**EditorPathTests:** enqueue SysEx + PC, verify categories. Test editor enqueue with `forward(buffer, false, queue)` showing queue still receives editor traffic (separate test method).

### CMakeLists.txt Insertion

After `MidiOutboundQueue.cpp` in plugin sources:

```cmake
Source/Core/Audio/InstrumentMidiForwarder.cpp
Source/Core/MIDI/EditorPath.cpp
```

Test target — add `.cpp` files + test units; include new sources in test `target_sources` block (same pattern as queue tests).

### Project Structure Notes

- `InstrumentMidiForwarder` → `Source/Core/Audio/` per architecture table ([architecture.md:471,504])
- `EditorPath` → `Source/Core/MIDI/` (peer to `MidiManager`, not inside `Queue/`)
- Namespace `Core`, Allman braces, `k` prefix constants, `_` suffix private members
- Includes: `"Core/Audio/InstrumentMidiForwarder.h"`, `"Core/MIDI/EditorPath.h"`

### What NOT to Do

- Do **not** implement `MidiManager::run()` queue consumer — Story 2.9
- Do **not** route APVTS parameter changes to SysEx — Stories 2.4–2.6
- Do **not** add header Instrument/Editor LEDs — Story 2.8
- Do **not** call `MidiOutput` / `MidiSender` from `processBlock` or `InstrumentMidiForwarder`
- Do **not** forward Program Change or SysEx on instrument path
- Do **not** modify `MidiOutboundQueue` priority/dequeue logic
- Do **not** migrate `sendSysExWithDelay` RPC to queue (2.9 + deferred-work)
- Do **not** implement full Keyboard From port routing UI — property + forwarder gate only
- Do **not** add GUI dependencies to Core Audio/MIDI classes

### Previous Story Intelligence (2.1 + 2.2)

From [2-1-midioutboundqueue-core.md]:

- Two-queue priority: realtime always before SysEx on `dequeue()`.
- `enqueueRealtime` accepts PC — **EditorPath** uses this; instrument path must **not** pass PC.
- Review deferred: SysEx via `enqueueRealtime`, unbounded depth, TOCTOU on `isEmpty` — consumer 2.9; producer discipline **this story**.

From [2-2-sysexdelayprofile-and-inter-message-delay.md]:

- `SysExInterMessageDelay` wired on `sendSysExWithDelay` only; queue consumer applies gate after SysEx send in 2.9.
- `performDeviceInquiry` / `requestSysExData` bypass gate — leave as-is.

From [deferred-work.md]:

- Producer classification fixes item "SysEx via enqueueRealtime" — instrument filter is the enforcement point.

### Git Intelligence

Recent Epic 2 commits:

- `bb370e6` — `MidiOutboundQueue` + tests (2.1)
- `8c3a830` — `SysExDelayProfile`, `SysExInterMessageDelay`, `MidiManager` delay migration (2.2)

Pattern: Core/MIDI/Queue for queue infra; Tests/Unit mirror; sprint-status + deferred-work updated per story; English imperative commit + bullets.

### Latest Tech / JUCE Notes

- **JUCE 8.0.12** — `juce::MidiMessage::isNoteOnOrOff()`, `isController()`, `isPitchWheel()` for filter.
- **Track arming:** No JUCE API for "is track armed" in `processBlock`; rely on host not delivering MIDI when disarmed (Ableton/Live standard). Document SM-2 manual validation.
- **Standalone Keyboard From:** `juce::PluginHostType::getPluginLoadedAs() == wrapperType_Standalone` pattern already in `PluginProcessor.cpp` anonymous namespace.

### References

- Story 2.3 epic AC — [epics.md:481-492]
- FR-5, FR-6 — [epics.md:27-28], [prd.md:153-165]
- D-056 arming vs editor — [.decision-log.md:423-427]
- AD-2 ownership — [architecture.md:107-127]
- AD-3 producers — [architecture.md:130-143]
- D-050/D-051 MIDI thread — [.decision-log.md:392] (queue decision)
- Project MIDI invariants — [project-context.md:152-169]
- Queue API — [MidiOutboundQueue.h], [MidiOutboundQueue.cpp]
- `processBlock` stub — [PluginProcessor.cpp:170-175]
- `MidiManager` direct send — [MidiManager.cpp:118-175]
- `MidiManager::run` stub — [MidiManager.cpp:302-308]
- Story 2.1 — [2-1-midioutboundqueue-core.md]
- Story 2.2 — [2-2-sysexdelayprofile-and-inter-message-delay.md]
- Deferred queue findings — [deferred-work.md]

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Debug Log References

### Completion Notes List

- Added `InstrumentMidiForwarder` (audio thread): Note/CC/PB only; strips PC/SysEx/pressure.
- Added `EditorPath` (MIDI thread): `enqueueSysEx` / `enqueueProgramChange` → shared `MidiOutboundQueue`.
- `PluginProcessor` owns queue + forwarder; `processBlock` arming: DAW = non-empty MIDI buffer, Standalone = `keyboardFromEnabled`.
- `MidiManager` editor sends queue-only; RPC (`sendSysExWithDelay`, inquiry, requests) unchanged for Story 2.9.
- Unit tests: `InstrumentMidiForwarderTests`, `EditorPathTests`. Full test runner + VST3 build OK.

### File List

- Source/Core/Audio/InstrumentMidiForwarder.h
- Source/Core/Audio/InstrumentMidiForwarder.cpp
- Source/Core/MIDI/EditorPath.h
- Source/Core/MIDI/EditorPath.cpp
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Tests/Unit/InstrumentMidiForwarderTests.cpp
- Tests/Unit/EditorPathTests.cpp
- CMakeLists.txt

### Change Log

- 2026-06-04: Story 2.3 — dual MIDI producers (instrument + editor) wired to `MidiOutboundQueue`; editor fire-and-forget until 2.9 consumer.

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Story status: **ready-for-dev**
- Next workflow: `dev-story` for implementation; `code-review` when complete
- Blocking dependency for hardware outbound: Story **2.9** (queue consumer)
