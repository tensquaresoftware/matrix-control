#include "Core/Audio/AudioFromSourceSync.h"

#include "Core/Audio/AudioInputSourceCatalog.h"

namespace Core
{
    AudioFromSourceSyncDecision decideAudioFromSourceSync(const juce::String& sourceId) noexcept
    {
        AudioFromSourceSyncDecision decision;

        if (sourceId.isEmpty())
            return decision;

        decision.passthroughActive = true;
        decision.shouldWriteChannelModeProperty = true;
        decision.channelMode = static_cast<AudioFromChannelMode>(
            AudioInputSourceCatalog::channelModeForSourceId(sourceId));
        decision.monoSourceChannelIndex = AudioInputSourceCatalog::monoChannelIndexForSourceId(sourceId);
        return decision;
    }

    void applyAudioFromSourceSync(AudioPassthroughProcessor& processor,
                                  const AudioFromSourceSyncDecision& decision) noexcept
    {
        if (! decision.passthroughActive)
        {
            processor.setPassthroughActive(false);
            return;
        }

        processor.setChannelMode(decision.channelMode);
        processor.setMonoSourceChannelIndex(decision.monoSourceChannelIndex);
        // Arm routing before re-enabling so the audio thread never sees stale maps.
        processor.setPassthroughActive(true);
    }
}
