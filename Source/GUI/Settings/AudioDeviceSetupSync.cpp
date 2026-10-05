#include "AudioDeviceSetupSync.h"

namespace AudioDeviceSetupSync
{
    namespace
    {
        void applyValidatedProfileToSetup(juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                          const Core::ValidatedAudioDeviceProfile& validated)
        {
            setup.inputChannels = validated.inputChannels;
            setup.outputChannels = validated.outputChannels;

            if (validated.applySampleRate)
                setup.sampleRate = validated.sampleRate;

            if (validated.applyBufferSize)
                setup.bufferSize = validated.bufferSize;
        }

        bool tryRestoreMatchingProfile(const Core::AudioDeviceProfileKey& key,
                                       juce::AudioIODevice* device,
                                       juce::AudioDeviceManager::AudioDeviceSetup& candidateSetup)
        {
            if (device == nullptr)
                return false;

            const auto profiles = Core::loadAudioDeviceProfiles();
            const int index = Core::findProfileIndex(profiles, key);
            if (index < 0)
                return false;

            const auto validated = Core::validateProfileAgainstCapabilities(
                profiles.getReference(index),
                capabilitiesFromDevice(device));
            applyValidatedProfileToSetup(candidateSetup, validated);
            return true;
        }

        struct LiveSetupMutation
        {
            juce::AudioDeviceManager& deviceManager;
            juce::AudioDeviceManager::AudioDeviceSetup& setup;
            juce::AudioIODevice*& device;
            bool& restoringFlag;
        };

        bool applySetupWithRestoreGuard(LiveSetupMutation& live,
                                        const juce::AudioDeviceManager::AudioDeviceSetup& setup)
        {
            live.restoringFlag = true;
            const auto error = live.deviceManager.setAudioDeviceSetup(setup, true);
            live.restoringFlag = false;
            return error.isEmpty();
        }

        void refreshSetupAfterApply(LiveSetupMutation& live)
        {
            live.setup = live.deviceManager.getAudioDeviceSetup();
            live.device = live.deviceManager.getCurrentAudioDevice();
        }

        bool applyPreferredOverlayIfNeeded(LiveSetupMutation& live,
                                           const Core::PreferredSetupChangePlan& plan)
        {
            if (! plan.shouldRestore)
                return false;

            auto candidate = live.setup;
            candidate.sampleRate = plan.sampleRateToApply;
            candidate.bufferSize = plan.bufferSizeToApply;
            const bool applied = applySetupWithRestoreGuard(live, candidate);
            refreshSetupAfterApply(live);
            return applied;
        }

        struct ProfileIdentityRestoreResult
        {
            bool attempted = false;
            bool succeeded = false;
            bool hadExistingProfile = false;
        };

        struct ProfileIdentityRestoreInput
        {
            LiveSetupMutation& live;
            bool hasSeededIdentity = false;
            Core::AudioDeviceIdentity previousIdentity;
            Core::AudioDeviceIdentity currentIdentity;
            Core::AudioDevicePreferredSetup& preferred;
        };

        ProfileIdentityRestoreResult tryRestoreProfileOnIdentityChange(
            ProfileIdentityRestoreInput& input)
        {
            ProfileIdentityRestoreResult result;
            if (! Core::shouldRestoreAudioDeviceProfile(input.hasSeededIdentity,
                                                        input.previousIdentity,
                                                        input.currentIdentity))
            {
                return result;
            }

            result.attempted = true;
            const auto key = profileKeyFromLiveSetup(input.live.deviceManager,
                                                     input.live.setup,
                                                     input.live.device);
            result.hadExistingProfile = Core::findProfileIndex(Core::loadAudioDeviceProfiles(), key) >= 0;

            auto candidate = input.live.setup;
            if (! tryRestoreMatchingProfile(key, input.live.device, candidate))
                return result;

            if (applySetupWithRestoreGuard(input.live, candidate))
                result.succeeded = true;

            refreshSetupAfterApply(input.live);
            if (result.succeeded)
            {
                input.preferred = Core::capturePreferredSetup(input.preferred,
                                                              input.live.setup.sampleRate,
                                                              input.live.setup.bufferSize);
            }

            return result;
        }
    }

    bool deviceSupportsSampleRate(juce::AudioIODevice* device, double sampleRate)
    {
        if (device == nullptr || sampleRate <= 0.0)
            return false;

        for (const auto rate : device->getAvailableSampleRates())
        {
            if (juce::approximatelyEqual(rate, sampleRate))
                return true;
        }

        return false;
    }

