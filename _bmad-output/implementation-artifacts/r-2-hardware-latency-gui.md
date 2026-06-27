---
organization: Ten Square Software
project: Matrix-Control
title: Story R-2 — Hardware Latency GUI
author: BMad Agent
status: done
baseline_commit: 651d9c706dd72f0799e75d58d11f91e40ceee8ca
sources:
  - planning-artifacts/sprint-change-proposal-2026-06-06.md
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/r-1-remove-plugin-audio-input-bus.md
  - project-context.md
created: 2026-06-06
updated: 2026-06-06
---

# Story R-2: Hardware Latency GUI

Status: done

<!-- FR-4b. Sprint Change Proposal 2026-06-06. Depends on R-1 (done). Hosted plugin: user monitors synth on separate DAW audio track; this control reports analog round-trip delay for host PDC. -->

## Story

As a user recording Matrix-1000 audio in my DAW,
I want to set Hardware Latency (ms) in Matrix-Control,
so that my DAW can compensate for analog round-trip delay (FR-4b).

## Acceptance Criteria

### AC 1 — Header control (Settings page not yet built — Epic 7.7)

1. **Numeric control** in **HeaderPanel right cluster** (before SCALE): label `PluginDisplayNames::HeaderPanel::kHardwareLatencyLabel`, compact `tss::Slider`.
2. Range **0.0–100.0 ms**, step **0.1 ms**, unit `"ms"`.
3. Visible in **both** hosted plugin and standalone modes (unlike Input Gain — this is DAW PDC parity, not audio passthrough).
4. Follow existing header slider pattern (`inputGainSlider_`: `setRange`, `setUnit`, `PluginEditor` restore + `onChange`).

### AC 2 — Host latency reporting

1. User value (ms) converted to samples: `round(ms * sampleRate / 1000.0)` using current processor sample rate.
2. **`setLatencySamples(samples)`** then **`updateHostDisplay()`** (JUCE notifies host of latency change — equivalent intent to `reportLatency()` in sprint proposal).
3. Re-apply when **sample rate changes** (`prepareToPlay`) and when **value changes** (slider / state restore).
4. Default **0.0 ms** → **0 samples** (no compensation until user sets value).

### AC 3 — APVTS persistence

1. Property key **`hardwareLatencyMs`** on `apvts.state` (routing-pref pattern like `inputGainDb` — **not** an `AudioParameter`).
2. Add `PluginIDs::Settings::kHardwareLatencyMsId = "hardwareLatencyMs"`.
3. Round-trip via `getStateInformation` / `setStateInformation` (inherits from `apvts.copyState()`).
4. `initializeHardwareLatencyProperty()` in constructor: default 0.0 if missing; clamp + quantize on load.

### AC 4 — Pure conversion helpers (testable)

1. **`Core::HardwareLatency`** namespace in `Source/Core/Audio/HardwareLatency.h/.cpp`:
   - `kMinMs`, `kMaxMs`, `kStepMs` constants
   - `clampMs(float)`, `quantizeMs(float)`, `msToSamples(float ms, double sampleRate)`
2. Unit tests in `Tests/Unit/HardwareLatencyTests.cpp`.

### AC 5 — User documentation

1. **`Documentation/hardware-latency.md`**: explains analog round-trip (DAW → MIDI → synth → ADC → audio track), how to set control, typical values table (USB interface + Matrix-1000 DAC/ADC ballpark ranges).

### AC 6 — Tests and build

1. `HardwareLatencyTests`: clamp, quantize (0.1 step), ms→samples at 44100/48000/96000, edge 0 and 100 ms.
2. Register test file + `HardwareLatency.cpp` in `CMakeLists.txt` `Matrix-Control_Tests` target.
3. All existing unit tests pass; **VST3 + Standalone** presets build cleanly.

## Tasks / Subtasks

