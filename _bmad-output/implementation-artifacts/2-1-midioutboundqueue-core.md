---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.1 — MidiOutboundQueue Core
author: BMad Agent
status: done
baseline_commit: 8b0cfb1021750fbce88af158318b57a59443822b
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/1-5-patch-name-bytes-0-7-sync.md
created: 2026-06-04
---

# Story 2.1: MidiOutboundQueue Core

Status: done

## Story

As a performer,
I want all outbound MIDI merged in one queue with realtime priority,
so that playing notes is never blocked by SysEx floods (FR-7, NFR-2, NFR-3).

## Acceptance Criteria

1. `MidiOutboundQueue` lives in `Source/Core/MIDI/Queue/MidiOutboundQueue.{h,cpp}`, namespace `Core`. Zero GUI dependencies. No direct `MidiOutput` or `MidiSender` calls inside the queue itself — it is pure data structure.
2. `enqueueRealtime(juce::MidiMessage)` enqueues Note On/Off, CC, Pitch Bend, or Program Change messages. Thread-safe: safe to call from the audio thread.
3. `enqueueSysEx(juce::MemoryBlock)` enqueues a SysEx payload. Thread-safe: safe to call from the message thread or dedicated MIDI thread.
4. `dequeue()` returns `std::optional<Core::MidiOutboundQueue::Message>` — a tagged struct with `category` (`kRealtime` or `kSysEx`) and the payload. Realtime messages are always returned before any pending SysEx message, regardless of enqueue order.
5. `isEmpty() const noexcept` returns `true` when both queues are empty.
6. Unit tests in `Tests/Unit/MidiOutboundQueueTests.cpp` (JUCE `UnitTest` subclass) verify: (a) single realtime enqueue/dequeue round-trip; (b) SysEx enqueued before realtime dequeues realtime first; (c) N realtime + M SysEx enqueued interleaved → all N realtime come out before any of the M SysEx; (d) empty queue → `dequeue()` returns `std::nullopt`, `isEmpty()` is `true`.
7. `Source/Core/MIDI/Queue/MidiOutboundQueue.cpp` added to `PLUGIN_SOURCES` and to `Matrix-Control_Tests` `target_sources`; `Tests/Unit/MidiOutboundQueueTests.cpp` added to the test target. All existing 31 tests pass.
8. `MidiManager` is **not** modified in this story. The queue is created but not yet wired to the consumer (Story 2.9).

## Tasks / Subtasks

