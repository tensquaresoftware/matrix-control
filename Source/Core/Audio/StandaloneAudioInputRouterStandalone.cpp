#include "Core/Audio/StandaloneAudioInputRouterDetail.h"

#include "Core/Audio/AudioInputSourceCatalog.h"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace Core::StandaloneAudioInputRouterDetail
{
    namespace
    {
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
}
