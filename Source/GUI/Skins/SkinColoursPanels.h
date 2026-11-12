#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Panels
    {
        // Under-GUI diagnostic fill (plugin + standalone); normally covered by Header/Body/Footer.
        inline constexpr ColourElement kWindowSurfaceBackground = {
            skinColourKey(SkinObjectType::kWindowSurface, SkinColourProperty::kBackground),
            ColourChart::kGrey950,
            ColourChart::kGrey950,
            ColourChart::kCream400
        };

        inline constexpr ColourElement kHeaderPanelBackground = {
            skinColourKey(SkinObjectType::kHeaderPanel, SkinColourProperty::kBackground),
            ColourChart::kGrey950,
            ColourChart::kGrey950,
            ColourChart::kCream400
        };

        inline constexpr ColourElement kBodyPanelBackground = {
            skinColourKey(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground),
            ColourChart::kGrey900,
            ColourChart::kGrey900,
            ColourChart::kCream
        };

        inline constexpr ColourElement kFooterPanelBackground = {
            skinColourKey(SkinObjectType::kFooterPanel, SkinColourProperty::kBackground),
            ColourChart::kGrey950,
            ColourChart::kGrey950,
            ColourChart::kCream400
        };

        // Black chrome grey for Header cartouche badge/frame and Footer HELP chrome (Cream uses cafe tones).
        inline constexpr juce::uint32 kMainChromeGrey = ColourChart::kGrey300;

        inline constexpr ColourElement kHeaderPanelBorder = {
            skinColourKey(SkinObjectType::kHeaderPanel, SkinColourProperty::kBorder),
            kMainChromeGrey,
            kMainChromeGrey,
            ColourChart::kCafe800
        };

        inline constexpr ColourElement kFooterPanelBorder = {
            skinColourKey(SkinObjectType::kFooterPanel, SkinColourProperty::kBorder),
            kMainChromeGrey,
            kMainChromeGrey,
            ColourChart::kCafe900
        };

        // MIDI-queue alert badge cut-out text (fill uses FooterMessage Error text).
        inline constexpr ColourElement kFooterMessageErrorBackground = {
            skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kBackground, SkinColourState::kError),
            ColourChart::kBlack,
            ColourChart::kBlack,
            ColourChart::kBlack
        };

        // Modal chrome + durable close-cross glyph (wired by dialogs / Settings / About).
        inline constexpr ColourElement kDialogBorder = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kBorder),
            ColourChart::kGrey525,
            ColourChart::kGrey525,
            ColourChart::kCafe600
        };

        inline constexpr ColourElement kDialogTitleBand = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kTitleBand),
            ColourChart::kBlack,
            ColourChart::kBlack,
            ColourChart::kCream400
        };

        inline constexpr ColourElement kDialogGlyph = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kGlyph),
            ColourChart::kRed900,
            ColourChart::kRed900,
            ColourChart::kRed900
        };

        // Overlay dialog title drawn on the title band.
        inline constexpr ColourElement kDialogText = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kText),
            ColourChart::kGrey200,
            ColourChart::kGrey200,
            ColourChart::kCafe950
        };

        // Settings selected-tab label (fill stays Dialog TitleBand).
        inline constexpr ColourElement kDialogTextOn = {
            skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kText, SkinColourState::kOn),
            ColourChart::kGrey200,
            ColourChart::kGrey200,
            ColourChart::kCafe950
        };
    }
}
