#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace NumberBox
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kTextFocus = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kEditorBackground = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorBackground),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kBrandCream
            };

            // Opaque chart colours (not Transparent): match editor fill so selection
            // stays visually quiet on BLACK*, same idea as Cafe500 on CREAM BrandCream.
            inline constexpr ColourElement kEditorSelectionBackground = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorSelectionBackground),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kCafe500
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kCaret),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kWhite
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };
        }

        namespace EnvelopeDisplay
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kEnvelope = {
                skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kEnvelope),
                ColourChart::kGreen300,
                ColourChart::kBlue400,
                ColourChart::kGreen600
            };
        }

        namespace TrackGeneratorDisplay
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kShaper = {
                skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kShaper),
                ColourChart::kGreen300,
                ColourChart::kBlue400,
                ColourChart::kGreen600
            };
        }

        namespace PatchNameDisplay
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kText),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            // Solid baked match for former primary.withAlpha(0.50f) over black display fill.
            inline constexpr ColourElement kSecondaryText = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kSecondaryText),
                ColourChart::kRed800,
                ColourChart::kRed800,
                ColourChart::kRed800
            };

            // Caret-slot glyph ink over the red caret plate (absolute Black).
            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe850
            };
        }

        namespace ReadOnlyValueField
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kTextError = {
                skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText, SkinColourState::kError),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kCafe850
            };
        }

        /** Led MCS: k1 Header (this namespace), k2 Modales. */
        namespace Led
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOff),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBorder, SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kMaxOn = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kGreen300,
                ColourChart::kYellow400,
                ColourChart::kGreen500
            };
        }

        namespace LedK2
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground,
                              SkinColourState::kOff, ColourSet::k2),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kMaxOn = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground,
                              SkinColourState::kOn, ColourSet::k2),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBorder,
                              SkinColourState::kOff, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };
        }

        /** PeakIndicator MCS: k1 Header (this namespace), k2 Modales. */
        namespace PeakIndicator
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kPeakIndicator,
                              SkinColourProperty::kBackground,
                              SkinColourState::kOff),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kPeakIndicator,
                              SkinColourProperty::kBorder,
                              SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kMaxOn = {
                skinColourKey(SkinObjectType::kPeakIndicator,
                              SkinColourProperty::kBackground,
                              SkinColourState::kOn),
                ColourChart::kGreen300,
                ColourChart::kYellow400,
                ColourChart::kGreen500
            };
        }

        namespace PeakIndicatorK2
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground,
                              SkinColourState::kOff, ColourSet::k2),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kMaxOn = {
                skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground,
                              SkinColourState::kOn, ColourSet::k2),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBorder,
                              SkinColourState::kOff, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };
        }
    }
}
