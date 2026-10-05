#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

#include "Core/Audio/AudioDevicePreferredSetup.h"
#include "Core/Audio/AudioDeviceProfiles.h"

namespace AudioDeviceSetupSync
{
    bool deviceSupportsSampleRate(juce::AudioIODevice* device, double sampleRate);
    bool deviceSupportsBufferSize(juce::AudioIODevice* device, int bufferSize);

    Core::AudioDeviceIdentity identityFromSetup(const juce::AudioDeviceManager::AudioDeviceSetup& setup);
    Core::AudioDeviceCapabilities capabilitiesFromDevice(juce::AudioIODevice* device);

    Core::AudioDeviceProfileKey profileKeyFromLiveSetup(
        const juce::AudioDeviceManager& deviceManager,
        const juce::AudioDeviceManager::AudioDeviceSetup& setup,
        juce::AudioIODevice* device);

    void captureLiveSetupAsProfile(const juce::AudioDeviceManager& deviceManager,
                                   const juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                   juce::AudioIODevice* device);

    /** Sync preferred setup + interface profiles after a live device-manager change. */
    struct SyncState
    {
        Core::AudioDevicePreferredSetup preferred;
        Core::AudioDeviceIdentity lastDeviceIdentity;
        bool restoringSetup = false;
        bool hasSeededDeviceIdentity = false;
    };

    void syncPreferredSetupFromDeviceManager(juce::AudioDeviceManager& deviceManager, SyncState& state);

    inline bool isAsioDeviceType(const juce::String& typeName) noexcept
    {
        return typeName.containsIgnoreCase("ASIO");
    }

    /** Under ASIO, keep both endpoint names equal (prefer the side the user just changed). */
    inline void linkAsioDeviceNames(juce::String& inputName, juce::String& outputName, bool preferOutput)
    {
        const auto preferred = preferOutput ? outputName : inputName;
        if (preferred.isEmpty())
        {
            inputName = {};
            outputName = {};
            return;
        }

        inputName = preferred;
        outputName = preferred;
    }
}
