---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.7 — Audio Passthrough and Peak Indicator
author: BMad Agent
status: review
baseline_commit: bd49cd9
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/2-1-midioutboundqueue-core.md
  - implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
  - project-context.md
created: 2026-06-05
---

# Story 2.7: Audio Passthrough and Peak Indicator

Status: done

<!-- Note: Validation is optional. Run validate-create-story for quality check before dev-story. -->

## Story

As a performer,
I want synth audio passed through with gain and peak display,
so that I can monitor input level in the plugin (FR-8, partial FR-39).

## Acceptance Criteria

1. **`Core::AudioPassthroughProcessor`** in `Source/Core/Audio/AudioPassthroughProcessor.{h,cpp}` — namespace `Core`, zero GUI. Copies/mixes the plugin **input bus → output bus** with user **Input Gain** (linear multiplier). Must be safe to call only from the audio thread (`processBlock`). No allocations, locks, or logging in `process()`.
2. **`PluginProcessor::processBlock` delegation (AD-2, D-053):** After existing `InstrumentMidiForwarder` call, delegate audio to `AudioPassthroughProcessor`. Total `processBlock` body stays **≤ 15 lines** excluding braces — do **not** inline passthrough math in `PluginProcessor`.
3. **Virtual instrument registration (minimal FR-4 slice):** Update `CMakeLists.txt` / `juce_add_plugin` so the product builds as a **virtual instrument**: `IS_SYNTH TRUE`, `AU_MAIN_TYPE kAudioUnitType_MusicDevice`, `VST3_CATEGORIES Instrument` (remove `Fx`). Enable a named stereo **input bus** (`"Audio From"`) plus existing stereo output in `BusesProperties`. **Defer** mono-vs-stereo bus switching on `deviceType` to Epic 8 — use **stereo input** default until `DeviceTypeRegistry` exists.
4. **Input Gain control:** Header **Input Gain** slider adjusts passthrough level. Persist as `apvts.state` property `inputGainDb` (`float`, default `0.0f`, range **−24 dB … +12 dB** per addendum intent). Convert to linear gain in `prepareToPlay` or cache in `std::atomic<float>` updated from the message thread when the slider changes — **never** read `juce::var` on the audio thread (same lesson as `keyboardFromEnabled` in Story 2.9b review).
5. **Audio From combo (partial FR-39 / D-055):**
   - **Plugin (hosted):** Combo visible; lists host input routing options for the plugin input bus (minimum: **Stereo** / **Mono L** / **Mono R** channel modes). No direct hardware `AudioDeviceManager` open in plugin mode. Persist `audioFromChannelMode` (`juce::String` or int enum) in `apvts.state`.
   - **Standalone:** Combo lists physical inputs from `juce::AudioDeviceManager` (active device input channel names). Selecting an entry configures the standalone input channel map via the JUCE standalone device manager (through `PluginProcessor` API — Core may use `juce_audio_devices`, not GUI headers). Persist `audioFromSourceId`.
