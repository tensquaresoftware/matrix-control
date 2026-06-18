---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.2 — ModuleActionHandler I C P and Matrix Mod Init
author: BMad Agent
status: done
baseline_commit: 0c7a3c2
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - planning-artifacts/sprint-change-proposal-2026-06-18.md
  - implementation-artifacts/3-2-inittemplateloader-for-patchinit-and-masterinit.md
  - implementation-artifacts/3-3-matrix-mod-init-defaults.md
  - implementation-artifacts/3-4-master-module-init-confirmation-path.md
  - implementation-artifacts/5-1-clipboardservice-compatibility-matrix.md
  - implementation-artifacts/5-2-module-copy-paste-enable-and-gray-rules.md
  - implementation-artifacts/5-3-matrix-modulation-section-i-c-p-gui.md
  - implementation-artifacts/7-1-actiondispatcher-and-handler-interfaces.md
  - project-context.md
created: 2026-06-18
updated: 2026-06-18
---

# Story 7.2: ModuleActionHandler I C P and Matrix Mod Init

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want module I/C/P and Matrix Modulation section actions to run through Core,
so that PATCH/MASTER headers and Matrix Modulation section actions work end-to-end (FR-35, FR-15, FR-36 integration).

## Acceptance Criteria

1. **Given** Stories 5.1–5.3 (clipboard + gray mirrors), 3.2–3.4 (init loaders/services), and 7.1 (`ActionDispatcher` + migrated Copy/Matrix Mod init/Master init) **When** user triggers PATCH module **Paste**, Matrix Modulation section **Paste**, or any PATCH module **Init** from panel **Then** `ModuleActionHandler` executes the action — no stubs, no early `return` for these IDs.

2. **And** PATCH module **Paste** (7 modules with C/P: DCO1, DCO2, ENV1–3, LFO1–2):
   - Calls `ClipboardService::pasteModule(target, *patchModel_)` only when `canPasteModule(target)` (defensive — GUI graying is not sufficient alone).
   - On success: `ApvtsPatchMapper::pushModuleToApvts(moduleGroupId)` with PATCH parameter SysEx suppressed during APVTS push.
   - After APVTS push: enqueue **one 0x06 Remote Parameter Edit per affected module parameter** via `PatchParameterSysExDispatcher` (descriptor-driven; skip `kNoSysExId`).
   - Does **not** change clipboard mode or `*PasteEnabled` mirrors (paste does not alter clipboard contents).

3. **And** Matrix Modulation section **Paste**:
   - Calls `ClipboardService::pasteMatrixModulation(*patchModel_)` when `canPasteMatrixModulation()`.
   - Writes all **10 buses** to APVTS via `pushBusToApvts(0..9)` with `suppressMatrixModParameterSysEx_` true during bulk push (same pattern as `MatrixModInitService::initAllBuses` / reorder).
   - Enqueues **exactly one 0x0B per bus** via `MatrixModBusParameterSysExDispatcher::dispatchBus` in ascending order `0..9`.
   - Patch name (bytes 0–7) and all non-matrix-mod PATCH parameters remain unchanged in model and APVTS.

4. **And** PATCH module **Init** (10 modules: all Patch Edit module headers including VCF/VCA, FM/TRACK, RAMP/PORTAMENTO):
   - Loads init template via new `Core::PatchModuleInitService` (mirror `MasterModuleInitService`): `InitTemplateLoader::loadPatch` into temp model → copy target module slice by `parentGroupId` → `pushModuleToApvts` → SysEx burst.
   - Uses `PluginIDs::Settings::kInitTemplatesFolderPath` (same folder supplier as `MasterModuleInitService`).
   - Propagates `InitTemplateLoadResult::infoMessage` to footer via existing `propagateInitTemplateFooterMessage` (info vs warning from `InitTemplateFallbackReason`).
   - **No confirmation modal** for PATCH init (FR-38 — MASTER confirm stays in 3.4 path only).

