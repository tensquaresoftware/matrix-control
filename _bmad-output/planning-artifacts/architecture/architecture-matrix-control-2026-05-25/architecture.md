---
organization: Ten Square Software
project: Matrix-Control
title: Architecture Decision Document
author: BMad Agent
status: complete
version: "1.0"
sources:
  - ../../prds/prd-matrix-control-2026-05-25/prd.md
  - ../../prds/prd-matrix-control-2026-05-25/addendum.md
  - ../../prds/prd-matrix-control-2026-05-25/.decision-log.md
  - ../../project-context.md
  - ../../../reference-docs/oberheim/index.md
created: 2026-05-29
updated: 2026-05-29
stepsCompleted: [1, 2, 3, 4, 5, 6, 7, 8]
lastStep: 8
completedAt: 2026-05-29
inputDocuments:
  - prd-matrix-control-2026-05-25/prd.md
  - prd-matrix-control-2026-05-25/addendum.md
  - prd-matrix-control-2026-05-25/.decision-log.md
  - project-context.md
  - reference-docs/oberheim/index.md
workflowType: architecture
---

# Architecture Decision Document

_Brownfield JUCE 8 plugin — extends existing descriptor-driven GUI and partial Core MIDI stack. Architecture v1.0 complete (steps 1–8)._

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
| `ClipboardService` | Type-aware module/patch/matrix-modulation clipboard |
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
- **Future consideration (R-C):** Direct `sendMessageNow()` from `processBlock()` for notes/CC may improve sample-accurate timing. Requires explicit AD revision and hardware play-testing. v1 retains queue path (R-D — Sprint Change Proposal 2026-06-06).

### AD-11 — Audio bus layout by host context

**Approved:** Sprint Change Proposal 2026-06-06 (supersedes D-055 hosted-plugin input bus rationale).

| Context | Input bus | Output bus | `AudioPassthroughProcessor` |
|---|---|---|---|
| Hosted plugin (VST3/AU) | **None** | Stereo (silent) | No-op; output cleared |
| Standalone | Stereo (optional enable) | Stereo | Passthrough + gain + peak |

Detection: `juce::JUCEApplicationBase::isStandaloneApp()` or equivalent build flag.

Mono vs stereo **input channel selection** (M-1000 vs M-6/6R) applies to **standalone** only (`DeviceTypeRegistry`, Epic 8). Hosted plugin: user monitors synth audio on a **separate DAW audio track** (Option D).

**Standalone:** `StandaloneAudioInputRouter` + `AudioDeviceManager` — unchanged.  
**Hosted:** `BusesProperties` declares output only; header Audio From / Input Gain / peak hidden in plugin mode.

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
| `ModuleActionHandler` | Module I/C/P, Matrix Modulation section I/C/P, per-bus Matrix Mod init |
| `PatchManagerActionHandler` | Bank, Internal, Computer Patches |
| `MutatorActionHandler` | MUTATE, RETRY, DELETE, CLEAR, EXPORT, COMPARE toggle, Defrag (Settings) |

Panels: `setProperty(id, timestamp)` only. Debounce for History combobox selection lives in handler or dedicated `HistorySelectionDebouncer`.

### AD-6 — PatchMutatorEngine internal design

