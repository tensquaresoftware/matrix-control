#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct ColourChart
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

    // Unified Green ladder (ex-Emerald + CREAM greens), luminance-ordered; RGB preserved.
    static constexpr juce::uint32 kGreen100    = 0xFFC8F5D8;
    static constexpr juce::uint32 kGreen200    = 0xFFBDD9B8;
    /** Formerly kEmerald400. */
    static constexpr juce::uint32 kGreen300    = 0xFF00DDAD;
    static constexpr juce::uint32 kGreen400    = 0xFF7AB896;
    static constexpr juce::uint32 kGreen500    = 0xFF39BD74;
    static constexpr juce::uint32 kGreen600    = 0xFF2F8F62;
    static constexpr juce::uint32 kGreen700    = 0xFF247A52;
    /** Formerly kEmerald700. */
    static constexpr juce::uint32 kGreen750    = 0xFF00785E;
    static constexpr juce::uint32 kGreen800    = 0xFF1F6B48;
    /** Formerly kEmerald900. */
    static constexpr juce::uint32 kGreen900    = 0xFF06471C;
    /** Formerly kEmerald950. */
    static constexpr juce::uint32 kGreen950    = 0xFF002D0E;

    static constexpr juce::uint32 kBlue950     = 0xFF012959;
    static constexpr juce::uint32 kBlue800     = 0xFF053979;
    /** BLACK/CHROMA Patch fills (from trial BLACK/DUO 4). */
    static constexpr juce::uint32 kBlue600     = 0xFF034FAD;
    static constexpr juce::uint32 kBlue400     = 0xFF28A0F8;

    static constexpr juce::uint32 kOrange950   = 0xFF422514;
    static constexpr juce::uint32 kOrange800   = 0xFF7A4511;
    /** BLACK/CHROMA Master fills / accents (from trial BLACK/DUO 4). */
    static constexpr juce::uint32 kOrange700   = 0xFF854408;
    /** Footer WARNING badge — pumpkin red-orange, distinct from Master BrandOrange. */
    static constexpr juce::uint32 kOrange500   = 0xFFD44818;
    static constexpr juce::uint32 kOrange400   = 0xFFEE8B21;

    /** BLACK/CHROMA Header yellows (warm #FFCC00 family from trial BLACK/DUO 4). */
    static constexpr juce::uint32 kYellow950   = 0xFF3D3208;
    static constexpr juce::uint32 kYellow800   = 0xFF5C4A00;
    static constexpr juce::uint32 kYellow700   = 0xFF806600;
    static constexpr juce::uint32 kYellow400   = 0xFFFFCC00;

    // Cafe ladder (ex-Cream* + former Cafe*), luminance-ordered; RGB preserved.
    static constexpr juce::uint32 kCafe50      = 0xFFD9CFB8; // was Cream400
    static constexpr juce::uint32 kCafe150     = 0xFFD0C6B0; // was Cream450
    /** Mid fill between Cafe150 and Cafe300 (CREAM button hover); hue aligned on the Cafe ladder. */
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
