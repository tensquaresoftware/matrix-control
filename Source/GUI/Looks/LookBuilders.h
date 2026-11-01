#pragma once

#include "GUI/Skins/SkinValues.h"
#include "WidgetLooks.h"

namespace TSS
{
    class ISkin;

    ButtonLook buttonLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    // Red fill / black text / white hover — value-pressure PANIC alert look.
    ButtonLook buttonAlertLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    SliderLook sliderLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    LabelLook labelLookFromSkin(const ISkin& skin);
    LabelLook chromeTextLabelLookFromSkin(const ISkin& skin);
    LabelLook brandLabelLookFromSkin(const ISkin& skin);
    HorizontalSeparatorLook horizontalSeparatorLookFromSkin(const ISkin& skin);
    ComboBoxLook comboBoxLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    PopupMenuLook popupMenuLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    EnvelopeDisplayLook envelopeDisplayLookFromSkin(const ISkin& skin);
    TrackGeneratorDisplayLook trackGeneratorDisplayLookFromSkin(const ISkin& skin);
    PatchNameDisplayLook patchNameDisplayLookFromSkin(const ISkin& skin);
    ReadOnlyValueFieldLook readOnlyValueFieldLookFromSkin(const ISkin& skin,
                                                          ColourSet colourSet = ColourSet::k1);
    LedLook ledLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    LedLook peakIndicatorLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    GroupLabelLook groupLabelLookFromSkin(const ISkin& skin);
    ToggleLook toggleLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    NumberBoxLook numberBoxLookFromSkin(const ISkin& skin, ColourSet colourSet = ColourSet::k1);
    SectionHeaderLook sectionHeaderLookFromSkin(const ISkin& skin);
    ModuleHeaderLook moduleHeaderLookFromSkin(const ISkin& skin);
    ModulationBusHeaderLook modulationBusHeaderLookFromSkin(const ISkin& skin);
}
