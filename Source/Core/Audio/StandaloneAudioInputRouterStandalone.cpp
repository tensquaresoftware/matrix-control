#include "Core/Audio/StandaloneAudioInputRouterDetail.h"

#include "Core/Audio/AudioInputSourceCatalog.h"
#include "Core/Audio/SceneAudioSafety.h"

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

                    return Core::AudioInputSourceCatalog::buildEntriesForActiveChannels(
                        device->getName(),
                        setup.inputChannels,
                        numChannels);
                }
            }

            return {};
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

        // Input None without muteInput: empty device + cleared channels, banner stays off.
        auto setup = holder->deviceManager.getAudioDeviceSetup();
        setup.inputDeviceName = {};
        setup.inputChannels.clear();
        setup.useDefaultInputChannels = false;

        const auto setupError = holder->deviceManager.setAudioDeviceSetup(setup, true);

        if (setupError.isNotEmpty())
            return false;

        holder->saveAudioDeviceState();
        holder->settings->setValue(kSceneAudioSafetyDefaultsAppliedProperty, true);

        if (auto* propertiesFile = dynamic_cast<juce::PropertiesFile*>(holder->settings.get()))
            propertiesFile->saveIfNeeded();

        return true;
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
}
