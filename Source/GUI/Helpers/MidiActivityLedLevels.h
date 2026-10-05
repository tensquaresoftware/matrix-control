#pragma once

#include "Core/MIDI/MidiActivityTracker.h"
#include "GUI/Widgets/Led.h"

namespace TSS::MidiActivityLedLevels
{
    // Named path contract for apply() — unit tests pin these without constructing Leds.
    inline constexpr auto kKeyboardPath = Core::MidiActivityTracker::Path::kInstrument;
    inline constexpr auto kSynthFromPath = Core::MidiActivityTracker::Path::kMidiFromInbound;
    inline constexpr auto kSynthToPath = Core::MidiActivityTracker::Path::kOutbound;

    /** Shared header / Settings MIDI activity flux — one tracker, three LED paths. */
    inline void apply(const Core::MidiActivityTracker& tracker,
                      Led& keyboardLed,
                      Led& synthFromLed,
                      Led& synthToLed) noexcept
    {
        keyboardLed.setLevel(tracker.getActivityLevel(kKeyboardPath));
        synthFromLed.setLevel(tracker.getActivityLevel(kSynthFromPath));
        synthToLed.setLevel(tracker.getActivityLevel(kSynthToPath));
    }
}