```
PatchMutatorEngine
├── MutationHistoryStore      // roots M00–M99 + per-root retries R00–R99
│   └── MutationEntry         // result buffer, parentSnapshot, indices
├── MutationAlgorithm         // Amount/Random + module toggles — see mutation-algorithm-spec.md
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
| §4.2 Dual-role (FR-4–9) | `InstrumentMidiForwarder`, `AudioPassthroughProcessor` (standalone), queue | Header routing, LEDs; peak standalone-only (AD-11) |
| §4.3 PATCH (FR-10–13) | Mappers + SysEx dispatch | PatchEditPanel, displays |
| §4.4 Matrix Mod (FR-14–15, 50) | SysEx dispatch + bus reorder service + `MatrixModInitService` | MatrixModPanel (`ModulationBusHeader` I/C/P, per-bus init) |
| §4.5 MASTER (FR-16–18) | MasterModel + dispatch | MasterEditPanel, Settings |
| §4.6–4.8 Patch Manager (FR-19–29, 51–52) | `PatchFileService`, `DirtyPatchTracker`, handlers | PM panels |
| §4.9 Mutator (FR-30–34, 54–60) | `PatchMutatorEngine` | PatchMutatorPanel |
| §4.10 I/C/P (FR-35–38) | `ClipboardService`, Init loaders | Module headers (PATCH/MASTER); Matrix Modulation section header I/C/P |
| §4.11–13 Shell (FR-39–49) | Settings, `ActionDispatcher`, models | Header, Footer, Settings |

---

## Implementation Epics (aligned D-058)

| Epic | Scope | Unblocks |
|---|---|---|
| **E0** | **P-001** tree migration (`Source→src`, `builds/`, `tests/`, `docs/`, `_local/`) | Clean target paths for all new code |
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

1. ~~**MutationAlgorithm**~~ — **approved** in [`mutation-algorithm-spec.md`](mutation-algorithm-spec.md) v1.0 (Story 6.1, sign-off 2026-06-19, Option A)
2. **SysExDelayProfile** — string matching rules for Tauntek/Gligli/Nordcore vs stock
3. **Matrix Mod bus opcode** — 0x0B vs 0x06 block (PRD open question #3)
4. **APVTS property catalog** — full list for Patch Manager + Mutator mirrors (E6/E7 stories)
5. ~~**P-001 tree migration**~~ — **documented** AD-9/AD-10; execute Epic E0

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

## Project Structure & Boundaries (Step 6)

### AD-9 — P-001 tree migration: document vs execute

| Phase | What | When |
|---|---|---|
| **Step 6 (this document)** | Freeze **target tree**, rename map, migration checklist | Now |
| **Epic E0 — P-001 execution** | Physical `git mv`, CMake, `.gitignore`, `.cursorrules`, CI, docs links | **Before Epic E1** (recommended) |
| **Epics E1–E10** | New Core files land in **target** paths under `src/` | After E0 |

**Step 6 does not rename folders in the repo.** That is a dedicated migration PR (or series) so feature work is not mixed with mass path churn.

**Include-path rule (migration):** CMake `target_include_directories` changes from `Source/` → `src/`. Internal folders **`Core/`**, **`GUI/`**, **`Shared/`** stay **PascalCase** — existing includes like `"Core/MIDI/MidiManager.h"` remain valid; only the root prefix changes.

### AD-10 — Project root paths (runtime, not compile-time)

**Problem (brownfield):** `ApvtsLogger` and `MidiLogger` resolve default log directories via `MATRIX_CONTROL_PROJECT_ROOT`, defined in CMake as `"${CMAKE_CURRENT_SOURCE_DIR}"` — an **absolute path baked in at configure/compile time**. Moving the repo (e.g. to another disk) leaves logs writing to the **old** location until a full reconfigure/rebuild.

**Decision:** All project-owned paths (logs first; same pattern for future workbench-relative dev assets) must resolve **at runtime** relative to the **Matrix-Control repository root**, not a compile-time string.

| Rule | Detail |
|---|---|
| **SSOT** | New `ProjectPaths` utility under `src/Shared/` (or `src/Core/` if Shared stays definitions-only) |
| **Discovery** | Walk up from `currentExecutableFile` (and optionally CWD) until a directory contains `CMakeLists.txt` whose `project()` name is `Matrix-Control` (or a root marker file `.matrix-control-root`) |
| **Override** | Optional env var `MATRIX_CONTROL_PROJECT_ROOT` for dev/CI only — never required for normal use |
| **Log layout** | `{projectRoot}/logs/midi/`, `{projectRoot}/logs/apvts/` (lowercase subfolders; ex-`Logs/MIDI`, `Logs/APVTS`) |
| **Remove** | CMake `target_compile_definitions(… MATRIX_CONTROL_PROJECT_ROOT=…)` — loggers call `ProjectPaths::getLogsDirectory(…)` instead |
| **When** | **Epic E0** (same PR series as P-001 renames) — mandatory before relying on moved tree |

**Plugin-in-DAW note:** If discovery fails (no repo root found), file logging falls back to a documented user-data location or disables file output with a one-line `MidiLogger`/`ApvtsLogger` warning — exact fallback framed in E0 story (D-041 release opt-in still applies).

**Scope:** E0 fixes loggers; audit other code for hard-coded or CMake-baked absolute paths as part of the same epic.

### Root directory rename map (P-001)

| Current (baseline) | Target | Git | Notes |
|---|---|---|---|
| `Source/` | `src/` | versioned | Application code |
| `Assets/` | `assets/` | versioned | Fonts, README images |
| `Logs/` | `logs/` | **gitignore** | Runtime logs (`logs/midi/`, `logs/apvts/`) — see AD-10 |
| `Documentation/` | `docs/` | versioned | Public `.md`, kebab-case filenames |
| `Quality/Tests/` | `tests/` | versioned | CI unit tests only |
| `Quality/` (non-test) | `_local/quality/` | **gitignore** | Personal / scratch |
| `Workbench/` | `_local/workbench/` | **gitignore** | Syx samples, Live projects |
| `Builds/` | `builds/` | **gitignore** | `builds/macos/`, `builds/windows/`, `builds/linux/` |
| `_bmad/`, `_bmad-output/` | unchanged | versioned | BMad tooling + artifacts |
| `_local/` | unchanged | **gitignore** | Personal workspace root |

### Target repository tree (post P-001)

```
Matrix-Control/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── .cursorrules
├── assets/
│   └── fonts/ …
├── logs/                            # gitignored — runtime (midi/, apvts/)
├── docs/
│   ├── README.md
│   └── development/
│       ├── architecture/          # optional mirror of key public arch notes
│       └── gui/                   # ex-Documentation/Development/GUI/
├── tests/
│   ├── CMakeLists.txt
│   └── core/
│       ├── models/
│       ├── midi/
│       ├── services/
│       └── actions/
├── builds/                          # gitignored
│   ├── macos/
│   ├── windows/
│   └── linux/
├── src/
│   ├── Core/
│   │   ├── PluginProcessor.h/.cpp
│   │   ├── Factories/               # ApvtsFactory, LayoutBuilder, Validator
│   │   ├── Loggers/
│   │   ├── Models/                  # [NEW E1] PatchModel, MasterModel, mappers
│   │   ├── Init/                    # [NEW E3] InitDefaults, InitTemplateLoader
│   │   ├── Actions/                 # [NEW E7] ActionDispatcher, *Handler
│   │   ├── Services/                # [NEW] see service map below
│   │   ├── Audio/                   # [NEW E2] InstrumentMidiForwarder, AudioPassthroughProcessor
│   │   └── MIDI/
│   │       ├── Ports/
│   │       ├── Transport/
│   │       ├── SysEx/
│   │       └── Queue/               # [NEW E2] MidiOutboundQueue, SysExDelayProfile
│   ├── GUI/                         # existing hierarchy preserved
│   │   ├── PluginEditor.*
│   │   ├── MainComponent.*
│   │   ├── Factories/
│   │   ├── Widgets/
│   │   ├── Panels/
│   │   ├── Looks/
│   │   ├── Skins/
│   │   ├── Layout/
│   │   └── Tests/                   # TestComponent — JUCE_DEBUG only
│   └── Shared/
│       └── Definitions/             # PluginIDs, Descriptors, DisplayNames, DesignDimensions
├── _bmad/
├── _bmad-output/
│   ├── planning-artifacts/
│   └── reference-docs/
└── _local/                          # gitignored
    ├── quality/
    └── workbench/
