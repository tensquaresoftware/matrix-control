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
        testSeparateInputOutputBusBuffers();
        testPeakTracksMaxSample();
        testPeakHoldRetainsLevelBriefly();
        testSilenceProducesZeroPeak();
        testDisabledInputBusClearsOutput();
        testZeroInputChannelsClearsOutput();
        testZeroLengthBlockIsSafe();
    }

private:
    void testUnityGainPassthrough()
    {
        beginTest("Unity gain passthrough copies input to output");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> input(2, 4);
        juce::AudioBuffer<float> output(2, 4);
        input.setSample(0, 0, 0.5f);
        input.setSample(1, 0, -0.25f);
        input.setSample(0, 1, 0.125f);
        input.setSample(1, 1, 0.75f);
        output.clear();

        processor.process(input, output, 1.0f);

        expectEquals(output.getSample(0, 0), 0.5f);
        expectEquals(output.getSample(1, 0), -0.25f);
        expectEquals(output.getSample(0, 1), 0.125f);
        expectEquals(output.getSample(1, 1), 0.75f);
    }

    void testGainScaling()
    {
        beginTest("Linear gain scales output samples");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(1, 1, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> input(1, 2);
        juce::AudioBuffer<float> output(1, 2);
        input.setSample(0, 0, 0.5f);
        input.setSample(0, 1, -1.0f);
        output.clear();

        processor.process(input, output, 2.0f);

        expectEquals(output.getSample(0, 0), 1.0f);
        expectEquals(output.getSample(0, 1), -2.0f);
    }

    void testMonoLeftDuplicatesLeftToBothOutputs()
    {
        beginTest("MONO L duplicates left input to both outputs with single gain pass");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> input(2, 2);
        juce::AudioBuffer<float> output(2, 2);
        input.setSample(0, 0, 0.5f);
        input.setSample(1, 0, 0.25f);
        output.clear();

        processor.process(input, output, 2.0f);

        expectEquals(output.getSample(0, 0), 1.0f);
        expectEquals(output.getSample(1, 0), 1.0f);
    }

    void testMonoRightDuplicatesRightToBothOutputs()
    {
        beginTest("MONO R duplicates right input to both outputs with single gain pass");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoRight);

        juce::AudioBuffer<float> input(2, 2);
        juce::AudioBuffer<float> output(2, 2);
        input.setSample(0, 0, 0.5f);
        input.setSample(1, 0, 0.25f);
        output.clear();

        processor.process(input, output, 2.0f);

        expectEquals(output.getSample(0, 0), 0.5f);
        expectEquals(output.getSample(1, 0), 0.5f);
    }

    void testSeparateInputOutputBusBuffers()
    {
        beginTest("Separate input and output bus buffers copy across channel groups");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> processBlockBuffer(4, 4);
        processBlockBuffer.clear();
        processBlockBuffer.setSample(0, 0, 0.75f);
        processBlockBuffer.setSample(1, 0, 0.0f);

        juce::AudioBuffer<float> input(processBlockBuffer.getArrayOfWritePointers(), 2, 4);
        juce::AudioBuffer<float> output(processBlockBuffer.getArrayOfWritePointers() + 2, 2, 4);

        processor.process(input, output, 1.0f);

        expectEquals(output.getSample(0, 0), 0.75f);
        expectEquals(output.getSample(1, 0), 0.75f);
        expectEquals(input.getSample(0, 0), 0.75f);
    }

    void testPeakHoldRetainsLevelBriefly()
    {
        beginTest("Peak hold retains level during brief silence");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(1, 1, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> loudInput(1, 64);
        juce::AudioBuffer<float> loudOutput(1, 64);
        loudInput.clear();
        loudOutput.clear();
        loudInput.setSample(0, 0, 0.9f);
        processor.process(loudInput, loudOutput, 1.0f);
        expect(processor.getPeakLevel() > 0.85f);

        juce::AudioBuffer<float> silentInput(1, 64);
        juce::AudioBuffer<float> silentOutput(1, 64);
        silentInput.clear();
        silentOutput.clear();
        processor.process(silentInput, silentOutput, 1.0f);

        expect(processor.getPeakLevel() > 0.85f);
    }

    void testZeroLengthBlockIsSafe()
    {
        beginTest("Zero-length block is safe and keeps peak valid");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> loudInput(2, 8);
        juce::AudioBuffer<float> loudOutput(2, 8);
        loudInput.clear();
        loudOutput.clear();
        loudInput.setSample(0, 0, 1.0f);
        processor.process(loudInput, loudOutput, 1.0f);
        expect(processor.getPeakLevel() > 0.9f);

        juce::AudioBuffer<float> emptyInput(2, 0);
        juce::AudioBuffer<float> emptyOutput(2, 0);
        processor.process(emptyInput, emptyOutput, 1.0f);

        expect(std::isfinite(processor.getPeakLevel()));
        expect(processor.getPeakLevel() >= 0.0f);
    }

    void testPeakTracksMaxSample()
    {
        beginTest("Peak level tracks post-gain maximum sample");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(1, 1, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kMonoLeft);

        juce::AudioBuffer<float> input(1, 3);
        juce::AudioBuffer<float> output(1, 3);
        input.setSample(0, 0, 0.2f);
        input.setSample(0, 1, 0.8f);
        input.setSample(0, 2, -0.4f);
        output.clear();

        processor.process(input, output, 1.0f);

        expectEquals(processor.getPeakLevel(), 0.8f);
    }

    void testSilenceProducesZeroPeak()
    {
        beginTest("Silence decays peak toward zero");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> loudInput(2, 8);
        juce::AudioBuffer<float> loudOutput(2, 8);
        loudInput.clear();
        loudOutput.clear();
        loudInput.setSample(0, 0, 1.0f);
        processor.process(loudInput, loudOutput, 1.0f);
        expect(processor.getPeakLevel() > 0.9f);

        juce::AudioBuffer<float> silentInput(2, 512);
        juce::AudioBuffer<float> silentOutput(2, 512);
        silentInput.clear();
        silentOutput.clear();

        for (int i = 0; i < 600; ++i)
            processor.process(silentInput, silentOutput, 1.0f);

        expect(processor.getPeakLevel() < 0.02f);
    }

    void testZeroInputChannelsClearsOutput()
    {
        beginTest("Zero input channels clears output (hosted AD-11)");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(0, 2, false, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> input(0, 4);
        juce::AudioBuffer<float> output(2, 4);
        output.setSample(0, 0, 0.9f);
        output.setSample(1, 0, 0.9f);

        processor.process(input, output, 2.0f);

        expectEquals(output.getSample(0, 0), 0.0f);
        expectEquals(output.getSample(1, 0), 0.0f);
        expectEquals(processor.getPeakLevel(), 0.0f);
    }

    void testDisabledInputBusClearsOutput()
    {
        beginTest("Disabled input bus clears output and peak");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, false, 44100.0);
        processor.setChannelMode(Core::AudioFromChannelMode::kStereo);

        juce::AudioBuffer<float> input(2, 4);
        juce::AudioBuffer<float> output(2, 4);
        input.setSample(0, 0, 0.9f);
        input.setSample(1, 0, 0.9f);
        output.setSample(0, 0, 0.9f);
        output.setSample(1, 0, 0.9f);

        processor.process(input, output, 2.0f);

        expectEquals(output.getSample(0, 0), 0.0f);
        expectEquals(output.getSample(1, 0), 0.0f);
        expectEquals(processor.getPeakLevel(), 0.0f);
    }
};

static AudioPassthroughProcessorTests audioPassthroughProcessorTests;
