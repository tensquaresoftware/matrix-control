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
        /**
            Catalog not ready yet — leave APVTS untouched; retry when channels appear.
            Callers must check shouldDefer before treating !selectionKept as a clear.
        */
        bool shouldDefer = false;
    };

    /**
        Keep the saved id only when still valid for the current Input identity + catalog.
        Empty active-channel catalog while a non-empty id is saved: defer (do not clear) —
        CoreAudio often enumerates channels after the first editor refresh / profile restore.
        If both Input identities are already known and differ, clear even when the catalog
        is still empty — never keep a channel selection across interfaces.
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

        if (boundInputDeviceIdentity.isNotEmpty()
            && currentInputDeviceIdentity.isNotEmpty()
            && boundInputDeviceIdentity != currentInputDeviceIdentity)
        {
            decision.shouldClearBoundIdentity = true;
            return decision;
        }

        if (catalogIds.isEmpty())
        {
            decision.sourceIdToApply = savedSourceId;
            decision.shouldDefer = true;
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

    /**
        Preferred-after-inquiry must not re-arm monitoring when AUDIO FROM is None
        (explicit user choice or safety clear after Input identity change).
        Only remap an existing non-empty selection toward the preferred kind.
    */
    inline bool shouldApplyPreferredAudioFrom(const juce::String& currentSourceId) noexcept
    {
        return currentSourceId.isNotEmpty();
    }

    /**
        When a persisted Input/Output name is non-empty but absent from the live device list
        (Scarlett powered off, etc.), force that endpoint to None instead of keeping an OS fallback.
    */
    inline bool shouldForceAudioEndpointToNone(const juce::String& persistedDeviceName,
                                               const juce::StringArray& availableDeviceNames) noexcept
    {
        return persistedDeviceName.isNotEmpty()
            && ! availableDeviceNames.contains(persistedDeviceName);
    }

    /** Both Input and Output scans empty: enumeration is incomplete, not "every device missing". */
    inline bool areAudioDeviceNameListsStillIncomplete(
        const juce::StringArray& availableInputNames,
        const juce::StringArray& availableOutputNames) noexcept
    {
        return availableInputNames.isEmpty() && availableOutputNames.isEmpty();
    }

    /** One-sided empty list: that endpoint's scan is incomplete, so do not force it to None. */
    inline bool shouldApplyMissingDeviceNoneForEndpoint(
        const juce::StringArray& availableNamesForEndpoint) noexcept
    {
        return ! availableNamesForEndpoint.isEmpty();
    }

    /** Endpoint names stored in JUCE AudioDeviceManager state XML (`audioSetup`). */
    struct PersistedAudioEndpoints
    {
        juce::String inputDeviceName;
        juce::String outputDeviceName;
    };

    inline PersistedAudioEndpoints readPersistedAudioEndpoints(const juce::XmlElement* audioSetupXml) noexcept
    {
        PersistedAudioEndpoints endpoints;

        if (audioSetupXml == nullptr)
            return endpoints;

        const auto legacyName = audioSetupXml->getStringAttribute("audioDeviceName");

        if (legacyName.isNotEmpty())
        {
            endpoints.inputDeviceName = legacyName;
            endpoints.outputDeviceName = legacyName;
            return endpoints;
        }

        endpoints.inputDeviceName = audioSetupXml->getStringAttribute("audioInputDeviceName");
        endpoints.outputDeviceName = audioSetupXml->getStringAttribute("audioOutputDeviceName");
        return endpoints;
    }
}
