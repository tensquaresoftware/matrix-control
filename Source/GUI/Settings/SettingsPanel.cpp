#include "SettingsPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Core/Audio/HardwareLatency.h"

using tss::SkinColourId;

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

SettingsPanel::SettingsPanel(tss::ISkin& skin, bool isPluginMode)
    : skin_(&skin)
    , isPluginMode_(isPluginMode)
    , skinLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kSkinLabel)
    , skinComboBox_(kComboWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , uiScaleLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kUiScaleLabel)
    , uiScaleComboBox_(kComboWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , hardwareLatencyLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kHardwareLatencyLabel)
    , hardwareLatencySlider_(kSliderWidth_, kControlHeight_, tss::sliderLookFromSkin(skin),
                             tss::SliderConfig{
                                 Core::HardwareLatency::kMinMs,
                                 Core::HardwareLatency::kMaxMs,
                                 Core::HardwareLatency::kMinMs,
                                 Core::HardwareLatency::kStepMs,
                                 "ms"})
    , audioFromLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kAudioFromLabel)
    , audioFromComboBox_(kAudioFromComboWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , inputGainLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kInputGainLabel)
    , inputGainSlider_(kSliderWidth_, kControlHeight_, tss::sliderLookFromSkin(skin),
                       tss::SliderConfig{
                           PluginAudioConstants::kMinInputGainDb,
                           PluginAudioConstants::kMaxInputGainDb,
                           0.0,
                           1.0,
                           "dB",
                           "-inf"})
    , peakIndicator_(kPeakIndicatorWidth_, kControlHeight_)
    , masterOpsLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kMasterOperationsSection)
    , masterOpsPlaceholder_(kComboWidth_ * 2, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , policiesLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kPoliciesSection)
    , policiesPlaceholder_(kComboWidth_ * 2, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , defragLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kDefragSection)
    , defragPlaceholder_(kComboWidth_ * 2, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
    , loggingLabel_(kLabelWidth_, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kLoggingSection)
    , loggingPlaceholder_(kComboWidth_ * 2, kControlHeight_, tss::labelLookFromSkin(skin), PluginDisplayNames::Settings::kComingSoon)
{
    setOpaque(true);

    addAndMakeVisible(skinLabel_);
    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack,
                          static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream,
                          static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));
    skinComboBox_.setSelectedId(static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack), juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);

    addAndMakeVisible(uiScaleLabel_);
    uiScaleComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k50, PluginIDs::Settings::ScaleLevels::k50);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k75, PluginIDs::Settings::ScaleLevels::k75);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k100, PluginIDs::Settings::ScaleLevels::k100);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k125, PluginIDs::Settings::ScaleLevels::k125);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k150, PluginIDs::Settings::ScaleLevels::k150);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k175, PluginIDs::Settings::ScaleLevels::k175);
    uiScaleComboBox_.addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k200, PluginIDs::Settings::ScaleLevels::k200);
    uiScaleComboBox_.setSelectedId(PluginIDs::Settings::ScaleLevels::k100, juce::dontSendNotification);
    addAndMakeVisible(uiScaleComboBox_);

    addAndMakeVisible(hardwareLatencyLabel_);
    addAndMakeVisible(hardwareLatencySlider_);

    audioFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
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
    const int comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);
    const int sliderWidth = juce::roundToInt(static_cast<float>(kSliderWidth_) * uiScale_);
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    const int audioComboWidth = juce::roundToInt(static_cast<float>(kAudioFromComboWidth_) * uiScale_);
    const int peakWidth = juce::roundToInt(static_cast<float>(kPeakIndicatorWidth_) * uiScale_);

    auto layoutRow = [&](tss::Label& label, juce::Component& control, int controlWidth)
    {
        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        label.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        control.setBounds(row.removeFromLeft(controlWidth).withHeight(controlHeight));
        if (auto* slider = dynamic_cast<tss::Slider*>(&control))
            slider->setUiScale(uiScale_);
        else if (auto* combo = dynamic_cast<tss::ComboBox*>(&control))
            combo->setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    };

    auto layoutPlaceholderRow = [&](tss::Label& label, tss::Label& placeholder)
    {
        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        label.setUiScale(uiScale_);
        row.removeFromLeft(gap);
        placeholder.setBounds(row);
        placeholder.setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    };

    layoutRow(skinLabel_, skinComboBox_, comboWidth);
    layoutRow(uiScaleLabel_, uiScaleComboBox_, comboWidth);
    bounds.removeFromTop(rowGap);

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

void SettingsPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;

    const auto labelLook = tss::labelLookFromSkin(skin);
    skinLabel_.setLook(labelLook);
    uiScaleLabel_.setLook(labelLook);
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

    const auto comboLook = tss::comboBoxLookFromSkin(skin);
    const auto popupLook = tss::popupMenuLookFromSkin(skin);
    skinComboBox_.setLook(comboLook);
    skinComboBox_.setPopupMenuLook(popupLook);
    uiScaleComboBox_.setLook(comboLook);
    uiScaleComboBox_.setPopupMenuLook(popupLook);
    audioFromComboBox_.setLook(comboLook);
    audioFromComboBox_.setPopupMenuLook(popupLook);

    hardwareLatencySlider_.setLook(tss::sliderLookFromSkin(skin));
    inputGainSlider_.setLook(tss::sliderLookFromSkin(skin));
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

juce::String SettingsPanel::getSelectedPortIdentifier(const tss::ComboBox& combo,
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
