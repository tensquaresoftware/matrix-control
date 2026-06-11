#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Panels
    {
        inline constexpr ColourElement kHeaderPanelBackground = {
            "HeaderPanelBackground",
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
            ColourChart::kDarkGrey1,
            ColourChart::kDarkGrey1
        };
    }
}
