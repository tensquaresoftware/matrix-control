---
organization: Ten Square Software
project: Matrix-Control
title: Story 6.4 — MUTATE Action
author: BMad Agent
status: review
baseline_commit: 0f2078d
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/prd.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/addendum.md
  - planning-artifacts/prds/prd-matrix-control-2026-05-25/.decision-log.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/architecture.md
  - planning-artifacts/architecture/architecture-matrix-control-2026-05-25/mutation-algorithm-spec.md
  - implementation-artifacts/6-1-mutationalgorithm-specification.md
  - implementation-artifacts/6-2-mutationhistorystore-two-level-mr.md
  - implementation-artifacts/6-3-mutationnaming-and-display-names.md
  - implementation-artifacts/7-4-mutatoractionhandler.md
  - project-context.md
created: 2026-06-19
updated: 2026-06-19
---

# Story 6.4: MUTATE Action

Status: done

<!-- Ultimate context engine analysis completed — comprehensive developer guide created -->

## Story

As a sound designer,
I want MUTATE to create a new root from the auditioned patch,
so that I explore variations without losing other roots (FR-55, FR-30).

## Acceptance Criteria

1. **Given** Stories 6.1–6.3 complete (approved `mutation-algorithm-spec.md`, `MutationHistoryStore`, `MutationNaming`) **When** `MutationAlgorithm` and `PatchMutatorEngine` are implemented under `Source/Core/Services/PatchMutator/` **Then** Core-only services exist with **no GUI dependencies** and normative algorithm behaviour per `mutation-algorithm-spec.md` v1.0.1.

2. **And** `MutationAlgorithm::apply(PatchModel& inOut, const MutationRecipe& recipe, IRandomSource& rng)` implements §3–§8 of the spec:
   - Early exit when `amountPercent == 0` OR `randomPercent == 0` — output byte-identical, returns `false`, **no RNG consumed**
   - Int jitter (§4.1), choice re-roll (§4.2), module toggle mask (§5), bytes 0–7 never changed (§6)
   - Matrix Mod Option **A** (§7) — bytes 104–133 eligible even when all module toggles off
   - Injectable `IRandomSource` — no hard-coded `juce::Random` inside the algorithm class
   - Returns `true` only when at least one byte 8–133 changed; `false` otherwise

3. **And** `PatchMutatorEngine::mutate()` orchestrates the full MUTATE pipeline (D-083):
   - **Audition input:** when history is **empty** → copy live `*patchModel_` (editor buffer); when history is **non-empty** → copy `result` buffer of the **audition-selected** entry (`selectedRootIndex_` + `selectedRetryIndex_`, defaulting to root-only when retry not set)
   - **Parent snapshot:** copy of audition input **before** algorithm runs (stored with new entry)
   - Build `MutationRecipe` from **current** APVTS state (Amount/Random int parameters + ten enable state properties — spec §2)
   - Run `MutationAlgorithm::apply` on working copy
   - `store.peekNextRootIndex()` → allocate gap-aware root index; on exhaustion return failure with `defragModalRequested = true` (D-087, FR-59)
   - `MutationNaming::applyPatchName(mutatedModel, rootIndex)` **after** algorithm
   - `store.insertRoot(rootIndex, mutatedModel, parentSnapshot)` — store holds post-naming buffers
   - On success: push mutated buffer to editor (`bufferToApvts` + `PatchNameSyncer::bufferToApvts` under `ActionExecutionHooks` suppress) and enqueue **one** full patch SysEx **0x01** via `MidiManager::sendPatch` using current patch number from device limits (same pattern as `PatchManagerActionHandler`)
   - On **no-op recipe** (`amountPercent == 0` OR `randomPercent == 0`): return `{ success: false, footerMessage }` — **do not** insert root, **do not** send SysEx
   - **Never deletes** existing roots; new root index = `max(existing Mi) + 1` with gaps preserved

