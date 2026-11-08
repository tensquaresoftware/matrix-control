#pragma once

#include <vector>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    /** Compiled ColourChart entries as named pastilles (seed for Dev skins). */
    std::vector<Pastille> colourChartPastilles();

    /** Resolve ARGB to a ColourChart pastille name, or empty if unknown. */
    juce::String pastilleNameForArgb(juce::uint32 argb);

    std::optional<juce::uint32> argbForPastilleName(const std::vector<Pastille>& palette,
                                                    const juce::String& name);
}
