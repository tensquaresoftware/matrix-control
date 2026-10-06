#pragma once

#include "WidgetLooks.h"

namespace TSS
{
    class ISkin;
    
    ButtonLook buttonLookFromSkin(const ISkin& skin);
    // Red fill / black text / white hover — queue-pressure PANIC alert look.
    ButtonLook buttonAlertLookFromSkin(const ISkin& skin);
    SliderLook sliderLookFromSkin(const ISkin& skin);
    // Settings monochrome greys (ComboBox ButtonLike parity); opt-in per control.
    SliderLook sliderLookButtonLikeFromSkin(const ISkin& skin);
    LabelLook labelLookFromSkin(const ISkin& skin);
    LabelLook darkPanelLabelLookFromSkin(const ISkin& skin);
    LabelLook brandLabelLookFromSkin(const ISkin& skin);
    HorizontalSeparatorLook horizontalSeparatorLookFromSkin(const ISkin& skin);
    ComboBoxLook comboBoxLookFromSkin(const ISkin& skin);
    PopupMenuLook popupMenuLookFromSkin(const ISkin& skin);
    EnvelopeDisplayLook envelopeDisplayLookFromSkin(const ISkin& skin);
    TrackGeneratorDisplayLook trackGeneratorDisplayLookFromSkin(const ISkin& skin);
    PatchNameDisplayLook patchNameDisplayLookFromSkin(const ISkin& skin);
    GroupLabelLook groupLabelLookFromSkin(const ISkin& skin);
    ToggleLook toggleLookFromSkin(const ISkin& skin);
    NumberBoxLook numberBoxLookFromSkin(const ISkin& skin);
    SectionHeaderLook sectionHeaderLookFromSkin(const ISkin& skin);
    ModuleHeaderLook moduleHeaderLookFromSkin(const ISkin& skin);
    ModulationBusHeaderLook modulationBusHeaderLookFromSkin(const ISkin& skin);
}
