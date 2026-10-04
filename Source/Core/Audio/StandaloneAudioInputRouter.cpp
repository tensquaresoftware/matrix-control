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

    juce::String StandaloneAudioInputRouter::getCurrentInputDeviceName()
    {
        return StandaloneAudioInputRouterDetail::getCurrentInputDeviceName();
    }

    bool StandaloneAudioInputRouter::applySceneAudioSafetyDefaultsIfNeeded()
    {
        return StandaloneAudioInputRouterDetail::applySceneAudioSafetyDefaultsIfNeeded();
    }

    bool StandaloneAudioInputRouter::applyMissingAudioDeviceNonePolicy()
    {
        return StandaloneAudioInputRouterDetail::applyMissingAudioDeviceNonePolicy();
    }

    bool StandaloneAudioInputRouter::applyAvailableAudioDeviceProfileAtLaunch()
    {
        return StandaloneAudioInputRouterDetail::applyAvailableAudioDeviceProfileAtLaunch();
    }

    void StandaloneAudioInputRouter::scheduleAvailableAudioDeviceProfileRestoreAtLaunch()
    {
        StandaloneAudioInputRouterDetail::scheduleAvailableAudioDeviceProfileRestoreAtLaunch();
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

    void StandaloneAudioInputRouter::disableInputMonitoring()
    {
        StandaloneAudioInputRouterDetail::disableInputMonitoring();
    }

    void StandaloneAudioInputRouter::setShowAudioMidiSettingsHandler(ShowAudioMidiSettingsHandler handler)
    {
        StandaloneAudioInputRouterDetail::setShowAudioMidiSettingsHandler(std::move(handler));
    }

    void StandaloneAudioInputRouter::clearShowAudioMidiSettingsHandler()
    {
        StandaloneAudioInputRouterDetail::clearShowAudioMidiSettingsHandler();
    }

    void StandaloneAudioInputRouter::showAudioMidiSettingsDialog()
    {
        StandaloneAudioInputRouterDetail::showAudioMidiSettingsDialog();
    }
}
