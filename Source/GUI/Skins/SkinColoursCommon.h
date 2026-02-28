#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "ColourChart.h"

namespace tss::SkinColours
{
    struct ColourElement
    {
        const char* elementName;
        juce::uint32 blackVariant;
        juce::uint32 creamVariant;
    };

    namespace Common
    {
        static constexpr juce::uint32 kBackgroundDisabled = ColourChart::kDarkGrey3;
        static constexpr juce::uint32 kBorderDisabled = ColourChart::kDarkGrey4;
        static constexpr juce::uint32 kDecorationDisabled = ColourChart::kDarkGrey5;
        static constexpr juce::uint32 kContentDisabled = ColourChart::kDarkGrey5;
        static constexpr juce::uint32 kContentHoover = ColourChart::kDarkGrey1;
    }
}
