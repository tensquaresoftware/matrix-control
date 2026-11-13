#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/** Blue / Orange / Yellow ladders (BLACK/CHROMA + CREAM/CHROMA). */
struct ColourChartChroma
{
    // Blue: x00 = CREAM/CHROMA Patch; x50 = BLACK/CHROMA Patch.
    static constexpr juce::uint32 kBlue100     = 0xFFD3EBFB;
    static constexpr juce::uint32 kBlue200     = 0xFF96C7E9;
    static constexpr juce::uint32 kBlue400     = 0xFF5E99CD;
    static constexpr juce::uint32 kBlue450     = 0xFF28A0F8;
    static constexpr juce::uint32 kBlue500     = 0xFF2F8BDC;
    static constexpr juce::uint32 kBlue600     = 0xFF1A60B7;
    static constexpr juce::uint32 kBlue650     = 0xFF034FAD;
    static constexpr juce::uint32 kBlue700     = 0xFF0F56AC;
    static constexpr juce::uint32 kBlue750     = 0xFF053979;
    static constexpr juce::uint32 kBlue800     = 0xFF144682;
    static constexpr juce::uint32 kBlue950     = 0xFF012959;

    // Orange: x00 + Orange550 = CREAM/CHROMA Master; 350/650/750 = BLACK/CHROMA Master.
    static constexpr juce::uint32 kOrange100   = 0xFFF9E6CA;
    static constexpr juce::uint32 kOrange200   = 0xFFE5BE86;
    static constexpr juce::uint32 kOrange350   = 0xFFEE8B21;
    static constexpr juce::uint32 kOrange400   = 0xFFC08B4B;
    /** Footer WARNING badge — pumpkin red-orange, distinct from Master BrandOrange. */
    static constexpr juce::uint32 kOrange500   = 0xFFD44818;
    static constexpr juce::uint32 kOrange550   = 0xFFE5942F;
    static constexpr juce::uint32 kOrange600   = 0xFFBF5F0E;
    static constexpr juce::uint32 kOrange650   = 0xFF854408;
    static constexpr juce::uint32 kOrange700   = 0xFFAC560C;
    static constexpr juce::uint32 kOrange750   = 0xFF7A4511;
    static constexpr juce::uint32 kOrange800   = 0xFF7C4215;
    static constexpr juce::uint32 kOrange950   = 0xFF422514;

    // Yellow: x00 = CREAM/CHROMA Header; 300/650/850 = BLACK/CHROMA Header.
    static constexpr juce::uint32 kYellow100   = 0xFFF8F0CD;
    static constexpr juce::uint32 kYellow200   = 0xFFF3DF7A;
    static constexpr juce::uint32 kYellow300   = 0xFFFFCC00;
    static constexpr juce::uint32 kYellow400   = 0xFFC7AB2D;
    static constexpr juce::uint32 kYellow500   = 0xFFD9B404;
    static constexpr juce::uint32 kYellow600   = 0xFF9D7D04;
    static constexpr juce::uint32 kYellow650   = 0xFF806600;
    static constexpr juce::uint32 kYellow700   = 0xFF907204;
    static constexpr juce::uint32 kYellow800   = 0xFF735C03;
    static constexpr juce::uint32 kYellow850   = 0xFF5C4A00;
    static constexpr juce::uint32 kYellow950   = 0xFF3D3208;
};
