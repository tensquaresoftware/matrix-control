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
    , settingsButton_(kSettingsButtonWidth_, kControlHeight_, TSS::buttonLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kSettingsButton)
    , uiElementsButton_(kUiElementsButtonWidth_, kControlHeight_, TSS::buttonLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kUiElementsButton)
{
    setOpaque(true);

    logo_.setSkin(skin);
    logo_.onPopupRequested = [this] { showLogoPopup(); };
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

    addAndMakeVisible(settingsButton_);
    uiElementsButton_.setClickingTogglesState(true);
    addAndMakeVisible(uiElementsButton_);

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
    const float scaledHeight = static_cast<float>(height_) * sf;
    const float controlY = (scaledHeight - controlHeight) * 0.5f;

    const float editorMidiFromLabelWidth = static_cast<float>(kEditorMidiFromLabelWidth_) * sf;
    const float midiToLabelWidth = static_cast<float>(kMidiToLabelWidth_) * sf;
    const float keyboardFromLabelWidth = static_cast<float>(kKeyboardFromLabelWidth_) * sf;
    const float portComboWidth = static_cast<float>(kPortComboBoxWidth_) * sf;
    const float ledSize = static_cast<float>(kLedSize_) * sf;
    const float settingsButtonWidth = static_cast<float>(kSettingsButtonWidth_) * sf;
    const float uiElementsButtonWidth = static_cast<float>(kUiElementsButtonWidth_) * sf;
    const float leftPadding = static_cast<float>(kLeftPadding_) * sf;
    const float logoGapAfter = static_cast<float>(TSS::Design::Panels::Header::kLogoGapAfter) * sf;
    const float rightPadding = static_cast<float>(kRightPadding_) * sf;

    logo_.setUiScale(uiScale_);
    const int logoWidth = logo_.getPreferredWidth();
    const int logoHeight = bounds.getHeight();
    const int logoX = juce::roundToInt(static_cast<float>(bounds.getX()) + leftPadding);
    logo_.setBounds(logoX, bounds.getY(), logoWidth, logoHeight);

    float x = static_cast<float>(logoX + logoWidth) + logoGapAfter;
    const int y = juce::roundToInt(static_cast<float>(bounds.getY()) + controlY);
    const int h = juce::roundToInt(controlHeight);
    const int ledY = juce::roundToInt(static_cast<float>(bounds.getY()) + controlY
                                      + (controlHeight - ledSize) * 0.5f);
    const int ledH = juce::roundToInt(ledSize);

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

    auto endPacket = [&]()
    {
        x += packetExternalGap - gap;
    };

    placePacketLed(instrumentActivityLed_);
    placePacketLabel(keyboardFromLabel_, keyboardFromLabelWidth);
    placePacketCombo(keyboardFromComboBox_, portComboWidth);
    endPacket();

    placePacketLed(editorActivityLed_);
    placePacketLabel(midiFromLabel_, editorMidiFromLabelWidth);
    placePacketCombo(midiFromComboBox_, portComboWidth);
    endPacket();

    placePacketLed(midiToActivityLed_);
    placePacketLabel(midiToLabel_, midiToLabelWidth);
    placePacketCombo(midiToComboBox_, portComboWidth);
    endPacket();

    const float rightClusterGap = gap * 2.0f;
    const float settingsButtonX = static_cast<float>(bounds.getRight()) - rightPadding - settingsButtonWidth;
    const float uiElementsButtonX = settingsButtonX - rightClusterGap - uiElementsButtonWidth;

    uiElementsButton_.setBounds(juce::roundToInt(uiElementsButtonX), y, juce::roundToInt(uiElementsButtonWidth), h);
    uiElementsButton_.setUiScale(uiScale_);

    settingsButton_.setBounds(juce::roundToInt(settingsButtonX), y, juce::roundToInt(settingsButtonWidth), h);
    settingsButton_.setUiScale(uiScale_);
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
    settingsButton_.setLook(TSS::buttonLookFromSkin(skin));
    uiElementsButton_.setLook(TSS::buttonLookFromSkin(skin));
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

    if (isPluginMode_)
        configurePluginModeKeyboardFrom();
    else
        configureStandaloneKeyboardFrom();

    resized();
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

void HeaderPanel::populateInputPortCombo(TSS::ComboBox& combo, std::vector<juce::String>& identifiers)
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

void HeaderPanel::populateOutputPortCombo(TSS::ComboBox& combo, std::vector<juce::String>& identifiers)
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

juce::String HeaderPanel::getSelectedPortIdentifier(const TSS::ComboBox& combo,
                                                    const std::vector<juce::String>& identifiers) const
{
    return getPortIdentifierForItemId(identifiers, combo.getSelectedId());
}
