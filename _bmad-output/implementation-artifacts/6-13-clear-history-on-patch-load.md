---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.13 — Clear History on Patch Load
author: BMad Agent
status: done
baseline_commit: 57478cb
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/6-9-delete-and-clear-history.md
  - implementation-artifacts/6-12-recipe-persistence-and-action-enabled-states.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - implementation-artifacts/4-5-import-name-reconciliation.md
  - implementation-artifacts/4-6-previous-and-next-file-navigation.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.13: Clear History on Patch Load

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want Mutator history cleared when I load a new patch,
so that history always belongs to the current patch context (FR-31).

## Acceptance Criteria

1. **Given** Stories 6.2–6.12 and AD-7 persistence boundaries **When** a patch loads from a **Computer Patches `.syx` file** (including Previous/Next navigation that triggers load) **Then** in-memory `MutationHistoryStore` is empty **And** initial snapshot is cleared (`hasInitialSnapshot() == false`) **And** APVTS history mirrors reset to empty-history defaults (`kHistoryMList`/`kHistoryRList` empty, `kSelectedM = -1`, `kSelectedR = kSelectedRRootOnly`, `kCompareActive = false`) **And** action enabled mirrors reflect empty history (mutate enabled; retry/export/delete/clear disabled per FR-60).

2. **And** `PatchMutatorEngine::resetSessionForPatchLoad()` implements the reset — **not** `clearHistory()`:
   - Calls `forceExitCompare()` (no toggleCompare exit branch — same pattern as delete/clear/defrag)
   - Calls `historyStore_.clear()` (roots/retries only — unchanged 6.2 semantics)
   - Calls new `historyStore_.clearInitialSnapshot()` (6.13 adds this API)
   - Resets `selectedRootIndex_ = -1`, `selectedRetryIndex_ = kRootOnly`
   - Calls `syncHistoryUiProperties(apvts_)` (includes `refreshActionEnabledMirrors`)
   - **Does not** call `auditionAfterHistoryMutation()` — loaded patch is already in `PatchModel`/APVTS; load path sends SysEx separately

