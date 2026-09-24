#include "HeaderPanel.h"

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

#include "GUI/Helpers/ComboBoxLiveRefresh.h"
#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Widgets/HeaderLogoPopupMenu.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Helpers/InputGainSliderText.h"
#include "Shared/Definitions/PluginAudioConstants.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using TSS::SkinColourId;

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

    [[nodiscard]] bool audioFromItemSetUnchanged(const TSS::ComboBox& combo,
                                                 const std::vector<juce::String>& currentIds,
                                                 const std::vector<juce::String>& nextIds,
                                                 const juce::StringArray& channelNames)
    {
        if (! TSS::ComboBoxLiveRefresh::identifiersEqual(currentIds, nextIds))
            return false;

        if (combo.getNumItems() != static_cast<int>(nextIds.size()) + 1)
            return false;

        const int count = channelNames.size();
        for (int i = 0; i < count; ++i)
        {
            if (combo.getItemText(i + 1) != channelNames[i].toUpperCase())
                return false;
        }

        return true;
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
                       TSS::makeInputGainSliderConfig())
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
    TSS::MidiPortComboPopulation::populateInputPortCombo(midiFromComboBox_, midiFromPortIdentifiers_);
    TSS::MidiPortComboPopulation::populateOutputPortCombo(midiToComboBox_, midiToPortIdentifiers_);

    if (isPluginMode_)
        configurePluginKeyboardFrom();
    else
        configureStandaloneKeyboardFrom();
}

void HeaderPanel::configureStandaloneKeyboardFrom()
{
    keyboardFromComboBox_.setEnabled(true);
    TSS::MidiPortComboPopulation::populateInputPortCombo(keyboardFromComboBox_,
                                                         keyboardFromPortIdentifiers_);
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
    return TSS::MidiPortComboPopulation::findItemIdForPortIdentifier(identifiers, deviceId);
}

juce::String HeaderPanel::getSelectedPortIdentifier(const TSS::ComboBox& combo,
                                                    const std::vector<juce::String>& identifiers) const
{
    return TSS::MidiPortComboPopulation::selectedPortId(combo, identifiers);
}

void HeaderPanel::populateAudioFromCombo(const juce::StringArray& channelNames,
                                         const juce::StringArray& channelIds)
{
    const auto previousSourceId = getSelectedAudioFromSourceId();
    const int count = juce::jmin(channelNames.size(), channelIds.size());

    std::vector<juce::String> nextIds;
    nextIds.reserve(static_cast<size_t>(count));
    for (int i = 0; i < count; ++i)
        nextIds.push_back(channelIds[i]);

    const bool itemSetUnchanged = audioFromItemSetUnchanged(
        audioFromComboBox_, audioFromSourceIdentifiers_, nextIds, channelNames);
    const auto action = TSS::ComboBoxLiveRefresh::planRefresh(
        audioFromComboBox_.isPopupOpen(), itemSetUnchanged);

    if (action == TSS::ComboBoxLiveRefresh::Action::kSkipRebuild)
    {
        if (count == 0)
            audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
        else
            selectAudioFromSourceId(previousSourceId);
        return;
    }

    const auto rebuild = [this, &channelNames, &nextIds, count, &previousSourceId]()
    {
        audioFromComboBox_.clear(juce::dontSendNotification);
        audioFromSourceIdentifiers_ = nextIds;

        audioFromComboBox_.addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel,
                                   kPortSentinelItemId);

        for (int i = 0; i < count; ++i)
        {
            const int itemId = i + kFirstDeviceItemId;
            audioFromComboBox_.addItem(channelNames[i].toUpperCase(), itemId);
        }

        if (count == 0)
        {
            audioFromComboBox_.setSelectedId(kPortSentinelItemId, juce::dontSendNotification);
            return;
        }

        selectAudioFromSourceId(previousSourceId);
    };

    if (action == TSS::ComboBoxLiveRefresh::Action::kDismissRebuildReopen)
        TSS::ComboBoxLiveRefresh::rebuildPreservingOpenPopup(audioFromComboBox_, rebuild);
    else
        rebuild();
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
