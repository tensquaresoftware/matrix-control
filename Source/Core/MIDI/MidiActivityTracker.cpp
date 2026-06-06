#include "Core/MIDI/MidiActivityTracker.h"

namespace Core
{
    float MidiActivityTracker::computeLevelFromTimestamp(juce::int64 lastNotifyMs,
                                                         juce::int64 nowMs) noexcept
    {
        if (lastNotifyMs <= 0)
            return 0.0f;

        const juce::int64 elapsed = nowMs - lastNotifyMs;

        if (elapsed <= kActivityHoldMs_)
            return 1.0f;

        const juce::int64 decayElapsed = elapsed - kActivityHoldMs_;

        if (decayElapsed >= kActivityDecayMs_)
            return 0.0f;

        const float decayRatio = static_cast<float>(decayElapsed)
                                 / static_cast<float>(kActivityDecayMs_);
        return std::exp(-decayRatio * kDecayExponent_);
    }

    void MidiActivityTracker::notifyActivity(Path path) noexcept
    {
        const auto now = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());

        switch (path)
        {
            case Path::kInstrument:
                instrumentLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kEditor:
                editorLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kOutbound:
                outboundLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;

            case Path::kMidiFromInbound:
                midiFromInboundLastNotifyMs_.store(now, std::memory_order_relaxed);
                break;
        }
    }

    float MidiActivityTracker::getActivityLevel(Path path) const noexcept
    {
        const auto now = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());

        juce::int64 lastNotify = 0;

        switch (path)
        {
            case Path::kInstrument:
                lastNotify = instrumentLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kEditor:
                lastNotify = editorLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kOutbound:
                lastNotify = outboundLastNotifyMs_.load(std::memory_order_relaxed);
                break;

            case Path::kMidiFromInbound:
                lastNotify = midiFromInboundLastNotifyMs_.load(std::memory_order_relaxed);
                break;
        }

        return computeLevelFromTimestamp(lastNotify, now);
    }
}
