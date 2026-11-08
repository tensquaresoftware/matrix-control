#include "Skin.h"

namespace TSS
{
    namespace
    {
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

    juce::Colour Skin::getComboBoxBackgroundColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kComboBox,
                         SkinColourProperty::kBackground,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxBorderColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kComboBox,
                         SkinColourProperty::kBorder,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxFocusBorderColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kComboBox,
                         SkinColourProperty::kFocusBorder,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getComboBoxTriangleColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kComboBox,
                         SkinColourProperty::kTriangle,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getComboBoxTextColour(bool isEnabled, ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kComboBox,
                         SkinColourProperty::kText,
                         enabledState(isEnabled),
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuBackgroundColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kBackground,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuBorderColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kBorder,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuSeparatorColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kSeparator,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuTextColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kText,
                         SkinColourState::kDefault,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuBackgroundHooverColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kBackground,
                         SkinColourState::kHover,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuTextHooverColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kText,
                         SkinColourState::kHover,
                         colourSet);
    }

    juce::Colour Skin::getPopupMenuScrollbarColour(ColourSet colourSet) const
    {
        return getColour(SkinObjectType::kPopupMenu,
                         SkinColourProperty::kScrollbar,
                         SkinColourState::kDefault,
                         colourSet);
    }
}
