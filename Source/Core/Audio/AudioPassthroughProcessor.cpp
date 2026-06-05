#include "Core/Audio/AudioPassthroughProcessor.h"

namespace Core
{
    void AudioPassthroughProcessor::prepare(int numInputChannels,
                                            int numOutputChannels,
                                            bool inputBusEnabled,
                                            double sampleRate) noexcept
    {
        updateChannelLayout(numInputChannels, numOutputChannels, inputBusEnabled);
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        peakHold_ = 0.0f;
        peakHoldSamplesRemaining_ = 0;
        peakDisplay_.store(0.0f, std::memory_order_relaxed);
    }

    void AudioPassthroughProcessor::updateChannelLayout(int numInputChannels,
                                                         int numOutputChannels,
                                                         bool inputBusEnabled) noexcept
    {
        numInputChannels_ = numInputChannels;
        numOutputChannels_ = numOutputChannels;
        inputBusEnabled_ = inputBusEnabled;
    }

    void AudioPassthroughProcessor::setChannelMode(AudioFromChannelMode mode) noexcept
    {
        channelMode_.store(static_cast<int>(mode), std::memory_order_relaxed);
    }

    int AudioPassthroughProcessor::mapSourceChannel(int outputChannel) const noexcept
    {
        const auto mode = static_cast<AudioFromChannelMode>(channelMode_.load(std::memory_order_relaxed));

        if (numInputChannels_ <= 0)
            return -1;

        switch (mode)
        {
            case AudioFromChannelMode::kMonoLeft:
                return 0;

            case AudioFromChannelMode::kMonoRight:
                return numInputChannels_ > 1 ? 1 : 0;

            case AudioFromChannelMode::kStereo:
            default:
                return outputChannel < numInputChannels_ ? outputChannel : -1;
        }
    }

    void AudioPassthroughProcessor::updatePeakBallistics(float blockPeak, int numSamples) noexcept
    {
        if (!std::isfinite(blockPeak) || blockPeak < 0.0f)
            blockPeak = 0.0f;

        const int holdSamples = juce::roundToInt(static_cast<float>(sampleRate_) * kPeakHoldSeconds);

        if (blockPeak >= peakHold_)
        {
            peakHold_ = blockPeak;
            peakHoldSamplesRemaining_ = holdSamples;
        }
        else if (peakHoldSamplesRemaining_ > 0)
        {
            peakHoldSamplesRemaining_ = std::max(0, peakHoldSamplesRemaining_ - numSamples);
        }
        else if (peakHold_ > 0.0f && kPeakDecaySeconds > 0.0f)
        {
            const float decayPerSample = std::exp(-1.0f / (static_cast<float>(sampleRate_) * kPeakDecaySeconds));
            const float decay = std::pow(decayPerSample, static_cast<float>(juce::jmax(1, numSamples)));
            peakHold_ *= decay;
        }

        peakDisplay_.store(juce::jlimit(0.0f, 1.0f, peakHold_), std::memory_order_relaxed);
    }

    void AudioPassthroughProcessor::process(juce::AudioBuffer<float>& buffer, float gainLinear) noexcept
    {
        const int numSamples = buffer.getNumSamples();
        const int numBufferChannels = buffer.getNumChannels();

        if (numSamples <= 0)
        {
            if (numOutputChannels_ > 0)
                updatePeakBallistics(0.0f, 0);

            return;
        }

        if (numOutputChannels_ <= 0)
            return;

        if (!std::isfinite(gainLinear))
            gainLinear = 0.0f;

        const int numChannelsToProcess = juce::jmin(numOutputChannels_, numBufferChannels);

        if (!inputBusEnabled_ || numInputChannels_ <= 0)
        {
            for (int channel = 0; channel < numChannelsToProcess; ++channel)
                buffer.clear(channel, 0, numSamples);

            updatePeakBallistics(0.0f, numSamples);
            return;
        }

        for (int outputChannel = 0; outputChannel < numChannelsToProcess; ++outputChannel)
        {
            const int sourceChannel = mapSourceChannel(outputChannel);

            if (sourceChannel < 0 || sourceChannel >= numBufferChannels)
            {
                buffer.clear(outputChannel, 0, numSamples);
                continue;
            }

            if (sourceChannel != outputChannel)
                buffer.copyFrom(outputChannel, 0, buffer, sourceChannel, 0, numSamples);
        }

        float blockPeak = 0.0f;

        for (int outputChannel = 0; outputChannel < numChannelsToProcess; ++outputChannel)
        {
            const int sourceChannel = mapSourceChannel(outputChannel);

            if (sourceChannel < 0)
                continue;

            auto* channelData = buffer.getWritePointer(outputChannel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                const float scaled = channelData[sample] * gainLinear;

                if (!std::isfinite(scaled))
                    channelData[sample] = 0.0f;
                else
                {
                    channelData[sample] = scaled;
                    blockPeak = std::max(blockPeak, std::abs(scaled));
                }
            }
        }

        updatePeakBallistics(blockPeak, numSamples);
    }
}