4. **And** first MUTATE on empty history creates root **M00** (index 0) from current editor patch; subsequent MUTATE from auditioned **Mi** or **Mi-Rj** creates root at `peekNextRootIndex()` (e.g. after M00 and M02 exist, next is M03).

5. **And** `MutatorActionResult` struct exists with `success`, `footerMessage`, `footerSeverity`, `defragModalRequested` — blocked mutate (100 roots / index exhaustion) returns `success = false` with user-facing English footer message; does **not** throw.

6. **And** `PatchMutatorEngine` declares the full public API expected by Story 7.4 (`retry`, `toggleCompare`, `deleteSelected`, `clearHistory`, `exportHistory`, `defragHistory`, `auditionSelectedHistoryEntry`, `syncHistoryUiProperties`, `refreshActionEnabledMirrors`) but **only `mutate()` is implemented** in this story — other methods return `{ success: false }` or no-op stubs documented in header.

7. **And** engine exposes `setAuditionSelection(int rootIndex, int retryIndex = MutationHistoryStore::kRootOnly)` for tests and future Story 6.6/6.7 wiring (until `selectedM`/`selectedR` APVTS properties exist).

8. **And** unit tests:
   - `Tests/Unit/MutationAlgorithmTests.cpp` — golden vectors GV-01–GV-04 from spec Appendix A using `InitDefaults::patchData()` and seeded `IRandomSource`; pin GV-04 byte 13 on first verified run
   - `Tests/Unit/PatchMutatorEngineTests.cpp` — mutate: empty history → M00; gap allocation; limit rejection; no-op recipe (A=0) does not insert; non-empty history uses audition selection; SysEx called once on success (recording fake `MidiManager` or injectable send callback)
   - Full `Matrix-Control_Tests` Debug target remains green

9. **And** `CMakeLists.txt` registers `MutationAlgorithm.cpp`, `PatchMutatorEngine.cpp`, and both test files in plugin and test targets.

10. **And** this story delivers **algorithm + engine `mutate()` + tests only** — no `MutatorActionHandler` wiring (**7.4**), no APVTS `historyMList`/`selectedM` sync (**6.6**), no `retry()` (**6.5**), no history audition debounce (**6.7**), no COMPARE/DELETE/CLEAR/EXPORT (**6.8–6.11**), no recipe prefs persistence (**6.12**), no clear-on-patch-load (**6.13**), no `PluginProcessor` engine construction (**7.4** composition root).

## Tasks / Subtasks