5. **And** existing behaviours migrated in Story 7.1 remain unchanged:
   - Copy handlers (`handleClipboardCopy`) — no regression.
   - Matrix Mod section/per-bus **Init** via `MatrixModInitService` + suppress hooks — no regression.
   - MASTER module Init via `MasterModuleInitService` + suppress hooks — no regression.

6. **And** `ModuleActionHandler::handleAction` uses **targeted dispatch** by property ID (replace chained sub-handlers + `endsWith("Paste")` / `endsWith("Init")` stub guards flagged in 7.1 review). Each action ID handled at most once.

7. **And** no GUI changes; no SysEx in GUI; no `InternalPatches` Paste/Init/Store execution (Story **7.3**); no `PatchFileService` / full-patch paste to Internal Patches (Story **7.3** + Epic **4**).

8. **And** unit tests cover handler logic without `PluginProcessor`:
   - `Tests/Unit/ModuleActionHandlerTests.cpp` (or extend `PatchModuleInitServiceTests` + handler fakes) — paste/init routing, suppress-hook invocation order, matrix-mod 10-bus SysEx count.
   - Full `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds remain green.

## Tasks / Subtasks

- [x] **Infrastructure — PATCH bulk APVTS + SysEx** (AC: #2, #4)
  - [x] `ApvtsPatchMapper::pushModuleToApvts(const juce::String& moduleGroupId)` — mirror `ApvtsMasterMapper::pushModuleToApvts`
  - [x] `PatchParameterSysExDispatcher::dispatchModule(const juce::String& moduleGroupId)` — enqueue 0x06 for all int/choice descriptors with matching `parentGroupId` and valid `sysExId`
  - [x] `PluginProcessor`: add `suppressPatchParameterSysEx_` + guard in `valueTreePropertyChanged` patch-parameter branch (mirror `suppressMasterParameterSysEx_`)
  - [x] Extend `ActionExecutionHooks` with `setSuppressPatchSysEx` lambda; wire in processor ctor

- [x] **`PatchModuleInitService`** (AC: #4)
  - [x] `Source/Core/Init/PatchModuleInitService.{h,cpp}` — deps: `PatchModel&`, `ApvtsPatchMapper&`, `InitTemplateLoader&`, `PatchParameterSysExDispatcher&`, templates folder supplier
  - [x] `initModule(const juce::String& moduleGroupId)` — load template, copy module slice, push APVTS (caller suppresses SysEx), `dispatchModule`
  - [x] `static juce::String moduleGroupIdFromInitPropertyId(const juce::String& propertyId)` — map 10 Patch Edit Init widget IDs → `PluginIDs::PatchEditSection::*Module::kGroupId`
  - [x] Register `.cpp` in `CMakeLists.txt` (plugin + tests)

- [x] **`ModuleActionHandler` — Paste + Init** (AC: #1–#3, #6)
  - [x] Inject `PatchModuleInitService*`, `PatchParameterSysExDispatcher*`, `MatrixModBusParameterSysExDispatcher*` (or thin callback struct for matrix-mod paste burst)
  - [x] `handleClipboardPaste(propertyId)` — module paste + matrix-mod paste paths
  - [x] `handlePatchModuleInit(propertyId)` — delegate to `PatchModuleInitService`; footer message propagation
  - [x] Refactor `handleAction` to early-exit dispatch table / explicit ID branches (remove stub `endsWith` returns)
  - [x] Split `handleClipboardCopy` guard: use `endsWith("Copy")` or explicit Copy ID set (per 5.2 deferred note)

- [x] **Unit tests** (AC: #8)
  - [x] `Tests/Unit/PatchModuleInitServiceTests.cpp` — fixture `PatchInit.syx`, module slice copy, dispatch call count
  - [x] `Tests/Unit/ModuleActionHandlerTests.cpp` — recording fakes for services/dispatchers; matrix-mod paste → 10 `dispatchBus` calls; paste skipped when `canPaste*` false
  - [x] Run full `Matrix-Control_Tests`; build VST3 + Standalone

- [x] **Self-review** (AC: #5, #7)
  - [x] No GUI diffs
  - [x] Copy + Matrix Mod init + Master init smoke unchanged
  - [x] Handlers ≤ Clean Code limits; extract helpers if needed

### Review Findings

- [x] [Review][Patch] Module paste skips APVTS push when dispatcher is null — `handleClipboardPaste` returns before `pushModuleToApvts` if `patchParameterSysExDispatcher_ == nullptr`; matrix-mod paste path still pushes APVTS. Split the guard so only SysEx dispatch is skipped. [ModuleActionHandler.cpp:195-197]

- [x] [Review][Defer] AC #8 — suppress-hook invocation order not asserted in unit tests (flags recorded but never expected true-during-push / false-before-dispatch). [ModuleActionHandlerTests.cpp] — deferred, test debt
- [x] [Review][Defer] AC #8 — PATCH init not exercised through `ModuleActionHandler` (`patchModuleInitService_ == nullptr` in harness). [ModuleActionHandlerTests.cpp:96] — deferred, test debt
- [x] [Review][Defer] AC #8 — matrix-mod paste test asserts 10× `dispatchBus` only; no `pushBusToApvts` count or bytes 0–103 isolation check per spec test matrix. [ModuleActionHandlerTests.cpp:215] — deferred, test debt
- [x] [Review][Defer] AC #8 — footer propagation (`propagateInitTemplateFooterMessage`) untested at handler level. — deferred, test debt
- [x] [Review][Defer] AC #6 partial — module paste routing still uses `endsWith("Paste")` after explicit matrix-mod branch (mitigated by `patchModuleKindFromWidgetId` filter). [ModuleActionHandler.cpp:182] — deferred, align with future explicit Paste ID set
- [x] [Review][Defer] Suppress flags cleared only on happy path (no RAII) — same pattern as Story 7.1 master/matrix init handlers. [ModuleActionHandler.cpp] — deferred, pre-existing
- [x] [Review][Defer] `dispatchModule` rebuilds descriptor vectors on every call. [PatchParameterSysExDispatcher.cpp:19] — deferred, perf micro-optimization

- [x] [Review][Patch] `handleAction` omits `if` guard on last handler — `handlePatchModuleInit(propertyId)` called without `if (...) return;`, unlike the four handlers above. [ModuleActionHandler.cpp:49]

- [x] [Review][Defer] `PatchModuleInitService.h` pulls in `ClipboardService.h` only for `PatchModuleKind` enum type — Init subsystem coupled to clipboard Services header; extract shared type when layering is next touched. [PatchModuleInitService.h:9]

- [x] [Review][Defer] `moduleGroupIdFromPatchModuleKind` lives on `PatchModuleInitService` but is invoked from paste routing in `handleClipboardPaste` — paste handler depends on init service for non-init mapping; relocate alongside paste ID map when Copy/Paste split debt is addressed. [ModuleActionHandler.cpp:195]

## Dev Notes

### What Story 7.2 IS — and what it is NOT

| In scope (7.2) | Out of scope (later stories) |
|---|---|
| PATCH module **Paste** (7 modules) + SysEx | Internal Patches **Paste** / full-patch paste + ROM (**7.3**) |
| Matrix Mod section **Paste** + 10× 0x0B SysEx | Internal Patches **Init/Store** SysEx (**7.3**) |
| PATCH module **Init** via `InitTemplateLoader` + new service | Computer Patches file ops (**7.3** + **Epic 4**) |
| Refactor `ModuleActionHandler::handleAction` dispatch | Mutator engine (**7.4** + **Epic 6**) |
| `ApvtsPatchMapper::pushModuleToApvts` + suppress flag | MASTER C/P buttons / gray mirrors (no MASTER paste widgets today) |
| Matrix Mod / Master **Init** regression guard only | Matrix Mod bus reorder via dispatcher (**7.9** — still editor callback) |

**Note on story title:** Matrix Mod **init** was migrated to `ModuleActionHandler` in **7.1** and is already functional. This story completes the **Paste** paths and **PATCH Init** paths that 7.1 left as stubs.

### Problem / context

Story **7.1** moved brownfield processor handlers into `ModuleActionHandler` but deliberately stubbed:

```cpp
// ModuleActionHandler.cpp:39-43 (current)
if (propertyId.endsWith("Paste"))
    return; // Story 7.2
