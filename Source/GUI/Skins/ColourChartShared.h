#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/** Brand plates, greys, reds — shared across product skin families. */
struct ColourChartShared
{
    static constexpr juce::uint32 kTransparent = 0x00000000;
    static constexpr juce::uint32 kBlack       = 0xFF000000;
    static constexpr juce::uint32 kWhite       = 0xFFFFFFFF;

    /** Brand plate / logo cream (formerly kCream). */
    static constexpr juce::uint32 kBrandCream  = 0xFFE9E8D5;
    /** Brand primary blue (formerly kBlue). */
    static constexpr juce::uint32 kBrandBlue   = 0xFF004FB0;
    /** Brand primary orange (formerly kOrange). */
    static constexpr juce::uint32 kBrandOrange = 0xFFF87A00;

    static constexpr juce::uint32 kGrey950     = 0xFF151515;
    static constexpr juce::uint32 kGrey900     = 0xFF202020;
    static constexpr juce::uint32 kGrey800     = 0xFF303030;
    static constexpr juce::uint32 kGrey700     = 0xFF404040;
    static constexpr juce::uint32 kGrey600     = 0xFF505050;
    /** Formerly kGrey525 — same RGB, 50-step name. */
    static constexpr juce::uint32 kGrey550     = 0xFF5E5E5E;
    static constexpr juce::uint32 kGrey500     = 0xFF606060;
    static constexpr juce::uint32 kGrey400     = 0xFF707070;
    /** Midway HELP / INFO chrome. */
    static constexpr juce::uint32 kGrey350     = 0xFF888888;
    static constexpr juce::uint32 kGrey300     = 0xFF999999;
    static constexpr juce::uint32 kGrey200     = 0xFFB9B9B9;
    static constexpr juce::uint32 kGrey100     = 0xFFDDDDDD;

    static constexpr juce::uint32 kRed900      = 0xFF9A131D;
    /** Solid match for primary red at ~50% over black (PatchNameDisplay secondary). */
    static constexpr juce::uint32 kRed800      = 0xFF800000;
    /** Cream secondary red (Mxx-Ryy) — same hue family as Red600, not maroon Red800. */
    static constexpr juce::uint32 kRed700      = 0xFFA84A4A;
    /** Cream primary accent — softer than pure Red500 on Cafe plates. */
    static constexpr juce::uint32 kRed600      = 0xFFC62828;
    static constexpr juce::uint32 kRed500      = 0xFFFF0000;
};
