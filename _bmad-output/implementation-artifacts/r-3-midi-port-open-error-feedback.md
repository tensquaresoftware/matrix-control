---
organization: Ten Square Software
project: Matrix-Control
title: Story R-3 — MIDI Port Open Error Feedback
author: BMad Agent
status: done
baseline_commit: f851b78
sources:
  - planning-artifacts/sprint-change-proposal-2026-06-06.md
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/r-2-hardware-latency-gui.md
  - implementation-artifacts/r-1-remove-plugin-audio-input-bus.md
  - project-context.md
created: 2026-06-06
updated: 2026-06-06
---

# Story R-3: MIDI Port Open Error Feedback

Status: done

<!-- FR-44. Sprint Change Proposal 2026-06-06. Prerequisite for R-5 (Windows MIDI docs link from in-app message). R-1 and R-2 done. -->

## Story

As a Windows user sharing MIDI ports with my DAW,
I want clear GUI feedback when port open fails,
so that I can diagnose exclusive-access conflicts (FR-44).

## Acceptance Criteria

### AC 1 — Footer propagation on port open failure

1. When **`MidiInputPort::openPort`** or **`MidiOutputPort::openPort`** fails (device found but JUCE returns `nullptr`, or device ID not in available list), **`MidiManager::setMidiInputPort` / `setMidiOutputPort`** propagate a non-blocking footer message via **`ExceptionPropagator::propagateToApvts`** → `uiMessageText` / `uiMessageSeverity` = `"error"`.
2. **Do not** use `updateErrorState` (`lastError` / `errorType`) for port-open failures — that path is for SysEx/runtime errors; footer SSOT for user-facing port errors is **`ExceptionPropagator`** (FR-44, same as `WidgetFactoryValidator`).
3. **Empty device ID** (clear port) remains success — no footer message.
4. **Success-only APVTS port ID write** in `PluginProcessor::setMidiInputPort` / `setMidiOutputPort` unchanged — failed open must **not** update `midiInputPortId` / `midiOutputPortId` (existing behaviour in `MidiPortRoutingPropertyTests`).

### AC 2 — Message content

1. Footer text **names the port** using human-readable **device name** (resolve from `juce::MidiInput/Output::getAvailableDevices()` when ID matches; fallback to raw identifier).
2. Distinguish direction: **"MIDI From"** (input) vs **"MIDI To"** (output).
3. Failure kinds:
   - **Device not found** — ID absent from current device list (e.g. unplugged interface).
   - **Open rejected** — device listed but `openDevice` returned `nullptr` (typical Windows exclusive access).
4. On **`JUCE_WINDOWS`**, **open rejected** messages append hint: port may be in use by another app (DAW); suggest loopMIDI / closing other app (full guide deferred to **R-5** — no doc link required in this story).
5. On macOS/Linux, open rejected hint: port may be in use by another application (no Windows-specific loopMIDI text).

### AC 3 — MidiLogger

1. Log structured error on failure: direction, port name/ID, **failure reason code** (`not_found` | `open_rejected`).
2. JUCE `openDevice` does not expose OS error codes — log reason enum + port metadata; do **not** invent Win32 error numbers.

### AC 4 — Testable helpers + unit tests

1. Pure formatter in **`Core::MidiPortOpenFeedback`** (`Source/Core/MIDI/MidiPortOpenFeedback.h/.cpp`):
   - `enum class FailureReason { kNotFound, kOpenRejected }`
   - `formatFooterMessage(isInput, portDisplayName, FailureReason)` — platform-conditional hint for `kOpenRejected`
   - `propagateOpenFailure(apvts, message)` — wraps `ExceptionPropagator` + `WidgetFactoryException` severity Error
2. **`Tests/Unit/MidiPortOpenFeedbackTests.cpp`**: formatter strings for input/output × both failure reasons; Windows vs non-Windows hint branch (use `#if JUCE_WINDOWS` in test expectations).
3. **`Tests/Unit/MidiPortOpenFailurePropagationTests.cpp`**: call `MidiManager::setMidiOutputPort("matrix-control-nonexistent-…")` → assert `apvts.state.getProperty("uiMessageText")` non-empty and contains port direction / failure wording; **`midiOutputPortId` unchanged**.
4. Register new sources in **`CMakeLists.txt`** `Matrix-Control_Tests` target.
5. All existing unit tests pass.

### AC 5 — Scope boundaries

1. **`KeyboardFromMidiInput::setPort`** — out of scope (direct JUCE open, no `MidiInputPort` wrapper); R-5 docs cover general Windows MIDI conflicts.
2. **No** `Documentation/` guide in this story (R-5).
3. **No** footer link to docs yet (R-5 adds link from in-app message).

