#pragma once

#include <atomic>
#include <cmath>

#include <juce_core/juce_core.h>

namespace Core
{
    /// Ephemeral MIDI activity levels for instrument vs editor outbound paths (FR-9).
    /// Writers: any producer/consumer thread (lock-free atomics). Readers: message thread only.
    class MidiActivityTracker
    {
    public:
        enum class Path
        {
            kInstrument,
            kEditor,
            kOutbound
        };

        void notifyActivity(Path path) noexcept;
        float getActivityLevel(Path path) const noexcept;

        /// Test seam — decay math with explicit timestamps (message-thread reads only).
        static float computeLevelFromTimestamp(juce::int64 lastNotifyMs, juce::int64 nowMs) noexcept;

    private:
        inline constexpr static juce::int64 kActivityHoldMs_ = 150;
        inline constexpr static juce::int64 kActivityDecayMs_ = 350;
        inline constexpr static float kDecayExponent_ = 5.0f;

        std::atomic<juce::int64> instrumentLastNotifyMs_{ 0 };
        std::atomic<juce::int64> editorLastNotifyMs_{ 0 };
        std::atomic<juce::int64> outboundLastNotifyMs_{ 0 };
    };
}
