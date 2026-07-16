#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ColourChart.h"

namespace TSS::SkinColours
{
    struct ColourElement
    {
        const char* elementName;
        juce::uint32 blackVariant;
        juce::uint32 creamVariant;
    };

    namespace Common
    {
        // Shared disabled defaults. Buttons/toggles/ButtonLike use Grey2 backgrounds and
        // enabled-border colours locally; standard combos and slider fond use Grey3 here.
        static constexpr juce::uint32 kBackgroundDisabled = ColourChart::kDarkGrey3;
        static constexpr juce::uint32 kBorderDisabled = ColourChart::kDarkGrey4;
        static constexpr juce::uint32 kDecorationDisabled = ColourChart::kDarkGrey2;
        static constexpr juce::uint32 kContentDisabled = ColourChart::kDarkGrey4;
        static constexpr juce::uint32 kContentHoover = ColourChart::kDarkGrey1;
    }
}
