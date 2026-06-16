#pragma once

#include <atomic>
#include <cmath>

#include <juce_core/juce_core.h>

namespace Core
{
    /// Ephemeral MIDI activity edges for instrument vs editor outbound paths (FR-9).
    /// Returns a short pulse after each notify so the ~30 Hz UI poll can catch it.
    /// Visual hold/decay is owned by Led (kReleaseTimeMs_).
    /// Writers: any producer/consumer thread (lock-free atomics). Readers: message thread only.
    class MidiActivityTracker
    {
    public:
        enum class Path
        {
            kInstrument,
            kEditor,
            kOutbound,
            kMidiFromInbound
        };

        void notifyActivity(Path path) noexcept;
        float getActivityLevel(Path path) const noexcept;

        /// Test seam — pulse math with explicit timestamps (message-thread reads only).
        static float computeLevelFromTimestamp(juce::int64 lastNotifyMs, juce::int64 nowMs) noexcept;

    private:
        /// Coalesces bursts between UI polls; not a visual hold constant.
        inline constexpr static juce::int64 kActivityPulseMs_ = 34;

        std::atomic<juce::int64> instrumentLastNotifyMs_{ 0 };
        std::atomic<juce::int64> editorLastNotifyMs_{ 0 };
        std::atomic<juce::int64> outboundLastNotifyMs_{ 0 };
        std::atomic<juce::int64> midiFromInboundLastNotifyMs_{ 0 };
    };
}
