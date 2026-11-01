#include "LookBuilders.h"

#include "GUI/Layout/Design/DesignPanels.h"
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

    ButtonLook buttonLookFromSkin(const ISkin& skin)
    {
        ButtonLook look;
        look.backgroundOff = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOff);
        look.backgroundOn = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOn);
        look.backgroundHover = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kHover);
        look.backgroundClicked = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kClicked);
        look.borderOff = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff);
        look.borderOn = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn);
        look.textOff = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOff);
        look.textOn = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOn);
        look.textHover = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kHover);
        look.textClicked = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kClicked);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kDisabled);
        look.borderDisabled = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kDisabled);
        look.textDisabled = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kDisabled);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ButtonLook buttonAlertLookFromSkin(const ISkin& skin)
    {
        // Keep base border colours; only fill/text switch to alert.
        // Reuse FooterMessage Error pastilles + DarkPanel hover white (no ColourChart dig).
        ButtonLook look = buttonLookFromSkin(skin);
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

    SliderLook sliderLookFromSkin(const ISkin& skin)
    {
        SliderLook look;
        look.trackEnabled = skin.getSliderTrackColour(true);
        look.trackDisabled = skin.getSliderTrackColour(false);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled);
        look.valueBarEnabled = skin.getSliderValueBarColour(true);
        look.valueBarDisabled = skin.getSliderValueBarColour(false);
        look.textEnabled = skin.getSliderTextColour(true);
        look.textDisabled = skin.getSliderTextColour(false);
        look.focusBorder = skin.getColour(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder);
        // Reuse NumberBox editor white so slider edit digits/caret stay high-contrast on the green plate.
        look.editorText = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText);
        look.editorCaret = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    SliderLook sliderLookButtonLikeFromSkin(const ISkin& skin)
    {
        SliderLook look;
        look.trackEnabled = skin.getColour(SkinObjectType::kSliderButtonLike, SkinColourProperty::kTrack);
        look.focusBorder = skin.getColour(SkinObjectType::kSliderButtonLike, SkinColourProperty::kFocusBorder);
        look.valueBarEnabled = skin.getColour(SkinObjectType::kSliderButtonLike, SkinColourProperty::kValueBar);
        look.textEnabled = skin.getColour(SkinObjectType::kSliderButtonLike, SkinColourProperty::kText);
        // Disabled roles keep the shared slider disabled tokens.
        look.trackDisabled = skin.getSliderTrackColour(false);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled);
        look.valueBarDisabled = skin.getSliderValueBarColour(false);
        look.textDisabled = skin.getSliderTextColour(false);
        // High-contrast editor digits/caret on the dark ButtonLike plate.
        look.editorText = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText);
        look.editorCaret = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook labelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook darkPanelLabelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    LabelLook brandLabelLookFromSkin(const ISkin& skin)
    {
        LabelLook look;
        look.text = skin.getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        look.font = skin.getBrandFontBold().withHeight(static_cast<float>(Design::Panels::Header::kLogoFontHeight));
        return look;
    }

    HorizontalSeparatorLook horizontalSeparatorLookFromSkin(const ISkin& skin)
    {
        HorizontalSeparatorLook look;
        look.line = skin.getColour(SkinObjectType::kHorizontalSeparator, SkinColourProperty::kLine);
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
        look.textDisabled = skin.getColour(SkinObjectType::kPopupMenu,
                                           SkinColourProperty::kText,
                                           SkinColourState::kDisabled);
        look.backgroundHover = skin.getPopupMenuBackgroundHooverColour(false);
        look.textHover = skin.getPopupMenuTextHooverColour(false);
        look.scrollbar = skin.getPopupMenuScrollbarColour(false);

        look.backgroundButtonLike = skin.getPopupMenuBackgroundColour(true);
        look.borderButtonLike = skin.getPopupMenuBorderColour(true);
        look.separatorButtonLike = skin.getPopupMenuSeparatorColour(true);
        look.textButtonLike = skin.getPopupMenuTextColour(true);
        look.textDisabledButtonLike = skin.getColour(SkinObjectType::kPopupMenuButtonLike,
                                                     SkinColourProperty::kText,
                                                     SkinColourState::kDisabled);
        look.backgroundHoverButtonLike = skin.getPopupMenuBackgroundHooverColour(true);
        look.textHoverButtonLike = skin.getPopupMenuTextHooverColour(true);
        look.scrollbarButtonLike = skin.getPopupMenuScrollbarColour(true);

        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    EnvelopeDisplayLook envelopeDisplayLookFromSkin(const ISkin& skin)
    {
        EnvelopeDisplayLook look;
        look.background = skin.getColour(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBackground);
        look.border = skin.getColour(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBorder);
        look.envelope = skin.getColour(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kEnvelope);
        return look;
    }

    TrackGeneratorDisplayLook trackGeneratorDisplayLookFromSkin(const ISkin& skin)
    {
        TrackGeneratorDisplayLook look;
        look.background = skin.getColour(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBackground);
        look.border = skin.getColour(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBorder);
        look.curve = skin.getColour(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kShaper);
        return look;
    }

    PatchNameDisplayLook patchNameDisplayLookFromSkin(const ISkin& skin)
    {
        // Secondary Mutator/Compare label uses a solid map entry (no withAlpha shortcut).
        // Primary idle/hover alphas (80% / 100%) stay paint-owned for the editability affordance.
        constexpr float kSecondaryFontHeightRatio = 0.7f;

        PatchNameDisplayLook look;
        look.background = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBackground);
        look.border = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBorder);
        look.text = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kText);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kPatchName);
        look.secondaryText = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kSecondaryText);
        look.secondaryFont = look.font.withHeight(look.font.getHeight() * kSecondaryFontHeightRatio);
        look.caretInk = skin.getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret);
        return look;
    }

    ReadOnlyValueFieldLook readOnlyValueFieldLookFromSkin(const ISkin& skin)
    {
        ReadOnlyValueFieldLook look;
        look.background = skin.getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground);
        look.text = skin.getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText);
        return look;
    }

    LedLook ledLookFromSkin(const ISkin& skin)
    {
        LedLook look;
        look.backgroundOff = skin.getColour(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOff);
        look.borderOff = skin.getColour(SkinObjectType::kLed, SkinColourProperty::kBorder, SkinColourState::kOff);
        look.maxOn = skin.getColour(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn);
        return look;
    }

    LedLook peakIndicatorLookFromSkin(const ISkin& skin)
    {
        LedLook look;
        look.backgroundOff = skin.getColour(SkinObjectType::kPeakIndicator,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kOff);
        look.borderOff = skin.getColour(SkinObjectType::kPeakIndicator,
                                        SkinColourProperty::kBorder,
                                        SkinColourState::kOff);
        look.maxOn = skin.getColour(SkinObjectType::kPeakIndicator,
                                    SkinColourProperty::kBackground,
                                    SkinColourState::kOn);
        return look;
    }

    GroupLabelLook groupLabelLookFromSkin(const ISkin& skin)
    {
        GroupLabelLook look;
        look.text = skin.getColour(SkinObjectType::kGroupLabel, SkinColourProperty::kText);
        look.line = skin.getColour(SkinObjectType::kGroupLabel, SkinColourProperty::kLine);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    ToggleLook toggleLookFromSkin(const ISkin& skin)
    {
        ToggleLook look;
        look.border = skin.getColour(SkinObjectType::kToggle, SkinColourProperty::kBorder);
        look.backgroundOff = skin.getToggleBackgroundColour(false);
        look.backgroundOn = skin.getToggleBackgroundColour(true);
        look.textOff = skin.getToggleTextColour(false);
        look.textOn = skin.getToggleTextColour(true);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kDisabled);
        look.borderDisabled = skin.getColour(SkinObjectType::kToggle, SkinColourProperty::kBorder, SkinColourState::kDisabled);
        look.textDisabled = skin.getColour(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kDisabled);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    NumberBoxLook numberBoxLookFromSkin(const ISkin& skin)
    {
        NumberBoxLook look;
        look.background = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kBackground);
        look.backgroundDisabled = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kBackground, SkinColourState::kDisabled);
        look.borderOff = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff);
        look.borderOn = skin.getColour(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn);
        look.text = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kText);
        look.textFocus = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus);
        look.editorBackground = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorBackground);
        look.editorText = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText);
        look.editorSelectionBackground = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorSelectionBackground);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kDefault);
        return look;
    }

    SectionHeaderLook sectionHeaderLookFromSkin(const ISkin& skin)
    {
        SectionHeaderLook look;
        look.text = skin.getColour(SkinObjectType::kSectionHeader, SkinColourProperty::kText);
        look.lineBlue = skin.getColour(SkinObjectType::kSectionHeader, SkinColourProperty::kLineBlue);
        look.lineOrange = skin.getColour(SkinObjectType::kSectionHeader, SkinColourProperty::kLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kSectionHeader);
        return look;
    }

    ModuleHeaderLook moduleHeaderLookFromSkin(const ISkin& skin)
    {
        ModuleHeaderLook look;
        look.text = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kText);
        look.textFocus = skin.getColour(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus);
        look.lineBlue = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue);
        look.lineOrange = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kModuleHeader);
        return look;
    }

    ModulationBusHeaderLook modulationBusHeaderLookFromSkin(const ISkin& skin)
    {
        ModulationBusHeaderLook look;
        look.text = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kText);
        look.lineBlue = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue);
        look.lineOrange = skin.getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange);
        look.font = resolvedTypographyFont(skin, TypographyStyleId::kModuleHeader);
        return look;
    }
}
