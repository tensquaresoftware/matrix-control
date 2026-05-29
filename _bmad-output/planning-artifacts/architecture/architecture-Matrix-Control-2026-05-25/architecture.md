---
organization: Ten Square Software
project: Matrix-Control
title: Architecture Decision Document
author: BMad Agent
status: draft
version: "0.2"
sources:
  - ../../prds/prd-Matrix-Control-2026-05-25/prd.md
  - ../../prds/prd-Matrix-Control-2026-05-25/addendum.md
  - ../../prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - ../../project-context.md
  - ../../../reference-docs/oberheim/index.md
created: 2026-05-29
updated: 2026-05-29
stepsCompleted: [1, 2, 3, 4, 5]
inputDocuments:
  - prd-Matrix-Control-2026-05-25/prd.md
  - prd-Matrix-Control-2026-05-25/addendum.md
  - prd-Matrix-Control-2026-05-25/.decision-log.md
  - project-context.md
  - reference-docs/oberheim/index.md
workflowType: architecture
---

# Architecture Decision Document

_Brownfield JUCE 8 plugin — extends existing descriptor-driven GUI and partial Core MIDI stack. Builds collaboratively; steps 5–8 (patterns, structure detail, validation, handoff) pending._

---

## Project Context Analysis

### Requirements Overview

**Functional requirements:** PRD v1.0 defines **FR-1–FR-60** across device connection, dual-role MIDI/audio, PATCH/MASTER/Matrix Mod editing, Patch Manager (bank, internal, computer, **Mutator**), module I/C/P, header/settings, and action dispatch. Highest architectural density: **unified outbound MIDI queue** (FR-7), **descriptor-driven PatchModel** (FR-49), **ActionDispatcher hub** (FR-47), **two-level Mutator history** (FR-54–FR-60).

**Non-functional requirements:** SysEx reliability and EPROM-adaptive delay (NFR-2), thread safety (NFR-3), descriptor single source of truth (NFR-4), Clean Code limits (NFR-5), CI test pyramid (NFR-1). Primary acceptance gate: **4 h hardware session** without synth hang (SM-1).

**Scale & complexity:**

- Primary domain: **Desktop audio plugin** (AU/VST3/Standalone), real-time MIDI + low-latency audio passthrough
- Complexity level: **High** for a single-instrument editor (brownfield + 60 FRs + hardware coupling)
- Estimated new Core components: **~12 services** (see § Core Component Map)

### Technical Constraints & Dependencies

| Constraint | Source |
|---|---|
| JUCE 8.0.12, C++17, CMake → `Builds/` | project-context |
| GUI → Core dependency direction (strict) | project-context, PRD NFR-4 |
| Descriptor-driven APVTS; no parallel SysEx offset tables | D-057, FR-49 |
| Brownfield: extend `Source/`, no tree migration until P-001 | D-001, project-context |
| Matrix-1000 SysEx baseline 10 ms; M-6/6R 20 ms | addendum, Oberheim refs |
| Patch name 8 chars; Mutator names `Mxx` / `Mxx-Ryy` | FR-55, FR-56 |

### Cross-Cutting Concerns

1. **Thread boundaries** — audio / MIDI / message threads
2. **APVTS as hub** — parameters, properties, ActionDispatcher triggers
3. **SysEx rate limiting** — unified queue, profile from Device Inquiry
4. **Exception → footer** — `ExceptionPropagator` + `uiMessageText`
5. **UI scale layout** — no global AffineTransform; ScaledLayout helpers

---

## Foundation: Brownfield Starter (Step 3)

**Decision:** No greenfield starter. **Extend baseline tag `v0.0.66-alpha-pre-bmad`.**

**Keep (wire / complete):**

