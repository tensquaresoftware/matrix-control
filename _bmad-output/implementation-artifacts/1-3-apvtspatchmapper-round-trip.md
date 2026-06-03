---
story_key: 1-3-apvtspatchmapper-round-trip
epic: 1
story: 3
status: review
baseline_commit: cc2936b
---

# Story 1.3: ApvtsPatchMapper Round-Trip

Status: review

## Story

As a sound designer,
I want APVTS PATCH parameters synced with `PatchModel`,
So that widget edits and the packed buffer stay consistent (FR-49, FR-10 foundation).

## Acceptance Criteria

1. `ApvtsPatchMapper` lives in `Source/Core/Models/ApvtsPatchMapper.{h,cpp}`, takes `juce::AudioProcessorValueTreeState&` and `Core::PatchModel&` as constructor parameters. Zero GUI dependencies, zero SysEx dispatch — it maps values only.
2. `apvtsToBuffer()` iterates **all 10** `PatchEditSection::*Module` int and choice descriptor vectors in the same order as `ApvtsFactory::getAllIntParameters/getAllChoiceParameters`, reads each parameter's raw APVTS value via `getRawParameterValue`, and writes it to `PatchModel` via `setValue`/`setChoiceIndex`. No offset table exists outside `PluginDescriptors`.
3. `bufferToApvts()` iterates the same descriptor sets, reads each value from `PatchModel`, and pushes it to the APVTS `AudioParameter` via `setValueNotifyingHost(param->convertTo0to1(value))`. Callers must ensure message-thread context.
4. `PluginProcessor` gains `std::unique_ptr<Core::PatchModel> patchModel_` and `std::unique_ptr<Core::ApvtsPatchMapper> apvtsPatchMapper_`; public getters `getPatchModel()` and `getApvtsPatchMapper()` are added. `valueTreePropertyChanged` identifies patch parameters via a cached `patchParameterIds_` set and calls `apvtsPatchMapper_->apvtsToBuffer()` on a match.
5. No duplicate SysEx offset tables exist anywhere outside `PluginDescriptors` after this story.
6. `ApvtsPatchMapperTests` (`Tests/Unit/`) uses a `juce::UnitTest` subclass with a minimal `TestAudioProcessor` hosting APVTS. It covers: (a) APVTS→model direction with synthetic descriptors; (b) model→APVTS direction with the same; (c) a reference round-trip using the committed `Patch 71.syx` fixture and a small set of well-known byte offsets verified against the decoded SysEx.
7. `Source/Core/Models/ApvtsPatchMapper.cpp` and `Tests/Unit/ApvtsPatchMapperTests.cpp` are added to `CMakeLists.txt` test target `target_sources`. All existing tests (PatchModel, MasterModel, SysExEncoder, SysExParser, MidiManager) pass unchanged.

## Tasks / Subtasks

