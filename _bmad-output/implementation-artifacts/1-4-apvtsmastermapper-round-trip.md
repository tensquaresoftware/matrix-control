---
story_key: 1-4-apvtsmastermapper-round-trip
epic: 1
story: 4
status: done
baseline_commit: 110a37f058c13efb63f54c178aa9a3540ad49284
---

# Story 1.4: ApvtsMasterMapper Round-Trip

Status: done

## Story

As a sound designer,
I want APVTS MASTER parameters synced with `MasterModel`,
So that master editing uses the same SSOT pattern (FR-49, FR-16 foundation).

## Acceptance Criteria

1. `ApvtsMasterMapper` lives in `Source/Core/Models/ApvtsMasterMapper.{h,cpp}`, takes `juce::AudioProcessorValueTreeState&` and `Core::MasterModel&` as constructor parameters. Zero GUI dependencies, zero SysEx dispatch — it maps values only.
2. `apvtsToBuffer()` iterates the `MasterEditSection::kIntParameters` and `MasterEditSection::kChoiceParameters` section-level aggregates (unlike PatchMapper which iterates per-module, MasterEditSection already provides combined vectors), reads each parameter's raw APVTS value via `getRawParameterValue`, and writes it to `MasterModel` via `setValue`/`setChoiceIndex`. No offset table exists outside `PluginDescriptors`.
3. `bufferToApvts()` iterates the same descriptor sets, reads each value from `MasterModel`, and pushes it to the APVTS `AudioParameter` via `setValueNotifyingHost(param->convertTo0to1(value))`. Callers must ensure message-thread context.
4. `PluginProcessor` gains `std::unique_ptr<Core::MasterModel> masterModel_` and `std::unique_ptr<Core::ApvtsMasterMapper> apvtsMasterMapper_`; public getters `getMasterModel()` and `getApvtsMasterMapper()` are added. `valueTreePropertyChanged` identifies master parameters via a cached `masterParameterIds_` set and calls `apvtsMasterMapper_->apvtsToBuffer()` on a match.
5. No duplicate SysEx offset tables exist anywhere outside `PluginDescriptors` after this story.
6. `ApvtsMasterMapperTests` (`Tests/Unit/`) uses a `juce::UnitTest` subclass. It covers: (a) APVTS→model direction; (b) model→APVTS direction; (c) a reference round-trip using `Tests/Fixtures/Masters/Master 1.syx` and non-zero bytes resolved at runtime.
7. `Source/Core/Models/ApvtsMasterMapper.cpp`, `Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp`, and `Tests/Unit/ApvtsMasterMapperTests.cpp` are added to `CMakeLists.txt` test target `target_sources`. All existing tests pass unchanged.

## Tasks / Subtasks

