// Extracted from PluginProcessor.cpp for modular maintenance.
// Audio passthrough, input gain, Audio From routing and hardware latency.

#include <cmath>

#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Core/Audio/AudioInputSourceCatalog.h"
#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/DeviceAudioInputPreference.h"
#include "Core/Audio/HardwareLatency.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PluginProcessorInternal;

float PluginProcessor::dbToLinearGain(float gainDb) noexcept
{
    if (gainDb <= PluginAudioConstants::kSilenceInputGainDb)
        return 0.0f;

    return std::pow(10.0f, gainDb / 20.0f);
}

bool PluginProcessor::getInstrumentPathEnabled(const juce::MidiBuffer& midiMessages) const
{
    if (isStandaloneWrapper())
    {
        const auto property = apvts.state.getProperty("keyboardFromEnabled", false);
        return property.isBool() && static_cast<bool>(property);
    }

    return midiMessages.getNumEvents() > 0;
}

void PluginProcessor::ensureAudioInputBusEnabled()
{
    if (getBusCount(true) <= 0)
        return;

    enableAllBuses();
}

int PluginProcessor::getAudioFromInputChannelCount() const noexcept
{
    if (getBusCount(true) <= 0)
        return 0;

    return getChannelCountOfBus(true, 0);
}

int PluginProcessor::getMainOutputChannelCount() const noexcept
{
    if (getBusCount(false) <= 0)
        return 0;

    return getChannelCountOfBus(false, 0);
}

void PluginProcessor::refreshAudioPassthroughLayout(double sampleRate)
{
    const bool inputEnabled = getTotalNumInputChannels() > 0
                              && getAudioFromInputChannelCount() > 0;
    audioPassthroughProcessor_->prepare(getAudioFromInputChannelCount(),
                                        getMainOutputChannelCount(),
                                        inputEnabled,
                                        sampleRate);
}

void PluginProcessor::syncAudioRuntimeFromState()
{
    const auto gainDb = static_cast<float>(apvts.state.getProperty("inputGainDb", 0.0f));
    const float sanitizedDb = std::isfinite(gainDb) ? gainDb : 0.0f;
    const float snappedDb = PluginAudioConstants::snapInputGainDb(sanitizedDb);
    inputGainLinear_.store(dbToLinearGain(snappedDb), std::memory_order_relaxed);

    const auto sourceId = apvts.state.getProperty("audioFromSourceId", juce::String()).toString();

    syncAudioPassthroughFromSourceId(sourceId);
}

void PluginProcessor::syncAudioPassthroughFromSourceId(const juce::String& sourceId)
{
    const bool sourceSelected = sourceId.isNotEmpty();

    if (sourceSelected)
    {
        const int channelMode = Core::AudioInputSourceCatalog::channelModeForSourceId(sourceId);
        audioPassthroughProcessor_->setChannelMode(static_cast<Core::AudioFromChannelMode>(channelMode));
        audioPassthroughProcessor_->setMonoSourceChannelIndex(
            Core::AudioInputSourceCatalog::monoChannelIndexForSourceId(sourceId));
        // Arm routing before re-enabling so the audio thread never sees stale maps.
        audioPassthroughProcessor_->setPassthroughActive(true);
    }
    else
    {
        // Silence first so a late audio block cannot use the previous route.
        audioPassthroughProcessor_->setPassthroughActive(false);
    }

    if (! isStandaloneWrapper())
        return;

    // JUCE standalone defaults muteInput on (feedback protection) and shows a sticky
    // banner. Clear mute only when a real Audio From source is applied. NO INPUT must
    // not call disableInputMonitoring — that is software silence only.
    if (! sourceSelected)
        return;

    runSyncOnMessageThread([]
    {
        Core::StandaloneAudioInputRouter::enableInputMonitoring();
    });
}

void PluginProcessor::setInputGainDb(float gainDb)
{
    const float snappedDb = PluginAudioConstants::snapInputGainDb(gainDb);
    inputGainLinear_.store(dbToLinearGain(snappedDb), std::memory_order_relaxed);
    apvts.state.setProperty("inputGainDb", snappedDb, nullptr);
}

void PluginProcessor::setHardwareLatencyMs(float latencyMs)
{
    const float quantizedMs = Core::HardwareLatency::quantizeMs(latencyMs);
    apvts.state.setProperty(PluginIDs::Settings::kHardwareLatencyMs, quantizedMs, nullptr);
    applyHardwareLatencyToHost();
    notifyNonParameterStateChanged();
}

float PluginProcessor::getHardwareLatencyMs() const
{
    const auto property = apvts.state.getProperty(PluginIDs::Settings::kHardwareLatencyMs, 0.0f);
    return Core::HardwareLatency::quantizeMs(static_cast<float>(property));
}

