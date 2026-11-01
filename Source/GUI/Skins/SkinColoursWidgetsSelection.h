#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace ComboBox
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground),
                ColourChart::kEmerald950,
                ColourChart::kEmerald950
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle),
                ColourChart::kEmerald700,
                ColourChart::kEmerald700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder),
                ColourChart::kEmerald900,
                ColourChart::kEmerald900
            };

            // Closed-face border (shared with former ButtonLike border pastille).
            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        /** Former ComboBoxButtonLike pastilles — seeded onto ComboBox at ColourSet::k4 only. */
        namespace ComboBoxK4
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTriangle = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            // Former ButtonLike focus used Border; seed FocusBorder at k4 to match.
            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k4),
                ColourChart::kGrey950,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                              SkinColourState::kDisabled, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kTriangleDisabled = {
                skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kContentDisabled,
                Common::kContentDisabled
            };
        }

        namespace PopupMenu
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground),
                ColourChart::kEmerald950,
                ColourChart::kEmerald950
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            // Solid muted emerald replacing former text.withAlpha(0.5f) on disabled items.
            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled),
                ColourChart::kEmerald700,
                ColourChart::kEmerald700
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kEmerald400,
                ColourChart::kEmerald400
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover),
                Common::kContentHover,
                Common::kContentHover
            };

            // Scrollbar pastille (shared hex with former ButtonLike scrollbar).
            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };
        }

        /** Former PopupMenuButtonLike pastilles — seeded onto PopupMenu at ColourSet::k4 only. */
        namespace PopupMenuK4
        {
            inline constexpr ColourElement kBackground = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            inline constexpr ColourElement kSeparator = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kContentDisabled,
                Common::kContentDisabled
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                              SkinColourState::kHover, ColourSet::k4),
                ColourChart::kGrey200,
                ColourChart::kGrey200
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                              SkinColourState::kHover, ColourSet::k4),
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kScrollbar = {
                skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800
            };
        }
    }
}
