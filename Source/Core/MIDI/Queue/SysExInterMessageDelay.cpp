#include "Core/MIDI/Queue/SysExInterMessageDelay.h"

#include <chrono>
#include <thread>

namespace Core
{
    SysExInterMessageDelay::SysExInterMessageDelay(SysExDelayProfile profile) noexcept
        : profile_ { profile }
    {
    }

    void SysExInterMessageDelay::setProfile(SysExDelayProfile profile) noexcept
    {
        profile_ = profile;
    }

    int SysExInterMessageDelay::getRequiredDelayMs() const noexcept
    {
        return profile_.getDelayMs();
    }

    int SysExInterMessageDelay::millisUntilNextAllowed(juce::int64 nowMs) const noexcept
    {
        if (lastSysExSentMs_ < 0)
            return 0;

        const auto readyAt = lastSysExSentMs_ + static_cast<juce::int64>(profile_.getDelayMs());
        if (nowMs >= readyAt)
            return 0;

        return static_cast<int>(readyAt - nowMs);
    }

    void SysExInterMessageDelay::recordSysExSent(juce::int64 nowMs) noexcept
    {
        lastSysExSentMs_ = nowMs;
    }

    void SysExInterMessageDelay::waitUntilReady()
    {
        const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
        const int waitMs = millisUntilNextAllowed(nowMs);

        if (waitMs > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(waitMs));
    }
}