void PluginProcessor::syncHardwareLatencyFromState()
{
    const auto property = apvts.state.getProperty(PluginIDs::Settings::kHardwareLatencyMs, Core::HardwareLatency::kMinMs);
    const float quantizedMs = Core::HardwareLatency::quantizeMs(static_cast<float>(property));

    if (! juce::approximatelyEqual(static_cast<float>(property), quantizedMs))
        apvts.state.setProperty(PluginIDs::Settings::kHardwareLatencyMs, quantizedMs, nullptr);

    applyHardwareLatencyToHost();
}

void PluginProcessor::applyHardwareLatencyToHost()
{
    const double sampleRate = audioPassthroughSampleRate_ > 0.0 ? audioPassthroughSampleRate_ : 44100.0;
    const float latencyMs = getHardwareLatencyMs();
    const int latencySamples = Core::HardwareLatency::msToSamples(latencyMs, sampleRate);
    setLatencySamples(latencySamples);
    updateHostDisplay();
}

void PluginProcessor::setAudioFromChannelMode(int mode)
{
    const int clampedMode = juce::jlimit(0, 2, mode);
    audioPassthroughProcessor_->setChannelMode(static_cast<Core::AudioFromChannelMode>(clampedMode));
    apvts.state.setProperty("audioFromChannelMode", clampedMode, nullptr);
}

void PluginProcessor::setAudioFromSourceId(const juce::String& sourceId)
{
    apvts.state.setProperty("audioFromSourceId", sourceId, nullptr);
    syncAudioPassthroughFromSourceId(sourceId);

    if (sourceId.isEmpty())
        return;

    const int channelMode = Core::AudioInputSourceCatalog::channelModeForSourceId(sourceId);
    apvts.state.setProperty("audioFromChannelMode", channelMode, nullptr);
}

juce::StringArray PluginProcessor::getAudioInputSourceNames() const
{
    juce::StringArray names;
    const auto entries = Core::AudioInputSourceCatalog::buildForProcessor(isStandaloneWrapper());

    for (const auto& entry : entries)
        names.add(entry.displayName);

    return names;
}

juce::StringArray PluginProcessor::getAudioInputSourceIds() const
{
    juce::StringArray ids;
    const auto entries = Core::AudioInputSourceCatalog::buildForProcessor(isStandaloneWrapper());

    for (const auto& entry : entries)
        ids.add(entry.sourceId);

    return ids;
}

void PluginProcessor::initializeAudioProperties()
{
    if (!apvts.state.hasProperty("inputGainDb"))
        apvts.state.setProperty("inputGainDb", 0.0f, nullptr);

    if (!apvts.state.hasProperty("audioFromChannelMode"))
        apvts.state.setProperty("audioFromChannelMode", 0, nullptr);

    if (!apvts.state.hasProperty("audioFromSourceId"))
        apvts.state.setProperty("audioFromSourceId", juce::String(), nullptr);

    const auto savedGainDb = static_cast<float>(apvts.state.getProperty("inputGainDb", 0.0f));
    setInputGainDb(savedGainDb);

    auto savedSourceId = apvts.state.getProperty("audioFromSourceId", juce::String()).toString();

    if (savedSourceId.isEmpty())
    {
        const auto savedChannelMode = static_cast<int>(apvts.state.getProperty("audioFromChannelMode", 0));

        switch (savedChannelMode)
        {
            case 1: savedSourceId = "mono:0"; break;
            case 2: savedSourceId = "mono:1"; break;
            case 0:
            default: savedSourceId = "stereo:0"; break;
        }
    }

    if (savedSourceId.isNotEmpty())
        setAudioFromSourceId(savedSourceId);
}

void PluginProcessor::applyPreferredStandaloneAudioFromForDeviceType()
{
    if (! isStandaloneWrapper())
        return;

    const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const auto kind = Core::preferredAudioFromKind(deviceType, deviceDetected);

    if (kind == Core::PreferredAudioFromKind::kNone)
        return;

    const auto catalogIds = getAudioInputSourceIds();
    const auto currentSourceId = apvts.state.getProperty("audioFromSourceId", juce::String()).toString();
    const juce::String prefix = (kind == Core::PreferredAudioFromKind::kMono) ? "mono:" : "stereo:";

    if (currentSourceId.startsWith(prefix) && catalogIds.contains(currentSourceId))
        return;

    const auto preferredSourceId = Core::pickPreferredAudioFromSourceId(
        deviceType,
        deviceDetected,
        catalogIds);

    if (preferredSourceId.isEmpty())
        return;

    setAudioFromSourceId(preferredSourceId);
}
