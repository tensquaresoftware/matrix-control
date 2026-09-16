#include "HeaderPanel.h"

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Widgets/HeaderLogoPopupMenu.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
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

namespace
{
    void dismissOpenHeaderLogoPopupMenus()
    {
        for (int i = juce::Component::getNumCurrentlyModalComponents(); --i >= 0;)
        {
            if (auto* menu = dynamic_cast<TSS::HeaderLogoPopupMenu*>(
                    juce::Component::getCurrentlyModalComponent(i)))
            {
                menu->detachContextualHelpBinder();
                menu->exitModalState(0);
            }
        }
    }
}

HeaderPanel::~HeaderPanel()
{
    dismissOpenHeaderLogoPopupMenus();
    contextualHelpBinder_.reset();
}

HeaderPanel::HeaderPanel(TSS::ISkin& skin, const HeaderPanelDimensions& dimensions)
    : dimensions_(dimensions)
    , skin_(&skin)
    , logo_(skin, dimensions.logoWidth, dimensions.logoHeight)
    , midiFromLabel_(dimensions.editorMidiFromLabelWidth, dimensions.controlHeight, TSS::darkPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kEditorMidiFromLabel)
    , midiFromComboBox_(dimensions.portComboBoxWidth, dimensions.controlHeight, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , editorActivityLed_(dimensions.ledSize, dimensions.ledSize)
    , midiToLabel_(dimensions.midiToLabelWidth, dimensions.controlHeight, TSS::darkPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kMidiToLabel)
    , midiToComboBox_(dimensions.portComboBoxWidth, dimensions.controlHeight, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , midiToActivityLed_(dimensions.ledSize, dimensions.ledSize)
    , keyboardFromLabel_(dimensions.keyboardFromLabelWidth, dimensions.controlHeight, TSS::darkPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kKeyboardFromLabel)
    , keyboardFromComboBox_(dimensions.portComboBoxWidth, dimensions.controlHeight, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , instrumentActivityLed_(dimensions.ledSize, dimensions.ledSize)
    , audioFromLabel_(dimensions.audioFromLabelWidth, dimensions.controlHeight, TSS::darkPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kAudioFromLabel)
    , audioFromComboBox_(dimensions.portComboBoxWidth, dimensions.controlHeight, TSS::comboBoxLookFromSkin(skin), TSS::ComboBox::Style::ButtonLike)
    , inputGainLabel_(dimensions.inputGainLabelWidth, dimensions.controlHeight, TSS::darkPanelLabelLookFromSkin(skin), PluginDisplayNames::HeaderPanel::kInputGainLabel)
    , inputGainSlider_(dimensions.inputGainSliderWidth, dimensions.controlHeight, TSS::sliderLookFromSkin(skin),
                       TSS::SliderConfig{
                           static_cast<double>(PluginAudioConstants::kInputGainSilenceIndex),
                           static_cast<double>(PluginAudioConstants::kInputGainMaxIndex),
                           static_cast<double>(PluginAudioConstants::kInputGainDefaultIndex),
                           1.0,
                           {},
                           {},
                           inputGainNormalizedFill,
                           inputGainFormatValue})
    , peakIndicator_(dimensions.peakIndicatorWidth, dimensions.controlHeight)
    , undoButton_(dimensions.undoButtonWidth,
                   dimensions.controlHeight,
                   TSS::buttonLookFromSkin(skin),
                   PluginDisplayNames::HeaderPanel::kUndo)
    , redoButton_(dimensions.redoButtonWidth,
                   dimensions.controlHeight,
                   TSS::buttonLookFromSkin(skin),
                   PluginDisplayNames::HeaderPanel::kRedo)
    , panicButton_(dimensions.panicButtonWidth,
                   dimensions.controlHeight,
                   TSS::buttonLookFromSkin(skin),
                   PluginDisplayNames::HeaderPanel::kPanic)
{
    setOpaque(true);
    wireLogoCallbacks();
    wireActionButtons();
    addChildControls(skin);
    populateMidiPortLists();
    syncPanicEnabledFromMidiToSelection();
    registerContextualHelp();
}

void HeaderPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::HeaderPanel::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    contextualHelpBinder_->bind(&midiFromComboBox_, Help::kMidiFrom);
    contextualHelpBinder_->bind(&midiToComboBox_, Help::kMidiTo);
    contextualHelpBinder_->bind(&keyboardFromComboBox_,
                                isPluginMode_ ? Help::kHost : Help::kKeyboardFrom);
    contextualHelpBinder_->bind(&audioFromComboBox_, Help::kAudioFrom);
    contextualHelpBinder_->bind(&inputGainSlider_, Help::kInputGain);
    contextualHelpBinder_->bind(&undoButton_, Help::kUndo);
    contextualHelpBinder_->bind(&redoButton_, Help::kRedo);
    contextualHelpBinder_->bind(&panicButton_, Help::kPanic);
    contextualHelpBinder_->bind(&logo_, Help::kLogo);
    contextualHelpBinder_->bind(&instrumentActivityLed_, Help::kKeyboardFromActivityLed);
    contextualHelpBinder_->bind(&editorActivityLed_, Help::kMidiFromActivityLed);
    contextualHelpBinder_->bind(&midiToActivityLed_, Help::kMidiToActivityLed);
    contextualHelpBinder_->bind(&peakIndicator_, Help::kAudioPeakIndicator);
}

void HeaderPanel::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kHeaderPanelBackground));
}

