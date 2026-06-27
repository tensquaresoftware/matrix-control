---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.5 — RETRY from Parent Snapshot
author: BMad Agent
status: done
baseline_commit: 5284dd9
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md
  - implementation-artifacts/6-4-mutate-action.md
  - implementation-artifacts/6-3-mutationnaming-and-display-names.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-20
updated: 2026-06-20
---

# Story 6.5: RETRY from Parent Snapshot

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want RETRY to branch from parent snapshot of selected entry,
so that Absynth-style retries do not cascade-delete siblings (FR-56, FR-30).

## Acceptance Criteria

1. **Given** Story 6.4 complete (`PatchMutatorEngine::mutate()` implemented and tested) **When** `PatchMutatorEngine::retry()` is implemented **Then** it orchestrates the full RETRY pipeline per D-083 / FR-56 with **no GUI dependencies** and **no handler wiring** (7.4 deferred).

2. **And** RETRY requires **non-empty** history — empty store returns `{ success: false, footerMessage }` (English warning); **no** store insert, **no** SysEx. Aligns with FR-60 (RETRY enabled only when history non-empty — enabled-state mirrors are Story 6.12).

3. **And** selected root **Mi** resolves from `setAuditionSelection` / `selectedRootIndex_` + `selectedRetryIndex_`:
   - Non-empty history + explicit selection → use `selectedRootIndex_` (and `selectedRetryIndex_` only to pick **which entry's `parentSnapshot`** — retry always lands under the **same Mi**)
   - Non-empty history + `selectedRootIndex_ < 0` → safe default: highest sorted root index (same fallback as `resolveAuditionBuffer()`)
   - Missing root at resolved index → `{ success: false, footerMessage }`

4. **And** algorithm input is **`parentSnapshot`** of the resolved selected entry — **not** `result`:
   - Selection root-only (`kRootOnly`) → `getEntry(Mi, kRootOnly)->parentSnapshot`
   - Selection `Mi-Rj` → `getEntry(Mi, Rj)->parentSnapshot`
   - **Do not** call `resolveAuditionBuffer()` for RETRY (that returns `result` — MUTATE-only semantics)

5. **And** RETRY pipeline (normative — mirror `mutate()` structure):
   - If `isRetrySlotsFull(rootIndex)` OR `isRetryIndexExhausted(rootIndex)` → failure + `defragModalRequested = true` (D-087)
   - `recipe = buildRecipeFromApvts()`; if `amountPercent == 0` OR `randomPercent == 0` → failure footer (no insert, no SysEx)
   - `inputBuffer` = copy of selected entry's `parentSnapshot`
   - `parentSnapshot` = copy of `inputBuffer` (state immediately before this retry is created)
   - `working = PatchModel(inputBuffer)`; `algorithm.apply(working, recipe, rng_)` with fresh `rng_.setSeedRandomly()` per click (spec §8.1)
   - If algorithm returns `false` → failure with `kNoMutationChangeFooterMessage` (reuse existing constant)
   - `retryIndex = peekNextRetryIndex(Mi)`; on failure → failure + `defragModalRequested`
   - `MutationNaming::applyPatchName(working, Mi, retryIndex)` **after** algorithm
   - `store.insertRetry(Mi, retryIndex, working, parentSnapshot)`
   - `pushResultToEditorAndSynth(working)` — one full patch SysEx **0x01** via existing helper
   - Return `{ success: true }`

6. **And** first RETRY on root **M05** (no prior retries) creates **`M05-R00`** at retry index **0**; subsequent RETRY on same Mi allocates `max(existing R)+1` with gaps preserved (store `peekNextRetryIndex` — do not duplicate allocation logic).

7. **And** RETRY **never deletes** existing roots or sibling retries — only `insertRetry`; preloaded `Mi-R00` and `Mi-R02` both remain after new retry.

8. **And** unit tests in `Tests/Unit/PatchMutatorEngineTests.cpp` cover:
   - `retry_emptyHistory_blocked` — no insert, no SysEx
   - `retry_firstRetry_createsR00` — M00 + RETRY → `hasRetry(0, 0)`, name bytes `M00-R00`
   - `retry_usesParentSnapshot_notResult` — distinct `result` vs `parentSnapshot` on root; RETRY output parent chain references `parentSnapshot` byte markers, not `result` markers
   - `retry_fromSelectedRetry_usesThatEntryParentSnapshot` — select `Mi-R00`, RETRY → new retry's stored `parentSnapshot` matches **R00 entry's** `parentSnapshot`, not root's
   - `retry_gapAllocation` — delete retry, next RETRY reuses gap index
   - `retry_limitBlocks` — 100 retries on one Mi → `success == false`, `defragModalRequested == true`
   - `retry_noOpRecipe_blocked` — A=0 → no retry inserted, no SysEx
   - `retry_sendsSysExOnce` — fake queue records exactly one 0x01 patch SysEx
   - `retry_neverDeletesExistingRetries` — R00 + R02 present; RETRY → both still present + new retry
   - `retry_staysUnderSameRoot` — RETRY never calls `insertRoot`; `rootCount()` unchanged
   - Full `Matrix-Control_Tests` Debug target remains green

9. **And** this story delivers **`retry()` implementation + tests only** — no `MutatorActionHandler` wiring (**7.4**), no APVTS `historyMList`/`selectedM` sync (**6.6**), no debounced history audition (**6.7**), no COMPARE/DELETE/CLEAR/EXPORT (**6.8–6.11**), no recipe prefs persistence (**6.12**), no clear-on-patch-load (**6.13**), no new CMake entries (engine `.cpp` already registered in 6.4).

## Tasks / Subtasks

- [x] **Implement `resolveSelectedEntryForRetry()`** (AC: #3–#4)
  - [x] Private helper in `PatchMutatorEngine` — returns `std::optional<MutationEntry>` for selected Mi / Mi-Rj
  - [x] Mirror root-index fallback from `resolveAuditionBuffer()` but read **`parentSnapshot`** field from entry
  - [x] Document in header: RETRY input semantics differ from MUTATE audition semantics (D-083)

- [x] **Implement `PatchMutatorEngine::retry()`** (AC: #1–#7)
  - [x] Replace stub body in `PatchMutatorEngine.cpp`
  - [x] Reuse `buildRecipeFromApvts`, `pushResultToEditorAndSynth`, footer constants from `mutate()`
  - [x] Optional: extract shared pre-check helper for no-op recipe if `mutate()` and `retry()` duplicate — only if it stays ≤15 lines and does not widen scope

- [x] **Unit tests** (AC: #8)
  - [x] Extend `PatchMutatorEngineTests::runTest()` with retry test cases listed in AC #8
  - [x] Reuse `EngineHarness`, `makeDistinctBuffer`, `countPatchSysExMessages` patterns from mutate tests
  - [x] Run `cmake --build Builds/macOS --target Matrix-Control_Tests`

- [x] **Self-review** (AC: #9)
  - [x] No GUI / panel / handler changes
  - [x] Grep `resolveAuditionBuffer` — not used inside `retry()`
  - [x] Grep `insertRoot` — not called from `retry()`
  - [x] Algorithm unchanged — same `MutationAlgorithm::apply` as MUTATE
  - [x] Naming only via `MutationNaming::applyPatchName` with retry index

## Dev Notes

### What Story 6.5 IS — and what it is NOT

| In scope (6.5) | Out of scope |
|---|---|
| `PatchMutatorEngine::retry()` full orchestration | `MutatorActionHandler` delegation (**7.4**) |
| `resolveSelectedEntryForRetry()` private helper | History M/R APVTS properties (**6.6**) |
| RETRY unit tests in `PatchMutatorEngineTests` | Debounced history audition (**6.7**) |
| Distinct footer messages for empty history / missing root | COMPARE / DELETE / CLEAR / EXPORT (**6.8–6.11**) |
| Reuse mutate push/SysEx/recipe patterns | Enabled-state button graying (**6.12**) |
| | Clear history on patch load (**6.13**) |
| | `MutationAlgorithm` changes (already complete) |
| | `MutationNaming` changes (retry names already in 6.3) |

**Critical boundary:** RETRY button routing stays in empty `MutatorActionHandler` until **7.4**. This story makes `retry()` callable and tested — same pattern as 6.4 for `mutate()`.

### MUTATE vs RETRY — do not conflate (D-083)

| Aspect | MUTATE (`mutate()`) | RETRY (`retry()`) |
|---|---|---|
| History required | No (empty → live editor) | **Yes** (empty → blocked) |
| Algorithm input | Selected entry **`result`** (audition buffer) | Selected entry **`parentSnapshot`** |
| Store operation | `insertRoot` → new **Mi** | `insertRetry` → same **Mi**, new **R** |
| Index allocation | `peekNextRootIndex()` | `peekNextRetryIndex(Mi)` |
| Naming | `applyPatchName(working, rootIndex)` | `applyPatchName(working, rootIndex, retryIndex)` |
| Deletes siblings | Never | Never |

**Absynth semantics:** RETRY re-draws randomness from the **pre-mutation** state of the selected entry. Multiple retries under the same Mi explore variants without cumulative drift from the selected **result**.

### RETRY pipeline (normative — implement exactly)

```
retry():
  1. If historyStore_.isEmpty() → return failure footer (empty history)
  2. rootIndex = resolveSelectedRootIndex()   // same fallback rules as mutate audition root
  3. If !hasRoot(rootIndex) → return failure footer
  4. If isRetrySlotsFull(rootIndex) OR isRetryIndexExhausted(rootIndex)
       → return failure + defragModalRequested
  5. selectedEntry = resolveSelectedEntryForRetry(rootIndex, selectedRetryIndex_)
     If !selectedEntry → return failure footer
  6. recipe = buildRecipeFromApvts()
  7. If recipe.amountPercent == 0 OR recipe.randomPercent == 0 → return failure (no insert, no SysEx)
  8. inputBuffer = PatchModel(selectedEntry->parentSnapshot)
  9. parentSnapshot = copy(inputBuffer)
 10. working = PatchModel(inputBuffer)
 11. rng_.setSeedRandomly(); algorithm.apply(working, recipe, rngSource)
 12. If !mutated → return kNoMutationChangeFooterMessage
 13. retryIndex = peekNextRetryIndex(rootIndex)  // must succeed if step 4 passed
 14. MutationNaming::applyPatchName(working, rootIndex, retryIndex)
 15. insertRetry(rootIndex, retryIndex, working, parentSnapshot)
 16. pushResultToEditorAndSynth(working)
 17. return { success: true }
```

### `resolveSelectedEntryForRetry()` rules

| `selectedRetryIndex_` | Entry used for `parentSnapshot` source |
|---|---|
| `kRootOnly` (-1) | `getEntry(Mi, kRootOnly)` |
| `0..99` | `getEntry(Mi, selectedRetryIndex_)` if exists; else fallback to root-only entry |

Root index resolution (when `selectedRootIndex_ < 0`): `getSortedRootIndices().getLast()` — identical to `resolveAuditionBuffer()` lines 244–252.

### Parent snapshot stored on new retry

Same contract as MUTATE: `parentSnapshot` on the **new** entry = buffer state **immediately before** `algorithm.apply`. Because RETRY input **is** already the selected entry's `parentSnapshot`, the stored `parentSnapshot` on the new retry equals the pre-mutation working copy (byte-identical to algorithm input unless algorithm is a no-op, which is rejected before insert).

### Footer messages

Reuse existing anonymous-namespace constants in `PatchMutatorEngine.cpp`:

| Case | Message / behaviour |
|---|---|
| Empty history | New constant e.g. `kEmptyHistoryFooterMessage` — "Mutation history is empty." |
| Missing root / entry | New constant e.g. `kNoSelectionFooterMessage` |
| No-op recipe | `kNoOpRecipeFooterMessage` (existing) |
| Algorithm no change | `kNoMutationChangeFooterMessage` (existing) |
| Retry limit / exhaustion | `kHistoryLimitFooterMessage` + `defragModalRequested` (existing) |

All `footerSeverity = kFooterSeverityWarning`. English only.

### Store API (already implemented — 6.2)

Do **not** reimplement gap logic:

```cpp
std::optional<int> peekNextRetryIndex(int rootIndex) const noexcept;
bool insertRetry(int rootIndex, int retryIndex, const PatchModel& result, const PatchModel& parentSnapshot);
bool isRetrySlotsFull(int rootIndex) const noexcept;
bool isRetryIndexExhausted(int rootIndex) const noexcept;
int retryCount(int rootIndex) const noexcept;
bool hasRetry(int rootIndex, int retryIndex) const noexcept;
```

`peekNextRetryIndex` returns `0` for first retry (R00) when no retries exist — verified in `MutationHistoryStoreTests`.

### Naming (already implemented — 6.3)

```cpp
MutationNaming::applyPatchName(working, rootIndex, retryIndex);
// M05 + retry 0 → bytes 0-7 "M05-R00" (7 chars)
```

No changes to `MutationNaming` in this story.

### Algorithm (already implemented — 6.4)

`MutationAlgorithm::apply` is **stateless** relative to MUTATE vs RETRY — only the engine chooses input buffer. Spec §8.1: fresh `juce::Random` per click; RETRY draws fresh values from parent snapshot.

### APVTS → editor → SysEx push

Identical to `mutate()` — call existing `pushResultToEditorAndSynth(working)`:

1. `memcpy` into `*patchModel_`
2. `pushPatchModelToApvtsWithSuppress`
3. `midiManager_->sendPatch` with `getCurrentPatchNumber_()` callback

No handler or panel involvement.

### File structure (this story)

```
Source/Core/Services/PatchMutator/
├── PatchMutatorEngine.h            (UPDATE — private helper + header comment)
├── PatchMutatorEngine.cpp          (UPDATE — implement retry())
└── (no new files)

Tests/Unit/
├── PatchMutatorEngineTests.cpp     (UPDATE — retry test cases)
```

### Testing requirements

Extend `PatchMutatorEngineTests` using existing harness. Suggested assertions:

| Test | Key assert |
|---|---|
| `retry_emptyHistory_blocked` | `rootCount()==0`, `retryCount(0)==0`, SysEx count 0 |
| `retry_firstRetry_createsR00` | Preload M00 via mutate or `insertRoot`; `retry()` → `hasRetry(0,0)`, name `M00-R00` |
| `retry_usesParentSnapshot_notResult` | Root: `parentSnapshot[8]=0xAA`, `result[8]=0xBB`; RETRY → new retry's `parentSnapshot[8]==0xAA` |
| `retry_fromSelectedRetry_usesThatEntryParentSnapshot` | R00 parent marker ≠ root parent marker; select R00; RETRY → new entry parent matches R00 parent |
| `retry_gapAllocation` | Insert R00, delete R00, RETRY → index 0 reused |
| `retry_limitBlocks` | Insert 100 retries on Mi; RETRY → `!success`, `defragModalRequested` |
| `retry_noOpRecipe_blocked` | `setRecipe(0,100)`; RETRY → no new retry |
| `retry_sendsSysExOnce` | Success path → one 0x01 SysEx in queue |
| `retry_neverDeletesExistingRetries` | R00 + R02 exist; after RETRY both `hasRetry` still true |
| `retry_staysUnderSameRoot` | `rootCount()` same before/after; `hasRoot(Mi)` unchanged |

**Marker bytes:** use offset 8+ for test markers — bytes 0–7 are PATCH NAME (naming overwrites on insert).

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Architecture compliance

- **Core ↛ GUI** — no `juce_gui_*`, no `PluginProcessor` edits
- Path: `Source/Core/Services/PatchMutator/` per AD-6
- SysEx orchestration in engine, not panel or handler (AD-4, AD-5)
- Descriptor-driven mutation — algorithm unchanged; no new offset tables
- English source/comments only
- Message-thread only (same contract as `MutationHistoryStore`)

### Previous story intelligence

**6.4 (done):** `mutate()` pipeline is the template — copy structure, swap input resolution, `insertRetry`, retry naming. `resolveAuditionBuffer()` returns **result** — **never** use for RETRY. Stub `retry()` returns `{ success: false }` today. Review deferred: long `mutate()`, 7-param constructor, thread-safety doc — do not regress mutate tests when adding retry.

**6.3 (done):** `applyPatchName(model, rootIndex, retryIndex)` for `Mxx-Ryy`. Max 7 chars for retry names.

**6.2 (done):** `insertRetry` requires existing root. `peekNextRetryIndex` gap-aware. `parentSnapshot` stored per entry independently.

### Git intelligence

| Commit | Relevance |
|---|---|
| `5284dd9` | Latest — Story 6-4 `MutationAlgorithm` + `PatchMutatorEngine::mutate()` |
| `0f2078d` | Story 6-3 — `MutationNaming` |
| `74a025b` | Story 6-2 — `MutationHistoryStore` |

**Brownfield:** `retry()` is a stub at `PatchMutatorEngine.cpp:152-156`. Implement in place; tests extend existing file.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — no new dependencies
- `PatchModel::kBufferSize` == 134 bytes
- No web research required — RETRY I/O fully specified in PRD addendum and D-083

### Project context reference

- `project-context.md` — English artifacts; Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — reuse existing helpers; avoid widening 6.4 scope
- FR-30 full patch SysEx on RETRY; FR-56 parent-snapshot input; FR-59 defrag modal on retry limit
- D-083 RETRY semantics; D-087 defrag on blocked RETRY

### References

- [epics.md — Story 6.5]
- [prd.md — FR-56]
- [addendum.md — RETRY (FR-56), parentSnapshot field]
- [.decision-log.md — D-083, D-085, D-087]
- [mutation-algorithm-spec.md — §8.1 RETRY randomness; Story 6.5 row in traceability table]
- [6-4-mutate-action.md — mutate pipeline template, boundary table, stub API]
- [6-3-mutationnaming-and-display-names.md — retry naming]
- [6-2-mutationhistorystore-two-level-mr.md — insertRetry / peekNextRetryIndex]
- [7-4-mutatoractionhandler.md — handler wires retry() when Epic 6 complete]
- [PatchMutatorEngine.cpp — mutate() reference implementation]
- [MutationHistoryStore.h — store retry API]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- Implemented `resolveSelectedRootIndex()` + `resolveSelectedEntryForRetry()` — RETRY reads `parentSnapshot`, not `result` (D-083).
- `retry()` mirrors `mutate()` pipeline with `insertRetry` / retry naming; no `resolveAuditionBuffer` or `insertRoot`.
- Added 11 retry unit tests; all PatchMutatorEngine tests pass; full `Matrix-Control_Tests` Debug target green.

### Completion Notes List

- ✅ `PatchMutatorEngine::retry()` — full RETRY pipeline from parent snapshot (FR-56, D-083)
- ✅ Empty history, missing root/entry, no-op recipe, algorithm no-op, retry limit — all return appropriate footers
- ✅ `defragModalRequested` on retry slot exhaustion (D-087)
- ✅ 11 new unit tests covering AC #8; existing mutate tests unchanged

### File List

- Source/Core/Services/PatchMutator/PatchMutatorEngine.h
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp
- Tests/Unit/PatchMutatorEngineTests.cpp

### Change Log

- 2026-06-20: Story 6.5 created — RETRY pipeline from parentSnapshot, MUTATE vs RETRY contrast table, test matrix, and Epic 6 boundary table.
- 2026-06-20: Implemented `retry()`, `resolveSelectedRootIndex()`, `resolveSelectedEntryForRetry()` + 11 unit tests.
- 2026-06-20: Code review — no patch or decision items; test-depth deferrals recorded.

### Review Findings

- [x] [Review][Defer] Footer messages not asserted in retry tests [`PatchMutatorEngineTests.cpp`] — deferred, test-depth gap (AC8)
- [x] [Review][Defer] `retry_usesParentSnapshot_notResult` does not prove algorithm input source [`PatchMutatorEngineTests.cpp:354-356`] — deferred, stored parentSnapshot always matches input; stronger assertion optional
- [x] [Review][Defer] Missing-root failure path untested [`PatchMutatorEngine.cpp:165-170`] — deferred, test gap (AC3)
- [x] [Review][Defer] Highest-sorted-root fallback untested [`PatchMutatorEngine.cpp:369-373`] — deferred, test gap (AC3)
- [x] [Review][Defer] `retry_noOpRecipe_blocked` covers Amount=0 only [`PatchMutatorEngineTests.cpp:441`] — deferred, Random=0 path unverified (AC5)

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **done**
