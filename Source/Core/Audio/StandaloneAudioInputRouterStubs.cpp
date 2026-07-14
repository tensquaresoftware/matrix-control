#include "Core/Audio/StandaloneAudioInputRouterDetail.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames()
    {
        return {};
    }

    juce::StringArray getInputChannelIds()
    {
        return {};
    }

    std::vector<Core::AudioInputSourceEntry> getCatalogEntries()
    {
        return {};
    }

    void addAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        juce::ignoreUnused(listener);
    }

    void removeAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        juce::ignoreUnused(listener);
    }

    void enableInputMonitoring()
    {
    }

    void showAudioMidiSettingsDialog()
    {
    }
}
