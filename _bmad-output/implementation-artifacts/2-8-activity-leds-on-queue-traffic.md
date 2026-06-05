---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.8 — Activity LEDs on Queue Traffic
author: BMad Agent
status: done
baseline_commit: 837e423
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md
  - implementation-artifacts/2-7-audio-passthrough-and-peak-indicator.md
  - implementation-artifacts/2-9-wire-midimanager-queue-consumer.md
  - implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
  - project-context.md
created: 2026-06-05
---

# Story 2.8: Activity LEDs on Queue Traffic

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a performer,
I want Instrument and Editor LEDs to reflect MIDI activity,
so that I see which path is active (FR-9, UX-DR3).

## Acceptance Criteria

1. **`Core::MidiActivityTracker`** in `Source/Core/MIDI/MidiActivityTracker.{h,cpp}`, namespace `Core`, zero GUI. Thread-safe activity timestamps for two paths: **`kInstrument`** and **`kEditor`**. Public API:
   - `void notifyActivity(Path path) noexcept` — safe from audio thread, MIDI callback thread, and MIDI manager thread; **no allocations, no locks held across callbacks** (use `std::atomic<juce::int64>` millisecond timestamps via `juce::Time::getMillisecondCounterHiRes()` or equivalent).
   - `float getActivityLevel(Path path) const noexcept` — returns **0…1** decay level computed from elapsed time since last notify (message-thread reads only).
   - Decay constants documented in Dev Notes (see UX pattern below).
2. **Producer hooks (Story 2.3 paths):**
   - `InstrumentMidiForwarder::forward` — after each successful `queue.enqueueRealtime(...)`, call `tracker.notifyActivity(kInstrument)`.
   - `KeyboardFromMidiInput::processIncomingMessage` — same after each allowed enqueue (standalone Keyboard From is instrument path).
   - `EditorPath::enqueueSysEx` and `EditorPath::enqueueProgramChange` — call `tracker.notifyActivity(kEditor)` after enqueue.
3. **Consumer hook (Story 2.9):** `MidiManager::dispatchOutboundMessage` — after a message is accepted for send (post output guard, pre/ post send OK), classify and notify:
   - `MessageCategory::kSysEx` → `kEditor`
   - `MessageCategory::kRealtime` + `isProgramChange()` → `kEditor`
   - `MessageCategory::kRealtime` + note/CC/pitch wheel → `kInstrument`
   - Skip notify when message is dropped (empty SysEx, output unavailable skip).
4. **Do not modify `MidiOutboundQueue` priority, mutex, or message struct** — path is known at producers; realtime queue is shared by instrument notes and editor PC (Story 2.1 design). **Reject** tagging inside the queue without an architecture decision.
5. **`ActivityLed` widget** in `Source/GUI/Widgets/ActivityLed.{h,cpp}` — small square indicator (not a continuous meter). Solid fill when active; dim idle background (mirror `PeakIndicator` pattern). Colour = **`SkinColourId::kEnvelopeDisplayEnvelope`** (same SSOT as peak + envelope curve — Story 2.7). API: `setSkin`, `setUiScale`, `setLevel(float 0…1)`.
6. **Header layout (FR-39 partial):** Add two labelled indicators to `HeaderPanel`:
   - **INSTRUMENT :** label (`PluginDisplayNames::HeaderPanel::kInstrumentLabel`) + `ActivityLed`
   - **EDITOR :** label (`PluginDisplayNames::HeaderPanel::kEditorLabel`) + `ActivityLed`
   - Placement: **after peak indicator**, before SCALE/SKIN (brief header order: routing → audio → **activity LEDs** → scale/skin). Width constants divisible by 4; header height unchanged (`PluginDesignDimensions::GUI::kHeaderHeight`).
