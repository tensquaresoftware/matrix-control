#pragma once

#include <juce_core/juce_core.h>

#include "Core/Audio/AudioPassthroughProcessor.h"

namespace Core
{
    /** Pure decision for Audio From source id → passthrough + optional channel-mode property. */
    struct AudioFromSourceSyncDecision
    {
        bool passthroughActive = false;
        bool shouldWriteChannelModeProperty = false;
        AudioFromChannelMode channelMode = AudioFromChannelMode::kStereo;
        int monoSourceChannelIndex = 0;
    };

    AudioFromSourceSyncDecision decideAudioFromSourceSync(const juce::String& sourceId) noexcept;

    void applyAudioFromSourceSync(AudioPassthroughProcessor& processor,
                                  const AudioFromSourceSyncDecision& decision) noexcept;
}
