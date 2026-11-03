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
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTextFocus = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus),
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kEditorBackground = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorBackground),
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kEditorSelectionBackground = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorSelectionBackground),
                ColourChart::kTransparent,
                ColourChart::kTransparent
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText),
                ColourChart::kWhite,
                ColourChart::kWhite
            };

            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kCaret),
                ColourChart::kWhite,
                ColourChart::kWhite
            };
        }

        namespace EnvelopeDisplay
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey700
            };

            inline constexpr ColourElement kEnvelope = {
                skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kEnvelope),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };
        }

        namespace TrackGeneratorDisplay
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey700
            };

            inline constexpr ColourElement kShaper = {
                skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kShaper),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };
        }

        namespace PatchNameDisplay
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBackground),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kText),
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            // Solid baked match for former primary.withAlpha(0.50f) over black display fill.
            inline constexpr ColourElement kSecondaryText = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kSecondaryText),
                ColourChart::kRed800,
                ColourChart::kRed800
            };

            // Caret-slot glyph ink over the red caret plate (absolute Black).
            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret),
                ColourChart::kBlack,
                ColourChart::kBlack
            };
        }

        namespace ReadOnlyValueField
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground),
                ColourChart::kGrey700,
                ColourChart::kGrey700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTextError = {
                skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText, SkinColourState::kError),
                ColourChart::kRed500,
                ColourChart::kRed500
            };
        }

        namespace Led
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOff),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBorder, SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kMaxOn = {
                skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };
        }

        namespace PeakIndicator
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kPeakIndicator,
                              SkinColourProperty::kBackground,
                              SkinColourState::kOff),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kPeakIndicator,
                              SkinColourProperty::kBorder,
                              SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kMaxOn = {
                skinColourKey(SkinObjectType::kPeakIndicator,
                              SkinColourProperty::kBackground,
                              SkinColourState::kOn),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };
        }
    }
}
