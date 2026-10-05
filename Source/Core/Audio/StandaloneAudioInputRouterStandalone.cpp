#include "Core/Audio/StandaloneAudioInputRouterDetail.h"

#include "Core/Audio/AudioDeviceProfiles.h"
#include "Core/Audio/AudioInputSourceCatalog.h"
#include "Core/Audio/SceneAudioSafety.h"
#include "Core/PluginProcessor.h"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>
#include <juce_data_structures/juce_data_structures.h>

namespace Core::StandaloneAudioInputRouterDetail
{
    namespace
    {
        std::function<void()>& showAudioMidiSettingsHandler()
        {
            static std::function<void()> handler;
            return handler;
        }

        std::vector<Core::AudioInputSourceEntry> buildActiveDeviceCatalogEntries()
        {
            if (auto* holder = juce::StandalonePluginHolder::getInstance())
            {
                if (auto* device = holder->deviceManager.getCurrentAudioDevice())
                {
                    const auto setup = holder->deviceManager.getAudioDeviceSetup();
                    const int numChannels = device->getInputChannelNames().size();
                    // Prefer JUCE Input endpoint name so catalog labels match Audio Settings
                    // (macOS often names the opened device after Speakers while Input is Mic).
                    const auto inputIdentity = setup.inputDeviceName.isNotEmpty()
                                                   ? setup.inputDeviceName
                                                   : device->getName();

                    return Core::AudioInputSourceCatalog::buildEntriesForActiveChannels(
                        inputIdentity,
                        setup.inputChannels,
                        numChannels);
                }
            }

            return {};
        }

        juce::StringArray collectAvailableDeviceNames(juce::AudioDeviceManager& deviceManager,
                                                      bool wantInputDevices)
        {
            juce::StringArray names;

            const auto currentTypeName = deviceManager.getCurrentAudioDeviceType();
            if (currentTypeName.isEmpty())
                return names;

            for (auto* type : deviceManager.getAvailableDeviceTypes())
            {
                if (type == nullptr || type->getTypeName() != currentTypeName)
                    continue;

                type->scanForDevices();
                names.addArray(type->getDeviceNames(wantInputDevices));
            }

            names.removeDuplicates(false);
            return names;
        }

        bool clearEndpointToNone(juce::AudioDeviceManager::AudioDeviceSetup& setup, bool clearInput)
        {
            if (clearInput)
            {
                if (setup.inputDeviceName.isEmpty() && setup.inputChannels.isZero())
                    return false;

                setup.inputDeviceName = {};
                setup.inputChannels.clear();
                setup.useDefaultInputChannels = false;
                return true;
            }

            if (setup.outputDeviceName.isEmpty() && setup.outputChannels.isZero())
                return false;

            setup.outputDeviceName = {};
            setup.outputChannels.clear();
            setup.useDefaultOutputChannels = false;
            return true;
        }

        void persistHolderAudioSetup(juce::StandalonePluginHolder& holder)
        {
            holder.saveAudioDeviceState();

            if (auto* propertiesFile = dynamic_cast<juce::PropertiesFile*>(holder.settings.get()))
                propertiesFile->saveIfNeeded();
        }

        bool applyClearedEndpoints(juce::StandalonePluginHolder& holder,
                                   bool clearInput,
                                   bool clearOutput)
        {
            auto setup = holder.deviceManager.getAudioDeviceSetup();
            bool changed = false;

            if (clearInput)
                changed = clearEndpointToNone(setup, true) || changed;

            if (clearOutput)
                changed = clearEndpointToNone(setup, false) || changed;

            if (! changed)
                return false;

            if (holder.deviceManager.setAudioDeviceSetup(setup, true).isEmpty())
            {
                persistHolderAudioSetup(holder);
                return true;
            }

            if (! clearInput)
                return false;

            setup = holder.deviceManager.getAudioDeviceSetup();

            if (! clearEndpointToNone(setup, true))
                return false;

            if (holder.deviceManager.setAudioDeviceSetup(setup, true).isNotEmpty())
                return false;

            persistHolderAudioSetup(holder);
            return true;
        }
    }

    juce::StringArray getInputChannelNames()
    {
        juce::StringArray names;

        for (const auto& entry : buildActiveDeviceCatalogEntries())
            names.add(entry.displayName);

        return names;
    }

    juce::StringArray getInputChannelIds()
    {
        juce::StringArray ids;

        for (const auto& entry : buildActiveDeviceCatalogEntries())
            ids.add(entry.sourceId);

        return ids;
    }

    std::vector<Core::AudioInputSourceEntry> getCatalogEntries()
    {
        return buildActiveDeviceCatalogEntries();
    }

