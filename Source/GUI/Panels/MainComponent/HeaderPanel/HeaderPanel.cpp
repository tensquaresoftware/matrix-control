#include "HeaderPanel.h"

#include <juce_audio_devices/juce_audio_devices.h>

#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HeaderLogoPopupMenu.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Looks/LookBuilders.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

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
                return static_cast<int>(i) + HeaderPanel::kFirstDeviceItemId;
        }

        return HeaderPanel::kPortSentinelItemId;
    }

    juce::String getPortIdentifierForItemId(const std::vector<juce::String>& identifiers, int itemId)
    {
        if (itemId < HeaderPanel::kFirstDeviceItemId)
            return {};

        const auto index = static_cast<size_t>(itemId - HeaderPanel::kFirstDeviceItemId);
        if (index >= identifiers.size())
            return {};

        return identifiers[index];
    }

    float inputGainNormalizedFill(double value)
    {
        return PluginAudioConstants::inputGainIndexToNormalizedFill(static_cast<int>(std::round(value)));
    }

    juce::String inputGainFormatValue(double value)
    {
        const int index = static_cast<int>(std::round(value));

        if (index <= PluginAudioConstants::kInputGainSilenceIndex)
            return "-" + juce::String::charToString(static_cast<juce::juce_wchar>(0x221E)) + " dB";

        const auto db = PluginAudioConstants::inputGainIndexToDb(index);
        return juce::String(static_cast<int>(std::round(db))) + " dB";
    }
}

