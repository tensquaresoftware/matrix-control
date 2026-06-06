#include "Core/Audio/StandaloneAudioInputRouter.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    __attribute__((weak)) juce::StringArray getInputChannelNames()
    {
        return {};
    }

    __attribute__((weak)) juce::StringArray getInputChannelIds()
    {
        return {};
    }

    __attribute__((weak)) std::vector<Core::AudioInputSourceEntry> getCatalogEntries()
    {
        return {};
    }

    __attribute__((weak)) void addAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        juce::ignoreUnused(listener);
    }

    __attribute__((weak)) void removeAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        juce::ignoreUnused(listener);
    }

    __attribute__((weak)) void enableInputMonitoring()
    {
    }
}
