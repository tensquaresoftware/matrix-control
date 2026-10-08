// Extracted from SettingsPanel.cpp for modular maintenance.
// Widget construction and initial combo wiring for the settings dialog.

#include "SettingsPanel.h"

#include "Core/Audio/HardwareLatency.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

std::unique_ptr<TSS::Label> SettingsPanel::makeLabel(TSS::ISkin& skin, int width, const juce::String& text)
{
    return std::make_unique<TSS::Label>(width, kControlHeight_, TSS::labelLookFromSkin(skin), text);
}

std::unique_ptr<TSS::ComboBox> SettingsPanel::makeCombo(TSS::ISkin& skin, int width)
{
    return std::make_unique<TSS::ComboBox>(width,
                                           kControlHeight_,
                                           TSS::comboBoxLookFromSkin(skin),
                                           TSS::ComboBox::Style::ButtonLike);
}

std::unique_ptr<TSS::Button> SettingsPanel::makeButton(TSS::ISkin& skin, int width, const juce::String& text)
{
    return std::make_unique<TSS::Button>(width, kControlHeight_, TSS::buttonLookFromSkin(skin), text);
}

void SettingsPanel::setupInterfaceSection(TSS::ISkin& skin)
{
    uiScaleLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kUiScaleRowLabel);
    uiScaleCombo_ = makeCombo(skin, kComboWidth_);
    skinLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kSkinRowLabel);
    skinCombo_ = makeCombo(skin, kComboWidth_);
    infoMessageLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kInfoMessageLabel);
    infoMessageCombo_ = makeCombo(skin, kComboWidth_);
    contextualHelpLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kContextualHelpLabel);
    contextualHelpCombo_ = makeCombo(skin, kComboWidth_);
    gettingStartedLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kGettingStartedLabel);
    gettingStartedAutoOpenCombo_ = makeCombo(skin, kComboWidth_);
    runSetupAgainButton_ =
        makeButton(skin, kComboWidth_, PluginDisplayNames::Settings::kRunSetupAgainButton);

    addAndMakeVisible(*uiScaleLabel_);
    addAndMakeVisible(*uiScaleCombo_);
    addAndMakeVisible(*skinLabel_);
    addAndMakeVisible(*skinCombo_);
    addAndMakeVisible(*infoMessageLabel_);
    addAndMakeVisible(*infoMessageCombo_);
    addAndMakeVisible(*contextualHelpLabel_);
    addAndMakeVisible(*contextualHelpCombo_);
    addAndMakeVisible(*gettingStartedLabel_);
    addAndMakeVisible(*gettingStartedAutoOpenCombo_);
    addAndMakeVisible(*runSetupAgainButton_);
}

void SettingsPanel::setupDeviceSection(TSS::ISkin& skin)
{
    hardwareLatencyLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kHardwareLatencyLabel);
    hardwareLatencySlider_ = std::make_unique<TSS::Slider>(
        kSliderWidth_,
        kControlHeight_,
        TSS::sliderLookButtonLikeFromSkin(skin),
        TSS::SliderConfig{
            .minValue = Core::HardwareLatency::kMinMs,
            .maxValue = Core::HardwareLatency::kMaxMs,
            .defaultValue = Core::HardwareLatency::kMinMs,
            .step = Core::HardwareLatency::kStepMs,
            .unit = "ms"});
    epromTypeLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kEpromTypeLabel);
    epromTypeCombo_ = makeCombo(skin, kComboWidth_);

    addAndMakeVisible(*hardwareLatencyLabel_);
    addAndMakeVisible(*hardwareLatencySlider_);
    addAndMakeVisible(*epromTypeLabel_);
    addAndMakeVisible(*epromTypeCombo_);
}

void SettingsPanel::setupPatchSection(TSS::ISkin& skin)
{
    matrix1000PatchesLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kMatrix1000PatchesLabel);
    matrix1000PatchesCombo_ = makeCombo(skin, kComboWidth_);
    computerPatchesLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kComputerPatchesLabel);
    computerPatchesCombo_ = makeCombo(skin, kComboWidth_);
    unsavedStateLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kUnsavedStateLabel);
    unsavedStateCombo_ = makeCombo(skin, kComboWidth_);
    patchInitTemplateLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kInitTemplateLabel);
    patchSaveAsInitButton_ =
        makeButton(skin, kSaveAsInitWidth_, PluginDisplayNames::Settings::kSaveAsInitButton);
    patchDeleteInitButton_ =
        makeButton(skin, kDeleteInitWidth_, PluginDisplayNames::Settings::kDeleteButton);

    addAndMakeVisible(*matrix1000PatchesLabel_);
    addAndMakeVisible(*matrix1000PatchesCombo_);
    addAndMakeVisible(*computerPatchesLabel_);
    addAndMakeVisible(*computerPatchesCombo_);
    addAndMakeVisible(*unsavedStateLabel_);
    addAndMakeVisible(*unsavedStateCombo_);
    addAndMakeVisible(*patchInitTemplateLabel_);
    addAndMakeVisible(*patchSaveAsInitButton_);
    addAndMakeVisible(*patchDeleteInitButton_);
}

