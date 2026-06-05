#pragma once

#include <functional>
#include <unordered_map>

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

    /// Routes a single PATCH APVTS parameter change to Remote Parameter Edit (0x06) SysEx.
    /// Message-thread only — invoked from PluginProcessor::valueTreePropertyChanged.
    class PatchParameterSysExDispatcher
    {
    public:
        using EnqueueRemoteEditFn = std::function<void(int parameterNumber, juce::uint8 packedValue)>;

        PatchParameterSysExDispatcher(const PatchModel& patchModel, EnqueueRemoteEditFn enqueueRemoteEdit);

        void dispatch(const juce::String& parameterId) const;

    private:
        static std::unordered_map<juce::String, PluginDescriptors::IntParameterDescriptor> buildIntMap();
        static std::unordered_map<juce::String, PluginDescriptors::ChoiceParameterDescriptor> buildChoiceMap();

        juce::uint8 readPackedByte(int sysExOffset) const noexcept;

        const PatchModel& patchModel_;
        EnqueueRemoteEditFn enqueueRemoteEdit_;
        std::unordered_map<juce::String, PluginDescriptors::IntParameterDescriptor> intByParameterId_;
        std::unordered_map<juce::String, PluginDescriptors::ChoiceParameterDescriptor> choiceByParameterId_;
    };
}
