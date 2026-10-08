#include "SettingsPanel.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using TSS::SkinColourId;

SettingsPanel::SettingsPanel(TSS::ISkin& skin, bool isPluginMode)
    : skin_(&skin)
    , isPluginMode_(isPluginMode)
    , activeTabId_(PluginIDs::Settings::LastTab::kDefault)
{
    setOpaque(true);

    setupInterfaceSection(skin);
    setupDeviceSection(skin);
    setupPatchSection(skin);
    setupPatchMutatorSection(skin);
    setupMasterSection(skin);
    populateComboItems();
    applyComboPopupLooks(skin);

    setPluginMode(isPluginMode);
    refreshEpromTypeItems(PluginIDs::Settings::EpromType::kDefault);
}

void SettingsPanel::attachAudioPage(SettingsAudioPage::Config config)
{
    if (isPluginMode_ || audioPage_ != nullptr)
        return;

    audioPage_ = std::make_unique<SettingsAudioPage>(std::move(config));
    addChildComponent(*audioPage_);
    if (contextualHelpBinder_ != nullptr)
        audioPage_->registerContextualHelp(*contextualHelpBinder_);
    updatePageVisibility();
    resized();
}

void SettingsPanel::attachMidiPage(SettingsMidiPage::Config config)
{
    if (midiPage_ != nullptr)
        return;

    midiPage_ = std::make_unique<SettingsMidiPage>(std::move(config));
    addChildComponent(*midiPage_);
    if (contextualHelpBinder_ != nullptr)
        midiPage_->registerContextualHelp(*contextualHelpBinder_);
    updatePageVisibility();
    resized();
}

void SettingsPanel::registerContextualHelp(TSS::ContextualHelpBinder::FooterResolver resolveFooter)
{
    namespace Help = PluginDisplayNames::Settings::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(std::move(resolveFooter));
    contextualHelpBinder_->setHostShowingPredicate([this] { return isShowing(); });

    contextualHelpBinder_->bind(uiScaleLabel_.get(), Help::kUiScale);
    contextualHelpBinder_->bind(uiScaleCombo_.get(), Help::kUiScale);
    contextualHelpBinder_->bind(skinLabel_.get(), Help::kSkin);
    contextualHelpBinder_->bind(skinCombo_.get(), Help::kSkin);
    contextualHelpBinder_->bind(infoMessageCombo_.get(), Help::kInfoMessage);
    contextualHelpBinder_->bind(contextualHelpCombo_.get(), Help::kContextualHelp);
    contextualHelpBinder_->bind(gettingStartedLabel_.get(), Help::kGettingStartedAutoOpen);
    contextualHelpBinder_->bind(gettingStartedAutoOpenCombo_.get(), Help::kGettingStartedAutoOpen);
    contextualHelpBinder_->bind(runSetupAgainButton_.get(), Help::kRunSetupAgain);
    contextualHelpBinder_->bind(hardwareLatencySlider_.get(), Help::kHardwareLatency);
    contextualHelpBinder_->bind(epromTypeCombo_.get(), Help::kEpromType);
    contextualHelpBinder_->bind(matrix1000PatchesCombo_.get(), Help::kMatrix1000Patches);
    contextualHelpBinder_->bind(computerPatchesCombo_.get(), Help::kComputerPatches);
    contextualHelpBinder_->bind(unsavedStateCombo_.get(), Help::kUnsavedState);
    contextualHelpBinder_->bind(patchSaveAsInitButton_.get(), Help::kPatchSaveAsInit);
    contextualHelpBinder_->bind(patchDeleteInitButton_.get(), Help::kPatchDeleteInit);
    contextualHelpBinder_->bind(deleteWarningCombo_.get(), Help::kDeleteWarning);
    contextualHelpBinder_->bind(defragHistoryLabel_.get(), Help::kDefragHistory);
    contextualHelpBinder_->bind(defragHistoryButton_.get(), Help::kDefragHistory);
    contextualHelpBinder_->bind(masterLoadButton_.get(), Help::kMasterLoad);
    contextualHelpBinder_->bind(masterSaveAsButton_.get(), Help::kMasterSaveAs);
    contextualHelpBinder_->bind(masterInitButton_.get(), Help::kMasterInit);
    contextualHelpBinder_->bind(masterSaveAsInitButton_.get(), Help::kMasterSaveAsInit);
    contextualHelpBinder_->bind(masterDeleteInitButton_.get(), Help::kMasterDeleteInit);

    if (midiPage_ != nullptr)
        midiPage_->registerContextualHelp(*contextualHelpBinder_);
    if (audioPage_ != nullptr)
        audioPage_->registerContextualHelp(*contextualHelpBinder_);
}

void SettingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void SettingsPanel::resized()
{
    const int padding = TSS::ScaledLayout::scaledInt(static_cast<float>(kPadding_), uiScale_);
    layoutContent(getLocalBounds().reduced(padding));
}

void SettingsPanel::layoutLabeledControlRow(juce::Rectangle<int>& bounds,
                                            const RowLayoutMetrics& metrics,
                                            const LabeledControlRowArgs& args)
{
    auto row = bounds.removeFromTop(metrics.controlHeight);
    const int x = row.getX();
    const int y = row.getY();

    args.label->setBounds(x, y, metrics.labelWidth, metrics.controlHeight);
    args.label->setUiScale(uiScale_);
    args.control->setBounds(x + metrics.labelWidth, y, args.controlWidth, metrics.controlHeight);
    if (auto* slider = dynamic_cast<TSS::Slider*>(args.control))
        slider->setUiScale(uiScale_);
    else if (auto* combo = dynamic_cast<TSS::ComboBox*>(args.control))
        combo->setUiScale(uiScale_);
    bounds.removeFromTop(metrics.rowGap);
}

void SettingsPanel::layoutButtonRow(juce::Rectangle<int>& bounds,
                                    const RowLayoutMetrics& metrics,
                                    const ButtonRowLayoutArgs& args)
{
    auto row = bounds.removeFromTop(metrics.controlHeight);
    const int x = row.getX();
    const int y = row.getY();

    args.label->setBounds(x, y, metrics.labelWidth, metrics.controlHeight);
    args.label->setUiScale(uiScale_);

    int cursorX = x + metrics.labelWidth;
    auto widthIt = args.buttonWidths.begin();

    for (auto* button : args.buttons)
    {
        const int width = (widthIt != args.buttonWidths.end()) ? *widthIt++ : metrics.comboWidth;
        button->setBounds(cursorX, y, width, metrics.controlHeight);
        button->setUiScale(uiScale_);
        cursorX += width + metrics.buttonGap;
    }

    bounds.removeFromTop(metrics.rowGap);
}

void SettingsPanel::layoutDeviceSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    if (SettingsShellMetrics::deviceShowsHardwareLatency(isPluginMode_))
    {
        layoutLabeledControlRow(bounds,
                                metrics,
                                LabeledControlRowArgs{ hardwareLatencyLabel_.get(),
                                                       hardwareLatencySlider_.get(),
                                                       metrics.sliderWidth });
    }

    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ epromTypeLabel_.get(),
                                                   epromTypeCombo_.get(),
                                                   metrics.comboWidth });
}

void SettingsPanel::layoutPatchSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ matrix1000PatchesLabel_.get(),
                                                   matrix1000PatchesCombo_.get(),
                                                   metrics.comboWidth });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ computerPatchesLabel_.get(),
                                                   computerPatchesCombo_.get(),
                                                   metrics.comboWidth });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ unsavedStateLabel_.get(),
                                                   unsavedStateCombo_.get(),
                                                   metrics.comboWidth });
    layoutButtonRow(bounds,
                    metrics,
                    ButtonRowLayoutArgs{ patchInitTemplateLabel_.get(),
                                         { patchSaveAsInitButton_.get(), patchDeleteInitButton_.get() },
                                         { metrics.saveAsInitWidth, metrics.deleteInitWidth } });
}