3. **And** `MutationHistoryStore::clearInitialSnapshot()` clears `hasInitialSnapshot_` and zeroes `initialSnapshot_` buffer **And** `clear()` behaviour is unchanged — still preserves initial snapshot (6.2 AC #3, 6.9 CLEAR semantics).

4. **And** `clearHistory()` (user CLEAR action) **still preserves** initial snapshot and still calls `auditionAfterHistoryMutation()` — no regression to Story 6.9.

5. **And** recipe state is **unchanged** on patch load: Amount, Random, and all ten module enable toggles retain current APVTS values (FR-34 boundary vs FR-31).

6. **And** COMPARE is disabled after patch load until the next successful MUTATE captures a new initial snapshot (FR-32: compare disabled when history empty; panel `refreshCompareUiState` already keys off `kCompareActive` + empty history list).

7. **And** patch-load hook is wired at the existing FR-31 anchor in `PatchManagerActionHandler::applyLoadedPatchToApvtsAndSynth()` — replace the comment with a call to processor-provided callback **after** `syncLoadedPatchToApvts()` and **before** `sendPatch` to synth.

8. **And** wiring uses `ActionExecutionHooks::onPatchLoaded` (`std::function<void()>`) set in `PluginProcessor` to delegate to `patchMutatorEngine_->resetSessionForPatchLoad()` — **minimal** `PatchMutatorEngine` construction in processor is **in scope** for this story (composition-root start; full `MutatorActionHandler` delegation remains **7.4**).

9. **And** `requestCurrentPatch` synth-load path: **no call sites exist today** — document hook contract only; when a future story loads packed bytes from synth into `PatchModel`, it **must** invoke the same `onPatchLoaded` callback. **Out of scope:** bank/patch navigation that only sends MIDI Program Change without loading editor bytes; Internal Patches INIT/PASTE.

10. **And** patch load reset is **distinct** from DAW session reload (Story 6.12 `resetEphemeralMutatorStateAfterSessionLoad`) — session reload resets APVTS mirrors only (no engine instance today); patch load resets live engine store when wired.

11. **And** unit tests:
    - `PatchMutatorEngineTests`: `resetSessionForPatchLoad` clears history + initial snapshot + compare; recipe properties untouched; no SysEx audition; enabled mirrors empty-history matrix
    - Regression: `clear_keepsInitialSnapshot` still passes
    - `PatchManagerActionHandlerTests`: load success invokes `onPatchLoaded` callback (mock hook)

12. **And** this story delivers **engine reset API + store snapshot clear + processor hook wiring + tests only**:
    - **No** `MutatorActionHandler` button delegation (**7.4**)
    - **No** `requestCurrentPatch` user flow implementation
    - **No** INIT/PASTE as patch-load triggers
    - **No** panel changes (panel already reacts to APVTS mirrors)

## Tasks / Subtasks

- [x] **Add `clearInitialSnapshot()` to store** (AC: #3, #4)
  - [x] `MutationHistoryStore.h` / `.cpp`
  - [x] Unit test in `MutationHistoryStoreTests.cpp` if not covered via engine test

- [x] **Implement `resetSessionForPatchLoad()`** (AC: #2, #4, #6)
  - [x] Declare in `PatchMutatorEngine.h`
  - [x] Implement in `PatchMutatorEngine.cpp` (≤15 lines — extract helper if needed)
  - [x] Do **not** modify `clearHistory()` initial-snapshot policy

- [x] **Wire processor + handler hook** (AC: #7, #8, #10)
  - [x] Add `onPatchLoaded` to `ActionExecutionHooks.h`
  - [x] Construct `PatchMutatorEngine` in `PluginProcessor` (mirror 7.4 ctor args from `PatchMutatorEngineTests::EngineHarness`)
  - [x] Set `actionHooks.onPatchLoaded` before `PatchManagerActionHandler` construction
  - [x] Call hook in `applyLoadedPatchToApvtsAndSynth` after `syncLoadedPatchToApvts()`

- [x] **Unit tests** (AC: #11)
  - [x] Engine: reset after mutate/compare/populated history
  - [x] Handler: mock `onPatchLoaded` fired on successful `.syx` load
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

- [x] **Self-review**
  - [x] Grep — hook called from exactly one load completion path today
  - [x] Grep — recipe property IDs not touched in reset path
  - [x] Verify `clearHistory` regression tests green

## Dev Notes

### What Story 6.13 IS — and what it is NOT

| In scope (6.13) | Out of scope |
|---|---|
| FR-31 patch-load history clear | DAW session reload (6.12 — separate hook) |
| Initial snapshot reset on patch load | CLEAR button preserving snapshot (6.9) |
| `resetSessionForPatchLoad()` engine API | `MutatorActionHandler` wiring (7.4) |
| `onPatchLoaded` hook + minimal engine in processor | Full handler delegation (7.4) |
| Computer `.syx` load path (incl. Prev/Next) | `requestCurrentPatch` RPC flow (no call sites) |
| Engine + handler callback tests | Panel/GUI changes |
| COMPARE forced off via `forceExitCompare` | Dirty-patch confirmation on load (Epic 9) |

**Critical distinction — three reset paths:**

| Trigger | API | Clears M/R history | Clears initial snapshot | Audition SysEx | Recipe |
|---|---|---|---|---|---|
| User **CLEAR** | `clearHistory()` | Yes | **No** | Yes (initial snapshot) | Unchanged |
| **Patch load** (.syx) | `resetSessionForPatchLoad()` | Yes | **Yes** | **No** (load sends patch) | Unchanged |
| **DAW session reload** | `resetEphemeralMutatorStateAfterSessionLoad()` | APVTS mirrors only | N/A (store empty until engine wired) | No | Restored from XML |

### `resetSessionForPatchLoad()` algorithm

```
resetSessionForPatchLoad():
  forceExitCompare()
  historyStore_.clear()
  historyStore_.clearInitialSnapshot()
  selectedRootIndex_ = -1
  selectedRetryIndex_ = kRootOnly
  syncHistoryUiProperties(apvts_)
  return { success: true }
```

**Do not** call `auditionAfterHistoryMutation()` — `applyLoadedPatchToApvtsAndSynth` already calls `midiManager_->sendPatch(...)` with the newly loaded buffer.

### Existing hook (replace comment)

```522:530:Source/Core/Actions/PatchManagerActionHandler.cpp
    void PatchManagerActionHandler::applyLoadedPatchToApvtsAndSynth(const DeviceMemoryLimits& limits)
    {
        syncLoadedPatchToApvts();

        // FR-31 hook: MutationHistoryStore::clear() on patch load (Epic 6.13).

        if (midiManager_ != nullptr)
            midiManager_->sendPatch(static_cast<juce::uint8>(getCurrentPatch(limits)), patchModel_->data());
    }
```

**Replace with:**

```cpp
syncLoadedPatchToApvts();

if (hooks_.onPatchLoaded)
    hooks_.onPatchLoaded();

if (midiManager_ != nullptr)
    midiManager_->sendPatch(...);
```

**Load entry points that reach this method today:**

| User action | Handler path |
|---|---|
| Computer Patches combo selection | `handleLoadSelectedPatchFile` |
| LOAD PREVIOUS / LOAD NEXT (when selection wraps) | `advanceComputerPatchesSelection` → `handleLoadSelectedPatchFile` |

Stories 4.5 and 4.6 explicitly deferred mutator clear to this hook — do not add duplicate calls elsewhere.

### Patch load vs synth navigation (FR-31 wording)

PRD FR-31: *"cleared on new patch load from synth or `.syx` file"*.

**Brownfield reality:**

- **`.syx` load** — `patchModel_->loadFrom(packed)` in `decodeAndReconcilePatchFile` → confirmed in-scope.
- **Synth load** — `MidiManager::requestCurrentPatch()` exists but has **zero production call sites**. Bank/patch navigation (`applyPatchCoordinates`) updates coordinates and sends MIDI sync; it does **not** load packed bytes into `PatchModel`.
- **Future contract:** any code path that loads a new packed patch into `PatchModel` from synth RPC must call `hooks_.onPatchLoaded` (or `resetSessionForPatchLoad()` via engine pointer) — same as `.syx` load.

**Explicitly NOT patch load for FR-31:**

- Internal Patches **INIT** (`handleInternalPatchInit`) — template init, not user "load"
- Internal Patches **PASTE** — clipboard paste
- Bank/patch coordinate change without editor byte sync

### Minimal processor engine wiring (6.13 scope, 7.4 continues)

Story 7.4 defers general processor/handler wiring but lists patch-load clear as **6.13** ownership. Instantiate engine using the same dependency pattern as tests:

```152:175:Tests/Unit/PatchMutatorEngineTests.cpp
    struct EngineHarness
    {
        TestAudioProcessorMutator proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        // ... MidiManager, PatchFileService, SysExEncoder ...
        Core::PatchMutatorEngine engine;
    };
```

Processor already owns `patchModel_`, `apvtsPatchMapper_`, `patchNameSyncer_`, `midiManager`, `patchFileService_`, `sysExEncoder_`. Add:

```cpp
std::unique_ptr<Core::PatchMutatorEngine> patchMutatorEngine_;

// In ctor, after dependencies exist:
patchMutatorEngine_ = std::make_unique<Core::PatchMutatorEngine>(
    patchModel_.get(),
    apvtsPatchMapper_.get(),
    patchNameSyncer_.get(),
    midiManager.get(),
    apvts,
    actionHooks,
    [this]() { return getCurrentPatchNumberForMutator(); },  // extract lambda like 7.4 notes
    patchFileService_.get(),
    sysExEncoder_.get());

actionHooks.onPatchLoaded = [this]() {
    if (patchMutatorEngine_ != nullptr)
        patchMutatorEngine_->resetSessionForPatchLoad();
};
```

**7.4 handoff:** reuse `patchMutatorEngine_` instance — inject into `MutatorActionHandler` when implementing delegation. Do **not** create a second engine.

### `clearInitialSnapshot()` on store

6.2 AC #3 mandates `clear()` does not touch initial snapshot. Add sibling API:

```cpp
void MutationHistoryStore::clearInitialSnapshot() noexcept
{
    hasInitialSnapshot_ = false;
    initialSnapshot_.fill(0);
}
```

No change to `clear()` — existing test `clear_keepsInitialSnapshot` must remain green.

### APVTS / UI effects after reset

`syncHistoryUiProperties` empty-store branch already sets:

- `kHistoryMList` / `kHistoryRList` → empty → panel shows `<EMPTY>` / disabled R combo
- `kSelectedM = -1`, `kSelectedR = kSelectedRRootOnly`
- `refreshActionEnabledMirrors` → FR-60 empty-history matrix

`forceExitCompare()` sets `kCompareActive = false` → panel `refreshCompareUiState` disables compare button when history empty:

```492:501:Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
void PatchMutatorPanel::refreshCompareUiState()
{
    const bool compareActive = static_cast<bool>(apvts_.state.getProperty(MutatorState::kCompareActive,
                                                                         false));
    // ...
    if (compareButton_ != nullptr)
        compareButton_->setEnabled(compareActive || ! historyEmpty);
```

No panel edits required — engine/processor hook drives existing listeners.

### Session reload vs patch load (6.12 boundary)

6.12 `setStateInformation` calls `resetEphemeralMutatorStateAfterSessionLoad` + `setActionEnabledMirrorsForEmptyHistory` because engine was not in processor. After 6.13:

- **Session reload:** keep 6.12 APVTS reset; also call `patchMutatorEngine_->resetSessionForPatchLoad()` if engine exists (store may be stale if user mutated before save). Consider consolidating both paths through `onPatchLoaded` or a shared `resetMutatorSession()` helper — **optional refactor**; minimum fix: invoke engine reset from `resetEphemeralMutatorStateAfterSessionLoad` path in processor after `replaceState`.

**Recommended (single story):** In `PluginProcessor::resetEphemeralMutatorStateAfterSessionLoad()`, after APVTS mirror reset, call `patchMutatorEngine_->resetSessionForPatchLoad()` when engine non-null — keeps RAM store aligned with APVTS on DAW reload. Recipe still restored from XML before this call (6.12 order preserved).

### File structure (this story)

```
Source/Core/Actions/
├── ActionExecutionHooks.h                   (UPDATE — onPatchLoaded)
└── PatchManagerActionHandler.cpp            (UPDATE — invoke hook)

Source/Core/
├── PluginProcessor.h                        (UPDATE — patchMutatorEngine_ member)
├── PluginProcessor.cpp                      (UPDATE — construct engine, set hook, session-load sync)
└── Services/PatchMutator/
    ├── MutationHistoryStore.h               (UPDATE — clearInitialSnapshot)
    ├── MutationHistoryStore.cpp             (UPDATE)
    ├── PatchMutatorEngine.h                 (UPDATE — resetSessionForPatchLoad)
    └── PatchMutatorEngine.cpp               (UPDATE)

Tests/Unit/
├── PatchMutatorEngineTests.cpp              (UPDATE — reset + regression)
├── PatchManagerActionHandlerTests.cpp       (UPDATE — onPatchLoaded fired)
└── MutationHistoryStoreTests.cpp            (UPDATE — optional clearInitialSnapshot)
```

No CMake changes expected (engine/store already registered).

### Testing requirements

**PatchMutatorEngineTests** — add:

| Test | Setup | Assert |
|---|---|---|
| `reset_afterMutate_clearsHistoryAndSnapshot` | mutate success | store empty; `!hasInitialSnapshot()`; compare false |
| `reset_afterCompareActive` | mutate → compare on | compare false; saved M/R cleared |
| `reset_preservesRecipe` | set Amount/Random/toggles | unchanged after reset |
| `reset_noAuditionSysEx` | mutate (SysEx count > 0) | reset adds no sendPatch |
| `reset_enabledMirrorsEmptyHistory` | after mutate | mutate=T, retry/export/delete/clear=F |
| `clear_keepsInitialSnapshot` | (existing) | still passes |

**PatchManagerActionHandlerTests** — extend load harness:

1. Set `hooks.onPatchLoaded = [&]() { callbackInvoked = true; }`
2. Load valid `.syx` via `kSelectPatchFile`
3. Assert callback invoked once

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **FR-31** — session-only history; cleared on patch load
- **FR-32** — compare disabled when history empty / no snapshot
- **FR-34** — recipe independent of history reset
- **AD-7** — initial snapshot RAM-only; cleared on patch load; recipe in session XML
- **AD-5** — handler remains thin; hook callback only
- **Core ↛ GUI** — engine sets APVTS properties; panel listens
- English source/comments only
- Functions ≤ 15 lines — extract if orchestration grows

### Previous story intelligence (6.12 — done)

- `MutatorSessionPersistence` helpers for APVTS ephemeral reset — reuse for session reload alignment, not for patch-load store clear
- `refreshActionEnabledMirrors` hooked at end of `syncHistoryUiProperties` — patch-load reset inherits automatically
- Review deferred engine store clear on session load — **resolve in 6.13** when engine constructed

### Previous story intelligence (6.9 — done)

- `clearHistory()` preserves initial snapshot by design — **do not change**
- `forceExitCompare()` shared helper — reuse in `resetSessionForPatchLoad`
- `auditionAfterHistoryMutation()` after clear — **omit** on patch load

### Previous story intelligence (6.8 — done)

- Initial snapshot captured on first MUTATE only
- Deferred: *"Initial snapshot not reset on patch load"* — **this story closes it**
- Compare panel UX driven by `kCompareActive` + empty history — no panel work

### Previous story intelligence (6.2 — done)

- `clear()` vs initial snapshot separation — extend with `clearInitialSnapshot()` without breaking AC #3

### Git intelligence

| Commit | Relevance |
|---|---|
| `57478cb` | Story 6.12 — session persistence, enabled mirrors, `MutatorSessionPersistence` |
| `b725b26` | Story 6.11 — export uses initial snapshot; must be absent after patch load until mutate |
| `8b2d916` | Story 6.9 — `clearHistory()` snapshot preservation |
| `87a2a97` | Story 6.8 — compare + initial snapshot capture |

**Brownfield gap this story closes:**

- FR-31 comment stub at `PatchManagerActionHandler.cpp:526`
- Initial snapshot survives patch load (deferred from 6.8 review)
- No `PatchMutatorEngine` in processor — history mutations in tests only

### Story 7.4 handoff

- Reuse `patchMutatorEngine_` — inject into `MutatorActionHandler`
- `onPatchLoaded` hook already wired — 7.4 verifies no regression when handler calls mutate/retry
- Session reload + patch load both leave engine in consistent empty state

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `std::function` hook, existing `ActionExecutionHooks` pattern
- No new dependencies; no web research required

### Project context reference

- `project-context.md` — Core/GUI separation; APVTS taxonomy
- `CONVENTIONS.md` — no French in source; Patch Manager handler owns file load
- FR-31, FR-32, FR-34; AD-7; D-085 initial snapshot semantics

### References

- [epics.md — Story 6.13]
- [prd.md — FR-31, FR-32, FR-34]
- [addendum.md — History session-only §]
- [architecture.md — AD-7 persistence boundaries]
- [6-2-mutationhistorystore-two-level-mr.md — clear() vs initial snapshot]
- [6-8-compare-mode.md — snapshot reset deferral]
- [6-9-delete-and-clear-history.md — resetSessionForPatchLoad naming]
- [6-12-recipe-persistence-and-action-enabled-states.md — session vs patch load boundary]
- [7-4-mutatoractionhandler.md — engine reuse, 6.13 ownership]
- [4-5-import-name-reconciliation.md — FR-31 hook deferral]
- [4-6-previous-and-next-file-navigation.md — load path deferral]
- [PatchManagerActionHandler.cpp — applyLoadedPatchToApvtsAndSynth]
- [PatchMutatorEngine.cpp — clearHistory, forceExitCompare, syncHistoryUiProperties]
- [MutatorSessionPersistence.h — session reload helpers]

## Dev Agent Record

### Agent Model Used

Claude (Cursor Agent)

### Debug Log References

- Built and ran `Matrix-Control_Tests` — all tests green including new reset/hook tests and `clear_keepsInitialSnapshot` regression.

### Completion Notes List

- Added `MutationHistoryStore::clearInitialSnapshot()` — sibling to `clear()`; preserves 6.2 CLEAR semantics.
- Implemented `PatchMutatorEngine::resetSessionForPatchLoad()` — forceExitCompare, clear history + snapshot, sync APVTS mirrors; no audition SysEx.
- Wired `ActionExecutionHooks::onPatchLoaded` from `applyLoadedPatchToApvtsAndSynth` (single call site); processor constructs `patchMutatorEngine_` and delegates hook.
- Session reload (`resetEphemeralMutatorStateAfterSessionLoad`) also resets live engine store when present (6.12 alignment).
- Tests: engine reset matrix (5 cases), store `clearInitialSnapshot`, handler `loadSelected_invokesOnPatchLoaded`.

### File List

- Source/Core/Actions/ActionExecutionHooks.h
- Source/Core/Actions/PatchManagerActionHandler.cpp
- Source/Core/PluginProcessor.h
- Source/Core/PluginProcessor.cpp
- Source/Core/Services/PatchMutator/MutationHistoryStore.h
- Source/Core/Services/PatchMutator/MutationHistoryStore.cpp
- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Tests/Unit/MutationHistoryStoreTests.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp
- Tests/Unit/PatchManagerActionHandlerTests.cpp
- _bmad-output/implementation-artifacts/sprint-status.yaml

### Change Log

- 2026-06-20: Story 6.13 — FR-31 patch-load history clear via `resetSessionForPatchLoad`, `onPatchLoaded` hook, minimal processor engine wiring, unit tests.

### Review Findings

- [x] [Review][Patch] Sprint status metadata contradicts itself [`sprint-status.yaml`:2-38] — fixed: `last_updated` aligned with story status.
- [x] [Review][Patch] Engine reset tests omit APVTS history mirror assertions [`PatchMutatorEngineTests.cpp`] — fixed: `reset_afterMutate_clearsHistoryAndSnapshot` asserts empty M/R lists and selection reset.

- [x] [Review][Defer] Hook contract for future synth load not documented in source [`ActionExecutionHooks.h`:14] — deferred, pre-existing; AC #9 contract documented in story; optional source comment for future `requestCurrentPatch` paths.
- [x] [Review][Defer] Handler hook test covers select-file load only [`PatchManagerActionHandlerTests.cpp`] — deferred, pre-existing; Prev/Next share `applyLoadedPatchToApvtsAndSynth` — extra path test is polish.
- [x] [Review][Defer] No PluginProcessor integration test for engine + hook wiring [`PluginProcessor.cpp`] — deferred, pre-existing; story AC #12 scopes unit tests only.
- [x] [Review][Defer] `getCurrentPatchNumberForMutator()` silent fallback to patch 0 [`PluginProcessor.cpp`:1508] — deferred, pre-existing; same pattern planned for 7.4 handler wiring.
