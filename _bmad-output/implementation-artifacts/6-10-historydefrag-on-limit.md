---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.10 — HistoryDefrag on Limit
author: BMad Agent
status: done
baseline_commit: 8b2d916
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - implementation-artifacts/6-9-delete-and-clear-history.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/6-3-mutationnaming-and-display-names.md
  - implementation-artifacts/6-4-mutate-action.md
  - implementation-artifacts/6-5-retry-from-parent-snapshot.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - implementation-artifacts/7-7-settings-page-consolidation.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.10: HistoryDefrag on Limit

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want Defrag when M or R slots exhaust due to gaps,
so that I can continue mutating (FR-59, D-087).

## Acceptance Criteria

1. **Given** Story 6.2 (`MutationHistoryStore` gap-aware allocation) and Stories 6.4–6.5 (engine sets `defragModalRequested` on limit) **When** `HistoryDefragService` is implemented under `Source/Core/Services/PatchMutator/` **Then** it compacts session history to contiguous indices:
   - Roots renumbered **M00…** in ascending **list order** (not numeric gap-fill — sorted old roots → new 0…N−1).
   - Retries per root renumbered **R00…** in ascending list order for that root.
   - **Logical content preserved:** `result` and `parentSnapshot` buffers unchanged except patch name bytes 0–7 (updated via `MutationNaming::applyPatchName`).
   - `MutationEntry.rootIndex` / `retryIndex` fields updated to new indices.
   - Initial snapshot **unchanged** (AD-7 — not part of M/R lists).
   - Empty store → `success = false` (no-op).

2. **And** `PatchMutatorEngine::defragHistory()` replaces the stub at `PatchMutatorEngine.cpp:419-423`:
   - **Blocked** when `historyStore_.isEmpty()` → `{ success: false, footerMessage: kEmptyHistoryFooterMessage, footerSeverity: warning }`.
   - Calls `HistoryDefragService::defrag(historyStore_, selectionHint)` where `selectionHint` carries current `(selectedRootIndex_, selectedRetryIndex_)`.
   - **Selection remapping:** service returns new `(m, r)` for the entry that was selected before defrag; if selection was invalid/empty, clamp like `syncHistoryUiProperties` (highest root, root-only).
   - `forceExitCompare()` before store mutation (indices change — same rationale as 6.9 delete/clear).
   - `syncHistoryUiProperties(apvts_)` after success.
   - `auditionAfterHistoryMutation()` after success (reuse 6.9 helper — idempotent SysEx guard).
   - Returns `{ success: true, footerMessage: kDefragCompleteFooterMessage, footerSeverity: info }` (English user string, e.g. `"Mutation history renumbered."`).

3. **And** after successful defrag, allocation helpers unblock:
   - Scenario: single root at index **99** → `isRootIndexExhausted() == true`, `peekNextRootIndex()` nullopt.
   - After defrag → root at **0**, `peekNextRootIndex()` returns **1**, `isRootIndexExhausted() == false`.
   - Same policy for retry gap exhaustion (e.g. single retry at **99** under a root → after defrag retry at **0**, `peekNextRetryIndex` returns **1**).

4. **And** limit-block behaviour from Stories 6.4–6.5 remains unchanged:
   - `mutate()` / `retry()` still set `defragModalRequested = true` + `kHistoryLimitFooterMessage` when slots full or index exhausted.
   - This story does **not** remove or weaken existing `mutate_limitBlocks` / `retry_limitBlocks` tests.

5. **And** unit tests cover Core + engine without GUI:
   - **`HistoryDefragServiceTests`** (new) or extend `MutationHistoryStoreTests` if service is thin — prefer dedicated test file mirroring `MutationNamingTests` pattern.
   - **`PatchMutatorEngineTests`** — engine orchestration cases listed in Testing Requirements below.
   - Full `Matrix-Control_Tests` target remains green.