6. **Peak level indicator (D-071, FR-8):** New header widget (e.g. `PeakIndicator` in `Source/GUI/Widgets/`) — **not** a continuous VU meter. **Solid vertical fill** proportional to **post-gain peak** on the input bus. Colour = **`SkinColourId::kEnvelopeDisplayEnvelope`** (same token as `EnvelopeDisplay` curve — `look_.envelope` in `EnvelopeDisplay.cpp:199`). Update on message thread via `juce::Timer` (~30 Hz) reading `AudioPassthroughProcessor::getPeakLevel()` (`std::atomic<float>` 0…1). **No** `repaint()` from audio thread.
7. **Peak ballistics:** Hold peak briefly (~1 s hold, smooth decay) so the indicator is readable; document chosen constants in Dev Notes. Clip/near-clip at 0 dBFS → fill reaches top of indicator bounds.
8. **Header layout:** Add **AUDIO FROM** label + combo, **INPUT GAIN** label + compact slider, and peak indicator to `HeaderPanel` using existing `tss::Label`, `tss::ComboBox`, `tss::Slider` patterns. New display strings in `PluginDisplayNames::HeaderPanel`; width constants in `HeaderPanel` or `PluginDesignDimensions` (divisible by 4). Accept interim crowding (routing combos from 2.9b already fill the bar) — same compromise philosophy as Story 2.9b; do **not** block on Figma or Story 7.8 logo popup.
9. **`PluginEditor` wiring:** Mirror 2.9b port-restore pattern — restore `inputGainDb`, `audioFromChannelMode` / `audioFromSourceId` at construction; `onChange` → `PluginProcessor` setters; peak indicator receives skin updates in `updateSkin()`.
10. **Silence / no input bus:** When input bus disabled or buffer empty, output silence (or clear), peak decays to zero — no NaN/Inf, no runaway gain.
11. **Tests + build:** `AudioPassthroughProcessorTests` — unity gain passthrough, gain scaling, peak tracks max sample, silence → peak 0. Optional `HeaderPanel` layout smoke test if harness exists. Register new `.cpp` in `CMakeLists.txt` plugin + test targets. All existing unit tests pass; **Standalone + VST3** build cleanly.

## Tasks / Subtasks

