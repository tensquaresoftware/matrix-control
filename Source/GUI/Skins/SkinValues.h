#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
    enum class SkinValueId
    {
        kLabelFontSize,
        kLabelPadding,
        kGroupLabelFontSize,
        kSliderWidth,
        kSliderHeight,
        kSliderTrackHeight,
        kButtonWidth,
        kButtonHeight,
        kComboBoxWidth,
        kComboBoxHeight,
        kNumberBoxWidth,
        kNumberBoxHeight,
        kWidgetRoundedCorner,
        kSeparatorThickness,
        kHeaderHeight,
        kFooterHeight,
        kModulePanelPadding,
        
        kNumValues
    };

    enum class SkinColourId
    {
        // Panel colours
        kHeaderPanelBackground,
        kHeaderPanelLabelText,
        kBodyPanelBackground,
        kFooterPanelBackground,
        
        // SectionHeader colours
        kSectionHeaderText,
        kSectionHeaderLineBlue,
        kSectionHeaderLineOrange,
        
        // ModuleHeader colours
        kModuleHeaderText,
        kModuleHeaderLineBlue,
        kModuleHeaderLineOrange,
        
        // GroupLabel colours
        kGroupLabelText,
        kGroupLabelLine,
        
        // Label colours
        kLabelText,
        
        // Separator colours
        kVerticalSeparatorLine,
        kHorizontalSeparatorLine,
        
        // Button colours
        kButtonBackgroundOff,
        kButtonBorderOff,
        kButtonTextOff,
        kButtonBackgroundOn,
        kButtonBorderOn,
        kButtonTextOn,
        kButtonBackgroundHover,
        kButtonTextHover,
        kButtonBackgroundClicked,
        kButtonTextClicked,
        
        // Toggle colours
        kToggleBorder,
        kToggleBackgroundOff,
        kToggleTextOff,
        kToggleBackgroundOn,
        kToggleTextOn,
        
        // Slider colours
        kSliderTrackEnabled,
        kSliderTrackDisabled,
        kSliderValueBarEnabled,
        kSliderValueBarDisabled,
        kSliderTextEnabled,
        kSliderTextDisabled,
        kSliderFocusBorder,
        
        // ComboBox colours
        kComboBoxBackgroundEnabled,
        kComboBoxBackgroundDisabled,
        kComboBoxBorderEnabled,
        kComboBoxBorderDisabled,
        kComboBoxFocusBorder,
        kComboBoxTriangleEnabled,
        kComboBoxTriangleDisabled,
        kComboBoxTextEnabled,
        kComboBoxTextDisabled,
        kComboBoxButtonLikeBackground,
        kComboBoxButtonLikeBackgroundDisabled,
        kComboBoxButtonLikeBorder,
        kComboBoxButtonLikeBorderDisabled,
        kComboBoxButtonLikeText,
        kComboBoxButtonLikeTextDisabled,
        kComboBoxButtonLikeTriangle,
        kComboBoxButtonLikeTriangleDisabled,
        
        // PopupMenu colours
        kPopupMenuBackground,
        kPopupMenuBorder,
        kPopupMenuSeparator,
        kPopupMenuText,
        kPopupMenuBackgroundHover,
        kPopupMenuTextHover,
        kPopupMenuScrollbar,
        kPopupMenuBackgroundButtonLike,
        kPopupMenuBorderButtonLike,
        kPopupMenuSeparatorButtonLike,
        kPopupMenuTextButtonLike,
        kPopupMenuBackgroundHoverButtonLike,
        kPopupMenuTextHoverButtonLike,
        kPopupMenuScrollbarButtonLike,
        
        // NumberBox colours
        kNumberBoxText,
        kNumberBoxDot,
        kNumberBoxEditorBackground,
        kNumberBoxEditorSelectionBackground,
        kNumberBoxEditorText,
        
        // Display colours
        kEnvelopeDisplayBackground,
        kEnvelopeDisplayBorder,
        kEnvelopeDisplayEnvelope,
        kPatchNameDisplayBackground,
        kPatchNameDisplayBorder,
        kPatchNameDisplayText,
        kTrackGeneratorDisplayBackground,
        kTrackGeneratorDisplayBorder,
        kTrackGeneratorDisplayShaper,
        
        kNumColours
    };

    constexpr float kBaseFontHeight = 14.0f;
    constexpr float kModuleHeaderFontHeight = 16.0f;
    constexpr float kSectionHeaderFontHeight = 20.0f;

    constexpr float kDefaultValues[] = {
        10.0f,  // kLabelFontSize
        2.0f,   // kLabelPadding
        11.0f,  // kGroupLabelFontSize
        50.0f,  // kSliderWidth
        70.0f,  // kSliderHeight
        16.0f,  // kSliderTrackHeight
        50.0f,  // kButtonWidth
        20.0f,  // kButtonHeight
        100.0f, // kComboBoxWidth
        20.0f,  // kComboBoxHeight
        50.0f,  // kNumberBoxWidth
        20.0f,  // kNumberBoxHeight
        2.0f,   // kWidgetRoundedCorner
        1.0f,   // kSeparatorThickness
        40.0f,  // kHeaderHeight
        30.0f,  // kFooterHeight
        10.0f,  // kModulePanelPadding
    };

    static_assert(sizeof(kDefaultValues) / sizeof(float) == static_cast<int>(SkinValueId::kNumValues),
                  "kDefaultValues array size must match SkinValueId enum count");
}
