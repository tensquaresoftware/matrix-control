#include "Core/Audio/InstrumentMidiForwarder.h"

namespace Core
{
    namespace
    {
        bool isAllowedInstrumentMessage(const juce::MidiMessage& msg) noexcept
        {
            return msg.isNoteOnOrOff() || msg.isController() || msg.isPitchWheel();
        }
    }

    void InstrumentMidiForwarder::forward(const juce::MidiBuffer& midiMessages,
                                            bool instrumentPathEnabled,
                                            MidiOutboundQueue& queue) const
    {
        if (!instrumentPathEnabled)
            return;

        for (const auto metadata : midiMessages)
        {
            const auto message = metadata.getMessage();
            if (isAllowedInstrumentMessage(message))
                queue.enqueueRealtime(message);
        }
    }
}
