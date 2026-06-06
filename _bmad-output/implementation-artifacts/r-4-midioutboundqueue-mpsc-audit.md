---
organization: Ten Square Software
project: Matrix-Control
title: Story R-4 — MidiOutboundQueue MPSC Audit
author: BMad Agent
status: done
baseline_commit: f301b621
sources:
  - planning-artifacts/sprint-change-proposal-2026-06-06.md
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - implementation-artifacts/r-3-midi-port-open-error-feedback.md
  - implementation-artifacts/deferred-work.md
  - project-context.md
created: 2026-06-06
updated: 2026-06-06
---

# Story R-4: MidiOutboundQueue MPSC Audit

Status: done

<!-- AD-3. Sprint Change Proposal 2026-06-06. Recommended before Epic E3. R-1/R-2/R-3 done. Closes deferred 2-1 concurrency test item. -->

## Story

As a developer,
I want a documented MPSC contract on `MidiOutboundQueue`,
so that concurrent producers (audio + message threads) are safe before Epic 3 (NFR-3).

## Acceptance Criteria

### AC 1 — Documented threading contract in header

1. **`MidiOutboundQueue.h`** carries a block comment (above the class) documenting:
   - **Pattern:** MPSC (multi-producer, single-consumer).
   - **Producers** (all may call concurrently):
     - `InstrumentMidiForwarder` — audio thread → `enqueueRealtime` only.
     - `EditorPath` — message thread → `enqueueSysEx` + `enqueueRealtime` (Program Change).
     - `KeyboardFromMidiInput` — JUCE MIDI input callback thread → `enqueueRealtime` only (standalone Keyboard From).
   - **Consumer:** `MidiManager::run()` on dedicated MIDI thread → `dequeue()` only.
   - **Priority:** `dequeue()` always drains `realtimeQueue_` before `sysExQueue_` (FR-7).
   - **Forbidden:** `MidiOutput::sendMessageNow` or any blocking I/O from producers; consumer only sends.
   - **Not SPSC:** `juce::AbstractFifo` rejected in Story 2.1 — mutex-backed dual queues are intentional.
2. Per-method thread table in comment: which threads may call each public method.
3. **Audit finding** one-liner in comment: e.g. `// Audit R-4 (2026-06-06): std::mutex MPSC — no impl change required` or describe fix if applied.

### AC 2 — Implementation correctness

1. **Verify** existing `std::mutex` + dual `std::queue` design satisfies MPSC (multiple producers lock independently; single consumer on MIDI thread).
2. **If insufficient** (data race, lost messages under contention): fix in-place — prefer keeping mutex MPSC over lock-free SPSC swap; do **not** introduce `AbstractFifo` without AD revision.
3. Preserve Story 2.1 behaviours: realtime-before-SysEx priority, O(1) enqueue, no GUI deps, no `MidiOutput` in queue class.
4. **Out of scope:** max depth / flush API / `enqueueRealtime&&` overload (deferred-work.md 2-1 items).

### AC 3 — Dual-producer + consumer stress test

1. New test in **`Tests/Unit/MidiOutboundQueueTests.cpp`** (or dedicated sibling file registered in CMake):
   - **Producer A** (std::thread): enqueue `kRealtimeCount` unique Note On messages (channel 1, note = index).
   - **Producer B** (std::thread): enqueue `kSysExCount` unique SysEx payloads (first data byte = index).
   - **Consumer** (std::thread): loop `dequeue()` until `kRealtimeDequeued == kRealtimeCount && kSysExDequeued == kSysExCount` or timeout.
2. Assert **zero message loss** — counts match; each unique ID seen exactly once.
3. Assert **priority invariant:** while both queues had pending work at enqueue time, no SysEx dequeued before all realtime messages that were already queued — use post-join drain phase OR track: at any dequeue, if realtime queue non-empty at lock acquisition, category must be `kRealtime` (stress with high iteration count, e.g. ≥1000 messages each side).
4. Test completes in CI without hardware (< 5 s wall time).
5. All existing unit tests pass.

### AC 4 — Build & sprint hygiene

1. No CMake change unless new test file added.
2. `sprint-status.yaml`: `r-4-midioutboundqueue-mpsc-audit` → `done` when complete.

## Tasks / Subtasks

