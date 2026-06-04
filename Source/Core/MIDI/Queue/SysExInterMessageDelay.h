#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/Queue/SysExDelayProfile.h"

namespace Core
{
    // Enforces minimum gap after each outbound SysEx EOX (not between realtime MIDI).
    // Single MIDI-thread consumer assumed; setProfile rarely from inquiry path.
    class SysExInterMessageDelay
    {
    public:
        SysExInterMessageDelay() = default;
        explicit SysExInterMessageDelay(SysExDelayProfile profile) noexcept;

        void setProfile(SysExDelayProfile profile) noexcept;
        int getRequiredDelayMs() const noexcept;

        int millisUntilNextAllowed(juce::int64 nowMs) const noexcept;
        void recordSysExSent(juce::int64 nowMs) noexcept;

        void waitUntilReady();

    private:
        SysExDelayProfile profile_ { SysExDelayProfile::stockDefault() };
        juce::int64 lastSysExSentMs_ { -1 };
    };
}
