#pragma once

#include "GUI/Skins/ColourChartCafe.h"
#include "GUI/Skins/ColourChartChroma.h"
#include "GUI/Skins/ColourChartGreen.h"
#include "GUI/Skins/ColourChartShared.h"

/**
 * Product colour constants facade.
 * Split by family for ownership (Shared / Green / Chroma / Cafe); call sites keep ColourChart::k*.
 */
struct ColourChart : ColourChartShared, ColourChartGreen, ColourChartChroma, ColourChartCafe
{
};