HeaderPanel::HeaderPanel(TSS::ISkin& skin, int width, int height)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , midiFromLabel_(kEditorMidiFromLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kEditorMidiFromLabel)
    , midiFromComboBox_(kPortComboBoxWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , editorActivityLed_(kLedSize_, kLedSize_)
    , midiToLabel_(kMidiToLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kMidiToLabel)
    , midiToComboBox_(kPortComboBoxWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , midiToActivityLed_(kLedSize_, kLedSize_)
    , keyboardFromLabel_(kKeyboardFromLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kKeyboardFromLabel)
    , keyboardFromComboBox_(kPortComboBoxWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , instrumentActivityLed_(kLedSize_, kLedSize_)
    , audioFromLabel_(kAudioFromLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kAudioFromLabel)
    , audioFromComboBox_(kPortComboBoxWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , inputGainLabel_(kInputGainLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kInputGainLabel)
    , inputGainSlider_(kInputGainSliderWidth_, kControlHeight_, TSS::sliderLookFromSkin(skin),
                       TSS::SliderConfig{
                           static_cast<double>(PluginAudioConstants::kInputGainSilenceIndex),
                           static_cast<double>(PluginAudioConstants::kInputGainMaxIndex),
                           static_cast<double>(PluginAudioConstants::kInputGainDefaultIndex),
                           1.0,
                           {},
                           {},
                           inputGainNormalizedFill,
                           inputGainFormatValue})
    , peakIndicator_(kPeakIndicatorWidth_, kControlHeight_)
{
    setOpaque(true);

    logo_.setSkin(skin);
    logo_.onPopupRequested = [this] { showLogoPopup(); };
    logo_.onSettingsRequested = [this]
    {
        if (onSettingsRequested)
            onSettingsRequested();
    };
    logo_.onUiTestsToggleRequested = [this]
    {
        if (onUiTestsToggleRequested)
            onUiTestsToggleRequested();
    };
    logo_.onUiScaleReset = [this]
    {
        if (onUiScaleReset)
            onUiScaleReset();
    };
    addAndMakeVisible(logo_);
    addAndMakeVisible(midiFromLabel_);
    midiFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(midiFromComboBox_);
    editorActivityLed_.setSkin(skin);
    addAndMakeVisible(editorActivityLed_);

    addAndMakeVisible(midiToLabel_);
    midiToComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(midiToComboBox_);
    midiToActivityLed_.setSkin(skin);
    addAndMakeVisible(midiToActivityLed_);

    addAndMakeVisible(keyboardFromLabel_);
    keyboardFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(keyboardFromComboBox_);
    instrumentActivityLed_.setSkin(skin);
    addAndMakeVisible(instrumentActivityLed_);

    audioFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    addAndMakeVisible(audioFromLabel_);
    addAndMakeVisible(audioFromComboBox_);
    addAndMakeVisible(inputGainLabel_);
    addAndMakeVisible(inputGainSlider_);
    peakIndicator_.setSkin(skin);
    addAndMakeVisible(peakIndicator_);

    populateMidiPortLists();
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));

    const float systemDisplayScale = TSS::ScaledDrawing::systemDisplayScaleForComponent(*this);
    const float borderThickness = TSS::ScaledDrawing::snappedStrokeThicknessFromDesign(
        static_cast<float>(TSS::Design::Panels::kPanelEdgeBorderThickness),
        uiScale_,
        systemDisplayScale,
        TSS::ScaledDrawing::StrokeSnapPolicy::kRound);

    auto borderLine = getLocalBounds().toFloat().removeFromBottom(borderThickness);
    g.setColour(skin_->getColour(SkinColourId::kVerticalSeparatorLine));
    g.fillRect(borderLine);
}

void HeaderPanel::resized()
{
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;
    const float gap = static_cast<float>(kGap_) * sf;
    const float packetExternalGap = static_cast<float>(kPacketExternalGap_) * sf;
    const float controlHeight = static_cast<float>(kControlHeight_) * sf;
    const int panelHeight = bounds.getHeight();
    const int controlHeightPx = juce::roundToInt(controlHeight);
    const int controlY = bounds.getY() + (panelHeight - controlHeightPx) / 2;

    const float editorMidiFromLabelWidth = static_cast<float>(kEditorMidiFromLabelWidth_) * sf;
    const float midiToLabelWidth = static_cast<float>(kMidiToLabelWidth_) * sf;
    const float keyboardFromLabelWidth = static_cast<float>(kKeyboardFromLabelWidth_) * sf;
    const float audioFromLabelWidth = static_cast<float>(kAudioFromLabelWidth_) * sf;
    const float inputGainLabelWidth = static_cast<float>(kInputGainLabelWidth_) * sf;
    const float portComboWidth = static_cast<float>(kPortComboBoxWidth_) * sf;
    const float inputGainSliderWidth = static_cast<float>(kInputGainSliderWidth_) * sf;
    const float peakIndicatorWidth = static_cast<float>(kPeakIndicatorWidth_) * sf;
    const float ledSize = static_cast<float>(kLedSize_) * sf;
    const int ledSizePx = juce::roundToInt(ledSize);
    const int ledY = bounds.getY() + (panelHeight - ledSizePx) / 2;
    const float leftPadding = static_cast<float>(kLeftPadding_) * sf;
    const float logoGapAfter = static_cast<float>(TSS::Design::Panels::Header::kLogoGapAfter) * sf;

    logo_.setUiScale(uiScale_);
    const int logoWidth = logo_.getPreferredWidth();
    const int logoHeight = bounds.getHeight();
    const int logoX = juce::roundToInt(static_cast<float>(bounds.getX()) + leftPadding);
    logo_.setBounds(logoX, bounds.getY(), logoWidth, logoHeight);

    float x = static_cast<float>(logoX + logoWidth) + logoGapAfter;
    const int y = controlY;
    const int h = controlHeightPx;
    const int ledH = ledSizePx;

    auto placePacketLabel = [&](TSS::Label& label, float labelWidth)
    {
        label.setBounds(juce::roundToInt(x), y, juce::roundToInt(labelWidth), h);
        label.setUiScale(uiScale_);
        x += labelWidth + gap;
    };

    auto placePacketCombo = [&](TSS::ComboBox& combo, float comboWidth)
    {
        combo.setBounds(juce::roundToInt(x), y, juce::roundToInt(comboWidth), h);
        combo.setUiScale(uiScale_);
        x += comboWidth + gap;
    };

    auto placePacketLed = [&](TSS::Led& led)
    {
        led.setBounds(juce::roundToInt(x), ledY, juce::roundToInt(ledSize), ledH);
        led.setUiScale(uiScale_);
        x += ledSize + gap;
    };

    auto placePacketSlider = [&](TSS::Slider& slider, float sliderWidth)
    {
        slider.setBounds(juce::roundToInt(x), y, juce::roundToInt(sliderWidth), h);
        slider.setUiScale(uiScale_);
        x += sliderWidth + gap;
    };

    auto placePacketPeak = [&](TSS::PeakIndicator& peak)
    {
        peak.setBounds(juce::roundToInt(x), y, juce::roundToInt(peakIndicatorWidth), h);
        peak.setUiScale(uiScale_);
        x += peakIndicatorWidth + gap;
    };

    auto endPacket = [&]()
    {
        x += packetExternalGap - gap;
    };

    if (!isPluginMode_)
    {
        placePacketLed(instrumentActivityLed_);
        placePacketLabel(keyboardFromLabel_, keyboardFromLabelWidth);
        placePacketCombo(keyboardFromComboBox_, portComboWidth);
        endPacket();
    }

    placePacketLed(editorActivityLed_);
    placePacketLabel(midiFromLabel_, editorMidiFromLabelWidth);
    placePacketCombo(midiFromComboBox_, portComboWidth);
    endPacket();

    placePacketLed(midiToActivityLed_);
    placePacketLabel(midiToLabel_, midiToLabelWidth);
    placePacketCombo(midiToComboBox_, portComboWidth);
    endPacket();

    if (!isPluginMode_)
    {
        placePacketLabel(audioFromLabel_, audioFromLabelWidth);
        placePacketCombo(audioFromComboBox_, portComboWidth);
        endPacket();

        placePacketLabel(inputGainLabel_, inputGainLabelWidth);
        placePacketSlider(inputGainSlider_, inputGainSliderWidth);
        placePacketPeak(peakIndicator_);
        endPacket();
    }
}

void HeaderPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    logo_.setSkin(skin);
    midiFromLabel_.setLook(TSS::labelLookFromSkin(skin));
    midiFromComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    midiFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    midiToLabel_.setLook(TSS::labelLookFromSkin(skin));
    midiToComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    midiToComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    keyboardFromLabel_.setLook(TSS::labelLookFromSkin(skin));
    keyboardFromComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    keyboardFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    editorActivityLed_.setSkin(skin);
    midiToActivityLed_.setSkin(skin);
    instrumentActivityLed_.setSkin(skin);
    audioFromLabel_.setLook(TSS::labelLookFromSkin(skin));
    audioFromComboBox_.setLook(TSS::comboBoxLookFromSkin(skin));
    audioFromComboBox_.setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    inputGainLabel_.setLook(TSS::labelLookFromSkin(skin));
    inputGainSlider_.setLook(TSS::sliderLookFromSkin(skin));
    peakIndicator_.setSkin(skin);
}

void HeaderPanel::showLogoPopup()
{
    if (skin_ == nullptr)
        return;

    TSS::HeaderLogoPopupMenu::show(
        logo_,
        *skin_,
        uiScale_,
        currentSkinItemId_,
        currentUiScaleId_,
        [this](int skinItemId)
        {
            currentSkinItemId_ = skinItemId;
            if (onSkinSelected)
                onSkinSelected(skinItemId);
        },
        [this](int scaleId)
        {
            currentUiScaleId_ = scaleId;
            if (onUiScaleSelected)
                onUiScaleSelected(scaleId);
        },
        [this]
        {
            if (onSettingsRequested)
                onSettingsRequested();
        });
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
    updateKeyboardFromVisibility();
    updateAudioControlsVisibility();

    if (!isPluginMode_)
        configureStandaloneKeyboardFrom();

    resized();
}

void HeaderPanel::updateAudioControlsVisibility()
{
    const bool showAudioControls = !isPluginMode_;

    audioFromLabel_.setVisible(showAudioControls);
    audioFromComboBox_.setVisible(showAudioControls);
    inputGainLabel_.setVisible(showAudioControls);
    inputGainSlider_.setVisible(showAudioControls);
    peakIndicator_.setVisible(showAudioControls);
}

void HeaderPanel::updateKeyboardFromVisibility()
{
    const bool showKeyboardFrom = !isPluginMode_;

    instrumentActivityLed_.setVisible(showKeyboardFrom);
    keyboardFromLabel_.setVisible(showKeyboardFrom);
    keyboardFromComboBox_.setVisible(showKeyboardFrom);
}

void HeaderPanel::populateMidiPortLists()
{
    populateInputPortCombo(midiFromComboBox_, midiFromPortIdentifiers_);
    populateOutputPortCombo(midiToComboBox_, midiToPortIdentifiers_);

    if (!isPluginMode_)
        configureStandaloneKeyboardFrom();
}

void HeaderPanel::populateInputPortCombo(TSS::ComboBox& combo, std::vector<juce::String>& identifiers)
{
    const juce::String previousIdentifier = getPortIdentifierForItemId(identifiers, combo.getSelectedId());

    combo.clear(juce::dontSendNotification);
    identifiers.clear();

    combo.addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel, kPortSentinelItemId);

    const auto devices = juce::MidiInput::getAvailableDevices();
    for (int i = 0; i < devices.size(); ++i)
    {
        const auto& device = devices.getReference(i);
        const int itemId = i + kFirstDeviceItemId;
        combo.addItem(device.name, itemId);
        identifiers.push_back(device.identifier);
    }

    combo.setSelectedId(findItemIdForPortIdentifier(identifiers, previousIdentifier),
                        juce::dontSendNotification);
}

