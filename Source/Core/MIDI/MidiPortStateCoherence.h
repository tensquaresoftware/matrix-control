#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Shared/Definitions/PluginDisplayNames.h"

namespace Core
{
    /** After a port open attempt: APVTS / combo must mirror a successful open, else the
        currently open device id (empty = none sentinel). */
    inline juce::String coherentPortIdAfterOpenAttempt(bool openSucceeded,
                                                       const juce::String& requestedId,
                                                       const juce::String& currentlyOpenId) noexcept
    {
        if (openSucceeded)
            return requestedId;

        return currentlyOpenId;
    }

    /** Standalone: MIDI From and Keyboard From must not share the same input device id. */
    inline bool isMidiFromKeyboardFromConflict(const juce::String& midiFromId,
                                               const juce::String& keyboardFromId) noexcept
    {
        return midiFromId.isNotEmpty()
            && keyboardFromId.isNotEmpty()
            && midiFromId == keyboardFromId;
    }

    /** Exact-string clear of the same-device conflict footer (leave unrelated footers alone). */
    inline void clearMidiFromKeyboardFromConflictFooterIfPresent(juce::ValueTree& state)
    {
        if (state.getProperty("uiMessageText").toString()
            != juce::String(PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter))
        {
            return;
        }

        state.setProperty("uiMessageText", juce::String(), nullptr);
        state.setProperty("uiMessageSeverity", juce::String(), nullptr);
    }

    /** Option 2 (V1.2): when alignToOpenReality is true (final / reporting sync), write the
        coherent port id into APVTS. Soft intermediate retries pass false to keep the desired id. */
    inline void maybeAlignApvtsPortIdAfterOpenAttempt(juce::ValueTree& state,
                                                      const juce::Identifier& property,
                                                      bool alignToOpenReality,
                                                      bool openSucceeded,
                                                      const juce::String& requestedId,
                                                      const juce::String& currentlyOpenId)
    {
        if (! alignToOpenReality)
            return;

        const auto coherentId = coherentPortIdAfterOpenAttempt(openSucceeded, requestedId, currentlyOpenId);
        if (coherentId != state.getProperty(property).toString())
            state.setProperty(property, coherentId, nullptr);
    }

    /** UI soft dead-port policy: only attempt force-reopen when presence says not live. */
    [[nodiscard]] inline bool shouldForceReopenForUiRefresh(const juce::String& desiredId,
                                                            bool identifierStillLive) noexcept
    {
        return desiredId.isNotEmpty() && ! identifierStillLive;
    }

    /** After a force-reopen attempt: clear APVTS only when reopen failed (fail-open if reopen ok). */
    [[nodiscard]] inline bool shouldClearDeadPortAfterReopen(const juce::String& desiredId,
                                                            bool reopenSucceeded) noexcept
    {
        return desiredId.isNotEmpty() && ! reopenSucceeded;
    }
}
