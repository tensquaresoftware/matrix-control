#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace ComboBox
        {
            namespace Standard
            {
                inline constexpr ColourElement kBackground = {
                    "ComboBoxStandardBackground",
                    ColourChart::kGreen1,
                    ColourChart::kGreen1
                };

                inline constexpr ColourElement kTriangle = {
                    "ComboBoxStandardTriangle",
                    ColourChart::kGreen3,
                    ColourChart::kGreen3
                };

                inline constexpr ColourElement kText = {
                    "ComboBoxStandardText",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4,
                };

                inline constexpr ColourElement kFocusBorder = {
                    "ComboBoxStandardFocusBorder",
                    ColourChart::kGreen2,
                    ColourChart::kGreen2
                };

                inline constexpr ColourElement kBackgroundDisabled = {
                    "ComboBoxStandardBackgroundDisabled",
                    Common::kBackgroundDisabled,
                    Common::kBackgroundDisabled
                };

                inline constexpr ColourElement kTriangleDisabled = {
                    "ComboBoxStandardTriangleDisabled",
                    Common::kContentDisabled,
                    Common::kContentDisabled
                };

                inline constexpr ColourElement kTextDisabled = {
                    "ComboBoxStandardTextDisabled",
                    Common::kContentDisabled,
                    Common::kContentDisabled,
                };
            }

            namespace ButtonLike
            {
                inline constexpr ColourElement kBackground = {
                    "ComboBoxButtonLikeBackground",
                    ColourChart::kBlack,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kBorder = {
                    "ComboBoxButtonLikeBorder",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey3
                };

                inline constexpr ColourElement kText = {
                    "ComboBoxButtonLikeText",
                    ColourChart::kLightGrey2,
                    ColourChart::kDarkGrey1,
                };

                inline constexpr ColourElement kTriangle = {
                    "ComboBoxButtonLikeTriangle",
                    ColourChart::kDarkGrey5,
                    ColourChart::kDarkGrey1
                };

                inline constexpr ColourElement kBackgroundDisabled = {
                    "ComboBoxButtonLikeBackgroundDisabled",
                    ColourChart::kDarkGrey1,
                    ColourChart::kDarkGrey1,
                };

                // Same chart values as enabled border (black/cream variants).
                inline constexpr ColourElement kBorderDisabled = {
                    "ComboBoxButtonLikeBorderDisabled",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey3
                };

                inline constexpr ColourElement kTextDisabled = {
                    "ComboBoxButtonLikeTextDisabled",
                    Common::kContentDisabled,
                    Common::kContentDisabled,
                };

                inline constexpr ColourElement kTriangleDisabled = {
                    "ComboBoxButtonLikeTriangleDisabled",
                    Common::kContentDisabled,
                    Common::kContentDisabled
                };
            }
        }

        namespace PopupMenu
        {
            namespace Standard
            {
                inline constexpr ColourElement kBackground = {
                    "PopupMenuStandardBackground",
                    ColourChart::kGreen1,
                    ColourChart::kGreen1
                };

                inline constexpr ColourElement kBorder = {
                    "PopupMenuStandardBorder",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4
                };

                inline constexpr ColourElement kSeparator = {
                    "PopupMenuStandardSeparator",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4
                };

                inline constexpr ColourElement kText = {
                    "PopupMenuStandardText",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4,
                };

                inline constexpr ColourElement kBackgroundHoover = {
                    "PopupMenuStandardBackgroundHoover",
                    ColourChart::kGreen4,
                    ColourChart::kGreen4,
                };

                inline constexpr ColourElement kTextHoover = {
                    "PopupMenuStandardTextHoover",
                    Common::kContentHoover,
                    Common::kContentHoover,
                };
            }

            namespace ButtonLike
            {
                inline constexpr ColourElement kBackground = {
                    "PopupMenuButtonLikeBackground",
                    ColourChart::kBlack,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kBorder = {
                    "PopupMenuButtonLikeBorder",
                    ColourChart::kDarkGrey5,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kSeparator = {
                    "PopupMenuButtonLikeSeparator",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey4
                };

                inline constexpr ColourElement kText = {
                    "PopupMenuButtonLikeText",
                    ColourChart::kLightGrey2,
                    ColourChart::kDarkGrey1,
                };

                inline constexpr ColourElement kBackgroundHoover = {
                    "PopupMenuButtonLikeBackgroundHoover",
                    ColourChart::kLightGrey2,
                    ColourChart::kLightGrey2
                };

                inline constexpr ColourElement kTextHoover = {
                    "PopupMenuButtonLikeTextHoover",
                    ColourChart::kBlack,
                    ColourChart::kDarkGrey1,
                };

                inline constexpr ColourElement kScrollbar = {
                    "PopupMenuButtonLikeScrollbar",
                    ColourChart::kDarkGrey3,
                    ColourChart::kDarkGrey3
                };
            }
        }
    }
}