void SettingsPanel::setupPatchMutatorSection(TSS::ISkin& skin)
{
    deleteWarningLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kDeleteWarningLabel);
    deleteWarningCombo_ = makeCombo(skin, kComboWidth_);
    defragHistoryLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kDefragHistoryLabel);
    defragHistoryButton_ =
        makeButton(skin, kDefragButtonWidth_, PluginDisplayNames::Settings::kDefragButton);
    defragHistoryButton_->setEnabled(false);

    addAndMakeVisible(*deleteWarningLabel_);
    addAndMakeVisible(*deleteWarningCombo_);
    addAndMakeVisible(*defragHistoryLabel_);
    addAndMakeVisible(*defragHistoryButton_);
}

void SettingsPanel::setupMasterSection(TSS::ISkin& skin)
{
    masterUtilityLabel_ = makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kUtilityLabel);
    masterLoadButton_ = makeButton(skin, kUtilityLoadWidth_, PluginDisplayNames::Settings::kLoadButton);
    masterSaveAsButton_ =
        makeButton(skin, kUtilitySaveAsWidth_, PluginDisplayNames::Settings::kSaveAsButton);
    masterInitButton_ = makeButton(skin, kUtilityInitWidth_, PluginDisplayNames::Settings::kInitButton);
    masterInitTemplateLabel_ =
        makeLabel(skin, kLabelWidth_, PluginDisplayNames::Settings::kInitTemplateLabel);
    masterSaveAsInitButton_ =
        makeButton(skin, kSaveAsInitWidth_, PluginDisplayNames::Settings::kSaveAsInitButton);
    masterDeleteInitButton_ =
        makeButton(skin, kDeleteInitWidth_, PluginDisplayNames::Settings::kDeleteButton);

    addAndMakeVisible(*masterUtilityLabel_);
    addAndMakeVisible(*masterLoadButton_);
    addAndMakeVisible(*masterSaveAsButton_);
    addAndMakeVisible(*masterInitButton_);
    addAndMakeVisible(*masterInitTemplateLabel_);
    addAndMakeVisible(*masterSaveAsInitButton_);
    addAndMakeVisible(*masterDeleteInitButton_);
}

void SettingsPanel::populateComboItems()
{
    using namespace PluginIDs::Settings::ScaleLevels;
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k50, k50);
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k75, k75);
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k100, k100);
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k125, k125);
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k150, k150);
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k175, k175);
    uiScaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k200, k200);

    using namespace PluginIDs::Settings::SkinVariants;
    skinCombo_->addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack, kBlack);
    skinCombo_->addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream, kCream);

    using namespace PluginIDs::Settings::InfoMessage;
    infoMessageCombo_->addItem(PluginDisplayNames::Settings::kKeep, kKeep);
    infoMessageCombo_->addItem(PluginDisplayNames::Settings::kAutoClear, kAutoClear);

    using namespace PluginIDs::Settings::ContextualHelp;
    contextualHelpCombo_->addItem(PluginDisplayNames::Settings::kShow, kShow);
    contextualHelpCombo_->addItem(PluginDisplayNames::Settings::kHide, kHide);

    using namespace PluginIDs::Settings::GettingStartedAutoOpen;
    gettingStartedAutoOpenCombo_->addItem(PluginDisplayNames::Settings::kShowWhenIncomplete,
                                          kShowWhenIncomplete);
    gettingStartedAutoOpenCombo_->addItem(PluginDisplayNames::Settings::kNeverAtLaunch,
                                          kNeverAtLaunch);

    using namespace PluginIDs::Settings::Matrix1000PatchesNamesMode;
    matrix1000PatchesCombo_->addItem(PluginDisplayNames::Settings::kDisplayMusicalNames,
                                     kDisplayMusicalNames);
    matrix1000PatchesCombo_->addItem(PluginDisplayNames::Settings::kDisplayHardwareNames,
                                     kDisplayHardwareNames);

    using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;
    computerPatchesCombo_->addItem(PluginDisplayNames::Settings::kDisplaySysexNames, kDisplaySysexNames);
    computerPatchesCombo_->addItem(PluginDisplayNames::Settings::kDisplayFileNames, kDisplayFileNames);
    computerPatchesCombo_->addItem(PluginDisplayNames::Settings::kAskOncePerLoad, kAskOncePerLoad);

    using namespace PluginIDs::Settings::UnsavedStatePolicy;
    unsavedStateCombo_->addItem(PluginDisplayNames::Settings::kAlwaysWarn, kAlwaysWarn);
    unsavedStateCombo_->addItem(PluginDisplayNames::Settings::kNeverWarn, kNeverWarn);

    deleteWarningCombo_->addItem(PluginDisplayNames::Settings::kAlwaysWarn,
                                 PluginIDs::Settings::DeleteWarningPolicy::kAlwaysWarn);
    deleteWarningCombo_->addItem(PluginDisplayNames::Settings::kNeverWarn,
                                 PluginIDs::Settings::DeleteWarningPolicy::kNeverWarn);
}

