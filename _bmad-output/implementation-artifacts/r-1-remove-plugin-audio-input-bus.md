---
organization: Ten Square Software
project: Matrix-Control
title: Story R-1 — Remove Plugin Audio Input Bus
author: BMad Agent
status: done
baseline_commit: c013da1 + uncommitted working tree (passthrough bus-buffer refactor, MidiActivityTracker kMidiFromInbound rename)
sources:
  - planning-artifacts/sprint-change-proposal-2026-06-06.md
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/2-7-audio-passthrough-and-peak-indicator.md
  - implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
  - implementation-artifacts/2-11-header-panel-layout-and-widget-styling.md
  - project-context.md
created: 2026-06-06
updated: 2026-06-06
---

# Story R-1: Remove Plugin Audio Input Bus

Status: done

<!-- Sprint Change Proposal 2026-06-06 approved. Implements AD-11 Option R-A. Supersedes D-055 hosted-plugin input bus. Stories 2.7 / 2.9b / 2.11 ACs revised in epics.md — scope narrows to standalone passthrough. -->

## Story

As an Ableton/Logic/Cubase user,
I want Matrix-Control to behave as a standard MIDI instrument without routing synth audio through the plugin,
so that my DAW routing is natural and unambiguous (AD-11, Option R-A).

## Acceptance Criteria

### AC 1 — Hosted plugin: no audio input bus

1. **`PluginProcessor` constructor `BusesProperties`:** when **not** standalone (`!isStandaloneWrapper()`), declare **stereo output only** — **no** `.withInput("Audio From", …)`.
2. **Standalone:** retain `.withInput("Audio From", juce::AudioChannelSet::stereo(), true)` + stereo output (unchanged from Story 2.7).
3. **`isBusesLayoutSupported`:** accept output mono/stereo; when `getBusCount(true) == 0`, accept empty/disabled input layouts (already partially implemented in working tree — verify and keep).
4. **`ensureAudioInputBusEnabled`:** no-op when `getBusCount(true) <= 0` (already guarded — verify).
5. **Detection SSOT:** reuse anonymous `isStandaloneWrapper()` in `PluginProcessor.cpp` (`PluginHostType::getPluginLoadedAs() == wrapperType_Standalone`). Do **not** introduce a second detection mechanism.

### AC 2 — Silent hosted audio output

1. **`processBlock`:** when hosted (zero input channels / no input bus), `AudioPassthroughProcessor` receives `inputEnabled == false` and clears stereo output each block.
2. **`AudioPassthroughProcessor::process`:** existing disabled-input path clears output and zeroes peak — **preserve**; no synth audio passthrough in plugin mode.
3. **No read of input bus buffer** in hosted mode — `getBusBuffer(..., true, 0)` with zero channels must remain safe (JUCE returns empty buffer view).
4. **`processBlock` body** stays thin (delegate to `AudioPassthroughProcessor`; no inline DSP).

### AC 3 — Standalone passthrough unchanged

1. **`StandaloneAudioInputRouter`** path unchanged — physical input via `AudioDeviceManager`, catalog entries, channel mode mapping.
2. **`AudioInputSourceCatalog::buildForProcessor(true)`** returns hardware channel entries; standalone combo + gain + peak behave as after Story 2.11.
3. **Persist** `audioFromSourceId`, `audioFromChannelMode`, `inputGainDb` in `apvts.state` — session round-trip in standalone.

### AC 4 — Header UI: audio controls standalone-only

1. **`HeaderPanel::setPluginMode(true)`** (hosted): **hide** packets **AUDIO FROM** (label + combo) and **INPUT GAIN** (label + slider + peak indicator). Use `setVisible(false)` **and** skip layout placement in `resized()` when `isPluginMode_`.
2. **`setPluginMode(false)`** (standalone): show all routing packets including audio.
3. **MIDI routing + Activity LEDs** (Stories 2.8 / 2.11) remain visible in **both** modes — primary MIDI feedback in plugin mode.
4. **Layout:** when audio packets hidden, left cluster ends after MIDI TO packet; no empty gap where audio controls were.

### AC 5 — PluginEditor wiring guards