- [x] **Core conversion + processor wiring** (AC: #2, #3, #4)
  - [x] Add `HardwareLatency.h/.cpp` with clamp/quantize/msToSamples
  - [x] Add `PluginIDs::Settings::kHardwareLatencyMsId`, constants in `PluginAudioConstants` or HardwareLatency header
  - [x] `PluginProcessor`: `initializeHardwareLatencyProperty()`, `setHardwareLatencyMs()`, `applyHardwareLatencyToHost()`, call from `prepareToPlay` and `setStateInformation` (via sync)
  - [x] Public getter `getHardwareLatencyMs()` for editor restore

- [x] **Header UI + editor wiring** (AC: #1)
  - [x] `PluginDisplayNames::HeaderPanel::kHardwareLatencyLabel`
  - [x] `HeaderPanel`: label + slider members, layout in right cluster before SCALE, skin/scale updates
  - [x] `PluginEditor`: restore from apvts, `onValueChange` → `setHardwareLatencyMs`

- [x] **Documentation** (AC: #5)
  - [x] `Documentation/hardware-latency.md` with typical values table

- [x] **Tests + build** (AC: #6)
  - [x] `HardwareLatencyTests.cpp`
  - [x] CMakeLists test target update
  - [x] Run `Matrix-Control_Tests` + macOS-ARM-Debug VST3/Standalone build

## Dev Notes

### Problem / context

After R-1 (AD-11), hosted workflow = **instrument track (MIDI only)** + **separate DAW audio track** for synth return. Ableton External Instrument exposes Hardware Latency for PDC; Matrix-Control must expose the same via JUCE latency reporting.

**Not** plugin audio passthrough — no change to `AudioPassthroughProcessor` or input bus layout.

### Persistence pattern (mirror `inputGainDb`)

| Concern | Pattern |
|---|---|
| Storage | `apvts.state.setProperty("hardwareLatencyMs", float, nullptr)` |
| Init | `initializeHardwareLatencyProperty()` in ctor after `initializeAudioProperties()` |
| Restore | `setStateInformation` → existing `apvts.replaceState` → add `syncHardwareLatencyFromState()` |
| UI write | `PluginProcessor::setHardwareLatencyMs(float)` — single SSOT for clamp + apvts + host report |

### Latency API (JUCE 8.0.12)

```cpp
setLatencySamples(int newLatency) noexcept;
updateHostDisplay();  // call after latency change so host refreshes PDC
```

Use `audioPassthroughSampleRate_` (set in `prepareToPlay`) for conversion; fallback 44100.0 before first prepare.

**Threading:** slider `onValueChange` runs on message thread — safe for `setLatencySamples` / `updateHostDisplay`.

### UI placement decision

| Option | Verdict |
|---|---|
| Settings page (Epic 7.7) | Not built — defer |
| Footer | Message-only today — wrong semantics |
| **Header right cluster** | **Selected** — before SCALE/SKIN; visible plugin + standalone |

### References

- [sprint-change-proposal-2026-06-06.md §4.1 FR-4b, Story R-2]
- [epics.md Story R-2]
- [architecture.md §AD-11]
- [r-1-remove-plugin-audio-input-bus.md]

## Dev Agent Record

### Agent Model Used

Claude (Cursor agent)

### Debug Log References

- Fixed float `expectEquals` in quantize test → `expectWithinAbsoluteError`
- `5.0 ms @ 44100 Hz` → 220 samples via `juce::roundToInt` (not 221)

### Implementation Plan

1. Pure `Core::HardwareLatency` conversion unit
2. `PluginProcessor` apvts.state property + `setLatencySamples` / `updateHostDisplay`
3. Header right-cluster slider + PluginEditor wiring
4. User doc + unit tests

### Completion Notes List

- FR-4b implemented: HW LATENCY slider (0–100 ms, 0.1 step) in header; visible plugin + standalone.
- Persistence: `hardwareLatencyMs` on `apvts.state`; session round-trip via existing state XML.
- Host PDC: `setLatencySamples` + `updateHostDisplay` on value change, state restore, and `prepareToPlay`.
- `HardwareLatencyTests` pass; VST3 + Standalone macOS-ARM-Debug build clean.

### File List

- Source/Core/Audio/HardwareLatency.h (new)
- Source/Core/Audio/HardwareLatency.cpp (new)
- Source/Core/PluginProcessor.h (updated)
- Source/Core/PluginProcessor.cpp (updated)
- Source/Shared/Definitions/PluginIDs.h (updated)
- Source/Shared/Definitions/PluginDisplayNames.h (updated)
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h (updated)
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp (updated)
- Source/GUI/PluginEditor.cpp (updated)
- Source/GUI/Widgets/Slider.cpp (updated)
- Documentation/hardware-latency.md (new)
- Tests/Unit/HardwareLatencyTests.cpp (new)
- CMakeLists.txt (updated)

### Review Findings

- [x] [Review][Patch] Slider changes regress Input Gain keyboard/drag behaviour [`Source/GUI/Widgets/Slider.cpp`] — resolved: `SliderConfig` at construction (min/max/default/step/unit/minimumDisplayText); Shift = step × 10.
- [x] [Review][Defer] Standalone header layout overlap [`HeaderPanel.cpp`] — deferred: dedicated SETTINGS window (new story Epic 7.7+) will host header-adjacent controls with proper layout.
- [x] [Review][Patch] Shift+arrow coarse step on HW latency slider [`Slider.cpp`] — resolved by generic step model (0.1 ms normal, 1.0 ms Shift).
- [x] [Review][Defer] Redundant `setHardwareLatencyMs` + `updateHostDisplay` on editor open [`Source/GUI/PluginEditor.cpp:130-132`] — deferred, harmless idempotent host refresh.
- [x] [Review][Defer] Sample-rate fallback 44100 before first `prepareToPlay` [`Source/Core/PluginProcessor.cpp:512-518`] — deferred, corrected on prepare; same pattern as audio passthrough.
- [x] [Review][Defer] No automated APVTS round-trip test for `hardwareLatencyMs` — deferred, mirrors `inputGainDb` gap.

## Change Log

- 2026-06-06: Story created (ready-for-dev) — FR-4b Hardware Latency; R-1 prerequisite satisfied.
- 2026-06-06: Code review complete — SliderConfig refactor; layout overlap deferred to Story 7.7; status → done.
