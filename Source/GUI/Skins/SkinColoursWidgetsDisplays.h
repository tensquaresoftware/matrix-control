#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours
{
    namespace Widgets
    {
        namespace NumberBox
        {
            inline constexpr ColourElement kText = {
                "NumberBoxText",
                ColourChart::kRed,
                ColourChart::kRed,
            };

            inline constexpr ColourElement kDot = {
                "NumberBoxDot",
                ColourChart::kRed,
                ColourChart::kRed,
            };

            inline constexpr ColourElement kEditorBackground = {
                "NumberBoxEditorBackground",
                ColourChart::kRed,
                ColourChart::kRed,
            };

            inline constexpr ColourElement kEditorSelectionBackground = {
                "NumberBoxEditorSelectionBackground",
                ColourChart::kWhite,
                ColourChart::kWhite,
            };

            inline constexpr ColourElement kEditorText = {
                "NumberBoxEditorText",
                ColourChart::kBlack,
                ColourChart::kBlack,
            };
        }

        namespace EnvelopeDisplay
        {
            inline constexpr ColourElement kBackground = {
                "EnvelopeDisplayBackground",
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                "EnvelopeDisplayBorder",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey4
            };

            inline constexpr ColourElement kEnvelope = {
                "EnvelopeDisplayEnvelope",
                ColourChart::kGreen4,
                ColourChart::kGreen4,
            };
        }

        namespace TrackGeneratorDisplay
        {
            inline constexpr ColourElement kBackground = {
                "TrackGeneratorDisplayBackground",
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                "TrackGeneratorDisplayBorder",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey4
            };

            inline constexpr ColourElement kShaper = {
                "TrackGeneratorDisplayShaper",
                ColourChart::kGreen4,
                ColourChart::kGreen4,
            };
        }

        namespace PatchNameDisplay
        {
            inline constexpr ColourElement kBackground = {
                "PatchNameDisplayBackground",
                ColourChart::kBlack,
                ColourChart::kBlack
            };

            inline constexpr ColourElement kBorder = {
                "PatchNameDisplayBorder",
                ColourChart::kDarkGrey3,
                ColourChart::kDarkGrey4
            };

            inline constexpr ColourElement kText = {
                "PatchNameDisplayText",
                ColourChart::kRed,
                ColourChart::kRed,
            };
        }
    }
}
