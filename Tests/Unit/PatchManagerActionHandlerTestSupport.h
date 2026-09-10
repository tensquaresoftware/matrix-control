#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Factories/ApvtsFactory.h"
#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/PatchInitService.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DeviceMemoryLimits.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchManager = PluginIDs::PatchManagerSection;
namespace InternalPatches = PatchManager::InternalPatchesModule::StandaloneWidgets;
namespace BankUtility = PatchManager::BankUtilityModule;
namespace MutatorState = PatchManager::PatchMutatorModule::StateProperties;
namespace ComputerPatches = PatchManager::ComputerPatchesModule;
namespace PatchNameIds = PluginIDs::PatchEditSection::PatchNameModule;
namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;
namespace Policy = PluginIDs::Settings::ComputerPatchesNamesPolicy;

namespace PatchManagerActionHandlerTestSupport
{
    class TestAudioProcessorPatchManager : public juce::AudioProcessor
    {
    public:
        TestAudioProcessorPatchManager()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, &undoManager, "P", ApvtsFactory::createParameterLayout())
        {
        }

        juce::UndoManager undoManager;
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

    struct QueueScanResult
    {
        bool setBank = false;
        bool unlockBank = false;
        bool patchData = false;
        bool editBufferPatch = false;
        int setBankValue = -1;
        int patchNumber = -1;
        int patchSysExCount = 0;
        int programChangeCount = 0;
        int lastProgramChange = -1;
    };

    QueueScanResult scanQueue(Core::MidiOutboundQueue& queue);

    juce::File fixturesPatchesDir();
    juce::File createTempScanDir();
    void copyFixturePatchToDir(const juce::File& dir, const juce::String& fileName);
    // Most tests start from a synth whose slot is already known; pass false to exercise the
    // "coordinates still undefined" startup path.
    void initializePatchManagerState(juce::ValueTree& state,
                                     int bank,
                                     int patch,
                                     bool coordinatesEstablished = true);

    struct HandlerHarness
    {
        struct PatchLoadHookState
        {
            bool invoked = false;
        };

        struct GateState
        {
            bool allow = true;
            int calls = 0;
            bool lastIncludeUnsavedEditWarning = false;
        };

        struct DumpFakeState
        {
            // Prefer useSuccessfulDeviceDump() in new tests. Failure paths set available = false.
            bool available = true;
            bool deferCallback = false;
            std::vector<juce::uint8> response;
            Core::ActionExecutionHooks::DeviceDumpCallback pendingCallback;
            juce::uint8 lastRequestedPatch = 0;
            int requestCount = 0;
        };

        struct NameRequiredBeforeStoreHookState
        {
            int calls = 0;
        };

        std::shared_ptr<PatchLoadHookState> patchLoadHookState;
        std::shared_ptr<GateState> gateState;
        std::shared_ptr<DumpFakeState> dumpFakeState;
        std::shared_ptr<NameRequiredBeforeStoreHookState> nameRequiredBeforeStoreHookState;
        TestAudioProcessorPatchManager proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::ClipboardService clipboard;
        Core::DirtyPatchTracker dirtyPatchTracker;
        SysExParser parser;
        SysExDecoder decoder;
        Core::InitTemplateLoader initLoader;
        Core::PatchInitService patchInitService;
        juce::File initTemplatesFolder;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiManager midiManager;
        Core::PatchSelectionMidiSync patchSelectionMidiSync;
        Core::DeviceMemoryLimits limits;
        Core::PatchFileService patchFileService;
        Core::PatchNameSyncer patchNameSyncer;
        SysExEncoder sysExEncoder;
        std::function<juce::File()> pickFolderCallback { []() { return juce::File(); } };
        std::function<juce::File(juce::File, juce::String)> pickSaveFileCallback {
            [](juce::File, juce::String) { return juce::File(); }
        };
        Core::PatchFileNameReconciler::Picker pickReconciliationCallback;
        bool suppressPatchSysEx { false };
        bool suppressMatrixModSysEx { false };
        Core::PatchManagerActionHandler handler;

        explicit HandlerHarness(Core::DeviceMemoryLimits limitsIn);

        void fireDeferredDump();
        void useSuccessfulDeviceDump();
        void triggerEditorialCheckpoint();

    private:
        Core::PatchManagerActionHandler::Dependencies makeDependencies();
        Core::ActionExecutionHooks makeHooks();
        void applyInitialHarnessState();
    };

    void simulateSelectPatchFileDispatch(HandlerHarness& harness);
    void fireAdjacentNavigation(HandlerHarness& harness, const juce::String& adjacentPropertyId);
    void fireInternalPatchNavigation(HandlerHarness& harness, const juce::String& navigationPropertyId);
    // When midFooterBeforeLoad is non-null, OPEN must take the deferred-select path;
    // the optional is set to the pre-load footer (often empty — that is no-flash success).
    void fireOpenAndDispatchLoad(HandlerHarness& harness,
                                 std::optional<juce::String>* midFooterBeforeLoad = nullptr);
    void setupComputerPatchesScan(HandlerHarness& harness, const juce::File& tempDir);

    // True when text is the combined first-load footer for that scan + Loaded (any recon variant).
    bool matchesCombinedFirstLoadFooter(const juce::String& text,
                                        int validCount,
                                        int invalidCount,
                                        const juce::File& file);

    // Mirrors production ActionDispatcher: SelectPatchFile property changes schedule a 150 ms load settle.
    struct SelectPatchFileLoadDispatcher : private juce::ValueTree::Listener
    {
        HandlerHarness& harness;
        bool armed = true;

        explicit SelectPatchFileLoadDispatcher(HandlerHarness& harnessIn);
        ~SelectPatchFileLoadDispatcher() override;

        void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;
    };
}
