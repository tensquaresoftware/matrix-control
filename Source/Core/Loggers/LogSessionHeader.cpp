#include "LogSessionHeader.h"

#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginVersion.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <mutex>
#include <utility>
#include <vector>

namespace LogSessionHeader
{
namespace
{
    std::atomic<std::uint32_t> nextInstanceId { 1 };
    std::atomic<std::uint32_t> reportingInstanceId { 0 };
    std::atomic<bool> audioSnapshotValid { false };
    std::atomic<double> audioSampleRateHz { 0.0 };
    std::atomic<int> audioBufferSizeSamples { 0 };
    std::mutex overrideMutex;
    juce::String formatOverride;
    juce::String hostOverride;

    juce::String wrapperFormatLabel()
    {
        using WT = juce::AudioProcessor::WrapperType;
        switch (juce::PluginHostType::getPluginLoadedAs())
        {
            case WT::wrapperType_Standalone: return "Standalone";
            case WT::wrapperType_AudioUnit:
            case WT::wrapperType_AudioUnitv3: return "AU";
            case WT::wrapperType_VST3: return "VST3";
            case WT::wrapperType_VST: return "VST";
            case WT::wrapperType_AAX: return "AAX";
            case WT::wrapperType_LV2: return "LV2";
            case WT::wrapperType_Unity: return "Unity";
            case WT::wrapperType_Undefined:
            default: return "Unknown";
        }
    }

    juce::String hostLabel()
    {
        // Format already says Standalone — Host is only meaningful inside a DAW.
        if (juce::PluginHostType::getPluginLoadedAs()
            == juce::AudioProcessor::wrapperType_Standalone)
        {
            return "n/a";
        }

        const juce::PluginHostType host;
        const juce::String description = host.getHostDescription();
        if (description.isNotEmpty())
            return description;

        return "Unknown";
    }

    juce::String cpuArchitectureLabel()
    {
#if JUCE_ARM
        return "arm64";
#elif JUCE_INTEL
 #if JUCE_64BIT
        return "x86_64";
 #else
        return "x86";
 #endif
#else
        return "unknown";
#endif
    }

    juce::String buildConfigLabel()
    {
#if JUCE_DEBUG
        return "Debug";
#else
        return "Release";
#endif
    }

    juce::String audioRateValue()
    {
        if (! audioSnapshotValid.load(std::memory_order_relaxed))
            return "n/a";

        const double hz = audioSampleRateHz.load(std::memory_order_relaxed);
        const auto whole = static_cast<juce::int64>(hz);
        const int decimals = (std::abs(hz - static_cast<double>(whole)) < 1.0e-9) ? 0 : 1;
        return juce::String(hz, decimals) + " Hz";
    }

    juce::String audioBufferValue()
    {
        if (! audioSnapshotValid.load(std::memory_order_relaxed))
            return "n/a";

        const int samples = audioBufferSizeSamples.load(std::memory_order_relaxed);
        return juce::String(samples) + " samples";
    }

    juce::String instanceValue()
    {
        const auto id = reportingInstanceId.load(std::memory_order_relaxed);
        if (id == 0)
            return "n/a";

        return juce::String(static_cast<juce::int64>(id));
    }

    juce::String formatAlignedFields(const std::vector<std::pair<juce::String, juce::String>>& fields)
    {
        int maxKeyLength = 0;
        for (const auto& field : fields)
            maxKeyLength = juce::jmax(maxKeyLength, field.first.length());

        juce::String block;
        for (const auto& field : fields)
        {
            block << field.first.paddedRight(' ', maxKeyLength)
                  << ": " << field.second << "\n";
        }
        return block;
    }
}

void setReportingInstanceId(std::uint32_t instanceId) noexcept
{
    reportingInstanceId.store(instanceId, std::memory_order_relaxed);
}

std::uint32_t allocateInstanceId() noexcept
{
    return nextInstanceId.fetch_add(1, std::memory_order_relaxed);
}

void setAudioSnapshot(double sampleRateHz, int bufferSizeSamples) noexcept
{
    audioSampleRateHz.store(sampleRateHz, std::memory_order_relaxed);
    audioBufferSizeSamples.store(bufferSizeSamples, std::memory_order_relaxed);
    audioSnapshotValid.store(sampleRateHz > 0.0 && bufferSizeSamples > 0,
                             std::memory_order_relaxed);
}

void clearAudioSnapshot() noexcept
{
    audioSnapshotValid.store(false, std::memory_order_relaxed);
    audioSampleRateHz.store(0.0, std::memory_order_relaxed);
    audioBufferSizeSamples.store(0, std::memory_order_relaxed);
}

void setNextHeaderFormatAndHost(const juce::String& formatLabel,
                                const juce::String& hostLabel)
{
    std::lock_guard<std::mutex> lock(overrideMutex);
    formatOverride = formatLabel;
    hostOverride = hostLabel;
}

juce::String buildMetadataBlock(const juce::File& logFile)
{
    juce::String formatLine;
    juce::String hostLine;
    {
        std::lock_guard<std::mutex> lock(overrideMutex);
        formatLine = formatOverride.isNotEmpty() ? formatOverride : wrapperFormatLabel();
        hostLine = hostOverride.isNotEmpty() ? hostOverride : hostLabel();
        formatOverride.clear();
        hostOverride.clear();
    }

    const juce::Time now = juce::Time::getCurrentTime();
    const std::vector<std::pair<juce::String, juce::String>> fields = {
        { "Organization", PluginDisplayNames::About::kOrganizationValue },
        { "Product", "Matrix-Control" },
        { "Version", PluginVersion::getVersionString() },
        { "Format", formatLine },
        { "Host", hostLine },
        { "OS", juce::SystemStats::getOperatingSystemName() + " / " + cpuArchitectureLabel() },
        { "Build", buildConfigLabel() },
        { "Sample rate", audioRateValue() },
        { "Buffer size", audioBufferValue() },
        { "Instance", instanceValue() },
        { "Date", now.toString(true, true, false, true) },
        { "Log file", logFile.getFileName() },
    };

    return formatAlignedFields(fields);
}
}
