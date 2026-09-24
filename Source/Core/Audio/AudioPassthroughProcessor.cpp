#include "Core/Audio/AudioPassthroughProcessor.h"

namespace Core
{
    void AudioPassthroughProcessor::prepare(int numInputChannels,
                                            int numOutputChannels,
                                            bool inputBusEnabled,
                                            double sampleRate) noexcept
    {
        updateChannelLayout(numInputChannels, numOutputChannels, inputBusEnabled);
        juce::ignoreUnused(sampleRate);
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

    void AudioPassthroughProcessor::setMonoSourceChannelIndex(int channelIndex) noexcept
    {
        monoSourceChannelIndex_.store(juce::jmax(0, channelIndex), std::memory_order_relaxed);
    }

    void AudioPassthroughProcessor::setPassthroughActive(bool active) noexcept
    {
        // Release publishes prior map stores; process loads with acquire.
        passthroughActive_.store(active, std::memory_order_release);
    }

    int AudioPassthroughProcessor::mapSourceChannel(int outputChannel) const noexcept
    {
        const auto mode = static_cast<AudioFromChannelMode>(channelMode_.load(std::memory_order_relaxed));

        if (numInputChannels_ <= 0)
            return -1;

        switch (mode)
        {
            case AudioFromChannelMode::kMonoLeft:
            case AudioFromChannelMode::kMonoRight:
            {
                const int channelIndex = monoSourceChannelIndex_.load(std::memory_order_relaxed);
                return channelIndex < numInputChannels_ ? channelIndex : -1;
            }

            case AudioFromChannelMode::kStereo:
            default:
                return outputChannel < numInputChannels_ ? outputChannel : -1;
        }
    }

    void AudioPassthroughProcessor::updatePeakLevel(float blockPeak) noexcept
    {
        if (!std::isfinite(blockPeak) || blockPeak < 0.0f)
            blockPeak = 0.0f;

        peakDisplay_.store(juce::jlimit(0.0f, 1.0f, blockPeak), std::memory_order_relaxed);
    }

    bool AudioPassthroughProcessor::shouldDuplicateMono() const noexcept
    {
        const auto mode = static_cast<AudioFromChannelMode>(channelMode_.load(std::memory_order_relaxed));
        return mode == AudioFromChannelMode::kMonoLeft
            || mode == AudioFromChannelMode::kMonoRight;
    }

    void AudioPassthroughProcessor::clearAllOutputChannels(juce::AudioBuffer<float>& output,
                                                           int numSamples) const noexcept
    {
        for (int channel = 0; channel < output.getNumChannels(); ++channel)
            output.clear(channel, 0, numSamples);
    }

    void AudioPassthroughProcessor::clearTrailingOutputChannels(juce::AudioBuffer<float>& output,
                                                                int firstChannel,
                                                                int numSamples) const noexcept
    {
        for (int channel = firstChannel; channel < output.getNumChannels(); ++channel)
            output.clear(channel, 0, numSamples);
    }

    float AudioPassthroughProcessor::applyGainAndTrackPeak(float sample,
                                                           float gainLinear,
                                                           float& blockPeak) noexcept
    {
        if (!std::isfinite(sample))
            return 0.0f;

        const float scaled = sample * gainLinear;

        if (!std::isfinite(scaled))
            return 0.0f;

        blockPeak = std::max(blockPeak, std::abs(scaled));
        return scaled;
    }

    float AudioPassthroughProcessor::processMonoDuplicate(const ProcessBuffers& buffers,
                                                          float gainLinear) noexcept
    {
        const int sourceChannel = monoSourceChannelIndex_.load(std::memory_order_relaxed);

        if (sourceChannel < 0 || sourceChannel >= buffers.numInputChannelsAvailable)
        {
            clearAllOutputChannels(buffers.output, buffers.numSamples);
            return 0.0f;
        }

        const float* inputData = buffers.input.getReadPointer(sourceChannel);
        float blockPeak = 0.0f;

        for (int sample = 0; sample < buffers.numSamples; ++sample)
        {
            const float scaled = applyGainAndTrackPeak(inputData[sample], gainLinear, blockPeak);

            for (int outputChannel = 0; outputChannel < buffers.numOutputChannelsToProcess; ++outputChannel)
                buffers.output.getWritePointer(outputChannel)[sample] = scaled;
        }

        return blockPeak;
    }

    float AudioPassthroughProcessor::processMappedChannels(const ProcessBuffers& buffers,
                                                           float gainLinear) noexcept
    {
        float blockPeak = 0.0f;

        for (int outputChannel = 0; outputChannel < buffers.numOutputChannelsToProcess; ++outputChannel)
        {
            const int sourceChannel = mapSourceChannel(outputChannel);

            if (sourceChannel < 0 || sourceChannel >= buffers.numInputChannelsAvailable)
            {
                buffers.output.clear(outputChannel, 0, buffers.numSamples);
                continue;
            }

            const float* inputData = buffers.input.getReadPointer(sourceChannel);
            float* outputData = buffers.output.getWritePointer(outputChannel);

            for (int sample = 0; sample < buffers.numSamples; ++sample)
                outputData[sample] = applyGainAndTrackPeak(inputData[sample], gainLinear, blockPeak);
        }

        return blockPeak;
    }

    void AudioPassthroughProcessor::process(const juce::AudioBuffer<float>& input,
                                            juce::AudioBuffer<float>& output,
                                            float gainLinear) noexcept
    {
        const int numSamples = juce::jmin(input.getNumSamples(), output.getNumSamples());

        if (numSamples <= 0)
        {
            if (numOutputChannels_ > 0)
                updatePeakLevel(0.0f);

            return;
        }

        if (numOutputChannels_ <= 0)
            return;

        if (!std::isfinite(gainLinear))
            gainLinear = 0.0f;

        const ProcessBuffers buffers {
            input,
            output,
            numSamples,
            juce::jmin(numInputChannels_, input.getNumChannels()),
            juce::jmin(numOutputChannels_, output.getNumChannels())
        };

        if (!passthroughActive_.load(std::memory_order_acquire)
            || !inputBusEnabled_
            || buffers.numInputChannelsAvailable <= 0)
        {
            clearAllOutputChannels(output, numSamples);
            updatePeakLevel(0.0f);
            return;
        }

        const float blockPeak = shouldDuplicateMono()
                                    ? processMonoDuplicate(buffers, gainLinear)
                                    : processMappedChannels(buffers, gainLinear);

        clearTrailingOutputChannels(output, buffers.numOutputChannelsToProcess, numSamples);
        updatePeakLevel(blockPeak);
    }
}
