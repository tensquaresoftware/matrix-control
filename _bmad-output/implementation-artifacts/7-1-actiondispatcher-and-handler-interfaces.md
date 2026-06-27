---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.1 — ActionDispatcher and Handler Interfaces
author: BMad Agent
status: review
baseline_commit: aa72810660c0561cdbfc65ed3ba8cbc06c9ba22f
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/sprint-change-proposal-2026-06-18.md
  - implementation-artifacts/5-2-module-copy-paste-enable-and-gray-rules.md
  - implementation-artifacts/5-3-matrix-modulation-section-i-c-p-gui.md
  - implementation-artifacts/7-7-settings-page-consolidation.md
  - project-context.md
created: 2026-06-18
updated: 2026-06-18
---

# Story 7.1: ActionDispatcher and Handler Interfaces

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a developer,
I want a central `ActionDispatcher` with three specialized handlers,
so that GUI panels only call `setProperty` with timestamps and Core owns all action routing (FR-47, AD-5, D-031, D-032).

## Acceptance Criteria

1. **Given** Epics 2–6 Core services exist (`ClipboardService`, `InitTemplateLoader`, `MatrixModInitService`, `MasterModuleInitService`, `MatrixModBusReorderService`, etc.) **When** a StandaloneWidget action property changes on the message thread **Then** `PluginProcessor::valueTreePropertyChanged` delegates **only** registered action-event IDs to `ActionDispatcher::onActionPropertyChanged(propertyId, newValue)` — parameter changes, state properties, and `*PasteEnabled` uiMirror properties continue to flow through existing processor logic unchanged.

2. **And** `ActionDispatcher` routes each registered action ID to exactly one handler:
   - `ModuleActionHandler` — PATCH/MASTER module I/C/P/Init, Matrix Modulation section I/C/P/Init, per-bus Matrix Mod init
   - `PatchManagerActionHandler` — Bank Utility, Internal Patches navigation/actions, Computer Patches actions (stubs OK where Epic 4 not done)
   - `MutatorActionHandler` — all `patchMutator*` StandaloneWidget IDs (stub/no-op body until Story 7.4 / Epic 6)

3. **And** existing brownfield behaviour is preserved by **moving** (not reimplementing) current `PluginProcessor` action handlers into the appropriate handler classes:
   - `handleClipboardCopyPropertyChange` → `ModuleActionHandler`
   - `handleMatrixModInitPropertyChange` → `ModuleActionHandler`
   - `handleMasterModuleInitPropertyChange` → `ModuleActionHandler`
   - `handlePatchManagerPropertyChange` → `PatchManagerActionHandler`

4. **And** unimplemented actions reached by registered IDs are explicitly no-ops or TODO stubs inside the correct handler (not left in `PluginProcessor`):
   - Module **Paste** (all PATCH modules + Matrix Mod section) — Story **7.2**
   - Patch module **Init** via `InitTemplateLoader` — Story **7.2**
   - Internal Patches **Init/Paste/Store** — Story **7.3**
   - Computer Patches file ops — Story **7.3** + Epic **4**
   - Mutator engine calls — Story **7.4** + Epic **6**

5. **And** `ActionDispatcher` + handlers live under `Source/Core/Actions/` with **no GUI headers**; `PluginProcessor` owns `std::unique_ptr<Core::ActionDispatcher>` and constructs handlers with injected Core service references (AD-2 composition root).

6. **And** routing table is data-driven (e.g. `ActionPropertyRegistry` or equivalent `unordered_map<juce::String, HandlerKind>`) — **not** an 80-line `dispatch()` switch (architecture anti-pattern).

7. **And** unit tests in `Tests/Unit/ActionDispatcherTests.cpp` verify routing to the correct handler using test doubles/mocks — **no GUI**, no `PluginProcessor` instantiation required for routing tests.