```

### New Core services — file placement (target paths)

| Service | Path under `src/Core/` |
|---|---|
| `PatchModel`, `MasterModel` | `Models/PatchModel.h`, `MasterModel.h`, `ApvtsPatchMapper.h`, … |
| `MidiOutboundQueue`, `SysExDelayProfile` | `MIDI/Queue/` |
| `InstrumentMidiForwarder`, `AudioPassthroughProcessor` | `Audio/` |
| `SysExDispatchService` (optional thin router) | `MIDI/` or `Services/` |
| `ActionDispatcher`, handlers | `Actions/` |
| `PatchFileService` | `Services/PatchFileService.h` |
| `ClipboardService` | `Services/ClipboardService.h` |
| `PatchMutatorEngine`, `MutationHistoryStore`, `HistoryDefragService` | `Services/PatchMutator/` (subfolder) |
| `DirtyPatchTracker` | `Services/DirtyPatchTracker.h` |
| `DeviceTypeRegistry` | `Services/DeviceTypeRegistry.h` or `MIDI/` |

### Architectural boundaries (integration)

```
┌─────────────────────────────────────────────────────────┐
│  src/GUI/          panels, widgets, attachments         │
│       │ setProperty / parameters only                   │
└───────┼─────────────────────────────────────────────────┘
        ▼