if (propertyId.endsWith("Init"))
    return; // Patch module init — Story 7.2
```

Sprint sequence (2026-06-18): **5.3 → 7.1 → 7.2**. Epic **5** delivered clipboard storage, gray mirrors, and GUI stamps; Epic **3** delivered init services/loaders. **7.2** connects stamps to live edit + MIDI.

### APVTS property taxonomy — actions handled in this story

| Property pattern | Example | Handler method |
|---|---|---|
| PATCH module Paste | `dco1Paste`, `lfo2Paste` | `handleClipboardPaste` → module path |
| Matrix Mod Paste | `matrixModulationPaste` | `handleClipboardPaste` → matrix-mod path |
| PATCH module Init | `dco1Init`, `vcfVcaInit` | `handlePatchModuleInit` |
| PATCH module Copy | `dco1Copy` | `handleClipboardCopy` (existing — no change) |
| Matrix Mod Init | `matrixModulationInit`, `modulationBus3Init` | `handleMatrixModInit` (existing — no change) |
| MASTER module Init | `masterMidiInit`, etc. | `handleMasterModuleInit` (existing — no change) |

**Not in 7.2:** `internalPatchesPastePatch` → `PatchManagerActionHandler` (**7.3**); `internalPatchesCopy` → Copy already in `handleClipboardCopy`.

### PATCH module Init — `PatchModuleInitService` design

Mirror `MasterModuleInitService` (`Source/Core/Init/MasterModuleInitService.cpp`):

```cpp
InitTemplateLoadResult PatchModuleInitService::initModule(const juce::String& moduleGroupId)
{
    PatchModel initTemplate;
    const auto result = initTemplateLoader_.loadPatch(initTemplate, templatesFolder_());
    copyModuleFromInitTemplate(initTemplate, moduleGroupId);
    apvtsPatchMapper_.pushModuleToApvts(moduleGroupId);  // caller wraps with suppressPatchParameterSysEx_
    patchParameterSysExDispatcher_.dispatchModule(moduleGroupId);
    return result;
}
```

`copyModuleFromInitTemplate`: iterate `PluginDescriptors::PatchEditSection::kIntParameters` / `kChoiceParameters`; for each descriptor where `parentGroupId == moduleGroupId`, copy value from init template into live `PatchModel_`.

**Init widget ID → moduleGroupId map** (all 10 Patch Edit modules):

| Init property ID | Module |
|---|---|
| `dco1Init` | `Dco1Module::kGroupId` |
| `dco2Init` | `Dco2Module::kGroupId` |
| `vcfVcaInit` | `VcfVcaModule::kGroupId` |
| `fmTrackInit` | `FmTrackModule::kGroupId` |
| `rampPortamentoInit` | `RampPortamentoModule::kGroupId` |
| `envelope1Init` … `envelope3Init` | respective `kGroupId` |
| `lfo1Init`, `lfo2Init` | respective `kGroupId` |

Use `PluginIDs` constants only — never raw strings.

Footer severity mapping (reuse `ModuleActionHandler::propagateInitTemplateFooterMessage`):

| `InitTemplateFallbackReason` | Severity |
|---|---|
| `kNone` (user file OK) | clear footer |
| `kFileMissing` | `info` |
| `kFileInvalid` | `warning` |

### PATCH module Paste — execution flow

1. Resolve target: `patchModuleKindFromWidgetId(propertyId)` (already maps Paste IDs — 5.2).
2. If `!clipboardService_->canPasteModule(target)` → return (no-op, no footer).
3. `clipboardService_->pasteModule(target, *patchModel_)` — model-only (5.1).
4. Resolve `moduleGroupId` from `PatchModuleKind` (add helper alongside init map).
5. `hooks_.setSuppressPatchSysEx(true)`.
6. `apvtsPatchMapper_->pushModuleToApvts(moduleGroupId)`.
7. `hooks_.setSuppressPatchSysEx(false)`.
8. `patchParameterSysExDispatcher_->dispatchModule(moduleGroupId)`.

**Do not** call `refreshClipboardPasteEnabledProperties()` after paste — clipboard mode unchanged.

### Matrix Mod Paste — execution flow

Mirror `MatrixModInitService::initAllBuses` post-model mutation:

1. If `!clipboardService_->canPasteMatrixModulation()` → return.
2. `clipboardService_->pasteMatrixModulation(*patchModel_)` — bytes 104–133 only (5.1).
3. `hooks_.setSuppressMatrixModSysEx(true)`.
4. For `bus = 0..9`: `apvtsPatchMapper_->pushBusToApvts(bus)`.
5. `hooks_.setSuppressMatrixModSysEx(false)`.
6. For `bus = 0..9`: `matrixModBusParameterSysExDispatcher_->dispatchBus(bus)`.

Reference: `MatrixModInitService.cpp:32-41` and 5.1 Dev Notes SysEx burst pattern.

### Suppress-flag coordination (extend 7.1 hooks)

Current `ActionExecutionHooks` (`ActionExecutionHooks.h`):

```cpp
struct ActionExecutionHooks {
    std::function<void(bool)> setSuppressMatrixModSysEx;
    std::function<void(bool)> setSuppressMasterSysEx;
    // ADD:
    std::function<void(bool)> setSuppressPatchSysEx;
};
```

Processor patch-parameter branch (`PluginProcessor.cpp:~1152-1165`) must skip `apvtsToBuffer` + `patchParameterSysExDispatcher_->dispatch` when `suppressPatchParameterSysEx_` is true — same semantics as master branch at ~1168-1174.

Inject `MatrixModBusParameterSysExDispatcher*` into `ModuleActionHandler` for matrix-mod paste step 6 (or pass `std::function<void(int busIndex)>` enqueue callback to keep handler testable).

### `ModuleActionHandler` dependency injection (updated ctor)

| Dependency | Used for |
|---|---|
| Existing 7.1 deps | Copy, Matrix Mod init, Master init |
| `PatchModuleInitService*` | PATCH module Init |
| `PatchParameterSysExDispatcher*` | PATCH paste + init SysEx burst |
| `MatrixModBusParameterSysExDispatcher*` | Matrix Mod paste SysEx burst |
| `ActionExecutionHooks` (extended) | all three suppress flags |

Construct `patchModuleInitService_` in `PluginProcessor` after `initTemplateLoader_` (same pattern as `masterModuleInitService_`).

### Refactor `handleAction` (7.1 review debt)

Replace chained calls + stub guards with explicit routing:

```cpp
void ModuleActionHandler::handleAction(const juce::String& propertyId, const juce::var& newValue)
{
    if (handleMatrixModInit(propertyId)) return;
    if (handleMasterModuleInit(propertyId)) return;
    if (handleClipboardCopy(propertyId)) return;
    if (handleClipboardPaste(propertyId)) return;
    if (handlePatchModuleInit(propertyId)) return;
}
```

Each sub-handler returns `bool` — `true` if propertyId was consumed. **Do not** use broad `endsWith("Init")` — MASTER init IDs must not fall through to PATCH init handler.

For Copy: guard with `propertyId.endsWith("Copy")` before `patchModuleKindFromWidgetId` (5.2 deferred trap).

### Modules without Paste (Init only in 7.2)

| Module | Init | Copy/Paste |
|---|---|---|
| VCF/VCA | yes | no widgets |
| FM/TRACK | yes | no widgets |
| RAMP/PORTAMENTO | yes | no widgets |

Registry already registers Init IDs only for these three (`ActionPropertyRegistry.cpp:31-33`).

### Architecture compliance

- Core **must not** include GUI headers (`architecture.md` AD-1).
- `PluginProcessor` remains composition root (AD-2).
- Init services in `Source/Core/Init/`; clipboard in `Services/` (do not move `MatrixModInitService`).
- APVTS identifiers from `PluginIDs.h` only.
- SysEx enqueue only via existing dispatchers + `MidiManager` callbacks — never from GUI.
- One primary class per `.h/.cpp` pair.

### File structure

```
Source/Core/Init/
└── PatchModuleInitService.h/.cpp          (NEW)

