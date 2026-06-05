#include "HeaderPanel.h"

#include <juce_audio_devices/juce_audio_devices.h>

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using tss::SkinColourId;

namespace
{
    int findItemIdForPortIdentifier(const std::vector<juce::String>& identifiers,
                                    const juce::String& deviceId)
    {
        if (deviceId.isEmpty())
            return HeaderPanel::kPortSentinelItemId;

        for (size_t i = 0; i < identifiers.size(); ++i)
        {
            if (identifiers[i] == deviceId)
                return static_cast<int>(i + 1);
        }

        return HeaderPanel::kPortSentinelItemId;
    }

    juce::String getPortIdentifierForItemId(const std::vector<juce::String>& identifiers, int itemId)
    {
        if (itemId <= HeaderPanel::kPortSentinelItemId)
            return {};

        const auto index = static_cast<size_t>(itemId - 1);
        if (index >= identifiers.size())
            return {};

        return identifiers[index];
    }
}

HeaderPanel::HeaderPanel(tss::ISkin& skin, int width, int height)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , midiFromLabel_(kEditorMidiFromLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kEditorMidiFromLabel, tss::LabelStyle::HeaderPanel)
    , midiFromComboBox_(kPortComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , editorActivityLed_(kActivityLedSize_, kActivityLedSize_)
    , midiToLabel_(kMidiToLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kMidiToLabel, tss::LabelStyle::HeaderPanel)
    , midiToComboBox_(kPortComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , midiToActivityLed_(kActivityLedSize_, kActivityLedSize_)
    , keyboardFromLabel_(kKeyboardFromLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kKeyboardFromLabel, tss::LabelStyle::HeaderPanel)
    , keyboardFromComboBox_(kPortComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , instrumentActivityLed_(kActivityLedSize_, kActivityLedSize_)
    , audioFromLabel_(kAudioFromLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kAudioFromLabel, tss::LabelStyle::HeaderPanel)
    , audioFromComboBox_(kPortComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , inputGainLabel_(kInputGainLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kInputGainLabel, tss::LabelStyle::HeaderPanel)
    , inputGainSlider_(kInputGainSliderWidth_, kControlHeight_, tss::sliderLookFromSkin(skin), 0.0)
    , peakIndicator_(kPeakIndicatorWidth_, kControlHeight_)
    , skinLabel_(kSkinLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kSkinLabel, tss::LabelStyle::HeaderPanel)
    , skinComboBox_(kSkinComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , uiScaleLabel_(kUiScaleLabelWidth_, kControlHeight_, tss::headerPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kUiScaleLabel, tss::LabelStyle::HeaderPanel)
    , uiScaleComboBox_(kScaleComboBoxWidth_, kControlHeight_, tss::comboBoxLookFromSkin(skin), tss::ComboBox::Style::ButtonLike)
    , uiElementsButton_(kUiElementsButtonWidth_, kControlHeight_, tss::buttonLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kUiElementsButton)
{
    setOpaque(true);

    addAndMakeVisible(midiFromLabel_);
    midiFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    addAndMakeVisible(midiFromComboBox_);
    editorActivityLed_.setSkin(skin);
    addAndMakeVisible(editorActivityLed_);

    addAndMakeVisible(midiToLabel_);
    midiToComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    addAndMakeVisible(midiToComboBox_);
    midiToActivityLed_.setSkin(skin);
    addAndMakeVisible(midiToActivityLed_);

    addAndMakeVisible(keyboardFromLabel_);
    keyboardFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    addAndMakeVisible(keyboardFromComboBox_);
    instrumentActivityLed_.setSkin(skin);
    addAndMakeVisible(instrumentActivityLed_);

    addAndMakeVisible(audioFromLabel_);
    audioFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    addAndMakeVisible(audioFromComboBox_);

    addAndMakeVisible(inputGainLabel_);
    inputGainSlider_.setRange(PluginAudioConstants::kMinInputGainDb,
                              PluginAudioConstants::kMaxInputGainDb,
                              0.1);
    inputGainSlider_.setUnit("dB");
    addAndMakeVisible(inputGainSlider_);

    peakIndicator_.setSkin(skin);
    addAndMakeVisible(peakIndicator_);

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

    addAndMakeVisible(skinLabel_);

    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack,
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack));
    skinComboBox_.addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream,
                         static_cast<int>(tss::Skin::SkinComboBoxItemId::kCream));
    skinComboBox_.setSelectedId(static_cast<int>(tss::Skin::SkinComboBoxItemId::kBlack), juce::dontSendNotification);
    addAndMakeVisible(skinComboBox_);

    uiElementsButton_.setClickingTogglesState(true);
    addAndMakeVisible(uiElementsButton_);

    populateMidiPortLists();
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void HeaderPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;
    const float gap = static_cast<float>(kGap_) * sf;
    const float packetExternalGap = static_cast<float>(kPacketExternalGap_) * sf;
    const float controlHeight = static_cast<float>(kControlHeight_) * sf;
    const float scaledHeight = static_cast<float>(height_) * sf;
    const float controlY = (scaledHeight - controlHeight) * 0.5f;

    const float editorMidiFromLabelWidth = static_cast<float>(kEditorMidiFromLabelWidth_) * sf;
    const float midiToLabelWidth = static_cast<float>(kMidiToLabelWidth_) * sf;
    const float keyboardFromLabelWidth = static_cast<float>(kKeyboardFromLabelWidth_) * sf;
    const float audioFromLabelWidth = static_cast<float>(kAudioFromLabelWidth_) * sf;
    const float inputGainLabelWidth = static_cast<float>(kInputGainLabelWidth_) * sf;
    const float portComboWidth = static_cast<float>(kPortComboBoxWidth_) * sf;
    const float inputGainSliderWidth = static_cast<float>(kInputGainSliderWidth_) * sf;
    const float peakIndicatorWidth = static_cast<float>(kPeakIndicatorWidth_) * sf;
    const float activityLedSize = static_cast<float>(kActivityLedSize_) * sf;
    const float uiScaleLabelWidth = static_cast<float>(kUiScaleLabelWidth_) * sf;
    const float scaleComboWidth = static_cast<float>(kScaleComboBoxWidth_) * sf;
    const float skinLabelWidth = static_cast<float>(kSkinLabelWidth_) * sf;
    const float skinComboWidth = static_cast<float>(kSkinComboBoxWidth_) * sf;
    const float uiElementsButtonWidth = static_cast<float>(kUiElementsButtonWidth_) * sf;
    const float leftPadding = static_cast<float>(kLeftPadding_) * sf;
    const float rightPadding = static_cast<float>(kRightPadding_) * sf;

    float x = static_cast<float>(bounds.getX()) + leftPadding;
    const int y = juce::roundToInt(static_cast<float>(bounds.getY()) + controlY);
    const int h = juce::roundToInt(controlHeight);
    const int activityLedY = juce::roundToInt(static_cast<float>(bounds.getY()) + controlY
                                              + (controlHeight - activityLedSize) * 0.5f);
    const int activityLedH = juce::roundToInt(activityLedSize);

    auto placePacketLabel = [&](tss::Label& label, float labelWidth)
    {
        label.setBounds(juce::roundToInt(x), y, juce::roundToInt(labelWidth), h);
        label.setUiScale(uiScale_);
        x += labelWidth + gap;
    };

    auto placePacketCombo = [&](tss::ComboBox& combo, float comboWidth)
    {
        combo.setBounds(juce::roundToInt(x), y, juce::roundToInt(comboWidth), h);
        combo.setUiScale(uiScale_);
        x += comboWidth + gap;
    };

    auto placePacketActivityLed = [&](tss::ActivityLed& led)
    {
        led.setBounds(juce::roundToInt(x), activityLedY, juce::roundToInt(activityLedSize), activityLedH);
        led.setUiScale(uiScale_);
        x += activityLedSize + gap;
    };

    auto placePacketSlider = [&](tss::Slider& slider, float sliderWidth)
    {
        slider.setBounds(juce::roundToInt(x), y, juce::roundToInt(sliderWidth), h);
        slider.setUiScale(uiScale_);
        x += sliderWidth + gap;
    };

    auto placePacketPeak = [&](tss::PeakIndicator& peak)
    {
        peak.setBounds(juce::roundToInt(x), y, juce::roundToInt(peakIndicatorWidth), h);
        peak.setUiScale(uiScale_);
        x += peakIndicatorWidth + gap;
    };

    auto endPacket = [&]()
    {
        x += packetExternalGap - gap;
    };

    placePacketActivityLed(instrumentActivityLed_);
    placePacketLabel(keyboardFromLabel_, keyboardFromLabelWidth);
    placePacketCombo(keyboardFromComboBox_, portComboWidth);
    endPacket();

    placePacketActivityLed(editorActivityLed_);
    placePacketLabel(midiFromLabel_, editorMidiFromLabelWidth);
    placePacketCombo(midiFromComboBox_, portComboWidth);
    endPacket();

    placePacketActivityLed(midiToActivityLed_);
    placePacketLabel(midiToLabel_, midiToLabelWidth);
    placePacketCombo(midiToComboBox_, portComboWidth);
    endPacket();

    placePacketLabel(audioFromLabel_, audioFromLabelWidth);
    placePacketCombo(audioFromComboBox_, portComboWidth);
    endPacket();

    placePacketLabel(inputGainLabel_, inputGainLabelWidth);
    placePacketSlider(inputGainSlider_, inputGainSliderWidth);
    placePacketPeak(peakIndicator_);
    endPacket();

    auto placeRightClusterLabelAndCombo = [&](tss::Label& label, float labelWidth, tss::ComboBox& combo, float comboWidth)
    {
        label.setBounds(juce::roundToInt(x), y, juce::roundToInt(labelWidth), h);
        label.setUiScale(uiScale_);
        x += labelWidth + gap;

        combo.setBounds(juce::roundToInt(x), y, juce::roundToInt(comboWidth), h);
        combo.setUiScale(uiScale_);
        x += comboWidth + packetExternalGap;
    };

    placeRightClusterLabelAndCombo(uiScaleLabel_, uiScaleLabelWidth, uiScaleComboBox_, scaleComboWidth);
    placeRightClusterLabelAndCombo(skinLabel_, skinLabelWidth, skinComboBox_, skinComboWidth);

    const float uiElementsButtonX = static_cast<float>(bounds.getRight()) - rightPadding - uiElementsButtonWidth;
    uiElementsButton_.setBounds(juce::roundToInt(uiElementsButtonX), y, juce::roundToInt(uiElementsButtonWidth), h);
    uiElementsButton_.setUiScale(uiScale_);
}

void HeaderPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    midiFromLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    midiFromComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    midiFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    midiToLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    midiToComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    midiToComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    keyboardFromLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    keyboardFromComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    keyboardFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    audioFromLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    audioFromComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    audioFromComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    inputGainLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    inputGainSlider_.setLook(tss::sliderLookFromSkin(skin));
    peakIndicator_.setSkin(skin);
    editorActivityLed_.setSkin(skin);
    midiToActivityLed_.setSkin(skin);
    instrumentActivityLed_.setSkin(skin);
    skinLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    skinComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    skinComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    uiScaleLabel_.setLook(tss::headerPanelLabelLookFromSkin(skin));
    uiScaleComboBox_.setLook(tss::comboBoxLookFromSkin(skin));
    uiScaleComboBox_.setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    uiElementsButton_.setLook(tss::buttonLookFromSkin(skin));
}

void HeaderPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

void HeaderPanel::setPluginMode(bool isPlugin)
{
    isPluginMode_ = isPlugin;

    if (isPluginMode_)
    {
        configurePluginModeKeyboardFrom();
    }
    else
    {
        configureStandaloneKeyboardFrom();
    }
}

void HeaderPanel::populateMidiPortLists()
{
    populateInputPortCombo(midiFromComboBox_, midiFromPortIdentifiers_);
    populateOutputPortCombo(midiToComboBox_, midiToPortIdentifiers_);

    if (isPluginMode_)
        configurePluginModeKeyboardFrom();
    else
        configureStandaloneKeyboardFrom();
}

void HeaderPanel::populateInputPortCombo(tss::ComboBox& combo, std::vector<juce::String>& identifiers)
{
    const juce::String previousIdentifier = getPortIdentifierForItemId(identifiers, combo.getSelectedId());

    combo.clear(juce::dontSendNotification);
    identifiers.clear();

    combo.addItem(PluginDisplayNames::HeaderPanel::kPortNoneSentinel, kPortSentinelItemId);

    const auto devices = juce::MidiInput::getAvailableDevices();
    for (int i = 0; i < devices.size(); ++i)
    {
        const auto& device = devices.getReference(i);
        const int itemId = i + 1;
        combo.addItem(device.name, itemId);
        identifiers.push_back(device.identifier);
    }

    combo.setSelectedId(findItemIdForPortIdentifier(identifiers, previousIdentifier),
                        juce::dontSendNotification);
}

void HeaderPanel::populateOutputPortCombo(tss::ComboBox& combo, std::vector<juce::String>& identifiers)
{
    const juce::String previousIdentifier = getPortIdentifierForItemId(identifiers, combo.getSelectedId());

    combo.clear(juce::dontSendNotification);
    identifiers.clear();

    combo.addItem(PluginDisplayNames::HeaderPanel::kPortNoneSentinel, kPortSentinelItemId);

    const auto devices = juce::MidiOutput::getAvailableDevices();
    for (int i = 0; i < devices.size(); ++i)
    {
        const auto& device = devices.getReference(i);
        const int itemId = i + 1;
        combo.addItem(device.name, itemId);
        identifiers.push_back(device.identifier);
    }

    combo.setSelectedId(findItemIdForPortIdentifier(identifiers, previousIdentifier),
                        juce::dontSendNotification);
}

void HeaderPanel::configurePluginModeKeyboardFrom()
{
    keyboardFromComboBox_.clear(juce::dontSendNotification);
    keyboardFromComboBox_.addItem(PluginDisplayNames::HeaderPanel::kHostDisplay, 1);
    keyboardFromComboBox_.setSelectedId(1, juce::dontSendNotification);
    keyboardFromComboBox_.setEnabled(false);
}

void HeaderPanel::configureStandaloneKeyboardFrom()
{
    keyboardFromComboBox_.setEnabled(true);
    populateInputPortCombo(keyboardFromComboBox_, keyboardFromPortIdentifiers_);
}

juce::String HeaderPanel::getSelectedMidiFromPortIdentifier() const
{
    return getSelectedPortIdentifier(midiFromComboBox_, midiFromPortIdentifiers_);
}

juce::String HeaderPanel::getSelectedMidiToPortIdentifier() const
{
    return getSelectedPortIdentifier(midiToComboBox_, midiToPortIdentifiers_);
}

juce::String HeaderPanel::getSelectedKeyboardFromPortIdentifier() const
{
    if (isPluginMode_)
        return {};

    return getSelectedPortIdentifier(keyboardFromComboBox_, keyboardFromPortIdentifiers_);
}

void HeaderPanel::selectMidiFromPort(const juce::String& deviceId)
{
    midiFromComboBox_.setSelectedId(findItemIdForIdentifier(midiFromPortIdentifiers_, deviceId),
                                    juce::dontSendNotification);
}

void HeaderPanel::selectMidiToPort(const juce::String& deviceId)
{
    midiToComboBox_.setSelectedId(findItemIdForIdentifier(midiToPortIdentifiers_, deviceId),
                                  juce::dontSendNotification);
}

void HeaderPanel::selectKeyboardFromPort(const juce::String& deviceId)
{
    if (isPluginMode_)
        return;

    keyboardFromComboBox_.setSelectedId(findItemIdForIdentifier(keyboardFromPortIdentifiers_, deviceId),
                                        juce::dontSendNotification);
}

int HeaderPanel::findItemIdForIdentifier(const std::vector<juce::String>& identifiers,
                                         const juce::String& deviceId) const
{
    return findItemIdForPortIdentifier(identifiers, deviceId);
}

juce::String HeaderPanel::getSelectedPortIdentifier(const tss::ComboBox& combo,
                                                    const std::vector<juce::String>& identifiers) const
{
    return getPortIdentifierForItemId(identifiers, combo.getSelectedId());
}

void HeaderPanel::populateAudioFromCombo(const juce::StringArray& channelNames,
                                          const juce::StringArray& channelIds)
{
    configureAudioFromCombo(channelNames, channelIds);
}

void HeaderPanel::configureAudioFromCombo(const juce::StringArray& channelNames,
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

juce::String HeaderPanel::getSelectedAudioFromSourceId() const
{
    return getSelectedPortIdentifier(audioFromComboBox_, audioFromSourceIdentifiers_);
}

void HeaderPanel::selectAudioFromSourceId(const juce::String& sourceId)
{
    audioFromComboBox_.setSelectedId(findItemIdForIdentifier(audioFromSourceIdentifiers_, sourceId),
                                     juce::dontSendNotification);
}
