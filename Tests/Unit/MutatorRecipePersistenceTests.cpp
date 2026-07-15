#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "Core/Services/PatchMutator/MutatorSessionPersistence.h"
#include "Shared/Definitions/PluginIDs.h"

namespace MutatorWidgets = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

class TestAudioProcessorPersistence : public juce::AudioProcessor
{
public:
    TestAudioProcessorPersistence()
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

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        auto state = apvts.copyState();
        Core::MutatorSessionPersistence::stripEphemeralStateForPersistence(state);
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState == nullptr || ! xmlState->hasTagName(apvts.state.getType()))
            return;

        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        Core::MutatorSessionPersistence::initializeRecipeState(apvts.state);
        Core::MutatorSessionPersistence::resetEphemeralStateAfterSessionLoad(apvts.state);
        Core::MutatorSessionPersistence::setActionEnabledMirrorsForEmptyHistory(apvts.state);
    }
};

namespace
{
    constexpr const char* kRecipeToggleIds[] = {
        MutatorWidgets::kEnableDco1,
        MutatorWidgets::kEnableDco2,
        MutatorWidgets::kEnableVcfVca,
        MutatorWidgets::kEnableFmTrack,
        MutatorWidgets::kEnableRampPortamento,
        MutatorWidgets::kEnableEnvelope1,
        MutatorWidgets::kEnableEnvelope2,
        MutatorWidgets::kEnableEnvelope3,
        MutatorWidgets::kEnableLfo1,
        MutatorWidgets::kEnableLfo2
    };
}

class MutatorRecipePersistenceTests : public juce::UnitTest
{
public:
    MutatorRecipePersistenceTests() : juce::UnitTest("MutatorRecipePersistence") {}

    void runTest() override
    {
        recipe_sessionRoundTrip_preservesRecipe_stripsHistory();
    }

private:
    void expectSerializedXmlOmitsEphemeralMutatorState(const juce::MemoryBlock& savedState)
    {
        const auto xml = juce::AudioProcessor::getXmlFromBinary(savedState.getData(),
                                                                static_cast<int>(savedState.getSize()));
        expect(xml != nullptr);

        const auto xmlString = xml->toString();
        expect(! xmlString.contains(MutatorState::kHistoryMutateList));
        expect(! xmlString.contains(MutatorState::kHistoryRetryList));
        expect(! xmlString.contains(MutatorState::kHistoryRetryListsByRoot));
        expect(! xmlString.contains(MutatorState::kSelectedMutateRootIndex));
        expect(! xmlString.contains(MutatorState::kSelectedRetryIndex));
        expect(! xmlString.contains(MutatorState::kCompareActive));
        expect(! xmlString.contains(MutatorState::kMutateEnabled));
        expect(! xmlString.contains(MutatorState::kRetryEnabled));
        expect(! xmlString.contains(MutatorState::kExportEnabled));
        expect(! xmlString.contains(MutatorState::kDeleteEnabled));
        expect(! xmlString.contains(MutatorState::kClearEnabled));
    }

    void recipe_sessionRoundTrip_preservesRecipe_stripsHistory()
    {
        beginTest("recipe_sessionRoundTrip_preservesRecipe_stripsHistory");

        TestAudioProcessorPersistence source;
        auto& sourceState = source.apvts.state;

        sourceState.setProperty(MutatorWidgets::kAmount, 42, nullptr);
        sourceState.setProperty(MutatorWidgets::kRandom, 17, nullptr);

        for (int i = 0; i < 10; ++i)
            sourceState.setProperty(kRecipeToggleIds[static_cast<size_t>(i)], (i % 2) == 0, nullptr);

        sourceState.setProperty(MutatorState::kHistoryMutateList, "M00", nullptr);
        sourceState.setProperty(MutatorState::kHistoryRetryList, "M00|R00", nullptr);
        sourceState.setProperty(MutatorState::kHistoryRetryListsByRoot, "0=—|R00", nullptr);
        sourceState.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        sourceState.setProperty(MutatorState::kSelectedRetryIndex, 0, nullptr);
        sourceState.setProperty(MutatorState::kCompareActive, true, nullptr);
        sourceState.setProperty(MutatorState::kMutateEnabled, false, nullptr);
        sourceState.setProperty(MutatorState::kExportEnabled, true, nullptr);

        juce::MemoryBlock savedState;
        source.getStateInformation(savedState);
        expectSerializedXmlOmitsEphemeralMutatorState(savedState);

        TestAudioProcessorPersistence restored;
        restored.setStateInformation(savedState.getData(), static_cast<int>(savedState.getSize()));

        const auto& restoredState = restored.apvts.state;

        expectEquals(static_cast<int>(restoredState.getProperty(MutatorWidgets::kAmount)), 42);
        expectEquals(static_cast<int>(restoredState.getProperty(MutatorWidgets::kRandom)), 17);

        for (int i = 0; i < 10; ++i)
        {
            const bool expected = (i % 2) == 0;
            expect(static_cast<bool>(restoredState.getProperty(kRecipeToggleIds[static_cast<size_t>(i)])) == expected);
        }

        expect(restoredState.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty());
        expect(restoredState.getProperty(MutatorState::kHistoryRetryList).toString().isEmpty());
        expect(restoredState.getProperty(MutatorState::kHistoryRetryListsByRoot).toString().isEmpty());
        expectEquals(static_cast<int>(restoredState.getProperty(MutatorState::kSelectedMutateRootIndex)), -1);
        expectEquals(static_cast<int>(restoredState.getProperty(MutatorState::kSelectedRetryIndex)),
                     MutatorState::kSelectedRetryRootOnly);
        expect(! static_cast<bool>(restoredState.getProperty(MutatorState::kCompareActive)));

        expect(static_cast<bool>(restoredState.getProperty(MutatorState::kMutateEnabled)));
        expect(! static_cast<bool>(restoredState.getProperty(MutatorState::kRetryEnabled)));
        expect(! static_cast<bool>(restoredState.getProperty(MutatorState::kExportEnabled)));
        expect(! static_cast<bool>(restoredState.getProperty(MutatorState::kDeleteEnabled)));
        expect(! static_cast<bool>(restoredState.getProperty(MutatorState::kClearEnabled)));
    }
};

static MutatorRecipePersistenceTests mutatorRecipePersistenceTests;
