#pragma once

#include <juce_core/juce_core.h>

namespace Core
{
    /** Snapshot of the last user-chosen sample rate / buffer for restore-on-device-switch. */
    struct AudioDevicePreferredSetup
    {
        double sampleRate = 0.0;
        int bufferSize = 0;
    };

    /** Device identity used to decide whether a manager change is a device switch. */
    struct AudioDeviceIdentity
    {
        juce::String outputDeviceName;
        juce::String inputDeviceName;
    };

    inline bool didAudioDeviceIdentityChange(const AudioDeviceIdentity& previous,
                                             const AudioDeviceIdentity& current) noexcept
    {
        return previous.outputDeviceName != current.outputDeviceName
            || previous.inputDeviceName != current.inputDeviceName;
    }

    /** Whether the preferred sample rate should replace the current setup value. */
    inline bool shouldRestorePreferredSampleRate(double preferredSampleRate,
                                                 double currentSampleRate,
                                                 bool preferredRateSupported) noexcept
    {
        return preferredSampleRate > 0.0
            && preferredRateSupported
            && ! juce::approximatelyEqual(currentSampleRate, preferredSampleRate);
    }

    /** Whether the preferred buffer size should replace the current setup value. */
    inline bool shouldRestorePreferredBufferSize(int preferredBufferSize,
                                                 int currentBufferSize,
                                                 bool preferredBufferSupported) noexcept
    {
        return preferredBufferSize > 0
            && preferredBufferSupported
            && currentBufferSize != preferredBufferSize;
    }

    /** Update stored preferences from a live setup (ignore invalid zeros). */
    inline AudioDevicePreferredSetup capturePreferredSetup(const AudioDevicePreferredSetup& previous,
                                                           double liveSampleRate,
                                                           int liveBufferSize) noexcept
    {
        AudioDevicePreferredSetup next = previous;

        if (liveSampleRate > 0.0)
            next.sampleRate = liveSampleRate;

        if (liveBufferSize > 0)
            next.bufferSize = liveBufferSize;

        return next;
    }

    /** Inputs for restore-then-capture orchestration on a device-manager change. */
    struct PreferredSetupChangeInput
    {
        AudioDeviceIdentity previousIdentity;
        AudioDeviceIdentity currentIdentity;
        AudioDevicePreferredSetup preferred;
        double liveSampleRate = 0.0;
        int liveBufferSize = 0;
        bool preferredRateSupported = false;
        bool preferredBufferSupported = false;
    };

    /** Plan produced by restore-then-capture: restore only on identity change. */
    struct PreferredSetupChangePlan
    {
        bool shouldRestore = false;
        double sampleRateToApply = 0.0;
        int bufferSizeToApply = 0;
        AudioDevicePreferredSetup preferredAfterCapture;
        AudioDeviceIdentity identityToStore;
    };

    /**
        Restore preferred rate/buffer only when the audio device identity changes.
        Same-device rate/buffer edits capture only (never undo the user's choice).
        Ordering is always restore (optional) then capture from the effective values.
    */
    inline PreferredSetupChangePlan planPreferredSetupChange(const PreferredSetupChangeInput& in) noexcept
    {
        PreferredSetupChangePlan plan;
        plan.identityToStore = in.currentIdentity;

        double effectiveRate = in.liveSampleRate;
        int effectiveBuffer = in.liveBufferSize;

        if (didAudioDeviceIdentityChange(in.previousIdentity, in.currentIdentity))
        {
            if (shouldRestorePreferredSampleRate(
                    in.preferred.sampleRate, in.liveSampleRate, in.preferredRateSupported))
            {
                effectiveRate = in.preferred.sampleRate;
                plan.shouldRestore = true;
            }

            if (shouldRestorePreferredBufferSize(
                    in.preferred.bufferSize, in.liveBufferSize, in.preferredBufferSupported))
            {
                effectiveBuffer = in.preferred.bufferSize;
                plan.shouldRestore = true;
            }

            if (plan.shouldRestore)
            {
                plan.sampleRateToApply = effectiveRate;
                plan.bufferSizeToApply = effectiveBuffer;
            }
        }

        plan.preferredAfterCapture = capturePreferredSetup(in.preferred, effectiveRate, effectiveBuffer);
        return plan;
    }

    /** Selector flags for the Matrix Audio Settings wrap (no MIDI sections). */
    struct AudioMidiSettingsSelectorPolicy
    {
        bool showMidiInputOptions = false;
        bool showMidiOutputSelector = false;
        bool showChannelsAsStereoPairs = true;
        bool hideAdvancedOptionsWithButton = false;
    };

    inline AudioMidiSettingsSelectorPolicy matrixAudioMidiSettingsSelectorPolicy() noexcept
    {
        return {};
    }

    /**
        Ordered buffer sizes to try when opening a sample rate: requested first, then the
        device default, then every other size from the device's available list (deduped).
        Pure / hardware-free — used by Settings AUDIO apply fallback.
    */
    inline juce::Array<int> buildBufferSizeTryOrder(int requestedBufferSize,
                                                    int deviceDefaultBufferSize,
                                                    const juce::Array<int>& availableBufferSizes)
    {
        juce::Array<int> order;

        const auto appendUniquePositive = [&order](int size)
        {
            if (size > 0 && ! order.contains(size))
                order.add(size);
        };

        appendUniquePositive(requestedBufferSize);
        appendUniquePositive(deviceDefaultBufferSize);
        for (const auto size : availableBufferSizes)
            appendUniquePositive(size);

        return order;
    }

    /**
        One buffer-try outcome: setup applied with no error, live device present, and
        live sample rate matches the requested rate (when a positive rate was requested).
    */
    inline bool didBufferFallbackTrySucceed(bool setSetupSucceeded,
                                            bool liveDevicePresent,
                                            double requestedSampleRate,
                                            double liveSampleRate) noexcept
    {
        if (! setSetupSucceeded || ! liveDevicePresent)
            return false;

        if (requestedSampleRate <= 0.0)
            return true;

        return juce::approximatelyEqual(liveSampleRate, requestedSampleRate);
    }

    /** After every buffer try failed, restore the setup that was live before the apply. */
    inline bool shouldRestorePreviousSetupAfterBufferFallback(bool fallbackOpened) noexcept
    {
        return ! fallbackOpened;
    }
}