    bool deviceSupportsBufferSize(juce::AudioIODevice* device, int bufferSize)
    {
        if (device == nullptr || bufferSize <= 0)
            return false;

        return device->getAvailableBufferSizes().contains(bufferSize);
    }

    Core::AudioDeviceIdentity identityFromSetup(const juce::AudioDeviceManager::AudioDeviceSetup& setup)
    {
        return { .outputDeviceName = setup.outputDeviceName, .inputDeviceName = setup.inputDeviceName };
    }

    Core::AudioDeviceCapabilities capabilitiesFromDevice(juce::AudioIODevice* device)
    {
        Core::AudioDeviceCapabilities capabilities;
        if (device == nullptr)
            return capabilities;

        capabilities.availableInputChannelCount = device->getInputChannelNames().size();
        capabilities.availableOutputChannelCount = device->getOutputChannelNames().size();
        capabilities.sampleRates = device->getAvailableSampleRates();
        capabilities.bufferSizes = device->getAvailableBufferSizes();
        return capabilities;
    }

    Core::AudioDeviceProfileKey profileKeyFromLiveSetup(
        const juce::AudioDeviceManager& deviceManager,
        const juce::AudioDeviceManager::AudioDeviceSetup& setup,
        juce::AudioIODevice* device)
    {
        const auto capabilities = capabilitiesFromDevice(device);
        return Core::buildProfileKey({
            .driverTypeName = deviceManager.getCurrentAudioDeviceType(),
            .inputDeviceName = setup.inputDeviceName,
            .outputDeviceName = setup.outputDeviceName,
            .availableInputChannelCount = capabilities.availableInputChannelCount,
            .availableOutputChannelCount = capabilities.availableOutputChannelCount,
        });
    }

    void captureLiveSetupAsProfile(const juce::AudioDeviceManager& deviceManager,
                                   const juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                   juce::AudioIODevice* device)
    {
        if (device == nullptr)
            return;

        const auto key = profileKeyFromLiveSetup(deviceManager, setup, device);
        if (! Core::shouldCaptureAudioDeviceProfile(key))
            return;

        Core::AudioDeviceProfile profile;
        profile.key = key;
        profile.inputChannels = setup.inputChannels;
        profile.outputChannels = setup.outputChannels;
        profile.sampleRate = setup.sampleRate;
        profile.bufferSize = setup.bufferSize;
        profile.lastUsedUtcMs = juce::Time::currentTimeMillis();

        auto profiles = Core::upsertProfileLru(Core::loadAudioDeviceProfiles(), profile);
        Core::saveAudioDeviceProfiles(profiles);
    }

    void syncPreferredSetupFromDeviceManager(juce::AudioDeviceManager& deviceManager, SyncState& state)
    {
        auto setup = deviceManager.getAudioDeviceSetup();
        auto* device = deviceManager.getCurrentAudioDevice();
        const auto currentIdentity = identityFromSetup(setup);
        LiveSetupMutation live { deviceManager, setup, device, state.restoringSetup };

        ProfileIdentityRestoreInput restoreInput { live,
                                                   state.hasSeededDeviceIdentity,
                                                   state.lastDeviceIdentity,
                                                   currentIdentity,
                                                   state.preferred };
        const bool hadSeededIdentity = state.hasSeededDeviceIdentity;
        const auto restore = tryRestoreProfileOnIdentityChange(restoreInput);
        if (! restore.succeeded)
        {
            const auto plan = Core::planPreferredSetupChange({
                .previousIdentity = state.lastDeviceIdentity,
                .currentIdentity = currentIdentity,
                .preferred = state.preferred,
                .liveSampleRate = setup.sampleRate,
                .liveBufferSize = setup.bufferSize,
                .preferredRateSupported = deviceSupportsSampleRate(device, state.preferred.sampleRate),
                .preferredBufferSupported = deviceSupportsBufferSize(device, state.preferred.bufferSize),
            });
            applyPreferredOverlayIfNeeded(live, plan);
            state.preferred = plan.preferredAfterCapture;
        }

        state.lastDeviceIdentity = currentIdentity;
        state.hasSeededDeviceIdentity = true;

        if (Core::shouldCaptureLiveSetupAsProfile({
                .hasSeededIdentity = hadSeededIdentity,
                .restoreAttempted = restore.attempted,
                .restoreSucceeded = restore.succeeded,
                .hadExistingProfileForKey = restore.hadExistingProfile,
                .liveKey = profileKeyFromLiveSetup(deviceManager, setup, device),
            }))
        {
            captureLiveSetupAsProfile(deviceManager, setup, device);
        }
    }

}
