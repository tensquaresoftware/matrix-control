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
        bufferTryOrderPutsRequestedThenDefaultThenRemaining();
        bufferTryOrderSkipsNonPositiveAndDedupes();
        bufferFallbackTrySucceedsOnlyWhenLiveRateMatches();
        shouldRestorePreviousSetupWhenFallbackDidNotOpen();
        bufferTryOrderKeepsComfortableRequestedFirst();
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

    void bufferTryOrderPutsRequestedThenDefaultThenRemaining()
    {
        beginTest("bufferTryOrderPutsRequestedThenDefaultThenRemaining");

        const juce::Array<int> available { 64, 128, 256, 512, 1024 };
        const auto order = Core::buildBufferSizeTryOrder(256, 512, available);

        expectEquals(order.size(), 5);
        expectEquals(order[0], 256);
        expectEquals(order[1], 512);
        expectEquals(order[2], 64);
        expectEquals(order[3], 128);
        expectEquals(order[4], 1024);
    }

    void bufferTryOrderSkipsNonPositiveAndDedupes()
    {
        beginTest("bufferTryOrderSkipsNonPositiveAndDedupes");

        const juce::Array<int> available { 0, 128, 256, -1 };
        const auto order = Core::buildBufferSizeTryOrder(128, 0, available);

        expectEquals(order.size(), 2);
        expectEquals(order[0], 128);
        expectEquals(order[1], 256);

        const auto requestedOnly = Core::buildBufferSizeTryOrder(512, 512, {});
        expectEquals(requestedOnly.size(), 1);
        expectEquals(requestedOnly[0], 512);
    }

    void bufferFallbackTrySucceedsOnlyWhenLiveRateMatches()
    {
        beginTest("bufferFallbackTrySucceedsOnlyWhenLiveRateMatches");

        // High SR + stale/coerced buffer: open OK only when live rate matches request.
        expect(Core::didBufferFallbackTrySucceed(true, true, 192000.0, 192000.0));
        expect(! Core::didBufferFallbackTrySucceed(true, true, 192000.0, 48000.0));
        expect(! Core::didBufferFallbackTrySucceed(false, true, 192000.0, 192000.0));
        expect(! Core::didBufferFallbackTrySucceed(true, false, 192000.0, 192000.0));

        // Lower rates / buffer-only: non-positive requested rate accepts any live rate.
        expect(Core::didBufferFallbackTrySucceed(true, true, 0.0, 48000.0));
        expect(Core::didBufferFallbackTrySucceed(true, true, 48000.0, 48000.0));
    }

    void shouldRestorePreviousSetupWhenFallbackDidNotOpen()
    {
        beginTest("shouldRestorePreviousSetupWhenFallbackDidNotOpen");

        expect(Core::shouldRestorePreviousSetupAfterBufferFallback(false));
        expect(! Core::shouldRestorePreviousSetupAfterBufferFallback(true));
    }

    void bufferTryOrderKeepsComfortableRequestedFirst()
    {
        beginTest("bufferTryOrderKeepsComfortableRequestedFirst");

        // High SR + buffer 512: requested 512 stays first, then default, then remainder.
        const juce::Array<int> available { 64, 128, 256, 512, 1024 };
        const auto order = Core::buildBufferSizeTryOrder(512, 256, available);

        expectEquals(order[0], 512);
        expectEquals(order[1], 256);
        expect(order.contains(1024));
    }
};

static AudioDevicePreferredSetupTests audioDevicePreferredSetupTests;
