#pragma once

// Shared integer column strip for Matrix Mod bus header labels and bus cell controls.
// Successive scaledInt widths + gaps — no float step accumulation across columns.
// Destination design width differs: header text 68 vs cell combo 104 (same destination X origin
// when preceding columns match).

#include "GUI/Layout/ScaledLayout.h"

namespace TSS
{
struct ModulationBusColumnStrip
{
    int busNumberX = 0;
    int busNumberW = 0;
    int sourceX = 0;
    int sourceW = 0;
    int amountX = 0;
    int amountW = 0;
    int destinationX = 0;
    int destinationW = 0;
    int initX = 0;
    int initW = 0;
};

inline ModulationBusColumnStrip computeModulationBusColumnStrip(float uiScale,
                                                                int designBusNumberW,
                                                                int designSourceW,
                                                                int designAmountW,
                                                                int designDestinationW,
                                                                int designInitW,
                                                                int designGap)
{
    ModulationBusColumnStrip strip;
    const int gap = ScaledLayout::scaledInt(static_cast<float>(designGap), uiScale);

    strip.busNumberW = ScaledLayout::scaledInt(static_cast<float>(designBusNumberW), uiScale);
    strip.sourceW = ScaledLayout::scaledInt(static_cast<float>(designSourceW), uiScale);
    strip.amountW = ScaledLayout::scaledInt(static_cast<float>(designAmountW), uiScale);
    strip.destinationW = ScaledLayout::scaledInt(static_cast<float>(designDestinationW), uiScale);
    strip.initW = ScaledLayout::scaledInt(static_cast<float>(designInitW), uiScale);

    int x = 0;
    strip.busNumberX = x;
    x += strip.busNumberW + gap;
    strip.sourceX = x;
    x += strip.sourceW + gap;
    strip.amountX = x;
    x += strip.amountW + gap;
    strip.destinationX = x;
    x += strip.destinationW + gap;
    strip.initX = x;

    return strip;
}
}
