#pragma once

#include <atomic>
#include <cstdint>

#include <juce_core/juce_core.h>

// Shared session banner for MidiLogger + ApvtsLogger (support / diagnostic files).
namespace LogSessionHeader
{
    void setReportingInstanceId(std::uint32_t instanceId) noexcept;
    std::uint32_t allocateInstanceId() noexcept;
    void setAudioSnapshot(double sampleRateHz, int bufferSizeSamples) noexcept;
    void clearAudioSnapshot() noexcept;

    // Optional one-shot overrides (sample layout dumps / tests). Cleared after buildMetadataBlock.
    void setNextHeaderFormatAndHost(const juce::String& formatLabel,
                                    const juce::String& hostLabel);

    // Key/value lines between the title banner and the separator (no trailing blank line).
    juce::String buildMetadataBlock(const juce::File& logFile);
}
