#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/Audio/StandaloneAudioInputRouterDetail.h"

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

    void StandaloneAudioInputRouter::showAudioMidiSettingsDialog()
    {
        StandaloneAudioInputRouterDetail::showAudioMidiSettingsDialog();
    }
}
