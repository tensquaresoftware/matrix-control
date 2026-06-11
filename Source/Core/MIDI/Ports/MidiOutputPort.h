#pragma once

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

#include "Core/MIDI/MidiPortOpenFeedback.h"

class MidiOutputPort
{
public:
    MidiOutputPort();
    ~MidiOutputPort();

    Core::MidiPortOpenResult openPort(const juce::String& deviceId);
    void closePort();
    bool isOpen() const noexcept;
    bool isOpenWithDevice(const juce::String& deviceId) const noexcept;
    juce::String getOpenDeviceId() const noexcept;
    juce::MidiOutput* getMidiOutput() const noexcept;

private:
    std::unique_ptr<juce::MidiOutput> midiOutput;
    bool portIsOpen;
    juce::String openDeviceId_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiOutputPort)
};
