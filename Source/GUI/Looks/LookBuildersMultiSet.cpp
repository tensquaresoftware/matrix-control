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
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ButtonLook buttonAlertLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        // Keep base border colours; only fill/text switch to alert.
        // Reuse FooterMessage Error pastilles + DarkPanel hover white (no ColourChart dig).
        ButtonLook look = buttonLookFromSkin(skin, colourSet);
        const auto red = skin.getColour(SkinObjectType::kFooterMessage,
                                        SkinColourProperty::kText,
                                        SkinColourState::kError);
        const auto black = skin.getColour(SkinObjectType::kFooterMessage,
                                          SkinColourProperty::kBackground,
                                          SkinColourState::kError);
        const auto white = skin.getColour(SkinObjectType::kDarkPanel,
                                          SkinColourProperty::kText,
                                          SkinColourState::kHover);

        look.backgroundOff = red;
        look.backgroundOn = red;
        look.backgroundHover = red;
        look.backgroundClicked = red;
        look.textOff = black;
        look.textOn = black;
        look.textHover = white;
        look.textClicked = white;
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
        // Same ColourSet as the slider when borrowing NumberBox editor tokens.
        look.editorText = skin.getColour(SkinObjectType::kNumberBox,
                                         SkinColourProperty::kEditorText,
                                         SkinColourState::kDefault,
                                         colourSet);
        look.editorCaret = skin.getColour(SkinObjectType::kNumberBox,
                                          SkinColourProperty::kEditorText,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    SliderLook sliderLookButtonLikeFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        SliderLook look;
        look.trackEnabled = skin.getColour(SkinObjectType::kSliderButtonLike,
                                           SkinColourProperty::kTrack,
                                           SkinColourState::kDefault,
                                           colourSet);
        look.focusBorder = skin.getColour(SkinObjectType::kSliderButtonLike,
                                          SkinColourProperty::kFocusBorder,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.valueBarEnabled = skin.getColour(SkinObjectType::kSliderButtonLike,
                                              SkinColourProperty::kValueBar,
                                              SkinColourState::kDefault,
                                              colourSet);
        look.textEnabled = skin.getColour(SkinObjectType::kSliderButtonLike,
                                          SkinColourProperty::kText,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.trackDisabled = skin.getSliderTrackColour(false, colourSet);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kSlider,
                                                 SkinColourProperty::kBackground,
                                                 SkinColourState::kDisabled,
                                                 colourSet);
        look.valueBarDisabled = skin.getSliderValueBarColour(false, colourSet);
        look.textDisabled = skin.getSliderTextColour(false, colourSet);
        look.editorText = skin.getColour(SkinObjectType::kNumberBox,
                                         SkinColourProperty::kEditorText,
                                         SkinColourState::kDefault,
                                         colourSet);
        look.editorCaret = skin.getColour(SkinObjectType::kNumberBox,
                                          SkinColourProperty::kEditorText,
                                          SkinColourState::kDefault,
                                          colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ComboBoxLook comboBoxLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        ComboBoxLook look;
        look.backgroundEnabled = skin.getComboBoxBackgroundColour(true, false, colourSet);
        look.backgroundDisabled = skin.getComboBoxBackgroundColour(false, false, colourSet);
        look.borderEnabled = skin.getComboBoxBorderColour(true, false, colourSet);
        look.borderDisabled = skin.getComboBoxBorderColour(false, false, colourSet);
        look.focusBorder = skin.getComboBoxFocusBorderColour(false, colourSet);
        look.triangleEnabled = skin.getComboBoxTriangleColour(true, false, colourSet);
        look.triangleDisabled = skin.getComboBoxTriangleColour(false, false, colourSet);
        look.textEnabled = skin.getComboBoxTextColour(true, false, colourSet);
        look.textDisabled = skin.getComboBoxTextColour(false, false, colourSet);

        look.buttonLikeBackground = skin.getComboBoxBackgroundColour(true, true, colourSet);
        look.buttonLikeBackgroundDisabled = skin.getComboBoxBackgroundColour(false, true, colourSet);
        look.buttonLikeBorder = skin.getComboBoxBorderColour(true, true, colourSet);
        look.buttonLikeBorderDisabled = skin.getComboBoxBorderColour(false, true, colourSet);
        look.buttonLikeText = skin.getComboBoxTextColour(true, true, colourSet);
        look.buttonLikeTextDisabled = skin.getComboBoxTextColour(false, true, colourSet);
        look.buttonLikeTriangle = skin.getComboBoxTriangleColour(true, true, colourSet);
        look.buttonLikeTriangleDisabled = skin.getComboBoxTriangleColour(false, true, colourSet);

        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    PopupMenuLook popupMenuLookFromSkin(const ISkin& skin, ColourSet colourSet)
    {
        PopupMenuLook look;
        look.background = skin.getPopupMenuBackgroundColour(false, colourSet);
        look.border = skin.getPopupMenuBorderColour(false, colourSet);
        look.separator = skin.getPopupMenuSeparatorColour(false, colourSet);
        look.text = skin.getPopupMenuTextColour(false, colourSet);
        look.textDisabled = skin.getColour(SkinObjectType::kPopupMenu,
                                           SkinColourProperty::kText,
                                           SkinColourState::kDisabled,
                                           colourSet);
        look.backgroundHover = skin.getPopupMenuBackgroundHooverColour(false, colourSet);
        look.textHover = skin.getPopupMenuTextHooverColour(false, colourSet);
        look.scrollbar = skin.getPopupMenuScrollbarColour(false, colourSet);

        look.backgroundButtonLike = skin.getPopupMenuBackgroundColour(true, colourSet);
        look.borderButtonLike = skin.getPopupMenuBorderColour(true, colourSet);
        look.separatorButtonLike = skin.getPopupMenuSeparatorColour(true, colourSet);
        look.textButtonLike = skin.getPopupMenuTextColour(true, colourSet);
        look.textDisabledButtonLike = skin.getColour(SkinObjectType::kPopupMenuButtonLike,
                                                     SkinColourProperty::kText,
                                                     SkinColourState::kDisabled,
                                                     colourSet);
        look.backgroundHoverButtonLike = skin.getPopupMenuBackgroundHooverColour(true, colourSet);
        look.textHoverButtonLike = skin.getPopupMenuTextHooverColour(true, colourSet);
        look.scrollbarButtonLike = skin.getPopupMenuScrollbarColour(true, colourSet);

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
        look.borderOff = skin.getColour(SkinObjectType::kButton,
                                        SkinColourProperty::kBorder,
                                        SkinColourState::kOff,
                                        colourSet);
        look.borderOn = skin.getColour(SkinObjectType::kButton,
                                       SkinColourProperty::kBorder,
                                       SkinColourState::kOn,
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
        look.editorSelectionBackground = skin.getColour(SkinObjectType::kNumberBox,
                                                        SkinColourProperty::kEditorSelectionBackground,
                                                        SkinColourState::kDefault,
                                                        colourSet);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }
}
