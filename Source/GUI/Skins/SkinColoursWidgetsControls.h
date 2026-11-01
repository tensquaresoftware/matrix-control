#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace Button
        {
            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOff),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTextOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOff),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kBackgroundOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorderOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTextOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOn),
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kHover),
                ColourChart::kWhite,
                ColourChart::kWhite
            };

            inline constexpr ColourElement kBackgroundClicked = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kClicked),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kTextClicked = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kClicked),
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        namespace Toggle
        {
            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOff),
                ColourChart::kEmerald950,
                ColourChart::kEmerald950
            };

            inline constexpr ColourElement kTextOff = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOff),
                ColourChart::kEmerald700,
                ColourChart::kEmerald700
            };

            inline constexpr ColourElement kBackgroundOn = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kEmerald900,
                ColourChart::kEmerald900
            };

            inline constexpr ColourElement kTextOn = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOn),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        namespace Slider
        {
            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack),
                ColourChart::kEmerald950,
                ColourChart::kEmerald950
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar),
                ColourChart::kEmerald700,
                ColourChart::kEmerald700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder),
                ColourChart::kEmerald900,
                ColourChart::kEmerald900
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled),
                Common::kDecorationDisabled,
                Common::kDecorationDisabled
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
                ColourChart::kGrey700,
                ColourChart::kGrey700
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        /** Former SliderButtonLike pastilles — seeded onto Slider at ColourSet::k4 only. */
        namespace SliderK4
        {
            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };
        }
    }
}
