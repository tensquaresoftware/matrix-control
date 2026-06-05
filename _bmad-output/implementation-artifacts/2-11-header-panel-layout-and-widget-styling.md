---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.11 — Header Panel Layout and Widget Styling
author: BMad Agent
status: ready-for-dev
baseline_commit: 837e423 (HEAD) + uncommitted Epic 2 work (2.8 ActivityLed/MidiActivityTracker in working tree)
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/2-7-audio-passthrough-and-peak-indicator.md
  - implementation-artifacts/2-8-activity-leds-on-queue-traffic.md
  - implementation-artifacts/2-9b-header-routing-controls-uat-slice.md
  - project-context.md
created: 2026-06-05
updated: 2026-06-05
---

# Story 2.11: Header Panel Layout and Widget Styling

Status: ready-for-dev

<!-- Supersedes Story 2.8 header layout AC #6 (INSTRUMENT/EDITOR labelled LED cluster). Full FR-41 shell polish remains Story 7.8. -->

## Story

As a performer,
I want the header routing controls laid out with consistent gaps and correctly styled audio/MIDI indicators,
so that I can read port selection, activity, and input level at a glance (FR-39, FR-8, FR-9 polish).

## Acceptance Criteria

### AC 1 — Gap-based layout (`kGap_` SSOT)

1. **`HeaderPanel::kGap_` (5 px design)** is the single horizontal spacing constant for the left routing cluster. Remove reliance on `kLeftPadding_` (15 px) for the routing row — first control starts at **`kGap_`** from the left edge of the panel.
2. **Within a logical packet** (label + control(s)): spacing between adjacent elements = **`kGap_`**.
3. **Between logical packets**: spacing = **`2 × kGap_`** (10 px design).
4. **Logical packets** (in order, left → right):
   | # | Packet | Members |
   |---|--------|---------|
   | 1 | MIDI FROM | Label + ComboBox + Editor activity LED |
   | 2 | MIDI TO | Label + ComboBox + Outbound activity LED |
   | 3 | KEYBOARD FROM | Label + ComboBox + Instrument activity LED |
   | 4 | AUDIO FROM | Label + ComboBox |
   | 5 | INPUT GAIN | Label + Slider + Peak indicator |
5. **`resized()`** uses scaled gaps (`kGap_ * uiScale_`) like today. Refactor placement helpers so packet-internal vs packet-external gaps cannot drift (one helper or explicit constants — no magic `gap * 2` scattered without comment).
6. **Header height unchanged** (`PluginDesignDimensions::GUI::kHeaderHeight`). **SCALE / SKIN / UI Elements** cluster on the right is out of scope for gap rework except ensuring it still fits after left-cluster width change.

### AC 2 — Activity LED placement (supersedes Story 2.8 layout)

1. **Remove** `instrumentLabel_`, `editorLabel_`, and their layout constants (`kInstrumentLabelWidth_`, `kEditorLabelWidth_`). Remove `addAndMakeVisible` / `setLook` for these labels. Remove **`PluginDisplayNames::HeaderPanel::kInstrumentLabel` / `kEditorLabel`** if no other references remain.
2. **Editor activity LED** — immediately to the **right** of `midiFromComboBox_`, gap **`kGap_`**. Still driven by `MidiActivityTracker::Path::kEditor`.
3. **Instrument activity LED** — immediately to the **right** of `keyboardFromComboBox_`, gap **`kGap_`**. Still driven by `Path::kInstrument`.
4. **New outbound activity LED** — `tss::ActivityLed midiToActivityLed_` immediately to the **right** of `midiToComboBox_`, gap **`kGap_`**. Reflects **any successful outbound MIDI send** (all categories/paths).
5. **Core — `MidiActivityTracker`:** add `Path::kOutbound`. Notify from `MidiManager::dispatchOutboundMessage` after each successful send (realtime and SysEx branches — same hook points as existing path-specific notify). **`getActivityLevel(kOutbound)`** uses same hold/decay constants as other paths. **Keep** existing `kInstrument` / `kEditor` path-specific notifies — outbound LED is additive, not a replacement.
6. **`PluginEditor::HeaderRefreshTimer`** — wire `midiToActivityLed_` from `getActivityLevel(kOutbound)` alongside existing instrument/editor reads.
7. **Public API:** `HeaderPanel::getMidiToActivityLed()` (or consistent naming). Keep existing instrument/editor getters.

