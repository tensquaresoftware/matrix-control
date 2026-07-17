#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    enum class SkinColourId
    {
        // Panel colours
        kHeaderPanelBackground,
        kBodyPanelBackground,
        kFooterPanelBackground,
        kDarkPanelText,
        kFooterMessageInfo,
        kFooterMessageSuccess,
        kFooterMessageWarning,
        kFooterMessageError,
        
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
        kButtonBackgroundDisabled,
        kButtonBorderDisabled,
        kButtonTextDisabled,
        
        // Toggle colours
        kToggleBorder,
        kToggleBackgroundOff,
        kToggleTextOff,
        kToggleBackgroundOn,
        kToggleTextOn,
        kToggleBackgroundDisabled,
        kToggleBorderDisabled,
        kToggleTextDisabled,
        
        // Slider colours
        kSliderTrackEnabled,
        kSliderTrackDisabled,
        kSliderBackgroundDisabled,
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

}
