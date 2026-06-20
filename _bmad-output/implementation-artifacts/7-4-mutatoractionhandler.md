---
organization: Ten Square Software
project: Matrix-Control
title: Story 7.4 — MutatorActionHandler
author: BMad Agent
status: review
baseline_commit: a1547e6
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/7-1-actiondispatcher-and-handler-interfaces.md
  - implementation-artifacts/7-3-patchmanageractionhandler-bank-and-internal.md
  - implementation-artifacts/7-3b-bank-utility-unlock-semantics-and-id-rename.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 7.4: MutatorActionHandler

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want Mutator buttons routed to `PatchMutatorEngine`,
so that MUTATE/RETRY/DELETE/CLEAR/EXPORT/COMPARE work from panel (FR-30–FR-34, FR-54–FR-60).

## Acceptance Criteria

> **HARD PREREQUISITE — Epic 6:** `PatchMutatorEngine` and its backing services (`MutationHistoryStore`, `MutationAlgorithm`, `MutationNaming`, history UI property sync, debounced audition) **must exist** before this story can be marked `done`. Epic 6 is **backlog** today — no `Source/Core/Services/PatchMutator/` code exists. **Do not implement mutation business logic in the handler**; wire only. If Epic 6 is incomplete, implement handler scaffold + tests with fakes and stop at `review` with explicit blocker note.

1. **Given** Story 7.1 (`ActionDispatcher` routes six Mutator event IDs to `MutatorActionHandler`) and Epic 6 `PatchMutatorEngine` available **When** user clicks MUTATE, RETRY, COMPARE, DELETE, CLEAR, or EXPORT **Then** `MutatorActionHandler::handleAction` delegates to the appropriate `PatchMutatorEngine` method — **no stubs**, no empty `return` for these IDs.

2. **And** each successful MUTATE, RETRY, and post-debounce History audition path enqueues **one** full patch SysEx **0x01** via `MidiManager::sendPatch` (FR-30, AD-4) — SysEx orchestration lives in `PatchMutatorEngine` (Epic 6), not in the handler; handler must not duplicate enqueue logic.

3. **And** History selection audition debounce (FR-57): when APVTS **state** properties `selectedM` / `selectedR` change (Story 6.6 — **not** the legacy single `patchMutatorHistory` stub), `PluginProcessor::valueTreePropertyChanged` routes to `MutatorActionHandler::onHistorySelectionChanged()` (or injected debouncer owned by handler) using shared constant `kComboboxPatchSendDebounceMs` (architecture debounce policy). Rapid combobox scrolling must not SysEx-flood the synth.

4. **And** blocked actions (FR-60): when engine returns failure/limit (100 roots, 100 retries, empty history for DELETE, etc.), handler propagates footer via APVTS `uiMessageText` + `uiMessageSeverity` — same pattern as `PatchManagerActionHandler::propagateRomBlockedFooter` / `ModuleActionHandler::propagateInitTemplateFooterMessage`. No modal from handler for limit cases — engine owns Defrag modal trigger (FR-59, Story 6.10).

5. **And** EXPORT (FR-33): handler invokes `PatchMutatorEngine::exportHistory(folder)` after obtaining destination folder via injected `std::function<juce::File()>` folder-picker callback from composition root (processor/editor). Handler must **not** include GUI headers; callback supplied at construction. If `PatchFileService::exportMutatorHistory` is the engine delegate (Story 6.11), call through engine only.

6. **And** COMPARE (FR-32): toggle delegates to engine; engine updates `compareActive` uiMirror + grays History M/R via APVTS properties (Story 6.8). Handler does not manipulate combo widgets directly.

7. **And** recipe state (Amount, Random, module enable toggles — FR-34) remains **state** properties / APVTS parameters handled by existing panel bindings — **not** routed through `ActionPropertyRegistry`. Handler reads recipe from APVTS only when engine requests current recipe snapshot (Epic 6.12).

8. **And** **no GUI changes** in this story (panel refactor to History M/R comboboxes → Epic 6.6; enabled-state graying polish → Epic 6.12 or follow-up UI story). `PatchMutatorPanel` already uses timestamp `setProperty` for buttons — preserve.