### AC 3 — Control dimensions

| Control | Design size | Notes |
|---------|-------------|-------|
| MIDI FROM / TO / KEYBOARD FROM / AUDIO FROM ComboBox | **96 × 20** | Unify `kAudioFromComboBoxWidth_` → 96; all four use same width constant |
| Input gain slider | **60 × 20** | Update `kInputGainSliderWidth_` |
| Peak indicator | **12 × 20** | Update `kPeakIndicatorWidth_` |
| Activity LEDs | **12 × 12** | Square; vertically centred in 20 px header row (`controlY` unchanged; LED height 12, centred in row) |

All dimensions scale with `uiScale_`.

### AC 4 — Input gain slider calibration

1. Range **−∞ to +12 dB** (replace current −24…+12 in `HeaderPanel` ctor).
2. Display unit **`dB`** via existing `tss::Slider::setUnit("dB")` — unit appears to the right of the numeric value (existing `drawText` behaviour).
3. **−∞ display:** when value is at minimum, show **`-inf`** (or `-∞` if font supports it) before the unit — extend `tss::Slider::drawText` minimally if needed; do not break other sliders (they use integer display today).
4. **Processor clamp:** update `PluginProcessor::setInputGainDb` and `syncAudioRuntimeFromState` — replace `juce::jlimit(-24.0f, 12.0f, …)` with minimum sentinel (e.g. **`-120.0f`** representing silence; map to linear gain `0.0f` in `dbToLinearGain` when `gainDb <= kMinGainDb`). Persist saved `inputGainDb` including sentinel; session round-trip must not clamp old −24 values incorrectly.
5. Preserve existing `PluginEditor` wiring (`onValueChange` → `setInputGainDb`) — only range/display/processor clamp changes.

### AC 5 — Peak indicator visual styling

1. Size **12 × 20** (AC 3).
2. **Border:** 2 px design thickness, colour **`#002D0E`** (`ColourChart::kGreen1`) — match slider widget border weight. Use `ScaledDrawing::snappedStrokeThicknessFromDesign(2.0f, uiScale_, systemDisplayScale, StrokeSnapPolicy::kFloor)` like `Slider::drawFocusBorder`.
3. **Background (no / low signal):** **`#00785E`** idle fill — SSOT: **`SliderLook::valueBarEnabled`** from `sliderLookFromSkin` (`ColourChart::kGreen3`). Do **not** use `trackEnabled` (that token is `kGreen1`, the dark border colour).
4. **Level fill:** proportional vertical fill using **`#00DDAD`** — SSOT: **`SliderLook::textEnabled`** / `ColourChart::kGreen4`.
5. Replace current alpha-blended envelope-colour fill (`SkinColourId::kEnvelopeDisplayEnvelope`) for header peak only. Resolve colours via `SliderLook` cached in `setSkin` (mirror how `tss::Slider` stores `look_`).

### AC 6 — Activity LED visual styling

1. Size **12 × 12**, square (AC 3).
2. **Border:** 2 px, `#002D0E` / `kGreen1` — same stroke pattern as peak indicator.
3. **Off / idle:** fill `#00785E` — SSOT: **`SliderLook::valueBarEnabled`** (`kGreen3`).
4. **On / active:** solid fill `#00DDAD` (`SliderLook::textEnabled` / `kGreen4`) at level 1.0; intermediate decay levels may lerp alpha toward idle — pulse+decay behaviour unchanged (Story 2.8 UX-DR3).
5. Remove vertical-bar fill metaphor; square LED fills entire inner rect (inside border). Drop `kEnvelopeDisplayEnvelope` for header LEDs.

### AC 7 — Tests and build

1. **`MidiActivityTrackerTests`:** `kOutbound` path independent of `kInstrument` / `kEditor`; notify sets level; decay behaves like other paths.
2. **Optional:** `MidiManagerTests` assert outbound notify on successful dispatch (closes 2.8 deferred item if cheap).
3. No layout pixel tests required — manual UAT checklist in Completion Notes.
4. All existing unit tests pass; Standalone + VST3 build cleanly.

## Tasks / Subtasks