- [x] **Implement `ApvtsPatchMapper`** (AC: #1, #2, #3, #5)
  - [x] Create `Source/Core/Models/ApvtsPatchMapper.h` — constructor, `apvtsToBuffer()`, `bufferToApvts()`, private members `apvts_`, `model_`, `intDescriptors_`, `choiceDescriptors_`
  - [x] Create `Source/Core/Models/ApvtsPatchMapper.cpp` — `buildIntDescriptors()` and `buildChoiceDescriptors()` aggregate all 10 `PatchEditSection::*Module` vectors (mirrors `ApvtsFactory::getAllIntParameters` for PATCH only); implement the four private sync helpers; no `PluginDescriptorsMasterEdit` or matrix mod descriptors included
  - [x] Verify: `apvtsToBuffer()` body ≤ 20 lines (orchestration), each helper ≤ 15 lines, class ≤ 200 lines, no magic numbers
- [x] **Wire into `PluginProcessor`** (AC: #4)
  - [x] Add `patchModel_` (`unique_ptr<Core::PatchModel>`) and `apvtsPatchMapper_` (`unique_ptr<Core::ApvtsPatchMapper>`) to `PluginProcessor` private section (after `midiManager_` in header)
  - [x] Add public getters `Core::PatchModel& getPatchModel()` and `Core::ApvtsPatchMapper& getApvtsPatchMapper()`
  - [x] Add `std::unordered_set<juce::String> patchParameterIds_` private member; populate via new `buildPatchParameterIdSet()` private method called from constructor
  - [x] Extend initializer list: `patchModel_ { std::make_unique<Core::PatchModel>() }`, then `apvtsPatchMapper_ { std::make_unique<Core::ApvtsPatchMapper>(apvts, *patchModel_) }` — model constructed before mapper
  - [x] In `valueTreePropertyChanged`: after `resolveParameterIdFromTree`, check `patchParameterIds_.count(parameterId) > 0` and call `apvtsPatchMapper_->apvtsToBuffer()` — existing logging and bank/patch number handlers unchanged
- [x] **Write `Tests/Unit/ApvtsPatchMapperTests.cpp`** (AC: #6)
  - [x] `TestAudioProcessor` minimal subclass (all pure-virtual overrides, APVTS member, layout from a flat set of synthetic patch parameters)
  - [x] Test A `apvtsToBuffer`: register 2 synthetic int + 1 choice param; set raw values; call mapper; assert `model.getValue/getChoiceIndex` return expected values
  - [x] Test B `bufferToApvts`: fill model bytes at descriptor offsets; call mapper; assert `*apvts.getRawParameterValue(id)` returns expected denormalized float
  - [x] Test C `referenceRoundTrip`: load `Tests/Fixtures/Patches/Patch 71.syx`, decode to 134-byte buffer, load into model; register synthetic descriptors for 3–4 known offsets (pick byte indices whose values are non-zero in the fixture — verify by `decoded[offset]` before registering); call `bufferToApvts`; zero the model; call `apvtsToBuffer`; assert model bytes at those offsets equal original decoded bytes
- [x] **Update `CMakeLists.txt`** (AC: #7)
  - [x] Add `Source/Core/Models/ApvtsPatchMapper.cpp` to `Matrix-Control_Tests` target_sources
  - [x] Add `Tests/Unit/ApvtsPatchMapperTests.cpp` to `Matrix-Control_Tests` target_sources
  - [x] Rebuild and confirm zero new warnings; all test suites pass

## Dev Notes

### What `ApvtsPatchMapper` IS — and must NOT do

The mapper is a **thin value-sync bridge** between the APVTS float representation and the PatchModel's 134-byte buffer. It reads/writes values using `descriptor.sysExOffset`, `minValue`, `maxValue`, and `choices.size()`. Nothing else.

**Hard constraints:**
- No SysEx, no MIDI, no queue — those arrive in Story 2.4
- No GUI headers (`juce_gui_basics`, `juce_gui_extra`)
- Not a listener — the mapper does not register itself as an APVTS listener; `PluginProcessor::valueTreePropertyChanged` is the trigger
- Patch name bytes 0–7 are not covered — the descriptor vectors contain no name offset; `PatchModel::setName/getName` remain Story 1.5 territory
- No hardcoded byte indices — all offsets come from `descriptor.sysExOffset`; any hardcoded offset is an AC #5 violation

### Descriptor iteration — critical pattern

`PatchEditSection` has **no section-level aggregate** `kIntParameters` / `kChoiceParameters` (unlike `MasterEditSection` which has section-level aggregates at `PluginDescriptors.h:129–131`). The mapper must iterate **per module**, exactly mirroring `ApvtsFactory::getAllIntParameters()` / `getAllChoiceParameters()` lines 21–30 and 48–57:

```
Dco1Module, Dco2Module, VcfVcaModule, FmTrackModule, RampPortamentoModule,
Envelope1Module, Envelope2Module, Envelope3Module, Lfo1Module, Lfo2Module
```

Do NOT include `MasterEditSection` or `MatrixModulationSection` — those belong to `ApvtsMasterMapper` (Story 1.4) and the matrix mod SysEx path (Story 2.6).

The 10-module aggregation call is an orchestration method — CONVENTIONS §3.1 allows up to 20 lines for these.

### APVTS raw value conventions

`apvts_.getRawParameterValue(id)` returns `std::atomic<float>*` pointing to the **denormalized** parameter value. For `AudioParameterInt` (range `[min, max]`), this is the integer as float (e.g., `42.0f` for value 42 — NOT normalized [0,1]).

- **APVTS → model (int):** `juce::roundToInt(*rawValue)` → pass to `PatchModel::setValue(descriptor, value)`
- **APVTS → model (choice):** `juce::roundToInt(*rawValue)` → pass to `PatchModel::setChoiceIndex(descriptor, index)`
- **Model → APVTS (int):** `param->convertTo0to1(static_cast<float>(model_.getValue(d)))` → `param->setValueNotifyingHost(normalised)`
- **Model → APVTS (choice):** `static_cast<float>(model_.getChoiceIndex(d))` → normalize via `convertTo0to1` → `setValueNotifyingHost`

`apvts_.getParameter(id)` returns `juce::RangedAudioParameter*` (base class of `AudioParameterInt` and `AudioParameterChoice`) — no `dynamic_cast` needed for `convertTo0to1` or `setValueNotifyingHost`.

### Threading invariants

- `apvtsToBuffer()` reads atomics (`getRawParameterValue`) and writes to `PatchModel`. For Story 1.3, it is called from `valueTreePropertyChanged` which runs on the message thread. Do not add audio-thread or lock-free machinery now — that belongs to a future threading hardening story.
- `bufferToApvts()` calls `setValueNotifyingHost` which is **message-thread-only** per JUCE contract. The method header must carry a brief comment: `// Message thread only — see JUCE AudioProcessor::setValueNotifyingHost.` Future callers from the MIDI thread (Story 2.4 / device SysEx ingest) must wrap this in `MessageManager::callAsync`.

### `PluginProcessor` integration

**Constructor initializer list order** (order matters — model must exist before mapper):
```
: AudioProcessor(...)
, apvts(...)
, midiManager(...)
, patchModel_{ std::make_unique<Core::PatchModel>() }
, apvtsPatchMapper_{ std::make_unique<Core::ApvtsPatchMapper>(apvts, *patchModel_) }
```

**`buildPatchParameterIdSet()`** — private method, called from the constructor body after `buildChoiceParameterMap()`. Iterates the same 10 `PatchEditSection::*Module::kIntParameters` and `kChoiceParameters` and inserts each `descriptor.parameterId` into `patchParameterIds_`. Use `std::unordered_set<juce::String>` for O(1) lookup in `valueTreePropertyChanged`.

**`valueTreePropertyChanged` update** — add this block after `resolveParameterIdFromTree` and before (or after) the existing `handleBankNumberChange` / `handlePatchNumberChange` calls:

```cpp
if (patchParameterIds_.count(parameterId) > 0)
    apvtsPatchMapper_->apvtsToBuffer();
```

Existing logging, bank-number, and patch-number handlers are unchanged.

**Includes to add to `PluginProcessor.cpp`:**
```cpp
#include "Core/Models/PatchModel.h"
#include "Core/Models/ApvtsPatchMapper.h"
```
Forward declarations in `PluginProcessor.h` are enough — `Core::PatchModel` and `Core::ApvtsPatchMapper` can be forward-declared there since they are stored as `unique_ptr`.

### Test harness — minimal `TestAudioProcessor`

The test target does NOT include `PluginProcessor.cpp` (it pulls in GUI headers). Create a minimal inner class in `ApvtsPatchMapperTests.cpp`:

```cpp
class TestAudioProcessor : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", std::move(layout))
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    // Required AudioProcessor overrides (all trivial):
    const juce::String getName() const override { return "Test"; }
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};
```

This is standard JUCE test boilerplate. Register synthetic `AudioParameterInt` and `AudioParameterChoice` in the layout to match your synthetic descriptors.

### Test C — reference round-trip

The existing `Tests/Fixtures/Patches/Patch 71.syx` (committed in Story 1.1) decodes to a 134-byte buffer with non-zero values at many offsets. To pick synthetic descriptor offsets:
1. Decode `Patch 71.syx` at the start of the test using `SysExDecoder::decodePatchSysEx`
2. Pick 3–4 byte indices **after offset 7** (i.e., non-name bytes) where `decoded[i] != 0`
3. Use those indices as `sysExOffset` in synthetic int descriptors (range `[0, 63]` or `[0, 127]` — choose a max that is ≥ `decoded[i]`)
4. Register those params in the `TestAudioProcessor` layout
5. Run the round-trip and assert that bytes at those offsets survive intact

This avoids linking `PluginDescriptorsPatchEdit.cpp` into the test binary while still providing a hardware-grounded test.

### File placement and CMake

- New files: `Source/Core/Models/ApvtsPatchMapper.h`, `Source/Core/Models/ApvtsPatchMapper.cpp`
- Test file: `Tests/Unit/ApvtsPatchMapperTests.cpp`
- `CMakeLists.txt`: add both `.cpp` files to `Matrix-Control_Tests` `target_sources` (around lines 227–234)
- The project does **not** glob sources — every new file must be listed explicitly

`Core ↛ GUI` rule applies: `ApvtsPatchMapper.h` must include only `juce_audio_processors`, `juce_core`, and project model/descriptor headers.

### Clean Code self-review before submitting

- [ ] `ApvtsPatchMapper`: class ≤ 200 lines, all methods ≤ 15 lines (orchestration `apvtsToBuffer`/`bufferToApvts` may reach 20)
- [ ] No method has > 3 parameters
- [ ] No magic numbers — all values come from descriptors or named JUCE APIs
- [ ] `patchParameterIds_` named to reveal intent (a set of known patch parameter IDs)
- [ ] Helpers `syncIntToBuffer`, `syncChoiceToBuffer`, `pushIntToApvts`, `pushChoiceToApvts` — each ≤ 5 lines
- [ ] `TestAudioProcessor` boilerplate stays inside the test `.cpp` (never leaks into production)

### Cross-story context

- **Story 1.4** (`ApvtsMasterMapper`) mirrors this story for `MasterModel`. The mapper class design established here becomes the template.
- **Story 1.5** (patch name sync) adds name-byte handling to `PluginProcessor` — it does NOT touch the mapper; name is stored differently from parameterized descriptors.
- **Story 2.4** (`APVTS PATCH parameter → SysEx 0x06`) adds SysEx dispatch on top of the existing `valueTreePropertyChanged` → `apvtsToBuffer` flow. The mapper itself is unchanged.
- Future `bufferToApvts()` call sites from the MIDI thread (device SysEx ingest) must use `MessageManager::callAsync` — do NOT add that now; document a thread-safety note in the method header as a pointer.

### References

- [Source: epics.md#Story-1.3 (line 411–422)]
- [Source: architecture.md#AD-2 (lines 107–128)] — `ApvtsPatchMapper` in PluginProcessor composition root
- [Source: architecture.md#Pattern-Examples (lines 369–384)] — `ApvtsPatchMapper::writeParameter(paramId, value)` example
- [Source: Source/Core/Factories/ApvtsFactory.cpp:12–64] — authoritative per-module iteration order for PATCH
- [Source: Source/Shared/Definitions/PluginDescriptors.h:134–205] — `PatchEditSection` per-module namespace layout (no section-level aggregates)
- [Source: Source/Core/Models/PatchModel.h / PatchModel.cpp] — model API to reuse
- [Source: Source/Core/Models/MasterModel.h / MasterModel.cpp] — symmetric twin for reference
- [Source: Source/Core/PluginProcessor.h / PluginProcessor.cpp] — files to modify; `buildChoiceParameterMap()` pattern to mirror for `buildPatchParameterIdSet()`
- [Source: Tests/Unit/MasterModelTests.cpp] — test style, `TestMain.cpp`, static test instance registration
- [Source: Tests/Unit/PatchModelTests.cpp] — synthetic descriptor pattern
- [Source: CMakeLists.txt:216–254] — test target wiring
- [Source: project-context.md#Architecture-Invariants, #Clean-Code-Limits]
- [Source: _bmad-output/implementation-artifacts/1-2-mastermodel-packed-buffer.md] — prior story learnings

## Dev Agent Record

### Agent Model Used

claude-sonnet-4-6

### Debug Log References

- `juce_Timer.cpp:376 / :99` assertions fire during `TestAudioProcessor` destruction in the no-message-manager test context — benign, values sync correctly via atomics. Pre-existing limitation of the headless test binary.

### Completion Notes List

- Implemented `ApvtsPatchMapper` with 2-param constructor (apvts + model), `buildIntDescriptors()` / `buildChoiceDescriptors()` public static methods aggregating all 10 `PatchEditSection::*Module` vectors, and four ≤5-line sync helpers.
- Added `PluginDescriptorsPatchEdit.cpp`, `PluginDescriptors.cpp`, `PluginHelpers.cpp` to the test binary (required by the mapper's static builders). Tests A/B use real param IDs from `buildIntDescriptors()` with made-up (synthetic) values; test C uses the adaptive round-trip approach from the Dev Notes.
- Added `ApvtsPatchMapper.cpp`, `PatchModel.cpp`, `PackedFieldCodec.cpp` to `PLUGIN_SOURCES` (models were previously only in the test binary; now required by the production plugin since `PluginProcessor.cpp` instantiates them).
- `buildPatchParameterIdSet()` mirrors `buildChoiceParameterMap()` pattern; called after it in the constructor body.
- All 24 tests pass (3 new ApvtsPatchMapper + 21 existing); VST3 plugin builds and deploys cleanly.

### File List

- Source/Core/Models/ApvtsPatchMapper.h (new)
- Source/Core/Models/ApvtsPatchMapper.cpp (new)
- Source/Core/PluginProcessor.h (modified)
- Source/Core/PluginProcessor.cpp (modified)
- Tests/Unit/ApvtsPatchMapperTests.cpp (new)
- CMakeLists.txt (modified)

## Change Log

- 2026-06-03: Implemented `Core::ApvtsPatchMapper` (AC #1-3, #5); wired into `PluginProcessor` with `valueTreePropertyChanged` trigger (AC #4); added `ApvtsPatchMapperTests` covering APVTS→buffer, buffer→APVTS, and reference round-trip with Patch 71.syx (AC #6); updated CMakeLists.txt for both test and plugin targets (AC #7). All 24 tests pass.
