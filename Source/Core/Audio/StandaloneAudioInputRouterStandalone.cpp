#include "Core/Audio/StandaloneAudioInputRouter.h"

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames()
    {
        juce::StringArray names;

        if (auto* holder = juce::StandalonePluginHolder::getInstance())
        {
            if (auto* device = holder->deviceManager.getCurrentAudioDevice())
            {
                const auto channelNames = device->getInputChannelNames();

                for (int i = 0; i < channelNames.size(); ++i)
                    names.add(channelNames[i]);
            }
        }

        return names;
    }

    juce::StringArray getInputChannelIds()
    {
        juce::StringArray ids;

        if (auto* holder = juce::StandalonePluginHolder::getInstance())
        {
            if (auto* device = holder->deviceManager.getCurrentAudioDevice())
            {
                for (int i = 0; i < device->getInputChannelNames().size(); ++i)
                    ids.add(juce::String(i));
            }
        }

        return ids;
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
                    const int channelIndex = sourceId.getIntValue();
                    const int numChannels = device->getInputChannelNames().size();

                    if (channelIndex >= 0 && channelIndex < numChannels)
                        setup.inputChannels.setBit(channelIndex);
                }

                holder->deviceManager.setAudioDeviceSetup(setup, true);
            }
        }
    }
}
