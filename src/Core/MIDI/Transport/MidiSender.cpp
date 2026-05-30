#include "MidiSender.h"

#include "Core/MIDI/Exceptions/Exceptions.h"
#include "Core/Loggers/MidiLogger.h"

MidiSender::MidiSender()
    : midiOutput(nullptr)
{
}

void MidiSender::setMidiOutput(juce::MidiOutput* output) noexcept
{
    midiOutput = output;
}

void MidiSender::sendSysEx(const juce::MemoryBlock& sysExData)
{
    ensureOutputAvailable();

    // The sysExData already contains F0 and F7 delimiters, so we send it directly
    // without using createSysExMessage() which would add them again
    midiOutput->sendMessageNow(juce::MidiMessage(sysExData.getData(), static_cast<int>(sysExData.getSize())));
    MidiLogger::getInstance().logSysExSent(sysExData);
}

void MidiSender::sendProgramChange(int programNumber, int channel)
{
    ensureOutputAvailable();

    juce::MidiMessage message = juce::MidiMessage::programChange(channel, programNumber);
    midiOutput->sendMessageNow(message);
    MidiLogger::getInstance().logProgramChange(static_cast<juce::uint8>(programNumber), "SENT");
}

void MidiSender::sendNoteOn(juce::uint8 noteNumber, juce::uint8 velocity, int channel)
{
    ensureOutputAvailable();

    juce::MidiMessage message = juce::MidiMessage::noteOn(channel, noteNumber, velocity);
    midiOutput->sendMessageNow(message);
}

void MidiSender::sendNoteOff(juce::uint8 noteNumber, juce::uint8 velocity, int channel)
{
    ensureOutputAvailable();

    juce::MidiMessage message = juce::MidiMessage::noteOff(channel, noteNumber, velocity);
    midiOutput->sendMessageNow(message);
}

void MidiSender::sendControlChange(int controllerNumber, int value, int channel)
{
    ensureOutputAvailable();

    juce::MidiMessage message = juce::MidiMessage::controllerEvent(channel, controllerNumber, value);
    midiOutput->sendMessageNow(message);
}

bool MidiSender::isOutputAvailable() const noexcept
{
    return midiOutput != nullptr;
}

void MidiSender::ensureOutputAvailable() const
{
    if (midiOutput == nullptr)
    {
        MidiLogger::getInstance().logError("MIDI output port not available");
        throw MidiConnectionException("MIDI output port not available");
    }
}