    juce::String getCurrentInputDeviceName()
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            return holder->deviceManager.getAudioDeviceSetup().inputDeviceName;

        return {};
    }

    bool applySceneAudioSafetyDefaultsIfNeeded()
    {
        auto* holder = juce::StandalonePluginHolder::getInstance();
        if (holder == nullptr || holder->settings == nullptr)
            return false;

        const bool alreadyApplied = holder->settings->getBoolValue(
            kSceneAudioSafetyDefaultsAppliedProperty, false);

        if (! shouldApplySceneAudioSafetyDefaults(alreadyApplied))
            return false;

        // Input + Output None without muteInput. Some platforms reject empty Output — fall back
        // to Input None only so the first-run flag can still stick.
        auto setup = holder->deviceManager.getAudioDeviceSetup();
        setup.inputDeviceName = {};
        setup.inputChannels.clear();
        setup.useDefaultInputChannels = false;
        setup.outputDeviceName = {};
        setup.outputChannels.clear();
        setup.useDefaultOutputChannels = false;

        auto setupError = holder->deviceManager.setAudioDeviceSetup(setup, true);

        if (setupError.isNotEmpty())
        {
            setup = holder->deviceManager.getAudioDeviceSetup();
            setup.inputDeviceName = {};
            setup.inputChannels.clear();
            setup.useDefaultInputChannels = false;
            setupError = holder->deviceManager.setAudioDeviceSetup(setup, true);

            if (setupError.isNotEmpty())
                return false;
        }

        holder->saveAudioDeviceState();
        holder->settings->setValue(kSceneAudioSafetyDefaultsAppliedProperty, true);

        if (auto* propertiesFile = dynamic_cast<juce::PropertiesFile*>(holder->settings.get()))
            propertiesFile->saveIfNeeded();

        return true;
    }

    bool applyMissingAudioDeviceNonePolicy()
    {
        auto* holder = juce::StandalonePluginHolder::getInstance();
        if (holder == nullptr || holder->settings == nullptr)
            return false;

        const auto availableInputs = collectAvailableDeviceNames(holder->deviceManager, true);
        const auto availableOutputs = collectAvailableDeviceNames(holder->deviceManager, false);
        if (areAudioDeviceNameListsStillIncomplete(availableInputs, availableOutputs))
            return false;

        const auto audioSetupXml = holder->settings->getXmlValue("audioSetup");
        const auto persisted = readPersistedAudioEndpoints(audioSetupXml.get());
        const bool clearInput = shouldApplyMissingDeviceNoneForEndpoint(availableInputs)
            && shouldForceAudioEndpointToNone(persisted.inputDeviceName, availableInputs);
        const bool clearOutput = shouldApplyMissingDeviceNoneForEndpoint(availableOutputs)
            && shouldForceAudioEndpointToNone(persisted.outputDeviceName, availableOutputs);

        if (! clearInput && ! clearOutput)
            return false;

        return applyClearedEndpoints(*holder, clearInput, clearOutput);
    }

    static juce::AudioDeviceManager::AudioDeviceSetup makeSetupFromProfile(
        const Core::AudioDeviceProfile& profile,
        juce::AudioDeviceManager::AudioDeviceSetup setup)
    {
        setup.inputDeviceName = profile.key.inputDeviceName;
        setup.outputDeviceName = profile.key.outputDeviceName;
        setup.useDefaultInputChannels = false;
        setup.useDefaultOutputChannels = false;
        return setup;
    }

    static Core::AudioDeviceCapabilities capabilitiesFromOpenedDevice(juce::AudioIODevice& device)
    {
        Core::AudioDeviceCapabilities capabilities;
        capabilities.availableInputChannelCount = device.getInputChannelNames().size();
        capabilities.availableOutputChannelCount = device.getOutputChannelNames().size();
        capabilities.sampleRates = device.getAvailableSampleRates();
        capabilities.bufferSizes = device.getAvailableBufferSizes();
        return capabilities;
    }

    static void applyValidatedProfileFields(juce::AudioDeviceManager::AudioDeviceSetup& setup,
                                     const Core::ValidatedAudioDeviceProfile& validated)
    {
        setup.inputChannels = validated.inputChannels;
        setup.outputChannels = validated.outputChannels;

        if (validated.applySampleRate)
            setup.sampleRate = validated.sampleRate;

        if (validated.applyBufferSize)
            setup.bufferSize = validated.bufferSize;
    }

    static void touchProfileLastUsed(juce::Array<Core::AudioDeviceProfile> profiles,
                              Core::AudioDeviceProfile profile)
    {
        profile.lastUsedUtcMs = juce::Time::currentTimeMillis();
        Core::saveAudioDeviceProfiles(Core::upsertProfileLru(std::move(profiles), profile));
    }

    static bool refineOpenedProfileSetup(juce::StandalonePluginHolder& holder,
                                  const Core::AudioDeviceProfile& profile,
                                  juce::Array<Core::AudioDeviceProfile> profiles)
    {
        auto& deviceManager = holder.deviceManager;
        auto setup = deviceManager.getAudioDeviceSetup();
        auto* device = deviceManager.getCurrentAudioDevice();
        if (device == nullptr)
            return false;

        const auto capabilities = capabilitiesFromOpenedDevice(*device);
        if (! Core::doesOpenedDeviceFingerprintMatchProfileKey(
                profile.key,
                capabilities.availableInputChannelCount,
                capabilities.availableOutputChannelCount))
        {
            persistHolderAudioSetup(holder);
            return true;
        }

        applyValidatedProfileFields(setup,
                                    Core::validateProfileAgainstCapabilities(profile, capabilities));

        if (deviceManager.setAudioDeviceSetup(setup, true).isEmpty())
            touchProfileLastUsed(std::move(profiles), profile);

        // Keep the opened interface even if the refined apply fails.
        persistHolderAudioSetup(holder);
        return true;
    }

    bool applyAvailableAudioDeviceProfileAtLaunch()
    {
        auto* holder = juce::StandalonePluginHolder::getInstance();
        if (holder == nullptr)
            return false;

        auto& deviceManager = holder->deviceManager;
        auto profiles = Core::loadAudioDeviceProfiles();
        if (profiles.isEmpty())
            return false;

        const int bestIndex = Core::findBestAvailableProfileIndex(
            profiles,
            deviceManager.getCurrentAudioDeviceType(),
            collectAvailableDeviceNames(deviceManager, true),
            collectAvailableDeviceNames(deviceManager, false));
        if (bestIndex < 0)
            return false;

        const auto profile = profiles.getReference(bestIndex);
        auto setup = makeSetupFromProfile(profile, deviceManager.getAudioDeviceSetup());
        if (deviceManager.setAudioDeviceSetup(setup, true).isNotEmpty())
            return false;

        if (auto* matrixProcessor = dynamic_cast<PluginProcessor*>(holder->processor.get()))
            matrixProcessor->setAudioFromSourceId({});

        return refineOpenedProfileSetup(*holder, profile, std::move(profiles));
    }

    class LaunchAudioDeviceProfileRestoreTimer final : public juce::Timer
    {
    public:
        void startSeries()
        {
            stopTimer();
            attemptIndex_ = 0;
            startTimer(delayMsForAttempt(0));
        }

    private:
        static constexpr int kMaxAttempts = 4;

        static int delayMsForAttempt(int attemptIndex) noexcept
        {
            constexpr int delaysMs[] = { 800, 1600, 3200, 5000 };
            return delaysMs[juce::jmin(attemptIndex, kMaxAttempts - 1)];
        }

        void timerCallback() override
        {
            stopTimer();

            if (applyAvailableAudioDeviceProfileAtLaunch())
                return;

            ++attemptIndex_;
            if (attemptIndex_ < kMaxAttempts)
                startTimer(delayMsForAttempt(attemptIndex_));
        }

        int attemptIndex_ = 0;
    };

    static LaunchAudioDeviceProfileRestoreTimer& launchProfileRestoreTimer()
    {
        static LaunchAudioDeviceProfileRestoreTimer timer;
        return timer;
    }

    void scheduleAvailableAudioDeviceProfileRestoreAtLaunch()
    {
        // Message-thread timer; safe to call from createPluginHolder on the message thread.
        launchProfileRestoreTimer().startSeries();
    }

    void addAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->deviceManager.addChangeListener(&listener);
    }

    void removeAudioDeviceChangeListener(juce::ChangeListener& listener)
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->deviceManager.removeChangeListener(&listener);
    }

    void enableInputMonitoring()
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->getMuteInputValue().setValue(false);
    }

    void disableInputMonitoring()
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->getMuteInputValue().setValue(true);
    }

    void setShowAudioMidiSettingsHandler(std::function<void()> handler)
    {
        showAudioMidiSettingsHandler() = std::move(handler);
    }

    void clearShowAudioMidiSettingsHandler()
    {
        showAudioMidiSettingsHandler() = {};
    }

    void showAudioMidiSettingsDialog()
    {
        // Prefer the Matrix-skinned editor overlay when registered.
        if (showAudioMidiSettingsHandler())
        {
            showAudioMidiSettingsHandler()();
            return;
        }

        // Fallback for early boot / missing editor: never open stock mute UI path.
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            holder->getMuteInputValue().setValue(false);
    }

    juce::AudioDeviceManager* getAudioDeviceManager()
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
            return &holder->deviceManager;

        return nullptr;
    }
}
