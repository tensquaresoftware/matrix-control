---
organization: Ten Square Software
project: Matrix-Control
title: Story 1.5 — Patch Name Bytes 0–7 Sync
author: BMad Agent
status: done
baseline_commit: 8b0cfb1021750fbce88af158318b57a59443822b
sources:
  - planning-artifacts/epics.md
  - planning-artifacts/architecture/architecture-Matrix-Control-2026-05-25/architecture.md
  - implementation-artifacts/1-4-apvtsmastermapper-round-trip.md
  - implementation-artifacts/1-3-apvtspatchmapper-round-trip.md
created: 2026-06-04
---

# Story 1.5: Patch Name Bytes 0–7 Sync

Status: done

## Story

As a sound designer,
I want the patch name in APVTS to reflect bytes 0–7 of the packed buffer,
so that PATCH NAME display and `.syx` filenames stay aligned (FR-13).

## Acceptance Criteria

1. `PatchNameSyncer` lives in `Source/Core/Models/PatchNameSyncer.{h,cpp}`, takes `juce::AudioProcessorValueTreeState&` and `Core::PatchModel&` as constructor parameters. Zero GUI dependencies, zero SysEx dispatch — sync only.
2. `apvtsToBuffer()` reads the string property keyed by `PluginIDs::PatchEditSection::PatchNameModule::kPatchName` from `apvts_.state`; truncates to `PatchModel::kNameLength` (8) characters; calls `patchModel_.setName(truncated)`. If the property is absent, it is treated as an empty string (silent no-op for the buffer).
3. `bufferToApvts()` (message-thread only — same contract as `ApvtsPatchMapper`) reads `patchModel_.getName()` and sets `apvts_.state.setProperty(kPatchName, name, nullptr)`.
4. `PluginIDs::PatchEditSection` gains a new sub-namespace `PatchNameModule` containing `constexpr const char* kPatchName = "patchName"`.
5. `PluginProcessor` gains `std::unique_ptr<Core::PatchNameSyncer> patchNameSyncer_` (after `apvtsPatchMapper_` in member list); public getter `getPatchNameSyncer()`; forward declaration in header. The constructor initialises the `patchName` property on `apvts.state` to `PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName` if absent (same guard pattern as `midiInputPortId`).
6. `PluginProcessor::valueTreePropertyChanged` detects `parameterId == PluginIDs::PatchEditSection::PatchNameModule::kPatchName` and calls `patchNameSyncer_->apvtsToBuffer()`.
7. `PatchNameSyncerTests` (`Tests/Unit/`) uses a `juce::UnitTest` subclass. It covers: (a) `bufferToApvts` direction — `PatchModel::setName` then `bufferToApvts`, verify `apvts.state` property equals `patchModel.getName()`; (b) `apvtsToBuffer` direction — set property on `apvts.state` then `apvtsToBuffer`, verify `patchModel.getName()` matches; (c) truncation — set property to a 12-char string, call `apvtsToBuffer`, verify buffer stores first 8 chars uppercase; (d) reference round-trip — load `Tests/Fixtures/Patches/` fixture, call `bufferToApvts`, verify property is non-empty and ≤ 8 chars.
8. `Source/Core/Models/PatchNameSyncer.cpp` is added to `PLUGIN_SOURCES` and to `Matrix-Control_Tests` `target_sources`; `Tests/Unit/PatchNameSyncerTests.cpp` is added to the test target. All existing 27 tests pass.

## Tasks / Subtasks

