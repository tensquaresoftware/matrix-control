#pragma once

#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Helpers/ComboBoxLiveRefresh.h"

namespace TSS::AudioFromComboItemSet
{
    struct ComboTexts
    {
        int numItems = 0;
        std::vector<juce::String> itemTexts;
    };

    /** Pure skip check for Audio From rebuild (ids + count + uppercase channel labels). */
    [[nodiscard]] inline bool itemSetUnchanged(const std::vector<juce::String>& currentIds,
                                               const std::vector<juce::String>& nextIds,
                                               const juce::StringArray& channelNames,
                                               const ComboTexts& combo) noexcept
    {
        if (! ComboBoxLiveRefresh::identifiersEqual(currentIds, nextIds))
            return false;

        if (combo.numItems != static_cast<int>(nextIds.size()) + 1)
            return false;

        if (static_cast<int>(combo.itemTexts.size()) != combo.numItems)
            return false;

        const int count = static_cast<int>(nextIds.size());
        for (int i = 0; i < count; ++i)
        {
            if (i >= channelNames.size())
                return false;

            if (combo.itemTexts[static_cast<size_t>(i + 1)] != channelNames[i].toUpperCase())
                return false;
        }

        return true;
    }
}
