#pragma once

#include "SkinColoursCommon.h"

namespace TSS::SkinColours::CreamChroma
{
    struct AccentOverride
    {
        SkinColourKey key;
        juce::uint32 argb;
    };

    /** CREAM/CHROMA = creamVariant column + these zone accents (validated trial CREAM/CHROMA 3).
        Patch (k1) Blue, Master (k2) Orange, Header Slider (k3) / level meters Yellow. */
    inline constexpr AccentOverride kAccentOverrides[] = {
        // Patch — Blue
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack), ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar), ColourChart::kBlue500 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText), ColourChart::kBlue100 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder), ColourChart::kBlue700 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground), ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder), ColourChart::kBlue700 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder), ColourChart::kBlue700 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText), ColourChart::kBlue100 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle), ColourChart::kBlue500 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground), ColourChart::kBlue200 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover),
          ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder), ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar), ColourChart::kBlue500 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator), ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText), ColourChart::kBlue800 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled),
          ColourChart::kBlue400 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover),
          ColourChart::kBlue100 },
        { skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOff),
          ColourChart::kBlue700 },
        { skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOn),
          ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOff),
          ColourChart::kBlue500 },
        { skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOn),
          ColourChart::kBlue100 },
        { skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kTrack), ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar), ColourChart::kBlue500 },
        { skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText), ColourChart::kBlue100 },
        { skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kEnvelope), ColourChart::kBlue600 },
        { skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kShaper),
          ColourChart::kBlue600 },

        // Master — Orange
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange600 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange550 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange100 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange700 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange600 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange700 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange700 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange100 },
        { skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange550 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange200 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground,
                        SkinColourState::kHover, ColourSet::k2), ColourChart::kOrange600 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange600 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange550 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange600 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                        SkinColourState::kDefault, ColourSet::k2), ColourChart::kOrange800 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                        SkinColourState::kDisabled, ColourSet::k2), ColourChart::kOrange400 },
        { skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText,
                        SkinColourState::kHover, ColourSet::k2), ColourChart::kOrange100 },

        // Header — Yellow
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                        SkinColourState::kDefault, ColourSet::k3), ColourChart::kYellow600 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar,
                        SkinColourState::kDefault, ColourSet::k3), ColourChart::kYellow500 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText,
                        SkinColourState::kDefault, ColourSet::k3), ColourChart::kYellow100 },
        { skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder,
                        SkinColourState::kDefault, ColourSet::k3), ColourChart::kYellow700 },
        { skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn),
          ColourChart::kYellow500 },
        { skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground, SkinColourState::kOn),
          ColourChart::kYellow500 },
    };
}
