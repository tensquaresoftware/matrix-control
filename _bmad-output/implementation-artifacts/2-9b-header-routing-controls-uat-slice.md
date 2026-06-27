---
organization: Ten Square Software
project: Matrix-Control
title: Story 2.9b — Header Routing Controls (UAT Slice)
author: BMad Agent
status: done
baseline_commit: b88c870
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/2-3-instrument-path-and-editor-path-producers.md
  - implementation-artifacts/2-6-matrix-mod-bus-parameter-sysex.md
  - project-context.md
created: 2026-06-05
updated: 2026-06-05
---

# Story 2.9b: Header Routing Controls (UAT Slice)

Status: done

<!-- Intentional Epic-2 slice — partial FR-39 only. Full shell (footer, graying matrix, logo popup, Audio From, LEDs) remains Story 7.8. -->

## Story

As a sound designer validating the Epic 2 MIDI stack on hardware,
I want MIDI From, MIDI To, and Keyboard From routing combos wired in `HeaderPanel`,
so that I can configure ports and run manual UAT without waiting for Story 7.8.

## Rationale (why 2.9b, not 7.8)

| Blocker today | Owner story |
|---|---|
| SysEx from APVTS never reaches synth | **2.9** (queue consumer) — **must be done first** |
| No UI to pick MIDI ports | **2.9b** (this story) |
| Audio passthrough + peak | **2.7** — out of scope here |
| Instrument/Editor LEDs | **2.8** — out of scope here |
| Footer messages, full FR-3/FR-45 polish, logo popup FR-41 | **7.8** — out of scope here |

**Execution order:** `2.9` → `2.9b` → manual UAT (SM-1). `2.7` / `2.8` may proceed in parallel if desired.

## Acceptance Criteria

1. **Dependency gate:** Story **2.9** is `done` before starting 2.9b. Manual SysEx UAT is only meaningful once `MidiManager::run()` drains `MidiOutboundQueue` via `MidiSender`. **Current brownfield:** `MidiManager::run()` is still a stub (`wait` loop only — `Source/Core/MIDI/MidiManager.cpp:321-327`); sprint status `2-9-wire-midimanager-queue-consumer: backlog`.
2. **`HeaderPanel` adds three routing controls** using existing `tss::Label` + `tss::ComboBox` widgets and `PluginDisplayNames` / `PluginDesignDimensions` conventions:
   - **MIDI FROM** — label `PluginDisplayNames::HeaderPanel::kMidiFromLabel` (new constant)
   - **MIDI TO** — label `PluginDisplayNames::HeaderPanel::kMidiToLabel` (new constant)
   - **KEYBOARD FROM** — label `PluginDisplayNames::HeaderPanel::kKeyboardFromLabel` (new constant)
3. **Port list population (message thread only):**
   - MIDI FROM + KEYBOARD FROM (standalone): `juce::MidiInput::getAvailableDevices()`
   - MIDI TO: `juce::MidiOutput::getAvailableDevices()`
   - First item = sentinel **none** (`"—"`, item id `0`) — interim sentinel until Epic 4.2 formalizes combobox sentinel states
   - Subsequent items: human-readable `device.name`, item id = stable hash of `device.identifier` **or** store `identifier` in a side map keyed by item id (document choice in Dev Notes; must round-trip to `device.identifier` for `PluginProcessor` APIs)
4. **MIDI FROM wiring:**
   - `onChange` → `PluginProcessor::setMidiInputPort(identifier)` (empty string when sentinel selected)
   - Property SSOT: existing `apvts.state` property `midiInputPortId` (already written by `setMidiInputPort` — `PluginProcessor.cpp:247-256`)
   - On `PluginEditor` construction: restore saved `midiInputPortId` — repopulate combo, select matching item, call `setMidiInputPort` if non-empty (re-open port after session load). Port open does **not** require MIDI thread running (`MidiManager::setMidiInputPort` opens port directly).
