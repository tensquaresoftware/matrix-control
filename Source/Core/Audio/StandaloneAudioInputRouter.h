#pragma once

#include <juce_core/juce_core.h>

namespace Core
{
    class StandaloneAudioInputRouter
    {
    public:
        static juce::StringArray getInputChannelNames();
        static juce::StringArray getInputChannelIds();
        static void applySourceId(const juce::String& sourceId);
    };
}
