#pragma once

#include <cstring>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Factories/ApvtsFactory.h"
#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Core/Services/PatchMutator/PatchLoadContext.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
namespace MutatorDisplayNames = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

// Friend accessor for private engine state/APIs used by unit tests.
class PatchMutatorEngineTestAccess
{
public:
    static Core::MutationHistoryStore& historyStore(Core::PatchMutatorEngine& engine);
    static void applySelectionFromApvts(Core::PatchMutatorEngine& engine);
    static Core::PatchModel resolveAuditionBuffer(Core::PatchMutatorEngine& engine);
};

namespace PatchMutatorEngineTestSupport
{
    class TestAudioProcessorMutator : public juce::AudioProcessor
    {
    public:
        TestAudioProcessorMutator();

        juce::AudioProcessorValueTreeState apvts;

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

    struct EngineHarness
    {
        TestAudioProcessorMutator proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::PatchNameSyncer patchNameSyncer;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager;
        SysExParser parser;
        SysExDecoder decoder;
        Core::PatchFileService patchFileService;
        bool suppressPatchSysEx { false };
        bool suppressMatrixModSysEx { false };
        int currentPatchNumber { 0 };
        Core::DeviceMemoryLimits deviceLimits {
            Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000)
        };
        Core::PatchMutatorEngine engine;

        EngineHarness();

        void setRecipe(int amount, int random, bool enableDco1 = true);
        void setMode(Core::MutationMode mode, Core::MutationPitchMode pitchMode);
        Core::MutationHistoryStore& store();
        void applySelectionFromApvts();
        Core::PatchModel resolveAuditionBuffer();
    };

    struct FullPatchOpcodeCounts
    {
        int slotWrite = 0;       // 0x01
        int editBuffer = 0;      // 0x0D
        int remoteParamEdit = 0; // 0x06
        int matrixModEdit = 0;   // 0x0B
    };

    struct ActionEnabledExpectations
    {
        bool mutate = false;
        bool retry = false;
        bool exportEnabled = false;
        bool deleteEnabled = false;
        bool clear = false;
        bool mutateAllocationBlocked = false;
        bool retryAllocationBlocked = false;
        bool mutateDefragRecovery = false;
        bool retryDefragRecovery = false;
    };

    int countPatchSysExMessages(Core::MidiOutboundQueue& queue);
    FullPatchOpcodeCounts countFullPatchOpcodes(Core::MidiOutboundQueue& queue);
    Core::PatchModel makeDistinctBuffer(int seed);
    juce::File makeTempExportDir();
    void expectActionEnabledMirrors(juce::UnitTest& test,
                                    const EngineHarness& harness,
                                    ActionEnabledExpectations expected);
}
