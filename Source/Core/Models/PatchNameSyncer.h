#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Core
{
    class PatchModel;

    class PatchNameSyncer
    {
    public:
        PatchNameSyncer(juce::AudioProcessorValueTreeState& apvts, PatchModel& model);

        void apvtsToBuffer();

        // Message thread only — ValueTree setProperty notifies listeners synchronously.
        void bufferToApvts();

    private:
        static juce::String truncateToMaxLength(const juce::String& name);

        juce::AudioProcessorValueTreeState& apvts_;
        PatchModel& model_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchNameSyncer)
    };

} // namespace Core
