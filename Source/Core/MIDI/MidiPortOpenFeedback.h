#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

namespace Core
{
enum class MidiPortOpenFailureReason
{
    kNone,
    kNotFound,
    kOpenRejected
};

struct MidiPortOpenResult
{
    bool succeeded() const noexcept { return failureReason == MidiPortOpenFailureReason::kNone; }

    MidiPortOpenFailureReason failureReason = MidiPortOpenFailureReason::kNone;
    juce::String portDisplayName;
};

namespace MidiPortOpenFeedback
{
juce::String failureReasonCode(MidiPortOpenFailureReason reason);

juce::String formatFooterMessage(bool isInput,
                                 const juce::String& portDisplayName,
                                 MidiPortOpenFailureReason reason);

void logOpenFailure(bool isInput,
                    const juce::String& portDisplayName,
                    const juce::String& deviceId,
                    MidiPortOpenFailureReason reason);

void propagateOpenFailure(juce::AudioProcessorValueTreeState& apvts,
                          bool isInput,
                          const juce::String& portDisplayName,
                          MidiPortOpenFailureReason reason);
}
}
