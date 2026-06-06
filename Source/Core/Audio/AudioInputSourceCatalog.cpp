#include "Core/Audio/AudioInputSourceCatalog.h"

#include <juce_audio_devices/juce_audio_devices.h>
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    using Core::AudioInputSourceEntry;

    juce::String formatMonoDisplayName(const juce::String& deviceName, int channelIndex0Based)
    {
        return deviceName + " (" + juce::String(channelIndex0Based + 1) + ")";
    }

    juce::String formatStereoDisplayName(const juce::String& deviceName, int firstChannelIndex0Based)
    {
        return deviceName + " (" + juce::String(firstChannelIndex0Based + 1) + "/"
               + juce::String(firstChannelIndex0Based + 2) + ")";
    }

    juce::String makeMonoSourceId(int channelIndex0Based)
    {
        return "mono:" + juce::String(channelIndex0Based);
    }

    juce::String makeStereoSourceId(int firstChannelIndex0Based)
    {
        return "stereo:" + juce::String(firstChannelIndex0Based);
    }

    void appendEntriesForDevice(const juce::String& deviceName,
                                int numInputChannels,
                                std::vector<AudioInputSourceEntry>& entries)
    {
        if (deviceName.isEmpty() || numInputChannels <= 0)
            return;

        for (int channel = 0; channel < numInputChannels; ++channel)
        {
            AudioInputSourceEntry entry;
            entry.displayName = formatMonoDisplayName(deviceName, channel);
            entry.sourceId = makeMonoSourceId(channel);
            entries.push_back(std::move(entry));
        }

        for (int firstChannel = 0; firstChannel + 1 < numInputChannels; firstChannel += 2)
        {
            AudioInputSourceEntry entry;
            entry.displayName = formatStereoDisplayName(deviceName, firstChannel);
            entry.sourceId = makeStereoSourceId(firstChannel);
            entries.push_back(std::move(entry));
        }
    }

    void appendEntriesForActiveChannels(const juce::String& deviceName,
                                        const juce::BigInteger& activeInputChannels,
                                        int numInputChannels,
                                        std::vector<AudioInputSourceEntry>& entries)
    {
        if (deviceName.isEmpty() || numInputChannels <= 0)
            return;

        for (int channel = 0; channel < numInputChannels; ++channel)
        {
            if (!activeInputChannels[channel])
                continue;

            AudioInputSourceEntry entry;
            entry.displayName = formatMonoDisplayName(deviceName, channel);
            entry.sourceId = makeMonoSourceId(channel);
            entries.push_back(std::move(entry));
        }

        for (int firstChannel = 0; firstChannel + 1 < numInputChannels; ++firstChannel)
        {
            if (!activeInputChannels[firstChannel] || !activeInputChannels[firstChannel + 1])
                continue;

            AudioInputSourceEntry entry;
            entry.displayName = formatStereoDisplayName(deviceName, firstChannel);
            entry.sourceId = makeStereoSourceId(firstChannel);
            entries.push_back(std::move(entry));
        }
    }

}

namespace Core
{
    std::vector<AudioInputSourceEntry> AudioInputSourceCatalog::buildEntriesForDevice(const juce::String& deviceName,
                                                                                       int numInputChannels)
    {
        std::vector<AudioInputSourceEntry> entries;
        appendEntriesForDevice(deviceName, numInputChannels, entries);
        return entries;
    }

    std::vector<AudioInputSourceEntry> AudioInputSourceCatalog::buildFromSystemScan()
    {
        std::vector<AudioInputSourceEntry> entries;

        juce::OwnedArray<juce::AudioIODeviceType> deviceTypes;
        juce::AudioDeviceManager deviceManager;
        deviceManager.createAudioDeviceTypes(deviceTypes);

        for (auto* deviceType : deviceTypes)
        {
            if (deviceType == nullptr)
                continue;

            deviceType->scanForDevices();

            const auto inputDeviceNames = deviceType->getDeviceNames(true);

            for (const auto& inputDeviceName : inputDeviceNames)
            {
                std::unique_ptr<juce::AudioIODevice> device(
                    deviceType->createDevice({}, inputDeviceName));

                if (device == nullptr)
                    continue;

                appendEntriesForDevice(inputDeviceName,
                                       device->getInputChannelNames().size(),
                                       entries);
            }
        }

        return entries;
    }

    std::vector<AudioInputSourceEntry> AudioInputSourceCatalog::buildEntriesForActiveChannels(
        const juce::String& deviceName,
        const juce::BigInteger& activeInputChannels,
        int numInputChannels)
    {
        std::vector<AudioInputSourceEntry> entries;
        appendEntriesForActiveChannels(deviceName, activeInputChannels, numInputChannels, entries);
        return entries;
    }

    std::vector<AudioInputSourceEntry> AudioInputSourceCatalog::buildForProcessor(bool isStandalone)
    {
        if (!isStandalone)
            return {};

        auto entries = StandaloneAudioInputRouter::getCatalogEntries();

        if (!entries.empty())
            return entries;

        return {};
    }

    int AudioInputSourceCatalog::channelModeForSourceId(const juce::String& sourceId) noexcept
    {
        if (sourceId.startsWith("stereo:"))
            return 0;

        if (sourceId.startsWith("mono:"))
        {
            const int channelIndex = sourceId.fromFirstOccurrenceOf(":", false, false).getIntValue();

            if (channelIndex <= 0)
                return 1;

            if (channelIndex == 1)
                return 2;

            return 1;
        }

        if (sourceId.isNotEmpty() && sourceId.containsOnly("0123456789"))
            return sourceId.getIntValue() <= 0 ? 1 : 2;

        return 0;
    }
}
