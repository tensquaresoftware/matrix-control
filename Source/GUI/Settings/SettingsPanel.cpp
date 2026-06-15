#include "SettingsPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
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
                                 {}})
    , masterOpsLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kMasterOperationsSection)
    , masterOpsPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , policiesLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kPoliciesSection)
    , policiesPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , defragLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kDefragSection)
    , defragPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , loggingLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kLoggingSection)
    , loggingPlaceholder_(kContentWidth_ - kLabelWidth_ - kGap_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , uiElementsButton_(kContentWidth_, kControlHeight_, TSS::buttonLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kUiElementsButton)
{
    setOpaque(true);

    addAndMakeVisible(hardwareLatencyLabel_);
    addAndMakeVisible(hardwareLatencySlider_);

    addAndMakeVisible(masterOpsLabel_);
    addAndMakeVisible(masterOpsPlaceholder_);
    addAndMakeVisible(policiesLabel_);
    addAndMakeVisible(policiesPlaceholder_);
    addAndMakeVisible(defragLabel_);
    addAndMakeVisible(defragPlaceholder_);
    addAndMakeVisible(loggingLabel_);
    addAndMakeVisible(loggingPlaceholder_);

    uiElementsButton_.setClickingTogglesState(true);
    addAndMakeVisible(uiElementsButton_);

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
    layoutPlaceholderRow(policiesLabel_, policiesPlaceholder_);
    layoutPlaceholderRow(defragLabel_, defragPlaceholder_);
    layoutPlaceholderRow(loggingLabel_, loggingPlaceholder_);

    bounds.removeFromTop(rowGap);
    uiElementsButton_.setBounds(bounds.removeFromBottom(controlHeight));
    uiElementsButton_.setUiScale(uiScale_);
}

void SettingsPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;

    const auto labelLook = TSS::labelLookFromSkin(skin);
    hardwareLatencyLabel_.setLook(labelLook);
    masterOpsLabel_.setLook(labelLook);
    masterOpsPlaceholder_.setLook(labelLook);
    policiesLabel_.setLook(labelLook);
    policiesPlaceholder_.setLook(labelLook);
    defragLabel_.setLook(labelLook);
    defragPlaceholder_.setLook(labelLook);
    loggingLabel_.setLook(labelLook);
    loggingPlaceholder_.setLook(labelLook);

    hardwareLatencySlider_.setLook(TSS::sliderLookFromSkin(skin));
    uiElementsButton_.setLook(TSS::buttonLookFromSkin(skin));

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
