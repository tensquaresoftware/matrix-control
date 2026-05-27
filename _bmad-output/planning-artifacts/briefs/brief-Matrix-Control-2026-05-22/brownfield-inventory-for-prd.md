# Brownfield Inventory for PRD — Matrix-Control

**Purpose:** Help the product owner decide what to keep, consolidate, or rework before PRD Update.  
**Baseline tag:** `v0.0.66-alpha-pre-bmad`  
**Generated:** 2026-05-23  
**Revised:** 2026-05-23 — M4L cross-reference removed (obsolete)

**Companion documents:**
- Product brief v0.3: `brief.md`
- Decision log: `.decision-log.md`

---

## Executive summary (PO read-this-first)

Matrix-Control today is a **visually mature Matrix-1000 editor shell** with a **solid parameter model (APVTS + descriptors)** and a **MIDI/SysEx Core stack that is written but not wired to the UI**.

| Layer | Maturity | PO takeaway |
|---|---|---|
| Descriptors / IDs / design dimensions | ✅ **Keep as-is** | Single source of truth — do not scatter |
| APVTS layout + validation | ✅ **Keep / consolidate** | Production-quality infrastructure |
| GUI widgets + panels + skins | ✅ **Keep** (visual) | Major investment — preserve layout & scaling |
| Interactive displays (env / track) | ✅ **Keep** | Bidirectional slider ↔ curve works in UI |
| Synth parameter editing (sliders/combos) | ✅ **Keep** | All Patch/Matrix/Master params attached to APVTS |
| Patch Manager UI | ⚠️ **Keep UI, replace wiring** | Buttons write timestamps — no business logic |
| Module I/C/P buttons | ⚠️ **Keep UI, implement Core** | Same timestamp stub pattern |
| MIDI / SysEx Core | ⚠️ **Keep & wire** | Code exists; `handleIncomingSysEx` empty; thread idle |
| PatchModel (APVTS ↔ 134 bytes) | ❌ **Build** | Missing entirely |
| MIDI ports / device UI | ❌ **Build** | API on processor; no HeaderPanel combos yet |
| Virtual instrument (audio + host MIDI) | ❌ **Build** | CMake still `IS_SYNTH FALSE`, `processBlock` empty |
| Patch Mutator logic | ❌ **Build** | UI only |

**CMake note (gap vs brief v0.3):** Plugin is currently registered as **audio effect** (`IS_SYNTH FALSE`, `AU_MAIN_TYPE kAudioUnitType_Effect`, `VST3_CATEGORIES Fx`). Brief targets **virtual instrument** — will require CMake/JUCE config change in a future epic.

---

## 1. Maturity legend

| Symbol | Meaning |
|---|---|
| ✅ **IMPLEMENTED** | Real logic, usable or complete |
| 🎨 **GUI-ONLY** | Visual + local interaction; no Core/MIDI effect |
| ⚠️ **PARTIAL** | Substantial code; incomplete integration |
| 🔌 **STUB** | Placeholder / empty / timestamp-only |
| ❌ **MISSING** | Not in codebase |

---

## 2. Shared definitions — `Source/Shared/Definitions/`

| Component | Status | Notes |
|---|---|---|
| `PluginIDs.h` | ✅ | All APVTS IDs, standalone widget IDs, settings keys |
| `PluginDisplayNames.h` | ✅ | User-visible labels |
| `PluginDesignDimensions.h` | ✅ | Figma reference dimensions; compile-time asserts |
| `PluginDescriptors.h` + 4 `.cpp` files | ✅ | Patch Edit, Master Edit, Matrix Mod, Patch Manager |
| `PluginHelpers.cpp` | ✅ | Group name helpers |
| `ApvtsTypes.h` | ✅ | ValueTree property conventions |
| `Matrix1000Limits.h` | ✅ | Banks, patches, bus count |

**PO decision:** ✅ **Keep entire descriptor system.** Extend only when adding parameters — never duplicate data elsewhere.

---

## 3. Core — `Source/Core/`

### 3.1 PluginProcessor

| Feature | Status | Detail |
|---|---|---|
| APVTS construction | ✅ | Full parameter tree from descriptors |
| State save/load (DAW) | ✅ | XML via APVTS |
| Descriptor validation at startup | ✅ | Throws on duplicate IDs / bad layout |
| MIDI port ID persistence | ✅ | Properties on APVTS state |
| GUI scale persistence | ✅ | `kGuiScaleId` |
| MidiManager lifecycle | ✅ | Thread start/stop in prepare/release |
| `processBlock()` | 🔌 | **Ignores audio and MIDI** |
| Bank/patch change handlers | 🔌 | Empty stubs |
| APVTS → MIDI on parameter change | ❌ | No listener sending SysEx |
| Audio From passthrough | ❌ | Not implemented |
| Instrument MIDI forward | ❌ | Not implemented |

**Files:** `PluginProcessor.cpp` (~505 lines), `PluginProcessor.h`

### 3.2 APVTS factories

