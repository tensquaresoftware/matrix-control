---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.7 — History Selection Audition with Debounce
author: BMad Agent
status: review
baseline_commit: dd7acbc
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/6-6-history-m-and-r-ui-properties.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - implementation-artifacts/4-6-previous-and-next-file-navigation.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.7: History Selection Audition with Debounce

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want selecting history entries to audition after debounce,
so that rapid scrolling does not SysEx-flood the synth (FR-57, FR-30).

## Acceptance Criteria

1. **Given** Story 6.6 (`kSelectedM` / `kSelectedR` APVTS mirrors + `PatchMutatorPanel` two-combo selection) and Story 2.1 (`MidiManager::sendPatch` / outbound queue) **When** `PatchMutatorEngine::auditionSelectedHistoryEntry()` is implemented **Then** it:
   - Calls `applySelectionFromApvts()` first
   - Returns immediately (no SysEx, no APVTS push) when history is empty or `selectedM < 0`
   - Resolves the audition buffer via existing `resolveAuditionBuffer()` semantics (Mi root-only or Mi-Rj result — D-083)
   - Pushes resolved buffer to editor + synth via existing `pushResultToEditorAndSynth()` (one full patch SysEx **0x01** per successful audition)
   - Updates PATCH NAME via existing `pushPatchModelToApvtsWithSuppress` path inside `pushResultToEditorAndSynth`

2. **And** **idempotent guard** prevents duplicate SysEx when editor buffer already matches the resolved audition buffer (byte-for-byte `PatchModel::kBufferSize` compare). This is required because `mutate()` / `retry()` already call `pushResultToEditorAndSynth()` then `syncHistoryUiProperties()` which writes `kSelectedM` / `kSelectedR` — Story **7.4** will debounce on those property changes; without the guard, every MUTATE/RETRY would double-send.

3. **And** shared Core debounce infrastructure is introduced per architecture debounce policy:
   - Single constant `kComboboxPatchSendDebounceMs` (recommended **150 ms** — no PRD millisecond value; align with human combobox scroll feel; document choice in code comment; update `.decision-log.md` only if Guillaume requests a different value)
   - Reusable debouncer class (recommended name: `ComboboxPatchSendDebouncer`) under `Source/Core/` (e.g. `Source/Core/Util/ComboboxPatchSendDebouncer.h` + `.cpp` or header-only if ≤15 lines per method)
   - API: `schedule(std::function<void()> callback)` — each call resets the timer; callback fires once on message thread after debounce window elapses
   - Uses `juce::Timer` on message thread (same pattern as `PluginProcessor::MatrixModSysExCoalesceTimer` but single-shot reset semantics, not bus bitmask coalesce)
   - **Computer Patches (FR-52)** does not use debounce today (Story 4.6 explicitly deferred it) — this story creates the shared helper; wiring Computer Patches to it is **out of scope** (future follow-up)

4. **And** `HistorySelectionDebouncer` type alias or thin wrapper around `ComboboxPatchSendDebouncer` is acceptable if Story 7.4 prefers a Mutator-specific name — behaviour must be identical; one implementation only.

5. **And** `auditionSelectedHistoryEntry()` **does not** run from:
   - `syncHistoryUiProperties()` (6.6 boundary — list/selection mirror only)
   - `mutate()` / `retry()` success paths (already push result)
   - Panel code (AD-5 — panel writes properties only)
   - **COMPARE active** — when `kCompareActive` is `true`, return immediately without SysEx (stub guard for Story 6.8; compare audition is 6.8)

6. **And** unit tests in `Tests/Unit/PatchMutatorEngineTests.cpp` cover audition without GUI:
   - `audition_emptyHistory_noSysEx` — empty store → no queue traffic
   - `audition_selectedRoot_sendsSysExOnce` — insert M00 with distinct buffer, select root-only, audition → one 0x01, editor buffer matches entry
   - `audition_selectedRetry_sendsSysExOnce` — M00 + R00, select R00, audition → buffer matches retry result
   - `audition_idempotent_skipsDuplicateSysEx` — push buffer already matches selection → second `auditionSelectedHistoryEntry()` → zero additional 0x01 messages
   - `audition_compareActive_noSysEx` — set `kCompareActive = true` → audition no-ops