void SettingsPanel::layoutPatchMutatorSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ deleteWarningLabel_.get(),
                                                   deleteWarningCombo_.get(),
                                                   metrics.comboWidth });
    layoutButtonRow(bounds,
                    metrics,
                    ButtonRowLayoutArgs{ defragHistoryLabel_.get(),
                                         { defragHistoryButton_.get() },
                                         { metrics.defragButtonWidth } });
}

void SettingsPanel::layoutMasterSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    layoutButtonRow(bounds,
                    metrics,
                    ButtonRowLayoutArgs{ masterUtilityLabel_.get(),
                                         { masterLoadButton_.get(),
                                           masterSaveAsButton_.get(),
                                           masterInitButton_.get() },
                                         { metrics.utilityLoadWidth,
                                           metrics.utilitySaveAsWidth,
                                           metrics.utilityInitWidth } });
    layoutButtonRow(bounds,
                    metrics,
                    ButtonRowLayoutArgs{ masterInitTemplateLabel_.get(),
                                         { masterSaveAsInitButton_.get(), masterDeleteInitButton_.get() },
                                         { metrics.saveAsInitWidth, metrics.deleteInitWidth } });
}

void SettingsPanel::layoutMidiSection(juce::Rectangle<int>& bounds)
{
    if (midiPage_ == nullptr)
        return;

    midiPage_->setBounds(bounds);
    midiPage_->setUiScale(uiScale_);
}

void SettingsPanel::layoutAudioSection(juce::Rectangle<int>& bounds)
{
    if (audioPage_ == nullptr)
        return;

    audioPage_->setBounds(bounds);
    audioPage_->setUiScale(uiScale_);
}

void SettingsPanel::layoutContent(juce::Rectangle<int> bounds)
{
    RowLayoutMetrics metrics;
    metrics.rowGap = TSS::ScaledLayout::scaledInt(static_cast<float>(kRowGap_), uiScale_);
    metrics.labelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kLabelWidth_), uiScale_);
    metrics.sliderWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kSliderWidth_), uiScale_);
    metrics.controlHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(kControlHeight_), uiScale_);
    metrics.comboWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kComboWidth_), uiScale_);
    metrics.buttonGap = TSS::ScaledLayout::scaledInt(static_cast<float>(kButtonGap_), uiScale_);
    metrics.utilityLoadWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kUtilityLoadWidth_), uiScale_);
    metrics.utilitySaveAsWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kUtilitySaveAsWidth_), uiScale_);
    metrics.utilityInitWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kUtilityInitWidth_), uiScale_);
    metrics.saveAsInitWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kSaveAsInitWidth_), uiScale_);
    metrics.deleteInitWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kDeleteInitWidth_), uiScale_);
    metrics.defragButtonWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(kDefragButtonWidth_), uiScale_);

    using namespace PluginIDs::Settings::LastTab;

    switch (activeTabId_)
    {
        case kDevice:
            layoutDeviceSection(bounds, metrics);
            break;
        case kMidi:
            layoutMidiSection(bounds);
            break;
        case kAudio:
            layoutAudioSection(bounds);
            break;
        case kPatch:
            layoutPatchSection(bounds, metrics);
            break;
        case kPatchMutator:
            layoutPatchMutatorSection(bounds, metrics);
            break;
        case kMaster:
            layoutMasterSection(bounds, metrics);
            break;
        case kUserInterface:
        default:
            layoutInterfaceSection(bounds, metrics);
            break;
    }
}

void SettingsPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    applyChildLooks(skin);
    if (midiPage_ != nullptr)
        midiPage_->setSkin(skin);
    if (audioPage_ != nullptr)
        audioPage_->setSkin(skin);
    repaint();
}

void SettingsPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

void SettingsPanel::setPluginMode(bool isPluginMode)
{
    isPluginMode_ = isPluginMode;
    updatePageVisibility();
    resized();
}

