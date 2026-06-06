#include "Core/Audio/StandaloneAudioInputRouter.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames();
    juce::StringArray getInputChannelIds();
    std::vector<Core::AudioInputSourceEntry> getCatalogEntries();
    void addAudioDeviceChangeListener(juce::ChangeListener& listener);
    void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
    void enableInputMonitoring();
}

namespace Core
{

    juce::StringArray StandaloneAudioInputRouter::getInputChannelNames()
    {
        return StandaloneAudioInputRouterDetail::getInputChannelNames();
    }

    juce::StringArray StandaloneAudioInputRouter::getInputChannelIds()
    {
        return StandaloneAudioInputRouterDetail::getInputChannelIds();
    }

    std::vector<AudioInputSourceEntry> StandaloneAudioInputRouter::getCatalogEntries()
    {
        return StandaloneAudioInputRouterDetail::getCatalogEntries();
    }

    void StandaloneAudioInputRouter::addAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        StandaloneAudioInputRouterDetail::addAudioDeviceChangeListener(listener);
    }

    void StandaloneAudioInputRouter::removeAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        StandaloneAudioInputRouterDetail::removeAudioDeviceChangeListener(listener);
    }

    void StandaloneAudioInputRouter::enableInputMonitoring()
    {
        StandaloneAudioInputRouterDetail::enableInputMonitoring();
    }
}
