#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct ColourChart
{
    static constexpr juce::uint32 kTransparent = 0x00000000;
    static constexpr juce::uint32 kBlack       = 0xFF000000;
    static constexpr juce::uint32 kWhite       = 0xFFFFFFFF;
    static constexpr juce::uint32 kCream       = 0xFFE9E8D5;

    static constexpr juce::uint32 kGrey950     = 0xFF151515;
    static constexpr juce::uint32 kGrey900     = 0xFF202020;
    static constexpr juce::uint32 kGrey800     = 0xFF303030;
    static constexpr juce::uint32 kGrey700     = 0xFF404040;
    static constexpr juce::uint32 kGrey600     = 0xFF505050;
    static constexpr juce::uint32 kGrey525     = 0xFF5E5E5E;
    static constexpr juce::uint32 kGrey500     = 0xFF606060;
    static constexpr juce::uint32 kGrey400     = 0xFF707070;
    /** Midway between former HELP #B0B0B0 and sticky INFO #606060 — furtive HELP chrome. */
    static constexpr juce::uint32 kGrey350     = 0xFF888888;
    static constexpr juce::uint32 kGrey300     = 0xFF999999;
    static constexpr juce::uint32 kGrey200     = 0xFFB9B9B9;
    static constexpr juce::uint32 kGrey100     = 0xFFDDDDDD;

    static constexpr juce::uint32 kRed900      = 0xFF9A131D;
    /** Solid match for primary red at ~50% over black (PatchNameDisplay secondary). */
    static constexpr juce::uint32 kRed800      = 0xFF800000;
    static constexpr juce::uint32 kRed500      = 0xFFFF0000;
    static constexpr juce::uint32 kOrange      = 0xFFF87A00;
    static constexpr juce::uint32 kYellow      = 0xFFFFFF00;
    static constexpr juce::uint32 kBlue        = 0xFF004FB0;

    static constexpr juce::uint32 kEmerald950  = 0xFF002D0E;
    static constexpr juce::uint32 kEmerald900  = 0xFF06471C;
    static constexpr juce::uint32 kEmerald700  = 0xFF00785E;
    static constexpr juce::uint32 kEmerald400  = 0xFF00DDAD;

    static constexpr juce::uint32 kBlue950     = 0xFF012959;
    static constexpr juce::uint32 kBlue800     = 0xFF053979;
    static constexpr juce::uint32 kBlue600     = 0xFF0A6E99;
    static constexpr juce::uint32 kBlue400     = 0xFF34C4F4;

    static constexpr juce::uint32 kOrange950   = 0xFF422514;
    static constexpr juce::uint32 kOrange800   = 0xFF7A4511;
    static constexpr juce::uint32 kOrange700   = 0xFF904809;
    static constexpr juce::uint32 kOrange400   = 0xFFF49325;

    static constexpr juce::uint32 kYellow950   = 0xFF3B360D;
    static constexpr juce::uint32 kYellow800   = 0xFF77770D;
    static constexpr juce::uint32 kYellow700   = 0xFF937910;
    static constexpr juce::uint32 kYellow400   = 0xFFECC93C;

    static constexpr juce::uint32 kCream400    = 0xFFD9CFB8;

    static constexpr juce::uint32 kCafe400     = 0xFFB5AF96;
    static constexpr juce::uint32 kCafe600     = 0xFF9A9480;
    static constexpr juce::uint32 kCafe700     = 0xFF8A826C;
    static constexpr juce::uint32 kCafe800     = 0xFF5C5746;
    static constexpr juce::uint32 kCafe900     = 0xFF3D3A2E;
    static constexpr juce::uint32 kCafe950     = 0xFF2E2C24;

    static constexpr juce::uint32 kGreen100    = 0xFFC8F5D8;
    static constexpr juce::uint32 kGreen200    = 0xFFBDD9B8;
    static constexpr juce::uint32 kGreen400    = 0xFF7AB896;
    static constexpr juce::uint32 kGreen500    = 0xFF39BD74;
    static constexpr juce::uint32 kGreen600    = 0xFF2F8F62;
    static constexpr juce::uint32 kGreen700    = 0xFF247A52;
    static constexpr juce::uint32 kGreen800    = 0xFF1F6B48;
};
