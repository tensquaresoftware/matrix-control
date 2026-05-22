#include "LookBuilders.h"

#include "GUI/Looks/TypographyStyles.h"
#include "GUI/Skins/ISkin.h"

namespace tss
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

    ButtonLook buttonLookFromSkin(const ISkin& skin)
    {
        ButtonLook look;
        look.backgroundOff = skin.getColour(SkinColourId::kButtonBackgroundOff);
        look.backgroundOn = skin.getColour(SkinColourId::kButtonBackgroundOn);
        look.backgroundHover = skin.getColour(SkinColourId::kButtonBackgroundHover);
        look.backgroundClicked = skin.getColour(SkinColourId::kButtonBackgroundClicked);
        look.borderOff = skin.getColour(SkinColourId::kButtonBorderOff);
        look.borderOn = skin.getColour(SkinColourId::kButtonBorderOn);
        look.textOff = skin.getColour(SkinColourId::kButtonTextOff);
        look.textOn = skin.getColour(SkinColourId::kButtonTextOn);
        look.textHover = skin.getColour(SkinColourId::kButtonTextHover);
        look.textClicked = skin.getColour(SkinColourId::kButtonTextClicked);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    SliderLook sliderLookFromSkin(const ISkin& skin)
    {
        SliderLook look;
        look.trackEnabled = skin.getSliderTrackColour(true);
        look.trackDisabled = skin.getSliderTrackColour(false);
        look.valueBarEnabled = skin.getSliderValueBarColour(true);
        look.valueBarDisabled = skin.getSliderValueBarColour(false);
        look.textEnabled = skin.getSliderTextColour(true);
        look.textDisabled = skin.getSliderTextColour(false);
        look.focusBorder = skin.getColour(SkinColourId::kSliderFocusBorder);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook labelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinColourId::kLabelText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook headerPanelLabelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinColourId::kHeaderPanelLabelText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    HorizontalSeparatorLook horizontalSeparatorLookFromSkin(const ISkin& skin)
    {
        HorizontalSeparatorLook look;
        look.line = skin.getColour(SkinColourId::kHorizontalSeparatorLine);
        return look;
    }

    ComboBoxLook comboBoxLookFromSkin(const ISkin& skin)
    {
        ComboBoxLook look;
        look.backgroundEnabled = skin.getComboBoxBackgroundColour(true, false);
        look.backgroundDisabled = skin.getComboBoxBackgroundColour(false, false);
        look.borderEnabled = skin.getComboBoxBorderColour(true, false);
        look.borderDisabled = skin.getComboBoxBorderColour(false, false);
        look.focusBorder = skin.getComboBoxFocusBorderColour(false);
        look.triangleEnabled = skin.getComboBoxTriangleColour(true, false);
        look.triangleDisabled = skin.getComboBoxTriangleColour(false, false);
        look.textEnabled = skin.getComboBoxTextColour(true, false);
        look.textDisabled = skin.getComboBoxTextColour(false, false);
        
        look.buttonLikeBackground = skin.getComboBoxBackgroundColour(true, true);
        look.buttonLikeBackgroundDisabled = skin.getComboBoxBackgroundColour(false, true);
        look.buttonLikeBorder = skin.getComboBoxBorderColour(true, true);
        look.buttonLikeBorderDisabled = skin.getComboBoxBorderColour(false, true);
        look.buttonLikeText = skin.getComboBoxTextColour(true, true);
        look.buttonLikeTextDisabled = skin.getComboBoxTextColour(false, true);
        look.buttonLikeTriangle = skin.getComboBoxTriangleColour(true, true);
        look.buttonLikeTriangleDisabled = skin.getComboBoxTriangleColour(false, true);
        
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    PopupMenuLook popupMenuLookFromSkin(const ISkin& skin)
    {
        PopupMenuLook look;
        look.background = skin.getPopupMenuBackgroundColour(false);
        look.border = skin.getPopupMenuBorderColour(false);
        look.separator = skin.getPopupMenuSeparatorColour(false);
        look.text = skin.getPopupMenuTextColour(false);
        look.backgroundHover = skin.getPopupMenuBackgroundHooverColour(false);
        look.textHover = skin.getPopupMenuTextHooverColour(false);
        look.scrollbar = skin.getPopupMenuScrollbarColour(false);
        
        look.backgroundButtonLike = skin.getPopupMenuBackgroundColour(true);
        look.borderButtonLike = skin.getPopupMenuBorderColour(true);
        look.separatorButtonLike = skin.getPopupMenuSeparatorColour(true);
        look.textButtonLike = skin.getPopupMenuTextColour(true);
        look.backgroundHoverButtonLike = skin.getPopupMenuBackgroundHooverColour(true);
        look.textHoverButtonLike = skin.getPopupMenuTextHooverColour(true);
        look.scrollbarButtonLike = skin.getPopupMenuScrollbarColour(true);
        
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    EnvelopeDisplayLook envelopeDisplayLookFromSkin(const ISkin& skin)
    {
        EnvelopeDisplayLook look;
        look.background = skin.getColour(SkinColourId::kEnvelopeDisplayBackground);
        look.border = skin.getColour(SkinColourId::kEnvelopeDisplayBorder);
        look.envelope = skin.getColour(SkinColourId::kEnvelopeDisplayEnvelope);
        return look;
    }

    TrackGeneratorDisplayLook trackGeneratorDisplayLookFromSkin(const ISkin& skin)
    {
        TrackGeneratorDisplayLook look;
        look.background = skin.getColour(SkinColourId::kTrackGeneratorDisplayBackground);
        look.border = skin.getColour(SkinColourId::kTrackGeneratorDisplayBorder);
        look.curve = skin.getColour(SkinColourId::kTrackGeneratorDisplayShaper);
        return look;
    }

    PatchNameDisplayLook patchNameDisplayLookFromSkin(const ISkin& skin)
    {
        PatchNameDisplayLook look;
        look.background = skin.getColour(SkinColourId::kPatchNameDisplayBackground);
        look.border = skin.getColour(SkinColourId::kPatchNameDisplayBorder);
        look.text = skin.getColour(SkinColourId::kPatchNameDisplayText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kPatchName);
        return look;
    }

    GroupLabelLook groupLabelLookFromSkin(const ISkin& skin)
    {
        GroupLabelLook look;
        look.text = skin.getColour(SkinColourId::kGroupLabelText);
        look.line = skin.getColour(SkinColourId::kGroupLabelLine);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    VerticalSeparatorLook verticalSeparatorLookFromSkin(const ISkin& skin)
    {
        VerticalSeparatorLook look;
        look.line = skin.getColour(SkinColourId::kVerticalSeparatorLine);
        return look;
    }

    ToggleLook toggleLookFromSkin(const ISkin& skin)
    {
        ToggleLook look;
        look.border = skin.getColour(SkinColourId::kToggleBorder);
        look.backgroundOff = skin.getToggleBackgroundColour(false);
        look.backgroundOn = skin.getToggleBackgroundColour(true);
        look.textOff = skin.getToggleTextColour(false);
        look.textOn = skin.getToggleTextColour(true);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    NumberBoxLook numberBoxLookFromSkin(const ISkin& skin)
    {
        NumberBoxLook look;
        look.background = skin.getColour(SkinColourId::kButtonBackgroundOn);
        look.borderOff = skin.getColour(SkinColourId::kButtonBorderOff);
        look.borderOn = skin.getColour(SkinColourId::kButtonBorderOn);
        look.text = skin.getColour(SkinColourId::kNumberBoxText);
        look.dot = skin.getColour(SkinColourId::kNumberBoxDot);
        look.editorBackground = skin.getColour(SkinColourId::kNumberBoxEditorBackground);
        look.editorText = skin.getColour(SkinColourId::kNumberBoxEditorText);
        look.editorSelectionBackground = skin.getColour(SkinColourId::kNumberBoxEditorSelectionBackground);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    SectionHeaderLook sectionHeaderLookFromSkin(const ISkin& skin)
    {
        SectionHeaderLook look;
        look.text = skin.getColour(SkinColourId::kSectionHeaderText);
        look.lineBlue = skin.getColour(SkinColourId::kSectionHeaderLineBlue);
        look.lineOrange = skin.getColour(SkinColourId::kSectionHeaderLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kSectionHeader);
        return look;
    }

    ModuleHeaderLook moduleHeaderLookFromSkin(const ISkin& skin)
    {
        ModuleHeaderLook look;
        look.text = skin.getColour(SkinColourId::kModuleHeaderText);
        look.lineBlue = skin.getColour(SkinColourId::kModuleHeaderLineBlue);
        look.lineOrange = skin.getColour(SkinColourId::kModuleHeaderLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kModuleHeader);
        return look;
    }

    ModulationBusHeaderLook modulationBusHeaderLookFromSkin(const ISkin& skin)
    {
        ModulationBusHeaderLook look;
        look.text = skin.getColour(SkinColourId::kModuleHeaderText);
        look.lineBlue = skin.getColour(SkinColourId::kModuleHeaderLineBlue);
        look.lineOrange = skin.getColour(SkinColourId::kModuleHeaderLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kModuleHeader);
        return look;
    }
}
