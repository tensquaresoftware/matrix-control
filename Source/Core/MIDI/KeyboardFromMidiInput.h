#pragma once

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace Core
{
    /// Standalone Keyboard From producer — dedicated juce::MidiInput → MidiOutboundQueue (FR-39 partial).
    /// Allowed: Note On/Off, Control Change, Pitch Bend (same allow-list as InstrumentMidiForwarder).
    /// Must only enqueue from the MIDI input callback thread (never audio thread).
    class KeyboardFromMidiInput : public juce::MidiInputCallback
    {
    public:
        explicit KeyboardFromMidiInput(MidiOutboundQueue& queue);
        ~KeyboardFromMidiInput() override;

        bool setPort(const juce::String& deviceId);
        void closePort();
        bool isPortOpen() const noexcept;

        void processIncomingMessage(const juce::MidiMessage& message);

        void handleIncomingMidiMessage(juce::MidiInput* source,
                                       const juce::MidiMessage& message) override;

    private:
        MidiOutboundQueue& queue_;
        std::unique_ptr<juce::MidiInput> midiInput_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardFromMidiInput)
    };
}
