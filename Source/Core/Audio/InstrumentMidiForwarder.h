#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace Core
{
    /// Forwards host/keyboard MIDI on the audio thread (FR-5).
    /// Allowed: Note On/Off, Control Change, Pitch Bend.
    /// Stripped (not enqueued): Program Change, SysEx, channel pressure, aftertouch, MTC, etc.
    /// Must only be called from the audio thread (processBlock).
    class InstrumentMidiForwarder
    {
    public:
        void forward(const juce::MidiBuffer& midiMessages,
                     bool instrumentPathEnabled,
                     MidiOutboundQueue& queue) const;
    };
}