void SettingsPanel::setActiveTab(int tabId)
{
    const int normalized = PluginIDs::Settings::LastTab::normalize(tabId, isPluginMode_);
    if (activeTabId_ == normalized)
    {
        updatePageVisibility();
        return;
    }

    activeTabId_ = normalized;
    updatePageVisibility();
    resized();
}

void SettingsPanel::setDeviceType(MatrixDeviceTypes::Type deviceType)
{
    if (deviceType_ == deviceType)
        return;

    deviceType_ = deviceType;
    const int currentId = epromTypeCombo_->getSelectedId();
    refreshEpromTypeItems(currentId > 0 ? currentId : PluginIDs::Settings::EpromType::kDefault);
}

void SettingsPanel::updatePageVisibility()
{
    using namespace PluginIDs::Settings::LastTab;

    const bool showUi = activeTabId_ == kUserInterface;
    const bool showDevice = activeTabId_ == kDevice;
    const bool showMidi = activeTabId_ == kMidi && midiPage_ != nullptr;
    const bool showAudio = activeTabId_ == kAudio && audioPage_ != nullptr;
    const bool showPatch = activeTabId_ == kPatch;
    const bool showMutator = activeTabId_ == kPatchMutator;
    const bool showMaster = activeTabId_ == kMaster;
    const bool showLatency = showDevice
                             && SettingsShellMetrics::deviceShowsHardwareLatency(isPluginMode_);

    setInterfaceSectionVisible(showUi);

    hardwareLatencyLabel_->setVisible(showLatency);
    hardwareLatencySlider_->setVisible(showLatency);
    epromTypeLabel_->setVisible(showDevice);
    epromTypeCombo_->setVisible(showDevice);

    if (midiPage_ != nullptr)
        midiPage_->setVisible(showMidi);

    if (audioPage_ != nullptr)
        audioPage_->setVisible(showAudio);

    matrix1000PatchesLabel_->setVisible(showPatch);
    matrix1000PatchesCombo_->setVisible(showPatch);
    computerPatchesLabel_->setVisible(showPatch);
    computerPatchesCombo_->setVisible(showPatch);
    unsavedStateLabel_->setVisible(showPatch);
    unsavedStateCombo_->setVisible(showPatch);
    patchInitTemplateLabel_->setVisible(showPatch);
    patchSaveAsInitButton_->setVisible(showPatch);
    patchDeleteInitButton_->setVisible(showPatch);

    deleteWarningLabel_->setVisible(showMutator);
    deleteWarningCombo_->setVisible(showMutator);
    defragHistoryLabel_->setVisible(showMutator);
    defragHistoryButton_->setVisible(showMutator);

    masterUtilityLabel_->setVisible(showMaster);
    masterLoadButton_->setVisible(showMaster);
    masterSaveAsButton_->setVisible(showMaster);
    masterInitButton_->setVisible(showMaster);
    masterInitTemplateLabel_->setVisible(showMaster);
    masterSaveAsInitButton_->setVisible(showMaster);
    masterDeleteInitButton_->setVisible(showMaster);
}

int SettingsPanel::refreshEpromTypeItems(int preferredSelectedId)
{
    const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType_);
    const int selectedId = Core::EpromTypePolicy::coerceForDeviceFamily(preferredSelectedId, family);

    epromTypeCombo_->clear(juce::dontSendNotification);
    Core::EpromTypePolicy::forEachValidItem(family, [this](int id)
    {
        epromTypeCombo_->addItem(Core::EpromTypePolicy::displayNameForId(id), id);
    });
    epromTypeCombo_->setSelectedId(selectedId, juce::dontSendNotification);
    return selectedId;
}

void SettingsPanel::refreshInitTemplateDeleteEnablement(bool patchInitExists, bool masterInitExists)
{
    patchDeleteInitButton_->setEnabled(patchInitExists);
    masterDeleteInitButton_->setEnabled(masterInitExists);
}

void SettingsPanel::refreshDefragHistoryEnablement(bool hasMutationHistory)
{
    defragHistoryButton_->setEnabled(hasMutationHistory);
}
