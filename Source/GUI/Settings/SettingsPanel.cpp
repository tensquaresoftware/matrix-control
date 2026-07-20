#include "SettingsPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Core/Audio/HardwareLatency.h"

using TSS::SkinColourId;

SettingsPanel::SettingsPanel(TSS::ISkin& skin, bool isPluginMode)
    : skin_(&skin)
    , isPluginMode_(isPluginMode)
    , hardwareLatencyLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kHardwareLatencyLabel)
    , hardwareLatencySlider_(kSliderWidth_, kControlHeight_, TSS::sliderLookFromSkin(skin),
                             TSS::SliderConfig{
                                 Core::HardwareLatency::kMinMs,
                                 Core::HardwareLatency::kMaxMs,
                                 Core::HardwareLatency::kMinMs,
                                 Core::HardwareLatency::kStepMs,
                                 "ms",
                                 {},
                                 {},
                                 {}})
    , masterOpsLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kMasterOperationsSection)
    , masterOpsPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , policiesLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kPoliciesSection)
    , nameReconciliationPolicyCombo_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::comboBoxLookFromSkin(skin))
    , unsavedEditWarningLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kUnsavedEditWarningLabel)
    , unsavedEditWarningPolicyCombo_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::comboBoxLookFromSkin(skin))
    , defragLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kDefragSection)
    , defragPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , loggingLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kLoggingSection)
    , loggingPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
{
    setOpaque(true);

    addAndMakeVisible(hardwareLatencyLabel_);
    addAndMakeVisible(hardwareLatencySlider_);

    addAndMakeVisible(masterOpsLabel_);
    addAndMakeVisible(masterOpsPlaceholder_);
    addAndMakeVisible(policiesLabel_);
    addAndMakeVisible(nameReconciliationPolicyCombo_);
    addAndMakeVisible(unsavedEditWarningLabel_);
    addAndMakeVisible(unsavedEditWarningPolicyCombo_);
    addAndMakeVisible(defragLabel_);
    addAndMakeVisible(defragPlaceholder_);
    addAndMakeVisible(loggingLabel_);
    addAndMakeVisible(loggingPlaceholder_);

    using namespace PluginIDs::Settings::NameReconciliationPolicy;
    nameReconciliationPolicyCombo_.addItem(PluginDisplayNames::Settings::kNameReconciliationPreferInternal,
                                         kPreferInternal);
    nameReconciliationPolicyCombo_.addItem(PluginDisplayNames::Settings::kNameReconciliationPreferFilename,
                                         kPreferFilename);
    nameReconciliationPolicyCombo_.addItem(PluginDisplayNames::Settings::kNameReconciliationAskOnce,
                                         kAskOncePerLoad);

    using namespace PluginIDs::Settings::UnsavedEditWarningPolicy;
    unsavedEditWarningPolicyCombo_.addItem(PluginDisplayNames::Settings::kUnsavedEditWarnAlways, kWarnAlways);
    unsavedEditWarningPolicyCombo_.addItem(PluginDisplayNames::Settings::kUnsavedEditNeverWarn, kNeverWarn);

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

void SettingsPanel::layoutContent(juce::Rectangle<int> bounds)
{
    const int rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    const int gap = juce::roundToInt(static_cast<float>(kGap_) * uiScale_);
    const int labelWidth = juce::roundToInt(static_cast<float>(kLabelWidth_) * uiScale_);
    const int sliderWidth = juce::roundToInt(static_cast<float>(kSliderWidth_) * uiScale_);
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);

    auto layoutRow = [&](TSS::Label& label, juce::Component& control, int controlWidth)
    {
        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        label.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        control.setBounds(row.removeFromLeft(controlWidth).withHeight(controlHeight));
        if (auto* slider = dynamic_cast<TSS::Slider*>(&control))
            slider->setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    };

    auto layoutPlaceholderRow = [&](TSS::Label& label, TSS::Label& placeholder)
    {
        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        label.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        placeholder.setBounds(row);
        placeholder.setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    };

    if (isPluginMode_)
    {
        layoutRow(hardwareLatencyLabel_, hardwareLatencySlider_, sliderWidth);
        bounds.removeFromTop(rowGap);
    }

    layoutPlaceholderRow(masterOpsLabel_, masterOpsPlaceholder_);
    layoutRow(policiesLabel_, nameReconciliationPolicyCombo_, bounds.getWidth() - labelWidth - gap);
    layoutRow(unsavedEditWarningLabel_, unsavedEditWarningPolicyCombo_, bounds.getWidth() - labelWidth - gap);
    layoutPlaceholderRow(defragLabel_, defragPlaceholder_);
    layoutPlaceholderRow(loggingLabel_, loggingPlaceholder_);
}

void SettingsPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;

    const auto labelLook = TSS::labelLookFromSkin(skin);
    hardwareLatencyLabel_.setLook(labelLook);
    masterOpsLabel_.setLook(labelLook);
    masterOpsPlaceholder_.setLook(labelLook);
    policiesLabel_.setLook(labelLook);
    nameReconciliationPolicyCombo_.setLook(TSS::comboBoxLookFromSkin(skin));
    unsavedEditWarningLabel_.setLook(labelLook);
    unsavedEditWarningPolicyCombo_.setLook(TSS::comboBoxLookFromSkin(skin));
    defragLabel_.setLook(labelLook);
    defragPlaceholder_.setLook(labelLook);
    loggingLabel_.setLook(labelLook);
    loggingPlaceholder_.setLook(labelLook);

    hardwareLatencySlider_.setLook(TSS::sliderLookFromSkin(skin));

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

    hardwareLatencyLabel_.setVisible(showPluginControls);
    hardwareLatencySlider_.setVisible(showPluginControls);
}
