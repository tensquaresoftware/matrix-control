#pragma once

#include "SkinColoursCommon.h"

namespace tss::SkinColours
{
    namespace Widgets
    {
        namespace Button
        {
            inline constexpr ColourElement kBackground = {
                "ButtonBackground",
                ColourChart::kBlack,
                ColourChart::kLightGrey2
            };

            inline constexpr ColourElement kBorder = {
                "ButtonBorder",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey4
            };

            inline constexpr ColourElement kText = {
                "ButtonText",
                ColourChart::kLightGrey2,
                ColourChart::kDarkGrey1,
            };

            inline constexpr ColourElement kBackgroundHoover = {
                "ButtonBackgroundHoover",
                ColourChart::kBlack,
                ColourChart::kLightGrey2
            };

            inline constexpr ColourElement kTextHoover = {
                "ButtonTextHoover",
                ColourChart::kWhite,
                ColourChart::kDarkGrey1,
            };

            inline constexpr ColourElement kBackgroundClicked = {
                "ButtonBackgroundClicked",
                ColourChart::kBlack,
                ColourChart::kLightGrey2
            };

            inline constexpr ColourElement kTextClicked = {
                "ButtonTextClicked",
                ColourChart::kRed,
                ColourChart::kRed,
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                "ButtonBackgroundDisabled",
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled,
            };

            inline constexpr ColourElement kBorderDisabled = {
                "ButtonBorderDisabled",
                Common::kBorderDisabled,
                Common::kBorderDisabled,
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
                ColourChart::kDarkGrey4
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

            inline constexpr ColourElement kTrackDisabled = {
                "SliderTrackDisabled",
                Common::kDecorationDisabled,
                Common::kDecorationDisabled
            };

            inline constexpr ColourElement kValueBarDisabled = {
                "SliderValueBarDisabled",
                Common::kDecorationDisabled,
                Common::kDecorationDisabled
            };

            inline constexpr ColourElement kTextDisabled = {
                "SliderTextDisabled",
                Common::kContentDisabled,
                Common::kContentDisabled,
            };
        }
    }
}