┌─────────────────────────────────────────────────────────┐
│  src/Core/PluginProcessor + Actions/ActionDispatcher    │
└───────┼─────────────────────────────────────────────────┘
        ▼
┌─────────────────────────────────────────────────────────┐
│  src/Core/Services/, Models/, MIDI/Queue/               │
└───────┼─────────────────────────────────────────────────┘
        ▼
┌─────────────────────────────────────────────────────────┐
│  src/Shared/Definitions/  (descriptors — read-only SSOT)  │
└─────────────────────────────────────────────────────────┘
```

**External integration:** Oberheim hardware via `MidiManager` → OS MIDI drivers only. No network APIs v1.

### Epic → directory mapping

| Epic | Primary locations |
|---|---|
| **E0 P-001** | Root renames, `ProjectPaths` + logger path fix (AD-10), `CMakeLists.txt`, `.gitignore`, `project-context`, `.cursorrules` |
| **E1** | `src/Core/Models/` |
| **E2** | `src/Core/MIDI/Queue/`, `Audio/`, processor SysEx listeners |
| **E3** | `src/Core/Init/` |
| **E4** | `src/Core/Services/PatchFileService.*` |
| **E5** | `src/Core/Services/ClipboardService.*` |
| **E6** | `src/Core/Services/PatchMutator/` |
| **E7** | `src/Core/Actions/` + panel rewire only |
| **E8** | `src/Core/Services/DeviceTypeRegistry.*`, processor bus layout |
| **E9** | `src/Core/Services/DirtyPatchTracker.*` |
| **E10** | `src/GUI/.../PatchEditDisplaysPanel/`, remove `InteractiveDisplayApvtsSync` bridge |

### P-001 migration checklist (Epic E0)

Execute in order; one thematic commit per bullet recommended:

1. Create `builds/` layout in `.gitignore`; deprecate `Builds/` entry; rename `Logs/` → `logs/` in `.gitignore`
2. `git mv Source src`, `Assets assets` (if tracked)
3. Move public docs to `docs/` with kebab-case renames (manifest of path map)
4. Move `Quality/Tests` → `tests/`; relocate personal `Quality/` → `_local/quality/`
5. **AD-10:** Add `ProjectPaths`; refactor `ApvtsLogger` / `MidiLogger` to runtime `{root}/logs/…`; remove `MATRIX_CONTROL_PROJECT_ROOT` compile define
6. Update `CMakeLists.txt`: all `Source/` → `src/`, output dir `builds/macos/`, add `tests/` subdirectory
7. Update `.cursorrules`, `project-context.md`, README paths
8. Update `.clangd` / compile_commands path if needed
9. Full build macOS + run boot validators (`ApvtsValidator`, `WidgetFactoryValidator`); verify logs land under **new** repo path after move simulation
10. Record **P-001 DONE** in decision log; bump `project-context` version

**Do not** mix E0 with E1 new feature files.

### Requirements → structure (FR categories)

| FR block | `src/` home |
|---|---|
| Device, dual-role, queue | `Core/MIDI/`, `Core/Audio/`, `PluginProcessor` |
| PATCH / MASTER / Matrix Mod | `Core/Models/`, `Core/MIDI/SysEx/`, `GUI/Panels/...` |
| Patch Manager | `Core/Services/`, `Core/Actions/`, `GUI/.../PatchManagerPanel/` |
| Mutator | `Core/Services/PatchMutator/`, `PatchMutatorPanel` |
| Settings / Defrag | `GUI/Settings/` (or panel TBD), `HistoryDefragService` |

---

## Architecture Validation Results (Step 7)

### Coherence Validation ✅

**Decision Compatibility:** AD-1–AD-11 align with brownfield baseline (JUCE 8.0.12, C++17, CMake). Composition root (AD-2), unified queue (AD-3), and APVTS routing (AD-4) are mutually consistent — no direct MIDI from audio thread, no GUI→SysEx shortcuts. P-001 target tree (AD-9) matches implementation patterns (Step 5) and epic order (D-058). AD-10 (`ProjectPaths`) resolves the logger absolute-path conflict without contradicting D-094 (logs gitignored).

**Pattern Consistency:** Naming, folder layout, APVTS taxonomy, and anti-patterns in Step 5 reinforce AD-1–AD-8. Epic E0 checklist references the same paths as the rename map. Mutator two-level store (AD-6) matches FR-54–FR-60 and D-082–D-087.

**Structure Alignment:** FR→component map, epic→directory map, and service placement table cover all PRD feature areas. Boundaries GUI→Core→Shared are explicit. E0 before E1 prevents new files landing in obsolete `Source/` paths.

### Requirements Coverage Validation ✅

**Epic Coverage:** E0–E10 each map to concrete `src/` locations. D-058 critical path (E1→E2→…) preserved. E0 unblocks all subsequent epics.

**Functional Requirements Coverage:** FR-1–FR-60 mapped in § FR → Component Mapping and Step 6 structure table. No orphan PRD section. Gaps are **algorithm/opcode detail**, not missing owners:

| Deferred detail | PRD ref | Architecture owner |
|---|---|---|
| Mutation Amount/Random rules | FR-30, §9 #7 | [`MutationAlgorithm`](mutation-algorithm-spec.md) — E6 Story 6.1 spec; E6 Story 6.4 impl |
| Matrix Mod bus opcode | §9 #3 | SysEx dispatch — E2 spike on hardware |
| SysEx automation throttle curves | §9 #2 | `SysExDelayProfile` — E2 + SM-1 |
| INIT hardcoded defaults | §9 #4 | `InitDefaults` — E3 (non-blocking) |
| M-6 Device ID member bytes | §9 #6 | `DeviceTypeRegistry` — E8 + hardware |

**Non-Functional Requirements Coverage:**

| NFR | Architectural support |
|---|---|
| NFR-1 CI tests | AD-8, `tests/` layout, E1–E7 test targets |
| NFR-2 SysEx reliability | AD-3 queue + delay profile; SM-1 gate |
| NFR-3 Thread safety | AD-3, patterns § MIDI queue-only outbound |
| NFR-4 Descriptor SSOT | AD-4, anti-patterns, FR-49 |
| NFR-5 Clean Code limits | project-context + Step 5 reference |
| NFR-6 Documentation | `docs/` target, Oberheim refs in `_bmad-output` |
| NFR-7 Release logging | D-041, D-094; Settings opt-in framed in E0/E7 |
| NFR-8 Platforms | Brownfield CMake; builds/{macos,windows,linux} |

### Implementation Readiness Validation ✅

**Decision Completeness:** AD-1–AD-10 documented. Technology stack and versions stated in context + AD-8. Seven PRD §9 items explicitly routed to epics — acceptable for v1.0 architecture (not blocking epics/stories).

**Structure Completeness:** Target tree, rename map, E0 checklist (10 steps), service paths, and integration diagram are specific enough for agent implementation.

**Pattern Completeness:** Conflict points, naming, APVTS classes, error→footer, Mutator naming, and anti-patterns cover primary agent divergence risks. APVTS property ID catalog remains a **story-level** deliverable (Important gap).

### Gap Analysis Results

**Critical Gaps:** None — no missing architectural owner for any FR block; open items are implementation spikes or owner input, not structural holes.

**Important Gaps:**

1. **APVTS property catalog** — Mutator/PM mirror property IDs not enumerated (E6/E7 stories)
2. ~~**MutationAlgorithm**~~ — approved [`mutation-algorithm-spec.md`](mutation-algorithm-spec.md) v1.0 (Story 6.1); implement in E6 Story 6.4
3. **SysExDelayProfile matching rules** — EPROM string detection (E2)
4. **Matrix Mod opcode** — confirm on hardware before E2 locks encoder (PRD §9 #3)

**Nice-to-Have Gaps:**

- Optional `docs/development/architecture/` public mirror of key ADs
- DAW-specific automation throttle tuning notes post-SM-1
- ENV shape clipboard scope (PRD §9 #5) — v1.x

### Validation Issues Addressed

- **Logger absolute paths (D-093):** AD-10 + E0 checklist step 5
- **Logs in git (D-094):** Confirmed gitignore; documented in P-001 rename map
- **P-001 timing:** Documented now, execute E0 before E1 (was listed as “defer” — updated in § Open Architecture Items)

### Architecture Completeness Checklist

**Requirements Analysis**

- [x] Project context thoroughly analyzed
- [x] Scale and complexity assessed
- [x] Technical constraints identified
- [x] Cross-cutting concerns mapped

**Architectural Decisions**

- [x] Critical decisions documented with versions
- [x] Technology stack fully specified
- [x] Integration patterns defined
- [x] Performance considerations addressed (queue, thread boundaries, no audio alloc)

**Implementation Patterns**

- [x] Naming conventions established
- [x] Structure patterns defined
- [x] Communication patterns specified
- [x] Process patterns documented

**Project Structure**

- [x] Complete directory structure defined
- [x] Component boundaries established
- [x] Integration points mapped
- [x] Requirements to structure mapping complete

### Architecture Readiness Assessment

**Overall Status:** **READY WITH MINOR GAPS**

**Confidence Level:** **High** for epic sequencing and Core/GUI boundaries; **medium** on Mutator algorithm and Matrix Mod opcode until E2/E6 hardware/spike stories close PRD §9 items.

**Key Strengths:**

- Brownfield inventory explicit — no false greenfield
- Unified MIDI queue + ActionDispatcher hub prevent the two highest-risk integration failures
- Mutator history model fully specified at architecture level (M/R, EXPORT, DEFRAG)
- P-001 + AD-10 + D-094 give E0 a executable checklist

**Areas for Future Enhancement:**

- APVTS property catalog as living appendix
- MutationAlgorithm tuning after user testing
- Public architecture mirror under `docs/` (optional)

### Implementation Handoff

**AI Agent Guidelines:**

- Follow AD-1–AD-10 and Step 5 patterns exactly
- Execute **Epic E0** before creating new Core files under target paths
- Never bypass `MidiOutboundQueue` or `ActionDispatcher`
- Refer to PRD v1.0 + this document + `project-context.md` for conflicts

**First Implementation Priority:** **Epic E0 — P-001 migration** (tree renames, `ProjectPaths`, logger fix, CMake/`tests/`/`builds/`), then **Epic E1 — PatchModel**.

**Next workflow:** `bmad-create-epics-and-stories` (E0–E10).

---

## Workflow Completion (Step 8)

Architecture workflow **complete** — 2026-05-29.

| Deliverable | Location |
|---|---|
| Architecture v1.0 | This document |
| Decision log | `prds/.../.decision-log.md` (D-088–D-094) |
| Agent rules | `_bmad-output/project-context.md` |

**Recommended next steps for Guillaume:**

1. **`bmad-create-epics-and-stories`** — shard E0–E10 into implementable stories
2. **`bmad-sprint-planning`** — after stories exist
3. **Epic E0 execution** — first code PR (migration + `ProjectPaths`)
4. **`bmad-ux`** — parallel if UI specs needed before E7 panel rewire

---

*Architecture v1.0 — workflow complete (2026-05-29).*
