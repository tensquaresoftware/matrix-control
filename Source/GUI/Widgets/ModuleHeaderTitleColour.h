#pragma once

#include <juce_graphics/juce_graphics.h>

namespace TSS
{
    // Shared by ModuleHeader paint and unit tests (ModuleHeader Text / Hover token).
    inline juce::Colour resolveModuleHeaderTitleTextColour(bool titleClickEnabled,
                                                           bool titleHovered,
                                                           juce::Colour text,
                                                           juce::Colour textHover)
    {
        return titleClickEnabled && titleHovered ? textHover : text;
    }

    // Trial: clickable-title hover also retints the deco line to match the title.
    inline juce::Colour resolveModuleHeaderDecoLineColour(bool titleClickEnabled,
                                                          bool titleHovered,
                                                          juce::Colour textHover,
                                                          juce::Colour lineDefault)
    {
        return titleClickEnabled && titleHovered ? textHover : lineDefault;
    }
}
