#pragma once

#include <vector>

#include <juce_core/juce_core.h>

namespace Core
{
    struct AudioInputSourceEntry
    {
        juce::String displayName;
        juce::String sourceId;
    };

    class AudioInputSourceCatalog
    {
    public:
        static std::vector<AudioInputSourceEntry> buildFromSystemScan();
        static std::vector<AudioInputSourceEntry> buildForProcessor(bool isStandalone);
        static std::vector<AudioInputSourceEntry> buildEntriesForDevice(const juce::String& deviceName,
                                                                          int numInputChannels);
        static std::vector<AudioInputSourceEntry> buildEntriesForActiveChannels(
            const juce::String& deviceName,
            const juce::BigInteger& activeInputChannels,
            int numInputChannels);
        static int channelModeForSourceId(const juce::String& sourceId) noexcept;
        static int monoChannelIndexForSourceId(const juce::String& sourceId) noexcept;
    };
}
