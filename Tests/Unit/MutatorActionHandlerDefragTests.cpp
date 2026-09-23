#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/MutatorActionHandler.h"
#include "Core/Factories/ApvtsFactory.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

namespace
{
    class TestAudioProcessor : public juce::AudioProcessor
    {
    public:
        TestAudioProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", ApvtsFactory::createParameterLayout())
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

    class RecordingEngine final : public Core::PatchMutatorEnginePort
    {
    public:
        int defragCallCount = 0;
        Core::MutatorActionResult mutateResult;
        Core::MutatorActionResult defragResult;

        Core::MutatorActionResult mutate() override { return mutateResult; }
        Core::MutatorActionResult retry() override { return {}; }
        Core::MutatorActionResult toggleCompare() override { return {}; }
        Core::MutatorActionResult deleteSelected() override { return {}; }
        Core::MutatorActionResult clearHistory() override { return {}; }
        Core::MutatorActionResult exportHistory(const juce::File&) override { return {}; }
        Core::MutatorActionResult exportHistoryResolved(const juce::File&,
                                                        Core::ExportCollisionResolution) override
        {
            return {};
        }

        Core::MutatorActionResult defragHistory() override
        {
            ++defragCallCount;
            return defragResult;
        }

        void auditionSelectedHistoryEntry() override {}
        void rebuildHistoryListMirrors() override {}
        void advanceHistorySelection(bool) override {}
    };
}

class MutatorActionHandlerDefragTests : public juce::UnitTest
{
public:
    MutatorActionHandlerDefragTests() : juce::UnitTest("MutatorActionHandlerDefrag") {}

    void runTest() override
    {
        mutate_limitDefragConfirm_publishesFooter();
    }

private:
    void mutate_limitDefragConfirm_publishesFooter()
    {
        beginTest("mutate_limitDefragConfirm_publishesFooter");

        TestAudioProcessor proc;
        RecordingEngine engine;
        engine.mutateResult.defragModalRequested = true;
        engine.defragResult.success = true;
        engine.defragResult.footerMessage =
            "PATCH MUTATOR: Mutation history renumbered. Mutations: 1 used / 99 available. "
            "Retries: 0 used / 100 left under them.";
        engine.defragResult.footerSeverity = "info";

        Core::MutatorActionHandler handler({
            proc.apvts,
            &engine,
            {},
            [](std::function<void()> onConfirmed)
            {
                if (onConfirmed)
                    onConfirmed();
            },
            {},
            {},
            {}
        });

        handler.handleAction(PatchMutator::kMutate, juce::int64(1));

        expectEquals(engine.defragCallCount, 1);
        expectEquals(proc.apvts.state.getProperty("uiMessageText").toString(),
                     engine.defragResult.footerMessage);
        expectEquals(proc.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String("info"));
    }
};

static MutatorActionHandlerDefragTests mutatorActionHandlerDefragTests;