- [x] **Audit current implementation** (AC: #1, #2)
  - [x] Read producers: `InstrumentMidiForwarder.cpp`, `EditorPath.cpp`, `KeyboardFromMidiInput.cpp`
  - [x] Read consumer: `MidiManager::run()` (`MidiManager.cpp:363`)
  - [x] Confirm mutex covers all queue mutations; document finding in header

- [x] **Header contract documentation** (AC: #1)
  - [x] Add MPSC block comment + method thread table to `MidiOutboundQueue.h`

- [x] **Stress test** (AC: #3)
  - [x] Implement dual-producer + consumer test with loss detection
  - [x] Register in `CMakeLists.txt` if new file

- [x] **Verification** (AC: #3, #4)
  - [x] Run `Matrix-Control_Tests`
  - [x] macOS-ARM-Debug build smoke

## Dev Notes

### Problem / context

Story 2.1 chose `std::mutex` over `juce::AbstractFifo` because AD-3 requires **two+ producers** (audio + message). Deferred-work flagged missing concurrency test and undocumented single-consumer assumption. Sprint Change Proposal R-4 closes this before Epic E3 (InitDefaults + heavy SysEx enqueue).

### Current implementation (brownfield — READ BEFORE EDIT)

```cpp
// MidiOutboundQueue.cpp — all paths hold queueMutex_
enqueueRealtime → realtimeQueue_.push
enqueueSysEx    → sysExQueue_.push
dequeue         → realtime first, then sysEx
isEmpty         → both empty check
```

**Verdict (pre-audit):** Already MPSC-safe. Expected outcome = documentation + stress test, not algorithm rewrite.

### Producer → method map

| Producer | Thread | Method | Payload |
|---|---|---|---|
| `InstrumentMidiForwarder` | Audio (`processBlock`) | `enqueueRealtime` | Note/CC/PB |
| `EditorPath` | Message (APVTS listeners) | `enqueueSysEx`, `enqueueRealtime` | SysEx, Program Change |
| `KeyboardFromMidiInput` | MIDI input callback | `enqueueRealtime` | Note/CC/PB |

Architecture AD-3 lists Instrument + Editor only; **KeyboardFromMidiInput** is a third realtime producer (Story 2.9b) — document in header.

### Consumer contract

`MidiManager::run()` (`MidiManager.cpp:363-380`):

- Polls `outboundQueue_.dequeue()` — never gates on prior `isEmpty()` (TOCTOU — deferred-work 2-1).
- Dispatches via `dispatchOutboundMessage` → `MidiSender` / `sendSysExWithDelay`.
- Only thread that should call `dequeue()`.

### Stress test sketch

```cpp
constexpr int kRealtimeCount = 1000;
constexpr int kSysExCount = 1000;
Core::MidiOutboundQueue queue;
std::atomic<int> realtimeDone { 0 }, sysExDone { 0 };
std::set<int> seenNotes, seenSysExIds;

// Producer A: noteOn(1, i, 0.5f) for i in [0, kRealtimeCount)
// Producer B: MemoryBlock { F0, i, F7 } for i in [0, kSysExCount)
// Consumer: dequeue until counts satisfied; use juce::Time timeout guard
// expect seenNotes.size() == kRealtimeCount && seenSysExIds.size() == kSysExCount
```

Use `std::thread` (C++17). Join producers before asserting consumer drained (or concurrent drain — both valid stress patterns; prefer **concurrent** enqueue+dequeue for race coverage).

### What NOT to do

- Do **not** replace mutex with `AbstractFifo` (SPSC only).
- Do **not** add max-depth / flush / rate-limit (deferred).
- Do **not** modify `MidiManager` wiring beyond comments if any.
- Do **not** change producer call sites unless audit finds a bug.

### Previous story intelligence (R-3)

- Test pattern: pure Core helper + `juce::UnitTest`; CMake `Matrix-Control_Tests` registration.
- Build: `cmake --preset macOS-ARM-Debug && cmake --build Builds/macOS-ARM --target Matrix-Control_Tests`.

### References

- [sprint-change-proposal-2026-06-06.md § Story R-4]
- [epics.md Epic R, Story R-4]
- [architecture.md § AD-3]
- [2-1-midioutboundqueue-core.md § Threading Contract, AbstractFifo rationale]
- [deferred-work.md § 2-1 concurrency test deferred]
- [MidiOutboundQueue.h / .cpp]
- [InstrumentMidiForwarder.cpp:26], [EditorPath.cpp:11-21], [KeyboardFromMidiInput.cpp:66]
- [MidiManager.cpp:363-380]

## Dev Agent Record

### Agent Model Used

Composer 2.5

### Debug Log References

- Audit: `std::mutex` + dual `std::queue` already MPSC — no impl change.
- Stress test: 1000 realtime + 1000 SysEx, 3 threads concurrent, zero loss.

### Completion Notes List

- MPSC contract documented in `MidiOutboundQueue.h` (3 producers + 1 consumer, AD-3).
- Audit finding: mutex MPSC sufficient; AbstractFifo rejected (SPSC).
- Added `testDualProducerConsumerStress` in `MidiOutboundQueueTests.cpp`.
- Full suite 1007 tests pass (macOS ARM Debug).

### File List

- `Source/Core/MIDI/Queue/MidiOutboundQueue.h`
- `Tests/Unit/MidiOutboundQueueTests.cpp`
- `_bmad-output/implementation-artifacts/r-4-midioutboundqueue-mpsc-audit.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`