---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.9 — DELETE and CLEAR History
author: BMad Agent
status: done
baseline_commit: 87a2a97
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/6-8-compare-mode.md
  - implementation-artifacts/6-7-history-selection-audition-with-debounce.md
  - implementation-artifacts/6-6-history-m-and-r-ui-properties.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - implementation-artifacts/deferred-work.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.9: DELETE and CLEAR History

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want to delete entries or clear all history,
so that I manage session mutations (FR-58, D-084).

## Acceptance Criteria

1. **Given** Story 6.2 (`MutationHistoryStore::deleteRoot` / `deleteRetry` / `clear`) and Stories 6.6–6.8 (APVTS sync, audition, compare) **When** `PatchMutatorEngine::deleteSelected()` is implemented **Then** it:
   - Calls `applySelectionFromApvts()` first.
   - **Blocked** when `historyStore_.isEmpty()` → `{ success: false, footerMessage: kEmptyHistoryFooterMessage, footerSeverity: warning }` — no store mutation, no SysEx.
   - **Blocked** when `selectedRootIndex_ < 0` or `!historyStore_.hasRoot(selectedRootIndex_)` → `{ success: false, footerMessage: kNoSelectionFooterMessage, footerSeverity: warning }`.
   - **Delete retry** when `selectedRetryIndex_ != kRootOnly` and `hasRetry(m, r)`: `historyStore_.deleteRetry(m, r)` — single entry removed; **gaps remain** (D-084).
   - **Delete root** when `selectedRetryIndex_ == kRootOnly`: `historyStore_.deleteRoot(m)` — **cascades** all **Mi-R*** for that root; returns `{ success: true, footerMessage: cascade notice, footerSeverity: info }` (English user string, e.g. `"Deleted M05 and all retries."` with formatted label from `MutationNaming::formatRootLabel`).
   - After successful delete: **selection fallback** per AC #2; `syncHistoryUiProperties(apvts_)`; **force-exit compare** per AC #4; **audition** post-delete buffer per AC #3; `{ success: true }` (retry-only delete may omit footer or use brief info — cascade delete **must** show footer per FR-58).

2. **And** **selection fallback after DELETE** (D-084, FR-58) — compute **before** store delete from sorted lists:
   - **Retry delete** at `(m, r)`: let `retries = getSortedRetryIndices(m)`; find index of `r`; if prior retry exists → select that retry index; else → `kRootOnly` (root-only `—` sentinel).
   - **Root delete** at `m`: let `roots = getSortedRootIndices()`; find index of `m`; if prior root exists → select that root with `kRootOnly`; else → empty history (`selectedRootIndex_ = -1`, `selectedRetryIndex_ = kRootOnly`).
   - `syncHistoryUiProperties` writes clamped `kSelectedM` / `kSelectedR` / list mirrors — panel shows `<EMPTY>` when empty (6.6).

