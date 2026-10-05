#include <juce_core/juce_core.h>

#include "Core/Audio/AudioDevicePreferredSetup.h"

class AudioDevicePreferredSetupTests : public juce::UnitTest
{
public:
    AudioDevicePreferredSetupTests() : juce::UnitTest("AudioDevicePreferredSetup") {}

    void runTest() override
    {
        restoresSampleRateWhenSupportedAndChanged();
        skipsSampleRateWhenUnsupportedOrAlreadyMatched();
        restoresBufferWhenSupportedAndChanged();
        skipsBufferWhenUnsupportedOrAlreadyMatched();
        captureIgnoresInvalidZeros();
        sameDeviceChangeCapturesWithoutRestore();
        deviceIdentityChangeRestoresThenCaptures();
        deviceIdentityChangeSkipsUnsupportedPreferred();
    }

private:
    void restoresSampleRateWhenSupportedAndChanged()
    {
        beginTest("restoresSampleRateWhenSupportedAndChanged");

        expect(Core::shouldRestorePreferredSampleRate(48000.0, 44100.0, true));
    }

    void skipsSampleRateWhenUnsupportedOrAlreadyMatched()
    {
        beginTest("skipsSampleRateWhenUnsupportedOrAlreadyMatched");

        expect(! Core::shouldRestorePreferredSampleRate(48000.0, 44100.0, false));
        expect(! Core::shouldRestorePreferredSampleRate(48000.0, 48000.0, true));
        expect(! Core::shouldRestorePreferredSampleRate(0.0, 44100.0, true));
    }

    void restoresBufferWhenSupportedAndChanged()
    {
        beginTest("restoresBufferWhenSupportedAndChanged");

        expect(Core::shouldRestorePreferredBufferSize(256, 512, true));
    }

    void skipsBufferWhenUnsupportedOrAlreadyMatched()
    {
        beginTest("skipsBufferWhenUnsupportedOrAlreadyMatched");

        expect(! Core::shouldRestorePreferredBufferSize(256, 512, false));
        expect(! Core::shouldRestorePreferredBufferSize(256, 256, true));
        expect(! Core::shouldRestorePreferredBufferSize(0, 512, true));
    }

    void captureIgnoresInvalidZeros()
    {
        beginTest("captureIgnoresInvalidZeros");

        const Core::AudioDevicePreferredSetup previous{ .sampleRate = 48000.0, .bufferSize = 256 };
        const auto next = Core::capturePreferredSetup(previous, 0.0, 0);

        expectEquals(next.sampleRate, 48000.0);
        expectEquals(next.bufferSize, 256);

        const auto updated = Core::capturePreferredSetup(previous, 44100.0, 128);
        expectEquals(updated.sampleRate, 44100.0);
        expectEquals(updated.bufferSize, 128);
    }

    void sameDeviceChangeCapturesWithoutRestore()
    {
        beginTest("sameDeviceChangeCapturesWithoutRestore");

        const Core::AudioDeviceIdentity identity{ .outputDeviceName = "Out A", .inputDeviceName = "In A" };
        const auto plan = Core::planPreferredSetupChange({
            .previousIdentity = identity,
            .currentIdentity = identity,
            .preferred = { .sampleRate = 48000.0, .bufferSize = 256 },
            .liveSampleRate = 44100.0,
            .liveBufferSize = 512,
            .preferredRateSupported = true,
            .preferredBufferSupported = true,
        });

        expect(! plan.shouldRestore);
        expectEquals(plan.preferredAfterCapture.sampleRate, 44100.0);
        expectEquals(plan.preferredAfterCapture.bufferSize, 512);
        expectEquals(plan.identityToStore.outputDeviceName, identity.outputDeviceName);
        expectEquals(plan.identityToStore.inputDeviceName, identity.inputDeviceName);
    }

    void deviceIdentityChangeRestoresThenCaptures()
    {
        beginTest("deviceIdentityChangeRestoresThenCaptures");

        const auto plan = Core::planPreferredSetupChange({
            .previousIdentity = { .outputDeviceName = "Out A", .inputDeviceName = "In A" },
            .currentIdentity = { .outputDeviceName = "Out B", .inputDeviceName = "In B" },
            .preferred = { .sampleRate = 48000.0, .bufferSize = 256 },
            .liveSampleRate = 44100.0,
            .liveBufferSize = 512,
            .preferredRateSupported = true,
            .preferredBufferSupported = true,
        });

        expect(plan.shouldRestore);
        expectEquals(plan.sampleRateToApply, 48000.0);
        expectEquals(plan.bufferSizeToApply, 256);
        expectEquals(plan.preferredAfterCapture.sampleRate, 48000.0);
        expectEquals(plan.preferredAfterCapture.bufferSize, 256);
        expectEquals(plan.identityToStore.outputDeviceName, juce::String("Out B"));
        expectEquals(plan.identityToStore.inputDeviceName, juce::String("In B"));
    }

    void deviceIdentityChangeSkipsUnsupportedPreferred()
    {
        beginTest("deviceIdentityChangeSkipsUnsupportedPreferred");

        const auto plan = Core::planPreferredSetupChange({
            .previousIdentity = { .outputDeviceName = "Out A", .inputDeviceName = "In A" },
            .currentIdentity = { .outputDeviceName = "Out B", .inputDeviceName = "In B" },
            .preferred = { .sampleRate = 96000.0, .bufferSize = 64 },
            .liveSampleRate = 44100.0,
            .liveBufferSize = 512,
            .preferredRateSupported = false,
            .preferredBufferSupported = false,
        });

        expect(! plan.shouldRestore);
        expectEquals(plan.preferredAfterCapture.sampleRate, 44100.0);
        expectEquals(plan.preferredAfterCapture.bufferSize, 512);
    }
};

static AudioDevicePreferredSetupTests audioDevicePreferredSetupTests;