void HeaderPanel::populateOutputPortCombo(TSS::ComboBox& combo, std::vector<juce::String>& identifiers)
{
    const juce::String previousIdentifier = getPortIdentifierForItemId(identifiers, combo.getSelectedId());

    combo.clear(juce::dontSendNotification);
    identifiers.clear();

    combo.addItem(PluginDisplayNames::HeaderPanel::kNoOutputSentinel, kPortSentinelItemId);

    const auto devices = juce::MidiOutput::getAvailableDevices();
    for (int i = 0; i < devices.size(); ++i)
    {
        const auto& device = devices.getReference(i);
        const int itemId = i + kFirstDeviceItemId;
        combo.addItem(device.name, itemId);
        identifiers.push_back(device.identifier);
    }

    combo.setSelectedId(findItemIdForPortIdentifier(identifiers, previousIdentifier),
                        juce::dontSendNotification);
}

void HeaderPanel::configureStandaloneKeyboardFrom()
{
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

juce::String HeaderPanel::getSelectedPortIdentifier(const TSS::ComboBox& combo,
                                                    const std::vector<juce::String>& identifiers) const
{
    return getPortIdentifierForItemId(identifiers, combo.getSelectedId());
}

void HeaderPanel::populateAudioFromCombo(const juce::StringArray& channelNames,
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
        audioFromComboBox_.addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel, kPortSentinelItemId);
        audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        return;
    }

    selectAudioFromSourceId(previousSourceId);
}

juce::String HeaderPanel::getSelectedAudioFromSourceId() const
{
    const int itemId = audioFromComboBox_.getSelectedId();
    if (itemId < 1)
        return {};

    const auto index = static_cast<size_t>(itemId - 1);
    if (index >= audioFromSourceIdentifiers_.size())
        return {};

    return audioFromSourceIdentifiers_[index];
}

void HeaderPanel::selectAudioFromSourceId(const juce::String& sourceId)
{
    if (sourceId.isEmpty())
    {
        audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        return;
    }

    for (size_t i = 0; i < audioFromSourceIdentifiers_.size(); ++i)
    {
        if (audioFromSourceIdentifiers_[i] == sourceId)
        {
            audioFromComboBox_.setSelectedId(static_cast<int>(i + 1), juce::dontSendNotification);
            return;
        }
    }

    if (! audioFromSourceIdentifiers_.empty())
        audioFromComboBox_.setSelectedId(1, juce::dontSendNotification);
    else
        audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
}
