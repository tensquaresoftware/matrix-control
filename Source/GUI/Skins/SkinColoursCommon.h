#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ColourChart.h"
#include "SkinValues.h"

namespace TSS::SkinColours
{
    struct ColourElement
    {
        SkinColourKey key;
        juce::uint32 blackVariant;
        juce::uint32 creamVariant;
    };

    namespace Common
    {
        // Shared disabled defaults. Buttons/toggles use Grey950 backgrounds and
        // enabled-border colours locally; standard combos and slider fill use Grey800 here.
        static constexpr juce::uint32 kBackgroundDisabled = ColourChart::kGrey800;
        static constexpr juce::uint32 kBorderDisabled = ColourChart::kGrey700;
        static constexpr juce::uint32 kDecorationDisabled = ColourChart::kGrey900;
        static constexpr juce::uint32 kContentDisabled = ColourChart::kGrey600;
        static constexpr juce::uint32 kContentHover = ColourChart::kGrey950;
    }
}