| Area | Existing assets |
|---|---|
| Descriptors | `PluginDescriptors*`, `PluginIDs`, `PluginDisplayNames`, `PluginDesignDimensions` |
| Factories | `ApvtsFactory`, `ApvtsLayoutBuilder`, `WidgetFactory` |
| Validators | `ApvtsValidator`, `WidgetFactoryValidator` |
| SysEx stack | `SysExParser`, `SysExEncoder`, `SysExDecoder`, `SysExConstants` |
| MIDI transport | `MidiInputPort`, `MidiOutputPort`, `MidiSender`, `MidiReceiver`, `MidiManager` (partial) |
| GUI panels | All Patch Manager panels including `PatchMutatorPanel` (property stubs) |
| Loggers | `ApvtsLogger`, `MidiLogger` |

**Build new (PRD gaps):** see § Core Component Map.

**Refactor (not rewrite):**

- `MidiManager` — add `MidiOutboundQueue`, rename `requestMasterData` → `requestMaster`, wire `MidiReceiver` → `handleIncomingSysEx`
- `PluginProcessor` — instrument category, `processBlock` SRP split, owns services
- `PatchEditDisplaysPanel` — remove Display→Slider bridge (D-018)

---

## Core Architectural Decisions (Step 4)

### AD-1 — Layered module structure (unchanged)

```
GUI/Panels/Widgets  →  PluginProcessor + ActionDispatcher  →  Core/Services  →  Shared/Definitions
                              ↓
                         MidiManager (MIDI thread)
```

Core services **must not** include GUI headers. GUI reads state via APVTS properties (`uiMirror` class per D-031).

### AD-2 — PluginProcessor as composition root

`PluginProcessor` owns (unique_ptr):

| Service | Responsibility |
|---|---|
| `PatchModel` / `MasterModel` | Packed 134/172 B buffers; patch name bytes 0–7 |
| `ApvtsPatchMapper` / `ApvtsMasterMapper` | Descriptor-driven APVTS ↔ buffer |
| `MidiManager` | Ports, Device Inquiry, queue consumer, sync RPC |
| `MidiOutboundQueue` | Instrument + Editor producers; priority + delay profile |
| `InstrumentMidiForwarder` | `processBlock` note/CC/bend → queue |
| `AudioPassthroughProcessor` | Input gain + peak metering hook |
| `ActionDispatcher` | Routes StandaloneWidget timestamps → handlers |
| `PatchFileService` | Computer Patches scan/load/save/export |
| `InitTemplateLoader` | `PatchInit.syx` / `MasterInit.syx` |
| `InitDefaults` | Hardcoded fallback buffers |
| `ClipboardService` | Type-aware module/patch clipboard |
| `PatchMutatorEngine` | Mutation algo + history store |
| `DirtyPatchTracker` | FR-51 unsaved snapshot compare |
| `DeviceTypeRegistry` | Maps Inquiry member bytes → `deviceType`; bus layout |

Construction order follows D-058 critical path; handlers receive references, not ownership cycles.

### AD-3 — MIDI unified outbound queue

**Producers:**

- `InstrumentMidiForwarder` — Note On/Off, CC, Pitch Bend only
- `EditorPath` — SysEx (0x01 patch, 0x03 master, 0x06 param, bus block), Program Change

**Consumer:** `MidiManager::run()` dequeues → `MidiSender`.

**Rules (FR-7, addendum):**

- Dequeue realtime before SysEx
- Inter-SysEx delay from `SysExDelayProfile` (stock vs optimised EPROM — Device Inquiry string match TBD in impl)
- Never call `MidiOutput` from audio thread

### AD-4 — APVTS change → SysEx routing

| Change source | Outbound pattern |
|---|---|
| PATCH `AudioParameter` | Remote Parameter Edit 0x06 (per descriptor) |
| MASTER `AudioParameter` | Full master 0x03 |
| Matrix Mod bus param | SOURCE+AMOUNT+DESTINATION block (opcode TBD — open question §9) |
| Full buffer replace | Patch 0x01 (STORE, file load, Mutator, Internal COPY load) |

`PluginProcessor` (or thin `SysExDispatchService`) listens to APVTS parameter changes — **not** GUI panels.

### AD-5 — ActionDispatcher handler split

