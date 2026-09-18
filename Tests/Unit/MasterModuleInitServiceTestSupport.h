#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/MidiChannelMasterCodec.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

// Minimal AudioProcessor host for MasterModuleInitService unit tests.
class TestAudioProcessorMasterInit : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessorMasterInit(juce::AudioProcessorValueTreeState::ParameterLayout layout)
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

namespace MasterModelTestHelpers
{
    inline void fillModelWithDistinctValues(Core::MasterModel& model)
    {
        using namespace PluginDescriptors::MasterEditSection;
        namespace MidiIds = PluginIDs::MasterEditSection::MidiModule;
        namespace VibratoIds = PluginIDs::MasterEditSection::VibratoModule;
        namespace MiscIds = PluginIDs::MasterEditSection::MiscModule;

        for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
        {
            if (d.parentGroupId == MidiIds::kGroupId)
                model.setValue(d, d.maxValue);
            else if (d.parentGroupId == VibratoIds::kGroupId)
                model.setValue(d, d.minValue);
            else if (d.parentGroupId == MiscIds::kGroupId)
                model.setValue(d, (d.minValue + d.maxValue) / 2);
        }

        model.setChoiceIndex(MidiModule::kChoiceParameters[0], 2);
        model.setChoiceIndex(VibratoModule::kChoiceParameters[0], 3);
        model.setChoiceIndex(MiscModule::kChoiceParameters[0], 1);
    }

    inline bool channelTripletMatch(const Core::MasterModel& lhs, const Core::MasterModel& rhs)
    {
        return lhs.data()[11] == rhs.data()[11]
            && lhs.data()[12] == rhs.data()[12]
            && lhs.data()[35] == rhs.data()[35];
    }

    inline bool moduleBytesMatch(const Core::MasterModel& lhs,
                                 const Core::MasterModel& rhs,
                                 const juce::String& moduleGroupId)
    {
        using namespace PluginDescriptors::MasterEditSection;

        for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
            if (d.parentGroupId == moduleGroupId && lhs.getValue(d) != rhs.getValue(d))
                return false;

        for (const auto& d : kChoiceParameters)
        {
            if (d.parentGroupId != moduleGroupId)
                continue;

            const bool channelMismatch =
                d.parameterId == PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel
                    ? ! channelTripletMatch(lhs, rhs)
                    : lhs.getChoiceIndex(d) != rhs.getChoiceIndex(d);
            if (channelMismatch)
                return false;
        }

        return true;
    }
}
