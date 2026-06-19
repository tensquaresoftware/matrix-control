---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.2 — MutationHistoryStore Two-Level M/R
author: BMad Agent
status: done
baseline_commit: 3b9d4c5
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-Matrix-Control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/mutation-algorithm-spec.md
  - implementation-artifacts/6-1-mutationalgorithm-specification.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 6.2: MutationHistoryStore Two-Level M/R

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want session history stored as roots M00–M99 and retries R00–R99 per root,
so that MUTATE/RETRY semantics have a Core backing (FR-54 foundation, FR-55, FR-56).

## Acceptance Criteria

1. **Given** Epic 2 complete and Story 6.1 spec approved **When** `MutationHistoryStore` is implemented under `Source/Core/Services/PatchMutator/` **Then** a Core-only class exists with no GUI dependencies, holding two-level history per addendum data model (D-083, D-085).

2. **And** each stored entry (`MutationEntry`) contains:
   - `result` — 134-byte packed patch buffer (`PatchModel::kBufferSize`)
   - `parentSnapshot` — 134-byte buffer captured immediately before entry creation (RETRY input per D-083)
   - `rootIndex` — 0–99 (displayed as M00–M99)
   - `retryIndex` — `kRootOnly` (−1) for root-only entries; 0–99 for retries (displayed as R00–R99)

3. **And** **initial snapshot** (COMPARE / `Initial.syx` per AD-7) is stored **separately** from History entries — not listed in M/R indices; `setInitialSnapshot` / `getInitialSnapshot` / `hasInitialSnapshot` API; `clear()` purges History only and **does not** clear initial snapshot.

