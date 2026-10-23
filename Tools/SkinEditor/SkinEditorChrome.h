#pragma once

#include <juce_graphics/juce_graphics.h>

namespace SkinEditorChrome
{
    // Photocraft / Skin Studio gutter spirit (greys, not Excel thickness).
    inline juce::Colour surfaceApp() { return juce::Colour(0xff151515); }
    inline juce::Colour surfacePanel() { return juce::Colour(0xff202020); }
    inline juce::Colour surfaceGutter() { return juce::Colour(0xff101010); }
    inline juce::Colour surfaceBand() { return juce::Colour(0xff1a1a1a); }
    inline juce::Colour textPrimary() { return juce::Colour(0xffdddddd); }
    inline juce::Colour textMuted() { return juce::Colour(0xff999999); }
    inline juce::Colour accent() { return juce::Colour(0xff00ddad); }

    inline constexpr int kGutter = 6;
    inline constexpr int kPanelPad = 8;
    inline constexpr int kBandHeight = 118;
}