- [x] **Design `Message` tagged struct** (AC: #4)
  - [x] Define `enum class MessageCategory { kRealtime, kSysEx }` inside `MidiOutboundQueue`
  - [x] Define `struct Message { MessageCategory category; juce::MidiMessage midiMessage; juce::MemoryBlock sysExData; }` inside `MidiOutboundQueue`
  - [x] Verify: no magic numbers, no GUI headers, all in `Core` namespace

- [x] **Implement `MidiOutboundQueue`** (AC: #1–#5)
  - [x] Create `Source/Core/MIDI/Queue/MidiOutboundQueue.h`
  - [x] Create `Source/Core/MIDI/Queue/MidiOutboundQueue.cpp`
  - [x] Two internal queues: `realtimeQueue_` and `sysExQueue_` — both `std::queue`, protected by `std::mutex queueMutex_`
  - [x] `enqueueRealtime`: lock mutex, push to `realtimeQueue_`
  - [x] `enqueueSysEx`: lock mutex, push to `sysExQueue_`
  - [x] `dequeue`: lock mutex, drain `realtimeQueue_` first; if empty drain `sysExQueue_`; return `std::nullopt` if both empty
  - [x] `isEmpty`: lock mutex, check both queues
  - [x] Verify: class ≤ 200 lines, all methods ≤ 15 lines, `JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR`

- [x] **Write `Tests/Unit/MidiOutboundQueueTests.cpp`** (AC: #6)
  - [x] Test A: single realtime round-trip
  - [x] Test B: SysEx before realtime → realtime dequeued first
  - [x] Test C: interleaved N realtime + M SysEx → all realtime before all SysEx
  - [x] Test D: empty queue → `std::nullopt` + `isEmpty()` true

- [x] **Update `CMakeLists.txt`** (AC: #7)
  - [x] Add `Source/Core/MIDI/Queue/MidiOutboundQueue.cpp` to `PLUGIN_SOURCES` (after existing MIDI sources block)
  - [x] Add same `.cpp` to `Matrix-Control_Tests` `target_sources`
  - [x] Add `Tests/Unit/MidiOutboundQueueTests.cpp` to test target
  - [x] Rebuild; confirm all 31 + new tests pass, VST3 builds cleanly

## Dev Notes

### Architecture Mandate — AD-3

`MidiOutboundQueue` is the single outbound channel for all MIDI output. From AD-3:

- **Producers:** `InstrumentMidiForwarder` (Note On/Off, CC, Pitch Bend) and `EditorPath` (SysEx, Program Change) — wired in Stories 2.3 and 2.9.
- **Consumer:** `MidiManager::run()` — wired in Story 2.9.
- **Rule:** `MidiOutput` is never called from the audio thread. The queue is the only path.

Story 2.1 creates the queue in isolation. No producers, no consumer, no wiring to `MidiManager`.

### Two-Queue Priority Design

Use two separate `std::queue` instances protected by a single `std::mutex`:

```cpp
std::queue<juce::MidiMessage> realtimeQueue_;
std::queue<juce::MemoryBlock> sysExQueue_;
std::mutex queueMutex_;
```

`dequeue()` drains `realtimeQueue_` first. This guarantees realtime priority without a heap-based priority queue (simpler, predictable, O(1)).

**Why not `juce::AbstractFifo`?** `AbstractFifo` is single-producer/single-consumer lock-free. Future Story 2.3 adds two producers (audio thread for realtime, message thread for SysEx). Multi-producer lock-free FIFOs require atomic CAS loops, adding complexity without measurable benefit at this stage. A `std::mutex` is correct and auditable. Optimize only with profiling evidence.

**Why two queues, not one tagged queue?** Keeps the priority rule structural rather than runtime: realtime queue is always drained before SysEx queue. No sorting needed.

### `Message` Struct — Keep It Flat

```cpp
// MidiOutboundQueue.h — nested inside class
enum class MessageCategory { kRealtime, kSysEx };

struct Message
{
    MessageCategory category;
    juce::MidiMessage midiMessage;
    juce::MemoryBlock sysExData;
};
```

For `kRealtime` messages, `sysExData` is empty (default-constructed `juce::MemoryBlock`). For `kSysEx`, `midiMessage` is default-constructed. This avoids `std::variant` complexity while remaining unambiguous. The consumer checks `category` before accessing the payload.

### Class Skeleton

```cpp
// MidiOutboundQueue.h
#pragma once

#include <mutex>
#include <optional>
#include <queue>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

namespace Core
{
    class MidiOutboundQueue
    {
    public:
        enum class MessageCategory { kRealtime, kSysEx };

        struct Message
        {
            MessageCategory category;
            juce::MidiMessage midiMessage;
            juce::MemoryBlock sysExData;
        };

        MidiOutboundQueue() = default;

        void enqueueRealtime(juce::MidiMessage message);
        void enqueueSysEx(juce::MemoryBlock sysEx);

        std::optional<Message> dequeue();
        bool isEmpty() const noexcept;

    private:
        std::queue<juce::MidiMessage> realtimeQueue_;
        std::queue<juce::MemoryBlock> sysExQueue_;
        mutable std::mutex queueMutex_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiOutboundQueue)
    };
}
```

All four methods ≤ 15 lines each. No magic numbers. `isEmpty()` is `const` — uses `mutable std::mutex`.

### `dequeue()` Implementation Pattern

```cpp
std::optional<Core::MidiOutboundQueue::Message> Core::MidiOutboundQueue::dequeue()
{
    std::lock_guard<std::mutex> lock(queueMutex_);

    if (!realtimeQueue_.empty())
        return Message { MessageCategory::kRealtime, realtimeQueue_.front(), {} };
        // then pop after returning? No — must pop before return
        // Use local + pop:
    ...
}
```

Correct pattern — pop after constructing the return value:

```cpp
std::optional<Message> Core::MidiOutboundQueue::dequeue()
{
    std::lock_guard<std::mutex> lock(queueMutex_);

    if (!realtimeQueue_.empty())
    {
        Message msg { MessageCategory::kRealtime, realtimeQueue_.front(), {} };
        realtimeQueue_.pop();
        return msg;
    }

    if (!sysExQueue_.empty())
    {
        Message msg { MessageCategory::kSysEx, {}, sysExQueue_.front() };
        sysExQueue_.pop();
        return msg;
    }

    return std::nullopt;
}
```

### Threading Contract

| Method | Safe threads | Notes |
|---|---|---|
| `enqueueRealtime` | any (incl. audio thread) | Acquires `queueMutex_` — brief lock, O(1) |
| `enqueueSysEx` | message thread, MIDI thread | Acquires `queueMutex_` |
| `dequeue` | MIDI thread only | Single consumer — `MidiManager::run()` in Story 2.9 |
| `isEmpty` | any | `mutable std::mutex` allows const call |

### Test Pattern — Mirror Existing Tests

```cpp
// Tests/Unit/MidiOutboundQueueTests.cpp
class MidiOutboundQueueTests : public juce::UnitTest
{
public:
    MidiOutboundQueueTests() : juce::UnitTest("MidiOutboundQueue") {}

    void runTest() override
    {
        beginTest("Realtime round-trip");
        // ...
    }
};

static MidiOutboundQueueTests midiOutboundQueueTests;
```

Use `juce::UnitTest` subclass (same pattern as `PatchNameSyncerTests`, `ApvtsPatchMapperTests`). No `MinimalAudioProcessor` needed — `MidiOutboundQueue` has no APVTS dependency.

### CMakeLists.txt — Exact Insertion Points

**`PLUGIN_SOURCES` block** — after `Source/Core/MIDI/MidiManager.cpp` (line ~130):
```cmake
Source/Core/MIDI/Queue/MidiOutboundQueue.cpp
```

**`Matrix-Control_Tests` `target_sources`** — after existing MIDI sources (line ~244 area):
```cmake
Source/Core/MIDI/Queue/MidiOutboundQueue.cpp
Tests/Unit/MidiOutboundQueueTests.cpp
```

New directory `Source/Core/MIDI/Queue/` must be created (no CMake `add_subdirectory` needed — JUCE flat source list).

### What NOT to Do

- Do **not** modify `MidiManager.h` or `MidiManager.cpp` — wiring is Story 2.9.
- Do **not** add delay logic — that is `SysExDelayProfile` in Story 2.2.
- Do **not** create `InstrumentMidiForwarder` or `EditorPath` — Story 2.3.
- Do **not** add a `juce::MidiOutput*` reference inside `MidiOutboundQueue` — it is pure data structure.
- Do **not** use `juce::AbstractFifo` unless `std::mutex` proves to be a problem (it won't in Story 2.1).

### Deferred Findings from Story 1.5 Applicable Here

The allocation-on-audio-thread concern flagged in Story 1.5 review applies here too: `juce::MidiMessage` copy on `enqueueRealtime` allocates for SysEx-typed messages (which should never arrive here). Note On/Off, CC, PB are small fixed-size `juce::MidiMessage` with no heap allocation. Document this assumption in a comment if the team wants explicit safety.

### Project Structure Notes

- New directory: `Source/Core/MIDI/Queue/` — consistent with architecture table (`MIDI/Queue/`)
- `MidiOutboundQueue` stays in `Core` namespace, zero GUI deps
- No `PluginIDs.h` or `PluginDescriptors` changes needed
- Test in `Tests/Unit/` — existing pattern

### References

- AD-3 (MIDI unified outbound queue) — [architecture.md:§AD-3]
- `MidiSender` (future consumer's tool) — [Source/Core/MIDI/Transport/MidiSender.h]
- `MidiManager::run()` stub (future wiring point, Story 2.9) — [Source/Core/MIDI/MidiManager.cpp:296-302]
- `SysExConstants::kMinSysExDelayMs = 10` — [Source/Core/MIDI/SysEx/SysExConstants.h:55] (used in Story 2.2, not here)
- Test pattern reference — [Tests/Unit/PatchNameSyncerTests.cpp]
- CMakeLists MIDI block — [CMakeLists.txt:122-130]; test target block — [CMakeLists.txt:225-252]
- Epic 2 story definitions — [_bmad-output/planning-artifacts/epics.md:§Story 2.1]

## Dev Agent Record

### Agent Model Used

claude-sonnet-4-6

### Debug Log References

### Completion Notes List

- `MidiOutboundQueue` implemented as pure data structure with two-queue priority design (realtime always before SysEx).
- `mutable std::mutex` on `isEmpty()` allows `const noexcept` contract while locking.
- `enqueueRealtime` and `enqueueSysEx` use `std::move` for efficient queue insertion.
- 4 unit tests (A–D from AC#6) pass; 35 total tests pass (31 pre-existing + 4 new), exit code 0.
- VST3 builds cleanly with new source.
- Note: `juce_String.cpp:327` assertions in 3 of 4 new tests are benign JUCE-internal noise (MessageManager absent in console test runner) — same class as pre-existing `juce_Timer.cpp` assertions; no test failure reported.

### File List

- Source/Core/MIDI/Queue/MidiOutboundQueue.h (new)
- Source/Core/MIDI/Queue/MidiOutboundQueue.cpp (new)
- Tests/Unit/MidiOutboundQueueTests.cpp (new)
- CMakeLists.txt (modified)

### Review Findings

- [x] [Review][Defer] Unbounded `std::queue` growth — no depth cap/backpressure [`MidiOutboundQueue.cpp:5`] — deferred, out of Story 2.1 scope; revisit with consumer wiring (2.9) or flood policy
- [x] [Review][Defer] SysEx-shaped `juce::MidiMessage` via `enqueueRealtime` — no reroute/guard [`MidiOutboundQueue.cpp:5`] — deferred, matches Story 1.5 note; producers 2.3 must classify correctly
- [x] [Review][Defer] `isEmpty()` then `dequeue()` TOCTOU across threads [`MidiOutboundQueue.cpp:38`] — deferred, consumer 2.9 should loop `dequeue()` only
- [x] [Review][Defer] SysEx `dequeue` copies `MemoryBlock` under mutex; audio `enqueueRealtime` can block [`MidiOutboundQueue.cpp:30`] — deferred, optimize if profiling shows issue (Story 2.2 delay + 2.9 consumer)
- [x] [Review][Defer] No `clear`/flush API for disconnect/panic [`MidiOutboundQueue.h`] — deferred, not in AC; Story 2.9 lifecycle
- [x] [Review][Defer] `enqueueRealtime` by-value param → extra copy before `std::move` [`MidiOutboundQueue.cpp:5`] — deferred, micro-opt; prefer `&&` if hot path proves costly
- [x] [Review][Defer] Empty `MemoryBlock` accepted on `enqueueSysEx` [`MidiOutboundQueue.cpp:11`] — deferred, consumer 2.9 may validate
- [x] [Review][Defer] `dequeue()` single-consumer not enforced in API [`MidiOutboundQueue.cpp:17`] — deferred, document in 2.9 `MidiManager::run()`; ThreadSanitizer optional later
- [x] [Review][Defer] Tests A–D only; no SysEx byte round-trip / concurrency [`MidiOutboundQueueTests.cpp:10`] — deferred, AC#6 satisfied; harden before hardware if desired
