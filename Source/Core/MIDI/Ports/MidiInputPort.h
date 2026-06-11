#pragma once

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

#include "Core/MIDI/MidiPortOpenFeedback.h"

class MidiInputPort
{
public:
    MidiInputPort();
    ~MidiInputPort();

    Core::MidiPortOpenResult openPort(const juce::String& deviceId,
                                      juce::MidiInputCallback* callback = nullptr);
    void closePort();
    bool isOpen() const noexcept;
    bool isOpenWithDevice(const juce::String& deviceId) const noexcept;
    juce::String getOpenDeviceId() const noexcept;
    juce::MidiInput* getMidiInput() const noexcept;

private:
    std::unique_ptr<juce::MidiInput> midiInput;
    bool portIsOpen;
    juce::String openDeviceId_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiInputPort)
};