| Class | Status | Lines (approx.) | Role |
|---|---|---|---|
| `ApvtsFactory` | ✅ | ~120 | Aggregates descriptors into parameters |
| `ApvtsLayoutBuilder` | ✅ | ~285 | Builds group tree; **excludes Patch Manager** (ValueTree properties) |
| `ApvtsValidator` | ✅ | ~496 | Duplicate detection, orphan groups, cycle check |

**PO decision:** ✅ **Keep.** Patch Manager correctly uses ValueTree properties, not APVTS params.

### 3.3 MIDI layer

| Class | Status | Role |
|---|---|---|
| `MidiInputPort` | ✅ | Open/close input, callback registration |
| `MidiOutputPort` | ✅ | Open/close output |
| `MidiSender` | ✅ | Send SysEx, Program Change |
| `MidiReceiver` | ✅ | Receive, buffer SysEx, wait for response |
| `MidiManager` | ⚠️ | Full **outbound API**; inbound handler **empty**; `run()` waits only |

**MidiManager outbound API (implemented, uncalled from app):**
- `setMidiInputPort` / `setMidiOutputPort`
- `sendPatch`, `sendMaster`, `sendProgramChange`
- `requestCurrentPatch`, `requestMasterData`
- `performDeviceInquiry`
- Device state properties (`deviceDetected`, `lastError`, etc.)

**MidiManager inbound:** `handleIncomingSysEx()` validates then **switch with empty cases** — no decode → APVTS update.

**PO decision:** ✅ **Keep Core MIDI classes.** Wire to UI + PatchModel; implement inbound decode path.

### 3.4 SysEx stack

| Class | Status | Role |
|---|---|---|
| `SysExParser` | ✅ | Structure, checksum, message type, nibble validation |
| `SysExEncoder` | ✅ | Patch, master, request, device inquiry |
| `SysExDecoder` | ✅ | Decode patch/master/device ID |
| `SysExConstants` | ✅ | 134 B patch, 172 B master; documents v1 exclusions |

**Unit tests:** `SysExParserTests`, `SysExEncoderTests` — ✅ real tests  
**MidiManagerTests** — 🔌 `expect(true)` + TODO

**PO decision:** ✅ **Keep & extend tests** (decoder, round-trip, PatchModel integration).

### 3.5 Loggers & exceptions

| Component | Status |
|---|---|
| `MidiLogger` | ✅ Debug/file logging |
| `ApvtsLogger` | ✅ Parameter change logging (debug) |
| `ExceptionPropagator` | ✅ Factory errors → footer message properties |

### 3.6 Missing Core (not started)

| Component | Needed for |
|---|---|
| `PatchModel` / parameter packing | APVTS ↔ 134-byte buffer |
| `MasterModel` | APVTS ↔ 172-byte buffer |
| `ActionDispatcher` | Init/Copy/Paste/Store, patch manager buttons |
| `PatchFileService` | `.syx` load/save/validate folder |
| `PatchMutatorEngine` | Randomize/mutate logic |
| `ClipboardService` | Type-aware module clipboard |
| Unified MIDI outbound queue | Dual-role Instrument + Editor (brief v0.3) |

---

## 4. GUI — `Source/GUI/`

### 4.1 Shell & settings

| Component | Status | Detail |
|---|---|---|
| `PluginEditor` | ✅ | Skin Black/Cream, UI scale, test harness toggle |
| `MainComponent` | ✅ | Header + Body + Footer; propagates `uiScale` |
| `HeaderPanel` | ⚠️ | UI scale + skin + UI Elements button only |
| `FooterPanel` | ⚠️ | Displays `uiMessageText` / severity from ValueTree |
| UI Scale 50–200% | ✅ | `ScaledLayout`, persisted, recursive propagation |
| Skins (Black/Cream) | ✅ | `Skin`, `LookBuilders`, colour tokens |
| Typography | ✅ | `TypographyStyles`, PT Sans Narrow (binary data) |

**Header missing (brief v0.3):** MIDI From, MIDI To, Keyboard From, Audio From, Input Gain, Instrument/Editor LEDs.

### 4.2 Factories & validation

| Component | Status |
|---|---|
| `WidgetFactory` | ✅ Creates widgets from descriptors; lookup maps |
| `WidgetFactoryValidator` | ✅ Validates factory vs descriptors |

### 4.3 Custom widgets

| Widget | Status | Notes |
|---|---|---|
| `Slider` | ✅ | Custom paint, APVTS via `ParameterCell` |
| `ComboBox` | ✅ | Custom popup menus |
| `Button` | ✅ | Init/Copy/Paste/action variants |
| `NumberBox` | ✅ | Patch manager bank/patch numbers |
| `Toggle` | ✅ | Patch mutator module toggles |
| `ParameterCell` | ✅ | APVTS attachment wrapper |
| `ModulationBusCell` | ✅ | 3 params + init bus button (stub action) |
| `ModuleHeader` | 🎨 | I/C/P buttons → timestamp property |
| `EnvelopeDisplay` | ✅ | Draw + mouse drag; synced via `PatchEditDisplaysPanel` |
| `TrackGeneratorDisplay` | ✅ | 5-point curve; synced bidirectionally |
| `PatchNameDisplay` | 🎨 | Static `"--------"`; not bound to patch name |
| Popup menu system | ✅ | Multi-column, scrollable, positioned |

