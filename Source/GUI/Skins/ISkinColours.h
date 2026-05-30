#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "SkinValues.h"

namespace tss
{
    class ISkinColours
    {
    public:
        virtual ~ISkinColours() = default;

        virtual juce::Colour getColour(SkinColourId colourId) const = 0;
        virtual void setColour(SkinColourId colourId, juce::Colour colour) = 0;

        virtual juce::Colour getToggleBackgroundColour(bool isOn) const = 0;
        virtual juce::Colour getToggleTextColour(bool isOn) const = 0;
        virtual juce::Colour getSliderTrackColour(bool isEnabled = true) const = 0;
        virtual juce::Colour getSliderValueBarColour(bool isEnabled = true) const = 0;
        virtual juce::Colour getSliderTextColour(bool isEnabled = true) const = 0;
        virtual juce::Colour getComboBoxBackgroundColour(bool isEnabled = true, bool isButtonLike = false) const = 0;
        virtual juce::Colour getComboBoxBorderColour(bool isEnabled = true, bool isButtonLike = false) const = 0;
        virtual juce::Colour getComboBoxFocusBorderColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getComboBoxTriangleColour(bool isEnabled = true, bool isButtonLike = false) const = 0;
        virtual juce::Colour getComboBoxTextColour(bool isEnabled = true, bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuBackgroundColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuBorderColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuSeparatorColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuTextColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuBackgroundHooverColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuTextHooverColour(bool isButtonLike = false) const = 0;
        virtual juce::Colour getPopupMenuScrollbarColour(bool isButtonLike = false) const = 0;
    };
}