| Handler | Actions |
|---|---|
| `ModuleActionHandler` | Module I/C/P, Matrix Mod section/bus init |
| `PatchManagerActionHandler` | Bank, Internal, Computer Patches |
| `MutatorActionHandler` | MUTATE, RETRY, DELETE, CLEAR, EXPORT, COMPARE toggle, Defrag (Settings) |

Panels: `setProperty(id, timestamp)` only. Debounce for History combobox selection lives in handler or dedicated `HistorySelectionDebouncer`.

### AD-6 — PatchMutatorEngine internal design

```
PatchMutatorEngine
├── MutationHistoryStore      // roots M00–M99 + per-root retries R00–R99
│   └── MutationEntry         // result buffer, parentSnapshot, indices
├── MutationAlgorithm         // Amount/Random + module toggles (open question §9)
├── MutationNaming            // format Mxx, Mxx-Ryy; max+1 with gaps
└── HistoryDefragService      // FR-59 contiguous renumber
```

**History UI binding:** APVTS properties mirror `historyMList`, `historyRList`, `selectedM`, `selectedR`, `compareActive` for GUI combos (exact property IDs in Architecture pass 2).

**EXPORT:** `PatchFileService::exportMutatorHistory(folder, store)` — layout per FR-33.

### AD-7 — Persistence boundaries

| Data | Persisted |
|---|---|
| Plugin prefs (ports, skin, scale, folders, Mutator recipe, policies) | Session XML |
| Patch parameters (when device connected) | **Not** restored — synth is source of truth |
| Mutator history | Session RAM only — cleared on patch load |
| Initial snapshot (COMPARE) | RAM only; `Initial.syx` on EXPORT |

### AD-8 — Testing strategy (Architecture slice)

Unit tests under future `tests/` (or `Quality/Tests/` until P-001):

- `PatchModel` / mapper round-trip vs Oberheim table samples
- `MidiOutboundQueue` ordering + delay mock
- `MutationHistoryStore` MUTATE/RETRY/DELETE/Defrag scenarios
- `ClipboardService` compatibility matrix
- `ActionDispatcher` routing mocks

No hardware in CI; SM-1 remains manual.

---

## FR → Component Mapping (initial)

| PRD section | Primary Core owner | GUI touchpoints |
|---|---|---|
| §4.1 Device (FR-1–3) | `MidiManager`, `DeviceTypeRegistry` | Header ports, footer identity |
| §4.2 Dual-role (FR-4–9) | `InstrumentMidiForwarder`, `AudioPassthroughProcessor`, queue | Header routing, LEDs, peak |
| §4.3 PATCH (FR-10–13) | Mappers + SysEx dispatch | PatchEditPanel, displays |
| §4.4 Matrix Mod (FR-14–15, 50) | SysEx dispatch + bus reorder service | MatrixModPanel |
| §4.5 MASTER (FR-16–18) | MasterModel + dispatch | MasterEditPanel, Settings |
| §4.6–4.8 Patch Manager (FR-19–29, 51–52) | `PatchFileService`, `DirtyPatchTracker`, handlers | PM panels |
| §4.9 Mutator (FR-30–34, 54–60) | `PatchMutatorEngine` | PatchMutatorPanel |
| §4.10 I/C/P (FR-35–38) | `ClipboardService`, Init loaders | Module headers |
| §4.11–13 Shell (FR-39–49) | Settings, `ActionDispatcher`, models | Header, Footer, Settings |

---

## Implementation Epics (aligned D-058)

| Epic | Scope | Unblocks |
|---|---|---|
| **E1** | PatchModel + MasterModel + mappers | All SysEx serialization |
| **E2** | MidiOutboundQueue + APVTS→SysEx wiring + delay profiles | Real editing |
| **E3** | InitDefaults + InitTemplateLoader | I/C/P init paths |
| **E4** | PatchFileService + Computer Patches | FR-25–29, 52, export |
| **E5** | ClipboardService | FR-35 |
| **E6** | PatchMutatorEngine + History UI properties | FR-54–60 |
| **E7** | ActionDispatcher + handlers (rewire panels) | All StandaloneWidgets |
| **E8** | DeviceTypeRegistry + audio bus layout | FR-1, FR-4 |
| **E9** | DirtyPatchTracker + FR-51 modal | Internal/Computer nav |
| **E10** | Display↔APVTS sync refactor | FR-11–12 |