5. **MIDI TO wiring:** same pattern with `setMidiOutputPort` / `midiOutputPortId` (`PluginProcessor.cpp:258-267`).
6. **KEYBOARD FROM — plugin (DAW):** combo **visible, disabled (grayed)**, fixed display **HOST** per PRD glossary and FR-45. No port selection; instrument path continues to use host MIDI buffer (Story 2.3 D-056). Do **not** write `keyboardFromEnabled` in plugin mode.
7. **KEYBOARD FROM — standalone:** combo enabled; selecting a port (non-sentinel) sets `keyboardFromEnabled=true` and persists new property `keyboardFromPortId` (`juce::String`, default empty). Clearing selection sets `keyboardFromEnabled=false` and clears `keyboardFromPortId`. **Core wiring required:** new minimal `Core::KeyboardFromMidiInput` in `Source/Core/MIDI/KeyboardFromMidiInput.{h,cpp}` — opens selected `juce::MidiInput`, filters to Note On/Off, CC, Pitch Bend (same allow-list as `InstrumentMidiForwarder::isAllowedInstrumentMessage` — `InstrumentMidiForwarder.cpp:7-10`), enqueues via `MidiOutboundQueue::enqueueRealtime` on the **MIDI callback thread** (never audio thread). Zero GUI dependencies. `PluginProcessor` owns instance; start/stop with port selection changes. **Do not** route Keyboard From through `processBlock` host buffer — standalone uses dedicated input port per PRD.
8. **Layout:** routing combos placed in header **left zone** (before or replacing the current left cluster of SCALE/SKIN if width requires — interim compromise documented in Dev Notes). `resized()` uses `uiScale_` multiplier like existing header controls. Header height unchanged (`PluginDesignDimensions::GUI::kHeaderHeight` = 32). FR-41 logo popup relocation of skin/scale is **deferred** — do not implement logo popup in this story.
9. **Explicitly out of scope (defer without partial stubs):** Audio From, Input Gain, peak indicator (2.7); Instrument/Editor LEDs (2.8); footer / `uiMessageText` (7.8); Device Inquiry UI lock (8.x); SETTINGS button; FR-41 logo popup; Epic 4.2 sentinel polish beyond interim `"—"`; `juce::MidiDeviceListConnection` hot-plug refresh (optional — document if deferred).
10. **Persistence:** port properties already ride in `getStateInformation` / `setStateInformation` via `apvts.copyState()` (`PluginProcessor.cpp:224-244`). Verify round-trip: save project → reload → combos show prior ports and ports re-open. No new persistence layer.
11. **Tests:** `HeaderPanelRoutingTests` or split tests acceptable — at minimum:
    - Unit/integration: mock or test harness verifying `PluginProcessor::setMidiInputPort` / `setMidiOutputPort` invoked with correct identifier on combo change (GUI test optional if harness-heavy)
    - `KeyboardFromMidiInputTests`: allowed messages enqueue; SysEx/PC stripped; disabled when no port
    - All existing unit tests pass; plugin builds VST3 + Standalone

## Tasks / Subtasks

