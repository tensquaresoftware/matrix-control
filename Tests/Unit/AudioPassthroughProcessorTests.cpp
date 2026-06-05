#include <juce_core/juce_core.h>

#include "Core/Audio/AudioPassthroughProcessor.h"

class AudioPassthroughProcessorTests : public juce::UnitTest
{
public:
    AudioPassthroughProcessorTests() : juce::UnitTest("AudioPassthroughProcessor") {}

    void runTest() override
    {
        testUnityGainPassthrough();
        testGainScaling();
        testMonoLeftDuplicatesLeftToBothOutputs();
        testMonoRightDuplicatesRightToBothOutputs();
        testPeakTracksMaxSample();
        testPeakHoldRetainsLevelBriefly();
        testSilenceProducesZeroPeak();
        testDisabledInputBusClearsOutput();
        testZeroLengthBlockIsSafe();
    }

private:
    void testUnityGainPassthrough()
    {
        beginTest("Unity gain passthrough copies input to output");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> buffer(2, 4);
        buffer.setSample(0, 0, 0.5f);
        buffer.setSample(1, 0, -0.25f);
        buffer.setSample(0, 1, 0.125f);
        buffer.setSample(1, 1, 0.75f);

        processor.process(buffer, 1.0f);

        expectEquals(buffer.getSample(0, 0), 0.5f);
        expectEquals(buffer.getSample(1, 0), -0.25f);
        expectEquals(buffer.getSample(0, 1), 0.125f);
        expectEquals(buffer.getSample(1, 1), 0.75f);
    }

    void testGainScaling()
    {
        beginTest("Linear gain scales output samples");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(1, 1, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> buffer(1, 2);
        buffer.setSample(0, 0, 0.5f);
        buffer.setSample(0, 1, -1.0f);

        processor.process(buffer, 2.0f);

        expectEquals(buffer.getSample(0, 0), 1.0f);
        expectEquals(buffer.getSample(0, 1), -2.0f);
    }

    void testMonoLeftDuplicatesLeftToBothOutputs()
    {
        beginTest("MONO L duplicates left input to both outputs with single gain pass");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> buffer(2, 2);
        buffer.setSample(0, 0, 0.5f);
        buffer.setSample(1, 0, 0.25f);

        processor.process(buffer, 2.0f);

        expectEquals(buffer.getSample(0, 0), 1.0f);
        expectEquals(buffer.getSample(1, 0), 1.0f);
    }

    void testMonoRightDuplicatesRightToBothOutputs()
    {
        beginTest("MONO R duplicates right input to both outputs with single gain pass");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoRight);

        juce::AudioBuffer<float> buffer(2, 2);
        buffer.setSample(0, 0, 0.5f);
        buffer.setSample(1, 0, 0.25f);

        processor.process(buffer, 2.0f);

        expectEquals(buffer.getSample(0, 0), 0.5f);
        expectEquals(buffer.getSample(1, 0), 0.5f);
    }

    void testPeakHoldRetainsLevelBriefly()
    {
        beginTest("Peak hold retains level during brief silence");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(1, 1, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> loudBuffer(1, 64);
        loudBuffer.clear();
        loudBuffer.setSample(0, 0, 0.9f);
        processor.process(loudBuffer, 1.0f);
        expect(processor.getPeakLevel() > 0.85f);

        juce::AudioBuffer<float> silentBuffer(1, 64);
        silentBuffer.clear();
        processor.process(silentBuffer, 1.0f);

        expect(processor.getPeakLevel() > 0.85f);
    }

    void testZeroLengthBlockIsSafe()
    {
        beginTest("Zero-length block is safe and keeps peak valid");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> loudBuffer(2, 8);
        loudBuffer.clear();
        loudBuffer.setSample(0, 0, 1.0f);
        processor.process(loudBuffer, 1.0f);
        expect(processor.getPeakLevel() > 0.9f);

        juce::AudioBuffer<float> emptyBuffer(2, 0);
        processor.process(emptyBuffer, 1.0f);

        expect(std::isfinite(processor.getPeakLevel()));
        expect(processor.getPeakLevel() >= 0.0f);
    }

    void testPeakTracksMaxSample()
    {
        beginTest("Peak level tracks post-gain maximum sample");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(1, 1, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> buffer(1, 3);
        buffer.setSample(0, 0, 0.2f);
        buffer.setSample(0, 1, 0.8f);
        buffer.setSample(0, 2, -0.4f);

        processor.process(buffer, 1.0f);

        expectEquals(processor.getPeakLevel(), 0.8f);
    }

    void testSilenceProducesZeroPeak()
    {
        beginTest("Silence decays peak toward zero");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> loudBuffer(2, 8);
        loudBuffer.clear();
        loudBuffer.setSample(0, 0, 1.0f);
        processor.process(loudBuffer, 1.0f);
        expect(processor.getPeakLevel() > 0.9f);

        juce::AudioBuffer<float> silentBuffer(2, 512);
        silentBuffer.clear();

        for (int i = 0; i < 600; ++i)
            processor.process(silentBuffer, 1.0f);

        expect(processor.getPeakLevel() < 0.02f);
    }

    void testDisabledInputBusClearsOutput()
    {
        beginTest("Disabled input bus clears output and peak");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, false, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> buffer(2, 4);
        buffer.setSample(0, 0, 0.9f);
        buffer.setSample(1, 0, 0.9f);

        processor.process(buffer, 2.0f);

        expectEquals(buffer.getSample(0, 0), 0.0f);
        expectEquals(buffer.getSample(1, 0), 0.0f);
        expectEquals(processor.getPeakLevel(), 0.0f);
    }
};

static AudioPassthroughProcessorTests audioPassthroughProcessorTests;
