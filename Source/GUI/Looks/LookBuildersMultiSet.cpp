#include "LookBuilders.h"

#include "GUI/Looks/TypographyStyles.h"
#include "GUI/Skins/ISkin.h"

namespace TSS
{
    namespace
    {
        juce::Font resolvedTypographyFont(const ISkin& skin, TypographyStyleId styleId)
        {
            const auto style = getTypographyStyle(styleId);
            const auto baseFont = style.useBoldFont ? skin.getBaseFontBold() : skin.getBaseFont();
            return baseFont.withHeight(style.fontHeight);
        }
    }

    ButtonLook buttonLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        auto colour = [&](SkinColourProperty property, SkinColourState state)
        {
            return skin.getColour(SkinObjectType::kButton, property, state, colourSet);
        };

        ButtonLook look;
        look.backgroundOff = colour(SkinColourProperty::kBackground, SkinColourState::kOff);
        look.backgroundOn = colour(SkinColourProperty::kBackground, SkinColourState::kOn);
        look.backgroundHover = colour(SkinColourProperty::kBackground, SkinColourState::kHover);
        look.backgroundClicked = colour(SkinColourProperty::kBackground, SkinColourState::kClicked);
        look.borderOff = colour(SkinColourProperty::kBorder, SkinColourState::kOff);
        look.borderOn = colour(SkinColourProperty::kBorder, SkinColourState::kOn);
        look.textOff = colour(SkinColourProperty::kText, SkinColourState::kOff);
        look.textOn = colour(SkinColourProperty::kText, SkinColourState::kOn);
        look.textHover = colour(SkinColourProperty::kText, SkinColourState::kHover);
        look.textClicked = colour(SkinColourProperty::kText, SkinColourState::kClicked);
        look.backgroundDisabled = colour(SkinColourProperty::kBackground, SkinColourState::kDisabled);
        look.borderDisabled = colour(SkinColourProperty::kBorder, SkinColourState::kDisabled);
        look.textDisabled = colour(SkinColourProperty::kText, SkinColourState::kDisabled);
        look.textEmphasis = colour(SkinColourProperty::kText, SkinColourState::kEmphasis);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ButtonLook buttonAlertLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        // Keep base border colours; only fill/text switch to alert.
        // Hover/clicked ink is its own pastille so normal Button Text Hover never leaks in.
        // Alert Error / SecondaryText Hover keys are UCS (k1 seeds); colourSet falls back in Skin::getColour.
        ButtonLook look = buttonLookFromSkin(skin, colourSet);
        auto colour = [&](SkinColourProperty property, SkinColourState state)
        {
            return skin.getColour(SkinObjectType::kButton, property, state, colourSet);
        };
        const auto fill = colour(SkinColourProperty::kBackground, SkinColourState::kError);
        const auto text = colour(SkinColourProperty::kText, SkinColourState::kError);
        const auto textHover = colour(SkinColourProperty::kSecondaryText, SkinColourState::kHover);

