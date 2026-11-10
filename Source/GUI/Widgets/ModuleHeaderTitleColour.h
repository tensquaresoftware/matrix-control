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
}
