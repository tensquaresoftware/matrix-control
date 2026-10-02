#pragma once

#include <functional>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/Audio/AudioInputSourceCatalog.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames();
    juce::StringArray getInputChannelIds();
    std::vector<Core::AudioInputSourceEntry> getCatalogEntries();
    juce::String getCurrentInputDeviceName();
    bool applySceneAudioSafetyDefaultsIfNeeded();
    void addAudioDeviceChangeListener(juce::ChangeListener& listener);
    void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
    void enableInputMonitoring();
    void disableInputMonitoring();
    void setShowAudioMidiSettingsHandler(std::function<void()> handler);
    void clearShowAudioMidiSettingsHandler();
    void showAudioMidiSettingsDialog();
}
