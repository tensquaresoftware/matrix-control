#pragma once

#include <algorithm>

#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    /** Sticky footer severity ids used by auto-clear policy (APVTS string mapping). */
    enum class StickyMessageSeverity
    {
        None = 0,
        Info,
        Warning,
        Error
    };

    /** Clamp invalid / missing preference ids to KEEP (session compatibility). */
    inline int normalizeInfoMessagePreference(int preferenceRaw)
    {
        using namespace PluginIDs::Settings::InfoMessage;

        if (preferenceRaw == kKeep || preferenceRaw == kAutoClear)
            return preferenceRaw;

        return kDefault;
    }

    /** True when preference is AUTO CLEAR after normalize. */
    inline bool isInfoMessageAutoClearEnabled(int preferenceRaw)
    {
        return normalizeInfoMessagePreference(preferenceRaw)
               == PluginIDs::Settings::InfoMessage::kAutoClear;
    }

    /** AUTO CLEAR applies only to sticky INFO; WARNING / ERROR never auto-clear. */
    inline bool shouldAutoClearStickySeverity(StickyMessageSeverity severity, int preferenceRaw)
    {
        if (severity != StickyMessageSeverity::Info)
            return false;

        return isInfoMessageAutoClearEnabled(preferenceRaw);
    }

    /** Remaining delay after wall-clock elapsed; never negative. */
    inline int remainingAutoClearDelayMs(int remainingAtStartMs, int elapsedMs)
    {
        return std::max(0, remainingAtStartMs - std::max(0, elapsedMs));
    }

    /** Pause while a furtive HELP overlay covers the sticky band. */
    inline bool shouldPauseAutoClearForHelp(bool autoClearArmed, bool helpCoversSticky)
    {
        return autoClearArmed && helpCoversSticky;
    }

    /** Resume remaining delay when HELP no longer covers and a pause held remainder. */
    inline bool shouldResumeAutoClearAfterHelp(bool autoClearArmed,
                                               bool wasPaused,
                                               bool helpCoversSticky,
                                               int remainingMs)
    {
        return autoClearArmed && wasPaused && ! helpCoversSticky && remainingMs > 0;
    }

    /** Fire clear only when remaining delay is exhausted and HELP is not covering. */
    inline bool shouldFireAutoClear(int remainingMs, bool helpCoversSticky)
    {
        return remainingMs <= 0 && ! helpCoversSticky;
    }

    /** Decision when HELP uncovers sticky while an AUTO CLEAR arm may still be live. */
    enum class AutoClearAfterHelpUncoverDecision
    {
        None = 0,
        ResumeRemaining,
        FireClear
    };

    inline AutoClearAfterHelpUncoverDecision decideAutoClearAfterHelpUncover(
        bool autoClearArmed,
        bool wasPaused,
        bool helpCoversSticky,
        int remainingMs)
    {
        if (shouldResumeAutoClearAfterHelp(autoClearArmed,
                                           wasPaused,
                                           helpCoversSticky,
                                           remainingMs))
            return AutoClearAfterHelpUncoverDecision::ResumeRemaining;

        if (autoClearArmed && shouldFireAutoClear(remainingMs, helpCoversSticky))
            return AutoClearAfterHelpUncoverDecision::FireClear;

        return AutoClearAfterHelpUncoverDecision::None;
    }

    /**
     * Armed timer fired while HELP covers sticky: defer clear, pause with remaining 0.
     * Caller must not clear sticky until HELP uncovers (then remaining 0 fires immediately).
     */
    struct AutoClearTimerFireWhileHelpState
    {
        bool deferred = false;
        bool paused = false;
        int remainingMs = 0;
    };

    inline AutoClearTimerFireWhileHelpState autoClearTimerFireWhileHelpCovers(bool autoClearArmed,
                                                                             bool helpCoversSticky)
    {
        if (! autoClearArmed || ! helpCoversSticky)
            return {};

        return { true, true, 0 };
    }

    /** Sticky severity badge hit-zone is shown only when sticky paints (not under HELP). */
    inline bool shouldShowStickySeverityBadgeHitArea(bool hasStickyMessage, bool helpCoversSticky)
    {
        return hasStickyMessage && ! helpCoversSticky;
    }
}
