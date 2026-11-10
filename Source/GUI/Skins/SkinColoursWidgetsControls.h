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
                ColourChart::kBlack,
                ColourChart::kCream400
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kTextOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOff),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe950
            };

            inline constexpr ColourElement kBackgroundOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kBorderOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kTextOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOn),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kHover),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBackgroundClicked = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kClicked),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextClicked = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kClicked),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };

            /** Called-out button label (e.g. current bank marker) — not Off/On/Focus. */
            inline constexpr ColourElement kTextEmphasis = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kEmphasis),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            /** Alert button (e.g. panic) idle fill and label. */
            inline constexpr ColourElement kBackgroundError = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kError),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed500
            };

            inline constexpr ColourElement kTextError = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kError),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            /** Alert button hover/clicked label — kept apart from normal Text Hover. */
            inline constexpr ColourElement kSecondaryTextHover = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kSecondaryText, SkinColourState::kHover),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe900
            };
        }

        namespace Toggle
        {
            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOff),
                ColourChart::kEmerald950,
                ColourChart::kBlue950,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kTextOff = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOff),
                ColourChart::kEmerald700,
                ColourChart::kBlue600,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kBackgroundOn = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kEmerald900,
                ColourChart::kBlue800,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTextOn = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOn),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950,
                ColourChart::kCream
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };
        }

        /** Slider MCS: k1 Patch (this namespace), k2 Master, k3 Header, k4 Modales. */
        namespace Slider
        {
            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack),
                ColourChart::kEmerald950,
                ColourChart::kBlue950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar),
                ColourChart::kEmerald700,
                ColourChart::kBlue600,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kBlue400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder),
                ColourChart::kEmerald900,
                ColourChart::kBlue800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled,
                ColourChart::kCream
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled),
                Common::kDecorationDisabled,
                Common::kDecorationDisabled,
                ColourChart::kCream
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe950
            };

            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kWhite
            };
        }

        namespace SliderK2
        {
            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kBackgroundDisabled,
                ColourChart::kGrey800,
                ColourChart::kCream
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey800,
                ColourChart::kOrange800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey200,
                ColourChart::kOrange400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kBlack,
                ColourChart::kOrange950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kDecorationDisabled,
                ColourChart::kBlue950,
                ColourChart::kCream
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGrey600,
                ColourChart::kOrange700,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDisabled, ColourSet::k2),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe950
            };

            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kWhite
            };
        }

        namespace SliderK3
        {
            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kBackgroundDisabled,
                ColourChart::kGrey800,
                ColourChart::kCream
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey800,
                ColourChart::kYellow800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey200,
                ColourChart::kYellow400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kBlack,
                ColourChart::kYellow950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kDecorationDisabled,
                ColourChart::kGrey900,
                ColourChart::kCream
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGrey600,
                ColourChart::kYellow700,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDisabled, ColourSet::k3),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe950
            };

            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kWhite
            };
        }

        namespace SliderK4
        {
            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCream
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kBackgroundDisabled,
                ColourChart::kGrey800,
                ColourChart::kCream
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kDecorationDisabled,
                ColourChart::kGrey900,
                ColourChart::kCream
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDisabled, ColourSet::k4),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe950
            };

            inline constexpr ColourElement kCaret = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kWhite
            };
        }

        /** UCS modal progress bar (mono greys on Black). */
        namespace ProgressBar
        {
            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kTrack),
                ColourChart::kEmerald950,
                ColourChart::kBlack,
                ColourChart::kCream
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar),
                ColourChart::kEmerald700,
                ColourChart::kGrey600,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText),
                ColourChart::kEmerald400,
                ColourChart::kGrey200,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kTrack, SkinColourState::kDisabled),
                Common::kDecorationDisabled,
                Common::kDecorationDisabled,
                ColourChart::kCream
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe600
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };

            /** Phase label above each bar (the percent readout uses Text). */
            inline constexpr ColourElement kSecondaryText = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe800
            };

            inline constexpr ColourElement kSecondaryTextDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe600
            };
        }
    }
}