- [x] **Create `AudioPassthroughProcessor`** (AC: #1, #7, #10)
  - [x] `process(buffer, gainLinear)` — copy input→output, apply gain, track peak atomically
  - [x] Peak hold/decay constants; `getPeakLevel()` for GUI

- [x] **Enable instrument + input bus** (AC: #3)
  - [x] `CMakeLists.txt`: `IS_SYNTH TRUE`, AU/VST3 categories
  - [x] `PluginProcessor` constructor `BusesProperties`: `.withInput("Audio From", stereo, true)` + stereo output
  - [x] Verify `acceptsMidi` / `isMidiEffect` unchanged

- [x] **Wire `PluginProcessor`** (AC: #2, #4, #5, #10)
  - [x] Member `audioPassthroughProcessor_`; call in `processBlock`
  - [x] `setInputGainDb(float)` + atomic linear cache
  - [x] `setAudioFromChannelMode` / `setAudioFromSource` (+ standalone device channel API)
  - [x] Init properties in dedicated helper (mirror `initializeMidiPortProperties`)

- [x] **Header UI** (AC: #5, #6, #8)
  - [x] `PluginDisplayNames`: `kAudioFromLabel`, `kInputGainLabel`
  - [x] `PeakIndicator` widget + look from skin envelope colour
  - [x] Extend `HeaderPanel`: combo, gain slider, peak indicator; `setPluginMode` branches
  - [x] `resized()` layout with `uiScale_`

- [x] **Wire `PluginEditor`** (AC: #9)
  - [x] Restore/persist gain + audio-from properties
  - [x] Timer or `MainComponent` timer callback for peak refresh
  - [x] Skin change updates peak colour

- [x] **Tests + CMake** (AC: #11)
  - [x] `Tests/Unit/AudioPassthroughProcessorTests.cpp`
  - [x] Full test suite + Standalone/VST3 build

### Review Findings

- [x] [Review][Patch] MONO L applies gain twice on right channel [`Source/Core/Audio/AudioPassthroughProcessor.cpp:91-118`]
- [x] [Review][Patch] Audio gain/channel mode not resynced after `setStateInformation` or `valueTreeRedirected` [`Source/Core/PluginProcessor.cpp:228-241`, `711-721`]
- [x] [Review][Patch] `inputBusEnabled_` stale when input bus toggled without `prepareToPlay` [`Source/Core/PluginProcessor.cpp:330-337`]
- [x] [Review][Patch] `process()` returns early on `numSamples == 0` without clearing output or decaying peak [`Source/Core/Audio/AudioPassthroughProcessor.cpp:74-75`]
- [x] [Review][Patch] No guard when `buffer.getNumChannels()` < `numOutputChannels_` [`Source/Core/Audio/AudioPassthroughProcessor.cpp:91-104`]
- [x] [Review][Patch] Non-finite `inputGainDb` not sanitized before `dbToLinearGain` [`Source/Core/PluginProcessor.cpp:339-344`]
- [x] [Review][Patch] Missing unit tests for MONO L/R channel modes and peak hold ballistics [`Tests/Unit/AudioPassthroughProcessorTests.cpp`]
- [x] [Review][Defer] Standalone audio-from combo not refreshed on device change [`Source/GUI/PluginEditor.cpp:73-77`] — deferred, follow-up UX
- [x] [Review][Defer] Weak symbols (`__attribute__((weak))`) not portable to MSVC [`Source/Core/Audio/StandaloneAudioInputRouterStubs.cpp:5-15`] — deferred, Windows build follow-up
- [x] [Review][Defer] Standalone `audioFromSourceId` uses unstable numeric channel indices [`Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp:35-36`] — deferred, device-change edge case
- [x] [Review][Defer] `getInstrumentPathEnabled` reads `apvts.state` on audio thread (standalone) [`Source/Core/PluginProcessor.cpp:321-324`] — deferred, pre-existing pattern extracted from `processBlock`
- [x] [Review][Defer] Redundant `setAudioFromSourceId` / `setInputGainDb` on startup (ctor + editor) [`PluginProcessor.cpp:386-388`, `PluginEditor.cpp:97-112`] — deferred, harmless duplication

## Dev Notes

### Brownfield baseline (verified 2026-06-05 @ `bd49cd9`)

| Component | Current state | File |
|---|---|---|
| `AudioPassthroughProcessor` | **Missing** — only `InstrumentMidiForwarder` exists | `Source/Core/Audio/` |
| `processBlock` | Forwards MIDI only; **`juce::ignoreUnused(audioBuffer)`** | `PluginProcessor.cpp:203-219` |
| Plugin category | **`IS_SYNTH FALSE`**, AU Effect, VST3 `Fx` | `CMakeLists.txt:334-341` |
| Input bus | Conditional `#if !JucePlugin_IsSynth` — **no input today** | `PluginProcessor.cpp:47-53` |
| `HeaderPanel` | MIDI routing + SCALE/SKIN/UI Elements — **no audio controls** | `HeaderPanel.{h,cpp}` |
| Peak / gain UI | **Missing** | — |
| Envelope curve colour SSOT | `SkinColourId::kEnvelopeDisplayEnvelope` | `SkinColoursWidgetsDisplays.h:56-60`, `EnvelopeDisplay.cpp:199` |

### Architecture mandate — AD-2 composition root

From architecture AD-2 and PRD addendum D-053:

```
processBlock():
  instrumentForwarder_->forward(...);
  audioPassthroughProcessor_->process(audioBuffer, cachedLinearGain_);
```

Reject inline audio copy/gain/peak in `PluginProcessor` — `.cursorrules` / project-context threading rules.

### Audio From — plugin vs standalone (D-055)

| Mode | Audio From behaviour |
|---|---|
| **Plugin** | Host routes Matrix-1000 audio → plugin **input bus** (Ableton External Instrument model). Combo selects **channel mode** on that bus (Stereo / Mono L / Mono R). No `AudioDeviceManager` hardware open. |
| **Standalone** | Combo lists **physical inputs** on the active audio device; selection maps input channels through `AudioDeviceManager` (JUCE standalone wrapper). |

**Epic 8 deferral:** Dynamic **mono vs stereo bus layout** when `deviceType` is Matrix-1000 vs Matrix-6/6R (addendum § Audio input bus layout) — **out of scope**. Ship stereo input bus; document follow-up in Epic 8 Story 8-4.

### Peak indicator — not a VU meter (D-071)

| Requirement | Implementation hint |
|---|---|
| Solid fill colour | `skin.getColour(SkinColourId::kEnvelopeDisplayEnvelope)` |
| Post-gain peak | Measure **after** gain in `AudioPassthroughProcessor` |
| No continuous VU | Single vertical bar; fill height = peak 0…1 |
| Threading | Atomic peak write in audio thread; `Timer` + `repaint()` on message thread only |

**Colour trap:** `LookBuilders::envelopeDisplayLookFromSkin` sets `look.envelope` correctly. Do **not** use `kTrackGeneratorDisplayShaper` for peak colour.

### Input Gain persistence pattern

Follow Story 2.9b port properties — **not** a synth APVTS automatable parameter:

```cpp
// apvts.state properties (new)
"inputGainDb"          // float, default 0.0f
"audioFromChannelMode" // plugin: String or int enum
"audioFromSourceId"    // standalone: String device channel id
```

Slider uses `juce::Slider` or `tss::Slider` in header style; dB display optional (tooltip or compact text deferred to 7.8).

### `AudioPassthroughProcessor` sketch

```cpp
namespace Core
{
    class AudioPassthroughProcessor
    {
    public:
        void process(juce::AudioBuffer<float>& buffer, float gainLinear) noexcept;
        float getPeakLevel() const noexcept { return peakDisplay_.load(); }
    private:
        std::atomic<float> peakDisplay_{ 0.0f };
        float peakHold_ { 0.0f }; // audio thread only
    };
}
```

- Input/output: use `buffer.getNumChannels()` / `getNumSamples()`; if input bus disconnected, clear output.
- Gain: `sample *= gainLinear`; clamp extreme dB to avoid denormals if needed.
- Peak: max abs across channels per block; apply hold/decay before storing to atomic.

**Peak ballistics constants (implemented):** `kPeakHoldSeconds = 1.0f`, `kPeakDecaySeconds = 1.0f` — sample-rate-aware hold counter + exponential per-sample decay after hold expires.

### Header layout note (post 2.9b)

Current left→right: **MIDI FROM | MIDI TO | KEYBOARD FROM | SCALE | SKIN | UI Elements** (`HeaderPanel.cpp:145-153`). Audio cluster placement options (pick one, document in Completion Notes):

1. Insert **AUDIO FROM | INPUT GAIN | peak** after KEYBOARD FROM, before SCALE/SKIN
2. Move SCALE/SKIN into a second row — **reject** (header height fixed at 32 px)

Compress combo/slider widths if needed; do not change `PluginDesignDimensions::GUI::kHeaderHeight`.

**Chosen layout:** Option 1 — audio cluster inserted after KEYBOARD FROM, before SCALE/SKIN.

### Prior art — copy these patterns

| Pattern | Reference |
|---|---|
| Core audio helper + thin `processBlock` | `InstrumentMidiForwarder` |
| Header combo + APVTS property | Story 2.9b `midiInputPortId` / `PluginEditor.cpp:51-57` |
| Port list population | `HeaderPanel::populateInputPortCombo` |
| Unit test style | `InstrumentMidiForwarderTests.cpp` |
| CMake new Core file | `InstrumentMidiForwarder.cpp` entries in `CMakeLists.txt:103,261` |
| Skin colour in widget | `EnvelopeDisplay::drawEnvelope` + `setSkin` refresh |

### Files to touch

| File | Action |
|---|---|
| `Source/Core/Audio/AudioPassthroughProcessor.{h,cpp}` | **NEW** |
| `Source/Core/PluginProcessor.{h,cpp}` | Wire passthrough, gain atomics, audio-from setters, `BusesProperties` |
| `Source/GUI/Widgets/PeakIndicator.{h,cpp}` | **NEW** (or equivalent name) |
| `Source/GUI/Panels/.../HeaderPanel.{h,cpp}` | Audio From, Input Gain, peak |
| `Source/GUI/PluginEditor.cpp` | Wiring + restore + peak timer |
| `Source/Shared/Definitions/PluginDisplayNames.h` | Header labels |
| `CMakeLists.txt` | IS_SYNTH, categories, new sources + tests |
| `Tests/Unit/AudioPassthroughProcessorTests.cpp` | **NEW** |

### Explicitly out of scope

- Instrument/Editor activity LEDs → **Story 2.8**
- Footer, logo popup, full FR-41 shell → **Story 7.8**
- `deviceType`-driven mono/stereo bus relayout → **Epic 8**
- Device Inquiry / UI lock → **Epic 8**
- DAW-specific External Instrument presets / routing docs → user manual

### Relationship to parallel stories

Stories **2.8** (LEDs) and **2.7** may proceed in parallel. Story **2.9b** is **done** — header routing combos must remain functional after layout changes.

### Testing standards

- JUCE `UnitTest` subclass, static registration (see `InstrumentMidiForwarderTests.cpp`)
- Synthetic `juce::AudioBuffer<float>` — no audio device required for Core tests
- Manual smoke: Standalone — select physical input, verify passthrough + peak; Plugin in Live — route synth out to plugin in, verify gain + peak

### Project Structure Notes

- Paths use `Source/Core/Audio/` (not architecture doc's legacy `src/Core/Audio/`).
- Core ↛ GUI — peak metering logic in Core; drawing in GUI widget only.
- All user-visible strings in `PluginDisplayNames.h`.

### References

- [Source: epics.md § Story 2.7]
- [Source: prd.md § FR-8, FR-4]
- [Source: addendum.md § PluginProcessor/dual-role, Peak level indicator, Audio input bus layout]
- [Source: architecture.md § AD-2, AD-3 threading]
- [Source: project-context.md § Threading, MIDI architecture header controls]
- [Source: implementation-artifacts/2-9b-header-routing-controls-uat-slice.md § Header layout, out-of-scope split]

## Dev Agent Record

### Agent Model Used

Claude 4.6 Sonnet (Cursor)

### Debug Log References

- Standalone `StandalonePluginHolder` header cannot compile in shared-code target — split into `StandaloneAudioInputRouter` with weak stubs (plugin/VST3) + strong impl linked only into `Matrix-Control_Standalone`.

### Completion Notes List

- `Core::AudioPassthroughProcessor` — input→output copy/mix, post-gain peak, 1 s hold + 1 s exponential decay (`kPeakHoldSeconds`, `kPeakDecaySeconds`).
- `PluginProcessor::processBlock` — 2-line body: MIDI forward + passthrough; `getInstrumentPathEnabled()` extracted.
- Virtual instrument: `IS_SYNTH TRUE`, AU `kAudioUnitType_MusicDevice`, VST3 `Instrument`; stereo `"Audio From"` input bus.
- Gain: `inputGainDb` APVTS property, `std::atomic<float> inputGainLinear_`, range −24…+12 dB.
- Audio From: plugin combo Stereo/Mono L/Mono R (`audioFromChannelMode`); standalone physical channels via `StandaloneAudioInputRouter` (`audioFromSourceId`).
- Header: audio cluster after KEYBOARD FROM (option 1); `PeakIndicator` + 30 Hz `PeakRefreshTimer` in `PluginEditor`.
- Tests: 5 `AudioPassthroughProcessor` unit tests pass; Standalone + VST3 build clean on macOS ARM.

### File List

- Source/Core/Audio/AudioPassthroughProcessor.h
- Source/Core/Audio/AudioPassthroughProcessor.cpp
- Source/Core/Audio/StandaloneAudioInputRouter.h
- Source/Core/Audio/StandaloneAudioInputRouter.cpp
- Source/Core/Audio/StandaloneAudioInputRouterStubs.cpp
- Source/Core/Audio/StandaloneAudioInputRouterStandalone.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Source/GUI/Widgets/PeakIndicator.h
- Source/GUI/Widgets/PeakIndicator.cpp
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h
- Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
- Source/GUI/PluginEditor.h
- Source/GUI/PluginEditor.cpp
- Source/Shared/Definitions/PluginDisplayNames.h
- CMakeLists.txt
- Tests/Unit/AudioPassthroughProcessorTests.cpp

### Change Log

- 2026-06-05: Code review — two-pass channel routing (MONO L/R fix), `syncAudioRuntimeFromState`, live bus layout refresh, guards + 4 new unit tests.
- 2026-06-05: Story 2.7 — audio passthrough, peak indicator, virtual instrument registration, header audio controls.