- [x] **Add `kPatchName` to `PluginIDs.h`** (AC: #4)
  - [x] Add `PatchNameModule` namespace inside `PatchEditSection` with `constexpr const char* kPatchName = "patchName"`

- [x] **Implement `PatchNameSyncer`** (AC: #1, #2, #3)
  - [x] Create `Source/Core/Models/PatchNameSyncer.h`
  - [x] Create `Source/Core/Models/PatchNameSyncer.cpp`
  - [x] Verify: class ≤ 200 lines, all methods ≤ 15 lines, no magic numbers, `kNameLength` via `PatchModel::kNameLength`

- [x] **Wire into `PluginProcessor`** (AC: #5, #6)
  - [x] Add forward declaration `class PatchNameSyncer;` in namespace `Core` in `PluginProcessor.h`
  - [x] Add `patchNameSyncer_` member and `getPatchNameSyncer()` getter
  - [x] Add constructor init-list entry and `initializePatchNameProperty()` private helper (guard like `midiInputPortId`)
  - [x] Add check in `valueTreePropertyChanged` for `kPatchName`

- [x] **Write `Tests/Unit/PatchNameSyncerTests.cpp`** (AC: #7)
  - [x] Test A: `bufferToApvts` direction
  - [x] Test B: `apvtsToBuffer` direction
  - [x] Test C: truncation (> 8 chars)
  - [x] Test D: reference round-trip with `.syx` fixture

- [x] **Update `CMakeLists.txt`** (AC: #8)
  - [x] Add `Source/Core/Models/PatchNameSyncer.cpp` to `PLUGIN_SOURCES`
  - [x] Add `Source/Core/Models/PatchNameSyncer.cpp` to test target `target_sources`
  - [x] Add `Tests/Unit/PatchNameSyncerTests.cpp` to test target
  - [x] Rebuild; confirm all 27 + new tests pass, VST3 builds cleanly

## Dev Notes

### Why NOT an `AudioParameter` — Critical Design Constraint

JUCE 8 has no `AudioParameterString` or `AudioParameterText` built-in type. Patch names are **not automatable parameters** — they are metadata. The correct JUCE 8 idiom for non-numeric plugin state is to store directly as a `juce::String` property on `apvts_.state` (the root `juce::ValueTree`). This is already the pattern used in this project:

```cpp
// PluginProcessor.cpp — exact same pattern to follow:
apvts.state.setProperty("midiInputPortId", deviceId, nullptr);
// ... read back:
apvts.state.getProperty("midiInputPortId").toString()
```

`PatchNameSyncer` follows this pattern exactly. Never create an `AudioParameterInt` per character or invent a custom `AudioProcessorParameter` subclass.

### `valueTreePropertyChanged` — How String Properties Route

When `apvts.state.setProperty("patchName", ...)` is called:
- `valueTreePropertyChanged` fires
- `property.toString()` = `"patchName"` (not `"value"`)
- `resolveParameterIdFromTree` early-exits: `if (propertyId != ApvtsTypes::kValue) return propertyId`
- Returns `"patchName"` as `parameterId`
- Current code checks `patchParameterIds_.count(parameterId)` → zero (not in that set)
- **Add after the existing mapper checks:**
  ```cpp
  if (parameterId == PluginIDs::PatchEditSection::PatchNameModule::kPatchName)
      patchNameSyncer_->apvtsToBuffer();
  ```

### `PatchModel` Already Handles Encoding

`PatchModel::setName()` and `getName()` already manage:
- Bytes 0–7 of the 134-byte buffer
- 6-bit ASCII Matrix charset encoding (low 6 bits, codes < `0x20` mapped to `0x40+`)
- `setName` folds to uppercase (Matrix display is uppercase-only)
- Truncation to 8 chars with space-padding for shorter strings
- `kNameLength = 8` and `kBufferSize = 134` are constants on `PatchModel`

`PatchNameSyncer` must **not** re-implement any charset logic. It simply calls:
```cpp
patchModel_.setName(truncatedString);   // apvtsToBuffer
patchModel_.getName();                  // bufferToApvts
```

### Truncation Must Happen in Syncer, Not in PatchModel

`PatchModel::setName` truncates silently. The story requires "validator feedback", so `apvtsToBuffer()` should truncate the input string to `PatchModel::kNameLength` characters **before** calling `setName`, so the caller can observe the limit. The validator feedback (AC wording) refers to the 8-char cap being enforced — no exception or error is needed at this stage (GUI validation comes in a future story).

### `PluginProcessor` Initialisation Pattern

Follow the `midiInputPortId` guard pattern exactly:
```cpp
if (!apvts.state.hasProperty(PluginIDs::PatchEditSection::PatchNameModule::kPatchName))
    apvts.state.setProperty(PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
                            juce::String(PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName),
                            nullptr);
```

This must happen **before** `apvts.addListener(this)` fires (or at minimum before any external APVTS access). Place the call in a new `initializePatchNameProperty()` private method, called from the constructor in the same location as `initializeMidiPortProperties()`.

Constructor init-list order (model before syncer):
```cpp
, patchModel_{ std::make_unique<Core::PatchModel>() }
, apvtsPatchMapper_{ std::make_unique<Core::ApvtsPatchMapper>(apvts, *patchModel_) }
, patchNameSyncer_{ std::make_unique<Core::PatchNameSyncer>(apvts, *patchModel_) }
```

### `PatchNameSyncer` Class Skeleton

```cpp
// PatchNameSyncer.h
namespace Core
{
    class PatchModel;

    class PatchNameSyncer
    {
    public:
        PatchNameSyncer(juce::AudioProcessorValueTreeState& apvts, PatchModel& model);

        void apvtsToBuffer();

        // Message thread only.
        void bufferToApvts();

    private:
        static juce::String truncateToMaxLength(const juce::String& name);

        juce::AudioProcessorValueTreeState& apvts_;
        PatchModel& model_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchNameSyncer)
    };
}
```

All methods ≤ 15 lines. `truncateToMaxLength` uses `PatchModel::kNameLength`, no magic number.

### Threading

Same contract as `ApvtsPatchMapper::bufferToApvts()`:
- `apvtsToBuffer()` — any thread (reads `ValueTree` property, calls `PatchModel::setName`)
- `bufferToApvts()` — **message thread only** — calls `apvts_.state.setProperty(..., nullptr)` which notifies listeners synchronously on the calling thread

Add the same comment as `ApvtsPatchMapper`:
```cpp
// Message thread only — ValueTree setProperty notifies listeners synchronously.
void bufferToApvts();
```

### Test Fixture — Use Existing Patches

`Tests/Fixtures/Patches/` contains reference `.syx` files. Use the same fixture-loading approach as `ApvtsPatchMapperTests`. The fixture decode path: `SysExDecoder::decodePatchSysEx(raw, patchBuffer)` then `patchModel.loadFrom(patchBuffer.data())`. After loading, `bufferToApvts()` should produce a property value that is non-empty and ≤ 8 chars. Check `Tests/Unit/ApvtsPatchMapperTests.cpp` for fixture loading boilerplate.

### No Changes to `PluginDescriptors` or `ApvtsPatchMapper`

Story 1-5 is additive. Do NOT modify:
- `PluginDescriptors.h` / any `PluginDescriptors*.cpp` — no new descriptor struct needed
- `ApvtsPatchMapper` — patch name is out of scope for the numeric mapper
- `PatchModel` — `getName()`/`setName()` are complete and sufficient

### `PluginIDs.h` — Where to Add

Insert the `PatchNameModule` namespace **inside** `PluginIDs::PatchEditSection`, after `Lfo2Module` and before the closing brace of `PatchEditSection`:

```cpp
namespace PatchNameModule
{
    constexpr const char* kPatchName = "patchName";
}
```

No `kGroupId` is needed (patch name is a single property, not a group).

### `CMakeLists.txt` — Lines to Update

Existing Models block (lines ~104–108):
```cmake
Source/Core/Models/ApvtsMasterMapper.cpp
Source/Core/Models/ApvtsPatchMapper.cpp
```
Add: `Source/Core/Models/PatchNameSyncer.cpp` to both `PLUGIN_SOURCES` and the test target `target_sources`.

Existing test sources block (lines ~226–238):
Add `Tests/Unit/PatchNameSyncerTests.cpp` after `ApvtsMasterMapperTests.cpp`.

### Project Structure Notes

- New files land in `Source/Core/Models/` (consistent with `PatchModel`, `ApvtsPatchMapper`)
- `PatchNameSyncer` stays in `Core` namespace (no GUI deps)
- `PluginIDs.h` in `Source/Shared/Definitions/` — only file outside `Core/` modified
- Test file in `Tests/Unit/` — existing pattern
- No new CMake targets; only `target_sources` additions

### References

- `PatchModel::getName()/setName()` — [Source/Core/Models/PatchModel.h:37-41], [Source/Core/Models/PatchModel.cpp:53-79]
- `kNameLength = 8` — [Source/Core/Models/PatchModel.h:21]
- `midiInputPortId` property pattern — [Source/Core/PluginProcessor.cpp:207,243-245]
- `resolveParameterIdFromTree` early-exit — [Source/Core/PluginProcessor.cpp:403-405]
- `valueTreePropertyChanged` mapper checks — [Source/Core/PluginProcessor.cpp:458-462]
- `kDefaultPatchName = "--------"` — [Source/Shared/Definitions/PluginDisplayNames.h:592]
- `ApvtsPatchMapper` pattern — [Source/Core/Models/ApvtsPatchMapper.h], [Source/Core/Models/ApvtsPatchMapper.cpp]
- `buildPatchParameterIdSet` / `buildMasterParameterIdSet` — [Source/Core/PluginProcessor.cpp:540-556]
- CMakeLists Models block — [CMakeLists.txt:104-108]; test sources block — [CMakeLists.txt:226-238]
- Story 1-4 learnings — [_bmad-output/implementation-artifacts/1-4-apvtsmastermapper-round-trip.md]

## Review Findings

### Patches

- [x] [Review][Patch] Absent `patchName` property must be a no-op (AC#2) — added `if (!apvts_.state.hasProperty(kPatchName)) return;` to `apvtsToBuffer`. [Source/Core/Models/PatchNameSyncer.cpp:15] (sources: auditor+edge+blind; resolved from decision D1 → option 1)
- [x] [Review][Patch] `valueTreeRedirected` resyncs patch name on state load — added `patchNameSyncer_->apvtsToBuffer()` alongside patch+master mapper resync. [Source/Core/PluginProcessor.cpp:554] (sources: blind+edge)

### Deferred

- [x] [Review][Defer] `apvtsToBuffer` has no thread contract and allocates — `valueTreePropertyChanged` may fire off the message thread; `getProperty().toString()` + `setName` allocate. Shared pre-existing pattern with `ApvtsPatchMapper`. [Source/Core/Models/PatchNameSyncer.cpp:15] — deferred, pre-existing
- [x] [Review][Defer] Lossy 6-bit/7-bit charset round-trip in `PatchModel` — `setName` masks 0x7F, `getName` masks 0x3F + remaps < 0x20; identity only in printable 0x20-0x5F band. Pre-existing `PatchModel` behavior, out of story scope. [Source/Core/Models/PatchModel.cpp:62] — deferred, pre-existing
- [x] [Review][Defer] Model name not seeded from default at startup — default `"--------"` set into APVTS before `addListener`, so `apvtsToBuffer` never runs at construction; model name stays zero-init (decodes `"@@@@@@@@"`) until first edit/patch load. Matches existing mapper construction pattern. [Source/Core/PluginProcessor.cpp:271] — deferred, pre-existing

## Dev Agent Record

### Agent Model Used

claude-sonnet-4-6

### Debug Log References

### Completion Notes List

- `PatchNameModule` namespace added to `PluginIDs::PatchEditSection` with `kPatchName = "patchName"`.
- `PatchNameSyncer` created in `Source/Core/Models/` — 2 methods + 1 private helper, all ≤ 15 lines, zero GUI deps.
- `PluginProcessor` wired: forward decl, `patchNameSyncer_` member, `getPatchNameSyncer()` getter, `initializePatchNameProperty()` guard (mirrors `midiInputPortId` pattern), `valueTreePropertyChanged` check.
- `PluginDisplayNames.h` included in `PluginProcessor.cpp` for `kDefaultPatchName`.
- 4 new tests in `PatchNameSyncerTests.cpp` cover all 4 AC#7 scenarios; all pass.
- 27 pre-existing tests still pass. VST3 builds cleanly.

### File List

- `Source/Shared/Definitions/PluginIDs.h` (modified)
- `Source/Core/Models/PatchNameSyncer.h` (new)
- `Source/Core/Models/PatchNameSyncer.cpp` (new)
- `Source/Core/PluginProcessor.h` (modified)
- `Source/Core/PluginProcessor.cpp` (modified)
- `Tests/Unit/PatchNameSyncerTests.cpp` (new)
- `CMakeLists.txt` (modified)
- `_bmad-output/implementation-artifacts/1-5-patch-name-bytes-0-7-sync.md` (modified)
- `_bmad-output/implementation-artifacts/sprint-status.yaml` (modified)