1. **`refreshAudioFromCombo`**, audio-from restore, `audioFromComboBox_.onChange`, and standalone-only `HeaderRefreshTimer` audio-from retry — **standalone only** (extend existing `isStandalone()` guards).
2. **Peak indicator timer:** skip `setLevel` when `!isStandalone()` OR peak hidden — avoid useless work.
3. **Input gain slider `onChange`:** may remain wired (hidden in plugin) or guard — either OK if slider invisible and gain ignored via silent output path.
4. **Do not** remove `inputGainDb` / `audioFromSourceId` from session state — values preserved for standalone reload.

### AC 6 — Tests and build

1. **`AudioPassthroughProcessorTests`:** add case **hosted-style layout** — `prepare(0, 2, false, 44100.0)`, non-empty output buffer with garbage samples → `process` clears to silence, peak → 0.
2. **Optional:** `PluginProcessor` bus-count smoke via existing test harness if present — not required if passthrough unit tests cover silent path.
3. **Remove or stop calling** `AudioInputSourceCatalog::buildPluginChannelModeEntries()` from production hosted paths (catalog may remain for tests or be deleted if unused).
4. All existing unit tests pass; **VST3 + Standalone** CMake presets build cleanly.

### AC 7 — Documentation trace (no artifact edits in this story)

1. Dev Agent Record notes that epics.md / architecture AD-11 / sprint proposal already updated — **no** PRD/epics edits in R-1 code PR unless drift found.
2. Completion note: hosted workflow = instrument track (MIDI) + separate DAW audio track for synth return (Option D).

## Tasks / Subtasks

