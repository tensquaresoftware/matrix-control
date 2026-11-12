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
                ColourChart::kCafe50
            };

            inline constexpr ColourElement kBorderOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe500
            };

            inline constexpr ColourElement kTextOff = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOff),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kBackgroundOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kBlack,
                ColourChart::kBlack,
                ColourChart::kCafe300
            };

            inline constexpr ColourElement kBorderOn = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe500
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
                ColourChart::kCafe300
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
                ColourChart::kCafe300
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
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe400
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
                ColourChart::kCafe750
            };
        }

        namespace Toggle
        {
            inline constexpr ColourElement kBorder = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe500
            };

            inline constexpr ColourElement kBackgroundOff = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOff),
                ColourChart::kGreen950,
                ColourChart::kBlue950,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kTextOff = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOff),
                ColourChart::kGreen750,
                ColourChart::kBlue600,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kBackgroundOn = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOn),
                ColourChart::kGreen900,
                ColourChart::kBlue800,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTextOn = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOn),
                ColourChart::kGreen300,
                ColourChart::kBlue400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                ColourChart::kGrey950,
                ColourChart::kGrey950,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kBorderDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder, SkinColourState::kDisabled),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe400
            };
        }

        /** Slider MCS: k1 Patch (this namespace), k2 Master, k3 Header, k4 Modales. */
        namespace Slider
        {
            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack),
                ColourChart::kGreen950,
                ColourChart::kBlue950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar),
                ColourChart::kGreen750,
                ColourChart::kBlue600,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText),
                ColourChart::kGreen300,
                ColourChart::kBlue400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder),
                ColourChart::kGreen900,
                ColourChart::kBlue800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled),
                Common::kBackgroundDisabled,
                Common::kBackgroundDisabled,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled),
                Common::kDecorationDisabled,
                Common::kDecorationDisabled,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe850
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
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGreen900,
                ColourChart::kOrange800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGreen300,
                ColourChart::kOrange400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGreen950,
                ColourChart::kOrange950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDisabled, ColourSet::k2),
                Common::kDecorationDisabled,
                ColourChart::kBlue950,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kGreen750,
                ColourChart::kOrange700,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDisabled, ColourSet::k2),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText,
                              SkinColourState::kDefault, ColourSet::k2),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe850
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
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGreen900,
                ColourChart::kYellow800,
                ColourChart::kGreen700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGreen300,
                ColourChart::kYellow400,
                ColourChart::kGreen100
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTrack = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGreen950,
                ColourChart::kYellow950,
                ColourChart::kGreen600
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDisabled, ColourSet::k3),
                Common::kDecorationDisabled,
                ColourChart::kGrey900,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kGreen750,
                ColourChart::kYellow700,
                ColourChart::kGreen500
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDisabled, ColourSet::k3),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText,
                              SkinColourState::kDefault, ColourSet::k3),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe850
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
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kFocusBorder = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey800,
                ColourChart::kGrey800,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kBackgroundDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kBackgroundDisabled,
                ColourChart::kGrey800,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kContentDisabled,
                ColourChart::kGrey600,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                              SkinColourState::kDisabled, ColourSet::k4),
                Common::kDecorationDisabled,
                ColourChart::kGrey900,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                              SkinColourState::kDisabled, ColourSet::k4),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kEditorText = {
                skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText,
                              SkinColourState::kDefault, ColourSet::k4),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe850
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
                ColourChart::kGreen950,
                ColourChart::kBlack,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kValueBar = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar),
                ColourChart::kGreen750,
                ColourChart::kGrey600,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText),
                ColourChart::kGreen300,
                ColourChart::kGrey200,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kTrackDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kTrack, SkinColourState::kDisabled),
                Common::kDecorationDisabled,
                Common::kDecorationDisabled,
                ColourChart::kBrandCream
            };

            inline constexpr ColourElement kValueBarDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
                ColourChart::kGrey700,
                ColourChart::kGrey700,
                ColourChart::kCafe400
            };

            inline constexpr ColourElement kTextDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe400
            };

            /** Phase label above each bar (the percent readout uses Text). */
            inline constexpr ColourElement kSecondaryText = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kSecondaryTextDisabled = {
                skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText, SkinColourState::kDisabled),
                Common::kContentDisabled,
                Common::kContentDisabled,
                ColourChart::kCafe400
            };
        }
    }
}
