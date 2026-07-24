#pragma once

// Shared integer equal-width strip for Patch Manager modules (bank buttons, memory buttons, toggles).
// Successive scaledInt widths + gaps — no float step × index accumulation.
// Fixed-width policy (no remainder absorption) preserves design X at 100 %.

#include "GUI/Layout/ScaledLayout.h"

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
// Places equal-width controls left-to-right starting at startX.
// Returns X immediately after the last control (no trailing gap).
inline int placeEqualWidthStrip(int startX,
                                int y,
                                float uiScale,
                                int designWidth,
                                int designHeight,
                                int designGap,
                                juce::Component* const* controls,
                                int count)
{
    const int w = ScaledLayout::scaledInt(static_cast<float>(designWidth), uiScale);
    const int h = ScaledLayout::scaledInt(static_cast<float>(designHeight), uiScale);
    const int gap = ScaledLayout::scaledInt(static_cast<float>(designGap), uiScale);

    int x = startX;
    for (int i = 0; i < count; ++i)
    {
        if (controls[i] != nullptr)
            controls[i]->setBounds(x, y, w, h);

        x += w;
        if (i + 1 < count)
            x += gap;
    }

    return x;
}
}
