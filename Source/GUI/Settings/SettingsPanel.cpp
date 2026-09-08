#include "SettingsPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"

using TSS::SkinColourId;

SettingsPanel::SettingsPanel(TSS::ISkin& skin, bool isPluginMode)
    : skin_(&skin)
    , isPluginMode_(isPluginMode)
{
    setOpaque(true);

    setupPatchSection(skin);
    setupPatchMutatorSection(skin);
    setupMasterSection(skin);
    populateComboItems();
    applyComboPopupLooks(skin);

    setPluginMode(isPluginMode);
}

void SettingsPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void SettingsPanel::resized()
{
    const float sf = uiScale_;
    const int padding = juce::roundToInt(static_cast<float>(kPadding_) * sf);
    layoutContent(getLocalBounds().reduced(padding));
}

void SettingsPanel::layoutSectionHeader(juce::Rectangle<int>& bounds, const SectionHeaderLayoutArgs& args)
{
    const int titleGap = juce::roundToInt(static_cast<float>(kSectionTitleGap_) * uiScale_);

    auto titleRow = bounds.removeFromTop(args.controlHeight);
    args.title->setBounds(titleRow);
    args.title->setUiScale(uiScale_);
    bounds.removeFromTop(titleGap);

    auto sepRow = bounds.removeFromTop(args.separatorHeight);
    args.separator->setBounds(sepRow);
    args.separator->setUiScale(uiScale_);
    bounds.removeFromTop(args.rowGap);
}

void SettingsPanel::layoutLabeledControlRow(juce::Rectangle<int>& bounds,
                                            const RowLayoutMetrics& metrics,
                                            const LabeledControlRowArgs& args)
{
    // Match ParameterCell: label column, then control immediately at labelWidth (no gap).
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

void SettingsPanel::layoutPlaceholderRow(juce::Rectangle<int>& bounds,
                                         const RowLayoutMetrics& metrics,
                                         TSS::Label& label,
                                         TSS::Label& placeholder)
{
    auto row = bounds.removeFromTop(metrics.controlHeight);
    const int x = row.getX();
    const int y = row.getY();

    label.setBounds(x, y, metrics.labelWidth, metrics.controlHeight);
    label.setUiScale(uiScale_);
    placeholder.setBounds(x + metrics.labelWidth, y, metrics.comboWidth, metrics.controlHeight);
    placeholder.setUiScale(uiScale_);
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

void SettingsPanel::layoutPatchSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    layoutSectionHeader(bounds,
                        SectionHeaderLayoutArgs{ patchSectionLabel_.get(),
                                                 patchSectionSeparator_.get(),
                                                 metrics.controlHeight,
                                                 metrics.separatorHeight,
                                                 metrics.rowGap });
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
    layoutSectionHeader(bounds,
                        SectionHeaderLayoutArgs{ patchMutatorSectionLabel_.get(),
                                                 patchMutatorSectionSeparator_.get(),
                                                 metrics.controlHeight,
                                                 metrics.separatorHeight,
                                                 metrics.rowGap });
    layoutLabeledControlRow(bounds,
                            metrics,
                            LabeledControlRowArgs{ deleteWarningLabel_.get(),
                                                   deleteWarningCombo_.get(),
                                                   metrics.comboWidth });
    layoutPlaceholderRow(bounds, metrics, *defragHistoryLabel_, *defragHistoryPlaceholder_);
}

void SettingsPanel::layoutMasterSection(juce::Rectangle<int>& bounds, const RowLayoutMetrics& metrics)
{
    layoutSectionHeader(bounds,
                        SectionHeaderLayoutArgs{ masterSectionLabel_.get(),
                                                 masterSectionSeparator_.get(),
                                                 metrics.controlHeight,
                                                 metrics.separatorHeight,
                                                 metrics.rowGap });

    if (isPluginMode_)
    {
        layoutLabeledControlRow(bounds,
                                metrics,
                                LabeledControlRowArgs{ hardwareLatencyLabel_.get(),
                                                       hardwareLatencySlider_.get(),
                                                       metrics.sliderWidth });
    }

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

void SettingsPanel::layoutContent(juce::Rectangle<int> bounds)
{
    RowLayoutMetrics metrics;
    metrics.rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    metrics.labelWidth = juce::roundToInt(static_cast<float>(kLabelWidth_) * uiScale_);
    metrics.sliderWidth = juce::roundToInt(static_cast<float>(kSliderWidth_) * uiScale_);
    metrics.controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    metrics.separatorHeight = juce::roundToInt(static_cast<float>(kSeparatorHeight_) * uiScale_);
    metrics.comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);
    metrics.buttonGap = juce::roundToInt(static_cast<float>(kButtonGap_) * uiScale_);
    metrics.utilityLoadWidth = juce::roundToInt(static_cast<float>(kUtilityLoadWidth_) * uiScale_);
    metrics.utilitySaveAsWidth = juce::roundToInt(static_cast<float>(kUtilitySaveAsWidth_) * uiScale_);
    metrics.utilityInitWidth = juce::roundToInt(static_cast<float>(kUtilityInitWidth_) * uiScale_);
    metrics.saveAsInitWidth = juce::roundToInt(static_cast<float>(kSaveAsInitWidth_) * uiScale_);
    metrics.deleteInitWidth = juce::roundToInt(static_cast<float>(kDeleteInitWidth_) * uiScale_);

    layoutPatchSection(bounds, metrics);
    bounds.removeFromTop(metrics.rowGap);
    layoutPatchMutatorSection(bounds, metrics);
    bounds.removeFromTop(metrics.rowGap);
    layoutMasterSection(bounds, metrics);
}

void SettingsPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    applyChildLooks(skin);
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
    updateModeSpecificVisibility();
    resized();
}

void SettingsPanel::updateModeSpecificVisibility()
{
    const bool showPluginControls = isPluginMode_;

    hardwareLatencyLabel_->setVisible(showPluginControls);
    hardwareLatencySlider_->setVisible(showPluginControls);
}

void SettingsPanel::refreshInitTemplateDeleteEnablement(bool patchInitExists, bool masterInitExists)
{
    patchDeleteInitButton_->setEnabled(patchInitExists);
    masterDeleteInitButton_->setEnabled(masterInitExists);
}
