#pragma once

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

class MidiOutputPort
{
public:
    MidiOutputPort();
    ~MidiOutputPort();

    bool openPort(const juce::String& deviceId);
    void closePort();
    bool isOpen() const noexcept;
    juce::MidiOutput* getMidiOutput() const noexcept;

private:
    std::unique_ptr<juce::MidiOutput> midiOutput;
    bool portIsOpen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiOutputPort)
};