- [ ] **Refactor `HeaderPanel::resized()`** (AC: #1, #2, #3)
  - [ ] Replace `kLeftPadding_` with `kGap_` for routing row origin
  - [ ] Implement packet helpers (internal `kGap_`, external `2×kGap_`)
  - [ ] Place LEDs adjacent to MIDI FROM / MIDI TO / KEYBOARD FROM combos
  - [ ] Remove instrument/editor labels and old LED cluster
  - [ ] Unify combo widths; update size constants; centre 12×12 LEDs vertically

- [ ] **Outbound MIDI activity** (AC: #2, #7)
  - [ ] Extend `MidiActivityTracker::Path` + atomic + tests
  - [ ] Notify `kOutbound` in `MidiManager::dispatchOutboundMessage` (both send branches)
  - [ ] Wire new LED in `HeaderPanel` + `PluginEditor` timer

- [ ] **Input gain slider + processor** (AC: #4)
  - [ ] Range −∞…+12 dB (`setRange` with min sentinel), `setUnit("dB")`, `-inf` display at minimum
  - [ ] Width 60 px; update `PluginProcessor` clamp + `dbToLinearGain` for silence at min

- [ ] **Restyle `PeakIndicator` and `ActivityLed`** (AC: #5, #6)
  - [ ] Cache `SliderLook` in `setSkin`; border + square LED + peak colours from look
  - [ ] Use `ScaledDrawing` for 2 px border stroke

- [ ] **Cleanup** (AC: #2)
  - [ ] Remove orphaned `kInstrumentLabel` / `kEditorLabel` from `PluginDisplayNames.h`
  - [ ] Note in Completion Notes: Story 2.8 label placement superseded; amend decision log D-014

- [ ] **Manual UAT + build** (AC: #7)
  - [ ] Verify gaps at 100% and 150% UI scale
  - [ ] Three MIDI LEDs + peak responds to audio input

## Dev Notes

### Brownfield baseline (verified 2026-06-05 @ working tree)

| Item | Current state | File |
|------|---------------|------|
| Left padding | `kLeftPadding_` = 15 px | `HeaderPanel.h:101` |
| Gap between packets | `gap * 2` after each combo in `placeLabelAndCombo` | `HeaderPanel.cpp:179` |
| LEDs | Separate labelled cluster after peak | `HeaderPanel.cpp:210-211` |
| Outbound LED | **Missing** | — |
| `MidiActivityTracker` paths | `kInstrument`, `kEditor` only | `MidiActivityTracker.h:15-18` |
| Combo widths | MIDI 96, Audio 72 | `HeaderPanel.h:88-89` |
| Input gain | −24…+12 dB, width 64, no unit | `HeaderPanel.cpp:89` |
| Processor gain clamp | `jlimit(-24, 12, …)` | `PluginProcessor.cpp:352,372` |
| Peak | 8×20, envelope colour fill | `PeakIndicator.cpp:22-36` |
| Activity LED | 8×20 vertical bar, envelope colour | `ActivityLed.cpp:22-36` |
| Header timer | Peak + 2 LEDs @ 30 Hz | `PluginEditor.cpp:14-34` |

**Note:** Story 2.8 (`MidiActivityTracker`, `ActivityLed`) exists in **uncommitted** working-tree changes. Merge/rebase before dev if branch diverges.

### Layout algorithm (reference)

```
x = kGap_
for each packet:
  place label at x; x += labelW + kGap_
  place combo/slider at x; x += controlW + kGap_
  [optional LED at x; x += ledW + kGap_]
  x += kGap_   // second half of inter-packet gap (total 2*kGap_ before next label)
```

Last element of packet 5 (peak) followed by `2×kGap_` before SCALE/SKIN cluster (unchanged right-side layout).

Suggested `resized()` refactor — replace `placeLabelAndCombo` / `placeLabelAndLed` with:

```cpp
// Packet-internal gap = gap; after packet += gap (total inter-packet = 2*gap)
auto placePacketLabel = [&](tss::Label& label, float labelW) { ... x += labelW + gap; };
auto placePacketControl = [&](juce::Component& c, float w) { ... x += w + gap; };
auto endPacket = [&]() { x += gap; };
```

### Colour SSOT (corrected)

| Role | Design hex | Codebase token | SliderLook field |
|------|------------|----------------|------------------|
| Border 2 px | `#002D0E` | `ColourChart::kGreen1` | N/A (draw with `kGreen1` or `look.focusBorder` — verify UAT) |
| Idle / background fill | `#00785E` | `ColourChart::kGreen3` | **`valueBarEnabled`** |
| Active / level fill | `#00DDAD` | `ColourChart::kGreen4` | **`textEnabled`** |

Epics text mentions `#0078E5` — treat as typo for **`#00785E`** unless owner confirms otherwise in UAT.

Prefer resolving colours through `sliderLookFromSkin(*skin_)` inside widget `setSkin` rather than duplicating hex literals.

### Story 2.8 supersession

Story 2.8 AC #6 placed **INSTRUMENT :** / **EDITOR :** labels after the peak indicator. This story **relocates** LEDs adjacent to port combos and **drops** those labels per updated UX. Decision log D-014 / 2026-05-23 “full labels” entry should be amended in Completion Notes.

### Scope boundaries

- **In scope:** left routing cluster layout, three activity LEDs, peak/slider/LED styling, input gain range/unit/processor clamp.
- **Out of scope:** FR-41 logo popup, moving SCALE/SKIN to popup (Story 7.8), ComboBox border styling, `ActivityLed` unused member cleanup (deferred 2.8 → fix here if trivial).

### Prior art — copy these patterns

| Pattern | Reference |
|---|---|
| 2 px scaled border | `Slider.cpp` — `ScaledDrawing::snappedStrokeThicknessFromDesign` + `drawRect` |
| SliderLook from skin | `LookBuilders.cpp` — `sliderLookFromSkin` |
| Header timer | `PluginEditor::HeaderRefreshTimer` — extend, do not add 4th timer |
| Activity decay | `MidiActivityTracker.cpp` — reuse constants for `kOutbound` |
| Outbound notify hook | `MidiManager.cpp:352-374` — add `notifyActivity(kOutbound)` after successful send |

### Manual UAT checklist

1. **Layout:** At 100% and 150% scale — first control starts 5 px from left; 10 px between packets; LEDs flush right of their combos.
2. **Editor LED:** APVTS slider tweak → LED after MIDI FROM pulses.
3. **Instrument LED:** Keyboard From notes (standalone) or host MIDI (plugin) → LED after KEYBOARD FROM pulses.
4. **Outbound LED:** Any successful MIDI send (notes or SysEx) → LED after MIDI TO pulses.
5. **Peak:** Audio input → 12×20 bar with green border; fill rises with level.
6. **Input gain:** Drag to min → shows `-inf dB`; audio mutes; +12 dB at max.
7. **Regression:** Port combos, SCALE/SKIN, UI Elements still functional; session save/reload ports unchanged.

## Technical Requirements

- **Thread safety:** LED levels read on message thread only (`HeaderRefreshTimer`). `notifyActivity(kOutbound)` from MIDI manager thread — same as existing paths (lock-free atomics).
- **Dependency direction:** `PeakIndicator` / `ActivityLed` may include `LookBuilders.h` / `WidgetLooks.h` — no Core deps. `MidiActivityTracker` stays GUI-free.
- **LED assignment vs path:** Editor→MIDI FROM combo; Instrument→KEYBOARD FROM; Outbound→MIDI TO — matches epics Given/Then wording.
- **Gain −∞:** Use finite sentinel in slider range (e.g. `-120.0f`); `dbToLinearGain` returns `0.0f` at/below sentinel. Do not use `std::numeric_limits` in APVTS property (non-finite JSON/XML risk).
- **Vertical centre 12×12 LED in 20 px row:** `ledY = y + (h - ledH) / 2` where `h=20`, `ledH=12` scaled.

## Architecture Compliance

- AD-3 queue: outbound LED reflects consumer send success — hook in `MidiManager::dispatchOutboundMessage` only (not enqueue).
- Panel rule: `HeaderPanel` composes widgets; `PluginEditor` wires timer reads — no direct `MidiManager` in GUI [architecture.md §Panel rule].
- Core ↛ GUI: activity logic in `MidiActivityTracker`; widgets paint from `SliderLook` only.
- FR-39 partial polish — full shell deferred to Story 7.8 (FR-41).

## Library / Framework Requirements

- **JUCE 8.0.12** — no new dependencies.
- **ScaledDrawing** — existing helper in GUI layer for HiDPI stroke snapping.
- Reuse `tss::Slider`, `tss::ActivityLed`, `tss::PeakIndicator` — extend in place, no new widget types.

## File Structure Requirements

| Action | Path |
|--------|------|
| UPDATE | `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.{h,cpp}` |
| UPDATE | `Source/Core/MIDI/MidiActivityTracker.{h,cpp}` |
| UPDATE | `Source/Core/MIDI/MidiManager.cpp` |
| UPDATE | `Source/Core/PluginProcessor.cpp` (gain clamp + `dbToLinearGain`) |
| UPDATE | `Source/GUI/PluginEditor.cpp` |
| UPDATE | `Source/GUI/Widgets/ActivityLed.{h,cpp}` |
| UPDATE | `Source/GUI/Widgets/PeakIndicator.{h,cpp}` |
| UPDATE | `Source/GUI/Widgets/Slider.cpp` (optional `-inf` display) |
| UPDATE | `Source/Shared/Definitions/PluginDisplayNames.h` |
| UPDATE | `Tests/Unit/MidiActivityTrackerTests.cpp` |
| UPDATE (optional) | `Tests/Unit/MidiManagerTests.cpp` |
| UPDATE | `CMakeLists.txt` — only if new files (unlikely) |

## Testing Requirements

- JUCE `UnitTest` pattern — extend `MidiActivityTrackerTests` for `kOutbound` independence and decay.
- Optional: spy/mock tracker in `MidiManagerTests` for outbound notify after dispatch.
- Run full `Matrix-Control_Tests` + Standalone/VST3 build via `cmake --preset macOS-ARM-Debug`.
- Manual UAT checklist above — no pixel layout unit tests.

## Previous Story Intelligence

**From Story 2.8 (done — working tree):**
- `HeaderRefreshTimer` already merges peak + 2 LEDs — add third LED read there.
- Decay: hold 150 ms, decay 350 ms, `kDecayExponent_ = 5.0f` — reuse for `kOutbound`.
- Deferred: no `MidiManager` dispatch test — optional in this story (AC 7.2).

**From Story 2.9b (done @ `bd49cd9`):**
- Routing row order: MIDI FROM → MIDI TO → KEYBOARD FROM → … — preserve combo wiring when inserting LEDs.
- Port restore / combo callbacks unchanged — layout-only story for routing controls.

**From Story 2.7 (done @ `837e423`):**
- Peak + input gain cluster exists — restyle peak, recalibrate gain range.
- Envelope colour was placeholder — this story switches to `SliderLook` tokens.

## Git Intelligence Summary

| Commit | Story | Relevance |
|--------|-------|-----------|
| `837e423` | 2.7 Audio/peak | PeakIndicator, input gain slider baseline |
| `bd49cd9` | 2.9b Routing | Header routing row, combo widths 96 |
| `e033d68` | 2.9 Consumer | `dispatchOutboundMessage` hook for outbound LED |
| Working tree | 2.8 LEDs | `MidiActivityTracker`, `ActivityLed` — commit before or during 2.11 |

## Latest Technical Information

- **JUCE Slider range:** `setRange(min, max, interval)` — use `-120.0, 12.0, 0.1` or similar; display `-inf` at min in custom `drawText`.
- **ColourChart verified:** `kGreen1=#002D0E`, `kGreen3=#00785E`, `kGreen4=#00DDAD` — [Source: `ColourChart.h`].
- **SliderLook mapping:** `trackEnabled`→kGreen1 (border), `valueBarEnabled`→kGreen3 (fill idle), `textEnabled`→kGreen4 (active) — [Source: `SkinColoursWidgetsControls.h`, `LookBuilders.cpp`].

## Project Context Reference

- C++17, JUCE 8.0.12, Allman braces, 4-space indent, `k` prefix constants [project-context.md]
- User-visible strings in `PluginDisplayNames.h`; design dimensions in `HeaderPanel` private statics (Story 2.9b pattern)
- Epic 2 polish story — does not block Epic 3; can run parallel to Story 2.10 if needed

### References

- [Source: `planning-artifacts/epics.md` § Story 2.11]
- [Source: `HeaderPanel.cpp:138-220` — current `resized()`]
- [Source: `MidiManager.cpp:352-374` — dispatch + notify hook points]
- [Source: `PluginEditor.cpp:14-34` — HeaderRefreshTimer]
- [Source: `implementation-artifacts/2-8-activity-leds-on-queue-traffic.md`]
- [Source: `SkinColoursWidgetsControls.h` — Slider colour tokens]
- [Source: `deferred-work.md` — optional MidiManager dispatch test]

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **ready-for-dev**

## Dev Agent Record

### Agent Model Used

{{agent_model_name_version}}

### Debug Log References

### Completion Notes List

### File List
