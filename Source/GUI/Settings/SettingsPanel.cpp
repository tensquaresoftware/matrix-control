#include "SettingsPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Core/Audio/HardwareLatency.h"

using TSS::SkinColourId;

namespace
{
    int findItemIdForPortIdentifier(const std::vector<juce::String>& identifiers,
                                    const juce::String& deviceId,
                                    int sentinelItemId)
    {
        if (deviceId.isEmpty())
            return sentinelItemId;

        for (size_t i = 0; i < identifiers.size(); ++i)
        {
            if (identifiers[i] == deviceId)
                return static_cast<int>(i + 1);
        }

        return sentinelItemId;
    }
}

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
    , audioFromLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kAudioFromLabel)
    , audioFromComboBox_(kAudioFromComboWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , inputGainLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kInputGainLabel)
    , inputGainSlider_(kSliderWidth_, kControlHeight_, TSS::sliderLookFromSkin(skin),
                       TSS::SliderConfig{
                           PluginAudioConstants::kMinInputGainDb,
                           PluginAudioConstants::kMaxInputGainDb,
                           0.0,
                           1.0,
                           "dB",
                           "-inf"})
    , peakIndicator_(kPeakIndicatorWidth_, kControlHeight_)
    , masterOpsLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kMasterOperationsSection)
    , masterOpsPlaceholder_(kComboWidth_ * 2, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , policiesLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kPoliciesSection)
    , policiesPlaceholder_(kComboWidth_ * 2, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , defragLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kDefragSection)
    , defragPlaceholder_(kComboWidth_ * 2, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , loggingLabel_(kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kLoggingSection)
    , loggingPlaceholder_(kComboWidth_ * 2, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
{
    setOpaque(true);

    addAndMakeVisible(hardwareLatencyLabel_);
    addAndMakeVisible(hardwareLatencySlider_);

    audioFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(audioFromLabel_);
    addAndMakeVisible(audioFromComboBox_);
    addAndMakeVisible(inputGainLabel_);
    addAndMakeVisible(inputGainSlider_);
    peakIndicator_.setSkin(skin);
    addAndMakeVisible(peakIndicator_);

    addAndMakeVisible(masterOpsLabel_);
    addAndMakeVisible(masterOpsPlaceholder_);
    addAndMakeVisible(policiesLabel_);
    addAndMakeVisible(policiesPlaceholder_);
    addAndMakeVisible(defragLabel_);
    addAndMakeVisible(defragPlaceholder_);
    addAndMakeVisible(loggingLabel_);
    addAndMakeVisible(loggingPlaceholder_);

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
    const int audioComboWidth = juce::roundToInt(static_cast<float>(kAudioFromComboWidth_) * uiScale_);
    const int peakWidth = juce::roundToInt(static_cast<float>(kPeakIndicatorWidth_) * uiScale_);

    auto layoutRow = [&](TSS::Label& label, juce::Component& control, int controlWidth)
    {
        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        label.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        control.setBounds(row.removeFromLeft(controlWidth).withHeight(controlHeight));
        if (auto* slider = dynamic_cast<TSS::Slider*>(&control))
            slider->setUiScale(uiScale_);
        else if (auto* combo = dynamic_cast<TSS::ComboBox*>(&control))
            combo->setUiScale(uiScale_);
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
    else
    {
        layoutRow(audioFromLabel_, audioFromComboBox_, audioComboWidth);

        auto row = bounds.removeFromTop(controlHeight);
        inputGainLabel_.setBounds(row.removeFromLeft(labelWidth));
        inputGainLabel_.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        inputGainSlider_.setBounds(row.removeFromLeft(sliderWidth).withHeight(controlHeight));
        inputGainSlider_.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        peakIndicator_.setBounds(row.removeFromLeft(peakWidth).withHeight(controlHeight));
        peakIndicator_.setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    }

    layoutPlaceholderRow(masterOpsLabel_, masterOpsPlaceholder_);
    layoutPlaceholderRow(policiesLabel_, policiesPlaceholder_);
    layoutPlaceholderRow(defragLabel_, defragPlaceholder_);
    layoutPlaceholderRow(loggingLabel_, loggingPlaceholder_);
}

void SettingsPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;

    const auto labelLook = TSS::labelLookFromSkin(skin);
    hardwareLatencyLabel_.setLook(labelLook);
    audioFromLabel_.setLook(labelLook);
    inputGainLabel_.setLook(labelLook);
    masterOpsLabel_.setLook(labelLook);
    masterOpsPlaceholder_.setLook(labelLook);
    policiesLabel_.setLook(labelLook);
    policiesPlaceholder_.setLook(labelLook);
    defragLabel_.setLook(labelLook);
    defragPlaceholder_.setLook(labelLook);
    loggingLabel_.setLook(labelLook);
    loggingPlaceholder_.setLook(labelLook);

    const auto comboLook = TSS::comboBoxLookFromSkin(skin);
    const auto popupLook = TSS::popupMenuLookFromSkin(skin);
    audioFromComboBox_.setLook(comboLook);
    audioFromComboBox_.setPopupMenuLook(popupLook);

    hardwareLatencySlider_.setLook(TSS::sliderLookFromSkin(skin));
    inputGainSlider_.setLook(TSS::sliderLookFromSkin(skin));
    peakIndicator_.setSkin(skin);

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
    const bool showStandaloneControls = !isPluginMode_;

    hardwareLatencyLabel_.setVisible(showPluginControls);
    hardwareLatencySlider_.setVisible(showPluginControls);

    audioFromLabel_.setVisible(showStandaloneControls);
    audioFromComboBox_.setVisible(showStandaloneControls);
    inputGainLabel_.setVisible(showStandaloneControls);
    inputGainSlider_.setVisible(showStandaloneControls);
    peakIndicator_.setVisible(showStandaloneControls);
}

void SettingsPanel::populateAudioFromCombo(const juce::StringArray& channelNames,
                                           const juce::StringArray& channelIds)
{
    const auto previousSourceId = getSelectedAudioFromSourceId();

    audioFromComboBox_.clear(juce::dontSendNotification);
    audioFromSourceIdentifiers_.clear();

    const int count = juce::jmin(channelNames.size(), channelIds.size());

    for (int i = 0; i < count; ++i)
    {
        const int itemId = i + 1;
        audioFromComboBox_.addItem(channelNames[i], itemId);
        audioFromSourceIdentifiers_.push_back(channelIds[i]);
    }

    if (count == 0)
    {
        audioFromComboBox_.addItem(PluginDisplayNames::HeaderPanel::kPortNoneSentinel, kPortSentinelItemId);
        audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        return;
    }

    selectAudioFromSourceId(previousSourceId);
}

juce::String SettingsPanel::getSelectedAudioFromSourceId() const
{
    return getSelectedPortIdentifier(audioFromComboBox_, audioFromSourceIdentifiers_);
}

void SettingsPanel::selectAudioFromSourceId(const juce::String& sourceId)
{
    audioFromComboBox_.setSelectedId(findItemIdForIdentifier(audioFromSourceIdentifiers_, sourceId),
                                     juce::dontSendNotification);
}

int SettingsPanel::findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                           const juce::String& deviceId) const
{
    return findItemIdForPortIdentifier(identifiers, deviceId, kPortSentinelItemId);
}

juce::String SettingsPanel::getSelectedPortIdentifier(const TSS::ComboBox& combo,
                                                      const std::vector<juce::String>& identifiers) const
{
    const int itemId = combo.getSelectedId();
    if (itemId <= kPortSentinelItemId)
        return {};

    const auto index = static_cast<size_t>(itemId - 1);
    if (index >= identifiers.size())
        return {};

    return identifiers[index];
}
