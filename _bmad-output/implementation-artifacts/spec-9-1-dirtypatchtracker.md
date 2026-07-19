---
title: '9.1 DirtyPatchTracker'
type: 'feature'
created: '2026-07-19'
status: 'done'
baseline_commit: '7f50b688c11e5ef2ac1c13e0d331c0856d21601d'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/project-context.md'
  - '{project-root}/_bmad-output/implementation-artifacts/epic-9-context.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** Core has no way to know whether the current patch diverged from the last loaded or synced snapshot, so Epic 9 cannot yet warn before navigation loses unsaved edits (FR-51 foundation).

**Approach:** Add a Core-only `DirtyPatchTracker` that stores a 134-byte PATCH packed snapshot, reports dirty via full-buffer compare, own it from `PluginProcessor`, and cover load/edit/save/reset with unit tests — without wiring the FR-51 modal or navigation gates (Story 9.2).

## Boundaries & Constraints

**Always:**
- Compare the full `PatchModel::kBufferSize` (134) packed buffer, including patch name bytes 0–7.
- When deriving current state from APVTS, run `ApvtsPatchMapper::apvtsToBuffer` and `PatchNameSyncer::apvtsToBuffer` before compare (same pairing used elsewhere for load/save).
- Keep the tracker in Core (`Source/Core/Services/DirtyPatchTracker.*`); no GUI includes; English-only source.
- Own as `std::unique_ptr` on `PluginProcessor` after `patchModel_` exists (same composition-root pattern as other Core services).
- Until the first `captureSnapshot`, `isDirty` is false (no baseline yet).
- Unit tests cover: capture → clean; mutate → dirty; re-capture / clear → clean; save-style reset; no-snapshot behaviour.

**Ask First:**
- Wiring `captureSnapshot` into live `PatchManagerActionHandler` load/save/STORE success paths in this story (default: leave for 9.2; tests drive the API).
- Changing Mutator history gate (`confirmPatchContextChange`) or merging it with dirty-state logic.
- Treating Master / Settings / Patch Manager UI state as part of dirty compare.

