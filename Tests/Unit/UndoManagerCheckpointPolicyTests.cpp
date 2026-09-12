#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchMutator/MutationPolicy.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/PatchFileService.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginIDs.h"

#include "PatchManagerActionHandlerTestSupport.h"

namespace
{

using namespace PluginDescriptors::PatchEditSection;

const PluginDescriptors::IntParameterDescriptor& firstPatchIntDescriptor()
{
    jassert(! Dco1Module::kIntParameters.empty());
    return Dco1Module::kIntParameters.front();
}

class EditorialCheckpointAudioProcessor : public juce::AudioProcessor
{
public:
    EditorialCheckpointAudioProcessor()
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, &undoManager, "P", ApvtsFactory::createParameterLayout())
    {
    }

    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    void establishEditorialCheckpoint(Core::DirtyPatchTracker& tracker,
                                      Core::ApvtsPatchMapper& mapper,
                                      Core::PatchNameSyncer& nameSyncer,
                                      Core::PatchModel& model)
    {
        undoManager.clearUndoHistory();
        mapper.apvtsToBuffer();
        nameSyncer.apvtsToBuffer();
        model.normalizeNameEncoding();
        tracker.captureSnapshot(model);
    }

    const juce::String getName() const override { return "EditorialCheckpointHarness"; }
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

struct MutateCheckpointHarness
{
    EditorialCheckpointAudioProcessor proc;
    Core::PatchModel model;
    Core::ApvtsPatchMapper mapper;
    Core::PatchNameSyncer patchNameSyncer;
    Core::DirtyPatchTracker dirtyPatchTracker;
    Core::MidiOutboundQueue queue;
    Core::MidiActivityTracker tracker;
    MidiManager midiManager;
    SysExParser parser;
    SysExDecoder decoder;
    Core::PatchFileService patchFileService;
    bool suppressPatchSysEx { false };
    bool suppressMatrixModSysEx { false };
    Core::PatchMutatorEngine engine;

    MutateCheckpointHarness()
        : mapper(proc.apvts, model)
        , patchNameSyncer(proc.apvts, model)
        , midiManager(proc.apvts, queue, tracker)
        , decoder(parser)
        , patchFileService(decoder)
        , engine(Core::PatchMutatorEngine::Dependencies {
                     &model,
                     &mapper,
                     &patchNameSyncer,
                     &midiManager,
                     proc.apvts,
                     []() { return 0; },
                     []() { return Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000); },
                     &patchFileService,
                     &midiManager.getSysExEncoder() },
                 Core::ActionExecutionHooks {
                     .setSuppressMatrixModSysEx = [this](bool s) { suppressMatrixModSysEx = s; },
                     .setSuppressPatchSysEx = [this](bool s) { suppressPatchSysEx = s; },
                     .onEditorialCheckpoint = [this]()
                     {
                         proc.establishEditorialCheckpoint(dirtyPatchTracker, mapper, patchNameSyncer, model);
                     } })
    {
        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty("deviceType", "Matrix-1000", nullptr);
        model.loadFrom(Core::InitDefaults::patchData());
        proc.establishEditorialCheckpoint(dirtyPatchTracker, mapper, patchNameSyncer, model);
    }

    void setMutateRecipe()
    {
        namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

        // Amount / Random no longer reach the algorithm — MODE owns the curve.
        // WILD + FREE matches PatchMutatorEngineTestSupport (avoids Drift no-op flakes).
        proc.apvts.state.setProperty(PatchMutator::kMode,
                                      static_cast<int>(Core::MutationMode::kWild),
                                      nullptr);
        proc.apvts.state.setProperty(PatchMutator::kPitch,
                                      static_cast<int>(Core::MutationPitchMode::kFree),
                                      nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableDco1, true, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableDco2, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableVcfVca, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableFmTrack, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableRampPortamento, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope1, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope2, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope3, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableLfo1, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableLfo2, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableMatrixMod, false, nullptr);
    }

