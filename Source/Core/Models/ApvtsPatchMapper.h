#pragma once

#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{
    class PatchModel;

    class ApvtsPatchMapper
    {
    public:
        ApvtsPatchMapper(juce::AudioProcessorValueTreeState& apvts, PatchModel& model);

        void apvtsToBuffer();

        // Message thread only — see JUCE AudioProcessor::setValueNotifyingHost.
        void bufferToApvts();

        // Push one Matrix Mod bus (source, amount, destination) from PatchModel to APVTS.
        // Caller must suppress Matrix Mod SysEx dispatch when used during bulk reorder (Story 2.10).
        void pushBusToApvts(int busIndex);

        static std::vector<PluginDescriptors::IntParameterDescriptor> buildIntDescriptors();
        static std::vector<PluginDescriptors::ChoiceParameterDescriptor> buildChoiceDescriptors();

    private:
        void syncIntToBuffer(const PluginDescriptors::IntParameterDescriptor& d);
        void syncChoiceToBuffer(const PluginDescriptors::ChoiceParameterDescriptor& d);
        void pushIntToApvts(const PluginDescriptors::IntParameterDescriptor& d);
        void pushChoiceToApvts(const PluginDescriptors::ChoiceParameterDescriptor& d);

        juce::AudioProcessorValueTreeState& apvts_;
        PatchModel& model_;
        std::vector<PluginDescriptors::IntParameterDescriptor> intDescriptors_;
        std::vector<PluginDescriptors::ChoiceParameterDescriptor> choiceDescriptors_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApvtsPatchMapper)
    };

} // namespace Core