- [x] **Conditional `BusesProperties`** (AC: #1)
  - [x] Extract `makeBusesProperties()` helper using `isStandaloneWrapper()`
  - [x] Standalone: input + output; hosted: output only
  - [x] Verify `isBusesLayoutSupported` for zero-input hosted layout

- [x] **Processor audio path** (AC: #2, #3)
  - [x] Confirm `processBlock` / `refreshAudioPassthroughLayout` use `getAudioFromInputChannelCount()` (0 when no bus)
  - [x] Confirm `syncAudioRuntimeFromState` / `setAudioFromSourceId` standalone guards intact
  - [x] No regression in `InstrumentMidiForwarder` / MIDI path

- [x] **HeaderPanel conditional UI** (AC: #4)
  - [x] Extend `setPluginMode` — visibility for audio from + input gain + peak members
  - [x] `resized()` — skip packets 4–5 when `isPluginMode_`; call `resized()` from `setPluginMode`

- [x] **PluginEditor guards** (AC: #5)
  - [x] Standalone-only audio-from combo refresh / restore
  - [x] Timer: peak update standalone-only (or when peak visible)

- [x] **Catalog cleanup** (AC: #6)
  - [x] `buildForProcessor(false)` — return empty vector OR delete `buildPluginChannelModeEntries` if unused
  - [x] Remove hosted channel-mode combo strings from runtime path

- [x] **Tests + build** (AC: #6, #7)
  - [x] New passthrough zero-input test
  - [x] Full unit test target + macOS-ARM-Debug VST3/Standalone build

## Dev Notes

### Problem / divergence (verified 2026-06-06)

| Area | Current (wrong for AD-11) | Target (AD-11) |
|---|---|---|
| `BusesProperties` | `.withInput("Audio From", stereo, true)` **always** (`PluginProcessor.cpp:57`) | Input bus **standalone only** |
| `AudioInputSourceCatalog` | `buildForProcessor(false)` → Stereo/Mono L/Mono R host modes | No hosted catalog — user uses DAW audio track |
| `HeaderPanel::setPluginMode` | Only reconfigures Keyboard From combo | Also **hide** Audio From / Input Gain / Peak |
| `HeaderPanel::resized` | Always lays out audio packets 4–5 | Skip when `isPluginMode_` |
| D-055 assumption | Single-track External Instrument parity | **Superseded** by D-055-R (standalone only) |

### Brownfield baseline (working tree on top of `c013da1`)

Recent uncommitted refactor (keep and extend):

- `AudioPassthroughProcessor::process(input, output, gain)` — separate bus buffers (matches JUCE sidechain layout).
- `PluginProcessor::processBlock` uses `getBusBuffer` for input/output buses.
- `isBusesLayoutSupported`, `ensureAudioInputBusEnabled`, `getAudioFromInputChannelCount` added.
- `MidiActivityTracker::Path::kEditor` renamed **`kMidiFromInbound`** — use current enum name in new code.

**Do not revert** bus-buffer passthrough refactor; R-1 layers AD-11 on top.

### Implementation pattern — conditional buses

```cpp
namespace
{
    juce::AudioProcessor::BusesProperties makeBusesProperties()
    {
        auto props = juce::AudioProcessor::BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true);

#if !JucePlugin_IsMidiEffect
        if (isStandaloneWrapper())
            props.withInput("Audio From", juce::AudioChannelSet::stereo(), true);
#endif
        return props;
    }
}

PluginProcessor::PluginProcessor()
    : AudioProcessor(makeBusesProperties())
```

`isStandaloneWrapper()` is valid in ctor — JUCE sets wrapper type before processor construction (same pattern as Story 2.9b).

### HeaderPanel hide pattern

```cpp
void HeaderPanel::setPluginMode(bool isPlugin)
{
    isPluginMode_ = isPlugin;
    const bool showAudio = !isPluginMode_;

    audioFromLabel_.setVisible(showAudio);
    audioFromComboBox_.setVisible(showAudio);
    inputGainLabel_.setVisible(showAudio);
    inputGainSlider_.setVisible(showAudio);
    peakIndicator_.setVisible(showAudio);

    if (isPluginMode_)
        configurePluginModeKeyboardFrom();
    else
        configureStandaloneKeyboardFrom();

    resized();
}
```

In `resized()`, wrap packets 4–5:

```cpp
if (!isPluginMode_)
{
    placePacketLabel(audioFromLabel_, ...);
    // ...
}
```

### Files to touch

| Action | Path |
|---|---|
| UPDATE | `Source/Core/PluginProcessor.cpp` — conditional `BusesProperties` |
| UPDATE | `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` — `setPluginMode`, `resized` |
| UPDATE | `Source/GUI/PluginEditor.cpp` — standalone guards for audio UI |
| UPDATE | `Source/Core/Audio/AudioInputSourceCatalog.cpp` — hosted catalog empty / remove plugin entries |
| UPDATE | `Tests/Unit/AudioPassthroughProcessorTests.cpp` — zero-input silent test |
| VERIFY | `Source/Core/Audio/AudioPassthroughProcessor.{h,cpp}` — no change expected |
| VERIFY | `Source/Core/Audio/StandaloneAudioInputRouter*` — no behavioral change |

**Out of scope:** R-2 Hardware Latency, R-3 MIDI errors, PRD/epics doc edits, Story 2.10, Epic 8.4 bus switching.

### Preserve (must not break)

- AD-3 MIDI queue / `InstrumentMidiForwarder` in `processBlock` — **before** audio passthrough.
- Activity LEDs + `HeaderRefreshTimer` MIDI levels — all modes.
- Standalone Keyboard From + port restore (Story 2.9b).
- Input gain −∞…+12 dB + peak styling (Story 2.11).
- `CMakeLists.txt` `IS_SYNTH TRUE` — instrument category unchanged; only bus layout differs by wrapper.

## Technical Requirements

- **Thread safety:** unchanged — gain on atomic; no new audio-thread GUI reads.
- **Wrapper detection:** `isStandaloneWrapper()` only; public `PluginProcessor::isStandalone()` for GUI.
- **Hosted output bus:** remains enabled stereo (silent) — hosts expect instrument output bus even if silent.
- **Session state:** keep storing audio properties in plugin sessions for users who switch between standalone and hosted builds of same project (harmless if ignored in hosted).

## Architecture Compliance

- **AD-11** — hosted: no input bus, silent output; standalone: passthrough + gain + peak [architecture.md §AD-11]
- **D-055-R** — supersedes hosted input bus rationale [sprint-change-proposal-2026-06-06.md §4.4]
- **AD-3** — untouched; no `sendMessageNow` from audio thread
- **FR-8 / FR-39 / FR-4** — revised mapping in epics.md; this story is the code implementation slice

## Library / Framework Requirements

- **JUCE 8.0.12** — `BusesProperties`, `getBusBuffer`, `PluginHostType::getPluginLoadedAs()`
- No new dependencies

## File Structure Requirements

- Core audio under `Source/Core/Audio/`
- Header under `Source/GUI/Panels/MainComponent/HeaderPanel/`
- Tests under `Tests/Unit/`
- Register no new `.cpp` files unless splitting helpers

## Testing Requirements

- JUCE `UnitTest` pattern — extend `AudioPassthroughProcessorTests`
- New test name suggestion: `"Zero input channels clears output (hosted AD-11)"`
- Run: `cmake --build Builds/macOS-ARM/Debug --target Matrix-Control_Tests` (or project preset) + plugin targets
- Manual smoke (Completion Notes): Live/Logic — load VST3, confirm **no** "Audio From" sidechain in host plugin I/O; header shows MIDI controls only; standalone still shows audio controls + passthrough

## Previous Story Intelligence

**From Story 2.7:** `AudioPassthroughProcessor` disabled-input path clears output — maps directly to hosted mode. Peak standalone-only now by AD-11, not 2.7 regression.

**From Story 2.9b:** `setPluginMode(!isStandalone())` wired in `PluginEditor` ctor — extend mode handler, do not duplicate standalone detection in GUI.

**From Story 2.11:** Packets 4 (AUDIO FROM) and 5 (INPUT GAIN + peak) are explicit layout units — hide as whole packets, not individual misaligned widgets.

## Git Intelligence Summary

| Commit | Relevance |
|---|---|
| `c013da1` | Unified audio input catalog + header labels — catalog split standalone vs plugin entries |
| `836ce3a` | Header layout packets — hide packets 4–5 in plugin mode |
| `837e423` | Story 2.7 passthrough — refactor to dual bus buffers in WIP |

## Latest Technical Information

- **VST3/AU constraint:** plugins cannot open hardware inputs; host allocates buses only. Option D (separate audio track) is industry standard for external instruments.
- **JUCE `getBusCount(true) == 0`:** valid for output-only processors; guard all input-bus access (already in WIP helpers).

## Project Context Reference

- C++17, Allman braces, 4-space indent, `k` prefix [project-context.md]
- Epic R inserted before E3; R-4 recommended before E3 but not blocking R-1

### References

- [sprint-change-proposal-2026-06-06.md §4.2 AD-11, Story R-1]
- [epics.md Epic R, Story R-1]
- [architecture.md §AD-11]
- [2-7-audio-passthrough-and-peak-indicator.md] — standalone scope after revision
- [2-11-header-panel-layout-and-widget-styling.md] — packet layout SSOT

## Dev Agent Record

### Agent Model Used

Composer (dev-story R-1)

### Debug Log References

- `BusesProperties` must be built via `PluginProcessor::makeBusesProperties()` static member — free function cannot access protected nested type.

### Completion Notes List

- AD-11 implemented: hosted VST3/AU = output-only bus + silent passthrough; standalone unchanged.
- Header packets 4–5 (Audio From, Input Gain, Peak) hidden in plugin mode; Activity LEDs remain.
- `buildPluginChannelModeEntries` removed; hosted catalog returns empty.
- Manual smoke Ableton Live 12: sidechain/input bus gone in hosted plugin; standalone audio controls visible. Logic not tested (trial expired).

### File List

- `Source/Core/PluginProcessor.{h,cpp}`
- `Source/Core/Audio/AudioInputSourceCatalog.{h,cpp}`
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp`
- `Source/GUI/PluginEditor.cpp`
- `Tests/Unit/AudioPassthroughProcessorTests.cpp`
- `_bmad-output/implementation-artifacts/r-1-remove-plugin-audio-input-bus.md`
- `_bmad-output/implementation-artifacts/sprint-status.yaml`

## Change Log

- 2026-06-06: Story created (ready-for-dev) — Sprint Change Proposal approved; AD-11 implementation context.
- 2026-06-06: Code review complete — Live smoke OK; status → done.
