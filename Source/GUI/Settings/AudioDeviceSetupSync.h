#pragma once

#include <juce_audio_devices/juce_audio_devices.h>

#include "Core/Audio/AudioDevicePreferredSetup.h"
#include "Core/Audio/AudioDeviceProfiles.h"
#include "GUI/Widgets/RadioButtonGroupLayout.h"

namespace AudioDeviceSetupSync
{
    bool deviceSupportsSampleRate(juce::AudioIODevice* device, double sampleRate);
    bool deviceSupportsBufferSize(juce::AudioIODevice* device, int bufferSize);

    /**
        Try opening `setup` with each buffer in `bufferTryOrder` until setAudioDeviceSetup
        succeeds and the live device sample rate matches `setup.sampleRate`.
        Returns true when a try succeeds; does not restore a prior setup on failure.
    */
    bool tryApplySetupWithBufferFallback(juce::AudioDeviceManager& deviceManager,
                                         juce::AudioDeviceManager::AudioDeviceSetup setup,
                                         const juce::Array<int>& bufferTryOrder);

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

    /** Resolve input/output names for a UI apply (ASIO link, or leave independent). */
    inline void resolveEndpointNamesForApply(const juce::String& deviceTypeName,
                                             juce::String& inputName,
                                             juce::String& outputName,
                                             bool preferOutput) noexcept
    {
        if (isAsioDeviceType(deviceTypeName))
            linkAsioDeviceNames(inputName, outputName, preferOutput);
    }

    /** Write an exclusive stereo-pair mask and clear the matching useDefault* flag. */
    inline void applyStereoPairToSetup(juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                       bool isInput,
                                       int pairIndex)
    {
        const auto mask = TSS::RadioButtonGroupLayout::stereoPairMask(pairIndex);
        if (isInput)
        {
            setup.inputChannels = mask;
            setup.useDefaultInputChannels = false;
            return;
        }

        setup.outputChannels = mask;
        setup.useDefaultOutputChannels = false;
    }
}
