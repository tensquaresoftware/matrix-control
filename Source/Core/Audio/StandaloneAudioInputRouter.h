#pragma once

#include <vector>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/Audio/AudioInputSourceCatalog.h"

namespace Core
{
    class StandaloneAudioInputRouter
    {
    public:
        static juce::StringArray getInputChannelNames();
        static juce::StringArray getInputChannelIds();
        static void applySourceId(const juce::String& sourceId);
        static std::vector<AudioInputSourceEntry> getCatalogEntries();
        static void addAudioDeviceChangeListener(juce::ChangeListener& listener);
        static void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
    };
}
