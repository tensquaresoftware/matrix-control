#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "SkinValues.h"

namespace TSS
{
    class ISkinColours
    {
    public:
        virtual ~ISkinColours() = default;

        virtual juce::Colour getColour(SkinObjectType object,
                                       SkinColourProperty property,
                                       SkinColourState state = SkinColourState::kDefault,
                                       ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getColour(SkinColourKey key) const = 0;
        virtual void setColour(SkinColourKey key, juce::Colour colour) = 0;

        virtual juce::Colour getToggleBackgroundColour(bool isOn,
                                                       ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getToggleTextColour(bool isOn,
                                                 ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getSliderTrackColour(bool isEnabled = true,
                                                  ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getSliderValueBarColour(bool isEnabled = true,
                                                     ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getSliderTextColour(bool isEnabled = true,
                                                 ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxBackgroundColour(bool isEnabled = true,
                                                         ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxBorderColour(bool isEnabled = true,
                                                     ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxFocusBorderColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxTriangleColour(bool isEnabled = true,
                                                       ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxTextColour(bool isEnabled = true,
                                                   ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuBackgroundColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuBorderColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuSeparatorColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuTextColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuBackgroundHooverColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuTextHooverColour(ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuScrollbarColour(ColourSet colourSet = ColourSet::k1) const = 0;
    };
}