- [x] **Implement `MutationAlgorithm`** (AC: #1–#2)
  - [x] `Source/Core/Services/PatchMutator/MutationAlgorithm.h`
  - [x] `Source/Core/Services/PatchMutator/MutationAlgorithm.cpp`
  - [x] `MutationRecipe`, `IRandomSource` in header (or `MutationRecipe.h` if shared)
  - [x] Descriptor iteration via `ApvtsPatchMapper::buildIntDescriptors()` / `buildChoiceDescriptors()` — never hardcode byte offsets except Matrix Mod §7.2 fallback
  - [x] `SeededRandom` test adapter implementing `IRandomSource` wrapping `juce::Random`

- [x] **Implement `PatchMutatorEngine` facade** (AC: #3–#7)
  - [x] `Source/Core/Services/PatchMutator/PatchMutatorEngine.h`
  - [x] `Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp`
  - [x] `MutatorActionResult` struct
  - [x] Constructor DI: `PatchModel*`, `ApvtsPatchMapper*`, `PatchNameSyncer*`, `MidiManager*`, `juce::AudioProcessorValueTreeState&`, `ActionExecutionHooks`, `std::function<int()>` currentPatchNumber resolver (mirror `PatchManagerActionHandler::getCurrentPatch`)
  - [x] Owns `MutationHistoryStore`, `MutationAlgorithm` (stateless instance)
  - [x] Private `buildRecipeFromApvts()` reading `PluginIDs::PatchManagerSection::PatchMutatorModule::*`
  - [x] Private `resolveAuditionBuffer()` and `pushResultToEditorAndSynth()`
  - [x] Stub unimplemented public methods with `// Story 6.x` comments

- [x] **CMake registration** (AC: #9)
  - [x] Add `.cpp` files to plugin target (same block as `MutationHistoryStore.cpp`)
  - [x] Add test sources + compile algorithm/engine `.cpp` in test target

- [x] **Unit tests** (AC: #8)
  - [x] `MutationAlgorithmTests.cpp` — GV-01 through GV-04
  - [x] `PatchMutatorEngineTests.cpp` — mutate orchestration with fakes
  - [x] Run `cmake --build Builds/macOS --target Matrix-Control_Tests`

- [x] **Self-review** (AC: #10)
  - [x] No GUI / panel / handler changes
  - [x] Grep `PatchMutatorPanel` — unchanged
  - [x] Algorithm does not touch bytes 0–7; naming only via `MutationNaming`
  - [x] No duplicate `peekNext*` logic outside store

### Review Findings

- [x] [Review][Defer] Constructor with 7 parameters vs project Clean Code limit (3) — deferred to Story 7.4 composition root; keep seven DI args for 6.4 as spec-written.

- [x] [Review][Patch] Misleading footer when algorithm returns false with non-zero Amount/Random [PatchMutatorEngine.cpp:102-107] — fixed: distinct `kNoMutationChangeFooterMessage`.

- [x] [Review][Patch] Amount/Random read via `state.getProperty` instead of APVTS parameters [PatchMutatorEngine.cpp:208-209] — resolved: Patch Manager uses ValueTree properties (`ApvtsLayoutBuilder`); added `readPatchMutatorPercent` with raw-parameter path + property fallback.

- [x] [Review][Patch] `mutate_emptyHistory_createsM00` asserts only first byte of name [PatchMutatorEngineTests.cpp:171] — fixed: assert `M`, `0`, `0`.

- [x] [Review][Patch] `resolveAuditionBuffer()` fallback not documented in header [PatchMutatorEngine.h:63] — fixed: header comment on fallback rules.

- [x] [Review][Patch] Dead `oldIndex` read in choice re-roll loop [MutationAlgorithm.cpp:67-69] — fixed: removed dead read.

- [x] [Review][Patch] No guard when int descriptor `maxValue < minValue` [MutationAlgorithm.cpp:42-47] — fixed: `if (range <= 0) continue`.

- [x] [Review][Defer] `mutate()` exceeds 20-line orchestration limit [PatchMutatorEngine.cpp:72-137] — deferred, style debt; extract helpers in a focused cleanup pass.
- [x] [Review][Defer] No documented thread-safety contract on engine [PatchMutatorEngine.h] — deferred, pre-existing JUCE message-thread pattern; document when 7.4 wires processor.
- [x] [Review][Defer] Silent fallback to live patch when history entry missing [PatchMutatorEngine.cpp:241-255] — deferred, defensive edge case; store should stay consistent.
- [x] [Review][Defer] Negative `getCurrentPatchNumber()` not rejected before `sendPatch` [PatchMutatorEngine.cpp:263-268] — deferred; `jlimit(0,255)` clamps; mirror `PatchManagerActionHandler` callback contract in 7.4.

## Dev Notes

### What Story 6.4 IS — and what it is NOT

| In scope (6.4) | Out of scope |
|---|---|
| `MutationAlgorithm::apply` per approved spec | `retry()` pipeline (**6.5**) |
| `PatchMutatorEngine::mutate()` full orchestration | `MutatorActionHandler` delegation (**7.4**) |
| `MutatorActionResult` + stub declarations for other engine methods | History M/R APVTS properties (**6.6**) |
| Recipe builder from APVTS at mutate time | Debounced history audition (**6.7**) |
| SysEx 0x01 on successful mutate (in engine) | COMPARE / DELETE / CLEAR / EXPORT (**6.8–6.11**) |
| `setAuditionSelection` for test/future UI | Defrag implementation (**6.10**) |
| Golden-vector + engine unit tests | Recipe session persistence (**6.12**) |
| CMake source registration | Clear history on patch load (**6.13**) |
| | `PluginProcessor` constructs engine (**7.4**) |
| | Button enabled-state graying (**6.12**) |

**Critical boundary:** MUTATE button click routing stays in the empty `MutatorActionHandler` stub until **7.4**. This story makes `PatchMutatorEngine::mutate()` callable and fully tested — 7.4 wires the button.

### MUTATE pipeline (normative — implement exactly)

Per D-083, architecture AD-6, and Story 6.3 engine integration notes:

```
mutate():
  1. If store.isRootIndexExhausted() → return failure + defragModalRequested
  2. recipe = buildRecipeFromApvts()
  3. If recipe.amountPercent == 0 OR recipe.randomPercent == 0 → return failure footer (no insert, no SysEx)
  4. auditionBuffer = resolveAuditionBuffer()     // empty store → *patchModel_; else selected entry.result
  5. parentSnapshot = copy(auditionBuffer)
  6. working = PatchModel(auditionBuffer)
  7. algorithm.apply(working, recipe, rng_)       // rng_ = juce::Random member, seeded per call or session
  8. rootIndex = store.peekNextRootIndex()        // must succeed if step 1 passed
  9. MutationNaming::applyPatchName(working, rootIndex)
 10. store.insertRoot(rootIndex, working, parentSnapshot)
 11. pushResultToEditorAndSynth(working)          // suppress hooks + sendPatch 0x01
 12. return { success: true }
```

**Audition semantics (D-083):** Input is the **auditioned result** (Mi or Mi-Rj), not `parentSnapshot`. RETRY (6.5) uses `parentSnapshot` — do not conflate.

**Empty history:** First MUTATE uses live editor patch; creates **M00** at index 0. No prior `setInitialSnapshot` required for MUTATE (COMPARE/6.8 owns initial snapshot).

**No-op recipe (A=0 or R=0):** Return failure early with English footer (e.g. "Set Amount and Random above 0 to mutate") — no store insert, no SysEx. Algorithm `apply()` still validates early-exit independently in unit tests.

### `resolveAuditionBuffer()` rules

| Store state | Selection | Input buffer |
|---|---|---|
| Empty | (ignored) | `*patchModel_` |
| Non-empty | `setAuditionSelection(Mi, kRootOnly)` | `getEntry(Mi).result` |
| Non-empty | `setAuditionSelection(Mi, Rj)` | `getEntry(Mi, Rj).result` |
| Non-empty | No selection set | **Safe default for tests:** highest sorted root, root-only entry; document in header — 6.6 will drive explicit selection |

### Recipe builder (`buildRecipeFromApvts`)

Read at mutate time — never cache.

| Source | APVTS access |
|---|---|
| Amount | `*apvts_.getParameter(PluginIDs::...::kAmount)` → int 0–100 |
| Random | `*apvts_.getParameter(PluginIDs::...::kRandom)` → int 0–100 |
| Module toggles | `apvts_.state.getProperty(PluginIDs::...::kEnableDco1)` etc. → bool |

Clamp Amount/Random to [0, 100]. Default toggles **false** when property missing.

Toggle → `kGroupId` mapping per spec §2.2 table (1:1 with `PluginIDs::PatchEditSection::*Module::kGroupId`).

### APVTS → editor → SysEx push pattern

Reuse the anonymous helper pattern from `PatchManagerActionHandler.cpp`:

```cpp
void pushPatchModelToApvtsWithSuppress(apvts, hooks, mapper, patchNameSyncer) {
    setPatchLoadSuppressHooks(hooks, true);
    mapper.bufferToApvts();
    if (patchNameSyncer) patchNameSyncer->bufferToApvts();
    flushDeferredApvtsParameterSync(apvts);  // copyState() flush
    setPatchLoadSuppressHooks(hooks, false);
}
```

Then `midiManager_->sendPatch(patchNumber, patchModel_->data())` — copy working buffer into `*patchModel_` before push+send.

**Patch number:** inject `std::function<int()>` resolved from bank/patch APVTS state (same semantics as `PatchManagerActionHandler::getCurrentPatch`). Do not duplicate bank/patch parsing inside engine — accept callback from future processor wiring; tests pass lambda returning `0`.

### `PatchMutatorEngine` constructor (DI table)

| Dependency | Purpose |
|---|---|
| `PatchModel*` | Live editor buffer; mutate read/write target |
| `ApvtsPatchMapper*` | `bufferToApvts` after mutate |
| `PatchNameSyncer*` | Sync PATCH NAME parameter from bytes 0–7 |
| `MidiManager*` | `sendPatch` for 0x01 (nullable in tests) |
| `juce::AudioProcessorValueTreeState&` | Recipe + future UI sync |
| `ActionExecutionHooks` | Suppress per-parameter SysEx during bulk load |
| `std::function<int()> getCurrentPatchNumber` | Target patch slot for SysEx |

Internal members: `MutationHistoryStore historyStore_`, `MutationAlgorithm algorithm_`, `juce::Random rng_`, `int selectedRootIndex_`, `int selectedRetryIndex_`.

### `MutationAlgorithm` implementation guidance

**Descriptor sources:** `ApvtsPatchMapper::buildIntDescriptors()` and `buildChoiceDescriptors()` return vectors with `parentGroupId`, `sysExOffset`, `minValue`, `maxValue`, `choices`.

**Module mask (§5):** For each descriptor, check `recipe.enable*` matching `parentGroupId`. Disabled module → skip (byte-identical).

**Matrix Mod (§7, Option A):** Eligible when `parentGroupId == PluginIDs::MatrixModulationSection::kGroupId` AND `sysExOffset` ∈ [104, 133]. Process even when all ten module toggles false.

**Int write path:** Read current value via `PatchModel` + `PackedFieldCodec` using descriptor — same path as mapper round-trip tests.

**Iteration order:** Deterministic — int descriptors first (build order), then choice descriptors (build order), then Matrix Mod slice ascending offset. Order matters for GV-04 seed reproducibility.

**`uniformRandomInt`:** `lo + rng.nextInt(hi - lo + 1)` per spec §8.2.

### Stub methods (declare now, implement later)

| Method | Story | Stub behaviour |
|---|---|---|
| `retry()` | 6.5 | `{ success: false }` |
| `toggleCompare()` | 6.8 | `{ success: false }` |
| `deleteSelected()` | 6.9 | `{ success: false }` |
| `clearHistory()` | 6.9 | `{ success: false }` |
| `exportHistory()` | 6.11 | `{ success: false }` |
| `defragHistory()` | 6.10 | `{ success: false }` |
| `auditionSelectedHistoryEntry()` | 6.7 | no-op |
| `syncHistoryUiProperties()` | 6.6 | no-op |
| `refreshActionEnabledMirrors()` | 6.12 | no-op |

7.4 compiles against full API; stubs prevent link errors.

### Architecture compliance

- **Core ↛ GUI** — no `juce_gui_*`, no `PluginProcessor` edits in this story
- Path: `Source/Core/Services/PatchMutator/` per AD-6
- One primary class per `.h/.cpp` pair (`MutationAlgorithm`, `PatchMutatorEngine`)
- Naming: PascalCase classes, `lowerCamelCase` methods, `name_` private members
- SysEx orchestration in engine, **not** panel or handler (AD-4, AD-5)
- Descriptor-driven mutation — no parallel `constexpr` byte offsets for patch parameters
- English source/comments only
- Message-thread only (same as `MutationHistoryStore`)

### File structure (this story)

```
Source/Core/Services/PatchMutator/
├── MutationHistoryStore.h/.cpp     (existing — 6.2)
├── MutationNaming.h/.cpp           (existing — 6.3)
├── MutationAlgorithm.h             (NEW)
├── MutationAlgorithm.cpp           (NEW)
├── PatchMutatorEngine.h            (NEW)
└── PatchMutatorEngine.cpp          (NEW)

Tests/Unit/
├── MutationAlgorithmTests.cpp      (NEW)
├── PatchMutatorEngineTests.cpp     (NEW)
├── MutationHistoryStoreTests.cpp   (existing)
└── MutationNamingTests.cpp         (existing)

CMakeLists.txt                      (UPDATE)
```

### CMake registration

Mirror existing `PatchMutator` entries:

- Plugin target (~line 129): add `MutationAlgorithm.cpp`, `PatchMutatorEngine.cpp`
- Test target (~lines 304–364): add both test `.cpp` + compile algorithm + engine `.cpp`

### Testing requirements

**`MutationAlgorithmTests`** — use `InitDefaults::patchData()` from `Source/Core/Init/InitDefaults.h`.

| Test | Vector | Assert |
|---|---|---|
| `gv01_amountZero_noOp` | GV-01 | All 134 bytes equal; `apply` → false |
| `gv01_randomZero_noOp` | GV-01 alt | Same |
| `gv02_dco1Only_changesDco1` | GV-02 seed `0x6D757461` | Bytes 0–7, 8 unchanged; ≥1 byte in {9,10,11,13,25,86,87} differs |
| `gv03_matrixModOnly` | GV-03 seed `0x4D61746D` | Bytes 8–103 unchanged; ≥1 byte 104–133 differs |
| `gv04_choiceDeterministic` | GV-04 seed `0x43484F31` | Pin byte 13 after first run |
| `allTogglesOff_matrixModStillMutates` | GV-03 | Option A scope |
| `moduleToggleMask_disablesModule` | Custom | DCO1 off → DCO1 offsets unchanged |

**`PatchMutatorEngineTests`** — fakes for `MidiManager` (or `std::function` send hook).

| Test | Assert |
|---|---|
| `mutate_emptyHistory_createsM00` | `rootCount()==1`, `getEntry(0)` name `M00` |
| `mutate_gapAllocation` | Insert M00, delete, insert → next index uses gap |
| `mutate_limitBlocks` | 100 roots → `success==false`, `defragModalRequested==true` |
| `mutate_noOpRecipe_blocked` | A=0 → `success==false`, no root inserted, no SysEx |
| `mutate_fromAuditionedRetry` | Preload M00 + M00-R00; select R00; mutate → M01 from R00 result |
| `mutate_sendsSysExOnce` | Fake records exactly one `sendPatch` |
| `mutate_neverDeletesRoots` | Preload M00, M02; mutate → both still present |

Run: `cmake --build Builds/macOS --target Matrix-Control_Tests`

### Previous story intelligence

**6.1 (done):** Approved spec v1.0.1 is normative. Option A for Matrix Mod. GV-04 output hex pinned during 6.4 implementation. Early-exit and `apply()` return rules fixed in review.

**6.2 (done):** `peekNextRootIndex()` returns `0` for first root, gap-aware thereafter. `insertRoot` requires valid index from peek. `parentSnapshot` stored separately from `result`. `clear()` keeps initial snapshot — mutate does not call `clear()`.

**6.3 (done):** `applyPatchName` after algorithm only. Index 0 → `M00`. Store entries must be post-naming before insert. Review: naming must not embed allocation logic.

**6.3 code patterns:** Stateless struct with static methods (`MutationNaming`); follow same namespace `Core` conventions.

### Git intelligence

| Commit | Relevance |
|---|---|
| `0f2078d` | Latest — Story 6-3 `MutationNaming` landed |
| `74a025b` | Story 6-2 — `MutationHistoryStore` |
| `f78f956` / `324fc3d` | Story 6.1 spec approved |
| `8345ebc` | Story 4.6 done — `pushPatchModelToApvtsWithSuppress` pattern stable |

**Brownfield:** `PatchMutator/` has store + naming. This story adds algorithm + engine — third and fourth implementation files in folder.

### Latest tech / framework notes

- **JUCE 8.0.12**, **C++17** — `juce::Random::setSeed` for golden vectors; no new dependencies
- `PatchModel::kBufferSize` == 134 bytes
- `InitDefaults::patchData()` matches spec Appendix A input hex
- No web research required — algorithm fully specified

### Project context reference

- `project-context.md` — English artifacts; Core/GUI separation; tests under `Tests/Unit/`
- `CONVENTIONS.md` — one class per file; Clean Code limits on handler/engine methods
- FR-30 full patch SysEx on mutate; FR-55 root creation; FR-59 defrag modal flag on limit
- D-083 MUTATE input/output semantics; D-087 defrag on blocked mutate

### References

- [epics.md — Story 6.4]
- [mutation-algorithm-spec.md — normative algorithm (v1.0.1)]
- [architecture.md — AD-6 PatchMutatorEngine, AD-4 SysEx routing]
- [.decision-log.md — D-083, D-085, D-087]
- [6-3-mutationnaming-and-display-names.md — naming pipeline order]
- [7-4-mutatoractionhandler.md — engine API contract, handler wiring deferred]
- [PatchManagerActionHandler.cpp — pushPatchModelToApvtsWithSuppress, sendPatch pattern]
- [MutationHistoryStore.h — store API]
- [MutationNaming.h — applyPatchName API]
- [PluginIDs.h — patchMutator* property IDs]
- [InitDefaults.h — golden vector input buffer]

## Dev Agent Record

### Agent Model Used

claude-4.6-sonnet-medium-thinking

### Debug Log References

- GV-04 byte 13 pinned to `0x01` (choice index 1) with seed `0x43484F31` — verified by `MutationAlgorithmTests`.

### Completion Notes List

- Implemented `MutationAlgorithm::apply` per mutation-algorithm-spec v1.0.1: early exit, int jitter, choice re-roll, module mask, Matrix Mod Option A (offsets 104–133), injectable `IRandomSource`.
- Implemented `PatchMutatorEngine::mutate()` full D-083 pipeline: recipe from APVTS state properties, audition buffer resolution, naming, history insert, APVTS push with hook suppress, single SysEx 0x01.
- Declared stub API for Stories 6.5–6.12 and 7.4 (`retry`, `toggleCompare`, `deleteSelected`, etc.).
- Added `MutationAlgorithmTests` (GV-01–GV-04 + mask tests) and `PatchMutatorEngineTests` (M00, gap, limit, no-op, audition retry, SysEx, root preservation).
- Full `Matrix-Control_Tests` and `Matrix-Control_Standalone` Debug builds green.

### File List

- Source/Core/Services/PatchMutator/MutationAlgorithm.h (new)
- Source/Core/Services/PatchMutator/MutationAlgorithm.cpp (new)
- Source/Core/Services/PatchMutator/PatchMutatorEngine.h (new)
- Source/Core/Services/PatchMutator/PatchMutatorEngine.cpp (new)
- Tests/Unit/MutationAlgorithmTests.cpp (new)
- Tests/Unit/PatchMutatorEngineTests.cpp (new)
- CMakeLists.txt (modified)

### Change Log

- 2026-06-19: Story 6.4 created — MutationAlgorithm + PatchMutatorEngine::mutate() developer guide with pipeline, DI table, golden tests, stub API for 7.4, and Epic 6 boundary table.
- 2026-06-19: Code review — 6 patches applied (footer message, M00 test, header doc, dead code, range guard, recipe read hybrid); DI struct deferred to 7.4; story marked done.

## Story Completion Status

- Ultimate context engine analysis completed — comprehensive developer guide created
- Status: **review**