void HeaderPanel::showLogoPopup()
{
    if (skin_ == nullptr)
        return;

    TSS::HeaderLogoPopupMenu::Config config;
    config.uiScale = uiScale_;
    config.currentSkinItemId = currentSkinItemId_;
    config.currentUiScaleId = currentUiScaleId_;
    config.onSkinSelected = [this](int skinItemId)
    {
        currentSkinItemId_ = skinItemId;
        if (onSkinSelected)
            onSkinSelected(skinItemId);
    };
    config.onUiScaleSelected = [this](int scaleId)
    {
        currentUiScaleId_ = scaleId;
        if (onUiScaleSelected)
            onUiScaleSelected(scaleId);
    };
    config.onAudioMidiSettingsRequested = isPluginMode_ ? nullptr : onAudioMidiSettingsRequested;
    config.onSettingsRequested = [this]
    {
        if (onSettingsRequested)
            onSettingsRequested();
    };
    config.onAboutRequested = [this]
    {
        if (onAboutRequested)
            onAboutRequested();
    };
    config.contextualHelpBinder = contextualHelpBinder_.get();

    TSS::HeaderLogoPopupMenu::show(logo_, *skin_, std::move(config));
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

    if (isPluginMode_)
        configurePluginKeyboardFrom();
    else
        configureStandaloneKeyboardFrom();

    namespace Help = PluginDisplayNames::HeaderPanel::ContextualHelp;
    if (contextualHelpBinder_ != nullptr)
    {
        contextualHelpBinder_->bind(&keyboardFromComboBox_,
                                    isPluginMode_ ? Help::kHost : Help::kKeyboardFrom);
    }
    else
    {
        registerContextualHelp();
    }

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
    instrumentActivityLed_.setVisible(true);
    keyboardFromLabel_.setVisible(true);
    keyboardFromComboBox_.setVisible(true);
}

void HeaderPanel::populateMidiPortLists()
{
    populateInputPortCombo(midiFromComboBox_, midiFromPortIdentifiers_);
    populateOutputPortCombo(midiToComboBox_, midiToPortIdentifiers_);

    if (isPluginMode_)
        configurePluginKeyboardFrom();
    else
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
        combo.addItem(device.name.toUpperCase(), itemId);
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
        combo.addItem(device.name.toUpperCase(), itemId);
        identifiers.push_back(device.identifier);
    }

    combo.setSelectedId(findItemIdForPortIdentifier(identifiers, previousIdentifier),
                        juce::dontSendNotification);
}

void HeaderPanel::configureStandaloneKeyboardFrom()
{
    keyboardFromComboBox_.setEnabled(true);
    populateInputPortCombo(keyboardFromComboBox_, keyboardFromPortIdentifiers_);
}

void HeaderPanel::configurePluginKeyboardFrom()
{
    keyboardFromComboBox_.clear(juce::dontSendNotification);
    keyboardFromPortIdentifiers_.clear();
    keyboardFromComboBox_.addItem(PluginDisplayNames::HeaderPanel::kHostDisplay, kPluginHostItemId);
    keyboardFromComboBox_.setSelectedId(kPluginHostItemId, juce::dontSendNotification);
    keyboardFromComboBox_.setEnabled(false);
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
    syncPanicEnabledFromMidiToSelection();
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

    audioFromComboBox_.addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel, kPortSentinelItemId);

    const int count = juce::jmin(channelNames.size(), channelIds.size());

    for (int i = 0; i < count; ++i)
    {
        const int itemId = i + kFirstDeviceItemId;
        audioFromComboBox_.addItem(channelNames[i].toUpperCase(), itemId);
        audioFromSourceIdentifiers_.push_back(channelIds[i]);
    }

    if (count == 0)
    {
        audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        return;
    }

    selectAudioFromSourceId(previousSourceId);
}

juce::String HeaderPanel::getSelectedAudioFromSourceId() const
{
    const int itemId = audioFromComboBox_.getSelectedId();
    if (itemId < kFirstDeviceItemId)
        return {};

    const auto index = static_cast<size_t>(itemId - kFirstDeviceItemId);
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
            audioFromComboBox_.setSelectedId(static_cast<int>(i) + kFirstDeviceItemId,
                                             juce::dontSendNotification);
            return;
        }
    }

    // Missing id: do not fall back to catalog[0] (can flip mono/stereo kind).
    // Keep sentinel selected so the UI does not pretend a different source is active.
    audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
}
