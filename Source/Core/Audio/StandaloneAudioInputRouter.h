#pragma once

#include <functional>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

#include "Core/Audio/AudioInputSourceCatalog.h"

namespace Core
{
    class StandaloneAudioInputRouter
    {
    public:
        using ShowAudioMidiSettingsHandler = std::function<void()>;

        static juce::StringArray getInputChannelNames();
        static juce::StringArray getInputChannelIds();
        static std::vector<AudioInputSourceEntry> getCatalogEntries();
        static juce::String getCurrentInputDeviceName();
        static bool applySceneAudioSafetyDefaultsIfNeeded();
        /** Force Input/Output to None when persisted devices are missing (no OS fallback keep). */
        static bool applyMissingAudioDeviceNonePolicy();
        static void addAudioDeviceChangeListener(juce::ChangeListener& listener);
        static void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
        static void enableInputMonitoring();
        static void disableInputMonitoring();
        static void setShowAudioMidiSettingsHandler(ShowAudioMidiSettingsHandler handler);
        static void clearShowAudioMidiSettingsHandler();
        static void showAudioMidiSettingsDialog();
    };
}