- [x] **Implement `ApvtsMasterMapper`** (AC: #1, #2, #3, #5)
  - [x] Create `Source/Core/Models/ApvtsMasterMapper.h`
  - [x] Create `Source/Core/Models/ApvtsMasterMapper.cpp` — `buildIntDescriptors()` returns `MasterEditSection::kIntParameters`, `buildChoiceDescriptors()` returns `MasterEditSection::kChoiceParameters`
  - [x] Verify: class ≤ 200 lines, all methods ≤ 15 lines, no magic numbers
- [x] **Wire into `PluginProcessor`** (AC: #4)
  - [x] Add `masterModel_`, `apvtsMasterMapper_`, `masterParameterIds_` to header; forward-declare `Core::MasterModel` and `Core::ApvtsMasterMapper`
  - [x] Add public getters `getMasterModel()` and `getApvtsMasterMapper()`
  - [x] Add `buildMasterParameterIdSet()` private method; call from constructor after `buildPatchParameterIdSet()`
  - [x] Extend initializer list and `valueTreePropertyChanged`
- [x] **Write `Tests/Unit/ApvtsMasterMapperTests.cpp`** (AC: #6)
  - [x] Tests A (APVTS→buffer), B (buffer→APVTS), C (reference round-trip with Master 1.syx)
- [x] **Update `CMakeLists.txt`** (AC: #7)
  - [x] Add `ApvtsMasterMapper.cpp` to test target and plugin PLUGIN_SOURCES; `PluginDescriptorsMasterEdit.cpp` omitted from test target (already covered by `PluginDescriptors.cpp` which contains all MasterEdit definitions); add `ApvtsMasterMapperTests.cpp` to test target
  - [x] Rebuild and confirm all 27 tests pass, VST3 plugin builds cleanly

## Dev Notes

### Key difference from `ApvtsPatchMapper`

`MasterEditSection` already provides section-level aggregates (`kIntParameters`, `kChoiceParameters` at `PluginDescriptors.h:129-131`). `buildIntDescriptors()` returns that vector directly — no per-module iteration needed. `buildChoiceDescriptors()` similarly.

### Parameter count

- Int: MidiModule (4) + VibratoModule (4) + MiscModule (3) = 11
- Choice: MidiModule (4) + VibratoModule (3) + MiscModule (4) = 11
- Total: 22 master parameters

### `PluginProcessor` integration

Constructor initializer list order (model before mapper):
```
, masterModel_{ std::make_unique<Core::MasterModel>() }
, apvtsMasterMapper_{ std::make_unique<Core::ApvtsMasterMapper>(apvts, *masterModel_) }
```

`buildMasterParameterIdSet()` mirrors `buildPatchParameterIdSet()` — calls `ApvtsMasterMapper::buildIntDescriptors()` and `buildChoiceDescriptors()`.

In `valueTreePropertyChanged`, add alongside the patch check:
```cpp
if (masterParameterIds_.count(parameterId) > 0)
    apvtsMasterMapper_->apvtsToBuffer();
```

### Test C — reference round-trip

`Master 1.syx` exists at `Tests/Fixtures/Masters/`. Decode with `decoder.decodeMasterSysEx(raw, buf)`. The fixture carries default (zero) signed values, but unsigned int params (Pedal1Select offset 17, Pedal2Select offset 18, Lever2Select offset 19, Lever3Select offset 20) may be non-zero. Find descriptors adaptively at runtime: iterate `buildIntDescriptors()`, pick those where `decoded[d.sysExOffset] != 0` and value is in `[d.minValue, d.maxValue]`. If none found, skip the round-trip sub-assertion (don't fail the test).

### CMakeLists.txt

Add to `Matrix-Control_Tests` target_sources:
- `Source/Core/Models/ApvtsMasterMapper.cpp`
- `Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp`
- `Tests/Unit/ApvtsMasterMapperTests.cpp`

Also add `ApvtsMasterMapper.cpp` to the plugin `PLUGIN_SOURCES`.

### Threading invariants

Same as Story 1.3: `bufferToApvts()` is message-thread-only. Add the same comment to the method header.

## Dev Agent Record

### Agent Model Used

claude-sonnet-4-6

### Debug Log References

### Completion Notes List

- Implemented `ApvtsMasterMapper` mirroring `ApvtsPatchMapper` pattern. Key difference: `buildIntDescriptors()` / `buildChoiceDescriptors()` return `MasterEditSection::kIntParameters` / `kChoiceParameters` section-level aggregates directly (no per-module iteration needed — those aggregates are already defined in `PluginDescriptors.cpp`).
- Discovered that `PluginDescriptors.cpp` already contains all `MasterEditSection::*` definitions (not just the APVTS group hierarchy). Adding `PluginDescriptorsMasterEdit.cpp` to the test target caused 12 duplicate symbol linker errors. Fix: omit it from the test target (covered by `PluginDescriptors.cpp`).
- Added `MasterModel.cpp` and `ApvtsMasterMapper.cpp` to `PLUGIN_SOURCES` (both required by `PluginProcessor` instantiation).
- `PluginProcessor` wired with `masterModel_`, `apvtsMasterMapper_`, `masterParameterIds_`, `buildMasterParameterIdSet()`, and `valueTreePropertyChanged` check — symmetric to the patch side.
- 27 total tests pass (3 new ApvtsMasterMapper + 24 existing). VST3 builds and deploys cleanly. Timer assertions benign (same pre-existing headless context issue as story 1.3).

### File List

- Source/Core/Models/ApvtsMasterMapper.h (new)
- Source/Core/Models/ApvtsMasterMapper.cpp (new)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/ApvtsMasterMapperTests.cpp (new)
- CMakeLists.txt (modified)

### Review Findings

- [x] [Review][Patch] `isSyncingBufferToApvts_` never set to `true` — removed dead flag and guard; no current call site for `bufferToApvts()`; reintroduce with `ScopedValueSetter` when wired. [Source/Core/PluginProcessor.cpp:458 / Source/Core/PluginProcessor.h:117]
- [x] [Review][Patch] Signed/unsigned comparison `result.size() >= 3` in `findRoundTripDescriptors` — fixed to `3u`. [Tests/Unit/ApvtsMasterMapperTests.cpp:104]
- [x] [Review][Defer] Hardcoded vector indices `intDescs[0]`, `intDescs[2]` in tests — fragile if descriptor order changes [Tests/Unit/ApvtsMasterMapperTests.cpp:130] — deferred, minor fragility, tests pass and document expected mapping

## Change Log

- 2026-06-03: Implemented `Core::ApvtsMasterMapper` (AC #1-3, #5); wired into `PluginProcessor` with `valueTreePropertyChanged` trigger (AC #4); added `ApvtsMasterMapperTests` covering APVTS→buffer, buffer→APVTS, and reference round-trip with Master 1.syx (AC #6); updated CMakeLists.txt for both test and plugin targets (AC #7). All 27 tests pass.
