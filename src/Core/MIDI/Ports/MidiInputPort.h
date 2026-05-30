#pragma once

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

class MidiInputPort
{
public:
    MidiInputPort();
    ~MidiInputPort();

    bool openPort(const juce::String& deviceId, juce::MidiInputCallback* callback = nullptr);
    void closePort();
    bool isOpen() const noexcept;
    juce::MidiInput* getMidiInput() const noexcept;

private:
    std::unique_ptr<juce::MidiInput> midiInput;
    bool portIsOpen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiInputPort)
};

