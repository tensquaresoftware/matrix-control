#pragma once

#include <functional>
#include <unordered_map>

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"

namespace Core
{

    /// Routes a single Matrix Mod bus APVTS parameter change to Remote Parameter Edit Matrix (0x0B) SysEx.
    /// Message-thread only — invoked from PluginProcessor::valueTreePropertyChanged.
    class MatrixModBusParameterSysExDispatcher
    {
    public:
        using EnqueueMatrixModBusEditFn = std::function<void(juce::uint8 bus,
                                                             juce::uint8 source,
                                                             juce::uint8 amount,
                                                             juce::uint8 destination)>;

        MatrixModBusParameterSysExDispatcher(const PatchModel& patchModel,
                                             EnqueueMatrixModBusEditFn enqueueMatrixModBusEdit);

        void dispatch(const juce::String& parameterId) const;

        void dispatchBus(int busIndex) const;

    private:
        static std::unordered_map<juce::String, int> buildParameterIdToBusMap();

        juce::uint8 readPackedByte(int sysExOffset) const noexcept;

        const PatchModel& patchModel_;
        EnqueueMatrixModBusEditFn enqueueMatrixModBusEdit_;
        std::unordered_map<juce::String, int> parameterIdToBus_;
    };

}
