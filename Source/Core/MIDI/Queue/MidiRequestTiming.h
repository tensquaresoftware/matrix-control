#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"

namespace Core::MidiRequestTiming
{
    // Named SSOT for dump + Device Inquiry settle / outbound-idle waits (V1.2).
    // Floors keep slow interfaces safe; profile delay can raise them for Matrix-6 stock, etc.
    constexpr int kMinDeviceSettleMs = 50;
    constexpr int kMinOutboundIdleTimeoutMs = 500;
    constexpr int kSettleProfileMultiplier = 5;
    constexpr int kIdleTimeoutProfileMultiplier = 50;

    // Device presence monitoring on already-open MIDI From/To.
    // Undetected retry stays fixed; detected intervals come from Settings PRESENCE CHECK
    // (PresenceCheckPolicy) — kDevicePresenceHeartbeatDetectedMs maps EVERY 5 S only.
    constexpr int kDevicePresenceRetryUndetectedMs = 1000;
    constexpr int kDevicePresenceHeartbeatDetectedMs = 5000;
    constexpr int kDevicePresenceHeartbeatEvery15sMs = 15000;
    constexpr int kDevicePresenceHeartbeatEvery30sMs = 30000;

    inline int deviceSettleMs(int profileDelayMs) noexcept
    {
        return juce::jmax(kMinDeviceSettleMs, profileDelayMs * kSettleProfileMultiplier);
    }

    inline int outboundIdleTimeoutMs(int profileDelayMs) noexcept
    {
        return juce::jmax(kMinOutboundIdleTimeoutMs,
                          profileDelayMs * kIdleTimeoutProfileMultiplier);
    }

    inline int deviceSettleMs(const SysExDelayProfile& profile) noexcept
    {
        return deviceSettleMs(profile.getDelayMs());
    }

    inline int outboundIdleTimeoutMs(const SysExDelayProfile& profile) noexcept
    {
        return outboundIdleTimeoutMs(profile.getDelayMs());
    }
}
