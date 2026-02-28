#pragma once

#include "SkinColoursCommon.h"

namespace tss::SkinColours
{
    namespace Widgets
    {
        namespace SectionHeader
        {
            inline constexpr ColourElement kText = {
                "SectionHeaderText",
                ColourChart::kLightGrey2,
                ColourChart::kLightGrey2
            };

            inline constexpr ColourElement kLineBlue = {
                "SectionHeaderLineBlue",
                ColourChart::kBlue,
                ColourChart::kBlue
            };

            inline constexpr ColourElement kLineOrange = {
                "SectionHeaderLineOrange",
                ColourChart::kOrange,
                ColourChart::kOrange
            };
        }

        namespace ModuleHeader
        {
            inline constexpr ColourElement kText = {
                "ModuleHeaderText",
                ColourChart::kLightGrey2,
                ColourChart::kLightGrey2
            };

            inline constexpr ColourElement kLineBlue = {
                "ModuleHeaderLineBlue",
                ColourChart::kBlue,
                ColourChart::kBlue
            };

            inline constexpr ColourElement kLineOrange = {
                "ModuleHeaderLineOrange",
                ColourChart::kOrange,
                ColourChart::kOrange
            };
        }

        namespace GroupLabel
        {
            inline constexpr ColourElement kText = {
                "GroupLabelText",
                ColourChart::kLightGrey2,
                ColourChart::kLightGrey2
            };

            inline constexpr ColourElement kLine = {
                "GroupLabelLine",
                ColourChart::kDarkGrey5,
                ColourChart::kDarkGrey5
            };
        }

        namespace Label
        {
            inline constexpr ColourElement kText = {
                "LabelText",
                ColourChart::kLightGrey2,
                ColourChart::kDarkGrey1
            };
        }

        namespace VerticalSeparator
        {
            inline constexpr ColourElement kLine = {
                "VerticalSeparatorLine",
                ColourChart::kDarkGrey5,
                ColourChart::kDarkGrey5
            };
        }

        namespace HorizontalSeparator
        {
            inline constexpr ColourElement kLine = {
                "HorizontalSeparatorLine",
                ColourChart::kDarkGrey5,
                ColourChart::kDarkGrey5
            };
        }
    }
}
