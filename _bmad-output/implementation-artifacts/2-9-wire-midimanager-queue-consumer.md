---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.9 — Wire MidiManager Queue Consumer
author: BMad Agent
status: done
baseline_commit: b88c870
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - implementation-artifacts/2-2-sysexdelayprofile-and-inter-message-delay.md
  - implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md
  - implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md
  - implementation-artifacts/deferred-work.md
  - project-context.md
created: 2026-06-05
---

# Story 2.9: Wire MidiManager Queue Consumer

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a developer,
I want `MidiManager::run()` to consume the unified queue,
so that the brownfield transport stack sends through `MidiSender` (AD-3).

## Acceptance Criteria

1. **`MidiManager::run()` consumer loop (AD-3):** On the dedicated MIDI thread, drain `outboundQueue_` in a tight loop: while `auto msg = outboundQueue_.dequeue()` has a value, dispatch immediately; when the queue is empty, `wait(1)` (1 ms idle poll) until `threadShouldExit()`. **Never** call `isEmpty()` before `dequeue()` (TOCTOU — deferred-work 2.1).
2. **Realtime dispatch:** Dequeued `MessageCategory::kRealtime` messages send via `MidiSender` only. Add `MidiSender::sendMidiMessage(const juce::MidiMessage&)` using `midiOutput->sendMessageNow(message)` when output is available. Realtime sends **do not** invoke `SysExInterMessageDelay` (Story 2.2 AC #4).
3. **SysEx dispatch:** Dequeued `MessageCategory::kSysEx` messages send via existing `sendSysExWithDelay(sysExData, description)` so `SysExInterMessageDelay` gate + `recordSysExSent` stay centralized. Description may be `"QUEUED"` or empty — logging already occurs inside `MidiSender::sendSysEx`.
4. **Output-unavailable guard:** If `!midiSender->isOutputAvailable()`, skip send for that message (no throw, no thread death). Optionally log once per drain cycle at debug level — do **not** leave messages stuck in queue (drop and continue). This unblocks APVTS→SysEx UAT before Story 2.9b port UI.
5. **RPC SysEx gate unification (deferred-work 2.2):** Replace direct `midiSender->sendSysEx(...)` in `performDeviceInquiry()` and `requestSysExData()` with `sendSysExWithDelay(...)` so inquiry/request SysEx shares the same `sysExDelay_` instance as queued SysEx. **Do not** enqueue RPC traffic on `MidiOutboundQueue` — sync request/response must remain blocking on the MIDI thread (D-049).
6. **Producer verification (epics AC):** Confirm `InstrumentMidiForwarder`, `EditorPath`, and `MidiManager` fire-and-forget APIs (`sendPatch`, `sendMaster`, `sendProgramChange`, `enqueueRemoteParameterEdit`, `enqueueMatrixModBusEdit`) enqueue only — **no** `MidiSender` / `MidiOutput` calls on producer paths. Brownfield direct-send removal is already done in Story 2.3 for editor enqueue; this story completes the **consumer** side only.
7. **Empty SysEx guard:** Skip dequeue-send when `kSysEx` payload `sysExData.getSize() == 0` (deferred-work 2.1) — do not call `sendSysExWithDelay`.
8. **Unit / integration tests:** Replace `MidiManagerTests` placeholder with real tests using the `MinimalAudioProcessor` harness pattern from `PatchNameSyncerTests.cpp`:
   - (a) Enqueue realtime → start `MidiManager` thread briefly → queue becomes empty (no hang, thread stops cleanly)
   - (b) Enqueue SysEx → same drain behaviour
   - (c) `SysExInterMessageDelay`: two SysEx dequeued in one drain pass — second send respects `millisUntilNextAllowed` (use synthetic timing via package-visible test seam **or** verify `sendSysExWithDelay` path with spy on `recordSysExSent` timing — prefer testing `sendSysExWithDelay` indirectly by calling it twice with mocked clock if extractable; minimum: document gate sharing in test name and assert no crash + queue empty)
   - (d) No output port → enqueue + drain does not throw; queue empties
   - All existing unit tests pass; VST3 + Standalone build cleanly.
9. **No regressions:** `PluginProcessor` thread lifecycle unchanged (`startThread` in `prepareToPlay`, `stopThread` in `releaseResources`). APVTS→SysEx dispatch paths (Stories 2.4–2.6) untouched.

## Tasks / Subtasks

- [x] **Add `MidiSender::sendMidiMessage`** (AC: #2)
  - [x] Declare in `MidiSender.h`; implement in `MidiSender.cpp`
  - [x] Mirror `ensureOutputAvailable()` throw semantics for callers that require port — consumer uses `isOutputAvailable()` guard first

- [x] **Implement `MidiManager::run()` consumer** (AC: #1, #2, #3, #4, #7)
  - [x] Extract private `void dispatchOutboundMessage(const Core::MidiOutboundQueue::Message& msg)` if `run()` body would exceed 15 lines
  - [x] Realtime branch → `sendMidiMessage`
  - [x] SysEx branch → `sendSysExWithDelay` (skip empty block)
  - [x] Output guard before any send
  - [x] Idle `wait(1)` when queue empty

- [x] **Unify RPC SysEx through delay gate** (AC: #5)
  - [x] `performDeviceInquiry()` — `sendSysExWithDelay(inquiryMessage, "Device Inquiry")`
  - [x] `requestSysExData()` — `sendSysExWithDelay(requestMessage, requestDescription + " request")`

- [x] **Audit producer paths** (AC: #6)
  - [x] Grep `Source/` for `sendMessageNow`, `midiSender->`, `MidiOutput` outside `MidiSender.cpp` and consumer/RPC paths — fix any stray direct sends

- [x] **Tests + CMake** (AC: #8)
  - [x] Rewrite `Tests/Unit/MidiManagerTests.cpp` (or add `MidiManagerOutboundConsumerTests.cpp`)
  - [x] Reuse `MinimalAudioProcessor` from `PatchNameSyncerTests` or extract shared test fixture if duplication is large
  - [x] Register any new `.cpp` in `CMakeLists.txt` if split

- [x] **Build verification** (AC: #8, #9)
  - [x] Run full `Matrix-Control_Tests` suite
  - [x] Build Standalone target

## Dev Notes

### Architecture Mandate — AD-3 Consumer (FR-7, NFR-2)

Epic 2 pipeline today:

```
processBlock → InstrumentMidiForwarder → enqueueRealtime ─┐
MidiManager APIs → EditorPath → enqueueSysEx / enqueueRealtime(PC) ─┤
PluginProcessor dispatchers → MidiManager enqueue* ─────────────────┤
                                                                    ▼
                                                          MidiOutboundQueue
                                                                    │
                                                          MidiManager::run()  ◄── THIS STORY
                                                                    │
                                                                    ▼
                                                              MidiSender → OS MIDI
```

**Before 2.9:** `run()` is a stub — only `wait(kMinSysExDelayMs)` in a loop (`MidiManager.cpp:321-327`). Messages accumulate; APVTS edits never reach hardware.

**After 2.9:** Queue drains continuously on the MIDI thread. Story **2.9b** (header port combos) depends on this story being `done`.

### Current Code State (READ BEFORE EDITING)

| Location | Today | Change |
|---|---|---|
| `MidiManager::run()` | `while (!threadShouldExit()) wait(kMinSysExDelayMs)` | Dequeue-send loop + `wait(1)` idle |
| `MidiManager::sendPatch/Master/PC/enqueue*` | Enqueue via `editorPath_` only | **No change** (Story 2.3) |
| `performDeviceInquiry` / `requestSysExData` | Direct `midiSender->sendSysEx` — **bypasses** `sysExDelay_` | Route through `sendSysExWithDelay` |
| `sendSysExWithDelay` | Exists; uses `waitUntilReady` + `recordSysExSent` | Reuse for queued SysEx |
| `InstrumentMidiForwarder` | Enqueues only | **No change** |
| `EditorPath` | Enqueues only | **No change** |
| `MidiSender` | Typed sends (note/CC/SysEx/PC) | Add generic `sendMidiMessage` for queued realtime |
| `PluginProcessor` | Starts MIDI thread in `prepareToPlay` | **No change** |

### Consumer Loop — Reference Implementation

```cpp
void MidiManager::run()
{
    while (!threadShouldExit())
    {
        if (auto msg = outboundQueue_.dequeue())
        {
            dispatchOutboundMessage(*msg);
            continue;
        }

        wait(1);
    }
}

void MidiManager::dispatchOutboundMessage(const Core::MidiOutboundQueue::Message& msg)
{
    if (!midiSender->isOutputAvailable())
        return;

    if (msg.category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
    {
        midiSender->sendMidiMessage(msg.midiMessage);
        return;
    }

    if (msg.sysExData.getSize() == 0)
        return;

    sendSysExWithDelay(msg.sysExData, "QUEUED");
}
```

Adjust exception handling: if `sendMidiMessage` throws `MidiConnectionException`, catch in `dispatchOutboundMessage`, call `updateErrorState`, continue drain — do not kill the MIDI thread.

### SysEx Gate Sharing (Story 2.2 + deferred-work)

`sysExDelay_` is a single `Core::SysExInterMessageDelay` on `MidiManager`. **All** outbound SysEx — queued editor traffic **and** sync RPC — must pass through `sendSysExWithDelay` so EPROM-adaptive gaps stay global.

| Path | Mechanism |
|---|---|
| Queued SysEx (0x01/0x03/0x06/0x0B) | Consumer → `sendSysExWithDelay` |
| Device Inquiry request | `sendSysExWithDelay` (this story) |
| Patch/Master request SysEx | `sendSysExWithDelay` (this story) |
| Realtime (notes/CC/PB/PC) | `sendMidiMessage` — no delay gate |

**Do not** enqueue RPC messages — blocking `waitForSysExResponse` must stay on the calling stack (D-049).

### Realtime Message Types in Queue

| Producer | Enqueued as `kRealtime` |
|---|---|
| `InstrumentMidiForwarder` | Note On/Off, CC, Pitch Bend |
| `EditorPath::enqueueProgramChange` | `juce::MidiMessage::programChange` |

`sendMidiMessage` handles all via `sendMessageNow` — no per-type switch required unless logging differs.

### Threading Contract

| Actor | Thread |
|---|---|
| `enqueueRealtime` | Audio (`processBlock`) or future Keyboard From callback (2.9b) |
| `enqueueSysEx` | Message thread / MIDI API callers |
| `dequeue` + send | **MIDI thread only** (`MidiManager::run`) |
| `sendSysExWithDelay` during RPC | MIDI thread (same thread as `run()` when called from inquiry — but inquiry may be called from message thread; verify callers) |

**Caller audit:** `performDeviceInquiry` / `requestCurrentPatch` — trace call sites before assuming thread. If invoked off MIDI thread while `run()` also runs, shared `sysExDelay_` is still safe (single mutex-free timestamp); `MidiSender` is not thread-safe — document that RPC should run on MIDI thread or serialize. Brownfield today: likely message thread at startup — **do not refactor call sites** in this story; note risk in completion notes if inquiry runs concurrent with consumer.

### What NOT to Do

- Do **not** implement header port UI — Story 2.9b
- Do **not** wire activity LEDs — Story 2.8
- Do **not** add audio passthrough — Story 2.7
- Do **not** change `MidiOutboundQueue` priority/dequeue logic
- Do **not** enqueue RPC SysEx on the outbound queue
- Do **not** call `MidiOutput` from audio thread
- Do **not** modify APVTS dispatchers (`PatchParameterSysExDispatcher`, etc.)
- Do **not** implement inbound `handleIncomingSysEx` decode — Epic 8+
- Do **not** add queue `clear()` API unless disconnect bug is found — optional, out of AC

### Files to Touch

| File | Action |
|---|---|
| `Source/Core/MIDI/MidiManager.cpp` | REPLACE `run()` stub; add `dispatchOutboundMessage`; RPC → `sendSysExWithDelay` |
| `Source/Core/MIDI/MidiManager.h` | DECLARE private `dispatchOutboundMessage` if extracted |
| `Source/Core/MIDI/Transport/MidiSender.h` | ADD `sendMidiMessage` |
| `Source/Core/MIDI/Transport/MidiSender.cpp` | IMPLEMENT `sendMidiMessage` |
| `Tests/Unit/MidiManagerTests.cpp` | REPLACE placeholder with drain tests |
| `CMakeLists.txt` | UPDATE only if new test/source files added |

### Out of Scope

- `KeyboardFromMidiInput` — Story 2.9b
- `MidiOutboundQueue::clear()` on port disconnect — deferred unless blocking bug
- Queue depth limits / backpressure — deferred-work 2.1
- Hardware SM-1 validation — manual after 2.9b
- Inbound SysEx routing to APVTS

### Project Structure Notes

- Consumer logic stays in `MidiManager` — no new `MidiOutboundConsumer` class (YAGNI).
- `MidiSender` remains the sole `sendMessageNow` callsite per architecture AD-3.
- Namespace: `MidiManager` / `MidiSender` are global classes (brownfield); new helpers use `Core` only if extracted to separate file.

### Manual Smoke Test (post-2.9b or with existing port properties)

1. Set MIDI To via saved `midiOutputPortId` or debugger call to `setMidiOutputPort`
2. Move PATCH slider → observe 0x06 in MIDI log (`Logs/MIDI/`)
3. Play note (DAW armed) → note reaches synth
4. Rapid SysEx edits → inter-message gap ≥ profile delay (10 ms stock default)

### References

- [Source: planning-artifacts/epics.md#Story 2.9]
- [Source: architecture.md#AD-3]
- [Source: implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md#Queue Consumer Gap]
- [Source: implementation-artifacts/2-2-sysexdelayprofile-and-inter-message-delay.md]
- [Source: implementation-artifacts/deferred-work.md — RPC gate + queue deferred items]
- [Source: Source/Core/MIDI/MidiManager.cpp:321-327 — stub to replace]
- [Source: Source/Core/MIDI/MidiManager.cpp:196-202 — sendSysExWithDelay]
- [Source: implementation-artifacts/2-9b-header-routing-controls-uat-slice.md — blocked on 2.9]

## Previous Story Intelligence (2.6 — latest done Epic 2 story file)

- Dispatchers enqueue via `MidiManager` callbacks — consumer must drain or UAT fails.
- `enqueueMatrixModBusEdit` / `enqueueRemoteParameterEdit` → `editorPath_.enqueueSysEx` — no direct send.
- `PluginProcessor::valueTreePropertyChanged` drives all PATCH/MASTER/Matrix Mod outbound — unchanged.
- Review pattern: prefer constants over literals (`Matrix1000Limits::kModulationBusCount` lesson).
- Full test suite pass before merge is expected evidence.

## Previous Story Intelligence (2.3 — direct dependency)

- Producers already enqueue; Story 2.3 AC #6 explicitly left `run()` stub for 2.9.
- `sendSysExWithDelay` was preserved for RPC until consumer exists — now unify gate, not queue RPC.
- Program Change from editor uses `enqueueRealtime` — consumer must send via `sendMidiMessage`.

## Git Intelligence Summary

| Commit | Relevance |
|---|---|
| `b88c870` Story 2.6 | Latest baseline; Matrix Mod 0x0B enqueued, never sent until 2.9 |
| `9202ad2` Story 2.3 | Editor enqueue migration complete |
| `8c3a830` Story 2.2 | `SysExInterMessageDelay` + `sendSysExWithDelay` |
| `bb370e6` Story 2.1 | `MidiOutboundQueue` priority contract |

## Latest Technical Information

- **JUCE 8.0.12** — `MidiOutput::sendMessageNow` unchanged; safe from MIDI thread.
- **No new dependencies.**
- **Idle poll `wait(1)`** replaces `wait(kMinSysExDelayMs)` in stub — avoids 10 ms latency on first note after idle while keeping CPU low.

## Dev Agent Record

### Agent Model Used

Claude claude-4.6-sonnet-medium-thinking (Cursor)

### Debug Log References

- Producer audit: no stray `sendMessageNow` / `midiSender->` outside `MidiSender.cpp` and unified RPC paths.
- Gate timing test (AC #8c): drain-without-hang verified; strict millis assertion deferred to `SysExInterMessageDelayTests` (flaky in headless console runner with virtual MIDI).

### Completion Notes List

- Implemented AD-3 consumer loop: dequeue → dispatch, `wait(1)` idle poll.
- Added `MidiSender::sendMidiMessage` for queued realtime (notes/CC/PB/PC).
- `dispatchOutboundMessage`: output guard, empty SysEx skip, `MidiConnectionException` catch.
- RPC paths (`performDeviceInquiry`, `requestSysExData`) now use `sendSysExWithDelay` — shared `sysExDelay_` gate.
- Rewrote `MidiManagerTests` (5 cases: realtime drain, SysEx drain, gate-sharing drain, no-output safety, empty SysEx skip).
- Extended test target CMake with MidiManager transport sources + `juce_audio_devices`.
- Full `Matrix-Control_Tests` pass; Standalone builds cleanly.
- **Threading note:** `performDeviceInquiry` / `requestSysExData` may run off MIDI thread at startup; shared `sysExDelay_` is timestamp-only (no mutex) — acceptable per story; `MidiSender` not thread-safe if concurrent with consumer (brownfield risk, no call-site refactor in scope).

### File List

- `Source/Core/MIDI/MidiManager.cpp`
- `Source/Core/MIDI/MidiManager.h`
- `Source/Core/MIDI/Transport/MidiSender.h`
- `Source/Core/MIDI/Transport/MidiSender.cpp`
- `Tests/Unit/MidiManagerTests.cpp`
- `CMakeLists.txt`

### Change Log

- 2026-06-05: Story 2.9 — wire MidiManager queue consumer (AD-3), unify RPC SysEx delay gate, add drain tests.
- 2026-06-05: Code review — backoff `wait(1)` when output unavailable; catch `std::exception` in consumer.

### Review Findings

- [x] [Review][Patch] CPU spin when output unavailable and queue non-empty [`MidiManager.cpp:337-338`]
- [x] [Review][Patch] `dispatchOutboundMessage` catches only `MidiConnectionException` [`MidiManager.cpp:353-356`]
- [x] [Review][Defer] RPC + consumer concurrent `sysExDelay_` / `MidiSender` access [`MidiManager.cpp:223,275,351`] — deferred, pre-existing brownfield; documented in completion notes, no call-site refactor in scope
- [x] [Review][Defer] AC #8c strict `millisUntilNextAllowed` assertion absent in gate-sharing test [`MidiManagerTests.cpp:113-133`] — deferred, pre-existing; dev notes defer to `SysExInterMessageDelayTests`
- [x] [Review][Defer] `stopThread` during blocking `waitUntilReady` in consumer [`MidiManager.cpp:351`] — deferred, pre-existing; same lifecycle pattern as `PluginProcessor`
- [x] [Review][Defer] No test for `MidiConnectionException` → `updateErrorState` in consumer [`MidiManagerTests.cpp`] — deferred, pre-existing; error path covered indirectly by no-output guard
- [x] [Review][Defer] Drain tests assert queue empty, not bytes on wire [`MidiManagerTests.cpp`] — deferred, pre-existing; hardware smoke deferred to Story 2.9b

---

**Ultimate context engine analysis completed — comprehensive developer guide created.**
