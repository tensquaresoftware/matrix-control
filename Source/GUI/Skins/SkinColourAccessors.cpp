#include "Skin.h"

namespace TSS
{
    namespace
    {
        SkinObjectType comboBoxObject(bool isButtonLike) noexcept
        {
            return isButtonLike ? SkinObjectType::kComboBoxButtonLike : SkinObjectType::kComboBox;
        }

        SkinObjectType popupMenuObject(bool isButtonLike) noexcept
        {
            return isButtonLike ? SkinObjectType::kPopupMenuButtonLike : SkinObjectType::kPopupMenu;
        }

        SkinColourState enabledState(bool isEnabled) noexcept
        {
            return isEnabled ? SkinColourState::kDefault : SkinColourState::kDisabled;
        }
    }

    juce::Colour Skin::getToggleBackgroundColour(bool isOn, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kToggle,
                         SkinColourProperty::kBackground,
                         isOn ? SkinColourState::kOn : SkinColourState::kOff,
                         colourSet);
    }

    juce::Colour Skin::getToggleTextColour(bool isOn, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kToggle,
                         SkinColourProperty::kText,
                         isOn ? SkinColourState::kOn : SkinColourState::kOff,
                         colourSet);
    }

    juce::Colour Skin::getSliderTrackColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kSlider,
                         SkinColourProperty::kTrack,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getSliderValueBarColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kSlider,
                         SkinColourProperty::kValueBar,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getSliderTextColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kSlider,
                         SkinColourProperty::kText,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxBackgroundColour(bool isEnabled,
                                                   bool isButtonLike,
                                                   ColourSet colourSet) const
    {
        return getColour(comboBoxObject(isButtonLike),
                         SkinColourProperty::kBackground,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxBorderColour(bool isEnabled,
                                               bool isButtonLike,
                                               ColourSet colourSet) const
    {
        return getColour(comboBoxObject(isButtonLike),
                         SkinColourProperty::kBorder,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxFocusBorderColour(bool isButtonLike, ColourSet colourSet) const
    {
        if (isButtonLike)
            return getColour(SkinObjectType::kComboBoxButtonLike,
                             SkinColourProperty::kBorder,
                             SkinColourState::kDefault,
                             colourSet);
        return getColour(SkinObjectType::kComboBox,
                         SkinColourProperty::kFocusBorder,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getComboBoxTriangleColour(bool isEnabled,
                                                 bool isButtonLike,
                                                 ColourSet colourSet) const
    {
        return getColour(comboBoxObject(isButtonLike),
                         SkinColourProperty::kTriangle,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxTextColour(bool isEnabled,
                                             bool isButtonLike,
                                             ColourSet colourSet) const
    {
        return getColour(comboBoxObject(isButtonLike),
                         SkinColourProperty::kText,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuBackgroundColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kBackground,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuBorderColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kBorder,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuSeparatorColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kSeparator,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuTextColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kText,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuBackgroundHooverColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kBackground,
                         SkinColourState::kHover,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuTextHooverColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kText,
                         SkinColourState::kHover,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuScrollbarColour(bool isButtonLike, ColourSet colourSet) const
    {
        return getColour(popupMenuObject(isButtonLike),
                         SkinColourProperty::kScrollbar,
                         SkinColourState::kDefault,
                         colourSet);
    }
}
