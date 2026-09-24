#pragma once

#include <atomic>
#include <cmath>

#include <juce_audio_basics/juce_audio_basics.h>

namespace Core
{
    enum class AudioFromChannelMode : int
    {
        kStereo = 0,
        kMonoLeft = 1,
        kMonoRight = 2
    };

    /// Copies/mixes plugin input bus → output bus with gain and peak metering (FR-8).
    /// Publishes instantaneous post-gain block peak; visual release lives in PeakIndicator.
    /// Must only be called from the audio thread (processBlock).
    class AudioPassthroughProcessor
    {
    public:
        void prepare(int numInputChannels, int numOutputChannels, bool inputBusEnabled, double sampleRate) noexcept;
        void updateChannelLayout(int numInputChannels, int numOutputChannels, bool inputBusEnabled) noexcept;
        void setChannelMode(AudioFromChannelMode mode) noexcept;
        void setMonoSourceChannelIndex(int channelIndex) noexcept;
        /** When false (empty audioFromSourceId / NO INPUT), clear outputs and peak without
            touching JUCE standalone device mute. */
        void setPassthroughActive(bool active) noexcept;
        void process(const juce::AudioBuffer<float>& input,
                     juce::AudioBuffer<float>& output,
                     float gainLinear) noexcept;
        float getPeakLevel() const noexcept { return peakDisplay_.load(std::memory_order_relaxed); }

    private:
        struct ProcessBuffers
        {
            const juce::AudioBuffer<float>& input;
            juce::AudioBuffer<float>& output;
            int numSamples = 0;
            int numInputChannelsAvailable = 0;
            int numOutputChannelsToProcess = 0;
        };

        int mapSourceChannel(int outputChannel) const noexcept;
        void updatePeakLevel(float blockPeak) noexcept;
        bool shouldDuplicateMono() const noexcept;
        void clearAllOutputChannels(juce::AudioBuffer<float>& output, int numSamples) const noexcept;
        void clearTrailingOutputChannels(juce::AudioBuffer<float>& output,
                                         int firstChannel,
                                         int numSamples) const noexcept;
        static float applyGainAndTrackPeak(float sample, float gainLinear, float& blockPeak) noexcept;
        float processMonoDuplicate(const ProcessBuffers& buffers, float gainLinear) noexcept;
        float processMappedChannels(const ProcessBuffers& buffers, float gainLinear) noexcept;

        std::atomic<float> peakDisplay_{ 0.0f };
        std::atomic<int> channelMode_{ static_cast<int>(AudioFromChannelMode::kStereo) };
        std::atomic<int> monoSourceChannelIndex_{ 0 };
        std::atomic<bool> passthroughActive_{ true };

        int numInputChannels_ { 0 };
        int numOutputChannels_ { 0 };
        bool inputBusEnabled_ { false };
    };
}