## Tasks / Subtasks

- [x] **Port failure classification** (AC: #1, #3)
  - [x] Add `MidiPortOpenResult` with reason from `MidiInputPort` / `MidiOutputPort::openPort`
  - [x] Resolve display name in port classes before logging/propagating

- [x] **Feedback helper + MidiManager wiring** (AC: #1, #2, #3)
  - [x] `MidiPortOpenFeedback.h/.cpp` — format + propagate
  - [x] `MidiManager::setMidiInputPort` / `setMidiOutputPort` call propagate on failure
  - [x] Structured `MidiLogger::logError` with reason code

- [x] **Unit tests** (AC: #4)
  - [x] `MidiPortOpenFeedbackTests.cpp`
  - [x] Propagation test via MidiManager + nonexistent device ID
  - [x] CMakeLists update

- [x] **Build verification** (AC: #4)
  - [x] Run `Matrix-Control_Tests`
  - [x] macOS-ARM-Debug build (VST3 + Standalone)

## Dev Notes

### Problem / context

Today port open failures only hit **`MidiLogger::logError`** in port classes — **no footer feedback**. User selects port in header → silent failure. Windows winMM often returns `nullptr` when DAW holds exclusive access.

**Prerequisite for R-5:** in-app message will later link to Windows MIDI setup guide.

### Existing infrastructure (reuse)

| Component | Role |
|---|---|
| `ExceptionPropagator` | Writes `uiMessageText`, `uiMessageSeverity` on APVTS |
| `FooterPanel` | Listens APVTS → paints message with severity |
| `WidgetFactoryException` | Message + severity for propagator |
| `MidiPortRoutingPropertyTests` | Success-only APVTS write — preserved |

### References

- [sprint-change-proposal-2026-06-06.md § Story R-3]
- [epics.md Epic R, Story R-3]
- [prd.md § FR-44 Footer messaging]
- [r-2-hardware-latency-gui.md] — test/build pattern
- [MidiPortRoutingPropertyTests.cpp] — APVTS success-only contract

## Dev Agent Record

### Agent Model Used

Claude (Cursor agent)

### Debug Log References

- Propagation test helpers must return `bool` (not `void`) for `expect(!...)` usage.

### Completion Notes List

- `MidiPortOpenResult` + `Core::MidiPortOpenFeedback` — format, log, propagate via `ExceptionPropagator`.
- `MidiManager` reports failures on input/output port set; no `updateErrorState` for port opens.
- Footer messages name port + direction; Windows open-rejected hint mentions loopMIDI (R-5 doc link deferred).
- 6 new unit tests pass; full suite green; VST3 + Standalone build clean.

### File List

- Source/Core/MIDI/MidiPortOpenFeedback.h (new)
- Source/Core/MIDI/MidiPortOpenFeedback.cpp (new)
- Source/Core/MIDI/Ports/MidiInputPort.h (updated)
- Source/Core/MIDI/Ports/MidiInputPort.cpp (updated)
- Source/Core/MIDI/Ports/MidiOutputPort.h (updated)
- Source/Core/MIDI/Ports/MidiOutputPort.cpp (updated)
- Source/Core/MIDI/MidiManager.cpp (updated)
- Tests/Unit/MidiPortOpenFeedbackTests.cpp (new)
- Tests/Unit/MidiPortOpenFailurePropagationTests.cpp (new)
- CMakeLists.txt (updated)

## Change Log

- 2026-06-06: Story created (ready-for-dev) — FR-44 MIDI port open error feedback; R-1/R-2 prerequisites satisfied; blocks R-5 doc link.
- 2026-06-06: Implementation complete — footer propagation, structured logging, unit tests; status → review.

### Review Findings

- [x] [Review][Decision] Stale footer after successful port open — resolved: option 1, `ExceptionPropagator::clearMessage` on successful input/output open [Source/Core/MIDI/MidiManager.cpp]

- [x] [Review][Patch] Missing regression test for AC1.3 empty clear path [Tests/Unit/MidiPortOpenFailurePropagationTests.cpp]

- [x] [Review][Patch] Missing assertion that port failure does not touch `lastError` / `errorType` (AC1.2) [Tests/Unit/MidiPortOpenFailurePropagationTests.cpp]

- [x] [Review][Defer] `MidiPortOpenResult` types live in `MidiPortOpenFeedback.h`, pulling `juce_audio_processors` into port headers via include — deferred, acceptable layering for now [Source/Core/MIDI/MidiPortOpenFeedback.h]

- [x] [Review][Defer] No end-to-end `kOpenRejected` propagation test — formatter branch covered; OS-level open rejection not mockable without harness — deferred [Tests/Unit/MidiPortOpenFailurePropagationTests.cpp]
