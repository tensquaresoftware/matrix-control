#pragma once

#include <vector>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/Audio/AudioInputSourceCatalog.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames();
    juce::StringArray getInputChannelIds();
    std::vector<Core::AudioInputSourceEntry> getCatalogEntries();
    void addAudioDeviceChangeListener(juce::ChangeListener& listener);
    void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
    void enableInputMonitoring();
}
