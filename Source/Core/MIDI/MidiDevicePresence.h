#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

namespace Core
{
    /** True when the JUCE MIDI identifier still maps to a live, online CoreMIDI endpoint
        (macOS/iOS). On other platforms, non-empty identifiers are treated as live (fail-open;
        no exclusive openDevice probe). */
    [[nodiscard]] bool isMidiInputIdentifierLive(const juce::String& identifier);
    [[nodiscard]] bool isMidiOutputIdentifierLive(const juce::String& identifier);

    /** Apple: drop offline CoreMIDI endpoints. Non-Apple: pass-through (fail-open). */
    [[nodiscard]] juce::Array<juce::MidiDeviceInfo> filterLiveMidiInputs(
        const juce::Array<juce::MidiDeviceInfo>& devices);
    [[nodiscard]] juce::Array<juce::MidiDeviceInfo> filterLiveMidiOutputs(
        const juce::Array<juce::MidiDeviceInfo>& devices);
}
