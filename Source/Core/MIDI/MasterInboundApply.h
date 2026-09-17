#pragma once

#include <functional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"

namespace Core
{
    struct QuietInboundMasterApplyArgs
    {
        MasterModel& model;
        ApvtsMasterMapper& mapper;
        juce::AudioProcessorValueTreeState& apvts;
        const std::vector<juce::uint8>& packedMaster;
        std::function<void()> cancelDebounce;
        bool& suppressMasterSysEx;
    };

    /** Quiet inbound Master apply shared by PluginProcessor and unit tests.
        Cancels pending Master debounce, suppresses outbound SysEx, loads the dump,
        pushes to APVTS, flushes deferred sync, then clears suppress. */
    inline void applyQuietInboundMasterDump(const QuietInboundMasterApplyArgs& args)
    {
        if (args.packedMaster.size() != SysExConstants::kMasterPackedDataSize)
            return;

        if (args.cancelDebounce)
            args.cancelDebounce();

        args.suppressMasterSysEx = true;
        args.model.loadFrom(args.packedMaster.data());
        args.mapper.bufferToApvts();
        (void) args.apvts.copyState();

        if (args.cancelDebounce)
            args.cancelDebounce();

        args.suppressMasterSysEx = false;
    }
}
