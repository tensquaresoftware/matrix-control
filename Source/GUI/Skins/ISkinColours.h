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
                                                         bool isButtonLike = false,
                                                         ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxBorderColour(bool isEnabled = true,
                                                     bool isButtonLike = false,
                                                     ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxFocusBorderColour(bool isButtonLike = false,
                                                          ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxTriangleColour(bool isEnabled = true,
                                                       bool isButtonLike = false,
                                                       ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getComboBoxTextColour(bool isEnabled = true,
                                                   bool isButtonLike = false,
                                                   ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuBackgroundColour(bool isButtonLike = false,
                                                          ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuBorderColour(bool isButtonLike = false,
                                                      ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuSeparatorColour(bool isButtonLike = false,
                                                         ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuTextColour(bool isButtonLike = false,
                                                    ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuBackgroundHooverColour(bool isButtonLike = false,
                                                                ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuTextHooverColour(bool isButtonLike = false,
                                                          ColourSet colourSet = ColourSet::k1) const = 0;
        virtual juce::Colour getPopupMenuScrollbarColour(bool isButtonLike = false,
                                                         ColourSet colourSet = ColourSet::k1) const = 0;
    };
}