    void writeProbeParam(int value)
    {
        const auto& descriptor = firstPatchIntDescriptor();
        // getParameterAsValue stores the denormalised parameter domain (same as getRawParameterValue).
        proc.apvts.getParameterAsValue(descriptor.parameterId).setValue(static_cast<float>(value));
    }
};

class UndoManagerCheckpointPolicyTests : public juce::UnitTest
{
public:
    UndoManagerCheckpointPolicyTests() : juce::UnitTest("UndoManagerCheckpointPolicy") {}

    void runTest() override
    {
        editorialCheckpoint_clearsUndoAfterRecordedEdit();
        fullPaste_clearsUndoAndRefreshesSnapshot();
        mutate_clearsUndoAndRefreshesSnapshotWhileRetainingMutatorHistory();
        mutate_retry_clearsUndoStack();
    }

private:
    void editorialCheckpoint_clearsUndoAfterRecordedEdit()
    {
        beginTest("establishEditorialCheckpoint seam clears undo stack after a recorded edit");

        using namespace PatchManagerActionHandlerTestSupport;

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);
        harness.model.loadFrom(Core::InitDefaults::patchData());
        harness.mapper.bufferToApvts();
        harness.proc.undoManager.clearUndoHistory();
        harness.handler.syncDirtySnapshotFromApvts();

        const auto& descriptor = firstPatchIntDescriptor();
        harness.proc.apvts.getParameterAsValue(descriptor.parameterId).setValue(
            static_cast<float>(descriptor.minValue + 5));

        expect(harness.proc.undoManager.canUndo());

        harness.triggerEditorialCheckpoint();

        expect(! harness.proc.undoManager.canUndo());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(harness.mapper,
                                                               harness.patchNameSyncer,
                                                               harness.model));
    }

    void fullPaste_clearsUndoAndRefreshesSnapshot()
    {
        beginTest("full patch paste clears editorial undo and refreshes dirty snapshot");

        using namespace PatchManagerActionHandlerTestSupport;
        namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.model.loadFrom(Core::InitDefaults::patchData());
        harness.mapper.bufferToApvts();
        harness.proc.undoManager.clearUndoHistory();
        harness.handler.syncDirtySnapshotFromApvts();

        const auto& descriptor = firstPatchIntDescriptor();
        harness.proc.apvts.getParameterAsValue(descriptor.parameterId).setValue(
            static_cast<float>(descriptor.minValue + 5));

        expect(harness.proc.undoManager.canUndo());

        Core::PatchModel clipboardModel;
        clipboardModel.loadFrom(Core::InitDefaults::patchData());
        harness.clipboard.copyFullPatch(clipboardModel, "BANK 0 / PATCH 0");

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        expect(! harness.proc.undoManager.canUndo());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(harness.mapper,
                                                               harness.patchNameSyncer,
                                                               harness.model));
    }

    void mutate_clearsUndoAndRefreshesSnapshotWhileRetainingMutatorHistory()
    {
        beginTest("MUTATE clears editorial undo and refreshes snapshot while retaining mutator history");

        MutateCheckpointHarness harness;
        harness.setMutateRecipe();
        harness.writeProbeParam(12);
        expect(harness.proc.undoManager.canUndo());

        const auto result = harness.engine.mutate();
        expect(result.success, "MUTATE should succeed with WILD/FREE + DCO1 scope");
        expect(! harness.proc.undoManager.canUndo());
        expect(harness.engine.rootCount() >= 1);
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(harness.mapper,
                                                               harness.patchNameSyncer,
                                                               harness.model));
    }

    void mutate_retry_clearsUndoStack()
    {
        beginTest("RETRY clears editorial undo stack after checkpoint");

        MutateCheckpointHarness harness;
        harness.setMutateRecipe();

        expect(harness.engine.mutate().success, "seed MUTATE should succeed with WILD/FREE + DCO1 scope");
        harness.writeProbeParam(14);
        expect(harness.proc.undoManager.canUndo());

        expect(harness.engine.retry().success, "RETRY should succeed after a seeded MUTATE root");
        expect(! harness.proc.undoManager.canUndo());
    }
};

static UndoManagerCheckpointPolicyTests undoManagerCheckpointPolicyTests;

} // namespace