void SettingsPanel::applyComboPopupLooks(TSS::ISkin& skin)
{
    const auto popupLook = TSS::popupMenuLookFromSkin(skin);
    uiScaleCombo_->setPopupMenuLook(popupLook);
    skinCombo_->setPopupMenuLook(popupLook);
    infoMessageCombo_->setPopupMenuLook(popupLook);
    contextualHelpCombo_->setPopupMenuLook(popupLook);
    gettingStartedAutoOpenCombo_->setPopupMenuLook(popupLook);
    epromTypeCombo_->setPopupMenuLook(popupLook);
    matrix1000PatchesCombo_->setPopupMenuLook(popupLook);
    computerPatchesCombo_->setPopupMenuLook(popupLook);
    unsavedStateCombo_->setPopupMenuLook(popupLook);
    deleteWarningCombo_->setPopupMenuLook(popupLook);
}

void SettingsPanel::applyChildLooks(TSS::ISkin& skin)
{
    const auto labelLook = TSS::labelLookFromSkin(skin);
    const auto comboLook = TSS::comboBoxLookFromSkin(skin);
    const auto buttonLook = TSS::buttonLookFromSkin(skin);

    uiScaleLabel_->setLook(labelLook);
    uiScaleCombo_->setLook(comboLook);
    skinLabel_->setLook(labelLook);
    skinCombo_->setLook(comboLook);
    infoMessageLabel_->setLook(labelLook);
    infoMessageCombo_->setLook(comboLook);
    contextualHelpLabel_->setLook(labelLook);
    contextualHelpCombo_->setLook(comboLook);
    gettingStartedLabel_->setLook(labelLook);
    gettingStartedAutoOpenCombo_->setLook(comboLook);
    runSetupAgainButton_->setLook(buttonLook);

    hardwareLatencyLabel_->setLook(labelLook);
    hardwareLatencySlider_->setLook(TSS::sliderLookButtonLikeFromSkin(skin));
    epromTypeLabel_->setLook(labelLook);
    epromTypeCombo_->setLook(comboLook);

    matrix1000PatchesLabel_->setLook(labelLook);
    matrix1000PatchesCombo_->setLook(comboLook);
    computerPatchesLabel_->setLook(labelLook);
    computerPatchesCombo_->setLook(comboLook);
    unsavedStateLabel_->setLook(labelLook);
    unsavedStateCombo_->setLook(comboLook);
    patchInitTemplateLabel_->setLook(labelLook);
    patchSaveAsInitButton_->setLook(buttonLook);
    patchDeleteInitButton_->setLook(buttonLook);

    deleteWarningLabel_->setLook(labelLook);
    deleteWarningCombo_->setLook(comboLook);
    defragHistoryLabel_->setLook(labelLook);
    defragHistoryButton_->setLook(buttonLook);

    masterUtilityLabel_->setLook(labelLook);
    masterLoadButton_->setLook(buttonLook);
    masterSaveAsButton_->setLook(buttonLook);
    masterInitButton_->setLook(buttonLook);
    masterInitTemplateLabel_->setLook(labelLook);
    masterSaveAsInitButton_->setLook(buttonLook);
    masterDeleteInitButton_->setLook(buttonLook);

    applyComboPopupLooks(skin);
}

void SettingsPanel::layoutButtonOnlyRow(juce::Rectangle<int>& bounds,
                                        const RowLayoutMetrics& metrics,
                                        TSS::Button& button,
                                        int buttonWidth)
{
    auto row = bounds.removeFromTop(metrics.controlHeight);
    button.setBounds(row.getX() + metrics.labelWidth, row.getY(), buttonWidth, metrics.controlHeight);
    button.setUiScale(uiScale_);
    bounds.removeFromTop(metrics.rowGap);
}

void SettingsPanel::layoutInterfaceSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ uiScaleLabel_.get(),
                                                   uiScaleCombo_.get(),
                                                   metrics.comboWidth });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ skinLabel_.get(),
                                                   skinCombo_.get(),
                                                   metrics.comboWidth });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ infoMessageLabel_.get(),
                                                   infoMessageCombo_.get(),
                                                   metrics.comboWidth });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ contextualHelpLabel_.get(),
                                                   contextualHelpCombo_.get(),
                                                   metrics.comboWidth });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ gettingStartedLabel_.get(),
                                                   gettingStartedAutoOpenCombo_.get(),
                                                   metrics.comboWidth });
    layoutButtonOnlyRow(bounds, metrics, *runSetupAgainButton_, metrics.comboWidth);
}

void SettingsPanel::setInterfaceSectionVisible(bool visible)
{
    uiScaleLabel_->setVisible(visible);
    uiScaleCombo_->setVisible(visible);
    skinLabel_->setVisible(visible);
    skinCombo_->setVisible(visible);
    infoMessageLabel_->setVisible(visible);
    infoMessageCombo_->setVisible(visible);
    contextualHelpLabel_->setVisible(visible);
    contextualHelpCombo_->setVisible(visible);
    gettingStartedLabel_->setVisible(visible);
    gettingStartedAutoOpenCombo_->setVisible(visible);
    runSetupAgainButton_->setVisible(visible);
}
