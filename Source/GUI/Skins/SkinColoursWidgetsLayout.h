#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace SectionHeader
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kLineBlue = {
                skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kLineBlue),
                ColourChart::kBrandBlue,
                ColourChart::kBrandBlue,
                ColourChart::kBrandBlue
            };

            inline constexpr ColourElement kLineOrange = {
                skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kLineOrange),
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange
            };
        }

        namespace ModuleHeader
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kLineBlue = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue),
                ColourChart::kBrandBlue,
                ColourChart::kBrandBlue,
                ColourChart::kBrandBlue
            };

            inline constexpr ColourElement kLineOrange = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange),
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange
            };

            // Clickable module title under the pointer.
            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText, SkinColourState::kHover),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed600
            };

            // Called-out badge inside a module (e.g. Ramp "M" master override).
            inline constexpr ColourElement kBackgroundEmphasis = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kBackground, SkinColourState::kEmphasis),
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange
            };

            inline constexpr ColourElement kTextEmphasis = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText, SkinColourState::kEmphasis),
                ColourChart::kGrey900,
                ColourChart::kGrey900,
                ColourChart::kBrandCream
            };
        }

        namespace GroupLabel
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kGroupLabel, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            inline constexpr ColourElement kLine = {
                skinColourKey(SkinObjectType::kGroupLabel, SkinColourProperty::kLine),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kCafe400
            };
        }

        namespace Label
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe850
            };

            // Plate behind a label being dragged (e.g. modulation bus source).
            inline constexpr ColourElement kBackgroundClicked = {
                skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kBackground, SkinColourState::kClicked),
                ColourChart::kBrandBlue,
                ColourChart::kBrandBlue,
                ColourChart::kBrandBlue
            };

            // Plate behind a label hovered as a drop target.
            inline constexpr ColourElement kBackgroundHover = {
                skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kBackground, SkinColourState::kHover),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed600
            };
        }

        namespace HorizontalSeparator
        {
            inline constexpr ColourElement kLine = {
                skinColourKey(SkinObjectType::kHorizontalSeparator, SkinColourProperty::kLine),
                ColourChart::kGrey600,
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };
        }

        namespace ChromeText
        {
            // Text on Header/Footer/dialog chrome — must not follow Cream body LabelText.
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey200,
                ColourChart::kCafe700
            };

            // Logo / brand hover pastille (absolute White) — Skin-mapped, not widget ColourChart dig.
            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText, SkinColourState::kHover),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kCafe750
            };

            // Called-out chrome text (e.g. About link hover) — Hover stays the logo white.
            inline constexpr ColourElement kTextEmphasis = {
                skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText, SkinColourState::kEmphasis),
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange,
                ColourChart::kBrandOrange
            };
        }

        namespace FooterMessage
        {
            inline constexpr ColourElement kMessageInfo = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kInfo),
                ColourChart::kGrey500,
                ColourChart::kGrey500,
                ColourChart::kCafe700
            };

            inline constexpr ColourElement kMessageWarning = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kWarning),
                ColourChart::kOrange500,
                ColourChart::kOrange500,
                ColourChart::kOrange500
            };

            inline constexpr ColourElement kMessageError = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kError),
                ColourChart::kRed500,
                ColourChart::kRed500,
                ColourChart::kRed600
            };

            // Sticky message close cross under the pointer.
            inline constexpr ColourElement kGlyphHover = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kGlyph, SkinColourState::kHover),
                ColourChart::kWhite,
                ColourChart::kWhite,
                ColourChart::kBlack
            };
        }
    }
}
