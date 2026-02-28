#pragma once

#include "SkinColoursCommon.h"

namespace tss::SkinColours
{
    namespace Panels
    {
        inline constexpr ColourElement kHeaderPanelBackground = {
            "HeaderPanelBackground",
            ColourChart::kLightGrey1,
            ColourChart::kLightGrey1
        };

        inline constexpr ColourElement kHeaderPanelLabelText = {
            "HeaderPanelLabelText",
            ColourChart::kDarkGrey1,
            ColourChart::kDarkGrey1
        };

        inline constexpr ColourElement kBodyPanelBackground = {
            "BodyPanelBackground",
            ColourChart::kDarkGrey2,
            ColourChart::kCream
        };

        inline constexpr ColourElement kFooterPanelBackground = {
            "FooterPanelBackground",
            ColourChart::kLightGrey1,
            ColourChart::kLightGrey1
        };
    }
}