7. **Message-thread refresh:** Extend `PluginEditor` timer pattern (reuse existing ~30 Hz timer alongside peak, or single `HeaderRefreshTimer` for peak + both LEDs). Read `processor.getMidiActivityTracker().getActivityLevel(...)` — **never** `repaint()` from audio/MIDI threads.
8. **UX pattern (UX-DR3 — formal UX spec pending, locked for v1 in this story):**
   - **Pulse + decay** (not a counter, not latched ON/OFF).
   - On `notifyActivity`, LED snaps to **1.0**; decays exponentially to 0 when idle.
   - Constants: **`kActivityHoldMs = 150`**, **`kActivityDecayMs = 350`** (document in tracker; tune in Completion Notes if manual UAT feels sluggish).
   - Full labels **INSTRUMENT** / **EDITOR** — not abbreviations (decision log 2026-05-23).
   - Enqueue + dequeue both call `notifyActivity` — rapid re-notify **extends** hold (same path), does not stack brightness above 1.0.
9. **Threading / persistence:** Activity state is **ephemeral** (not APVTS, not session XML) — event-like per D-259 property taxonomy. No `uiMirror` properties.
10. **Tests + build:** `Tests/Unit/MidiActivityTrackerTests.cpp` — notify sets level to 1.0; after simulated elapsed time level decays toward 0; instrument vs editor paths independent. Optional widget-free test that `EditorPath` notify fires on enqueue (mock tracker or spy). Register new `.cpp` in `CMakeLists.txt` plugin + test targets. All existing unit tests pass; Standalone + VST3 build cleanly.

## Tasks / Subtasks

