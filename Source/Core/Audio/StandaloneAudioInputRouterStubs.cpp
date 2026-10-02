#include "Core/Audio/StandaloneAudioInputRouterDetail.h"

#include <functional>

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

    juce::String getCurrentInputDeviceName()
    {
        return {};
    }

    bool applySceneAudioSafetyDefaultsIfNeeded()
    {
        return false;
    }

    bool applyMissingAudioDeviceNonePolicy()
    {
        return false;
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

    void disableInputMonitoring()
    {
    }

    void setShowAudioMidiSettingsHandler(std::function<void()> handler)
    {
        juce::ignoreUnused(handler);
    }

    void clearShowAudioMidiSettingsHandler()
    {
    }

    void showAudioMidiSettingsDialog()
    {
    }
}
