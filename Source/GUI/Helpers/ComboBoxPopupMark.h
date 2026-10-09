#pragma once

#include <juce_core/juce_core.h>

namespace TSS::ComboBoxPopupMark
{
    /** Open-list label: append suffix only when itemId matches the marked id. */
    [[nodiscard]] inline juce::String labelForItem(const juce::String& itemText,
                                                   int itemId,
                                                   int markedItemId,
                                                   const juce::String& suffix)
    {
        if (markedItemId == 0 || suffix.isEmpty() || itemId != markedItemId)
            return itemText;

        return itemText + suffix;
    }
}
