#pragma once

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
     * and no Mutator control is still hovered or focused.
     */
    inline bool shouldClearContextualHelpOverlay(int scheduledGeneration,
                                                 int currentGeneration,
                                                 bool anyMutatorControlStillActive)
    {
        if (scheduledGeneration != currentGeneration)
            return false;

        return ! anyMutatorControlStillActive;
    }
}
