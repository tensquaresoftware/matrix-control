#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace Core
{
    /// Editor-path producer (FR-6): SysEx and Program Change on the MIDI thread only.
    /// No MidiSender / MidiOutput — enqueue to MidiOutboundQueue for Story 2.9 consumer.
    class EditorPath
    {
    public:
        explicit EditorPath(MidiOutboundQueue& queue) noexcept;

        void enqueueSysEx(juce::MemoryBlock sysEx);
        void enqueueProgramChange(int programNumber, int channel = 1);

    private:
        MidiOutboundQueue& queue_;
    };
}
