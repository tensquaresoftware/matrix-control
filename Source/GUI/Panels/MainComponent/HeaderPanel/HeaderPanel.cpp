#include "HeaderPanel.h"

#include <memory>

#include <juce_audio_devices/juce_audio_devices.h>

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Layout/ScaledLayout.h"
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
    paintAudioCartouche(g);
}

void HeaderPanel::paintAudioCartouche(juce::Graphics& g)
{
    if (isPluginMode_ || audioCartoucheFrameBounds_.isEmpty())
        return;

    // Light cartouche chrome on dark header (same as DarkPanel text).
    const auto cartoucheChrome = skin_->getColour(SkinColourId::kDarkPanelText);
    const auto headerBg = skin_->getColour(SkinColourId::kHeaderPanelBackground);
    const int stroke = juce::jmax(1, audioCartoucheStrokePx_);

    g.setColour(cartoucheChrome);
    g.fillRect(audioCartoucheBadgeBounds_);

    auto look = TSS::darkPanelLabelLookFromSkin(*skin_);
    look.text = headerBg;
    g.setColour(look.text);
    g.setFont(look.font.withHeight(look.font.getHeight() * uiScale_).boldened());
    g.drawText(PluginDisplayNames::HeaderPanel::kAudioCartoucheLabel,
               audioCartoucheBadgeBounds_,
               juce::Justification::centred,
               false);

    g.setColour(cartoucheChrome);
    g.fillRect(audioCartoucheFrameBounds_.getX(),
               audioCartoucheFrameBounds_.getY(),
               audioCartoucheFrameBounds_.getWidth(),
               stroke);
    g.fillRect(audioCartoucheFrameBounds_.getX(),
               audioCartoucheFrameBounds_.getBottom() - stroke,
               audioCartoucheFrameBounds_.getWidth(),
               stroke);
    g.fillRect(audioCartoucheFrameBounds_.getRight() - stroke,
               audioCartoucheFrameBounds_.getY(),
               stroke,
               audioCartoucheFrameBounds_.getHeight());
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
    populateMidiPortLists({}, {}, {});
}

void HeaderPanel::populateMidiPortLists(const juce::String& keepOpenInputId,
                                        const juce::String& keepOpenOutputId,
                                        const juce::String& keepOpenKeyboardFromId)
{
    TSS::MidiPortComboPopulation::populateInputPortCombo(
        midiFromComboBox_, midiFromPortIdentifiers_, keepOpenInputId);
    TSS::MidiPortComboPopulation::populateOutputPortCombo(
        midiToComboBox_, midiToPortIdentifiers_, keepOpenOutputId);

    if (isPluginMode_)
        configurePluginKeyboardFrom();
    else
    {
        keyboardFromComboBox_.setEnabled(true);
        TSS::MidiPortComboPopulation::populateInputPortCombo(
            keyboardFromComboBox_, keyboardFromPortIdentifiers_, keepOpenKeyboardFromId);
    }
}

void HeaderPanel::configureStandaloneKeyboardFrom()
{
    keyboardFromComboBox_.setEnabled(true);
    TSS::MidiPortComboPopulation::populateInputPortCombo(keyboardFromComboBox_,
                                                         keyboardFromPortIdentifiers_);
}

void HeaderPanel::configurePluginKeyboardFrom()
{
    keyboardFromComboBox_.setUsesPortSentinelPopupChrome(false);
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