3. **And** **post-delete / post-clear audition** (FR-30, FR-58, vision input):
   - If history **non-empty** after operation: `pushResultToEditorAndSynth(resolveAuditionBuffer())` for new selection (idempotent `memcmp` guard — same as 6.7/6.8).
   - If history **empty** after operation: audition **initial snapshot** when `hasInitialSnapshot()`; else live editor buffer (`*patchModel_`). COMPARE must be off (AC #4).

4. **And** **compare side-effects** (resolves 6.8 deferred item):
   - If `kCompareActive == true` before DELETE or CLEAR: set `kCompareActive = false`; reset `compareSavedM_ = -1`, `compareSavedR_ = kRootOnly` — **do not** restore stale saved selection from compare enter.
   - Do not call compare exit branch of `toggleCompare()` (that would restore obsolete indices).

5. **And** `PatchMutatorEngine::clearHistory()`:
   - Calls `historyStore_.clear()` — purges all roots/retries; **does not** clear initial snapshot (6.2 AC #3 — initial snapshot reset is **6.13** patch-load only).
   - Force-exit compare (AC #4).
   - `syncHistoryUiProperties` → `kHistoryMList` empty, `kSelectedM == -1`, History M shows `<EMPTY>` sentinel via panel (6.6).
   - Audition per AC #3 (initial snapshot when available).
   - Returns `{ success: true }`.

6. **And** unit tests in `Tests/Unit/PatchMutatorEngineTests.cpp` cover engine behaviour without GUI:
   - `delete_emptyHistory_blocked` — empty store → fail, zero 0x01.
   - `delete_noSelection_blocked` — roots exist but `kSelectedM == -1` → fail.
   - `delete_retry_removesSingleEntry` — R02 deleted, R00 remains, gap at R01 if applicable.
   - `delete_root_cascadesRetries` — root + 2 retries → all gone; footer message non-empty on result.
   - `delete_retry_selectionMovesToPrevious` — M00+R02 selected, delete R02 → selects R00.
   - `delete_retry_firstRetry_fallsBackToRootOnly` — delete R00 → `kSelectedR == kRootOnly`.
   - `delete_root_selectionMovesToPreviousRoot` — M00, M05 roots; delete M05 → `kSelectedM == 0`.
   - `delete_lastEntry_emptyHistory_auditionsInitialSnapshot` — one root, delete → empty lists, one 0x01 matching initial snapshot.
   - `delete_lastEntry_disablesCompare` — compare active, delete last entry → `kCompareActive == false`.
   - `delete_whileCompareActive_noStaleRestore` — enter compare, delete entry → compare off; selection follows delete fallback not `compareSavedM_`.
   - `delete_syncsApvtsAfterSuccess` — `kHistoryMList` / `kHistoryRList` updated.
   - `clear_purgesHistory_emptySentinel` — roots inserted, clear → `rootCount() == 0`, `kSelectedM == -1`.
   - `clear_disablesCompare` — compare on, clear → `kCompareActive == false`.
   - `clear_auditionsInitialSnapshot` — snapshot set, clear → 0x01 buffer matches snapshot.
   - `clear_keepsInitialSnapshot` — `hasInitialSnapshot()` still true after clear.

7. **And** this story delivers **`deleteSelected()` + `clearHistory()` engine + unit tests only** — no `MutatorActionHandler` DELETE/CLEAR wiring (**7.4**), no DELETE/CLEAR button enabled-state mirrors (**6.12**), no initial-snapshot reset on clear (**6.13** patch-load hook), no panel changes (buttons already timestamp `setProperty`), no `refreshActionEnabledMirrors`.

8. **And** `CMakeLists.txt` unchanged; full `Matrix-Control_Tests` target remains green.

## Tasks / Subtasks

- [x] **Implement `deleteSelected()`** (AC: #1–#4)
  - [x] Replace stub at `PatchMutatorEngine.cpp:331-335`
  - [x] Add private helpers: `forceExitCompare()`, `resolveSelectionAfterDelete(...)`, `auditionAfterHistoryMutation()` (names flexible — keep functions ≤ 15 lines)
  - [x] Cascade footer uses `MutationNaming::formatRootLabel(rootIndex)`
  - [x] Reuse existing footer constants; add cascade message constant in anonymous namespace if needed

- [x] **Implement `clearHistory()`** (AC: #5)
  - [x] Replace stub at `PatchMutatorEngine.cpp:337-341`
  - [x] Share `forceExitCompare()` + audition helper with delete path

- [x] **Unit tests** (AC: #6)
  - [x] Extend `PatchMutatorEngineTests::runTest()` with delete/clear cases
  - [x] Reuse `EngineHarness`, `store()`, `makeDistinctBuffer`, `countPatchSysExMessages`

- [x] **Self-review** (AC: #7, #8)
  - [x] Grep `deleteSelected` / `clearHistory` — implemented in engine only; handler still stub
  - [x] Verify `syncHistoryUiProperties` called after every successful delete/clear
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Review Findings

- [x] [Review][Patch] Blocked-path tests omit footer contract — `delete_emptyHistory_blocked` and `delete_noSelection_blocked` assert only `!success`; AC #1 requires `footerMessage` and `footerSeverity: warning` [`PatchMutatorEngineTests.cpp:995-1021`]
- [x] [Review][Patch] `delete_root_cascadesRetries` omits `footerSeverity: info` assertion — AC #1 requires info severity on cascade delete [`PatchMutatorEngineTests.cpp:1076-1078`]
- [x] [Review][Patch] `delete_syncsApvtsAfterSuccess` omits `kHistoryRList` assertion — AC #6 table requires both list mirrors updated [`PatchMutatorEngineTests.cpp:1266-1274`]
- [x] [Review][Patch] `clear_purgesHistory_emptySentinel` omits empty-list mirror asserts — AC #6 expects `kHistoryMList` / `kHistoryRList` empty after clear [`PatchMutatorEngineTests.cpp:1295-1297`]
- [x] [Review][Patch] No test for non-empty post-delete audition SysEx — AC #3 path when history remains has no `countPatchSysExMessages` check (e.g. `delete_retry_selectionMovesToPrevious`) [`PatchMutatorEngineTests.cpp`]
- [x] [Review][Patch] Cascade footer uses inline string instead of `constexpr` constant — story task subtask requests named constant in anonymous namespace [`PatchMutatorEngine.cpp:382-385`]
- [x] [Review][Defer] `deleteSelected()` / `resolveSelectionAfterDelete()` exceed ≤15-line convention — helpers are correct; refactor optional [`PatchMutatorEngine.cpp:332-541`] — deferred, pre-existing convention tension

## Dev Notes

### What Story 6.9 IS — and what it is NOT

| In scope (6.9) | Out of scope |
|---|---|
| `deleteSelected()` + `clearHistory()` engine | `MutatorActionHandler::handleDelete` / `handleClear` (**7.4**) |
| Selection fallback after delete (engine) | DELETE/CLEAR button `setEnabled` mirrors (**6.12** / FR-60) |
| Post-delete/clear audition + SysEx 0x01 | Initial snapshot reset on patch load (**6.13**) |
| Force-exit compare on delete/clear (6.8 deferral) | Initial snapshot clear on CLEAR (store keeps snapshot per 6.2) |
| `syncHistoryUiProperties` after store mutation | Panel wiring / blink timer changes |
| Engine unit tests | Panel unit tests |

**Critical boundary:** Store CRUD already exists (6.2). Engine orchestrates selection, APVTS sync, compare teardown, and audition — same pattern as `mutate()` / `retry()` / `toggleCompare()`.

### `deleteSelected()` algorithm (implement exactly)

```
deleteSelected():
  applySelectionFromApvts()

  if historyStore_.isEmpty():
    return blocked (kEmptyHistoryFooterMessage)

  m = selectedRootIndex_
  if m < 0 || !hasRoot(m):
    return blocked (kNoSelectionFooterMessage)

  forceExitCompare()   // AC #4 — before selection recompute

  if selectedRetryIndex_ != kRootOnly && hasRetry(m, r):
    newM, newR = resolveSelectionAfterDelete(m, r, /*isRetryDelete=*/true)
    historyStore_.deleteRetry(m, r)
    footer = none (or optional brief info)
  else:
    newM, newR = resolveSelectionAfterDelete(m, kRootOnly, /*isRetryDelete=*/false)
    historyStore_.deleteRoot(m)   // cascade retries inside store
    footer = "Deleted {formatRootLabel(m)} and all retries." (info)

  selectedRootIndex_ = newM
  selectedRetryIndex_ = newR
  syncHistoryUiProperties(apvts_)
  auditionAfterHistoryMutation()
  return { success: true, footerMessage?, footerSeverity }
```

### Selection fallback helper (implement exactly)

```
resolveSelectionAfterDelete(m, r, isRetryDelete):
  if isRetryDelete:
    retries = getSortedRetryIndices(m)
    find index i where retries[i] == r
    if i > 0: return (m, retries[i-1])
    return (m, kRootOnly)

  // root delete
  roots = getSortedRootIndices()
  find index i where roots[i] == m
  if i > 0: return (roots[i-1], kRootOnly)
  return (-1, kRootOnly)   // empty history
```

**Do not** fill index gaps — fallback uses **list order**, not numeric index arithmetic.

### `clearHistory()` algorithm (implement exactly)

```
clearHistory():
  forceExitCompare()
  historyStore_.clear()          // does NOT clear initial snapshot
  selectedRootIndex_ = -1
  selectedRetryIndex_ = kRootOnly
  syncHistoryUiProperties(apvts_)
  auditionAfterHistoryMutation()
  return { success: true }
```

### `forceExitCompare()` (implement exactly)

```
forceExitCompare():
  if !readBoolProperty(kCompareActive): return
  apvts_.state.setProperty(kCompareActive, false)
  compareSavedM_ = -1
  compareSavedR_ = kRootOnly
```

**Never** call `toggleCompare()` exit branch from delete/clear — it would restore stale `compareSavedM_`/`compareSavedR_` (6.8 review deferral).

### `auditionAfterHistoryMutation()` (implement exactly)

```
auditionAfterHistoryMutation():
  if !historyStore_.isEmpty():
    buffer = resolveAuditionBuffer()
  else if historyStore_.hasInitialSnapshot():
    buffer = getInitialSnapshot()
  else:
    buffer = *patchModel_

  if memcmp(buffer, patchModel_) != 0:
    pushResultToEditorAndSynth(buffer)
```

### Property contract (extends 6.6 / 6.8)

| Property | Writer (6.9) | Reader (6.9) |
|---|---|---|
| `kHistoryMList` / `kHistoryRList` | `syncHistoryUiProperties` after delete/clear | `PatchMutatorPanel` |
| `kSelectedM` / `kSelectedR` | fallback + sync | panel combos |
| `kCompareActive` | `forceExitCompare()` on delete/clear | panel graying, audition guard |

### Existing stubs to replace

```331:341:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
MutatorActionResult PatchMutatorEngine::deleteSelected()
{
    MutatorActionResult result;
    return result;
}

MutatorActionResult PatchMutatorEngine::clearHistory()
{
    MutatorActionResult result;
    return result;
}
```

Store APIs already implemented:

```145:163:Source/Core/Services/PatchMutator/MutationHistoryStore.cpp
bool MutationHistoryStore::deleteRoot(int rootIndex)
{
    return roots_.erase(rootIndex) > 0;
}
// ... deleteRetry, clear
```

### File structure (this story)

```
Source/Core/Services/PatchMutator/
└── PatchMutatorEngine.cpp          (UPDATE — deleteSelected, clearHistory, private helpers)
└── PatchMutatorEngine.h            (UPDATE — private helper declarations if needed)

Tests/Unit/
└── PatchMutatorEngineTests.cpp     (UPDATE — delete/clear tests)
```

No GUI files. No handler files. No CMake changes.

### Testing requirements

**PatchMutatorEngineTests** — extend `EngineHarness` (already exposes `store()` via friend):

| Test | Setup | Assert |
|---|---|---|
| `delete_emptyHistory_blocked` | Empty store | `success == false`; 0 SysEx |
| `delete_noSelection_blocked` | Root inserted, `kSelectedM = -1` | `success == false` |
| `delete_retry_removesSingleEntry` | M00+R00+R02 | After delete R02: `hasRetry(0,2)==false`, `hasRetry(0,0)==true` |
| `delete_root_cascadesRetries` | M05 root + 2 retries | All gone; `footerMessage` non-empty |
| `delete_retry_selectionMovesToPrevious` | Select R02, delete R02 | `kSelectedR == 0` |
| `delete_retry_firstRetry_fallsBackToRootOnly` | Select R00, delete R00 | `kSelectedR == kRootOnly` |
| `delete_root_selectionMovesToPreviousRoot` | M00+M05, select M05 root, delete | `kSelectedM == 0` |
| `delete_lastEntry_emptyHistory_auditionsInitialSnapshot` | One root + snapshot | Empty lists; 1× 0x01 == snapshot |
| `delete_lastEntry_disablesCompare` | Compare on, delete last | `kCompareActive == false` |
| `delete_whileCompareActive_noStaleRestore` | Compare on with saved M/R, delete changes selection | Compare off; selection = fallback not saved compare indices |
| `delete_syncsApvtsAfterSuccess` | Two roots, delete one | `kHistoryMList` pipe string updated |
| `clear_purgesHistory_emptySentinel` | Insert roots, clear | `rootCount()==0`; `kSelectedM==-1` |
| `clear_disablesCompare` | Compare on, clear | `kCompareActive == false` |
| `clear_auditionsInitialSnapshot` | Snapshot + roots, clear | 0x01 matches snapshot |
| `clear_keepsInitialSnapshot` | Set snapshot, clear | `hasInitialSnapshot()` true |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **Core ↛ GUI** — engine only; panel reacts via APVTS properties (AD-5, AD-6).
- **AD-4** — full patch via `pushResultToEditorAndSynth` / `MidiManager::sendPatch`.
- **AD-7** — initial snapshot separate from history; `clear()` does not purge snapshot (6.2).
- **D-084** — gaps preserved; selection uses sorted list order.
- **FR-58** — cascade footer on root delete; CLEAR → `<EMPTY>` via sync.
- English source/comments only.
- Functions ≤ 15 lines — extract helpers.

### Previous story intelligence (6.8 — done)

- `compareSavedM_` / `compareSavedR_` written on compare enter only — delete/clear must invalidate, not restore.
- `syncHistoryUiProperties` empty branch sets `kSelectedM = -1` and clears lists — reuse after clear/last delete.
- `toggleCompare()` blocked when history empty — after delete-to-empty, compare already forced off before sync.
- Review deferred compare+delete → **this story** closes it.

### Previous story intelligence (6.7 — done)

- Idempotent `memcmp` before `pushResultToEditorAndSynth` — apply in `auditionAfterHistoryMutation`.
- `auditionSelectedHistoryEntry` no-ops when compare active — irrelevant once `forceExitCompare` runs first.

### Previous story intelligence (6.6 — done)

- `syncHistoryUiProperties` **must** be called after manual store ops (review deferral 6.6) — **this story** implements that for delete/clear.
- `<EMPTY>` sentinel: `PluginDisplayNames::...::kEmptyHistorySentinel` — panel shows when `kHistoryMList` empty.
- Root-only R sentinel: `kHistoryRootSentinel` (`—`).

### Previous story intelligence (6.2 — done)

- `deleteRoot` erases entire bucket (root + all retries) — engine does not loop retries.
- `clear()` does not touch initial snapshot — test `clear_keepsInitialSnapshot`.
- Store returns `bool` on delete — engine should treat `false` as internal error (should not happen after `hasRoot`/`hasRetry` guards); return `kNoSelectionFooterMessage` defensively.

### Git intelligence

| Commit | Relevance |
|---|---|
| `87a2a97` | Story 6.8 — compare toggle, `compareSavedM_`/`compareSavedR_`, initial snapshot in mutate |
| `9a9eaef` | Story 6.7 — audition primitive, compare guard |
| `dd7acbc` | Story 6.6 — `syncHistoryUiProperties`, two-combo APVTS mirrors |

**Brownfield:** `deleteSelected()` / `clearHistory()` empty stubs at `PatchMutatorEngine.cpp:331-341`. Panel DELETE/CLEAR buttons already wired via `connectButtonToApvts` — no panel work needed until 7.4 handler delegation.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — no new dependencies.
- No web research required — FR-58 and D-084 specify behaviour.

### Story 7.4 handoff

When 7.4 wires DELETE/CLEAR buttons:

```cpp
MutatorActionResult MutatorActionHandler::handleDelete()
{
    auto result = engine_->deleteSelected();
    if (! result.success || result.footerMessage.isNotEmpty())
        propagateFooterMessage(result.footerMessage, result.footerSeverity);
    return result;
}
```

Same pattern for `handleClear()` → `clearHistory()`. SysEx already sent inside engine — handler must not duplicate.

### Story 6.12 handoff (awareness)

FR-60 DELETE enabled when entry selected — `refreshActionEnabledMirrors` will gray DELETE when `kSelectedM == -1` or history empty. Engine blocked paths already return footer messages; 6.12 mirrors engine predicates.

### Story 6.13 handoff (awareness)

Patch load will call `clearHistory()`-like path **plus** reset initial snapshot. Do not clear initial snapshot in 6.9 `clearHistory()` — 6.13 extends or adds `resetSessionForPatchLoad()`.

### Project context reference

- `project-context.md` — Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — panel `setProperty` only for actions; history mirrors from engine sync
- FR-58 DELETE and CLEAR
- FR-30 full patch SysEx on mutation actions
- D-084 DELETE / CLEAR selection fallback
- `deferred-work.md` — compare+delete deferral resolved in 6.9

### References

- [epics.md — Story 6.9]
- [prd.md — FR-58, FR-30]
- [addendum.md — DELETE / CLEAR]
- [.decision-log.md — D-084]
- [architecture.md — AD-6, AD-7, Mutator History UI sync]
- [6-8-compare-mode.md — compare deferral, forceExitCompare requirement]
- [6-7-history-selection-audition-with-debounce.md — audition idempotency]
- [6-6-history-m-and-r-ui-properties.md — syncHistoryUiProperties, sentinels]
- [6-2-mutationhistorystore-two-level-mr.md — deleteRoot cascade, clear vs snapshot]
- [7-4-mutatoractionhandler.md — handleDelete/handleClear delegation]
- [PatchMutatorEngine.cpp — stubs at 331-341, syncHistoryUiProperties, toggleCompare]
- [PatchMutatorPanel.cpp — deleteButton_, clearButton_ already wired]

## Dev Agent Record

### Agent Model Used

Composer

### Debug Log References

- Fixed `delete_whileCompareActive_noStaleRestore` test: root-only selection on M05 before compare enter so root delete fallback selects M00 (retry delete would fallback to M05 root-only, not prior root).

### Completion Notes List

- Implemented `deleteSelected()` with blocked paths (empty history, no selection), retry vs root delete branches, D-084 selection fallback via sorted list order, cascade footer with `formatRootLabel`, and post-delete sync + audition.
- Implemented `clearHistory()` purging store while preserving initial snapshot; shared `forceExitCompare()` (no `toggleCompare` exit branch) and `auditionAfterHistoryMutation()` with idempotent SysEx guard.
- Added 15 engine unit tests covering delete/clear blocked paths, selection fallback, compare teardown, APVTS sync, and initial-snapshot audition.
- Verified `MutatorActionHandler` remains stub — DELETE/CLEAR wiring deferred to Story 7.4.
- Full `Matrix-Control_Tests` target green.

### File List

- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp

## Change Log

- 2026-06-20 — Story 6.9 implemented: engine deleteSelected/clearHistory, compare force-exit, selection fallback, 15 unit tests.
- 2026-06-20 — Code review: 6 patch findings applied (footer test contracts, kHistoryRList/empty-list asserts, post-delete SysEx, cascade footer constexpr).
