---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.3 — MutationNaming and Display Names
author: BMad Agent
status: review
baseline_commit: 8345ebc
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md
  - implementation-artifacts/6-1-mutationalgorithm-specification.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 6.3: MutationNaming and Display Names

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want mutation entries named Mxx and Mxx-Ryy in buffer and UI,
so that names match EXPORT filenames (FR-55, FR-56, FR-13 integration).

## Acceptance Criteria

1. **Given** Story 6.2 complete (`MutationHistoryStore` with gap-aware `peekNextRootIndex` / `peekNextRetryIndex`) **When** `MutationNaming` is implemented under `Source/Core/Services/PatchMutator/` **Then** a Core-only, stateless naming helper exists with **no GUI dependencies** and **no index allocation logic** (allocation stays in the store — D-083, D-085).

2. **And** **root-only patch name** (bytes 0–7) for `rootIndex` 0–99 formats as **`Mxx`** — two-digit zero-padded decimal (`M00`, `M05`, `M99`). First allocated root index **0** → **`M00`** (not M01).

3. **And** **retry patch name** for `(rootIndex, retryIndex)` with `retryIndex` 0–99 formats as **`Mxx-Ryy`** — exactly **7 characters** at maximum (`M99-R99`). First retry under root **M05** → **`M05-R00`**.

4. **And** `applyPatchName(PatchModel& model, int rootIndex, int retryIndex)` writes the formatted name into packed buffer bytes **0–7** via `PatchModel::setName()` — the **only** approved path for mutator name bytes (no direct buffer index writes in services).

5. **And** **display string helpers** (no buffer mutation) support downstream UI and export stories:
   - `formatRootLabel(rootIndex)` → `"M00"` … `"M99"` (History **M** combobox — Story 6.6)
   - `formatRetryLabel(retryIndex)` → `"R00"` … `"R99"` (History **R** combobox — Story 6.6; root-only sentinel `—` is **not** owned by this class)
   - `formatPatchName(rootIndex, retryIndex)` → `"Mxx"` or `"Mxx-Ryy"` (PATCH NAME field + packed bytes)
   - `formatExportStem(rootIndex, retryIndex)` → same stem as `formatPatchName` (`.syx` extension and folder layout added by Story 6.11 / `PatchFileNameSanitizer::ensureSyxExtension`)

6. **And** invalid inputs are rejected safely: `rootIndex` outside 0–99 or `retryIndex` outside 0–99 (when not `MutationHistoryStore::kRootOnly`) → formatting returns empty `juce::String` and `applyPatchName` returns `false` without mutating the model.

7. **And** unit tests in `Tests/Unit/MutationNamingTests.cpp` cover:
   - First root **M00**, first retry **M05-R00**
   - Boundary **M99** / **M99-R99** (7-char retry name)
   - `applyPatchName` round-trip via `PatchModel::getName()`
   - Bytes 0–7 updated; byte 8+ unchanged
   - Invalid index rejection
   - Full `Matrix-Control_Tests` target remains green

8. **And** this story delivers **naming service + tests only** — no `PatchMutatorEngine`, no MUTATE/RETRY orchestration (**6.4–6.5**), no APVTS `historyMList` sync (**6.6**), no `PatchNameSyncer` wiring (**6.7**), no `PatchFileService::exportMutatorHistory` (**6.11**), no Defrag renumber (**6.10**).

## Tasks / Subtasks

