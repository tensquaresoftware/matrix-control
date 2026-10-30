#pragma once

#include "Core/MIDI/Queue/MidiRequestTiming.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core::PresenceCheckPolicy
{
    /** Clamp invalid / missing preference ids to EVERY 15 S. */
    inline int normalize(int rawId) noexcept
    {
        using namespace PluginIDs::Settings::PresenceCheck;

        switch (rawId)
        {
            case kOnceOnConnect:
            case kEvery5s:
            case kEvery15s:
            case kEvery30s:
                return rawId;
            default:
                return kDefault;
        }
    }

    /** Detected-state heartbeat interval in ms; 0 = no periodic re-proof (ONCE ON CONNECT). */
    inline int presenceCheckDetectedIntervalMs(int preferenceId) noexcept
    {
        using namespace PluginIDs::Settings::PresenceCheck;

        switch (normalize(preferenceId))
        {
            case kOnceOnConnect:
                return 0;
            case kEvery5s:
                return MidiRequestTiming::kDevicePresenceHeartbeatDetectedMs;
            case kEvery15s:
                return MidiRequestTiming::kDevicePresenceHeartbeatEvery15sMs;
            case kEvery30s:
                return MidiRequestTiming::kDevicePresenceHeartbeatEvery30sMs;
            default:
                return MidiRequestTiming::kDevicePresenceHeartbeatEvery15sMs;
        }
    }

    /**
     * Detected timer interval after composing ONCE + soft-abort / unresponsive recovery.
     * Recovery uses EVERY 15 S cadence (not EVERY 5 S) to avoid slamming the bus.
     * Returns 0 when no detected heartbeat should run.
     */
    inline int detectedPresenceTimerIntervalMs(int preferenceId,
                                               bool deviceMidiUnresponsive) noexcept
    {
        const int mapped = presenceCheckDetectedIntervalMs(preferenceId);
        if (mapped == 0 && deviceMidiUnresponsive)
            return MidiRequestTiming::kDevicePresenceHeartbeatEvery15sMs;

        return mapped;
    }

    /**
     * Full presence-timer schedule decision. Returns 0 → stop timer.
     * !portsEligible → 0; !detected → undetected retry; else detectedPresenceTimerIntervalMs.
     */
    inline int monitoringIntervalMs(bool portsEligible,
                                    bool detected,
                                    int preferenceId,
                                    bool deviceMidiUnresponsive) noexcept
    {
        if (! portsEligible)
            return 0;

        if (! detected)
            return MidiRequestTiming::kDevicePresenceRetryUndetectedMs;

        return detectedPresenceTimerIntervalMs(preferenceId, deviceMidiUnresponsive);
    }
}
