#include "Core/Audio/StandaloneAudioInputRouter.h"

#include "Core/Audio/AudioInputSourceCatalog.h"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace Core::StandaloneAudioInputRouterDetail
{
    namespace
    {
        int parseChannelIndex(const juce::String& sourceId)
        {
            if (sourceId.startsWith("mono:") || sourceId.startsWith("stereo:"))
                return sourceId.fromFirstOccurrenceOf(":", false, false).getIntValue();

            if (sourceId.isNotEmpty() && sourceId.containsOnly("0123456789"))
                return sourceId.getIntValue();

            return -1;
        }

        bool isStereoSourceId(const juce::String& sourceId)
        {
            return sourceId.startsWith("stereo:");
        }

        std::vector<Core::AudioInputSourceEntry> buildActiveDeviceCatalogEntries()
        {
            if (auto* holder = juce::StandalonePluginHolder::getInstance())
            {
                if (auto* device = holder->deviceManager.getCurrentAudioDevice())
                {
                    return Core::AudioInputSourceCatalog::buildEntriesForDevice(
                        device->getName(),
                        device->getInputChannelNames().size());
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

    void applySourceId(const juce::String& sourceId)
    {
        if (auto* holder = juce::StandalonePluginHolder::getInstance())
        {
            if (auto* device = holder->deviceManager.getCurrentAudioDevice())
            {
                auto setup = holder->deviceManager.getAudioDeviceSetup();
                setup.inputChannels.clear();

                if (sourceId.isNotEmpty())
                {
                    const int channelIndex = parseChannelIndex(sourceId);
                    const int numChannels = device->getInputChannelNames().size();

                    if (channelIndex >= 0 && channelIndex < numChannels)
                    {
                        setup.inputChannels.setBit(channelIndex);

                        if (isStereoSourceId(sourceId) && channelIndex + 1 < numChannels)
                            setup.inputChannels.setBit(channelIndex + 1);
                    }
                }

                holder->deviceManager.setAudioDeviceSetup(setup, true);
            }
        }
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
}