- [x] **Confirm 2.9 complete** (AC: #1)
  - [x] `MidiManager::run()` dequeues and sends; parameter SysEx reaches hardware in smoke test

- [x] **Display names + IDs** (AC: #2)
  - [x] Add `kMidiFromLabel`, `kMidiToLabel`, `kKeyboardFromLabel`, `kHostDisplay` (`"HOST"`) to `PluginDisplayNames::HeaderPanel`
  - [x] Add combo width constants to `PluginDesignDimensions` if needed (divisible by 4)

- [x] **Extend `HeaderPanel`** (AC: #2, #8)
  - [x] Members: 3 labels + 3 combos; getters for editor wiring
  - [x] `populateMidiPortLists()` / `refreshPortLists()` — callable from `PluginEditor` on show
  - [x] `resized()` layout for routing row/cluster
  - [x] `setPluginMode(bool isPlugin)` — gray Keyboard From when plugin

- [x] **Wire `PluginEditor`** (AC: #4, #5, #6, #10)
  - [x] `onChange` lambdas → `pluginProcessor.setMidi*Port`
  - [x] Restore `midiInputPortId` / `midiOutputPortId` at construction (mirror gui scale restore pattern — `PluginEditor.cpp:41-46`)
  - [x] Pass `!isStandaloneWrapper()` into `HeaderPanel::setPluginMode` (use same `isStandaloneWrapper()` pattern as `PluginProcessor.cpp:27-35`)

- [x] **Standalone Keyboard From Core** (AC: #7)
  - [x] `KeyboardFromMidiInput.{h,cpp}` — filter + enqueue
  - [x] `PluginProcessor::setKeyboardFromPort(const juce::String& deviceId)` — toggles `keyboardFromEnabled`, manages input lifecycle
  - [x] Init `keyboardFromPortId` in `initializeMidiPortProperties()`
  - [x] `CMakeLists.txt` — plugin + test targets

- [x] **Tests + manual UAT checklist** (AC: #11)
  - [x] Unit tests for keyboard forwarder
  - [x] Document SM-1 steps in Completion Notes after dev

### Review Findings

- [x] [Review][Patch] Restore port manquant — combo sentinel mais `set*Port(savedId)` quand même [`Source/GUI/PluginEditor.cpp:51-66`]
- [x] [Review][Patch] `setKeyboardFromPort` échec `setPort` — pas rollback props, port fermé après switch raté [`Source/Core/PluginProcessor.cpp:289-293`, `Source/Core/MIDI/KeyboardFromMidiInput.cpp:23-44`]
- [x] [Review][Patch] Tests AC #11 manquants — `setMidiInputPort` / `setMidiOutputPort` / `setKeyboardFromPort` + props APVTS [`Tests/`]
- [x] [Review][Defer] Combo/backend mismatch si `MidiManager::set*Port` échoue — pattern brownfield pre-existing, 2.9b hérite [`Source/Core/PluginProcessor.cpp:249-268`] — deferred, pre-existing
- [x] [Review][Defer] Double forwarding host buffer en standalone — AC #7 assume buffer hôte vide ; edge case host non vide [`Source/Core/PluginProcessor.cpp:210-218`] — deferred, hors nominal UAT
- [x] [Review][Defer] MIDI FROM + KEYBOARD FROM même device — pas de garde UI, comportement OS indéfini [`HeaderPanel` / `PluginProcessor`] — deferred, hors scope UAT slice
- [x] [Review][Defer] `setStateInformation` sans resync ports/combos — restore uniquement au ctor `PluginEditor` [`PluginProcessor.cpp:233-246`] — deferred, pattern plugin standard ; reload session OK
- [x] [Review][Defer] Thread safety `keyboardFromEnabled` message/audio — `juce::var` non atomique [`PluginProcessor.cpp:213-214`] — deferred, pre-existing story 2.3

## Dev Notes

### Brownfield baseline (verified 2026-06-05 @ `b88c870`)

| Component | Current state | File |
|---|---|---|
| `HeaderPanel` | SCALE + SKIN + UI Elements only; no MIDI combos | `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.{h,cpp}` |
| `PluginDisplayNames::HeaderPanel` | `kSkinLabel`, `kUiScaleLabel`, `kUiElementsButton` only | `Source/Shared/Definitions/PluginDisplayNames.h:14-19` |
| `setMidiInputPort` / `setMidiOutputPort` | Implemented; persist `midiInputPortId` / `midiOutputPortId` | `PluginProcessor.{h,cpp}` |
| `keyboardFromEnabled` | Init in `initializeMidiPortProperties()`; read in `processBlock` standalone branch | `PluginProcessor.cpp:287-301, 208-214` |
| `keyboardFromPortId` | **Missing** | — |
| `KeyboardFromMidiInput` | **Missing** | — |
| `MidiManager::run()` | Stub — no dequeue | `MidiManager.cpp:321-327` |
| `PluginEditor` header wiring | Skin + scale + UI Elements only | `PluginEditor.cpp:39-69` |
| `InstrumentMidiForwarder` allow-list | Note On/Off, CC, Pitch Bend | `InstrumentMidiForwarder.cpp:7-10` |

### Brownfield APIs (reuse — do not reinvent)

```cpp
// PluginProcessor.h — already exist
void setMidiInputPort(const juce::String& deviceId);
void setMidiOutputPort(const juce::String& deviceId);

// apvts.state properties — already initialized in initializeMidiPortProperties()
"midiInputPortId"   // juce::String
"midiOutputPortId"  // juce::String
"keyboardFromEnabled" // bool, default false (Story 2.3)
```

`MidiManager::setMidiInputPort` opens **MIDI FROM** (editor path / SysEx receive via `MidiReceiver`). **Do not** reuse MIDI FROM port for Keyboard From — PRD defines separate ports.

### New APIs and property

| Item | Type | Purpose |
|---|---|---|
| `keyboardFromPortId` | `juce::String` | Standalone master keyboard port identifier |
| `setKeyboardFromPort(deviceId)` | `PluginProcessor` method | Toggle `keyboardFromEnabled`, manage `KeyboardFromMidiInput` lifecycle |
| `KeyboardFromMidiInput` | `Core` class | Dedicated `juce::MidiInput` → `MidiOutboundQueue::enqueueRealtime` |

### Item id ↔ device identifier mapping

`juce::ComboBox` item ids are `int`. `device.identifier` is `juce::String`. Recommended pattern:

- Item id `0` = sentinel none
- Items `1…N` map via `std::vector<juce::String> portIdentifiers_` indexed by `itemId - 1`
- Alternative: `juce::String::hashCode()` on identifier — only if collision risk accepted (prefer vector map)

### Header layout interim compromise

Current header (left → right): `SCALE | SKIN | … | UI Elements`. Routing combos are **higher priority for UAT** than scale/skin discoverability. Acceptable interim options (pick one in implementation, document in Completion Notes):

1. Routing combos left; move SCALE/SKIN right of routing (still before UI Elements)
2. Routing combos left; SCALE/SKIN remain but compress combo widths

Do **not** block story on Figma — D-014a logo popup is Epic 7.8.

### Relationship to Story 7.8

| FR / UX item | 2.9b | 7.8 |
|---|---|---|
| MIDI From / To combos | ✅ | polish + footer feedback |
| Keyboard From plugin grayed HOST | ✅ | same |
| Keyboard From standalone | ✅ minimal Core | polish |
| Audio From + Gain + peak | ❌ → 2.7 | verify integration |
| Activity LEDs | ❌ → 2.8 | verify integration |
| FR-3 full prefs policy | partial (ports only) | complete |
| FR-44 footer on grayed click | ❌ | ✅ |
| FR-41 logo popup | ❌ | conditional |
| FR-45 full graying matrix | partial (Keyboard From only) | complete |

When 7.8 starts, **extend** 2.9b wiring — do not duplicate port logic.

### Manual UAT checklist (SM-1 prep — run after 2.9 + 2.9b)

1. Standalone or Live: select **MIDI TO** → interface port wired to Matrix-1000 IN
2. Select **MIDI FROM** → interface port wired to Matrix-1000 OUT (SysEx return)
3. Twist PATCH parameter → confirm SysEx on wire (LED or external monitor)
4. Twist MASTER parameter → confirm full 0x03 SysEx
5. Twist Matrix Mod bus field → confirm single 0x0B per bus change
6. Plugin mode: **KEYBOARD FROM** shows HOST, grayed; armed Live track forwards notes
7. Standalone (optional): **KEYBOARD FROM** port → notes reach Matrix via MIDI TO
8. Save/reload session → port selections restored

## Technical Requirements

- **Thread safety:** Combo `onChange` on message thread only. `KeyboardFromMidiInput` MIDI callback → `enqueueRealtime` only (AD-3). Never `MidiOutput::sendMessageNow` from audio or MIDI input callback.
- **Dependency direction:** `KeyboardFromMidiInput` in `Source/Core/MIDI/` — zero GUI deps. `HeaderPanel` calls `PluginProcessor` facades via `PluginEditor` lambdas — never `MidiManager` directly (architecture panel rule).
- **Standalone vs plugin:** Detect via `isStandaloneWrapper()` anonymous namespace in `PluginProcessor.cpp` — expose `PluginProcessor::isStandalone()` public wrapper if editor cannot access anonymous fn.
- **Port restore timing:** `midiManager` constructed in `PluginProcessor` ctor (`PluginProcessor.cpp:57`) — safe to call `setMidi*Port` from `PluginEditor` ctor before `prepareToPlay`. MIDI thread starts later in `prepareToPlay` — port open is independent.
- **Keyboard From standalone path:** When port selected, `keyboardFromEnabled=true` arms `processBlock` instrument path **and** `KeyboardFromMidiInput` feeds queue from dedicated port. Ensure no double-forward if both paths active — standalone `processBlock` receives empty host buffer; only `KeyboardFromMidiInput` enqueues.

## Architecture Compliance

- AD-3 queue: Keyboard From is a **producer** like `InstrumentMidiForwarder`; consumer remains `MidiManager::run()` (Story 2.9)
- AD-7 persistence: port ids in session XML via existing `apvts.copyState()` — no separate prefs file
- Panel rule: `HeaderPanel` composes widgets; `PluginEditor` wires callbacks to `PluginProcessor` [Source: architecture.md §Panel rule]
- FR-39 partial: MIDI From/To/Keyboard From only; Audio From/LEDs deferred per epic split

## Library / Framework Requirements

- **JUCE 8.0.12** — `juce::MidiInput::getAvailableDevices()`, `juce::MidiOutput::getAvailableDevices()`, `juce::MidiInput::create()` / `start()` / `stop()` for Keyboard From
- No new third-party dependencies
- Use existing `tss::ComboBox`, `tss::Label` widgets — not raw `juce::ComboBox`

## File Structure Requirements

| Action | Path |
|---|---|
| UPDATE | `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.{h,cpp}` |
| UPDATE | `Source/Shared/Definitions/PluginDisplayNames.h` |
| UPDATE | `Source/Shared/Definitions/PluginDesignDimensions.h` (optional widths) |
| UPDATE | `Source/GUI/PluginEditor.cpp` |
| UPDATE | `Source/Core/PluginProcessor.{h,cpp}` |
| NEW | `Source/Core/MIDI/KeyboardFromMidiInput.{h,cpp}` |
| NEW | `Tests/Unit/KeyboardFromMidiInputTests.cpp` |
| NEW (optional) | `Tests/Unit/HeaderPanelRoutingTests.cpp` |
| UPDATE | `CMakeLists.txt` — add new `.cpp` to `PLUGIN_SOURCES` + `Matrix-Control_Tests` |

## Testing Requirements

- Follow existing JUCE `UnitTest` pattern in `Tests/Unit/` (see `InstrumentMidiForwarderTests.cpp`)
- `KeyboardFromMidiInputTests`: mirror forwarder filter matrix — Note/CC/PB enqueue; PC/SysEx stripped; no enqueue when port closed
- Processor port setter tests: can use direct `setMidiInputPort` / `setMidiOutputPort` calls + verify APVTS properties (no GUI harness required)
- All existing unit tests pass; `cmake --preset macOS-ARM-Debug` + test target green
- Manual SM-1 checklist (Dev Notes) after 2.9 consumer verified on hardware

## Previous Story Intelligence

**From Story 2.3 (done):**
- `keyboardFromEnabled` property seeded; standalone arming reads it in `processBlock` — 2.9b must **set** it via Keyboard From UI
- Instrument path allow-list established — reuse identical filter in `KeyboardFromMidiInput`
- Queue accumulates until 2.9 consumer — do not add direct `MidiSender` sends

**From Story 2.6 (done @ `b88c870`):**
- APVTS → SysEx dispatch wired for PATCH (0x06), MASTER (0x03), Matrix Mod (0x0B) — UAT steps 3–5 depend on 2.9 consumer, not 2.9b
- Pattern: thin Core dispatcher + `PluginProcessor::valueTreePropertyChanged` — 2.9b does **not** touch dispatchers
- CMake pattern: new `.cpp` in both plugin and test `target_sources`

## Git Intelligence Summary

Recent Epic 2 commits (newest first):

| Commit | Story | Relevance |
|---|---|---|
| `b88c870` | 2.6 Matrix Mod 0x0B | UAT step 5 — bus SysEx producer ready |
| `2e21abf` | 2.5 Master 0x03 | UAT step 4 |
| `d1fca4f` | 2.4 Patch 0x06 | UAT step 3 |
| `9202ad2` | 2.3 Instrument/Editor paths | `keyboardFromEnabled`, `InstrumentMidiForwarder` — direct reuse |
| `bb370e6` | 2.1 Outbound queue | `MidiOutboundQueue` SSOT for Keyboard From enqueue |

No header routing commits yet — greenfield UI wiring on brownfield processor APIs.

## Latest Technical Information

- **JUCE 8 MIDI device API:** `getAvailableDevices()` returns `juce::Array<MidiDeviceInfo>` with stable `identifier` strings — use `identifier` for persistence, `name` for display. Device list can change at runtime; optional `MidiDeviceListConnection` callback for refresh (defer if scope creep).
- **ComboBox item ids:** JUCE docs recommend positive ints; id `0` reserved for "no selection" in some patterns — sentinel at id `0` with `setSelectedId(0)` is consistent with project interim approach until Epic 4.2.

## Project Context Reference

- C++17, JUCE 8.0.12, Allman braces, 4-space indent, `k` prefix constants [project-context.md]
- `Source/` include root — paths like `"Core/MIDI/KeyboardFromMidiInput.h"`
- Epic 2 = MIDI transport; this story unblocks SM-1 hardware validation without waiting for Epic 7 shell

### References

- FR-39 partial, FR-45 partial, FR-3 partial — [prd.md §4.11]
- Header control glossary — [prd.md §3]
- D-014 header routing — [prd/.decision-log.md]
- AD-3 queue producers/consumer — [architecture.md]
- Queue accumulation until 2.9 — [2-3-instrument-path-and-editor-path-producers.md AC #6]
- `keyboardFromEnabled` standalone arming — [PluginProcessor.cpp processBlock]

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **ready-for-dev**
- **Blocker reminder:** Do not start implementation until Story 2.9 (`MidiManager::run()` consumer) is `done`

## Dev Agent Record

### Agent Model Used

Composer (Cursor)

### Completion Notes

- **2.9 gate:** Verified `MidiManager::run()` dequeues via `outboundQueue_.dequeue()` and dispatches (Story 2.9 done).
- **Header layout:** Option 1 — routing combos left (MIDI FROM → MIDI TO → KEYBOARD FROM), then SCALE/SKIN, UI Elements right. Width constants live in `HeaderPanel` private static members (divisible by 4).
- **Port mapping:** Item id `0` = sentinel `"—"`; ids `1…N` map via `std::vector<juce::String> portIdentifiers_` indexed by `itemId - 1`.
- **Plugin mode:** KEYBOARD FROM shows HOST, disabled; no `keyboardFromEnabled` / `keyboardFromPortId` writes.
- **Standalone Keyboard From:** `KeyboardFromMidiInput` opens dedicated `juce::MidiInput`, filters Note/CC/PB, enqueues via `enqueueRealtime` on MIDI callback thread.
- **Build/tests:** `cmake -B Builds/macOS/ARM -DMATRIX_BUILD_TESTS=ON` → `Matrix-Control_Tests` + `Matrix-Control_Standalone` green; 4 new `KeyboardFromMidiInputTests`.

**SM-1 manual UAT checklist (run on hardware):**

1. Standalone or Live: select **MIDI TO** → interface port wired to Matrix-1000 IN
2. Select **MIDI FROM** → interface port wired to Matrix-1000 OUT (SysEx return)
3. Twist PATCH parameter → confirm SysEx on wire (LED or external monitor)
4. Twist MASTER parameter → confirm full 0x03 SysEx
5. Twist Matrix Mod bus field → confirm single 0x0B per bus change
6. Plugin mode: **KEYBOARD FROM** shows HOST, grayed; armed Live track forwards notes
7. Standalone (optional): **KEYBOARD FROM** port → notes reach Matrix via MIDI TO
8. Save/reload session → port selections restored

### File List

- `Source/Core/MIDI/KeyboardFromMidiInput.h` (new)
- `Source/Core/MIDI/KeyboardFromMidiInput.cpp` (new)
- `Source/Core/PluginProcessor.h` (updated)
- `Source/Core/PluginProcessor.cpp` (updated)
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h` (updated)
- `Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp` (updated)
- `Source/GUI/PluginEditor.cpp` (updated)
- `Source/Shared/Definitions/PluginDisplayNames.h` (updated)
- `Tests/Unit/KeyboardFromMidiInputTests.cpp` (new)
- `CMakeLists.txt` (updated)

### Change Log

- 2026-06-05 — Code review patches: restore via combo identifier, `setKeyboardFromPort` rollback + combo revert on failure, `MidiPortRoutingPropertyTests`.
- 2026-06-05 — Story 2.9b: Header routing controls (MIDI FROM/TO, Keyboard From), `KeyboardFromMidiInput` Core producer, port restore wiring, unit tests.
