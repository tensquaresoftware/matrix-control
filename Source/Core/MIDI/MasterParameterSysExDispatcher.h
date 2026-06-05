#pragma once

#include <functional>
#include <unordered_set>

#include <juce_core/juce_core.h>

#include "Core/Models/MasterModel.h"

namespace Core
{

    /// Routes a single MASTER APVTS parameter change to a full Master Parameter Data (0x03) SysEx.
    /// Message-thread only — invoked from PluginProcessor::valueTreePropertyChanged.
    class MasterParameterSysExDispatcher
    {
    public:
        using EnqueueMasterFn = std::function<void(const juce::uint8* packedData)>;

        MasterParameterSysExDispatcher(const MasterModel& masterModel, EnqueueMasterFn enqueueMaster);

        void dispatch(const juce::String& parameterId) const;

    private:
        static std::unordered_set<juce::String> buildMasterParameterIdSet();

        const MasterModel& masterModel_;
        EnqueueMasterFn enqueueMaster_;
        std::unordered_set<juce::String> masterParameterIds_;
    };

}
