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
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kLineBlue = {
                skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kLineBlue),
                ColourChart::kBlue,
                ColourChart::kBlue
            };

            inline constexpr ColourElement kLineOrange = {
                skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kLineOrange),
                ColourChart::kOrange,
                ColourChart::kOrange
            };
        }

        namespace ModuleHeader
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kLineBlue = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue),
                ColourChart::kBlue,
                ColourChart::kBlue
            };

            inline constexpr ColourElement kLineOrange = {
                skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange),
                ColourChart::kOrange,
                ColourChart::kOrange
            };
        }

        namespace GroupLabel
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kGroupLabel, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey950
            };

            inline constexpr ColourElement kLine = {
                skinColourKey(SkinObjectType::kGroupLabel, SkinColourProperty::kLine),
                ColourChart::kGrey600,
                ColourChart::kGrey600
            };
        }

        namespace Label
        {
            inline constexpr ColourElement kText = {
                skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kText),
                ColourChart::kGrey200,
                ColourChart::kGrey950
            };
        }

        namespace HorizontalSeparator
        {
            inline constexpr ColourElement kLine = {
                skinColourKey(SkinObjectType::kHorizontalSeparator, SkinColourProperty::kLine),
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
                ColourChart::kGrey200
            };

            // Logo / brand hover pastille (absolute White) — Skin-mapped, not widget ColourChart dig.
            inline constexpr ColourElement kTextHover = {
                skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText, SkinColourState::kHover),
                ColourChart::kWhite,
                ColourChart::kWhite
            };
        }

        namespace FooterMessage
        {
            inline constexpr ColourElement kMessageInfo = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kInfo),
                ColourChart::kGrey500,
                ColourChart::kGrey500
            };

            inline constexpr ColourElement kMessageWarning = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kWarning),
                ColourChart::kOrange,
                ColourChart::kOrange
            };

            inline constexpr ColourElement kMessageError = {
                skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kError),
                ColourChart::kRed500,
                ColourChart::kRed500
            };
        }
    }
}