- [x] **Create `MidiActivityTracker`** (AC: #1, #8, #9)
  - [x] `Path` enum, atomics per path, decay math in `getActivityLevel`
  - [x] Header comment: message-thread read, any-producer write

- [x] **Wire producer hooks** (AC: #2)
  - [x] Pass `MidiActivityTracker&` into `InstrumentMidiForwarder::forward` (add parameter) or inject at construction — prefer explicit parameter on `forward()` to keep forwarder stateless
  - [x] `KeyboardFromMidiInput` ctor adds tracker ref; notify on enqueue
  - [x] `EditorPath` ctor adds tracker ref; notify on both enqueue methods
  - [x] Update `PluginProcessor` construction order: create `midiActivityTracker_` before `keyboardFromMidiInput_`, `midiManager`, wire refs

- [x] **Wire consumer hook** (AC: #3)
  - [x] `MidiManager` ctor accepts `MidiActivityTracker&`; notify in `dispatchOutboundMessage` with classification helper `pathForOutboundMessage(msg)`

- [x] **Create `ActivityLed` widget** (AC: #5)
  - [x] Paint: dim background + vertical or square fill proportional to level (match peak aesthetic — solid envelope colour)
  - [x] `setSkin` / `setUiScale` / `setLevel`

- [x] **Extend `HeaderPanel`** (AC: #6)
  - [x] Labels + LEDs; getters `getInstrumentActivityLed()`, `getEditorActivityLed()`
  - [x] `resized()` layout after peak cluster
  - [x] `setSkin` refresh for LEDs

- [x] **Wire `PluginEditor`** (AC: #7)
  - [x] Timer callback updates both LEDs from `getMidiActivityTracker()`
  - [x] `PluginProcessor::getMidiActivityTracker()` public accessor (mirror `getAudioPassthroughProcessor()`)

- [x] **Display names + CMake + tests** (AC: #10)
  - [x] `PluginDisplayNames::HeaderPanel::kInstrumentLabel`, `kEditorLabel`
  - [x] `MidiActivityTrackerTests.cpp`
  - [x] Full test suite + Standalone/VST3 build

### Review Findings

- [x] [Review][Patch] Cast `getMillisecondCounterHiRes()` to `juce::int64` before atomic store [Source/Core/MIDI/MidiActivityTracker.cpp:28-33]
- [x] [Review][Patch] Restore `explicit` on `EditorPath` and `KeyboardFromMidiInput` constructors [Source/Core/MIDI/EditorPath.h:15, KeyboardFromMidiInput.h:18]
- [x] [Review][Patch] Replace wall-clock sleep in `testRapidRenotifyExtendsHold` with `computeLevelFromTimestamp` seam [Tests/Unit/MidiActivityTrackerTests.cpp:403-412]
- [x] [Review][Patch] Extract decay exponent `5.0f` to named constant `kDecayExponent_` [Source/Core/MIDI/MidiActivityTracker.cpp:169]
- [x] [Review][Defer] `memory_order_relaxed` on activity atomics — matches `AudioPassthroughProcessor` peak pattern (Story 2.7); acquire/release optional hardening deferred
- [x] [Review][Defer] `ActivityLed` unused `width_`/`height_`/`uiScale_` — mirrors `PeakIndicator` prior art; cleanup deferred to Story 7.8 header polish
- [x] [Review][Defer] No unit test asserting `MidiManager::dispatchOutboundMessage` notifies tracker — optional coverage; producer + EditorPath tests cover main paths
- [x] [Review][Defer] Clock step-backward guard in `computeLevelFromTimestamp` — extremely rare on monotonic hi-res counter; low priority

## Dev Notes

### Brownfield baseline (verified 2026-06-05 @ `837e423`)

| Component | Current state | File |
|---|---|---|
| `MidiActivityTracker` | **Missing** | — |
| `ActivityLed` / header LEDs | **Missing** — `HeaderPanel` has routing + audio + peak only | `HeaderPanel.{h,cpp}` |
| `MidiOutboundQueue` | No path metadata; shared realtime queue | `MidiOutboundQueue.{h,cpp}` |
| Instrument producer | Enqueues realtime only | `InstrumentMidiForwarder.cpp`, `KeyboardFromMidiInput.cpp` |
| Editor producer | Enqueues SysEx + PC | `EditorPath.cpp` |
| Queue consumer | Drains + dispatches | `MidiManager.cpp:321-364` |
| Peak refresh pattern | 30 Hz private timer in `PluginEditor` | `PluginEditor.cpp:13-31` |
| LED display strings | **Missing** in `PluginDisplayNames` | `PluginDisplayNames.h` |

### UX-DR3 pattern (locked for implementation)

Formal UX deliverable is **backlog** (epics UX-DR3). Until a UX spec ships, implement per PRD FR-9 + decision log D-392/D-067:

| Aspect | Decision |
|---|---|
| Visual | Small **square** LED cell (~8 px design width, same as peak bar width) |
| Colour | `SkinColourId::kEnvelopeDisplayEnvelope` lit; 15% alpha idle background |
| Behaviour | Pulse + decay on queue traffic (enqueue **and** dequeue) |
| Labels | Full words: **INSTRUMENT :** / **EDITOR :** |
| Instrument meaning | Notes, CC, pitch bend outbound (host buffer or Keyboard From) |
| Editor meaning | SysEx + Program Change outbound (parameter edits, patch select, patch/master send) |

PRD user journey (UJ-1): disarmed track + envelope edit → **editor LED** confirms SysEx still flows while instrument LED idle.

### Why not hook only inside `MidiOutboundQueue`?

`enqueueRealtime` serves **both** paths (Story 2.1: editor PC uses realtime queue). Instrument vs editor is only known at the **producer** (or by inspecting message type on **dequeue**). Adding `Path` to every realtime enqueue would work but violates story scope (queue API frozen since 2.1). **Producer notify + dequeue classify** is the minimal correct design.

### `MidiActivityTracker` sketch

```cpp
namespace Core
{
    class MidiActivityTracker
    {
    public:
        enum class Path { kInstrument, kEditor };

        void notifyActivity(Path path) noexcept;
        float getActivityLevel(Path path) const noexcept;

    private:
        inline constexpr static juce::int64 kActivityHoldMs_ = 150;
        inline constexpr static juce::int64 kActivityDecayMs_ = 350;

        std::atomic<juce::int64> instrumentLastNotifyMs_{ 0 };
        std::atomic<juce::int64> editorLastNotifyMs_{ 0 };

        static float levelFromTimestamp(juce::int64 lastNotifyMs) noexcept;
    };
}
```

Decay: if `elapsed <= kActivityHoldMs` → return `1.0f`; else exponential falloff to 0 over `kActivityDecayMs` (same family as peak ballistics — Story 2.7).

### Dequeue classification helper

```cpp
MidiActivityTracker::Path pathForOutboundMessage(const MidiOutboundQueue::Message& msg) noexcept
{
    if (msg.category == MidiOutboundQueue::MessageCategory::kSysEx)
        return MidiActivityTracker::Path::kEditor;

    if (msg.midiMessage.isProgramChange())
        return MidiActivityTracker::Path::kEditor;

    return MidiActivityTracker::Path::kInstrument;
}
```

Reuse the same allow-list philosophy as `InstrumentMidiForwarder::isAllowedInstrumentMessage` — anything else realtime on dequeue should not happen if producers are disciplined; if it does, default `kInstrument` or skip notify (document choice in Completion Notes).

### Wiring diagram

```
processBlock / KeyboardFrom ──► InstrumentMidiForwarder / KeyboardFromMidiInput
                                      │ notify kInstrument
                                      ▼
EditorPath (SysEx/PC) ─────────────► MidiOutboundQueue ◄── MidiManager::run dequeue
       │ notify kEditor                      │ notify (classified)
       ▼                                     ▼
              MidiActivityTracker (atomics)
                       │ getActivityLevel (message thread)
                       ▼
              PluginEditor Timer ~30 Hz
                       ▼
              HeaderPanel ActivityLed × 2
```

### `PluginProcessor` construction (order matters)

```cpp
, midiActivityTracker_{ std::make_unique<Core::MidiActivityTracker>() }
, outboundQueue_{ std::make_unique<Core::MidiOutboundQueue>() }
, keyboardFromMidiInput_{ std::make_unique<Core::KeyboardFromMidiInput>(
      *outboundQueue_, *midiActivityTracker_) }
, midiManager(std::make_unique<MidiManager>(apvts, *outboundQueue_, *midiActivityTracker_))
```

`processBlock`:

```cpp
instrumentForwarder_->forward(midiMessages,
                              getInstrumentPathEnabled(midiMessages),
                              *outboundQueue_,
                              *midiActivityTracker_);
```

### Header layout (post 2.7)

Current left→right (`HeaderPanel.cpp:167-185`): MIDI FROM | MIDI TO | KEYBOARD FROM | AUDIO FROM | INPUT GAIN | peak | SCALE | SKIN | UI Elements.

Insert after peak:

```
… | peak | INSTRUMENT : [led] | EDITOR : [led] | SCALE | SKIN | …
```

Suggested design widths (tune in `resized`, divisible by 4):

| Constant | Value |
|---|---|
| `kInstrumentLabelWidth_` | 96 |
| `kEditorLabelWidth_` | 72 |
| `kActivityLedWidth_` | 8 |

Compress SCALE/SKIN combo widths only if crowding breaks at 200% UI scale — same compromise philosophy as Stories 2.7 / 2.9b.

### Prior art — copy these patterns

| Pattern | Reference |
|---|---|
| Core meter + GUI widget split | `AudioPassthroughProcessor` + `PeakIndicator` (Story 2.7) |
| Header timer refresh | `PluginEditor::PeakRefreshTimer` |
| Envelope colour SSOT | `PeakIndicator.cpp:22`, `EnvelopeDisplay.cpp:199` |
| Producer → queue | `EditorPath.cpp`, `InstrumentMidiForwarder.cpp` |
| Consumer dispatch | `MidiManager::dispatchOutboundMessage` |
| Display names | `PluginDisplayNames::HeaderPanel::*` |
| Unit test style | `AudioPassthroughProcessorTests.cpp` |

### Files to touch

| File | Action |
|---|---|
| `Source/Core/MIDI/MidiActivityTracker.{h,cpp}` | **NEW** |
| `Source/Core/Audio/InstrumentMidiForwarder.{h,cpp}` | Add tracker param to `forward` |
| `Source/Core/MIDI/KeyboardFromMidiInput.{h,cpp}` | Tracker ref + notify |
| `Source/Core/MIDI/EditorPath.{h,cpp}` | Tracker ref + notify |
| `Source/Core/MIDI/MidiManager.{h,cpp}` | Tracker ref + dequeue notify |
| `Source/Core/PluginProcessor.{h,cpp}` | Own tracker, accessor, wire ctor/processBlock |
| `Source/GUI/Widgets/ActivityLed.{h,cpp}` | **NEW** |
| `Source/GUI/Panels/.../HeaderPanel.{h,cpp}` | Labels + LEDs + layout |
| `Source/GUI/PluginEditor.{h,cpp}` | Timer updates LEDs |
| `Source/Shared/Definitions/PluginDisplayNames.h` | `kInstrumentLabel`, `kEditorLabel` |
| `CMakeLists.txt` | New sources + tests |
| `Tests/Unit/MidiActivityTrackerTests.cpp` | **NEW** |
| `Tests/Unit/InstrumentMidiForwarderTests.cpp` | Extend: tracker notified on forward (optional) |
| `Tests/Unit/EditorPathTests.cpp` | Extend: tracker notified on enqueue (optional) |

### Explicitly out of scope

- Footer / `uiMessageText` → Story 7.8
- Logo popup relocation of Skin/Scale → Story 7.8 (FR-41)
- New skin colour tokens for LED — reuse envelope colour unless UX spec adds distinct instrument/editor colours
- Inbound MIDI From activity (receive LED) — FR-9 is **outbound queue** traffic only
- APVTS persistence of LED state
- `MidiOutboundQueue` API changes
- Hardware SM-1 manual UAT checklist (document in Completion Notes after dev)

### Relationship to adjacent stories

| Story | Relationship |
|---|---|
| **2.3** | Defines instrument vs editor producers — hook points |
| **2.7** | Peak indicator + header layout slot immediately before LEDs |
| **2.9** | Consumer must be draining for dequeue notify to fire during hardware send |
| **2.9b** | Routing combos must remain functional after layout insert |
| **7.8** | Full header shell polish; may relocate Skin/Scale |

### Testing standards

- JUCE `UnitTest`, static registration
- `MidiActivityTrackerTests`: synthetic time — call `notifyActivity`, read level immediately = 1.0; simulate elapsed ms (extract package-visible test seam or inject clock lambda if needed)
- Manual smoke (SM-1 adjunct): Standalone with ports — play Keyboard From → instrument LED pulses; tweak APVTS slider → editor LED pulses; disarm DAW track in plugin mode → instrument idle, editor active on edit

### Previous story intelligence (2.7)

From [2-7-audio-passthrough-and-peak-indicator.md]:

- Peak uses 30 Hz timer — **merge** with LED refresh to one timer class to avoid duplicate timers.
- Envelope colour trap: use `kEnvelopeDisplayEnvelope`, not track-generator colours.
- Header crowding accepted until 7.8 — compress widths, not header height.
- Core ↛ GUI: all activity logic in `MidiActivityTracker`; widgets read levels only.

From 2.7 code review themes (apply proactively):

- Sanitize non-finite inputs N/A here; do guard null tracker refs in tests only.
- Never read `apvts.state` on audio thread for LED logic.

### Git intelligence

Recent Epic 2 commits:

- `837e423` — Story 2.7 audio passthrough + peak + header audio cluster
- `bd49cd9` — Story 2.9b header routing + Keyboard From
- `e033d68` — Story 2.9 queue consumer

Pattern: Core service + thin GUI widget + header wiring + unit tests + CMake; English imperative commit when user requests.

### Project Structure Notes

- Paths use `Source/Core/MIDI/` for tracker (peer to `EditorPath`, not inside `Queue/`).
- Widgets in `Source/GUI/Widgets/` namespace `tss::`.
- All user-visible strings in `PluginDisplayNames.h`.
- Allman braces, `k` prefix constants, private `_` suffix.

### References

- [Source: epics.md § Story 2.8, UX-DR3]
- [Source: prd.md § FR-9, FR-39, UJ-1]
- [Source: .decision-log.md § D-392 queue + LEDs, D-067 full labels]
- [Source: architecture.md § AD-3 dual producers, §4.2 dual-role]
- [Source: project-context.md § MIDI architecture, threading]
- [Source: implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md]
- [Source: implementation-artifacts/2-7-audio-passthrough-and-peak-indicator.md]
- [Source: implementation-artifacts/2-9-wire-midimanager-queue-consumer.md]
- [Source: MidiOutboundQueue.h], [InstrumentMidiForwarder.cpp], [EditorPath.cpp], [MidiManager.cpp:335-364]
- [Source: HeaderPanel.cpp:167-185], [PluginEditor.cpp:13-31], [PeakIndicator.cpp]

## Dev Agent Record

### Agent Model Used

Claude claude-4.6-sonnet-medium-thinking (Cursor)

### Debug Log References

- Decay: `exp(-5 * decayRatio)` — level ≈ 0 at `kActivityDecayMs` (350 ms) after hold (150 ms).
- Dequeue classify: unknown realtime → `kInstrument` (default per story sketch).
- `PeakRefreshTimer` → `HeaderRefreshTimer` — single 30 Hz timer for peak + both LEDs.

### Completion Notes List

- Implemented `Core::MidiActivityTracker` with lock-free atomics, hold 150 ms + exponential decay 350 ms.
- Producer notify on `InstrumentMidiForwarder`, `KeyboardFromMidiInput`, `EditorPath` enqueue.
- Consumer notify in `MidiManager::dispatchOutboundMessage` after successful send; skipped when output unavailable or empty SysEx.
- `ActivityLed` widget + header layout (INSTRUMENT / EDITOR labels after peak, before SCALE/SKIN).
- `HeaderRefreshTimer` at 30 Hz reads tracker on message thread.
- Tests: `MidiActivityTrackerTests` + tracker notify tests in `InstrumentMidiForwarderTests` / `EditorPathTests`; all unit tests pass; Standalone + VST3 build clean.
- Manual UAT (SM-1): Standalone — Keyboard From → instrument LED; APVTS edit → editor LED; plugin mode disarmed → editor only.

### File List

- Source/Core/MIDI/MidiActivityTracker.h
- Source/Core/MIDI/MidiActivityTracker.cpp
- Source/Core/Audio/InstrumentMidiForwarder.h
- Source/Core/Audio/InstrumentMidiForwarder.cpp
- Source/Core/MIDI/KeyboardFromMidiInput.h
- Source/Core/MIDI/KeyboardFromMidiInput.cpp
- Source/Core/MIDI/EditorPath.h
- Source/Core/MIDI/EditorPath.cpp
- Source/Core/MIDI/MidiManager.h
- Source/Core/MIDI/MidiManager.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Source/GUI/Widgets/ActivityLed.h
- Source/GUI/Widgets/ActivityLed.cpp
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
- Source/GUI/PluginEditor.h
- Source/GUI/PluginEditor.cpp
- Source/Shared/Definitions/PluginDisplayNames.h
- CMakeLists.txt
- Tests/Unit/MidiActivityTrackerTests.cpp
- Tests/Unit/InstrumentMidiForwarderTests.cpp
- Tests/Unit/EditorPathTests.cpp
- Tests/Unit/KeyboardFromMidiInputTests.cpp
- Tests/Unit/MidiManagerTests.cpp
- Tests/Unit/MidiPortRoutingPropertyTests.cpp
- Tests/Unit/PatchParameterSysExDispatcherTests.cpp
- Tests/Unit/MasterParameterSysExDispatcherTests.cpp
- Tests/Unit/MatrixModBusParameterSysExDispatcherTests.cpp

### Change Log

- 2026-06-05: Code review — 4 patches applied (int64 cast, explicit ctors, deterministic re-notify test, kDecayExponent_).

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Story status: **done**
- Next workflow: `code-review` when complete
