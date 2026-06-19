---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.8 — COMPARE Mode
author: BMad Agent
status: done
baseline_commit: 9a9eaef
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/6-7-history-selection-audition-with-debounce.md
  - implementation-artifacts/6-6-history-m-and-r-ui-properties.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.8: COMPARE Mode

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want COMPARE to toggle the initial snapshot against current mutations,
so that I hear the original patch (FR-32, UX-DR4).

## Acceptance Criteria

1. **Given** Story 6.2 (`MutationHistoryStore::setInitialSnapshot` / `getInitialSnapshot` / `hasInitialSnapshot`) and Story 6.7 (`auditionSelectedHistoryEntry` with `kCompareActive` early-return) **When** `PatchMutatorEngine::toggleCompare()` is implemented **Then** it toggles `kCompareActive` on APVTS state and:
   - **Enter compare** (was `false` → `true`): requires non-empty history (`!historyStore_.isEmpty()`); requires `historyStore_.hasInitialSnapshot()`; saves current `selectedM` / `selectedR` internally for restore; sets `kCompareActive = true`; auditions **initial snapshot** via `pushResultToEditorAndSynth(getInitialSnapshot())` (one full patch SysEx **0x01**); PATCH NAME follows initial snapshot bytes (pre-mutation name — D-085).
   - **Exit compare** (was `true` → `false`): sets `kCompareActive = false`; restores saved `selectedM` / `selectedR` to APVTS; auditions restored selection via `pushResultToEditorAndSynth(resolveAuditionBuffer())` after `applySelectionFromApvts()` (one **0x01**); PATCH NAME follows restored entry.
   - **Blocked enter** when history empty: returns `{ success: false, footerMessage, footerSeverity: warning }` — no property change, no SysEx.
   - **Blocked enter** when `!hasInitialSnapshot()`: same failure path (should not occur after first successful MUTATE — see AC #2).

2. **And** **initial snapshot capture** on first MUTATE: in `mutate()`, **before** algorithm runs, when `!historyStore_.hasInitialSnapshot()`, call `historyStore_.setInitialSnapshot(auditionBuffer)` where `auditionBuffer = resolveAuditionBuffer()` (live editor patch on empty history). Captures patch **as loaded before any MUTATE** per FR-32 / D-085. Subsequent MUTATE calls do **not** overwrite initial snapshot. RETRY does **not** set initial snapshot.

3. **And** while `kCompareActive == true`:
   - `auditionSelectedHistoryEntry()` remains a no-op (6.7 guard — do not remove).
   - History M/R selection properties (`kSelectedM`, `kSelectedR`) are **preserved** in APVTS for restore — engine must **not** clear them on enter.
   - `syncHistoryUiProperties()` does **not** auto-exit compare or change selection.

4. **And** `PatchMutatorPanel` reacts to `kCompareActive` (extend `valueTreePropertyChanged`):
   - **History M** and **History R** comboboxes `setEnabled(false)` when compare active; re-enable when compare exits (respect existing empty-history disabled state).
   - **COMPARE button** blinks while active (UX-DR4, D-029): private `juce::Timer` on panel at **2 Hz** toggling `compareButton_` alpha between `1.0f` and `0.35f`; timer stopped and alpha reset to `1.0f` on exit; button stays **clickable** (do not `setEnabled(false)` on compare button while active — user must re-click to exit).
   - **COMPARE button** `setEnabled(false)` when history empty (`kHistoryMList` empty or `kSelectedM == -1`); enabled when at least one root exists.

5. **And** **no synth bank STORE during compare** (FR-32, FR-45): `InternalPatchesPanel::updatePasteStoreEnabled` (or equivalent) also disables **STORE** when `kCompareActive == true` (`storePatchButton_->setEnabled(allowPasteStore && !compareActive)`). PASTE rule unchanged. Panel must listen to `kCompareActive` in `valueTreePropertyChanged`.

6. **And** unit tests in `Tests/Unit/PatchMutatorEngineTests.cpp` cover compare without GUI:
   - `mutate_firstRoot_capturesInitialSnapshot` — empty history, distinct editor buffer, successful mutate → `hasInitialSnapshot()` true; snapshot bytes match pre-mutate editor buffer (not M00 result).
   - `compare_emptyHistory_blocked` — `toggleCompare()` on empty store → `success == false`, `kCompareActive` stays false, zero 0x01.
   - `compare_enter_auditionsInitialSnapshot` — after mutate, `toggleCompare()` → one 0x01; editor buffer matches initial snapshot.
   - `compare_enter_setsCompareActive` — APVTS `kCompareActive == true` after enter.
   - `compare_exit_restoresSelection` — enter compare with M00+R00 selected, exit → `kSelectedM`/`kSelectedR` restored to pre-enter values.
   - `compare_exit_auditionsRestoredEntry` — exit compare → editor buffer matches restored history entry (not initial snapshot).
   - `compare_auditionBlockedWhileActive` — with compare active, `auditionSelectedHistoryEntry()` → no additional 0x01.
   - `mutate_secondRoot_doesNotOverwriteInitialSnapshot` — two mutates → initial snapshot still equals pre-first-mutate buffer.

7. **And** this story delivers **engine `toggleCompare` + initial snapshot capture in `mutate` + panel compare UX + Internal Patches STORE guard + unit tests** — no `MutatorActionHandler` COMPARE wiring (**7.4**), no DELETE/CLEAR compare side-effects (**6.9**), no full FR-60 enabled-state mirrors for MUTATE/RETRY/EXPORT/DELETE (**6.12**), no clear-on-patch-load initial-snapshot reset (**6.13**). Story 7.4 delegates `handleCompare` → `toggleCompare()` and relies on engine-updated `kCompareActive` for panel graying (7.4 AC #6).

8. **And** `CMakeLists.txt` unchanged unless new `.cpp` added; full `Matrix-Control_Tests` target remains green.

## Tasks / Subtasks

- [x] **Initial snapshot capture in `mutate()`** (AC: #2)
  - [x] Before algorithm, when `!hasInitialSnapshot()`, `setInitialSnapshot(auditionBuffer)`
  - [x] Add test `mutate_firstRoot_capturesInitialSnapshot` + `mutate_secondRoot_doesNotOverwriteInitialSnapshot`

- [x] **Implement `toggleCompare()`** (AC: #1, #3)
  - [x] Replace empty stub at `PatchMutatorEngine.cpp:272-276`
  - [x] Add private members `compareSavedM_`, `compareSavedR_` (or stack locals saved only on enter)
  - [x] Enter/exit branches with `pushResultToEditorAndSynth` reuse — no forked SysEx path
  - [x] Blocked paths return footer message (reuse existing footer constant pattern from mutate/retry)

- [x] **Panel compare UX** (AC: #4)
  - [x] `valueTreePropertyChanged`: handle `kCompareActive` → `refreshCompareUiState()`
  - [x] `refreshCompareUiState()`: gray history combos, start/stop blink timer, update compare button enabled
  - [x] Destructor: `stopTimer()` on blink timer
  - [x] Also refresh compare enabled state when `kHistoryMList` / `kSelectedM` changes

- [x] **Internal Patches STORE guard** (AC: #5)
  - [x] `InternalPatchesPanel`: listen `kCompareActive`; fold into `updatePasteStoreEnabled`

- [x] **Unit tests — engine** (AC: #6)
  - [x] Extend `PatchMutatorEngineTests` with compare cases
  - [x] Reuse `EngineHarness`, `countPatchSysExMessages`, `makeDistinctBuffer` from 6.4–6.7

- [x] **Self-review** (AC: #7, #8)
  - [x] Grep `toggleCompare` — defined in engine; not called from handler yet (7.4)
  - [x] Grep `kCompareActive` — engine writes; panel + InternalPatchesPanel read
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Review Findings

- [x] [Review][Patch] COMPARE button disabled while compare active when `kSelectedM == -1` [PatchMutatorPanel.cpp:358-359]

- [x] [Review][Defer] DELETE/CLEAR during compare may restore stale `compareSavedM_`/`compareSavedR_` [PatchMutatorEngine.cpp:287-294] — deferred, story 6.9 scope
- [x] [Review][Defer] Initial snapshot not reset on patch load / history clear without mutate [PatchMutatorEngine.cpp:126-127] — deferred, story 6.13 scope
- [x] [Review][Defer] `pushResultToEditorAndSynth` return value ignored on compare enter/exit [PatchMutatorEngine.cpp:293-324] — deferred, pre-existing 6.4–6.7 pattern

## Dev Notes

### What Story 6.8 IS — and what it is NOT

| In scope (6.8) | Out of scope |
|---|---|
| `toggleCompare()` engine implementation | `MutatorActionHandler::handleCompare` (**7.4**) |
| Initial snapshot capture on first `mutate()` | Initial snapshot reset on patch load (**6.13**) |
| `kCompareActive` APVTS toggle + audition | DELETE/CLEAR compare exit logic (**6.9**) |
| History M/R graying + COMPARE blink (panel) | Full FR-60 button mirrors (**6.12**) |
| Internal Patches STORE disabled during compare | Bank Utility / Computer Patches STORE (N/A or ROM rules elsewhere) |
| Engine unit tests | Panel unit tests (manual UAT until 7.4 wiring) |
| | `refreshActionEnabledMirrors` (**6.12**) |

**Critical boundary:** Story 6.7 implemented audition + debouncer; 6.8 owns compare toggle semantics and compare-specific UI. Story 7.4 wires COMPARE button timestamp → `toggleCompare()` — panel visual state must work from APVTS property alone so 6.8 is testable before 7.4.

### `toggleCompare()` algorithm (implement exactly)

```
toggleCompare():
  currentlyActive = readBoolProperty(kCompareActive)

  if currentlyActive:
    // EXIT compare
    state.setProperty(kCompareActive, false)
    state.setProperty(kSelectedM, compareSavedM_)
    state.setProperty(kSelectedR, compareSavedR_)
    applySelectionFromApvts()
    pushResultToEditorAndSynth(resolveAuditionBuffer())
    return { success: true }

  // ENTER compare
  if historyStore_.isEmpty():
    return { success: false, footer: "..." }

  if !historyStore_.hasInitialSnapshot():
    return { success: false, footer: "..." }

  compareSavedM_ = selectedRootIndex_   // or read from APVTS before any mutation
  compareSavedR_ = selectedRetryIndex_

  state.setProperty(kCompareActive, true)
  pushResultToEditorAndSynth(historyStore_.getInitialSnapshot())
  return { success: true }
```

**Save selection before enter:** read from internal `selectedRootIndex_` / `selectedRetryIndex_` after `applySelectionFromApvts()` so panel and engine agree.

**Idempotent SysEx on enter:** if editor buffer already matches initial snapshot (`memcmp`), skip `pushResultToEditorAndSynth` (same guard pattern as 6.7 audition). Document in test.

### Initial snapshot capture in `mutate()` (implement exactly)

Insert **after** `resolveAuditionBuffer()` and **before** `algorithm_.apply`:

```cpp
const PatchModel auditionBuffer = resolveAuditionBuffer();

if (! historyStore_.hasInitialSnapshot())
    historyStore_.setInitialSnapshot(auditionBuffer);

PatchModel parentSnapshot;
parentSnapshot.loadFrom(auditionBuffer.data());
// ... existing mutate pipeline
```

**Why here:** first MUTATE uses live editor patch (6.4); that buffer is the pre-mutation patch per FR-32. Initial snapshot is **not** M00 result and **not** listed in History combos (D-085, AD-7).

### Property contract (extends 6.6)

| Property | Writer (6.8) | Reader (6.8) |
|---|---|---|
| `kCompareActive` | `toggleCompare()`, `syncHistoryUiProperties` init default | `PatchMutatorPanel`, `InternalPatchesPanel`, `auditionSelectedHistoryEntry` guard |
| `kSelectedM` / `kSelectedR` | restored on compare **exit** only | preserved during compare (grayed, not cleared) |
| `kHistoryMList` / `kHistoryRList` | unchanged (6.6 sync) | panel uses for compare button enabled state |

### Panel compare UX

**File:** `PatchMutatorPanel.h` / `.cpp`

Add:
- `private juce::Timer` nested class or member `CompareBlinkTimer`
- `void refreshCompareUiState()`
- `bool compareBlinkVisible_ = true`

`refreshCompareUiState()` logic:
```
compareActive = readBoolProperty(kCompareActive)
historyEmpty = mList empty

compareButton_->setEnabled(!historyEmpty)
historyMComboBox_->setEnabled(!compareActive && !historyEmpty)
historyRComboBox_->setEnabled(!compareActive && !historyEmpty)

if (compareActive) startTimerHz(2) else stopTimer + alpha 1.0
```

**Blink target:** `compareButton_` (UX-DR4 active-mode indicator). PRD says "label blinks" — in current Figma layout the COMPARE **button** is the compare-mode indicator; no separate status label exists.

**Do not** block COMPARE button clicks while active (exit requires re-click).

### Internal Patches STORE guard

**File:** `InternalPatchesPanel.cpp`

In `valueTreePropertyChanged`, also react to `MutatorState::kCompareActive` (add include for `PluginIDs.h` mutator state namespace if needed).

Extend `updatePasteStoreEnabled`:
```cpp
const bool compareActive = static_cast<bool>(apvts_.state.getProperty(
    MutatorState::kCompareActive, false));
// ...
storePatchButton_->setEnabled(allowPasteStore && !compareActive);
```

Call `updatePasteStoreEnabled` from compare property change handler (re-read current bank from APVTS).

### Existing stubs to replace

```272:276:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
MutatorActionResult PatchMutatorEngine::toggleCompare()
{
    MutatorActionResult result;
    return result;
}
```

`mutate()` has no `setInitialSnapshot` call today — add per AC #2.

### File structure (this story)

```
Source/Core/Services/PatchMutator/
└── PatchMutatorEngine.cpp          (UPDATE — toggleCompare, mutate snapshot capture)
└── PatchMutatorEngine.h            (UPDATE — compareSavedM_/R_ private members if needed)

Source/GUI/Panels/.../PatchMutatorPanel.h   (UPDATE — blink timer)
Source/GUI/Panels/.../PatchMutatorPanel.cpp (UPDATE — kCompareActive listener, refreshCompareUiState)

Source/GUI/Panels/.../InternalPatchesPanel.cpp (UPDATE — STORE guard during compare)

Tests/Unit/
└── PatchMutatorEngineTests.cpp     (UPDATE — compare + snapshot capture tests)
```

No handler files. No new CMake sources unless timer extracted to `.cpp` (prefer panel-local private Timer class — header-only in `.cpp` anonymous namespace).

### Testing requirements

**PatchMutatorEngineTests** — extend `EngineHarness`:

| Test | Setup | Assert |
|---|---|---|
| `mutate_firstRoot_capturesInitialSnapshot` | Distinct editor buffer, empty history, mutate | `hasInitialSnapshot()`; snapshot == pre-mutate bytes |
| `mutate_secondRoot_doesNotOverwriteInitialSnapshot` | Two mutates | snapshot still first pre-mutate buffer |
| `compare_emptyHistory_blocked` | Empty store | `toggleCompare` fails; `kCompareActive` false; 0 SysEx |
| `compare_enter_auditionsInitialSnapshot` | After one mutate | 1× 0x01; buffer == initial snapshot |
| `compare_enter_setsCompareActive` | After mutate, enter | APVTS property true |
| `compare_exit_restoresSelection` | Select M+R, enter, exit | `kSelectedM`/`kSelectedR` restored |
| `compare_exit_auditionsRestoredEntry` | Enter then exit | buffer == restored entry result |
| `compare_auditionBlockedWhileActive` | Compare on, call audition | 0 new 0x01 |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

**Manual UAT (pre-7.4):** temporarily call `engine.toggleCompare()` from an existing test or debug hook; verify panel blink/gray/STORE disabled respond to `kCompareActive` property changes.

### Architecture compliance

- **Core ↛ GUI** — engine sets APVTS properties only; panel reads properties (AD-5, AD-6).
- **AD-4** — full patch via `pushResultToEditorAndSynth` / `MidiManager::sendPatch`.
- **AD-7** — initial snapshot RAM-only; captured on first MUTATE; cleared on patch load (**6.13**, not here).
- **FR-45** — Mutator/compare graying: History combos + STORE disabled.
- English source/comments only.
- Functions ≤ 15 lines — extract `enterCompare()` / `exitCompare()` private helpers if needed.

### Previous story intelligence (6.7 — done)

- `auditionSelectedHistoryEntry()` no-ops when `kCompareActive` true — **keep**; compare enter/exit use dedicated `pushResultToEditorAndSynth` paths.
- Idempotent `memcmp` guard in audition — apply same pattern on compare enter if buffer already matches initial snapshot.
- `ComboboxPatchSendDebouncer` is 7.4 — compare toggle is **not** debounced (single button click).
- Review deferred: debouncer destructor pending callback — irrelevant to 6.8.

### Previous story intelligence (6.6 — done)

- `kCompareActive` stub initialized in `syncHistoryUiProperties` and panel constructor.
- Panel `valueTreePropertyChanged` today listens only to history list/selection — **extend** for `kCompareActive`.
- History combos already `setEnabled(false)` when empty — compare graying must **compose** (disabled if empty OR compare active).

### Git intelligence

| Commit | Relevance |
|---|---|
| `9a9eaef` | Story 6.7 — audition primitive, `kCompareActive` guard, debouncer |
| `dd7acbc` | Story 6.6 — `kCompareActive` property stub, two-combo panel |
| `5284dd9` | Story 6.4 — `mutate()` pipeline; deferred initial snapshot to 6.8 |

**Brownfield:** `toggleCompare()` empty at `PatchMutatorEngine.cpp:272`. `setInitialSnapshot` never called from engine (only tests). Panel ignores `kCompareActive` for visuals.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17**
- `juce::Timer` on `PatchMutatorPanel` for blink — same family as `Logo`, `Led` (2 Hz reasonable; use `startTimerHz(2)`).
- `juce::Component::setAlpha` for blink — button stays hit-testable.
- No web research required — FR-32 and D-029 specify behaviour.

### Story 7.4 handoff

When 7.4 wires COMPARE button:

```cpp
MutatorActionResult MutatorActionHandler::handleCompare()
{
    auto result = engine_->toggleCompare();
    if (! result.success)
        propagateFooterMessage(result.footerMessage, result.footerSeverity);
    return result;
}
```

Panel and InternalPatchesPanel already react to `kCompareActive` — no handler GUI code.

### Story 6.13 handoff (awareness)

Patch load will clear history **and** reset initial snapshot + force `kCompareActive = false`. Do not implement in 6.8; ensure `toggleCompare` does not assume snapshot survives patch load.

### Project context reference

- `project-context.md` — Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — panel `setProperty` only for actions; compare visuals from state properties
- FR-32 Compare mode
- FR-45 Graying rules (Mutator/compare states)
- UX-DR4 COMPARE blinking + History graying
- D-029 session HISTORY + COMPARE semantics
- D-085 initial snapshot outside History / naming

### References

- [epics.md — Story 6.8]
- [prd.md — FR-32, FR-45]
- [addendum.md — Patch Mutator initial snapshot]
- [.decision-log.md — D-029, D-085]
- [architecture.md — AD-6, AD-7]
- [6-7-history-selection-audition-with-debounce.md — kCompareActive guard, audition boundary]
- [6-6-history-m-and-r-ui-properties.md — kCompareActive stub, panel listener extension point]
- [6-2-mutationhistorystore-two-level-mr.md — initial snapshot API]
- [7-4-mutatoractionhandler.md — handleCompare delegation, no panel changes]
- [PatchMutatorEngine.cpp — toggleCompare stub, mutate, auditionSelectedHistoryEntry]
- [PatchMutatorPanel.cpp — history combos, compareButton_, valueTreePropertyChanged]
- [InternalPatchesPanel.cpp — updatePasteStoreEnabled]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

### Completion Notes List

- Implemented `toggleCompare()` with enter/exit branches, selection save/restore, idempotent SysEx guard, and blocked paths for empty history / missing initial snapshot.
- Added initial snapshot capture in `mutate()` before algorithm runs (first MUTATE only).
- Extended `PatchMutatorPanel` with 2 Hz COMPARE button blink timer, history combo graying, and compare button enabled state.
- Extended `InternalPatchesPanel` to disable STORE during compare via `kCompareActive` listener.
- Added 8 unit tests covering snapshot capture and all compare scenarios; full `Matrix-Control_Tests` suite green.

### File List

- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
- Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp

### Change Log

- 2026-06-20 — Story 6.8 created: COMPARE toggle engine, initial snapshot capture, panel blink/gray, STORE guard.
- 2026-06-20 — Story 6.8 implemented: engine toggleCompare, mutate snapshot capture, panel UX, STORE guard, 8 unit tests.

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **done**
