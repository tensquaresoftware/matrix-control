#pragma once

#include <limits>

#include <juce_core/juce_core.h>

namespace TSS
{
    /** Display-only left-band help state — never written to APVTS sticky properties. */
    class ContextualHelpOverlay
    {
    public:
        void setDetail(const juce::String& detailText)
        {
            detail_ = detailText;
        }

        void clear()
        {
            detail_.clear();
        }

        bool isActive() const
        {
            return detail_.isNotEmpty();
        }

        const juce::String& getDetail() const
        {
            return detail_;
        }

    private:
        juce::String detail_;
    };

    /** True when the left band should paint help instead of the sticky APVTS message. */
    inline bool shouldPaintContextualHelpOverSticky(bool overlayActive)
    {
        return overlayActive;
    }

    /**
     * Anti-flicker leave/blur gate: apply clear only if this delayed callback is still current
     * and no bound control is still hovered or focused.
     */
    inline bool shouldClearContextualHelpOverlay(int scheduledGeneration,
                                                 int currentGeneration,
                                                 bool anyBoundControlStillActive)
    {
        if (scheduledGeneration != currentGeneration)
            return false;

        return ! anyBoundControlStillActive;
    }

    /**
     * Keep the HELP overlay while a combo / logo popup is still in play
     * (focus inside the menu, or the menu still modal). Caller should reschedule clear.
     */
    inline bool shouldDeferContextualHelpClearForMutatorPopup(bool focusInsidePopup,
                                                              bool popupModalActive)
    {
        return focusInsidePopup || popupModalActive;
    }

    /** True when clear may drop the overlay because this caller still owns the epoch. */
    inline bool shouldClearContextualHelpOverlayForEpoch(int ownedEpoch, int currentEpoch)
    {
        return ownedEpoch > 0 && ownedEpoch == currentEpoch;
    }

    /** Monotonic overlay epoch with wrap at INT_MAX (same policy as FooterPanel). */
    inline int nextContextualHelpOverlayEpoch(int currentEpoch)
    {
        if (currentEpoch == std::numeric_limits<int>::max())
            return 1;

        return currentEpoch + 1;
    }

    /**
     * Epoch-scoped clear shared by FooterPanel and unit tests.
     * Returns true when the overlay was cleared.
     */
    inline bool clearContextualHelpOverlayDetailIfEpoch(ContextualHelpOverlay& overlay,
                                                        int ownedEpoch,
                                                        int currentEpoch)
    {
        if (! shouldClearContextualHelpOverlayForEpoch(ownedEpoch, currentEpoch))
            return false;

        if (! overlay.isActive())
            return false;

        overlay.clear();
        return true;
    }
}
