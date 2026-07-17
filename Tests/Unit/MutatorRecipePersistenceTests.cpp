#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "Core/Services/PatchMutator/MutatorSessionPersistence.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace MutatorWidgets = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
namespace PatchNameIds = PluginIDs::PatchEditSection::PatchNameModule;
namespace PatchNameDefaults = PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets;

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

        auto restoredState = juce::ValueTree::fromXml(*xmlState);
        Core::MutatorSessionPersistence::resetEphemeralStateAfterSessionLoad(restoredState);
        apvts.replaceState(restoredState);
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
        MutatorWidgets::kEnableLfo2,
        MutatorWidgets::kEnableMatrixMod
    };
}

class MutatorRecipePersistenceTests : public juce::UnitTest
{
public:
    MutatorRecipePersistenceTests() : juce::UnitTest("MutatorRecipePersistence") {}

    void runTest() override
    {
        recipe_sessionRoundTrip_preservesRecipe_stripsHistory();
        recipe_sessionRoundTrip_stripsPatchName_resetsToDefault();
        recipe_staleXmlPatchName_loadForcesDefault();
        recipe_liveSession_patchNameRemainsUntilPersistenceStrip();
        recipe_initialize_missingDefaults_amount50_random25();
        recipe_initialize_clampsLegacyZeroAndOutOfRange();
        recipe_emptyHistoryMirrors_allTogglesOff_disablesMutate();
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
        expect(! xmlString.contains(PatchNameIds::kPatchName));
    }

    void recipe_sessionRoundTrip_preservesRecipe_stripsHistory()
    {
        beginTest("recipe_sessionRoundTrip_preservesRecipe_stripsHistory");

        TestAudioProcessorPersistence source;
        auto& sourceState = source.apvts.state;

        sourceState.setProperty(MutatorWidgets::kAmount, 42, nullptr);
        sourceState.setProperty(MutatorWidgets::kRandom, 17, nullptr);

        for (int i = 0; i < 11; ++i)
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

        for (int i = 0; i < 11; ++i)
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

    void recipe_sessionRoundTrip_stripsPatchName_resetsToDefault()
    {
        beginTest("recipe_sessionRoundTrip_stripsPatchName_resetsToDefault");

        TestAudioProcessorPersistence source;
        source.apvts.state.setProperty(PatchNameIds::kPatchName, "M00", nullptr);

        juce::MemoryBlock savedState;
        source.getStateInformation(savedState);
        expectSerializedXmlOmitsEphemeralMutatorState(savedState);

        TestAudioProcessorPersistence restored;
        restored.setStateInformation(savedState.getData(), static_cast<int>(savedState.getSize()));

        expectEquals(restored.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String(PatchNameDefaults::kDefaultPatchName));
    }

    void recipe_staleXmlPatchName_loadForcesDefault()
    {
        beginTest("recipe_staleXmlPatchName_loadForcesDefault");

        juce::ValueTree state("P");
        state.setProperty(PatchNameIds::kPatchName, "MY-PATCH", nullptr);

        Core::MutatorSessionPersistence::resetEphemeralStateAfterSessionLoad(state);

        expectEquals(state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String(PatchNameDefaults::kDefaultPatchName));
    }

    void recipe_liveSession_patchNameRemainsUntilPersistenceStrip()
    {
        beginTest("recipe_liveSession_patchNameRemainsUntilPersistenceStrip");

        juce::ValueTree live("P");
        live.setProperty(PatchNameIds::kPatchName, "M00", nullptr);

        expectEquals(live.getProperty(PatchNameIds::kPatchName).toString(), juce::String("M00"));

        auto toPersist = live.createCopy();
        Core::MutatorSessionPersistence::stripEphemeralStateForPersistence(toPersist);
        expect(! toPersist.hasProperty(PatchNameIds::kPatchName));
        expectEquals(live.getProperty(PatchNameIds::kPatchName).toString(), juce::String("M00"));
    }

    void recipe_initialize_missingDefaults_amount50_random25()
    {
        beginTest("recipe_initialize_missingDefaults_amount50_random25");

        juce::ValueTree state("P");
        expect(! state.hasProperty(MutatorWidgets::kAmount));
        expect(! state.hasProperty(MutatorWidgets::kRandom));

        Core::MutatorSessionPersistence::initializeRecipeState(state);

        expectEquals(static_cast<int>(state.getProperty(MutatorWidgets::kAmount)), 50);
        expectEquals(static_cast<int>(state.getProperty(MutatorWidgets::kRandom)), 25);
    }

    void recipe_initialize_clampsLegacyZeroAndOutOfRange()
    {
        beginTest("recipe_initialize_clampsLegacyZeroAndOutOfRange");

        juce::ValueTree state("P");
        state.setProperty(MutatorWidgets::kAmount, 0, nullptr);
        state.setProperty(MutatorWidgets::kRandom, 150, nullptr);

        Core::MutatorSessionPersistence::initializeRecipeState(state);

        expectEquals(static_cast<int>(state.getProperty(MutatorWidgets::kAmount)), 1);
        expectEquals(static_cast<int>(state.getProperty(MutatorWidgets::kRandom)), 100);

        state.setProperty(MutatorWidgets::kAmount, -3, nullptr);
        state.setProperty(MutatorWidgets::kRandom, 0, nullptr);
        Core::MutatorSessionPersistence::initializeRecipeState(state);

        expectEquals(static_cast<int>(state.getProperty(MutatorWidgets::kAmount)), 1);
        expectEquals(static_cast<int>(state.getProperty(MutatorWidgets::kRandom)), 1);
    }

    void recipe_emptyHistoryMirrors_allTogglesOff_disablesMutate()
    {
        beginTest("recipe_emptyHistoryMirrors_allTogglesOff_disablesMutate");

        juce::ValueTree state("P");
        Core::MutatorSessionPersistence::initializeRecipeState(state);
        Core::MutatorSessionPersistence::setActionEnabledMirrorsForEmptyHistory(state);

        expect(! static_cast<bool>(state.getProperty(MutatorState::kMutateEnabled)));
        expect(! static_cast<bool>(state.getProperty(MutatorState::kRetryEnabled)));
        expect(! static_cast<bool>(state.getProperty(MutatorState::kExportEnabled)));
        expect(! static_cast<bool>(state.getProperty(MutatorState::kDeleteEnabled)));
        expect(! static_cast<bool>(state.getProperty(MutatorState::kClearEnabled)));

        state.setProperty(MutatorWidgets::kEnableLfo2, true, nullptr);
        Core::MutatorSessionPersistence::setActionEnabledMirrorsForEmptyHistory(state);
        expect(static_cast<bool>(state.getProperty(MutatorState::kMutateEnabled)));
    }
};


static MutatorRecipePersistenceTests mutatorRecipePersistenceTests;
