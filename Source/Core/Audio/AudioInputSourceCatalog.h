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

        static int channelModeForSourceId(const juce::String& sourceId) noexcept;
    };
}
