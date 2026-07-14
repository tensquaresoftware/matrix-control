#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "Core/Services/SessionPersistencePolicy.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/PluginIDs.h"

namespace MatrixModWidgets = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;

class TestSessionPersistenceProcessor : public juce::AudioProcessor
{
public:
    TestSessionPersistenceProcessor()
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
        const auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState == nullptr || ! xmlState->hasTagName(apvts.state.getType()))
            return;

        auto restoredState = juce::ValueTree::fromXml(*xmlState);

        if (Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(restoredState))
            Core::SessionPersistencePolicy::stripPatchAndMasterParameters(restoredState);

        apvts.replaceState(restoredState);
    }
};

class SessionPersistencePolicyTests : public juce::UnitTest
{
public:
    SessionPersistencePolicyTests() : juce::UnitTest("SessionPersistencePolicy") {}

    void runTest() override
    {
        testShouldStripWhenBothPortsConfigured();
        testWhitespacePortsDoNotArmStrip();
        testStripRemovesPatchParameterValues();
        testStripRemovesMatrixModParameterValues();
        testPreservesPrefsWhenStripping();
        testNoStripWhenPortsMissing();
        testProcessorRoundTripStripsPatchParameter();
    }

private:
    void testShouldStripWhenBothPortsConfigured()
    {
        beginTest("shouldStripPatchAndMasterParameters requires both MIDI ports");

        juce::ValueTree state("PARAM");
        expect(! Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(state));

        state.setProperty("midiInputPortId", "input-a", nullptr);
        expect(! Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(state));

        state.setProperty("midiOutputPortId", "output-a", nullptr);
        expect(Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(state));
    }

    void testWhitespacePortsDoNotArmStrip()
    {
        beginTest("whitespace-only MIDI port IDs do not arm strip policy");

        juce::ValueTree state("PARAM");
        state.setProperty("midiInputPortId", "   ", nullptr);
        state.setProperty("midiOutputPortId", "output-a", nullptr);
        expect(! Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(state));

        state.setProperty("midiInputPortId", "input-a", nullptr);
        state.setProperty("midiOutputPortId", "\t", nullptr);
        expect(! Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(state));
    }

    void testStripRemovesPatchParameterValues()
    {
        beginTest("stripPatchAndMasterParameters removes patch parameter nodes");

        juce::ValueTree state("PARAM");
        state.setProperty("midiInputPortId", "input-a", nullptr);
        state.setProperty("midiOutputPortId", "output-a", nullptr);

        juce::ValueTree patchParam(ApvtsTypes::kParam);
        patchParam.setProperty("id", PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency, nullptr);
        patchParam.setProperty(ApvtsTypes::kValue, 0.75f, nullptr);
        state.appendChild(patchParam, nullptr);

        juce::ValueTree masterParam(ApvtsTypes::kParam);
        masterParam.setProperty("id", PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel, nullptr);
        masterParam.setProperty(ApvtsTypes::kValue, 0.25f, nullptr);
        state.appendChild(masterParam, nullptr);

        Core::SessionPersistencePolicy::stripPatchAndMasterParameters(state);

        expect(! state.getChildWithProperty("id", PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency).isValid());
        expect(! state.getChildWithProperty("id", PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel).isValid());
    }

    void testStripRemovesMatrixModParameterValues()
    {
        beginTest("stripPatchAndMasterParameters removes matrix-mod parameter nodes");

        juce::ValueTree state("PARAM");
        state.setProperty("midiInputPortId", "input-a", nullptr);
        state.setProperty("midiOutputPortId", "output-a", nullptr);

        juce::ValueTree matrixModParam(ApvtsTypes::kParam);
        matrixModParam.setProperty("id", MatrixModWidgets::kBus0Amount, nullptr);
        matrixModParam.setProperty(ApvtsTypes::kValue, 0.42f, nullptr);
        state.appendChild(matrixModParam, nullptr);

        Core::SessionPersistencePolicy::stripPatchAndMasterParameters(state);

        expect(! state.getChildWithProperty("id", MatrixModWidgets::kBus0Amount).isValid());
    }

    void testPreservesPrefsWhenStripping()
    {
        beginTest("stripPatchAndMasterParameters preserves non-parameter prefs");

        juce::ValueTree state("PARAM");
        state.setProperty("midiInputPortId", "input-a", nullptr);
        state.setProperty("midiOutputPortId", "output-a", nullptr);
        state.setProperty("settingsSkinVariant", "cream", nullptr);
        state.setProperty("guiScaleId", 3, nullptr);

        juce::ValueTree patchParam(ApvtsTypes::kParam);
        patchParam.setProperty("id", PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency, nullptr);
        patchParam.setProperty(ApvtsTypes::kValue, 0.75f, nullptr);
        state.appendChild(patchParam, nullptr);

        Core::SessionPersistencePolicy::stripPatchAndMasterParameters(state);

        expectEquals(state.getProperty("settingsSkinVariant").toString(), juce::String("cream"));
        expectEquals(static_cast<int>(state.getProperty("guiScaleId")), 3);
        expectEquals(state.getProperty("midiInputPortId").toString(), juce::String("input-a"));
    }

    void testNoStripWhenPortsMissing()
    {
        beginTest("patch params remain in state when strip policy is not armed");

        juce::ValueTree state("PARAM");
        state.setProperty("midiInputPortId", juce::String(), nullptr);
        state.setProperty("midiOutputPortId", juce::String(), nullptr);

        juce::ValueTree patchParam(ApvtsTypes::kParam);
        patchParam.setProperty("id", PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency, nullptr);
        patchParam.setProperty(ApvtsTypes::kValue, 0.75f, nullptr);
        state.appendChild(patchParam, nullptr);

        expect(! Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(state));
        expect(state.getChildWithProperty("id", PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency).isValid());
    }

    void testProcessorRoundTripStripsPatchParameter()
    {
        beginTest("session round-trip strips persisted patch param when MIDI ports configured");

        const auto patchParamId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency;

        TestSessionPersistenceProcessor fresh;
        const float defaultValue = fresh.apvts.getParameter(patchParamId)->getValue();

        TestSessionPersistenceProcessor source;
        source.apvts.getParameter(patchParamId)->setValueNotifyingHost(0.75f);
        const float savedValue = source.apvts.getParameter(patchParamId)->getValue();
        expect(savedValue != defaultValue);

        source.apvts.state.setProperty("midiInputPortId", "input-a", nullptr);
        source.apvts.state.setProperty("midiOutputPortId", "output-a", nullptr);

        juce::MemoryBlock savedState;
        source.getStateInformation(savedState);

        TestSessionPersistenceProcessor restored;
        restored.setStateInformation(savedState.getData(), static_cast<int>(savedState.getSize()));

        const float restoredValue = restored.apvts.getParameter(patchParamId)->getValue();
        expectEquals(restoredValue, defaultValue);
        expect(restoredValue != savedValue);
        expectEquals(restored.apvts.state.getProperty("midiInputPortId").toString(), juce::String("input-a"));
        expectEquals(restored.apvts.state.getProperty("midiOutputPortId").toString(), juce::String("output-a"));
    }
};

static SessionPersistencePolicyTests sessionPersistencePolicyTests;
