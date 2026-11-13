#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/** Cafe ladder (CREAM family plates). */
struct ColourChartCafe
{
    static constexpr juce::uint32 kCafe50      = 0xFFD9CFB8; // was Cream400
    static constexpr juce::uint32 kCafe150     = 0xFFD0C6B0; // was Cream450
    /** Mid fill between Cafe150 and Cafe300 (CREAM button hover). */
    static constexpr juce::uint32 kCafe200     = 0xFFC2BAA3;
    static constexpr juce::uint32 kCafe300     = 0xFFB5AF96; // was Cafe400 / Cream600
    static constexpr juce::uint32 kCafe400     = 0xFF9A9480; // was Cafe600 / Cream700
    static constexpr juce::uint32 kCafe500     = 0xFF8A826C; // was Cafe700 / Cream750
    static constexpr juce::uint32 kCafe600     = 0xFF78705A; // was Cream775
    static constexpr juce::uint32 kCafe700     = 0xFF5C5746; // was Cafe800 / Cream800
    static constexpr juce::uint32 kCafe750     = 0xFF3D3A2E; // was Cafe900 / Cream900
    static constexpr juce::uint32 kCafe850     = 0xFF2E2C24; // was Cafe950 / Cream950
    static constexpr juce::uint32 kCafe950     = 0xFF1A1712; // was Cream985
};