Epics E1–E2 are **sequential gate**; E7 can start in parallel once handler interfaces are frozen.

---

## Open Architecture Items (next sessions)

1. **MutationAlgorithm** — Amount/Random curves, per-parameter jitter rules, module toggle mask
2. **SysExDelayProfile** — string matching rules for Tauntek/Gligli/Nordcore vs stock
3. **Matrix Mod bus opcode** — 0x0B vs 0x06 block (PRD open question #3)
4. **APVTS property catalog** — full list for Patch Manager + Mutator mirrors
5. **P-001 tree migration** — defer until architecture v1.0 stable

---

## Implementation Patterns & Consistency Rules (Step 5)

Patterns below prevent divergent choices across AI agents and human contributors. **Authoritative detail:** `.cursorrules` + `project-context.md` — this section captures architecture-specific enforcement.

### Critical Conflict Points

| Area | Risk if unspecified |
|---|---|
| SysEx offset source | Duplicate offset tables beside descriptors |
| MIDI send path | Direct `MidiOutput` from audio thread or GUI |
| GUI business logic | SysEx or patch logic inside panels |
| APVTS property semantics | Mixing event timestamps with persistent state |
| New file placement | Services scattered outside `Source/Core/` |
| Mutator history model | Flat list vs two-level M/R store |

### Naming Patterns

**C++ (mandatory — `.cursorrules`):**

| Element | Pattern | Example |
|---|---|---|
| Class | PascalCase | `PatchMutatorEngine`, `MidiOutboundQueue` |
| Method | lowerCamelCase | `sendPatch`, `defragHistory` |
| Private member | `name_` | `historyStore_`, `apvts_` |
| Public constant | `kName` | `kMaxRootMutations` |
| Enum class / values | `PascalCase` / `kValue` | `DeviceType::kMatrix1000` |
| Namespace | PascalCase | `Core`, `GUI` |
| File pair | `ClassName.h` / `.cpp` | `PatchModel.h` |

**New Core service files** live under `Source/Core/` in domain subfolders:

- `Source/Core/Models/` — `PatchModel`, `MasterModel`, mappers
- `Source/Core/MIDI/` — existing + `Queue/MidiOutboundQueue`
- `Source/Core/Services/` — `PatchFileService`, `ClipboardService`, `PatchMutatorEngine`, etc.
- `Source/Core/Actions/` — `ActionDispatcher`, `*ActionHandler`
- `Source/Core/Init/` — `InitDefaults`, `InitTemplateLoader`

One primary class per `.h/.cpp` pair; no `Utils.cpp` junk drawers.

**APVTS identifiers** — register **only** in `PluginIDs.h` / layout builders; never string literals in panels.

### Structure Patterns

**Dependency rule (hard):**

```
GUI → Core → Shared/Definitions
Core ↛ GUI
```

**Panel rule:** Panels compose widgets and read/write APVTS. **No** `MidiManager`, **no** `PatchModel` direct access except via processor reference for narrow observers — prefer properties/parameters.

**Processor rule:** `PluginProcessor` constructs services, registers APVTS listeners, forwards `processBlock` to dedicated classes (< 15 lines body).

**Tests:** `Quality/Tests/` until P-001 → `tests/`; mirror Core folder structure; Catch2 or existing framework; file name `ClassNameTest.cpp`.

### Format Patterns

**Packed buffers:**

- Patch = **134** bytes; Master (M-1000) = **172** bytes
- Patch name = bytes **0–7**, 6-bit ASCII via model helpers
- All pack/unpack through **descriptors** (`sysExId`, `sysExOffset`) — never hardcode byte indices in services

**Mutator names:**

- Root display/file: `M00`–`M99` (two digits)
- Retry: `M05-R02` (7 chars max)
- Export paths: `ExportFolder/M05/M05-R02.syx`

**SysEx messages:** Build only via `SysExEncoder`; parse only via `SysExParser` / `SysExDecoder`.

### Communication Patterns

**APVTS property taxonomy (D-031):**

| Class | Pattern | Persist? | Example |
|---|---|---|---|
| **event** | `setProperty(id, Time::getCurrentTimeMillis())` | No | Button clicks |
| **state** | `setProperty(id, value)` | Yes | Bank index, Mutator toggles |
| **uiMirror** | Core writes, GUI reads | Optional | Paste enabled flags |

GUI **never** calls handler methods directly — timestamp change on property → `PluginProcessor` listener → `ActionDispatcher`.

**Parameter changes:** `AudioParameter` listeners in processor/services → enqueue SysEx (never synchronous hardware wait on message thread except documented sync RPC: Device Inquiry, explicit patch request).

**Core → GUI feedback:** Update APVTS properties or parameters; GUI uses attachments/listeners. **No** direct widget pointers from Core.

**Mutator History UI:** Core owns `MutationHistoryStore`; exposes read-only snapshot or syncs combo item lists via **uiMirror** properties after each MUTATE/RETRY/DELETE/Defrag.

### Process Patterns

**Error handling:**

- Core services: `std::optional`, `Result`-like types, or throw `SysExException` / `MidiTimeoutException`
- Catch at `PluginProcessor` or handler boundary → `ExceptionPropagator` → footer (`uiMessageText`)
- **No silent failures** — log via `MidiLogger` / `ApvtsLogger` when enabled
- User modals **only** where PRD requires: FR-51 unsaved warning, FR-59 Defrag confirm, MASTER init confirm

**Sync RPC (blocking OK):** Device Inquiry, `requestCurrentPatch`, `requestMaster` — MIDI thread only, never audio.

**Async outbound:** All fire-and-forget SysEx and notes → `MidiOutboundQueue`.

**Loading / busy states:** Use APVTS `uiMirror` or footer info — no blocking modal spinners on MIDI path.

**Debouncing:** Combobox-driven patch send (Computer Patches, Mutator History) — shared debounce helper in Core, single constant named `kComboboxPatchSendDebounceMs`.

### Enforcement Guidelines

**All agents MUST:**

1. Read descriptor before touching a synth parameter byte or APVTS id
2. Route outbound MIDI through `MidiOutboundQueue` (no ad-hoc `sendMessageNow` except inside queue consumer)
3. Keep functions ≤ **15 lines**; extract helpers immediately
4. Add unit test for each new Core service public behaviour
5. Update `.decision-log.md` when architecture-affecting choices diverge from this doc

**Verification:** CI unit tests (NFR-1); boot validators on startup; code review checklist against this section.

### Pattern Examples

**Good:**

- `PatchMutatorEngine::mutate()` builds new entry, pushes to store, calls `queueFullPatchSysEx(result)`
- `InternalPatchesPanel` sets `PluginIDs::internalPatchesStore` timestamp; handler performs STORE
- `ApvtsPatchMapper::writeParameter(paramId, value)` uses descriptor offset

**Anti-patterns:**

- `PatchMutatorPanel.cpp` calling `MidiOutput::sendMessageNow`
- Parallel `constexpr int kFilterCutoffOffset = 42` beside descriptor
- 80-line `ActionDispatcher::dispatch()` switch — split per handler
- Storing Mutator history in plugin state XML
- Global `AffineTransform` scale on editor root component

---

## Next Workflow Steps

- **Step 6:** Detailed `Source/Core/` directory tree and file checklist
- **Step 7:** Architecture validation against PRD FR checklist
- **Step 8:** Handoff → `bmad-create-epics-and-stories`

---

*Architecture draft v0.2 — steps 1–5 complete (2026-05-29).*