**Never:**
- FR-51 confirmation modal, Settings warn-always/never policy UI or param (not in APVTS yet), or navigation gating (internal prev/next, bank, patch #, Computer combobox/prev/next, OPEN).
- Comparing MasterModel / master APVTS, mutator history mirrors as the live baseline, or folder/combobox UI state.
- Silent-failure paths that swallow compare errors; keep Core ↛ GUI.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|----------|--------------|---------------------------|----------------|
| No baseline | Never captured | `hasSnapshot()==false`, `isDirty(...)==false` | N/A |
| Capture then unchanged | `captureSnapshot(model)` then same bytes | Clean | N/A |
| Capture then edit | Change any packed byte (param or name) | Dirty | N/A |
| Re-capture after edit | `captureSnapshot` again | Clean | N/A |
| Clear snapshot | `clearSnapshot` after dirty | No baseline; not dirty | N/A |
| APVTS path | Sync mapper + name syncer into model, then `isDirty` | Matches packed compare | Message-thread only for APVTS sync helpers |
| Master-only change | Only Master APVTS changed | Still clean (Master out of scope) | N/A |

</frozen-after-approval>

## Code Map

- `Source/Core/Models/PatchModel.h` -- 134-byte packed PATCH SSOT (`kBufferSize`)
- `Source/Core/Models/ApvtsPatchMapper.{h,cpp}` -- APVTS ↔ packed buffer for Patch Edit + Matrix Mod
- `Source/Core/Models/PatchNameSyncer.{h,cpp}` -- APVTS patch name ↔ bytes 0–7 (required with mapper)
- `Source/Core/Services/PatchMutator/MutationHistoryStore.{h,cpp}` -- snapshot `memcpy` / `memcmp` pattern to mirror
- `Source/Core/PluginProcessor.{h,cpp}` -- composition root; add `dirtyPatchTracker_` ownership (+ optional getter)
- `Source/Core/Actions/PatchManagerActionHandler.cpp` -- future capture sites (load/save/STORE); do not gate nav in 9.1
- `CMakeLists.txt` -- `PLUGIN_SOURCES` + `Matrix-Control_Tests` sources for tracker + tests
- `Tests/Unit/ApvtsPatchMapperTests.cpp` / `PatchModelTests.cpp` -- fixture/APVTS patterns to follow
- `_bmad-output/implementation-artifacts/epic-9-context.md` -- Epic 9 goal and 9.1→9.2 split

## Tasks & Acceptance

**Execution:**
- [x] `Source/Core/Services/DirtyPatchTracker.h` -- declare capture/clear/hasSnapshot/isDirty (+ optional APVTS sync helper) -- Core FR-51 foundation API
- [x] `Source/Core/Services/DirtyPatchTracker.cpp` -- store snapshot; full-buffer `memcmp`; no GUI -- implement compare SSOT
- [x] `Source/Core/PluginProcessor.h` / `.cpp` -- construct and own `dirtyPatchTracker_` -- composition-root wiring only
- [x] `CMakeLists.txt` -- register tracker `.cpp` on plugin + tests targets; add unit test file -- build/test discoverability
- [x] `Tests/Unit/DirtyPatchTrackerTests.cpp` -- cover I/O matrix scenarios (load/edit/save/reset/no-snapshot) -- story AC

**Acceptance Criteria:**
- Given no snapshot yet, when `isDirty` is queried, then the result is false and `hasSnapshot` is false.
- Given a captured snapshot and an unchanged packed buffer, when compared, then the tracker reports clean.
- Given a captured snapshot and any change to the packed PATCH buffer (including name bytes), when compared, then the tracker reports dirty.
- Given a dirty state, when snapshot is re-captured or cleared (save/reset cases), then the tracker returns to clean / no-baseline as designed.
- Given Master-only APVTS edits, when PATCH packed compare runs, then dirty state is unaffected.
- Given the running plugin, when Story 9.1 ships, then no FR-51 modal or navigation gate appears (reserved for 9.2).

## Spec Change Log

## Design Notes

Mirror `MutationHistoryStore::setInitialSnapshot`: copy 134 bytes into an owned `PatchModel` (or `std::array`) and compare with `std::memcmp` on `data()` / `kBufferSize`. Prefer a lean API that takes an already-synced `PatchModel&` for `isDirty`; if a convenience helper syncs APVTS, document message-thread-only and always pair mapper + name syncer.

Do not inject the tracker into `PatchManagerActionHandler` in 9.1 unless Ask First is answered yes. Document intended future capture points for 9.2: after successful internal device load, Computer file load (`applyLoadedPatchToApvtsAndSynth`), successful SAVE, and post-STORE (D-074). `confirmPatchContextChange` remains Mutator-history-only until 9.2.

## Verification

**Commands:**
- `cmake --build --preset macos-debug-arm64 --target Matrix-Control_Tests` -- expected: build succeeds with new sources
- `ctest --preset macos-debug-arm64 -R DirtyPatchTracker` (or run the unit-test binary filtering `DirtyPatchTracker`) -- expected: all new cases pass

**Manual checks (if no CLI):**
- Confirm `DirtyPatchTracker` symbols link in the plugin target and no navigation UI/modal changed in a smoke open of Patch Manager.

## Suggested Review Order

**Tracker API**

- Entry point: packed-buffer snapshot + full-buffer dirty compare
  [`DirtyPatchTracker.h:13`](../../Source/Core/Services/DirtyPatchTracker.h#L13)

- Capture / clear / memcmp implementation (clear zeroes snapshot bytes)
  [`DirtyPatchTracker.cpp:11`](../../Source/Core/Services/DirtyPatchTracker.cpp#L11)

- APVTS sync helper overwrites model before compare — documented contract
  [`DirtyPatchTracker.h:28`](../../Source/Core/Services/DirtyPatchTracker.h#L28)

**Composition root**

- Own tracker next to other Core services; no handler injection in 9.1
  [`PluginProcessor.cpp:173`](../../Source/Core/PluginProcessor.cpp#L173)

**Tests & build**

- I/O matrix coverage including APVTS-edit and Master isolation
  [`DirtyPatchTrackerTests.cpp:56`](../../Tests/Unit/DirtyPatchTrackerTests.cpp#L56)

- Register sources on plugin and test targets
  [`CMakeLists.txt`](../../CMakeLists.txt)

### Review Findings

- [x] [Review][Patch] Add APVTS-driven patch-name dirty coverage through `syncApvtsAndIsDirty` [Tests/Unit/DirtyPatchTrackerTests.cpp:164] — param APVTS path is tested; name-syncer half of the Always pairing is only exercised via `PatchModel::setName`, not via APVTS property → helper.
- [x] [Review][Defer] No public getter / no live captureSnapshot sites [PluginProcessor.h:333] — deferred, intentional 9.1 Ask First (already in deferred-work for 9.2)
- [x] [Review][Defer] Capture sites must sync APVTS→model before captureSnapshot — deferred, intentional 9.1 lean API (already in deferred-work for 9.2)
- [x] [Review][Defer] Patch name encode/decode asymmetry can false-dirty after hardware capture + APVTS name sync [PatchModel.cpp:53-69] — deferred, pre-existing