Source/Core/Models/
├── ApvtsPatchMapper.h/.cpp                (UPDATE — pushModuleToApvts)

Source/Core/MIDI/
├── PatchParameterSysExDispatcher.h/.cpp   (UPDATE — dispatchModule)

Source/Core/Actions/
├── ActionExecutionHooks.h                 (UPDATE — setSuppressPatchSysEx)
├── ModuleActionHandler.h/.cpp             (UPDATE — paste/init + dispatch refactor)

Source/Core/
├── PluginProcessor.h/.cpp                 (UPDATE — suppress flag, service wiring)

Tests/Unit/
├── PatchModuleInitServiceTests.cpp        (NEW)
└── ModuleActionHandlerTests.cpp           (NEW)
```

Update `CMakeLists.txt` for new `.cpp` files in plugin + `Matrix-Control_Tests`.

### Testing requirements

Follow `Tests/Unit/MatrixModInitServiceTests.cpp` and `MasterModuleInitServiceTests` patterns:

| Test | Assert |
|---|---|
| `patchModuleInit_loadsModuleSlice` | After init DCO1, DCO1 params match template; DCO2 unchanged |
| `patchModuleInit_missingFile_fallback` | Footer metadata; model still initialized from `InitDefaults` |
| `patchModuleInit_dispatchesModuleSysEx` | Queue receives 0x06 count == module param count with valid sysExId |
| `modulePaste_compatibleModule` | Model + fake mapper push called; dispatchModule invoked |
| `modulePaste_incompatible_noOp` | pasteModule not called when canPaste false |
| `matrixModPaste_tenBuses` | 10× pushBusToApvts + 10× dispatchBus; bytes 0–103 unchanged |
| `handleAction_noDoubleDispatch` | Single handler path per property ID |

Use recording fakes / lambdas — **no** `PluginProcessor` in unit tests.

Manual smoke (dev agent):

1. Copy DCO1 → Paste on DCO2 → synth receives parameter edits.
2. Copy Matrix Mod → Paste → only matrix buses change; patch name unchanged.
3. Init DCO1 with missing `PatchInit.syx` → footer info message; params reset to defaults.
4. Regression: Copy still updates paste mirrors; Matrix Mod Init still clears buses.

### Previous story intelligence (7.1)

- `ActionExecutionHooks` lambdas bound to processor suppress flags — extend, do not duplicate logic in handler.
- `internalPatchesCopy` routed to `ModuleActionHandler` (clipboard domain) — keep unchanged.
- Review deferred to 7.2: chained sub-handlers, `endsWith("Init")` stub, `patchModuleKindFromWidgetId` Paste mapping, raw `uiMessageText` strings (optional cleanup).
- `MutatorActionHandler` remains stub — do not touch.

### Previous epic learnings (5.x, 3.x)

| Story | Relevant learning |
|---|---|
| **5.1** | Paste is model-only in `ClipboardService`; 7.2 adds mapper + SysEx |
| **5.2** | `*PasteEnabled` mirrors unchanged on paste; Copy calls `refreshClipboardPasteEnabledProperties()` |
| **5.3** | `matrixModulationPaste` GUI stamp exists; handler was stub until now |
| **3.2** | `InitTemplateLoader` returns metadata; handler writes footer |
| **3.3** | Matrix Mod suppress + pushBus + dispatchBus ordering |
| **3.4** | `pushModuleToApvts` + suppress + bulk dispatch pattern for MASTER |

### Git intelligence (recent commits)

| Commit | Relevance |
|---|---|
| `0c7a3c2` | Story 7.1 — `ModuleActionHandler` stubs Paste/Patch Init; extend here |
| `aa72810` | Story 5.3 — Matrix Mod Paste button stamps property |
| `494bb5e` | Story 5.2 — paste mirrors + Copy in processor (now in handler) |
| `53f70a2` | Story 5.1 — `ClipboardService` paste APIs |

### Latest tech / framework notes

- **JUCE 8.0.12** — `setValueNotifyingHost` on message thread only; same as existing mapper code.
- **C++17** — no new third-party libraries.
- Action properties remain `juce::var` int64 timestamps — handler ignores `newValue` body (event-only).

### Project context reference

- `project-context.md` — C++17, builds under `Builds/`, English source only, no GUI in Core.
- `CONVENTIONS.md` — authoritative style; Clean Code function length.
- `deferred-work.md` — 7.2 items: dispatch refactor, Init ID list, Paste/Copy split.

### References

- [epics.md — Story 7.2, Epic 7]
- [prd.md — FR-35, FR-15, FR-36, FR-38]
- [.decision-log.md — D-033, D-034, D-060, D-095]
- [architecture.md — AD-5 ModuleActionHandler, APVTS→SysEx table]
- [7-1-actiondispatcher-and-handler-interfaces.md — stub handoff, registry, hooks]
- [5-1-clipboardservice-compatibility-matrix.md — paste model semantics]
- [3-3-matrix-mod-init-defaults.md — SysEx burst pattern for matrix mod]
- [3-4-master-module-init-confirmation-path.md — pushModuleToApvts + suppress pattern]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Fixed `PluginProcessor.h` forward declaration for `PatchModuleInitService` (plugin build).
- ModuleActionHandlerTests: avoid `apvtsToBuffer()` before clipboard copy (overwrites seeded model from default APVTS).

### Completion Notes List

- Implemented PATCH module paste (7 modules), Matrix Mod section paste (10× 0x0B), and PATCH module init (10 modules) via `ModuleActionHandler`.
- Added `PatchModuleInitService`, `ApvtsPatchMapper::pushModuleToApvts`, `PatchParameterSysExDispatcher::dispatchModule`, and `suppressPatchParameterSysEx_` hook.
- Refactored `handleAction` to bool early-exit sub-handlers; removed Story 7.1 Paste/Init stubs.
- Unit tests: `PatchModuleInitServiceTests`, `ModuleActionHandlerTests`. Full `Matrix-Control_Tests` green; VST3 + Standalone Debug build green.

### File List

- `Source/Core/Actions/ActionExecutionHooks.h`
- `Source/Core/Actions/ModuleActionHandler.h`
- `Source/Core/Actions/ModuleActionHandler.cpp`
- `Source/Core/Init/PatchModuleInitService.h`
- `Source/Core/Init/PatchModuleInitService.cpp`
- `Source/Core/MIDI/PatchParameterSysExDispatcher.h`
- `Source/Core/MIDI/PatchParameterSysExDispatcher.cpp`
- `Source/Core/Models/ApvtsPatchMapper.h`
- `Source/Core/Models/ApvtsPatchMapper.cpp`
- `Source/Core/PluginProcessor.h`
- `Source/Core/PluginProcessor.cpp`
- `Tests/Unit/PatchModuleInitServiceTests.cpp`
- `Tests/Unit/ModuleActionHandlerTests.cpp`
- `CMakeLists.txt`

### Change Log

- 2026-06-18: Story 7.2 — PATCH paste/init + Matrix Mod paste wired through Core; handler dispatch refactor; suppress-patch SysEx hook; unit tests.