**PO decision:** ✅ **Keep all custom widgets.** Wire `PatchNameDisplay`; implement I/C/P handlers in Core.

### 4.4 PATCH EDIT panels

| Module panel | APVTS params | I/C/P |
|---|---|---|
| DCO 1, DCO 2 | ✅ | 🔌 timestamp |
| VCF/VCA, FM/Track, Ramp/Portamento | ✅ | 🔌 |
| ENV 1, 2, 3 | ✅ | 🔌 |
| LFO 1, 2 | ✅ | 🔌 |

| Sub-panel | Status |
|---|---|
| `PatchEditTopModulesPanel` | ✅ Layout |
| `PatchEditBottomModulesPanel` | ✅ Layout |
| `PatchEditDisplaysPanel` | ✅ **Bidirectional APVTS ↔ displays** |
| `PatchEditPanel` | ✅ Orchestrates sync wiring |

### 4.5 MATRIX MODULATION

| Component | Status |
|---|---|
| `MatrixModulationPanel` | ✅ 10 × `ModulationBusCell` with real APVTS attachments |
| Init all busses button | 🔌 timestamp only |

### 4.6 MASTER EDIT

| Module | APVTS params | Init button |
|---|---|---|
| MIDI | ✅ | 🔌 |
| Vibrato | ✅ | 🔌 |
| Misc | ✅ | 🔌 |

### 4.7 PATCH MANAGER (all 🔌 — UI complete, zero Core)

| Module | UI widgets | Business logic |
|---|---|---|
| `BankUtilityPanel` | Bank 0–9, unlock | 🔌 timestamp on bank select |
| `InternalPatchesPanel` | Nav, bank/patch NumberBox, INIT/COPY/PASTE/STORE | 🔌 partial NumberBox ↔ properties; no MIDI |
| `ComputerPatchesPanel` | Nav, combobox, OPEN/SAVE/SAVE AS | 🔌 **hardcoded factory patch names** in combobox |
| `PatchMutatorPanel` | Amount/Random, module toggles, MUTATE/RETRY/HISTORY | 🔌 properties only; no mutation algorithm |

**PO decision:** ✅ **Keep all Patch Manager panels.** Replace stub wiring with Core services.

### 4.8 Dev tooling

| Component | Status |
|---|---|
| `TestComponent` / `TestButtons` / `TestSliders` | ✅ Widget sandbox at multiple scales |
| Toggle via HeaderPanel « UI Elements » | ✅ |

**PO decision:** Keep until UI scale validated everywhere, then optional removal.

---

## 5. Tests — `Quality/Tests/Unit/`

| Test file | Status |
|---|---|
| `SysExParserTests.cpp` | ✅ Real coverage |
| `SysExEncoderTests.cpp` | ✅ Real coverage |
| `MidiManagerTests.cpp` | 🔌 Placeholder |

---

## 6. Suggested keep / rework matrix (for your review)

| Area | Recommendation | Rationale |
|---|---|---|
| Descriptor + ID system | **Keep** | Architectural invariant |
| WidgetFactory + APVTS factories | **Keep** | DRY, validated at startup |
| All GUI panels & widgets | **Keep** | Large visual investment |
| Envelope/Track displays + sync | **Keep** | Working differentiator |
| Skin / UI scale / ScaledLayout | **Keep / finish** | UI scale refactor may remain on some widgets |
| SysEx parser/encoder/decoder | **Keep** | Foundation for PatchModel |
| MidiManager API shape | **Keep / wire** | Align with brief dual-role queue |
| Patch Manager button pattern (timestamps) | **Replace** | Temporary dev hook — implement ActionDispatcher |
| ComputerPatches hardcoded names | **Replace** | Real folder scan + `.syx` validation |
| CMake plugin category (Effect) | **Change** | Must become virtual instrument per brief |
| `processBlock` empty | **Implement** | Instrument path + audio passthrough |
| PatchModel + file I/O | **Build new** | Critical path to v1 |
| OpenGL plans | **Discard** | Per brief v0.3 |

---

## 7. Recommended reading order before PRD session

1. **This document** — mark keep/rework decisions in margin  
2. **`brief.md` v0.3** — product intent  
3. **`Documentation/Development/Specifications/Functional-Specification.md`** — treat as **draft** to reconcile  

---

## 8. Open PO checklist (fill during review)

- [ ] Descriptors / Factory / APVTS: keep as-is?  
- [ ] GUI shell + widgets: keep as-is?  
- [ ] Patch Manager UI: keep, rewire?  
- [ ] SysEx Core: keep, wire inbound?  
- [ ] CMake: migrate Effect → Instrument?  
- [ ] Module I/C/P: confirm 10 modules for v1 (per brief)  

---

*End of brownfield inventory — input for PRD Update and Architecture workflows.*
