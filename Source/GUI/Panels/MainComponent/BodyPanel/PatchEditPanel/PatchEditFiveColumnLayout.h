#pragma once

// Shared integer five-column placement for Patch Edit Top / Displays / Bottom rows.
// Successive removeFromLeft + ScaledLayout::scaledInt — no float step × index accumulation.
// Last column absorbs remainder width (closes ±1–2 px rounding vs parent; at 100 % equals design 152).

#include "GUI/Layout/ScaledLayout.h"

#include <array>
#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
inline void layoutPatchEditFiveColumns(juce::Rectangle<int> area,
                                       float uiScale,
                                       int designChildWidth,
                                       int designChildHeight,
                                       int designInterModuleGap,
                                       const std::array<juce::Component*, 5>& children)
{
    const int childW = ScaledLayout::scaledInt(static_cast<float>(designChildWidth), uiScale);
    const int childH = ScaledLayout::scaledInt(static_cast<float>(designChildHeight), uiScale);
    const int gap = ScaledLayout::scaledInt(static_cast<float>(designInterModuleGap), uiScale);

    for (int i = 0; i < 4; ++i)
    {
        auto* child = children[static_cast<size_t>(i)];
        const auto col = area.removeFromLeft(childW).withHeight(childH);
        if (child != nullptr)
            child->setBounds(col);
        area.removeFromLeft(gap);
    }

    auto* last = children[4];
    if (last != nullptr)
        last->setBounds(area.withHeight(childH));
}
}