6. **And** this story delivers **`HistoryDefragService` + store hook + `defragHistory()` engine + unit tests only**:
   - **No** limit modal UI (**7.4** shows modal when handler sees `defragModalRequested`; engine flag already set).
   - **No** Settings manual Defrag button (**7.7** Phase B replaces placeholder — calls same `defragHistory()`).
   - **No** `MutatorActionHandler` wiring (**7.4**).
   - **No** auto-retry of blocked MUTATE/RETRY after defrag from modal (**7.4** re-invokes action on user confirm).

7. **And** `CMakeLists.txt` updated — register `HistoryDefragService.cpp` in plugin + test targets (mirror `MutationNaming.cpp` entries ~lines 129–132 and 369–372).

## Tasks / Subtasks

- [x] **Implement store compaction hook** (AC: #1, #3)
  - [x] Add package-private method on `MutationHistoryStore` (e.g. `friend class HistoryDefragService` or public `compactIndices()` returning old→new maps) — keep CRUD API stable
  - [x] Rebuild `roots_` map with contiguous keys; preserve entry buffers

- [x] **Implement `HistoryDefragService`** (AC: #1)
  - [x] `HistoryDefragService.h/.cpp` in `Source/Core/Services/PatchMutator/`
  - [x] `struct HistoryDefragResult { bool success; int remappedRootIndex; int remappedRetryIndex; }`
  - [x] `static HistoryDefragResult defrag(MutationHistoryStore& store, std::pair<int,int> selectedIndices)`
  - [x] After renumber: loop all entries → `MutationNaming::applyPatchName` on `result` buffer

- [x] **Implement `defragHistory()`** (AC: #2)
  - [x] Replace stub at `PatchMutatorEngine.cpp:419-423`
  - [x] Reuse `forceExitCompare()`, `auditionAfterHistoryMutation()`, `syncHistoryUiProperties`
  - [x] Add `kDefragCompleteFooterMessage` constant in anonymous namespace

- [x] **Unit tests** (AC: #5)
  - [x] `Tests/Unit/HistoryDefragServiceTests.cpp` (or equivalent)
  - [x] Extend `PatchMutatorEngineTests.cpp`

- [x] **CMake + self-review** (AC: #6, #7)
  - [x] Register new `.cpp` in plugin + test targets
  - [x] Grep — no GUI includes in Core service
  - [x] `cmake --build Builds/macOS --target Matrix-Control_Tests`

## Dev Notes

### What Story 6.10 IS — and what it is NOT

| In scope (6.10) | Out of scope |
|---|---|
| `HistoryDefragService` contiguous renumber (FR-59) | Limit **modal** UI (`AlertWindow` / custom dialog) — **7.4** |
| `MutationHistoryStore` compaction hook | Settings **manual Defrag** row — **7.7** Phase B |
| `PatchMutatorEngine::defragHistory()` | `MutatorActionHandler` action routing — **7.4** |
| Patch name re-apply after renumber (`MutationNaming`) | Auto re-run blocked MUTATE/RETRY after modal confirm — **7.4** |
| Selection remapping + APVTS sync + audition | `refreshActionEnabledMirrors` — **6.12** |
| Compare force-exit on defrag | EXPORT layout — **6.11** |
| Store + engine unit tests | Panel / Settings UI changes |

**Critical boundary:** Gap exhaustion detection and `defragModalRequested` already exist (6.4–6.5). This story delivers the **Compaction engine** that makes Defrag meaningful. End-to-end limit UX (modal → confirm → defrag → retry action) spans **6.10 + 7.4**.

### Defrag algorithm (implement exactly)

```
HistoryDefragService::defrag(store, {selectedM, selectedR}):

  if store.isEmpty():
    return { success: false }

  // Capture identity of selected entry BEFORE mutation (if valid)
  optional<MutationEntry> selectedEntry
  if selectedM >= 0 && store.hasRoot(selectedM):
    if selectedR == kRootOnly:
      selectedEntry = store.getEntry(selectedM, kRootOnly)
    else if store.hasRetry(selectedM, selectedR):
      selectedEntry = store.getEntry(selectedM, selectedR)

  oldRoots = store.getSortedRootIndices()
  newRootsMap: oldRoot -> newRoot (0..oldRoots.size-1)

  newStore = empty map
  for i, oldRoot in enumerate(oldRoots):
    newRoot = i
    copy root entry: update rootIndex=newRoot, retryIndex=kRootOnly
    applyPatchName(result, newRoot, kRootOnly)

    oldRetries = store.getSortedRetryIndices(oldRoot)
    for j, oldRetry in enumerate(oldRetries):
      newRetry = j
      copy retry entry: update indices, applyPatchName(result, newRoot, newRetry)

  replace store.roots_ with newStore

  // Remap selection by matching parentSnapshot+result bytes OR tracking old->new maps
  remapped = resolveRemappedSelection(selectedEntry, newStore) // prefer map-based, not memcmp

  return { success: true, remappedRootIndex, remappedRetryIndex }
```

**Prefer explicit old→new maps during rebuild** over buffer memcmp — entries could theoretically collide in tests.

**Do not** fill deleted slots differently from full compaction — always renumber **all** remaining entries.

### `defragHistory()` algorithm (implement exactly)

```
defragHistory():
  if historyStore_.isEmpty():
    return blocked (kEmptyHistoryFooterMessage, warning)

  forceExitCompare()

  const selection = { selectedRootIndex_, selectedRetryIndex_ }
  const auto defragResult = HistoryDefragService::defrag(historyStore_, selection)

  if !defragResult.success:
    return { success: false }  // defensive — empty already guarded

  selectedRootIndex_ = defragResult.remappedRootIndex
  selectedRetryIndex_ = defragResult.remappedRetryIndex
  syncHistoryUiProperties(apvts_)
  auditionAfterHistoryMutation()

  return { success: true, footerMessage: kDefragCompleteFooterMessage, severity: info }
```

### Post-defrag selection remapping rules

| Before defrag | After defrag |
|---|---|
| Valid root-only `(M05, kRootOnly)` | Same logical root → new index in compacted order (e.g. M05 was 2nd root → `(1, kRootOnly)`) |
| Valid retry `(M05, R03)` | Same logical retry → new `(newM, newR)` |
| `selectedM == -1` | Remain `-1` or clamp to highest root per `syncHistoryUiProperties` — **pick one** and test it; recommend: set remapped to highest root + root-only if history non-empty |
| Compare was active | Already forced off; do not restore `compareSavedM_`/`compareSavedR_` |

### Existing code to extend (do not rewrite)

**Limit detection already wired** — do not duplicate:

```109:116:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
    if (historyStore_.isRootSlotsFull() || historyStore_.isRootIndexExhausted())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }
```

**Stub to replace:**

```419:423:Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
MutatorActionResult PatchMutatorEngine::defragHistory()
{
    MutatorActionResult result;
    return result;
}
```

**Gap exhaustion in store** (reference for tests):

```248:259:Tests/Unit/MutationHistoryStoreTests.cpp
    void gapExhausted_singleAt99()
    {
        // single root at 99 → peekNextRootIndex nullopt, isRootIndexExhausted true
    }
```

**Reuse from 6.9:** `forceExitCompare()`, `auditionAfterHistoryMutation()`, `syncHistoryUiProperties`, footer constant pattern in anonymous namespace.

### File structure (this story)

```
Source/Core/Services/PatchMutator/
├── MutationHistoryStore.h          (UPDATE — defrag hook / friend)
├── MutationHistoryStore.cpp        (UPDATE — compaction implementation)
├── HistoryDefragService.h          (NEW)
├── HistoryDefragService.cpp        (NEW)
├── PatchMutatorEngine.cpp          (UPDATE — defragHistory)
└── PatchMutatorEngine.h            (UPDATE only if helper needed)

Tests/Unit/
├── HistoryDefragServiceTests.cpp   (NEW)
└── PatchMutatorEngineTests.cpp     (UPDATE)

CMakeLists.txt                      (UPDATE)
```

No GUI files. No handler files.

### CMake registration

Mirror `MutationNaming.cpp` pattern:

```cmake
# Plugin target (~line 129)
Source/Core/Services/PatchMutator/HistoryDefragService.cpp

# Test target (~lines 308–312 sources + ~369 compile)
Tests/Unit/HistoryDefragServiceTests.cpp
Source/Core/Services/PatchMutator/HistoryDefragService.cpp
```

### Testing requirements

**HistoryDefragServiceTests** (or store tests for compaction):

| Test | Setup | Assert |
|---|---|---|
| `defrag_emptyStore_fails` | Empty store | `success == false` |
| `defrag_singleAt99_renumbersToZero` | Root at index 99 | After: `hasRoot(0)`, `!hasRoot(99)`, `peekNextRootIndex() == 1` |
| `defrag_rootsCompactInListOrder` | Roots at 0, 5, 99 | After: indices `[0,1,2]` |
| `defrag_retriesCompactPerRoot` | Root 0 retries at 0, 3, 7 | After: retries `[0,1,2]` |
| `defrag_preservesBufferPayload` | Distinct byte at offset ≥ 8 | Unchanged after defrag |
| `defrag_updatesPatchNames` | Named M99 / M05-R03 | After: names match new indices |
| `defrag_keepsInitialSnapshot` | Snapshot set + history | `hasInitialSnapshot()` still true; snapshot bytes unchanged |
| `defrag_retryGapAt99_unblocksPeek` | Retry at 99 only | After: retry at 0, `peekNextRetryIndex(0) == 1` |

**PatchMutatorEngineTests** — extend `EngineHarness`:

| Test | Setup | Assert |
|---|---|---|
| `defrag_emptyHistory_blocked` | Empty store | `!success`, footer warning, 0 SysEx |
| `defrag_gapExhaustion_thenMutateUnblocked` | Root at 99, recipe set | `mutate()` blocked + modal flag; `defragHistory()` success; `mutate()` success |
| `defrag_remapsSelectedRoot` | Root at 99 selected | After defrag: `kSelectedM == 0` |
| `defrag_remapsSelectedRetry` | M00 + R99 selected | After defrag: `kSelectedR == 0` (if only retry) |
| `defrag_syncsApvtsLists` | Gapped roots | `kHistoryMList` shows `M00\|M01\|...` contiguous labels |
| `defrag_disablesCompare` | Compare active | `kCompareActive == false` after defrag |
| `defrag_auditionsRemappedSelection` | Non-empty history | 1× 0x01 matching remapped entry |
| `defrag_successFooter` | Any non-empty | `footerSeverity == info`, message non-empty |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **AD-6** — `HistoryDefragService` as sibling under `Services/PatchMutator/` per architecture diagram.
- **Core ↛ GUI** — no `juce_gui_*`, no `AlertWindow` in Core.
- **AD-7** — initial snapshot not renumbered; session RAM only.
- **D-087** — gaps lost on defrag; max+1 allocation restored afterward.
- **FR-59** — contiguous M/R renumber + name update + footer confirmation (modal/Settings wiring deferred).
- English source/comments only.
- Functions ≤ 15 lines — extract helpers in service/engine.

### Previous story intelligence (6.9 — done)

- `forceExitCompare()` clears `compareSavedM_`/`compareSavedR_` without calling `toggleCompare()` exit branch — **reuse on defrag**.
- `auditionAfterHistoryMutation()` + idempotent `memcmp` SysEx guard — **reuse after defrag**.
- `syncHistoryUiProperties` **must** run after every store mutation (6.6 review deferral) — **call after defrag**.
- `kEmptyHistoryFooterMessage` already defined — reuse for blocked defrag.

### Previous story intelligence (6.4–6.5 — done)

- `kHistoryLimitFooterMessage` + `defragModalRequested` on root/retry exhaustion — **do not change**.
- `mutate_gapAllocation` test proves gaps work pre-defrag; add post-defrag continuation test in 6.10.
- After defrag, `mutate()` should allocate next contiguous index (not reopen old gaps).

### Previous story intelligence (6.3 — done)

- Defrag **must** call `MutationNaming::applyPatchName` on every entry's `result` buffer — store holds post-name buffers for EXPORT/audition.
- Root-only: `applyPatchName(model, newRoot, kRootOnly)`; retry: `applyPatchName(model, newRoot, newRetry)`.

### Previous story intelligence (6.2 — done)

- Internal `std::map<int, RootBucket>` — compaction rebuilds map keys.
- `peekNextIndex` policy: `maxExisting + 1` — defrag lowers max so allocation resumes.
- Review note: 6.2 deferred `HistoryDefragService` to this story — implement now, do not add parallel history structures.

### Git intelligence

| Commit | Relevance |
|---|---|
| `8b2d916` | Story 6.9 — `forceExitCompare`, `auditionAfterHistoryMutation`, delete/clear orchestration patterns |
| `87a2a97` | Story 6.8 — compare state; defrag must force-exit |
| `d933ea3` | Story 6.5 — retry limit + `defragModalRequested` |
| Earlier 6.4 | Mutate limit + gap allocation |

**Brownfield:** `defragHistory()` empty stub at `PatchMutatorEngine.cpp:419-423`. `HistoryDefragService` **does not exist yet** — create per AD-6. Settings Defrag row is placeholder `"Coming soon"` (`SettingsPanel.cpp:31`).

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — no new dependencies.
- No web research required — FR-59 and D-087 fully specify behaviour.
- Modal pattern reference for **7.4** (do not implement here): `MasterInitConfirmDialog` + `PluginEditor` gate (`openMasterInitConfirmDialog`) or `juce::AlertWindow` like patch name reconciliation (`PluginEditor.cpp:127-137`).

### Story 7.4 handoff (limit modal flow)

When handler wires MUTATE/RETRY:

```cpp
auto result = engine_->mutate(); // or retry()
propagateFooterMessage(result.footerMessage, result.footerSeverity);
if (result.defragModalRequested)
    showDefragLimitModal(
        [this] {
            auto defrag = engine_->defragHistory();
            propagateFooterMessage(defrag.footerMessage, defrag.footerSeverity);
            // Optionally re-invoke pending mutate/retry — product decision in 7.4
        },
        [] { /* Cancel — no-op */ });
```

Handler **must not** call `AlertWindow` from Core — modal stays GUI layer per 7-4 dev notes.

### Story 7.7 handoff (Settings manual Defrag)

Replace `defragPlaceholder_` with action button (Phase B):

- Disabled when `engine.rootCount() == 0` (FR-59).
- Confirmation dialog → `engine.defragHistory()` (same engine method as limit path).
- Footer via existing `uiMessageText` propagation pattern.

### Story 6.12 handoff (awareness)

After defrag, MUTATE/RETRY should become unblocked — `refreshActionEnabledMirrors` (6.12) should reflect `!isRootIndexExhausted()` predicates. No work in 6.10.

### Project context reference

- `project-context.md` — Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — no French in source; panel `setProperty` only for actions
- FR-59 Defrag renumber + footer
- D-087 gap exhaustion → modal (UI in 7.4) + manual Settings (7.7)
- AD-6 PatchMutatorEngine composition

### References

- [epics.md — Story 6.10]
- [prd.md — FR-59]
- [addendum.md — Defrag (FR-59, D-087)]
- [.decision-log.md — D-087]
- [architecture.md — AD-6 HistoryDefragService, AD-7, AD-8 Defrag tests]
- [6-2-mutationhistorystore-two-level-mr.md — gap policy, peekNext*, deferred defrag hook]
- [6-3-mutationnaming-and-display-names.md — applyPatchName after renumber]
- [6-4-mutate-action.md — defragModalRequested, defragHistory stub]
- [6-5-retry-from-parent-snapshot.md — retry exhaustion modal flag]
- [6-9-delete-and-clear-history.md — forceExitCompare, auditionAfterHistoryMutation]
- [7-4-mutatoractionhandler.md — modal GUI layer, handler mapping]
- [7-7-settings-page-consolidation.md — Phase B manual Defrag deferred]
- [PatchMutatorEngine.cpp — limit checks, defragHistory stub]
- [MutationHistoryStore.cpp — map structure, peekNextIndex]
- [MutationHistoryStoreTests.cpp — gapExhausted_singleAt99, gapExhausted_retryAt99]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Build: `cmake --build Builds/macOS --target Matrix-Control_Tests`
- Run: `Builds/macOS/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests` — all tests green

### Completion Notes List

- Added `HistoryDefragService` with map-based old→new index remapping, patch name re-apply via `MutationNaming::applyPatchName`, and selection remapping for valid M/R or `-1` (clamped by `syncHistoryUiProperties`).
- Added `MutationHistoryStore::replaceRootsForDefrag` (friend access) to rebuild contiguous `roots_` map without changing public CRUD API.
- Replaced `PatchMutatorEngine::defragHistory()` stub: empty guard, `forceExitCompare`, defrag, APVTS sync, audition, info footer.
- 10 service tests + 8 engine defrag tests; existing `mutate_limitBlocks` / `retry_limitBlocks` unchanged.

### File List

- Source/Core/Services/PatchMutator/HistoryDefragService.h (new)
- Source/Core/Services/PatchMutator/HistoryDefragService.cpp (new)
- Source/Core/Services/PatchMutator/MutationHistoryStore.h (modified)
- Source/Core/Services/PatchMutator/MutationHistoryStore.cpp (modified)
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp (modified)
- Tests/Unit/HistoryDefragServiceTests.cpp (new)
- Tests/Unit/PatchMutatorEngineTests.cpp (modified)
- CMakeLists.txt (modified)

## Change Log

- 2026-06-20 — Story 6.10 created: HistoryDefragService, store compaction, engine defragHistory(), selection remap, test plan; modal/Settings/handler deferred to 7.4/7.7.
- 2026-06-20 — Implemented HistoryDefragService, store compaction hook, defragHistory() orchestration, unit tests; story ready for review.
- 2026-06-20 — Code review: fixed invalid-retry selection clamp, strengthened tests, all patch findings resolved; story done.

### Review Findings

- [x] [Review][Patch] Invalid retry with valid root jumps to highest root instead of root-only clamp — `captureSelectedIndices` returns nullopt when retry is stale; `syncHistoryUiProperties` would keep M and reset R to kRootOnly [HistoryDefragService.cpp:40-43]
- [x] [Review][Patch] Remove dead `selectedEntry` capture and `juce::ignoreUnused` — index-map remapping is correct; leftover variable is misleading [HistoryDefragService.cpp:91-94]
- [x] [Review][Patch] Add engine test for `selectedM == -1` clamp after defrag — AC2 requires tested policy; behavior relies on `syncHistoryUiProperties` but is unverified [PatchMutatorEngineTests.cpp]
- [x] [Review][Patch] Assert `remappedRootIndex == 1` in `defrag_rootsCompactInListOrder` when selection is M05 — spec table example M05→(1, kRootOnly) not evidenced [HistoryDefragServiceTests.cpp:91-107]
- [x] [Review][Patch] Assert `parentSnapshot` bytes unchanged after defrag — AC1 logical-content preservation only tested on `result` buffer [HistoryDefragServiceTests.cpp:128-142]
- [x] [Review][Patch] Assert exact `kDefragCompleteFooterMessage` string in `defrag_successFooter` — AC2 specifies exact English message [PatchMutatorEngineTests.cpp:1553-1556]
- [x] [Review][Defer] `HistoryDefragService::defrag()` exceeds 15-line function limit — architecture guideline; extract compaction loop helper [HistoryDefragService.cpp:82-144] — deferred, style debt
- [x] [Review][Defer] `PatchMutatorEngine::defragHistory()` exceeds 15-line function limit — same architecture guideline [PatchMutatorEngine.cpp:421-449] — deferred, style debt
