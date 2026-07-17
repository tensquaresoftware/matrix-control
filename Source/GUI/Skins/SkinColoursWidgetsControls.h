#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace Button
        {
            inline constexpr ColourElement kBackground = {
                "ButtonBackground",
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                "ButtonBorder",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey3
            };

            inline constexpr ColourElement kText = {
                "ButtonText",
                ColourChart::kLightGrey2,
                ColourChart::kLightGrey2,
            };

            inline constexpr ColourElement kBackgroundHoover = {
                "ButtonBackgroundHoover",
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kTextHoover = {
                "ButtonTextHoover",
                ColourChart::kWhite,
                ColourChart::kWhite,
            };

            inline constexpr ColourElement kBackgroundClicked = {
                "ButtonBackgroundClicked",
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kTextClicked = {
                "ButtonTextClicked",
                ColourChart::kRed,
                ColourChart::kRed,
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                "ButtonBackgroundDisabled",
                ColourChart::kDarkGrey1,
                ColourChart::kDarkGrey1,
            };

            // Same chart values as enabled border (black/cream variants).
            inline constexpr ColourElement kBorderDisabled = {
                "ButtonBorderDisabled",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey3
            };

            inline constexpr ColourElement kTextDisabled = {
                "ButtonTextDisabled",
                Common::kContentDisabled,
                Common::kContentDisabled,
            };
        }

        namespace Toggle
        {
            inline constexpr ColourElement kBorder = {
                "ToggleBorder",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey3
            };

            inline constexpr ColourElement kBackgroundOff = {
                "ToggleBackgroundOff",
                ColourChart::kGreen1,
                ColourChart::kGreen1
            };

            inline constexpr ColourElement kTextOff = {
                "ToggleTextOff",
                ColourChart::kGreen3,
                ColourChart::kGreen3
            };

            inline constexpr ColourElement kBackgroundOn = {
                "ToggleBackgroundOn",
                ColourChart::kGreen2,
                ColourChart::kGreen2
            };

            inline constexpr ColourElement kTextOn = {
                "ToggleTextOn",
                ColourChart::kGreen4,
                ColourChart::kGreen4
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                "ToggleBackgroundDisabled",
                ColourChart::kDarkGrey1,
                ColourChart::kDarkGrey1,
            };

            // Same chart values as enabled border (black/cream variants).
            inline constexpr ColourElement kBorderDisabled = {
                "ToggleBorderDisabled",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey3
            };

            inline constexpr ColourElement kTextDisabled = {
                "ToggleTextDisabled",
                Common::kContentDisabled,
                Common::kContentDisabled,
            };
        }

        namespace Slider
        {
            inline constexpr ColourElement kTrack = {
                "SliderTrack",
                ColourChart::kGreen1,
                ColourChart::kGreen1
            };

            inline constexpr ColourElement kValueBar = {
                "SliderValueBar",
                ColourChart::kGreen3,
                ColourChart::kGreen3
            };

            inline constexpr ColourElement kText = {
                "SliderText",
                ColourChart::kGreen4,
                ColourChart::kGreen4,
            };

            inline constexpr ColourElement kFocusBorder = {
                "SliderFocusBorder",
                ColourChart::kGreen2,
                ColourChart::kGreen2
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                "SliderBackgroundDisabled",
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled
            };

            inline constexpr ColourElement kTrackDisabled = {
                "SliderTrackDisabled",
                Common::kDecorationDisabled,
                Common::kDecorationDisabled
            };

            inline constexpr ColourElement kValueBarDisabled = {
                "SliderValueBarDisabled",
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kTextDisabled = {
                "SliderTextDisabled",
                Common::kContentDisabled,
                Common::kContentDisabled,
            };
        }
    }
}