- [x] **Create `MutationNaming` skeleton** (AC: #1, #6)
  - [x] `Source/Core/Services/PatchMutator/MutationNaming.h`
  - [x] `Source/Core/Services/PatchMutator/MutationNaming.cpp`
  - [x] Stateless struct or class with static methods only (mirror `PatchFileNameSanitizer` pattern)
  - [x] Include `MutationHistoryStore.h` only for `kRootOnly` constant — no store instance

- [x] **Implement format helpers** (AC: #2–#3, #5)
  - [x] Private `formatTwoDigitIndex(int index)` — `juce::String::formatted("%02d", index)` or equivalent
  - [x] `formatRootLabel`, `formatRetryLabel`, `formatPatchName`, `formatExportStem`
  - [x] `formatExportStem` delegates to `formatPatchName` (identical stem per architecture § Format Patterns)

- [x] **Implement buffer application** (AC: #4)
  - [x] `applyPatchName(PatchModel&, int rootIndex, int retryIndex = MutationHistoryStore::kRootOnly)` → `bool`
  - [x] Call `model.setName(formatPatchName(...))` only when format succeeds

- [x] **CMake registration** (AC: #1, #7)
  - [x] Add `MutationNaming.cpp` to plugin `target_sources` (same block as `MutationHistoryStore.cpp`)
  - [x] Add `MutationNamingTests.cpp` to test target sources

- [x] **Unit tests** (AC: #7)
  - [x] Follow `MutationHistoryStoreTests` — `juce::UnitTest`, AAA, `beginTest`
  - [x] Helper: fill byte 8+ with marker; assert unchanged after `applyPatchName`
  - [x] Run `Matrix-Control_Tests` Debug

- [x] **Self-review** (AC: #8)
  - [x] No GUI / APVTS / SysEx / engine includes
  - [x] No duplicate `peekNext*` allocation logic
  - [x] Grep `PatchMutator/` — only naming files added (+ CMake)

## Dev Notes

### What Story 6.3 IS — and what it is NOT

| In scope (6.3) | Out of scope |
|---|---|
| `MutationNaming` format + `applyPatchName` | Index allocation `max+1` with gaps (**already 6.2** `peekNext*`) |
| String rules: `Mxx`, `Mxx-Ryy`, `Ryy` labels | `PatchMutatorEngine::mutate()` / `retry()` (**6.4–6.5**) |
| Packed bytes 0–7 via `PatchModel::setName` | `PatchNameSyncer::bufferToApvts` on audition (**6.7**) |
| Unit tests for naming | APVTS `historyMList` / `selectedM` (**6.6**) |
| CMake source registration | EXPORT folder creation (**6.11**) |
| | Defrag renumber + bulk rename (**6.10** calls `MutationNaming` after renumber) |
| | GUI PATCH NAME display changes |

**Critical boundary:** Epic AC mentions "max+1 with gaps" — that is **`MutationHistoryStore`** (Story 6.2). **`MutationNaming`** receives **already-allocated** `(rootIndex, retryIndex)` from the future engine and **formats + applies** the name. Do **not** reimplement allocation here.

### Naming rules (normative)

Per PRD FR-55/FR-56, addendum § Patch Mutator limits, D-085, architecture § Format Patterns:

| Context | Root only (retryIndex = `kRootOnly`) | Retry entry |
|---|---|---|
| PATCH NAME / bytes 0–7 | `M00`–`M99` (3 chars) | `M05-R02` (max **7** chars) |
| History **M** combo label | `M00`–`M99` | *(same root index — selection is per M)* |
| History **R** combo label | *(sentinel `—` — Story 6.6)* | `R00`–`R99` |
| EXPORT file stem | `M05.syx` | `M05-R02.syx` |
| EXPORT subfolder | `M05/` | files inside same `M05/` folder |

**0-based indices:** `rootIndex=0` → `M00`; `retryIndex=0` → `-R00` suffix. Matrix hardware uses 0-based numbering (D-085).

**Character set:** Generated names use only `M`, `R`, `-`, and digits — all valid Matrix 6-bit name characters. **No** `PatchFileNameSanitizer` pass required on generated mutator names (sanitizer remains for user/file stems in Epic 4).

**Initial snapshot:** COMPARE / `Initial.syx` keeps the **pre-mutation patch name** — never rewritten by `MutationNaming` (D-085).

### Recommended API

Place in `namespace Core`. Prefer stateless struct (like `PatchFileNameSanitizer`):

```cpp
namespace Core
{

struct MutationNaming
{
    static juce::String formatRootLabel(int rootIndex);
    static juce::String formatRetryLabel(int retryIndex);
    static juce::String formatPatchName(int rootIndex,
                                        int retryIndex = MutationHistoryStore::kRootOnly);
    static juce::String formatExportStem(int rootIndex,
                                         int retryIndex = MutationHistoryStore::kRootOnly);

    static bool applyPatchName(PatchModel& model,
                               int rootIndex,
                               int retryIndex = MutationHistoryStore::kRootOnly);

private:
    static bool isValidRootIndex(int rootIndex) noexcept;
    static bool isValidRetryIndex(int retryIndex) noexcept;
    static juce::String formatTwoDigit(int index);
};

} // namespace Core
```

**`formatPatchName` logic:**

```cpp
// root only:  "M" + twoDigit(rootIndex)
// retry:      "M" + twoDigit(rootIndex) + "-R" + twoDigit(retryIndex)
// retryIndex == MutationHistoryStore::kRootOnly (-1) → root-only form
```

**`applyPatchName`:** Build string → if empty return `false` → `model.setName(name)` → return `true`. `PatchModel::setName` already uppercases and space-pads to 8 bytes [Source: `PatchModel.cpp`].

### Engine integration order (Stories 6.4–6.5 — implement later, document now)

Correct pipeline for MUTATE (engine responsibility — **not** this story):

1. Read auditioned entry `result` buffer
2. Run `MutationAlgorithm::apply` (name bytes unchanged per spec §6)
3. `store.peekNextRootIndex()` → e.g. `3`
4. `MutationNaming::applyPatchName(mutatedModel, 3)` → name `M03`
5. `store.insertRoot(3, mutatedModel, parentSnapshot)`

RETRY pipeline:

1. Read selected entry's `parentSnapshot`
2. `MutationAlgorithm::apply`
3. `store.peekNextRetryIndex(rootIndex)` → e.g. `2`
4. `MutationNaming::applyPatchName(mutatedModel, rootIndex, 2)` → e.g. `M05-R02`
5. `store.insertRetry(rootIndex, 2, mutatedModel, parentSnapshot)`

Store always holds **post-naming** buffers so EXPORT (6.11) and audition SysEx (6.7) see correct bytes 0–7.

### Epic 6 cross-story map

| Story | Uses `MutationNaming` |
|---|---|
| **6.2** ✅ | Store holds buffers; naming applied **before** insert |
| **6.4** | `mutate()` calls `applyPatchName` after algorithm |
| **6.5** | `retry()` calls `applyPatchName` with retry index |
| **6.6** | Engine builds combo strings via `formatRootLabel` / `formatRetryLabel` |
| **6.7** | Auditioned entry name already in buffer; `PatchNameSyncer` reflects it |
| **6.10** | Defrag renumber → re-apply names for all entries with new indices |
| **6.11** | `formatExportStem` + `ensureSyxExtension` for filenames |
| **7.4** | Handler delegates to engine — no direct naming calls |

### Architecture compliance

- **Core ↛ GUI** — no `juce_gui_*`, no `PluginIDs` in naming implementation
- Path: `Source/Core/Services/PatchMutator/MutationNaming.{h,cpp}` per AD-6
- One primary class/struct per `.h/.cpp` pair
- Naming: `MutationNaming`, `lowerCamelCase` static methods, `k`-prefixed constants if needed
- Bytes 0–7 **only** through `PatchModel::setName` — never hardcode byte offsets in `MutationNaming` [Source: AD-1 anti-patterns, architecture § Format Patterns]
- English source/comments only
- Thread safety: not required — message thread only (same as store)

### File structure (this story)

```
Source/Core/Services/PatchMutator/
├── MutationHistoryStore.h/.cpp     (existing — 6.2)
├── MutationNaming.h                 (NEW)
└── MutationNaming.cpp               (NEW)

Tests/Unit/
└── MutationNamingTests.cpp          (NEW)

CMakeLists.txt                       (UPDATE — plugin + test sources)
```

### CMake registration

Mirror `MutationHistoryStore.cpp` entries in `CMakeLists.txt`:

- Plugin target (~line 129): add `Source/Core/Services/PatchMutator/MutationNaming.cpp`
- Test target (~lines 304, 361): add `MutationNamingTests.cpp` + compile `MutationNaming.cpp`

### Testing requirements

Follow `Tests/Unit/MutationHistoryStoreTests.cpp` pattern.

| Test | Assert |
|---|---|
| `formatRootLabel_firstRoot` | `formatRootLabel(0) == "M00"` |
| `formatRootLabel_padded` | `formatRootLabel(5) == "M05"`, `formatRootLabel(99) == "M99"` |
| `formatRetryLabel_firstRetry` | `formatRetryLabel(0) == "R00"` |
| `formatPatchName_rootOnly` | `formatPatchName(5, kRootOnly) == "M05"` |
| `formatPatchName_retry` | `formatPatchName(5, 2) == "M05-R02"` (length 7) |
| `formatPatchName_maxBoundary` | `formatPatchName(99, 99) == "M99-R99"` |
| `formatExportStem_matchesPatchName` | `formatExportStem(5, 2) == formatPatchName(5, 2)` |
| `applyPatchName_root_roundTrip` | apply + `getName()` == `"M03"` |
| `applyPatchName_retry_roundTrip` | apply(5, 0) + `getName()` == `"M05-R00"` |
| `applyPatchName_preservesOtherBytes` | byte 8+ unchanged after apply |
| `invalidRootIndex_rejected` | `formatPatchName(-1, kRootOnly).isEmpty()`, `applyPatchName` → false |
| `invalidRetryIndex_rejected` | `formatPatchName(0, -1)` only valid when `-1 == kRootOnly`; `formatPatchName(0, 100)` empty |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Previous story intelligence

**6.1 (done):** Algorithm **must not** touch bytes 0–7. Naming is strictly post-algorithm. Golden vectors assume pre-naming buffers — tests that check name bytes belong in `MutationNamingTests`, not `MutationAlgorithmTests`.

**6.2 (done):** Store exposes `kRootOnly = -1`. `peekNextRootIndex()` returns `0` for first root, `1` after M00, etc. Naming must format whatever index the store allocated — **no off-by-one** (index 0 → `M00`). Review lesson from 6.2: keep boundaries crisp — naming must not embed store internals beyond `kRootOnly`.

**6.2 code review deferrals:** `getInitialSnapshot()` without prior set returns zeros — irrelevant to naming. Retry exhausted on missing root — engine must `hasRoot` before naming retries.

### Git intelligence

| Commit | Relevance |
|---|---|
| `8345ebc` | Latest — Story 4.6 done |
| `74a025b` | Story 6.2 — `MutationHistoryStore` landed; naming is next file in `PatchMutator/` |
| `324fc3d` | MutationAlgorithm spec — §6 name exclusion |
| `f78f956` | Story 6.1 artifact |

**Brownfield:** `MutationHistoryStore` exists at `Source/Core/Services/PatchMutator/`. `MutationNaming` is the **second** file in that folder.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `juce::String::formatted` for zero-padding; no new dependencies
- `PatchModel::kNameLength == 8` — retry names max 7 chars fit with space pad from `setName`
- No web research required — pure string formatting on existing model API

### Project context reference

- `project-context.md` — English artifacts; French chat with Guillaume
- `CONVENTIONS.md` — Core/GUI separation; tests under `Tests/Unit/`
- FR-55/FR-56 naming consequences; FR-13 patch name / filename alignment (export validates in 6.11)
- D-085 0-based M/R naming; D-083 engine owns ordering of algorithm → naming → store insert

### References

- [epics.md — Story 6.3, Epic 6 overview]
- [prd.md — FR-55, FR-56, FR-13, FR-33 export naming]
- [addendum.md — § Patch Mutator UI, limits table, EXPORT layout]
- [.decision-log.md — D-085, D-083]
- [architecture.md — AD-6, § Format Patterns Mutator names]
- [mutation-algorithm-spec.md — §6 bytes 0–7 exclusion]
- [6-2-mutationhistorystore-two-level-mr.md — allocation vs naming boundary]
- [7-4-mutatoractionhandler.md — engine API expecting MutationNaming]
- [PatchModel.h / PatchModel.cpp — setName, getName, kNameLength]
- [PatchFileNameSanitizer.h — stateless service pattern reference]

## Dev Agent Record

### Agent Model Used

Claude Opus 4.6 (via Cursor)

### Debug Log References

- Implemented stateless `Core::MutationNaming` struct mirroring `PatchFileNameSanitizer` pattern.
- Validation uses `MutationHistoryStore::kMaxRoots` / `kMaxRetriesPerRoot` bounds; `kRootOnly` (-1) selects root-only patch name form.
- `applyPatchName` writes exclusively via `PatchModel::setName()` — no direct buffer byte access.

### Completion Notes List

- Added `MutationNaming.h/.cpp` with format helpers (`formatRootLabel`, `formatRetryLabel`, `formatPatchName`, `formatExportStem`) and `applyPatchName`.
- Root names: `M00`–`M99` (0-based). Retry names: `Mxx-Ryy` (max 7 chars at `M99-R99`).
- Invalid indices return empty string / `false` without mutating the model.
- 12 unit tests in `MutationNamingTests.cpp` — all pass; full `Matrix-Control_Tests` Debug suite green (exit 0).

### File List

- `Source/Core/Services/PatchMutator/MutationNaming.h` (new)
- `Source/Core/Services/PatchMutator/MutationNaming.cpp` (new)
- `Tests/Unit/MutationNamingTests.cpp` (new)
- `CMakeLists.txt` (modified)

### Change Log

- 2026-06-19: Story 6.3 created — MutationNaming developer guide with format rules, API contract, engine pipeline order, test matrix, and Epic 6 boundary table.
- 2026-06-19: Implemented `MutationNaming` service, unit tests, and CMake registration — ready for review.
- 2026-06-19: Code review passed — all acceptance criteria met; story marked done.

### Review Findings

- [x] [Review] Clean review (2026-06-19) — 3 layers (Blind Hunter, Edge Case Hunter, Acceptance Auditor); 0 décision à prendre, 0 correction, 0 à reporter; 13 points adversariaux écartés (conformes à la spec ou conventions projet). `Matrix-Control_Tests` : 12/12 tests MutationNaming verts.
