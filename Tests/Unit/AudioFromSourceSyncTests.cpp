#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "Core/Audio/AudioFromSourceSync.h"
#include "Core/Audio/AudioPassthroughProcessor.h"

class AudioFromSourceSyncTests : public juce::UnitTest
{
public:
    AudioFromSourceSyncTests() : juce::UnitTest("AudioFromSourceSync") {}

    void runTest() override
    {
        testEmptySourceDeactivatesWithoutChannelModeWrite();
        testSelectedSourceArmsMapsAndRequestsChannelModeWrite();
        testApplyEmptyThenSelectedRestoresPassthrough();
    }

private:
    void testEmptySourceDeactivatesWithoutChannelModeWrite()
    {
        beginTest("Empty Audio From source id plans inactive passthrough without channel-mode write");

        const auto decision = Core::decideAudioFromSourceSync({});

        expect(! decision.passthroughActive);
        expect(! decision.shouldWriteChannelModeProperty);
    }

    void testSelectedSourceArmsMapsAndRequestsChannelModeWrite()
    {
        beginTest("Selected mono source plans active passthrough and channel-mode write");

        const auto decision = Core::decideAudioFromSourceSync("mono:1");

        expect(decision.passthroughActive);
        expect(decision.shouldWriteChannelModeProperty);
        expectEquals(static_cast<int>(decision.channelMode),
                     static_cast<int>(Core::AudioFromChannelMode::kMonoLeft));
        expectEquals(decision.monoSourceChannelIndex, 1);
    }

    void testApplyEmptyThenSelectedRestoresPassthrough()
    {
        beginTest("Apply empty then selected source restores mapped output via shared sync helper");

        Core::AudioPassthroughProcessor processor;
        processor.prepare(2, 2, true, 44100.0);

        juce::AudioBuffer<float> input(2, 4);
        juce::AudioBuffer<float> output(2, 4);
        input.setSample(0, 0, 0.5f);
        input.setSample(1, 0, -0.25f);
        output.clear();

        Core::applyAudioFromSourceSync(processor, Core::decideAudioFromSourceSync({}));
        processor.process(input, output, 1.0f);
        expectEquals(output.getSample(0, 0), 0.0f);
        expectEquals(processor.getPeakLevel(), 0.0f);

        Core::applyAudioFromSourceSync(processor, Core::decideAudioFromSourceSync("stereo:0"));
        processor.process(input, output, 1.0f);
        expectEquals(output.getSample(0, 0), 0.5f);
        expectEquals(output.getSample(1, 0), -0.25f);
        expect(processor.getPeakLevel() > 0.4f);
    }
};

static AudioFromSourceSyncTests audioFromSourceSyncTests;