8. **And** `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds remain green; no regression in Copy, Matrix Mod init, Master module init, or Patch Manager bank/patch navigation.

## Tasks / Subtasks

- [x] **Scaffold `Source/Core/Actions/`** (AC: #2, #5, #6)
  - [x] `ActionHandlerKind` enum: `Module`, `PatchManager`, `Mutator`
  - [x] `ActionPropertyRegistry.{h,cpp}` — build static map from every `PluginIDs::*::StandaloneWidgets::*` action ID → `ActionHandlerKind` (codegen-style constexpr table preferred over runtime string literals scattered in handlers)
  - [x] `ActionDispatcher.{h,cpp}` — `onActionPropertyChanged(id, value)`; lookup registry; delegate to injected handler interface/reference
  - [x] `IActionHandler` base with `handleAction(const juce::String& propertyId, const juce::var& newValue)` (or three dedicated interfaces — pick one pattern, keep testable)

- [x] **Implement handler classes** (AC: #2, #3, #4)
  - [x] `ModuleActionHandler.{h,cpp}` — move bodies from `handleClipboardCopyPropertyChange`, `handleMatrixModInitPropertyChange`, `handleMasterModuleInitPropertyChange`; carry `suppressMatrixModParameterSysEx_` / `suppressMasterParameterSysEx_` coordination via callbacks or small `ActionExecutionContext` struct owned by processor and passed by reference
  - [x] `PatchManagerActionHandler.{h,cpp}` — move `handlePatchManagerPropertyChange` + helpers `parseBankButtonIndex`, `applyPatchCoordinates`, `getResolvedDeviceMemoryLimits` usage (inject `MidiManager*` for Program Change on patch number if still needed from state handler — see Dev Notes)
  - [x] `MutatorActionHandler.{h,cpp}` — stub: empty `handleAction` for all `PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::*` IDs

- [x] **Wire `PluginProcessor`** (AC: #1, #3, #5)
  - [x] Add `actionDispatcher_` member; construct after services in ctor
  - [x] In `valueTreePropertyChanged`, after parameter SysEx / clamping paths, call `actionDispatcher_->onActionPropertyChanged(parameterId, newValue)` **iff** `ActionPropertyRegistry::isActionProperty(parameterId)`
  - [x] Remove migrated `handle*` private methods from processor (or thin wrappers deleted)
  - [x] Register new `.cpp` files in `CMakeLists.txt` (plugin target **and** `Matrix-Control_Tests` if handlers are linked in tests)

- [x] **Unit tests** (AC: #7, #8)
  - [x] `Tests/Unit/ActionDispatcherTests.cpp` — recording fake handlers; assert `dco1Copy` → Module, `bankUtilitySelectBank3` → PatchManager, `patchMutatorMutate` → Mutator, `dco1PasteEnabled` → not dispatched, unknown ID → no handler call
  - [x] Run full `Matrix-Control_Tests`; build VST3 + Standalone

- [x] **Self-review** (AC: #8)
  - [x] No GUI changes in this story
  - [x] No Paste execution, no new SysEx paths
  - [x] No `PatchFileService` / Mutator engine wiring
  - [x] Handlers ≤ Clean Code limits (split helpers if a moved function exceeds ~25 lines)

## Dev Notes

### What Story 7.1 IS — and what it is NOT

| In scope (7.1) | Out of scope (later stories) |
|---|---|
| `ActionDispatcher` hub + registry | Module Paste + patch Init template load (**7.2**) |
| Three handler classes + interfaces | Internal STORE/PASTE/INIT SysEx (**7.3**) |
| Move existing processor action code | Computer Patches `PatchFileService` (**7.3** + **Epic 4**) |
| Mutator handler **stub** | `PatchMutatorEngine` delegation (**7.4** + **Epic 6**) |
| Routing unit tests | Matrix Mod bus reorder via dispatcher (**7.9** — reorder still via `PluginEditor` → `swapMatrixModBusContents` today) |
| CMake registration | Full graying / shell persistence (**7.8**) |

### Problem / context

Brownfield `PluginProcessor::valueTreePropertyChanged` already implements four action handler clusters inline (lines ~1148–1502). Stories **5.2** and **5.3** deliberately kept Copy handlers in the processor with an explicit handoff to **7.1**. Sprint Change Proposal (2026-06-18) sequence: **5.3 → 7.1 → 7.2**.

Epic **7** stories **7.7**, **7.9**, **7.10** were implemented out of order; **7.1** is still the structural prerequisite for **7.2–7.6** and for migrating reorder off the editor callback.

### APVTS property taxonomy (D-031) — classification for this story

| Class | Pattern | Routed via ActionDispatcher? | Examples |
|---|---|---|---|
| **event** | `setProperty(id, Time::getCurrentTime().toMilliseconds())` | **Yes** | `dco1Copy`, `bankUtilitySelectBank0`, `patchMutatorMutate` |
| **state** | `setProperty(id, value)` persisted | **No** (processor clamp/sync) | `internalPatchesCurrentBankNumber`, `bankUtilityBankLock`, Mutator Amount slider |
| **uiMirror** | Core → GUI enabled flags | **No** | `dco1PasteEnabled`, `matrixModulationPasteEnabled` |
| **parameter** | `AudioProcessorParameter` | **No** (existing SysEx dispatch) | `modulationBus0Source`, `dco1Frequency` |

GUI timestamp pattern (do not change):

```cpp
apvts_->state.setProperty(id, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
```

Sources: `ModuleHeader.cpp:227,256,266`, `ModulationBusCell.cpp:141`.

### Handler ownership split (AD-5)

| Handler | Register these `PluginIDs` groups |
|---|---|
| **ModuleActionHandler** | `PatchEditSection::*Module::StandaloneWidgets` (I/C/P where present), `MasterEditSection::*::StandaloneWidgets::kInit`, `MatrixModulationSection::StandaloneWidgets`, `MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus*Init` |
| **PatchManagerActionHandler** | `PatchManagerSection::BankUtilityModule::StandaloneWidgets`, `InternalPatchesModule::StandaloneWidgets`, `ComputerPatchesModule::StandaloneWidgets` |
| **MutatorActionHandler** | `PatchManagerSection::PatchMutatorModule::StandaloneWidgets` (include enable toggles — they are stateful but still mutator domain; route only **event** IDs if toggles use timestamps; Amount/Random are APVTS parameters, not actions) |

**Note:** `BankUtilityModule::StateProperties::kSelectedBank` and `kBankLock` are **state** properties set *by* handlers — do not register as inbound actions.

### Current processor code to migrate (read before editing)

| Method | File:lines (approx) | Target handler |
|---|---|---|
| `handleClipboardCopyPropertyChange` | `PluginProcessor.cpp:1413–1447` | `ModuleActionHandler` |
| `handleMatrixModInitPropertyChange` | `PluginProcessor.cpp:1301–1322` | `ModuleActionHandler` |
| `handleMasterModuleInitPropertyChange` | `PluginProcessor.cpp:1340–1364` | `ModuleActionHandler` |
| `handlePatchManagerPropertyChange` | `PluginProcessor.cpp:1449–1502` | `PatchManagerActionHandler` |
| `parseBankButtonIndex` | `PluginProcessor.cpp:1265–1281` | `PatchManagerActionHandler` (private) |
| `parseMatrixModBusInitIndex` | `PluginProcessor.cpp:1283–1299` | `ModuleActionHandler` (private) |

**Keep in `PluginProcessor`** (not action events):

- `handleBankNumberChange` / `handlePatchNumberChange` — react to NumberBox **state** edits + Program Change (`PluginProcessor.cpp:1174–1216`)
- Parameter SysEx blocks (`patchParameterIds_`, `masterParameterIds_`, `matrixModParameterIds_`)
- `refreshClipboardPasteEnabledProperties` / `initializeClipboardPasteEnabledProperties`
- `reconcilePatchManagerCoordinatesForDeviceType`, device type property listeners

### Suppress-flag coordination

`handleMatrixModInitPropertyChange` and `handleMasterModuleInitPropertyChange` toggle `suppressMatrixModParameterSysEx_` / `suppressMasterParameterSysEx_` on the processor. Options (pick one, document in Dev Agent Record):

1. **Preferred:** `struct ActionExecutionHooks { std::function<void(bool)> setSuppressMatrixModSysEx; ... }` passed into `ModuleActionHandler` at construction.
2. Friend class — avoid.

Do **not** duplicate suppress logic inside handlers without processor visibility.

### `ModuleActionHandler` dependencies (inject via ctor)

| Dependency | Used for |
|---|---|
| `juce::AudioProcessorValueTreeState&` | read/write properties, footer messages |
| `Core::PatchModel&` | clipboard copy |
| `Core::ApvtsPatchMapper&` | `apvtsToBuffer()` before copy |
| `Core::ClipboardService&` | copy operations |
| `Core::MatrixModInitService&` | bus/section init |
| `Core::MasterModuleInitService&` | MASTER module init |
| `std::function<void()>` refresh paste mirrors | `refreshClipboardPasteEnabledProperties` |
| `ActionExecutionHooks` | SysEx suppress flags |

### `PatchManagerActionHandler` dependencies

| Dependency | Used for |
|---|---|
| `juce::AudioProcessorValueTreeState&` | bank/patch coordinates |
| `MidiManager*` (nullable) | `sendProgramChange` when patch number state changes — only if navigation handler sets patch number |
| `Core::DeviceMemoryLimits` resolver | `getResolvedDeviceMemoryLimits()` pattern — extract to free function in `DeviceMemoryLimits` or inject lambda from processor |

### `MutatorActionHandler` (stub)

Register all mutator StandaloneWidget IDs in the registry so Story **7.4** only fills bodies. `handleAction` returns immediately. No `PatchMutatorEngine` include (Epic 6 backlog).

### Registry maintenance

Centralize action IDs in `ActionPropertyRegistry.cpp` using `PluginIDs` constants only — never raw strings. When adding a row, comment the widget source (ModuleHeader, InternalPatchesPanel, etc.).

**Deferred from 5.2:** `patchModuleKindFromWidgetId` also maps Paste IDs — `ModuleActionHandler` must guard Paste stubs with `endsWith("Paste")` or explicit Paste ID list until **7.2** implements paste.

### Architecture compliance

- Core **must not** include GUI headers (`architecture.md` AD-1).
- `PluginProcessor` remains composition root (AD-2).
- One primary class per `.h/.cpp` pair under `Actions/`.
- APVTS identifiers only from `PluginIDs.h`.
- No business logic added to GUI panels.

### File structure

```
Source/Core/Actions/
├── ActionDispatcher.h/.cpp
├── ActionPropertyRegistry.h/.cpp
├── IActionHandler.h                    # or split interfaces
├── ModuleActionHandler.h/.cpp
├── PatchManagerActionHandler.h/.cpp
└── MutatorActionHandler.h/.cpp

Tests/Unit/
└── ActionDispatcherTests.cpp
```

Update `CMakeLists.txt`:

- Plugin `target_sources`: add six `Actions/*.cpp` files near other `Source/Core/` entries (~line 107).
- `Matrix-Control_Tests`: add `ActionDispatcherTests.cpp` + link any new `.cpp` needed by tests (mirror `ClipboardServiceTests` pattern — tests include Core headers directly).

### Testing requirements

Follow `Tests/Unit/ClipboardServiceTests.cpp` — `juce::UnitTest` subclass registered in `Tests/TestMain.cpp`.

Minimum test cases:

1. Registry contains representative IDs from each handler domain.
2. `isActionProperty("dco1PasteEnabled") == false`.
3. Dispatcher invokes exactly one handler per action ID.
4. Dispatcher ignores unregistered IDs silently (no crash).
5. Optional: timestamp `var` type int64 passes through unchanged.

No `PluginProcessor` integration test required in **7.1** — routing isolation is sufficient per AC.

### Previous epic learnings (5.2, 5.3)

- Copy handlers must call `apvtsPatchMapper_->apvtsToBuffer()` before `ClipboardService` reads `PatchModel`.
- After copy, call `refreshClipboardPasteEnabledProperties()`.
- Matrix Mod Copy ID: `PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationCopy`.
- Do not route `*PasteEnabled` properties — GUI listeners depend on them (`ModuleHeader` `PasteEnabledPropertyListener`).
- `valueTreeRedirected` must still refresh paste mirrors (processor keeps this).

### Out-of-order Epic 7 stories (context only)

| Story | Status | Relevance to 7.1 |
|---|---|---|
| **7.7** Settings shell | done | Settings calls processor directly; no dispatcher needed for prefs sliders |
| **7.9** Matrix Mod reorder UX | done | Reorder still via `PluginEditor` → `swapMatrixModBusContents`; optional follow-up to route through `ModuleActionHandler` |
| **7.10** About modal | done | No action dispatch |

### Git intelligence (recent commits)

| Commit | Relevance |
|---|---|
| `aa72810` Story 5.3 — Matrix Mod section I/C/P GUI + Copy property IDs | Copy properties now exist; must be in registry |
| `494bb5e` Story 5.2 — paste-enabled mirrors + Copy handlers in processor | Code to migrate |
| `53f70a2` Story 5.1 — `ClipboardService` | Handler dependency |

### Latest tech / framework notes

- **JUCE 8.0.12** — `juce::UnitTest`, `ValueTree::Listener` on message thread only.
- Action properties use `juce::var` int64 milliseconds — no `std::chrono` required.
- No new third-party libraries.

### Project context reference

- `project-context.md` — C++17, builds under `Builds/`, English source only, no GUI in Core.
- `CONVENTIONS.md` — authoritative style; Clean Code function length.
- Critical path D-058: E7 ActionDispatcher unblocks handler stories **7.2–7.6**.

### References

- [epics.md — Story 7.1, Epic 7]
- [prd.md — FR-47]
- [.decision-log.md — D-031, D-032]
- [architecture.md — AD-2, AD-5, APVTS property taxonomy, `Source/Core/Actions/`]
- [sprint-change-proposal-2026-06-18.md — implementation sequence item 4]
- [5-2-module-copy-paste-enable-and-gray-rules.md — processor Copy handoff]
- [5-3-matrix-modulation-section-i-c-p-gui.md — Matrix Mod Copy IDs]
- [7-7-settings-page-consolidation.md — panels stay presentation-only]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking (Cursor)

### Debug Log References

- Suppress-flag coordination: `ActionExecutionHooks` with lambdas bound to `PluginProcessor::suppressMatrixModParameterSysEx_` / `suppressMasterParameterSysEx_`.
- `internalPatchesCopy` registered under `Module` handler (clipboard domain); navigation/store/paste remain `PatchManager` stubs until 7.2/7.3.
- Mutator registry includes event-button IDs only (`kMutate`, `kRetry`, etc.) — Amount/Random/History/enables are state or APVTS parameters.

### Completion Notes List

- Added `Source/Core/Actions/` hub: `ActionPropertyRegistry`, `ActionDispatcher`, `IActionHandler`, three handler classes, `ActionExecutionHooks`.
- Migrated processor action handlers into `ModuleActionHandler` and `PatchManagerActionHandler`; `MutatorActionHandler` is a no-op stub.
- `PluginProcessor::valueTreePropertyChanged` dispatches only registered action IDs via `ActionPropertyRegistry::isActionProperty`.
- Added `Tests/Unit/ActionDispatcherTests.cpp` with recording fake handlers (no `PluginProcessor` required).
- Full `Matrix-Control_Tests` pass; macOS Debug VST3 + Standalone build green.

### File List

- Source/Core/Actions/ActionHandlerKind.h (new)
- Source/Core/Actions/ActionExecutionHooks.h (new)
- Source/Core/Actions/IActionHandler.h (new)
- Source/Core/Actions/ActionPropertyRegistry.h (new)
- Source/Core/Actions/ActionPropertyRegistry.cpp (new)
- Source/Core/Actions/ActionDispatcher.h (new)
- Source/Core/Actions/ActionDispatcher.cpp (new)
- Source/Core/Actions/ModuleActionHandler.h (new)
- Source/Core/Actions/ModuleActionHandler.cpp (new)
- Source/Core/Actions/PatchManagerActionHandler.h (new)
- Source/Core/Actions/PatchManagerActionHandler.cpp (new)
- Source/Core/Actions/MutatorActionHandler.h (new)
- Source/Core/Actions/MutatorActionHandler.cpp (new)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/ActionDispatcherTests.cpp (new)
- CMakeLists.txt (modified)
- _bmad-output/implementation-artifacts/sprint-status.yaml (modified)

### Review Findings

- [x] [Review][Decision] **`internalPatchesCopy` routé vers `ModuleActionHandler` au lieu de `PatchManagerActionHandler`** — **Resolved (2026-06-18):** keep Module — clipboard domain; brownfield behaviour preserved; AC 2 ownership table updated intent in Dev Agent Record.

- [x] [Review][Defer] **Flags SysEx suppress sans RAII en cas d’exception** [`ModuleActionHandler.cpp:59-68,90-96`] — deferred, pre-existing (même pattern que `PluginProcessor` stories 3-3/3-4 ; déjà listé dans `deferred-work.md`).

- [x] [Review][Defer] **Sélection banque sans clamp aux limites device** [`PatchManagerActionHandler.cpp:70-73`] — deferred, pre-existing (comportement identique à l’ancien `handlePatchManagerPropertyChange`).

- [x] [Review][Defer] **`ModuleActionHandler::handleAction` enchaîne trois sous-handlers** [`ModuleActionHandler.cpp:33-37`] — deferred, dette structurelle brownfield ; refactor ciblé recommandé en 7.2.

- [x] [Review][Defer] **Garde `endsWith("Init")` trop large pour stubs patch Init** [`ModuleActionHandler.cpp:42-43`] — deferred, story 7.2 ; remplacer par liste explicite des IDs PatchEdit Init.

- [x] [Review][Defer] **Chaînes brutes `uiMessageText` / `uiMessageSeverity`** [`ModuleActionHandler.cpp:148,153`] — deferred, pre-existing (code migré tel quel ; `ExceptionPropagator` expose déjà des constantes).

## Change Log

- 2026-06-18: Story 7.1 implemented — ActionDispatcher hub, handler migration from PluginProcessor, routing unit tests.
- 2026-06-18: Code review — 1 decision resolved (keep `kCopyPatch` → Module), 5 deferred, 8 dismissed; story → done.
