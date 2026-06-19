#include <cstring>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/ModuleActionHandler.h"
#include "Core/Init/PatchModuleInitService.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/ClipboardService.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

class TestAudioProcessorModuleActions : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessorModuleActions(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", std::move(layout))
    {
    }

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

class ModuleActionHandlerTests : public juce::UnitTest
{
public:
    ModuleActionHandlerTests() : juce::UnitTest("ModuleActionHandler") {}

    void runTest() override
    {
        testModulePasteCompatibleModule();
        testModulePasteIncompatibleNoOp();
        testMatrixModPasteTenBuses();
        testHandleActionNoDoubleDispatchForPatchInit();
        testPatchInitDoesNotTriggerMasterInit();
    }

private:
    struct HandlerHarness
    {
        TestAudioProcessorModuleActions proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        Core::ClipboardService clipboard;
        Core::PatchParameterSysExDispatcher patchDispatcher;
        Core::MatrixModBusParameterSysExDispatcher matrixModDispatcher;
        Core::ModuleActionHandler handler;
        bool suppressPatchSysEx { false };
        bool suppressMatrixModSysEx { false };
        int patchDispatchCount { 0 };
        int matrixModDispatchCount { 0 };
        std::vector<int> matrixModDispatchedBuses;

        explicit HandlerHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout)
            : proc(std::move(layout))
            , mapper(proc.apvts, model)
            , patchDispatcher(model,
                              [this](int, juce::uint8) { ++patchDispatchCount; })
            , matrixModDispatcher(model,
                                  [this](juce::uint8 bus, juce::uint8, juce::uint8, juce::uint8)
                                  {
                                      ++matrixModDispatchCount;
                                      matrixModDispatchedBuses.push_back(static_cast<int>(bus));
                                  })
            , handler(proc.apvts,
                      &model,
                      &mapper,
                      &clipboard,
                      nullptr,
                      nullptr,
                      nullptr,
                      &patchDispatcher,
                      &matrixModDispatcher,
                      nullptr,
                      Core::ActionExecutionHooks{
                          [this](bool suppress) { suppressMatrixModSysEx = suppress; },
                          nullptr,
                          [this](bool suppress) { suppressPatchSysEx = suppress; },
                          nullptr })
        {
        }
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout makeEnvelopePasteLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (const auto& d : PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.minValue, d.maxValue, d.defaultValue));
        }

        for (const auto& d : PluginDescriptors::PatchEditSection::Envelope1Module::kChoiceParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.choices, d.defaultIndex));
        }

        return layout;
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeMatrixModLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            using namespace PluginDescriptors::MatrixModulationSection;

            const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(bus)];
            const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(bus)];

            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(intParams[0].parameterId, 1), intParams[0].displayName,
                intParams[0].minValue, intParams[0].maxValue, intParams[0].defaultValue));

            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(choiceParams[0].parameterId, 1), choiceParams[0].displayName,
                choiceParams[0].choices, choiceParams[0].defaultIndex));

            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(choiceParams[1].parameterId, 1), choiceParams[1].displayName,
                choiceParams[1].choices, choiceParams[1].defaultIndex));
        }

        return layout;
    }

    static int expectedSysExCountForModule(const juce::String& moduleGroupId)
    {
        int count = 0;

        for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
        {
            if (d.parentGroupId == moduleGroupId && d.sysExId != PluginDescriptors::kNoSysExId)
                ++count;
        }

        for (const auto& d : Core::ApvtsPatchMapper::buildChoiceDescriptors())
        {
            if (d.parentGroupId == moduleGroupId && d.sysExId != PluginDescriptors::kNoSysExId)
                ++count;
        }

        return count;
    }

    void testModulePasteCompatibleModule()
    {
        beginTest("module paste → model updated, APVTS push, dispatchModule invoked");

        HandlerHarness harness(makeEnvelopePasteLayout());

        using namespace PluginDescriptors::PatchEditSection;
        namespace PatchEditIds = PluginIDs::PatchEditSection;

        for (const auto& d : Envelope1Module::kIntParameters)
            harness.model.setValue(d, d.maxValue);

        harness.clipboard.copyModule(Core::PatchModuleKind::Env1, harness.model);

        for (const auto& d : Envelope1Module::kIntParameters)
            harness.model.setValue(d, d.minValue);

        harness.handler.handleAction(PatchEditIds::Envelope1Module::StandaloneWidgets::kPaste, juce::var());

        for (const auto& d : Envelope1Module::kIntParameters)
            expectEquals(harness.model.getValue(d), d.maxValue);

        expectEquals(harness.patchDispatchCount,
                     expectedSysExCountForModule(PatchEditIds::Envelope1Module::kGroupId));
    }

    void testModulePasteIncompatibleNoOp()
    {
        beginTest("module paste skipped when canPasteModule is false");

        HandlerHarness harness(makeEnvelopePasteLayout());

        const auto before = harness.model;
        harness.handler.handleAction(PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
                                     juce::var());

        expect(std::memcmp(before.data(), harness.model.data(), Core::PatchModel::kBufferSize) == 0);
        expectEquals(harness.patchDispatchCount, 0);
    }

    void testMatrixModPasteTenBuses()
    {
        beginTest("matrix mod paste → 10 dispatchBus calls in ascending order");

        HandlerHarness harness(makeMatrixModLayout());

        using namespace PluginDescriptors::MatrixModulationSection;

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(bus)];
            const auto& choiceParams = kModulationBusChoiceParameters[static_cast<size_t>(bus)];

            harness.model.setValue(intParams[0], static_cast<int>(bus + 1));
            harness.model.setChoiceIndex(choiceParams[0], 1);
            harness.model.setChoiceIndex(choiceParams[1], 2);
        }

        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyMatrixModulation(harness.model);

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            const auto& intParams = kModulationBusIntParameters[static_cast<size_t>(bus)];
            harness.model.setValue(intParams[0], 0);
        }

        harness.handler.handleAction(PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationPaste,
                                     juce::var());

        expectEquals(harness.matrixModDispatchCount, Matrix1000Limits::kModulationBusCount);

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
            expectEquals(harness.matrixModDispatchedBuses[static_cast<size_t>(bus)], bus);
    }

    void testHandleActionNoDoubleDispatchForPatchInit()
    {
        beginTest("paste property id does not trigger copy handler");

        HandlerHarness harness(makeEnvelopePasteLayout());

        harness.handler.handleAction(PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
                                     juce::var());

        expect(harness.clipboard.getMode() == Core::ClipboardMode::Empty);
        expectEquals(harness.patchDispatchCount, 0);
    }

    void testPatchInitDoesNotTriggerMasterInit()
    {
        beginTest("patch init property id is not consumed by master init handler alone");

        HandlerHarness harness(makeEnvelopePasteLayout());
        harness.handler.handleAction(PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
                                     juce::var());
        expectEquals(harness.patchDispatchCount, 0);
    }
};

static ModuleActionHandlerTests moduleActionHandlerTests;
