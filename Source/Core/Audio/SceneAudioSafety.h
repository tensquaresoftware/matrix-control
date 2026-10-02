#pragma once

#include <juce_core/juce_core.h>

namespace Core
{
    /** APVTS companion: Input device name bound to the current non-empty audioFromSourceId. */
    inline constexpr const char* kAudioFromBoundInputDeviceNameProperty =
        "audioFromBoundInputDeviceName";

    /** ApplicationProperties one-shot: first-run Input None + AUDIO FROM empty applied. */
    inline constexpr const char* kSceneAudioSafetyDefaultsAppliedProperty =
        "sceneAudioSafetyDefaultsApplied";

    /**
        Valid non-empty selection: id is in the active-channel catalog AND the current Input
        device identity matches the identity persisted with that selection.
        Empty source id (None) is always an accepted selection state.
    */
    inline bool isAudioFromSelectionValid(const juce::String& sourceId,
                                          const juce::String& boundInputDeviceIdentity,
                                          const juce::String& currentInputDeviceIdentity,
                                          const juce::StringArray& catalogIds) noexcept
    {
        if (sourceId.isEmpty())
            return true;

        return catalogIds.contains(sourceId)
            && boundInputDeviceIdentity.isNotEmpty()
            && boundInputDeviceIdentity == currentInputDeviceIdentity;
    }

    /** Outcome of syncing AUDIO FROM after catalog / Input-identity refresh. */
    struct AudioFromSelectionSyncDecision
    {
        juce::String sourceIdToApply;
        bool selectionKept = false;
        bool shouldClearBoundIdentity = false;
    };

    /**
        Keep the saved id only when still valid for the current Input identity + catalog.
        Otherwise clear to empty (None) — never silent-remap channel indices across devices.
    */
    inline AudioFromSelectionSyncDecision decideAudioFromSelectionSync(
        const juce::String& savedSourceId,
        const juce::String& boundInputDeviceIdentity,
        const juce::String& currentInputDeviceIdentity,
        const juce::StringArray& catalogIds) noexcept
    {
        AudioFromSelectionSyncDecision decision;

        if (savedSourceId.isEmpty())
        {
            decision.shouldClearBoundIdentity = boundInputDeviceIdentity.isNotEmpty();
            return decision;
        }

        if (isAudioFromSelectionValid(savedSourceId,
                                       boundInputDeviceIdentity,
                                       currentInputDeviceIdentity,
                                       catalogIds))
        {
            decision.sourceIdToApply = savedSourceId;
            decision.selectionKept = true;
            return decision;
        }

        decision.shouldClearBoundIdentity = true;
        return decision;
    }

    /** First-run gate (criterion C): apply once while the ApplicationProperties flag is unset. */
    inline bool shouldApplySceneAudioSafetyDefaults(bool defaultsAlreadyApplied) noexcept
    {
        return ! defaultsAlreadyApplied;
    }

    /**
        Init / restore: empty saved id stays empty. Never invent stereo:0 / mono:* from channel mode.
        Non-empty ids pass through unchanged.
    */
    inline juce::String resolveAudioFromSourceIdAtInit(const juce::String& savedSourceId) noexcept
    {
        return savedSourceId;
    }
}
