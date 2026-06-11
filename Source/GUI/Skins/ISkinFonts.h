#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkinFonts
    {
    public:
        virtual ~ISkinFonts() = default;

        virtual juce::Font getBaseFont() const = 0;
        virtual juce::Font getBaseFontBold() const = 0;
        virtual juce::Font getBrandFontBold() const = 0;
    };
}
