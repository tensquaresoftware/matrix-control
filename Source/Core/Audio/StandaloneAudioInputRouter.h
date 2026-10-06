#pragma once

#include <functional>
#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
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
        /** Standalone only; nullptr in plugin / when no holder. */
        static juce::AudioDeviceManager* getAudioDeviceManager();
        static bool applySceneAudioSafetyDefaultsIfNeeded();
        /** Force Input/Output to None when persisted devices are missing (no OS fallback keep). */
        static bool applyMissingAudioDeviceNonePolicy();
        /**
            After safety None policies: if a remembered interface is currently available,
            restore Input/Output + channels/rate/buffer from its disk profile.
            Does not invent AUDIO FROM from the profile, and must not clear a persisted
            APVTS selection — scene-safety sync decides keep vs clear after the setup settles.
        */
        static bool applyAvailableAudioDeviceProfileAtLaunch();
        /** Retry launch profile restore after device enumeration settles (message-thread timer). */
        static void scheduleAvailableAudioDeviceProfileRestoreAtLaunch();
        static void addAudioDeviceChangeListener(juce::ChangeListener& listener);
        static void removeAudioDeviceChangeListener(juce::ChangeListener& listener);
        static void enableInputMonitoring();
        static void disableInputMonitoring();
        static void setShowAudioMidiSettingsHandler(ShowAudioMidiSettingsHandler handler);
        static void clearShowAudioMidiSettingsHandler();
        static void showAudioMidiSettingsDialog();
    };
}
