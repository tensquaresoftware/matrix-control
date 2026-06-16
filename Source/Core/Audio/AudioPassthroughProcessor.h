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
        void process(const juce::AudioBuffer<float>& input,
                     juce::AudioBuffer<float>& output,
                     float gainLinear) noexcept;
        float getPeakLevel() const noexcept { return peakDisplay_.load(std::memory_order_relaxed); }

    private:
        int mapSourceChannel(int outputChannel) const noexcept;
        void updatePeakLevel(float blockPeak) noexcept;

        std::atomic<float> peakDisplay_{ 0.0f };
        std::atomic<int> channelMode_{ static_cast<int>(AudioFromChannelMode::kStereo) };
        std::atomic<int> monoSourceChannelIndex_{ 0 };

        int numInputChannels_ { 0 };
        int numOutputChannels_ { 0 };
        bool inputBusEnabled_ { false };
    };
}