        look.backgroundOff = fill;
        look.backgroundOn = fill;
        look.backgroundHover = fill;
        look.backgroundClicked = fill;
        look.textOff = text;
        look.textOn = text;
        look.textHover = textHover;
        look.textClicked = textHover;
        look.font = skin.getBaseFontBold().withHeight(look.font.getHeight());
        return look;
    }

    SliderLook sliderLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        SliderLook look;
        look.trackEnabled = skin.getSliderTrackColour(true, colourSet);
        look.trackDisabled = skin.getSliderTrackColour(false, colourSet);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kSlider,
                                                 SkinColourProperty::kBackground,
                                                 SkinColourState::kDisabled,
                                                 colourSet);
        look.valueBarEnabled = skin.getSliderValueBarColour(true, colourSet);
        look.valueBarDisabled = skin.getSliderValueBarColour(false, colourSet);
        look.textEnabled = skin.getSliderTextColour(true, colourSet);
        look.textDisabled = skin.getSliderTextColour(false, colourSet);
        look.focusBorder = skin.getColour(SkinObjectType::kSlider,
                                          SkinColourProperty::kFocusBorder,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.editorText = skin.getColour(SkinObjectType::kSlider,
                                         SkinColourProperty::kEditorText,
                                         SkinColourState::kDefault,
                                         colourSet);
        look.editorCaret = skin.getColour(SkinObjectType::kSlider,
                                          SkinColourProperty::kCaret,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ComboBoxLook comboBoxLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        ComboBoxLook look;
        look.backgroundEnabled = skin.getComboBoxBackgroundColour(true, colourSet);
        look.backgroundDisabled = skin.getComboBoxBackgroundColour(false, colourSet);
        look.borderEnabled = skin.getComboBoxBorderColour(true, colourSet);
        look.borderDisabled = skin.getComboBoxBorderColour(false, colourSet);
        look.focusBorder = skin.getComboBoxFocusBorderColour(colourSet);
        look.triangleEnabled = skin.getComboBoxTriangleColour(true, colourSet);
        look.triangleDisabled = skin.getComboBoxTriangleColour(false, colourSet);
        look.textEnabled = skin.getComboBoxTextColour(true, colourSet);
        look.textDisabled = skin.getComboBoxTextColour(false, colourSet);
        look.textFocus = skin.getColour(SkinObjectType::kComboBox,
                                        SkinColourProperty::kText,
                                        SkinColourState::kFocus,
                                        colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    PopupMenuLook popupMenuLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        PopupMenuLook look;
        look.background = skin.getPopupMenuBackgroundColour(colourSet);
        look.border = skin.getPopupMenuBorderColour(colourSet);
        look.separator = skin.getPopupMenuSeparatorColour(colourSet);
        look.text = skin.getPopupMenuTextColour(colourSet);
        look.textDisabled = skin.getColour(SkinObjectType::kPopupMenu,
                                           SkinColourProperty::kText,
                                           SkinColourState::kDisabled,
                                           colourSet);
        look.backgroundHover = skin.getPopupMenuBackgroundHooverColour(colourSet);
        look.textHover = skin.getPopupMenuTextHooverColour(colourSet);
        look.scrollbar = skin.getPopupMenuScrollbarColour(colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ReadOnlyValueFieldLook readOnlyValueFieldLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        ReadOnlyValueFieldLook look;
        look.background = skin.getColour(SkinObjectType::kReadOnlyValueField,
                                         SkinColourProperty::kBackground,
                                         SkinColourState::kDefault,
                                         colourSet);
        look.text = skin.getColour(SkinObjectType::kReadOnlyValueField,
                                   SkinColourProperty::kText,
                                   SkinColourState::kDefault,
                                   colourSet);
        return look;
    }

    LedLook ledLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        LedLook look;
        look.backgroundOff = skin.getColour(SkinObjectType::kLed,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kOff,
                                            colourSet);
        look.borderOff = skin.getColour(SkinObjectType::kLed,
                                        SkinColourProperty::kBorder,
                                        SkinColourState::kOff,
                                        colourSet);
        look.maxOn = skin.getColour(SkinObjectType::kLed,
                                    SkinColourProperty::kBackground,
                                    SkinColourState::kOn,
                                    colourSet);
        return look;
    }

    LedLook peakIndicatorLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        LedLook look;
        look.backgroundOff = skin.getColour(SkinObjectType::kPeakIndicator,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kOff,
                                            colourSet);
        look.borderOff = skin.getColour(SkinObjectType::kPeakIndicator,
                                        SkinColourProperty::kBorder,
                                        SkinColourState::kOff,
                                        colourSet);
        look.maxOn = skin.getColour(SkinObjectType::kPeakIndicator,
                                    SkinColourProperty::kBackground,
                                    SkinColourState::kOn,
                                    colourSet);
        return look;
    }

    ToggleLook toggleLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        ToggleLook look;
        look.border = skin.getColour(SkinObjectType::kToggle,
                                     SkinColourProperty::kBorder,
                                     SkinColourState::kDefault,
                                     colourSet);
        look.backgroundOff = skin.getToggleBackgroundColour(false, colourSet);
        look.backgroundOn = skin.getToggleBackgroundColour(true, colourSet);
        look.textOff = skin.getToggleTextColour(false, colourSet);
        look.textOn = skin.getToggleTextColour(true, colourSet);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kToggle,
                                                 SkinColourProperty::kBackground,
                                                 SkinColourState::kDisabled,
                                                 colourSet);
        look.borderDisabled = skin.getColour(SkinObjectType::kToggle,
                                             SkinColourProperty::kBorder,
                                             SkinColourState::kDisabled,
                                             colourSet);
        look.textDisabled = skin.getColour(SkinObjectType::kToggle,
                                           SkinColourProperty::kText,
                                           SkinColourState::kDisabled,
                                           colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    NumberBoxLook numberBoxLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        NumberBoxLook look;
        look.background = skin.getColour(SkinObjectType::kNumberBox,
                                         SkinColourProperty::kBackground,
                                         SkinColourState::kDefault,
                                         colourSet);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kNumberBox,
                                                 SkinColourProperty::kBackground,
                                                 SkinColourState::kDisabled,
                                                 colourSet);
        // NumberBox borders are UCS (k1 seeds only); higher colourSet args fall back in Skin::getColour.
        look.border = skin.getColour(SkinObjectType::kNumberBox,
                                     SkinColourProperty::kBorder,
                                     SkinColourState::kDefault,
                                     colourSet);
        look.borderDisabled = skin.getColour(SkinObjectType::kNumberBox,
                                             SkinColourProperty::kBorder,
                                             SkinColourState::kDisabled,
                                             colourSet);
        look.text = skin.getColour(SkinObjectType::kNumberBox,
                                   SkinColourProperty::kText,
                                   SkinColourState::kDefault,
                                   colourSet);
        look.textFocus = skin.getColour(SkinObjectType::kNumberBox,
                                        SkinColourProperty::kText,
                                        SkinColourState::kFocus,
                                        colourSet);
        look.editorBackground = skin.getColour(SkinObjectType::kNumberBox,
                                               SkinColourProperty::kEditorBackground,
                                               SkinColourState::kDefault,
                                               colourSet);
        look.editorText = skin.getColour(SkinObjectType::kNumberBox,
                                         SkinColourProperty::kEditorText,
                                         SkinColourState::kDefault,
                                         colourSet);
        look.editorCaret = skin.getColour(SkinObjectType::kNumberBox,
                                          SkinColourProperty::kCaret,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.editorSelectionBackground = skin.getColour(SkinObjectType::kNumberBox,
                                                        SkinColourProperty::kEditorSelectionBackground,
                                                        SkinColourState::kDefault,
                                                        colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }
}