9. **And** Settings manual Defrag (FR-59, D-087) is **out of scope** until a Settings action property ID exists (7.7 Phase B placeholder today). When added later, route via same handler or sibling method — document extension point in handler header comment only; do not implement Settings UI here.

10. **And** unit tests in `Tests/Unit/MutatorActionHandlerTests.cpp` cover handler delegation without `PluginProcessor` or GUI:
    - Each event ID calls the expected engine method (recording fake engine).
    - Blocked mutate/retry sets footer properties.
    - History selection change schedules debounced engine call (fake timer or injectable debouncer).
    - EXPORT invokes folder callback then engine export.
    - Full `Matrix-Control_Tests` + macOS Debug VST3/Standalone builds remain green.

## Tasks / Subtasks

- [x] **Verify Epic 6 gate** (AC: prerequisite)
  - [x] Confirm `Source/Core/Services/PatchMutator/PatchMutatorEngine.*` exists with methods listed in Dev Notes API contract
  - [x] If missing → implement handler + fakes + tests only; mark story `review` with blocker; do **not** fake engine logic in handler

- [x] **Extend `MutatorActionHandler` constructor** (AC: #1, #5, #7)
  - [x] Inject `PatchMutatorEngine*`, `juce::AudioProcessorValueTreeState&`, optional `std::function<juce::File()>` exportFolderPicker
  - [x] Optional `HistorySelectionDebouncer` member (or use shared Core debounce helper)
  - [x] Private methods: `handleMutate`, `handleRetry`, `handleCompare`, `handleDelete`, `handleClear`, `handleExport`, `propagateFooterMessage`

- [x] **Implement `handleAction` dispatch** (AC: #1, #4)
  - [x] Map `PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::{kMutate,kRetry,kCompare,kDelete,kClear,kExport}` → engine calls
  - [x] Translate engine `Result` / status enum to footer properties on failure
  - [x] Remove Story 7.4 stub comment body

- [x] **History selection debounce wiring** (AC: #3)
  - [x] Add `MutatorActionHandler::onHistorySelectionChanged()` public method
  - [x] In `PluginProcessor::valueTreePropertyChanged`, detect `selectedM` / `selectedR` state changes → delegate to handler (when Epic 6.6 properties exist)
  - [x] Use `kComboboxPatchSendDebounceMs` — extract shared debounce helper if not yet present from Epic 6.7
  - [x] Legacy `patchMutatorHistory` single combobox: no-op or forward compat until panel updated in 6.6

- [x] **`PluginProcessor` wiring** (AC: #1, #3)
  - [x] Construct `patchMutatorEngine_` after `patchModel_` / `apvtsPatchMapper_` / `midiManager` (Epic 6 composition — may already exist)
  - [x] Pass engine + export folder picker lambda into `MutatorActionHandler` ctor
  - [x] Wire patch-load history clear (Story 6.13) in processor paths — **not** in handler; verify no regression

- [x] **Unit tests** (AC: #10)
  - [x] `Tests/Unit/MutatorActionHandlerTests.cpp` — recording fake `PatchMutatorEngine`
  - [x] Register in `Tests/CMakeLists.txt` if needed
  - [x] Run full `Matrix-Control_Tests`; build VST3 + Standalone Debug

- [x] **Self-review** (AC: #8, #9)
  - [x] No GUI diffs
  - [x] No mutation algorithm in handler
  - [x] Grep `PatchMutatorPanel` — no SysEx / engine calls added
  - [x] Handlers ≤ Clean Code limits

## Dev Notes

### What Story 7.4 IS — and what it is NOT

| In scope (7.4) | Out of scope |
|---|---|
| Wire `MutatorActionHandler` → `PatchMutatorEngine` | `MutationAlgorithm` spec/impl (**Epic 6.1–6.4**) |
| Debounced history selection routing (FR-57) | History M/R combobox GUI (**Epic 6.6**) |
| Footer propagation on blocked actions (FR-60) | Enabled-state button graying (**Epic 6.12** / UI polish) |
| EXPORT folder callback injection | `PatchFileService` implementation (**Epic 4** + **6.11**) |
| `MutatorActionHandlerTests` | Settings Defrag button wiring (**7.7** Phase B) |
| Processor construct/wiring | Clear history on patch load logic (**6.13** — processor) |
| | Defrag modal UI (**6.10**) |

### Epic 6 dependency map (minimum for 7.4)

| Epic 6 story | Required deliverable for 7.4 |
|---|---|
| 6.1 | `MutationAlgorithm` spec approved (input to engine) |
| 6.2 | `MutationHistoryStore` |
| 6.3 | `MutationNaming` |
| 6.4 | `PatchMutatorEngine::mutate()` |
| 6.5 | `PatchMutatorEngine::retry()` |
| 6.6 | APVTS `historyMList`, `historyRList`, `selectedM`, `selectedR`, `compareActive` |
| 6.7 | Debounced audition inside engine or debouncer callable from handler |
| 6.8 | `PatchMutatorEngine::toggleCompare()` |
| 6.9 | `deleteSelected()`, `clearHistory()` |
| 6.10 | Defrag from engine on limit (modal) — handler only forwards footer |
| 6.11 | `exportHistory(folder)` (may delegate to `PatchFileService`) |
| 6.12 | Recipe read + action enabled queries (engine-side) |
| 6.13 | Processor clears engine on patch load — verify integration |

**Sprint status:** Epic 6 entirely **backlog** — treat 7.4 as **integration story** blocked until Epic 6 lands or ships in parallel on same branch.

### Current brownfield state (read before editing)

**Handler stub** (`MutatorActionHandler.cpp`):

```cpp
void MutatorActionHandler::handleAction(const juce::String&, const juce::var&)
{
    // Story 7.4 + Epic 6 — PatchMutatorEngine delegation
}
```

**Processor wiring** (`PluginProcessor.cpp` ~268):

```cpp
mutatorActionHandler_ = std::make_unique<Core::MutatorActionHandler>();
// No PatchMutatorEngine injection yet
```

**Registry** (`ActionPropertyRegistry.cpp` lines 94–100) — six event IDs → `ActionHandlerKind::Mutator`:

| Property ID | C++ constant | Kind |
|---|---|---|
| `patchMutatorMutate` | `kMutate` | event (timestamp) |
| `patchMutatorRetry` | `kRetry` | event |
| `patchMutatorCompare` | `kCompare` | event |
| `patchMutatorDelete` | `kDelete` | event |
| `patchMutatorClear` | `kClear` | event |
| `patchMutatorExport` | `kExport` | event |

**NOT in registry** (correct — state/parameters, not actions):

| Property ID | Kind | Routed via |
|---|---|---|
| `patchMutatorAmount` | APVTS int parameter + state mirror | Panel slider → `setProperty` |
| `patchMutatorRandom` | APVTS int parameter + state mirror | Panel slider |
| `patchMutatorHistory` | **state** (legacy single combo — replaced by 6.6) | Processor listener when 6.6 lands |
| `patchMutatorEnableDco1`…`kEnableLfo2` | state toggles | Panel → `setProperty` bool |

**Panel** (`PatchMutatorPanel.cpp`): buttons use `connectButtonToApvts` → timestamp `setProperty`; **no business logic** — compliant with AD-5. History combo writes `kHistory` as **state** (selected id), not timestamp.

### PatchMutatorEngine API contract (Epic 6 — handler expects this)

Place under `Source/Core/Services/PatchMutator/` per architecture AD-6. Handler depends on **interface or concrete class** — prefer narrow public surface:

```cpp
namespace Core {

struct MutatorActionResult
{
    bool success = false;
    juce::String footerMessage;
    juce::String footerSeverity; // "info" | "warning" | "error"
    bool defragModalRequested = false; // FR-59 — processor/editor shows modal
};

class PatchMutatorEngine
{
public:
    MutatorActionResult mutate();
    MutatorActionResult retry();
    MutatorActionResult toggleCompare();
    MutatorActionResult deleteSelected();
    MutatorActionResult clearHistory();
    MutatorActionResult exportHistory(const juce::File& destinationFolder);
    MutatorActionResult defragHistory(); // manual Settings + modal confirm path

    void auditionSelectedHistoryEntry(); // called after debounce (FR-57)
    void syncHistoryUiProperties(juce::AudioProcessorValueTreeState& apvts);

    // Optional — engine updates uiMirror enabled flags (FR-60, Story 6.12)
    void refreshActionEnabledMirrors(juce::AudioProcessorValueTreeState& apvts);
};

} // namespace Core
```

Handler **must not** duplicate: buffer mutation, history store CRUD, naming, SysEx packing, or APVTS history list serialization — engine owns AD-6.

### Action → engine mapping

| User action | Handler method | Engine call | SysEx |
|---|---|---|---|
| MUTATE click | `handleMutate` | `mutate()` | 0x01 on success (engine) |
| RETRY click | `handleRetry` | `retry()` | 0x01 on success |
| COMPARE click | `handleCompare` | `toggleCompare()` | 0x01 when entering compare (engine) |
| DELETE click | `handleDelete` | `deleteSelected()` | optional audition after delete (engine) |
| CLEAR click | `handleClear` | `clearHistory()` | audition initial snapshot (engine) |
| EXPORT click | `handleExport` | folder picker → `exportHistory(folder)` | none (file I/O) |
| History M/R change | `onHistorySelectionChanged` | debounce → `auditionSelectedHistoryEntry()` | 0x01 after debounce |

### History debounce implementation guidance

Architecture: shared `kComboboxPatchSendDebounceMs`; same policy as Computer Patches (FR-52, FR-57).

**Option A (preferred):** `HistorySelectionDebouncer` in `Source/Core/Services/PatchMutator/` or `Source/Core/Util/` — handler owns instance; `onHistorySelectionChanged()` resets timer; callback invokes `engine_->auditionSelectedHistoryEntry()`.

**Option B:** Debounce inside `PatchMutatorEngine` — handler calls `engine->onHistorySelectionChanged()` directly.

Do **not** block message thread waiting for hardware. Do **not** send SysEx synchronously on every combobox `onChange`.

**Processor hook** (after Epic 6.6 property IDs exist in `PluginIDs.h`):

```cpp
// PluginProcessor::valueTreePropertyChanged — illustrative
if (parameterId == PatchMutatorModule::StateProperties::kSelectedM
    || parameterId == PatchMutatorModule::StateProperties::kSelectedR)
{
    mutatorActionHandler_->onHistorySelectionChanged();
    return;
}
```

Until 6.6 lands, guard with `#ifdef` or property-existence check — do not break build on legacy `kHistory` only.

### EXPORT folder picker injection

EXPORT requires user folder choice (FR-33). Core cannot include `juce_gui_basics` file chooser in handler.

**Pattern:** inject at construction:

```cpp
using ExportFolderPicker = std::function<juce::File()>;

MutatorActionHandler(..., PatchMutatorEngine* engine, ExportFolderPicker pickExportFolder);
```

`PluginProcessor` or `PluginEditor` supplies lambda wrapping `FileChooser` on message thread. If picker returns invalid file → no-op.

### Footer propagation pattern

Mirror `PatchManagerActionHandler::propagateRomBlockedFooter()`:

```cpp
apvts_.state.setProperty(PluginIDs::...::kUiMessageText, message, nullptr);
apvts_.state.setProperty(PluginIDs::...::kUiMessageSeverity, severity, nullptr);
```

Use English user-facing strings from `PluginDisplayNames` constants (add Mutator-specific blocked messages in `PluginDisplayNames.h` if engine returns generic codes).

Defrag limit modal (FR-59): engine sets `defragModalRequested` or posts separate APVTS event — **modal UI stays in GUI layer** (processor/editor listener). Handler does not call `AlertWindow`.

### MutatorActionHandler — dependency injection

| Dependency | Used for |
|---|---|
| `juce::AudioProcessorValueTreeState&` | footer properties |
| `PatchMutatorEngine*` | all mutation operations |
| `ExportFolderPicker` | EXPORT destination |
| `HistorySelectionDebouncer*` (optional) | FR-57 debounce |

Nullable engine pointer acceptable for unit tests; production processor must always inject real engine once Epic 6 complete.

### Architecture compliance

- Core **must not** include GUI headers (AD-1).
- `PluginProcessor` remains composition root (AD-2).
- Services in `Source/Core/Services/PatchMutator/` (AD-6).
- APVTS identifiers from `PluginIDs.h` only.
- Full patch replace → **0x01** only (AD-4); engine enqueues via `MidiManager`.
- One primary class per `.h/.cpp` pair.
- No global `AffineTransform` UI scaling.

### File structure

```
Source/Core/Services/PatchMutator/          (Epic 6 — NEW, prerequisite)
├── PatchMutatorEngine.h/.cpp
├── MutationHistoryStore.h/.cpp
├── MutationAlgorithm.h/.cpp
├── MutationNaming.h/.cpp
└── HistorySelectionDebouncer.h/.cpp        (optional — Epic 6.7 or 7.4)

Source/Core/Actions/
├── MutatorActionHandler.h/.cpp             (UPDATE — full wiring)
└── ActionPropertyRegistry.cpp              (no change expected)

Source/Core/
├── PluginProcessor.h/.cpp                  (UPDATE — engine + handler wiring, history listener)

Source/Shared/Definitions/
└── PluginDisplayNames.h                    (UPDATE — Mutator footer messages if needed)

Tests/Unit/
└── MutatorActionHandlerTests.cpp           (NEW)
```

Update `CMakeLists.txt` for test file; Epic 6 adds engine sources separately.

### Testing requirements

Follow `PatchManagerActionHandlerTests` / `ModuleActionHandlerTests` — `juce::UnitTest`, recording fakes, no `PluginProcessor`.

| Test | Assert |
|---|---|
| `mutate_delegatesToEngine` | Fake engine `mutate()` call count == 1 |
| `retry_delegatesToEngine` | Fake engine `retry()` invoked |
| `compare_delegatesToEngine` | `toggleCompare()` invoked |
| `delete_delegatesToEngine` | `deleteSelected()` invoked |
| `clear_delegatesToEngine` | `clearHistory()` invoked |
| `export_invokesPickerThenEngine` | Picker called; `exportHistory(folder)` with returned path |
| `export_cancelledPicker_noEngineCall` | Invalid file → no export |
| `mutate_blocked_setsFooter` | Engine returns failure → `uiMessageText` set |
| `historySelection_debounced` | Rapid calls → single audition after debounce window |

Manual smoke (after Epic 6 + 7.4):

1. MUTATE → new M00 in history + 0x01 on MIDI monitor.
2. RETRY → M00-R00 + 0x01.
3. Rapid History M scroll → single 0x01 after pause.
4. COMPARE → initial snapshot auditions; History combos grayed (6.8 UI).
5. DELETE root → cascade footer notice.
6. EXPORT → folder layout per FR-33.
7. Full roots (100) → footer / Defrag modal (6.10).

### Previous story intelligence (7.3 / 7-3b)

- Handler DI pattern: inject services via ctor; processor constructs dependencies before handler.
- Footer warnings: dedicated `propagate*Footer()` private methods + `PluginDisplayNames` constants.
- `ActionExecutionHooks` / suppress flags: Mutator bulk APVTS push after mutate may need `setSuppressPatchSysEx` + `setSuppressMatrixModSysEx` — **engine** should accept hooks or call mapper internally; if engine pushes APVTS, mirror PatchManager INIT pattern from 7.3.
- Review deferred from 7-3b: duplicate MIDI on navigation — unrelated to Mutator; do not copy suppress listener workaround unless mutate triggers similar double-sync.

### Previous story intelligence (7.1)

- Mutator stub intentionally empty; registry already routes six IDs.
- `ActionDispatcherTests::testDispatcherRoutesMutatorMutate` — routing test exists; extend with handler unit tests, not dispatcher changes.
- Amount/Random/enables **not** action properties — do not register in `ActionPropertyRegistry`.

### Git intelligence (recent commits)

| Commit | Relevance |
|---|---|
| `a1547e6` | Stories 7.3 + 7-3b done — handler DI, footer, tests pattern to mirror |
| `a1e1bbc` | Story 7.2 — `ActionExecutionHooks`, suppress during bulk APVTS |
| `0c7a3c2` | Story 7.1 — `MutatorActionHandler` stub + registry |

### Latest tech / framework notes

- **JUCE 8.0.12** — `FileChooser` on message thread only; async chooser API if used from editor callback.
- **C++17** — no new third-party libraries.
- Action properties remain timestamp `juce::var` int64 — handler ignores `newValue` body (same as other handlers).
- Debounce timer: `juce::Timer` or `juce::MultiTimer` in Core — no GUI timer.

### Project context reference

- `project-context.md` — C++17, builds under `Builds/`, English source only, no GUI in Core.
- `CONVENTIONS.md` — authoritative style; Clean Code function length.
- Addendum § Patch Mutator (D-082–D-087) — data model, EXPORT layout, Defrag rules.

### References

- [epics.md — Story 7.4, Epic 6 stories 6.1–6.13]
- [prd.md — FR-30–FR-34, FR-54–FR-60]
- [addendum.md — Patch Mutator §, SysEx & debounce]
- [architecture.md — AD-5, AD-6, debounce policy]
- [7-1-actiondispatcher-and-handler-interfaces.md — registry, stub contract]
- [7-3-patchmanageractionhandler-bank-and-internal.md — handler DI, footer, tests]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Build: `cmake --build Builds/macOS --target Matrix-Control_Tests Matrix-Control_VST3 Matrix-Control_Standalone`
- Run: `Builds/macOS/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` — all tests green

### Completion Notes List

- Epic 6 gate satisfied — `PatchMutatorEngine` fully implemented; full wiring (not scaffold-only).
- Implemented `MutatorActionHandler` with DI: `PatchMutatorEngine*`, APVTS footer propagation, `ExportFolderPicker`, optional `DefragLimitModalGate`, and `ComboboxPatchSendDebouncer` for history audition (FR-57).
- `handleAction` dispatches all six Mutator event IDs to engine methods; blocked actions propagate `uiMessageText` / `uiMessageSeverity`.
- `PluginProcessor` injects engine + export-folder lambda (`setMutatorExportFolderPicker` for editor wiring); routes `selectedM` / `selectedR` changes to `onHistorySelectionChanged()`.
- Added 9 unit tests in `MutatorActionHandlerTests.cpp`; full suite + VST3/Standalone Debug builds green.
- No GUI changes. Defrag limit modal gate is injectable but not wired in editor yet (follow-up when modal UI lands).

### File List

- `Source/Core/Actions/MutatorActionHandler.h` (modified)
- `Source/Core/Actions/MutatorActionHandler.cpp` (modified)
- `Source/Core/PluginProcessor.h` (modified)
- `Source/Core/PluginProcessor.cpp` (modified)
- `Tests/Unit/MutatorActionHandlerTests.cpp` (new)
- `CMakeLists.txt` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)

## Change Log

- 2026-06-19: Story 7.4 created — MutatorActionHandler wiring guide with Epic 6 prerequisite and API contract.
- 2026-06-20: Story 7.4 implemented — handler wiring, processor integration, debounced history audition, unit tests.
- 2026-06-20: Code review fixes — export/defrag editor wiring, history debounce suppress hook, recording fake engine tests.

### Review Findings

- [x] [Review][Decision] **Gate modal Defrag non branchée en production** — Résolu : stub `AlertWindow` branché via `setMutatorDefragLimitModalGate` dans `PluginEditor`.
- [x] [Review][Decision] **Tests avec moteur réel vs fake enregistreur** — Résolu : `RecordingPatchMutatorEngine` (spy) + assertions sur compteurs d'appels.
- [x] [Review][Patch] **EXPORT inopérant : picker jamais branché dans l'éditeur** — Corrigé : `setMutatorExportFolderPicker` dans `PluginEditor.cpp`.
- [x] [Review][Patch] **Audition SysEx en double après sync moteur → APVTS** — Corrigé : hook `setSuppressMutatorHistorySelectionDebounce` + garde dans `syncHistoryUiProperties` / compare exit.
- [x] [Review][Patch] **Test `export_cancelledPicker` ne prouve pas l'absence d'appel moteur** — Corrigé : `exportCallCount == 0`.
- [x] [Review][Patch] **Tests MUTATE/RETRY sans assertion SysEx 0x01** — Remplacé par assertions `mutateCallCount` / `retryCallCount == 1` (contrat handler, conforme fake spy).
- [x] [Review][Defer] **`actionHooks.onPatchLoaded` copié avant assignation** [`Source/Core/PluginProcessor.cpp:238`] — deferred, pre-existing
