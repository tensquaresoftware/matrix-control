#include "Core/Audio/StandaloneAudioInputRouter.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames();
    juce::StringArray getInputChannelIds();
    void applySourceId(const juce::String& sourceId);
    std::vector<Core::AudioInputSourceEntry> getCatalogEntries();
    void addAudioDeviceChangeListener(juce::ChangeListener& listener);
    void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
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

    void StandaloneAudioInputRouter::applySourceId(const juce::String& sourceId)
    {
        StandaloneAudioInputRouterDetail::applySourceId(sourceId);
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
}
