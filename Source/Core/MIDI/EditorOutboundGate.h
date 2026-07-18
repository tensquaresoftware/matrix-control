#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
    /** FR-2: editor SysEx / Program Change only when a synth is detected.
        Device Inquiry is the unlock path and must bypass this gate (see maySendEditorSysEx). */
    inline bool isEditorOutboundAllowed(bool deviceDetected) noexcept
    {
        return deviceDetected;
    }

    /** Named allowlist: Universal Device Inquiry request (MMA non-realtime). */
    inline bool isDeviceInquirySysEx(const juce::MemoryBlock& sysEx) noexcept
    {
        constexpr auto kLen = SysExConstants::DeviceInquiry::kRequestMessageLength;
        if (sysEx.getSize() != kLen)
            return false;

        const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
        for (size_t i = 0; i < kLen; ++i)
        {
            if (data[i] != SysExConstants::DeviceInquiry::kRequestMessage[i])
                return false;
        }

        return true;
    }

    inline bool maySendEditorProgramChange(bool deviceDetected) noexcept
    {
        return isEditorOutboundAllowed(deviceDetected);
    }

    inline bool maySendEditorSysEx(bool deviceDetected, const juce::MemoryBlock& sysEx) noexcept
    {
        return isEditorOutboundAllowed(deviceDetected) || isDeviceInquirySysEx(sysEx);
    }

    /** Panels stay locked while undetected or while Mutator Compare is active. */
    inline bool isSectionLocked(bool deviceDetected, bool compareActive) noexcept
    {
        return ! deviceDetected || compareActive;
    }
}
