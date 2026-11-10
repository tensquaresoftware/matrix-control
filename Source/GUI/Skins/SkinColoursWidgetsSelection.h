#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        /** ComboBox MCS: k1 Patch (this namespace), k2 Master, k3 Modales + mono exceptions. */
        namespace ComboBox
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground),
                ColourChart::kEmerald950,
                ColourChart::kBlue950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle),
                ColourChart::kEmerald700,
                ColourChart::kBlue600,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder),
                ColourChart::kEmerald900,
                ColourChart::kBlue800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextFocus = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kFocus),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };
        }

        namespace ComboBoxK2
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kBlack,
                ColourChart::kOrange950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k2),
                ColourChart::kGrey950,
                ColourChart::kGrey800,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                              SkinColourState::kDisabled, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kOrange800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey200,
                ColourChart::kOrange400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextFocus = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kFocus, ColourSet::k2),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey600,
                ColourChart::kOrange700,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };
        }

        namespace ComboBoxK3
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k3),
                ColourChart::kGrey950,
                ColourChart::kGrey800,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                              SkinColourState::kDisabled, ColourSet::k3),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextFocus = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kFocus, ColourSet::k3),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };
        }

        /** PopupMenu MCS: aligned on ComboBox (k1 Patch, k2 Master, k3 Modales + mono exceptions). */
        namespace PopupMenu
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground),
                ColourChart::kEmerald950,
                ColourChart::kBlue950,
                ColourChart::kGreen200
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen800
            };

            // Solid muted ink replacing former text.withAlpha(0.5f) on disabled items.
            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled),
                ColourChart::kEmerald700,
                ColourChart::kBlue600,
                ColourChart::kGreen400
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover),
                Common::kContentHover,
                Common::kContentHover,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kGreen500
            };
        }

        namespace PopupMenuK2
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kBlack,
                ColourChart::kOrange950,
                ColourChart::kGreen200
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                              SkinColourState::kHover, ColourSet::k2),
                ColourChart::kGrey200,
                ColourChart::kOrange400,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey600,
                ColourChart::kOrange400,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey600,
                ColourChart::kOrange400,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey200,
                ColourChart::kOrange400,
                ColourChart::kGreen800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kContentDisabled,
                ColourChart::kOrange700,
                ColourChart::kGreen400
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kHover, ColourSet::k2),
                ColourChart::kBlack,
                ColourChart::kGrey950,
                ColourChart::kGreen100
            };
        }

        namespace PopupMenuK3
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                              SkinColourState::kHover, ColourSet::k3),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kHover, ColourSet::k3),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCream
            };
        }
    }
}