7. **And** unit tests in new `Tests/Unit/ComboboxPatchSendDebouncerTests.cpp` (or section in existing test file) cover debouncer:
   - `debouncer_rapidSchedule_firesOnce` — N rapid `schedule()` calls → callback invoked exactly once after debounce window
   - `debouncer_finalSelectionWins` — schedule A, schedule B before window → only B's callback runs
   - Use injectable clock or `juce::UnitTest::wait()` with real timer (keep tests deterministic; avoid flaky >500 ms waits)

8. **And** this story delivers **engine audition method + shared debouncer + unit tests only** — no `MutatorActionHandler` / `PluginProcessor::valueTreePropertyChanged` wiring (**7.4**), no COMPARE toggle behaviour (**6.8**), no DELETE/CLEAR audition side-effects (**6.9**), no Computer Patches debounce retrofit. Story 7.4 consumes `ComboboxPatchSendDebouncer` + `auditionSelectedHistoryEntry()`; prerequisite note in 7.4 AC is satisfied when 6.7 is `done`.

9. **And** `CMakeLists.txt` registers any new `.cpp` sources for main target and `Matrix-Control_Tests`; full test target remains green.

## Tasks / Subtasks

- [x] **Add debounce constant + helper** (AC: #3, #4)
  - [x] Create `kComboboxPatchSendDebounceMs` (e.g. in `Source/Core/Util/ComboboxPatchSendDebouncer.h` or `Source/Core/MIDI/SysEx/SysExTiming.h` — pick one location, no duplication)
  - [x] Implement `ComboboxPatchSendDebouncer` with `schedule(std::function<void()>)`
  - [x] Register sources in `CMakeLists.txt`

- [x] **Implement `auditionSelectedHistoryEntry()`** (AC: #1, #2, #5)
  - [x] Replace empty stub at `PatchMutatorEngine.cpp:302-304`
  - [x] Add private `auditionBufferMatchesEditor(const PatchModel&)` or inline `memcmp` guard
  - [x] Read `kCompareActive` — early return when true
  - [x] Do **not** call from `syncHistoryUiProperties`, `mutate()`, or `retry()`

- [x] **Unit tests — engine** (AC: #6)
  - [x] Extend `PatchMutatorEngineTests` with audition cases
  - [x] Reuse `EngineHarness`, `countPatchSysExMessages`, `makeDistinctBuffer` patterns from 6.4–6.6

- [x] **Unit tests — debouncer** (AC: #7)
  - [x] Add `ComboboxPatchSendDebouncerTests.cpp` + CMake entry
  - [x] Verify rapid-schedule coalescing

- [x] **Self-review** (AC: #8, #9)
  - [x] Grep `auditionSelectedHistoryEntry` — only defined in engine + called from tests (no handler/processor calls yet)
  - [x] Grep `PatchMutatorPanel` — still no `MidiManager` / engine includes
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

## Dev Notes

### What Story 6.7 IS — and what it is NOT

| In scope (6.7) | Out of scope |
|---|---|
| `auditionSelectedHistoryEntry()` implementation | `MutatorActionHandler::onHistorySelectionChanged()` (**7.4**) |
| Idempotent buffer-compare guard | `PluginProcessor` property listener routing (**7.4**) |
| `ComboboxPatchSendDebouncer` + `kComboboxPatchSendDebounceMs` | Computer Patches combobox debounce retrofit |
| Engine + debouncer unit tests | COMPARE toggle + blinking label (**6.8**) |
| `kCompareActive` early-return stub | DELETE/CLEAR post-delete audition (**6.9**) |
| | Panel or handler changes |

**Critical boundary:** Story 6.6 established panel → APVTS property writes only. Story 6.7 implements the **engine audition primitive** and the **debounce utility** that 7.4 will connect: `kSelectedM` / `kSelectedR` change → debouncer → `auditionSelectedHistoryEntry()`.

### `auditionSelectedHistoryEntry()` algorithm (implement exactly)

```
auditionSelectedHistoryEntry():
  if readBoolProperty(kCompareActive): return

  applySelectionFromApvts()

  if historyStore_.isEmpty() || selectedRootIndex_ < 0:
    return

  auditionModel = resolveAuditionBuffer()

  if memcmp(auditionModel.data(), patchModel_->data(), kBufferSize) == 0:
    return   // idempotent — avoids double SysEx after mutate/retry sync

  pushResultToEditorAndSynth(auditionModel)
```

**Reuse `pushResultToEditorAndSynth`** — do not fork APVTS push or SysEx enqueue logic. It already:
1. Copies buffer to `patchModel_`
2. Pushes to APVTS with suppress hooks + `PatchNameSyncer`
3. Calls `midiManager_->sendPatch(patchNumber, buffer)` when manager present

### Why idempotent guard is mandatory

End-to-end flow after Story 7.4:

```
User clicks MUTATE
  → handler → engine.mutate()
  → pushResultToEditorAndSynth (SysEx #1)
  → syncHistoryUiProperties writes kSelectedM, kSelectedR
  → processor hears property change
  → debouncer → auditionSelectedHistoryEntry()
  → WITHOUT guard: SysEx #2 (duplicate)
  → WITH guard: buffer already matches → skip
```

User scrolling History M/R:

```
Panel writes kSelectedM / kSelectedR
  → debouncer (150 ms)
  → auditionSelectedHistoryEntry()
  → buffer differs → SysEx once
```

### Debouncer design

Reference pattern: `PluginProcessor::MatrixModSysExCoalesceTimer` (`PluginProcessor.h:183-210`) — `juce::Timer`, message thread, `startTimer(delayMs)` on each trigger.

**Difference:** combobox debounce is **last-write-wins** single callback, not a bitmask flush:

```cpp
class ComboboxPatchSendDebouncer : private juce::Timer
{
public:
    void schedule(std::function<void()> callback);

private:
    void timerCallback() override;
    std::function<void()> pending_;
};
```

- `schedule()` stores `callback`, calls `startTimer(kComboboxPatchSendDebounceMs)` (restarts if already running)
- `timerCallback()` moves `pending_`, invokes once, clears
- Destructor `stopTimer()` — safe if handler outlives processor

**Testability:** optional constructor `ComboboxPatchSendDebouncer(int debounceMsOverride)` for fast unit tests (e.g. 20 ms).

### Property contract (unchanged from 6.6)

| Trigger (7.4 wiring) | Properties written by panel | Debounce target |
|---|---|---|
| User picks new root | `kSelectedM`, then `kSelectedR = kRootOnly` | One coalesced audition after both writes settle |
| User picks retry row | `kSelectedR` only | One audition |

Story 6.7 does **not** implement the listener — only documents that debouncer must coalesce the M-then-R double-write from `PatchMutatorPanel::historyMComboBox_->onChange` (`PatchMutatorPanel.cpp:249-262`).

### `syncHistoryUiProperties` must stay SysEx-free

```306:320:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
void PatchMutatorEngine::syncHistoryUiProperties(juce::AudioProcessorValueTreeState& apvts)
{
    // ...
    if (state.hasProperty(MutatorState::kSelectedM))
    {
        const int apvtsM = static_cast<int>(state.getProperty(MutatorState::kSelectedM, -1));
        if (apvtsM != selectedRootIndex_)
            applySelectionFromApvts();
    }
```

6.6 reads APVTS selection when M changes (panel-only path until 7.4). Do **not** add audition calls here.

### Existing stub to replace

```302:304:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
void PatchMutatorEngine::auditionSelectedHistoryEntry()
{
}
```

### File structure (this story)

```
Source/Core/Util/                          (NEW dir if missing)
├── ComboboxPatchSendDebouncer.h           (NEW — constant + class)
└── ComboboxPatchSendDebouncer.cpp         (NEW — if not header-only)

Source/Core/Services/PatchMutator/
└── PatchMutatorEngine.cpp                 (UPDATE — audition implementation)

Tests/Unit/
├── PatchMutatorEngineTests.cpp            (UPDATE — audition tests)
└── ComboboxPatchSendDebouncerTests.cpp    (NEW)

CMakeLists.txt                             (UPDATE — new sources)
```

No GUI files. No handler files.

### Testing requirements

**PatchMutatorEngineTests** — extend `EngineHarness`:

| Test | Setup | Assert |
|---|---|---|
| `audition_emptyHistory_noSysEx` | Default empty store | `countPatchSysExMessages == 0` after audition |
| `audition_selectedRoot_sendsSysExOnce` | Insert M00 distinct buffer; `setAuditionSelection(0, kRootOnly)` or set APVTS properties + apply | One 0x01; `model` bytes match entry |
| `audition_selectedRetry_sendsSysExOnce` | M00 + R00; select R00 | Buffer matches retry `result` |
| `audition_idempotent_skipsDuplicateSysEx` | Audition once (SysEx 1), drain queue, audition again | Second call → 0 new 0x01 |
| `audition_compareActive_noSysEx` | Non-empty history; `kCompareActive = true` | No SysEx |

**ComboboxPatchSendDebouncerTests:**

| Test | Assert |
|---|---|
| `debouncer_rapidSchedule_firesOnce` | 5 schedules in < debounce window → callback count == 1 |
| `debouncer_finalSelectionWins` | Schedule callback A, then B before fire → only B runs |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **Core ↛ GUI** — debouncer and audition live in Core; no panel includes
- **AD-4** — full patch via `MidiManager::sendPatch` / queue, not per-parameter 0x06
- **AD-5** — panel unchanged; handler wiring deferred to 7.4
- **AD-6** — audition on `PatchMutatorEngine`, not in handler business logic
- **Debouncing policy** — `kComboboxPatchSendDebounceMs` shared name per `architecture.md:372`
- English source/comments only
- Functions ≤ 15 lines — extract buffer-compare helper if needed

### Previous story intelligence (6.6 — done)

- `syncHistoryUiProperties()` fully implemented; writes `kHistoryMList`, `kHistoryRList`, `kSelectedM`, `kSelectedR`; **no SysEx** (explicit 6.6 AC #3)
- `applySelectionFromApvts()` wired into `mutate()`, `retry()`, `resolveAuditionBuffer()`
- Panel `onChange` writes `kSelectedM` + resets `kSelectedR` to `kSelectedRRootOnly` on M change
- Review resolved: `syncHistoryUiProperties` reads APVTS when `kSelectedM` differs from internal — 7.4 will also call sync on `kSelectedM` for R list rebuild
- `auditionSelectedHistoryEntry()` stub explicitly deferred to 6.7 (`deferred-work.md`)

### Git intelligence

| Commit | Relevance |
|---|---|
| `dd7acbc` | Story 6.6 — History M/R mirrors, two-combo panel, sync tests |
| `d933ea3` | Story 6.5 — `retry()` + `pushResultToEditorAndSynth` |
| `5284dd9` | Story 6.4 — `mutate()` + SysEx path |

**Brownfield:** `auditionSelectedHistoryEntry()` empty at `PatchMutatorEngine.cpp:302`. No debounce helper exists anywhere in repo (grep confirms). Computer Patches loads immediately on combobox change (`PatchManagerActionHandler.cpp:171-174`) — no debounce to copy literally; 6.7 **creates** the shared policy artifact architecture references.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17**
- `juce::Timer` for debounce — message-thread safe; same family as `DeferredMidiPortSyncTimer` / `MatrixModSysExCoalesceTimer` in `PluginProcessor.h`
- `std::function<void()>` for callback — store moved callback in debouncer; avoid capturing raw `PatchMutatorEngine*` without `SafePointer` in 7.4 (document in 7.4, not 6.7)
- No web research required — FR-57 and architecture specify behaviour; only ms constant is a product tuning choice (150 ms recommended)

### Story 7.4 handoff (for dev agent awareness)

When 7.4 implements wiring:

```cpp
// MutatorActionHandler or PluginProcessor — NOT in 6.7
void onHistorySelectionChanged()
{
    debouncer_.schedule([this] {
        if (engine_ != nullptr)
            engine_->auditionSelectedHistoryEntry();
    });
}

// PluginProcessor::valueTreePropertyChanged
if (property == kSelectedM || property == kSelectedR)
{
    mutatorActionHandler_->onHistorySelectionChanged();
    if (property == kSelectedM)
        patchMutatorEngine_->syncHistoryUiProperties(apvts_);  // 6.6 + 7.4
}
```

6.7 must make `auditionSelectedHistoryEntry()` safe for this call pattern (idempotent guard).

### Project context reference

- `project-context.md` — Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — no GUI in Core; reuse existing `pushResultToEditorAndSynth`
- FR-57 History selection audition with debounce
- FR-30 full patch SysEx 0x01 on audition
- addendum § SysEx & debounce — same policy as Computer Patches (helper created here; CP wiring later)

### References

- [epics.md — Story 6.7]
- [prd.md — FR-30, FR-57]
- [addendum.md — SysEx & debounce]
- [architecture.md — AD-4, AD-5, AD-6, debounce policy §372]
- [6-6-history-m-and-r-ui-properties.md — property contract, sync boundary, stub deferral]
- [7-4-mutatoractionhandler.md — debounce wiring AC #3, consumes 6.7 artifacts]
- [4-6-previous-and-next-file-navigation.md — FR-52 debounce explicitly out of scope]
- [PatchMutatorEngine.cpp — audition stub, pushResultToEditorAndSynth, resolveAuditionBuffer]
- [PatchMutatorPanel.cpp — M/R onChange property writes]
- [PluginProcessor.h — MatrixModSysExCoalesceTimer timer pattern]

## Dev Agent Record

### Agent Model Used

Claude claude-4.6-sonnet-medium-thinking (Cursor)

### Debug Log References

### Completion Notes List

- Implemented `ComboboxPatchSendDebouncer` with `kComboboxPatchSendDebounceMs` (150 ms) and injectable debounce override for tests.
- Implemented `auditionSelectedHistoryEntry()` with compare-active guard, selection apply, idempotent `memcmp` guard, and `pushResultToEditorAndSynth` reuse.
- Added 5 engine audition tests and 2 debouncer coalescing tests; full `Matrix-Control_Tests` suite green.

### File List

- `Source/Core/Util/ComboboxPatchSendDebouncer.h` (new)
- `Source/Core/Util/ComboboxPatchSendDebouncer.cpp` (new)
- `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp` (modified)
- `Tests/Unit/ComboboxPatchSendDebouncerTests.cpp` (new)
- `Tests/Unit/PatchMutatorEngineTests.cpp` (modified)
- `CMakeLists.txt` (modified)

### Change Log

- 2026-06-20 — Story 6.7: history selection audition primitive + shared combobox debouncer + unit tests.

### Review Findings

- [x] [Review][Defer] Destruction du debouncer sans exécution du callback en attente [`ComboboxPatchSendDebouncer.cpp:11-14`] — deferred, 7.4 documentera le cycle de vie handler/processor
- [x] [Review][Defer] Pas de test bout-en-bout mutate → audition pour le scénario AC #2 [`PatchMutatorEngineTests.cpp`] — deferred, couvert indirectement par `audition_idempotent_skipsDuplicateSysEx`
- [x] [Review][Defer] Pas de contrat thread message sur `ComboboxPatchSendDebouncer::schedule()` [`ComboboxPatchSendDebouncer.cpp:16-20`] — deferred, même convention que les autres `juce::Timer` du projet ; 7.4 câblera depuis le thread message

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **done**