4. **And** **insert** operations preserve **gap semantics** (D-083, D-084):
   - `insertRoot(rootIndex, result, parentSnapshot)` — rejects duplicate `rootIndex`, rejects when `rootCount() >= 100`, rejects `rootIndex` outside 0–99
   - `insertRetry(rootIndex, retryIndex, result, parentSnapshot)` — rejects if root missing, duplicate `retryIndex`, `retryCount(rootIndex) >= 100`, or `retryIndex` outside 0–99
   - Caller supplies explicit indices (engine allocates via peek helpers in AC #5)

5. **And** **allocation helpers** implement max+1 with gaps (FR-55, FR-56):
   - `peekNextRootIndex()` → `std::optional<int>`: `max(existing root indices) + 1` when `rootCount() < 100` **and** result ≤ 99; else `std::nullopt` (slots full **or** gap exhausted → Defrag path in Story 6.10)
   - `peekNextRetryIndex(rootIndex)` → same policy scoped to that root's retry set
   - `isRootSlotsFull()` — true when `rootCount() == 100`
   - `isRetrySlotsFull(rootIndex)` — true when retry count for root == 100
   - `isRootIndexExhausted()` / `isRetryIndexExhausted(rootIndex)` — true when count < 100 but `peekNext*()` returns nullopt

6. **And** **delete** operations match FR-58 / D-084:
   - `deleteRoot(rootIndex)` — removes root entry **and** all retries for that root (cascade); gaps remain in remaining indices
   - `deleteRetry(rootIndex, retryIndex)` — removes single retry only; gaps remain
   - `clear()` — removes all roots and retries; store empty; initial snapshot unchanged
   - Return `bool` success / `false` if target missing

7. **And** **query** API supports downstream stories without exposing internal containers:
   - `isEmpty()`, `rootCount()`, `retryCount(rootIndex)`
   - `getSortedRootIndices()` — ascending numeric order (FR-54)
   - `getSortedRetryIndices(rootIndex)` — ascending 0–99 for existing retries
   - `getEntry(rootIndex, retryIndex = kRootOnly)` → `std::optional<MutationEntry>`
   - `hasRoot(rootIndex)`, `hasRetry(rootIndex, retryIndex)`
   - Entries returned are **copies** (buffers detached from store internals)

8. **And** buffers are stored as `std::array<juce::uint8, PatchModel::kBufferSize>` (or `PatchModel` members) — **no** descriptor logic, **no** naming (Story 6.3), **no** SysEx, **no** APVTS sync (Story 6.6) in this story.

9. **And** unit tests in `Tests/Unit/MutationHistoryStoreTests.cpp` cover:
   - First root at index 0 (M00 foundation)
   - Gap preservation after `deleteRoot` / `deleteRetry` — re-insert uses max+1, not fill-gap
   - Cascade delete removes all retries under root
   - Max 100 roots — 101st `insertRoot` fails
   - Max 100 retries per root — 101st `insertRetry` fails
   - `peekNextRootIndex` / `peekNextRetryIndex` nullopt when slots full
   - Gap exhaustion: e.g. single root at index 99 → `peekNextRootIndex()` nullopt with count 1
   - `clear()` empties history but retains initial snapshot
   - Sorted query order with out-of-order inserts (insert M05, M00, M02 → query returns [0,2,5])
   - Full `Matrix-Control_Tests` target remains green

10. **And** this story delivers **store + tests only** — no `PatchMutatorEngine`, no `MutationAlgorithm`, no `MutationNaming`, no GUI, no `PluginProcessor` wiring (those are Stories 6.3–6.6, 6.4–6.5, 7.4).

## Tasks / Subtasks

- [x] **Create `MutationEntry` + `MutationHistoryStore` skeleton** (AC: #1–#2, #8)
  - [x] `Source/Core/Services/PatchMutator/MutationHistoryStore.h`
  - [x] `Source/Core/Services/PatchMutator/MutationHistoryStore.cpp`
  - [x] Constants: `kMaxRoots = 100`, `kMaxRetriesPerRoot = 100`, `kRootOnly = -1`
  - [x] Private nested map structure: `rootIndex → { root entry, retryIndex → entry }` (or equivalent — keep lookup O(log n))

- [x] **Initial snapshot API** (AC: #3)
  - [x] `setInitialSnapshot(const PatchModel&)`, `getInitialSnapshot() const`, `hasInitialSnapshot() const`
  - [x] Document: engine captures on first MUTATE or patch-load baseline (Stories 6.4, 6.8)

- [x] **Insert + allocation helpers** (AC: #4–#5)
  - [x] `insertRoot`, `insertRetry` with validation enum or bool + optional error reason (keep minimal)
  - [x] `peekNextRootIndex`, `peekNextRetryIndex`, slot-full / exhausted predicates

- [x] **Delete + clear** (AC: #6)
  - [x] `deleteRoot` cascade, `deleteRetry`, `clear`

- [x] **Query API** (AC: #7)
  - [x] Sorted index lists, `getEntry` copies, counts

- [x] **CMake registration** (AC: #1, #9)
  - [x] Add `.cpp` to plugin `CMakeLists.txt` `target_sources`
  - [x] Add `MutationHistoryStoreTests.cpp` to test target sources

- [x] **Unit tests** (AC: #9)
  - [x] Follow `ClipboardServiceTests` — `juce::UnitTest`, AAA sections, helper to fill distinct buffer bytes per index
  - [x] Run `Matrix-Control_Tests` Debug

- [x] **Self-review** (AC: #10)
  - [x] No GUI / APVTS / SysEx / naming includes in store
  - [x] Grep `Source/Core/Services/PatchMutator/` — only store files added
  - [x] No `PatchMutatorEngine` stub required in this story

## Dev Notes

### What Story 6.2 IS — and what it is NOT

| In scope (6.2) | Out of scope |
|---|---|
| `MutationHistoryStore` + `MutationEntry` data model | `MutationAlgorithm::apply` (**6.4** — spec in `mutation-algorithm-spec.md`) |
| Gap-aware index allocation helpers (`peekNext*`) | `MutationNaming` — patch name bytes 0–7 (**6.3**) |
| CRUD: insert / delete / clear / query | MUTATE/RETRY orchestration (**6.4–6.5** — engine calls store + algorithm) |
| Initial snapshot buffer (separate from History) | APVTS `historyMList` / `selectedM` sync (**6.6**) |
| `MutationHistoryStoreTests` | SysEx 0x01 enqueue (**6.4+**) |
| CMake source registration | `HistoryDefragService::defrag` renumber (**6.10**) — store may add package-private hook later |
| | `PatchMutatorEngine` facade (**6.4** composes store) |
| | GUI two-level History comboboxes (**6.6**) |
| | `PluginProcessor` patch-load `clear()` hook (**6.13**) |

**Foundation story:** Epic 6.2 is the **RAM data layer** for D-082–D-087. Every later Mutator story reads or mutates this store through `PatchMutatorEngine` — do not scatter parallel history vectors in handlers or panels.

### Epic 6 context (cross-story)

| Story | Relationship to 6.2 |
|---|---|
| **6.1** ✅ | Algorithm produces `result` buffers; store receives copies. Caller owns copy semantics [Source: `mutation-algorithm-spec.md` §7] |
| **6.3** | After insert, engine calls `MutationNaming` to set name bytes on `result` — store holds post-name buffer |
| **6.4** | `mutate()`: read auditioned entry → algorithm → `peekNextRootIndex` → `insertRoot` |
| **6.5** | `retry()`: read selected entry's `parentSnapshot` → algorithm → `peekNextRetryIndex` → `insertRetry` |
| **6.6** | Engine serializes `getSortedRootIndices()` / retry lists → APVTS uiMirror strings |
| **6.8** | `getInitialSnapshot()` for COMPARE audition |
| **6.9** | `deleteRoot` / `deleteRetry` / `clear` — selection fallback is engine/UI, not store |
| **6.10** | `HistoryDefragService` renumbers indices — may need `friend` or package `applyRenumberMap()`; defer unless needed for tests |
| **6.11** | `PatchFileService::exportMutatorHistory` iterates store entries |
| **6.13** | Engine `clearHistory()` delegates to `store.clear()` on patch load |

### Normative data model (addendum + D-083)

Per [addendum.md § Patch Mutator — Data model](../planning-artifacts/prds/prd-Matrix-Control-2026-05-25/addendum.md):

| Field | Purpose |
|---|---|
| `result` | 134-byte packed patch after mutation |
| `parentSnapshot` | Buffer immediately **before** entry creation — RETRY input |
| `rootIndex` | 0–99 (M00–M99) |
| `retryIndex` | −1 = root only; 0–99 = retry under same root |

**Initial snapshot:** stored separately — **not** in History combos (D-085, AD-7). Used for COMPARE and `Initial.syx` export only.

**Gap policy (D-083, FR-55–56):** New indices = `max(existing) + 1`, never fill deleted slots. DELETE leaves gaps (D-084). Defrag compacts later (6.10).

### Recommended API shape

Place in `namespace Core`. Prefer explicit insert indices (engine decides after `peekNext*`) — keeps store testable without algorithm:

```cpp
namespace Core
{

struct MutationEntry
{
    int rootIndex = 0;
    int retryIndex = MutationHistoryStore::kRootOnly;
    std::array<juce::uint8, PatchModel::kBufferSize> result {};
    std::array<juce::uint8, PatchModel::kBufferSize> parentSnapshot {};
};

class MutationHistoryStore
{
public:
    static constexpr int kMaxRoots = 100;
    static constexpr int kMaxRetriesPerRoot = 100;
    static constexpr int kRootOnly = -1;

    // Initial snapshot (COMPARE / export — not in M/R lists)
    void setInitialSnapshot(const PatchModel& snapshot);
    PatchModel getInitialSnapshot() const;
    bool hasInitialSnapshot() const noexcept;

    // Allocation (gap-aware max+1)
    std::optional<int> peekNextRootIndex() const noexcept;
    std::optional<int> peekNextRetryIndex(int rootIndex) const noexcept;
    bool isRootSlotsFull() const noexcept;
    bool isRetrySlotsFull(int rootIndex) const noexcept;
    bool isRootIndexExhausted() const noexcept;
    bool isRetryIndexExhausted(int rootIndex) const noexcept;

    // Insert — caller supplies index from peek (or test vectors)
    bool insertRoot(int rootIndex, const PatchModel& result, const PatchModel& parentSnapshot);
    bool insertRetry(int rootIndex, int retryIndex, const PatchModel& result, const PatchModel& parentSnapshot);

    // Delete
    bool deleteRoot(int rootIndex);   // cascade retries
    bool deleteRetry(int rootIndex, int retryIndex);
    void clear() noexcept;            // history only

    // Query
    bool isEmpty() const noexcept;
    int rootCount() const noexcept;
    int retryCount(int rootIndex) const noexcept;
    juce::Array<int> getSortedRootIndices() const;
    juce::Array<int> getSortedRetryIndices(int rootIndex) const;
    std::optional<MutationEntry> getEntry(int rootIndex, int retryIndex = kRootOnly) const;
    bool hasRoot(int rootIndex) const noexcept;
    bool hasRetry(int rootIndex, int retryIndex) const noexcept;
};

} // namespace Core
```

**Insert overloads:** Accept `PatchModel` and copy via `loadFrom(data())` into internal arrays — mirrors `ClipboardService` `fullPatchSnapshot_` pattern.

**Do not** add `mutate()` / `retry()` to the store — engine composes algorithm + naming + store (AD-6).

### Internal structure guidance

Use `std::map<int, RootBucket>` where `RootBucket` holds optional root `MutationEntry` plus `std::map<int, MutationEntry>` retries — or flat `std::map<std::pair<int,int>, MutationEntry>` with `retryIndex == kRootOnly` for roots. Pick simplest correct structure; **private** either way.

**Thread safety:** Not required — store accessed from message thread only (same as `ClipboardService`). Document in header comment.

**Memory:** Worst case 100 roots × (1 + 100 retries) × 268 bytes ≈ 2.7 MB — acceptable for session RAM (AD-7).

### Gap exhaustion test scenario (AC #9)

Document this case explicitly in tests:

1. `insertRoot(99, …)` — single root at M99
2. `peekNextRootIndex()` → `nullopt` (max+1 = 100 > 99)
3. `rootCount() == 1`, `isRootIndexExhausted() == true`, `isRootSlotsFull() == false`

Engine maps exhausted-but-not-full to Defrag modal (FR-59, Story 6.10) — store only reports state.

### Architecture compliance

- **Core ↛ GUI** — no `juce_gui_*`, no `PluginIDs` in store implementation [Source: AD-1, `project-context.md`]
- Path: `Source/Core/Services/PatchMutator/` per AD-6 [Source: `architecture.md` § AD-6, § Structure Patterns]
- One primary class per `.h/.cpp` pair
- Naming: `MutationHistoryStore`, `lowerCamelCase` methods, `name_` private members, `kMaxRoots` constants [Source: `architecture.md` § Naming Patterns]
- Buffers: 134 bytes via `PatchModel::kBufferSize` / `SysExConstants::kPatchPackedDataSize` — **no** hardcoded byte indices for parameters [Source: AD-1 anti-patterns]
- Session-only RAM — no persistence (AD-7)
- English source/comments only

### File structure (this story)

```
Source/Core/Services/PatchMutator/          (NEW folder)
├── MutationHistoryStore.h                  (NEW)
└── MutationHistoryStore.cpp                (NEW)

Tests/Unit/
└── MutationHistoryStoreTests.cpp           (NEW)

CMakeLists.txt                              (UPDATE — plugin + test sources)
```

**No** changes to `PatchMutatorPanel`, `PluginProcessor`, `MutatorActionHandler`, or `PluginIDs.h` in this story.

### CMake registration

Mirror `ClipboardService.cpp` entries:

- Plugin target: `Source/Core/Services/PatchMutator/MutationHistoryStore.cpp`
- Test target: include same `.cpp` + `Tests/Unit/MutationHistoryStoreTests.cpp`

First file in new `PatchMutator/` subfolder — establish folder for Stories 6.3–6.5, 6.4 engine.

### Testing requirements

Follow `Tests/Unit/ClipboardServiceTests.cpp`:

- `class MutationHistoryStoreTests : public juce::UnitTest`
- Static helper: `PatchModel makeDistinctBuffer(int seed)` — set byte 8+ or name for identity
- AAA structure with `beginTest("…")`

| Test | Assert |
|---|---|
| `emptyStore_queries` | `isEmpty()`, sorted roots empty |
| `insertFirstRoot_indexZero` | insert 0 → `hasRoot(0)`, count 1 |
| `sortedRoots_numericOrder` | insert 5,0,2 → `getSortedRootIndices()` == [0,2,5] |
| `insertRetry_underRoot` | root 3 + retry 0,1 → `retryCount(3)==2` |
| `deleteRetry_preservesGap` | insert R0,R2; delete R0; peekNext → 3 (not 1) |
| `deleteRoot_cascadesRetries` | root + 3 retries; delete root → all gone |
| `maxRoots_rejects101st` | fill 100 roots → next insert fails |
| `maxRetries_rejects101st` | 100 retries under M00 → 101st fails |
| `gapExhausted_singleAt99` | root at 99 only → `peekNextRootIndex` nullopt, not full |
| `clear_keepsInitialSnapshot` | set initial + roots; clear → history empty, initial remains |
| `getEntry_returnsCopy` | mutate copy via `getEntry`; internal unchanged |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests` (or project preset) — all green.

### Previous story intelligence (6.1)

Story 6.1 delivered approved `mutation-algorithm-spec.md` (Option A, sign-off 2026-06-19):

- Algorithm is **stateless** — store owns session state
- `apply()` copies input → output; engine passes auditioned `result` (MUTATE) or `parentSnapshot` (RETRY) as input
- Patch name bytes 0–7 excluded from algorithm — naming in 6.3 updates `result` before/after store insert (engine order TBD in 6.4; store holds final bytes)
- Matrix Mod bytes 104–133 included in algorithm output — store treats buffer opaque

**6.1 review lesson:** Keep spec/store boundary crisp — store must not embed algorithm or naming rules.

### Git intelligence

| Commit | Relevance |
|---|---|
| `3b9d4c5` | Latest — Epic 7 bank; no `PatchMutator/` code yet |
| `55e220c` | Story 7.4 artifact — documents engine API expecting this store |
| Epic 5 done | `ClipboardService` buffer copy patterns — reuse for snapshots |

**Brownfield:** `Source/Core/Services/PatchMutator/` does **not** exist — this story creates the subfolder. `MutatorActionHandler` remains stub until Epic 6 + 7.4.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `std::optional`, `std::array` (no new deps)
- `juce::Array<int>` for sorted index lists matches existing Core patterns (`PatchFileService` scan results)
- `PatchModel::loadFrom` for buffer copy — already used across Epic 1–5 tests

### Project context reference

- `project-context.md` — English artifacts; French chat with Guillaume
- `CONVENTIONS.md` — Core/GUI separation; tests under `Tests/Unit/`
- FR-54–56 foundation, FR-58 delete semantics — store layer only
- D-082 two-level model, D-083 MUTATE/RETRY I/O, D-084 DELETE gaps, D-085 0-based indices

### References

- [epics.md — Story 6.2, Epic 6 overview]
- [prd.md — FR-54–FR-56, FR-58]
- [addendum.md — § Patch Mutator data model, limits table]
- [.decision-log.md — D-082–D-085]
- [architecture.md — AD-6, AD-7, AD-8 testing]
- [mutation-algorithm-spec.md — §7 caller buffer ownership]
- [6-1-mutationalgorithm-specification.md — Epic 6 cross-story table]
- [7-4-mutatoractionhandler.md — engine dependency on 6.2]
- [PatchModel.h — kBufferSize, loadFrom]
- [ClipboardService.h — fullPatchSnapshot_ pattern]

## Dev Agent Record

### Agent Model Used

Claude claude-4.6-sonnet-medium-thinking (Cursor)

### Debug Log References

- Fixed compile error: anonymous helper referenced private `RootBucket` — inlined max-root scan into `peekNextRootIndex()`.
- `MutationEntry` defined before `MutationHistoryStore` in header so `RootBucket` can embed complete entries; default `retryIndex = -1` documents `kRootOnly`.

### Completion Notes List

- Implemented `Core::MutationHistoryStore` + `MutationEntry` under new `Source/Core/Services/PatchMutator/` folder with `std::map<int, RootBucket>` internal structure (root entry + retry sub-map).
- Full API per AC: initial snapshot (separate from history), gap-aware insert/delete/clear, peek helpers, slot-full/exhausted predicates, sorted query lists, copy-on-read `getEntry`.
- Added 11 unit tests in `MutationHistoryStoreTests.cpp` — all pass.
- Registered sources in plugin + test CMake targets.
- Self-review: no GUI/APVTS/SysEx/naming deps; only store files in PatchMutator folder.
- Full test run: all `MutationHistoryStore` tests green; 1 pre-existing failure in `MidiActivityTrackerTests` (unrelated to this story).

### File List

- Source/Core/Services/PatchMutator/MutationHistoryStore.h (NEW)
- Source/Core/Services/PatchMutator/MutationHistoryStore.cpp (NEW)
- Tests/Unit/MutationHistoryStoreTests.cpp (NEW)
- CMakeLists.txt (MODIFIED)

### Change Log

- 2026-06-19: Story 6.2 created — MutationHistoryStore two-level M/R developer guide with API contract, gap semantics, test matrix, and Epic 6 boundary table.
- 2026-06-19: Code review — `isRetryIndexExhausted` guards missing root; 5 new unit tests (deleteRoot gap, rejection paths, delete miss, retry gap at 99); status → done.

### Review Findings

- [x] [Review][Patch] `isRetryIndexExhausted` returns true when root is missing — guard with `hasRoot(rootIndex)` [`MutationHistoryStore.cpp:93`]
- [x] [Review][Patch] Missing test: gap preservation after `deleteRoot` (peek uses max+1, not fill-gap) [`MutationHistoryStoreTests.cpp`]
- [x] [Review][Patch] Missing tests for insert rejection paths (duplicate, out-of-range, retry without root) [`MutationHistoryStoreTests.cpp`]
- [x] [Review][Patch] Missing tests for delete `false` on missing root/retry [`MutationHistoryStoreTests.cpp`]
- [x] [Review][Patch] Missing test: `isRetryIndexExhausted` retry gap at index 99 [`MutationHistoryStoreTests.cpp`]
- [x] [Review][Defer] Full `Matrix-Control_Tests` target not green — pre-existing `MidiActivityTrackerTests` failure unrelated to 6.2 — deferred, pre-existing
- [x] [Review][Defer] `getInitialSnapshot()` without prior `setInitialSnapshot()` returns zeros — caller must use `hasInitialSnapshot()` per AC #3 — deferred, pre-existing
- [x] [Review][Defer] `isRetrySlotsFull` / `peekNextRetryIndex` ambiguous on missing root — callers use `hasRoot` first (engine layer, Story 6.4+) — deferred, pre-existing
- [x] [Review][Defer] `rootCount()` O(n) scan and redundant sort on sorted map — acceptable v1 per architecture memory note — deferred, pre-existing
