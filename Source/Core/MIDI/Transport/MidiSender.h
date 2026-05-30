#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/Exceptions/Exceptions.h"

class MidiSender
{
public:
    MidiSender();
    ~MidiSender() = default;

    void setMidiOutput(juce::MidiOutput* output) noexcept;

    void sendSysEx(const juce::MemoryBlock& sysExData);
    void sendProgramChange(int programNumber, int channel = 1);
    void sendNoteOn(juce::uint8 noteNumber, juce::uint8 velocity, int channel = 1);
    void sendNoteOff(juce::uint8 noteNumber, juce::uint8 velocity = 64, int channel = 1);
    void sendControlChange(int controllerNumber, int value, int channel = 1);
    
    bool isOutputAvailable() const noexcept;

private:
    juce::MidiOutput* midiOutput;

    void ensureOutputAvailable() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiSender)
};

