#include "EpromTypePromptDialog.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

EpromTypePromptDialog::EpromTypePromptDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
    , confirmButton_(PluginDisplayNames::Dialogs::EpromTypePrompt::kConfirm)
    , specifyLaterButton_(PluginDisplayNames::Dialogs::EpromTypePrompt::kSpecifyLater)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    midiFromLabel_ = std::make_unique<TSS::Label>(
        kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin),
        PluginDisplayNames::Dialogs::EpromTypePrompt::kMidiFromLabel);
    midiFromCombo_ = std::make_unique<TSS::ComboBox>(
        kComboWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin));
    midiFromCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));

    midiToLabel_ = std::make_unique<TSS::Label>(
        kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin),
        PluginDisplayNames::Dialogs::EpromTypePrompt::kMidiToLabel);
    midiToCombo_ = std::make_unique<TSS::ComboBox>(
        kComboWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin));
    midiToCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));

    epromTypeLabel_ = std::make_unique<TSS::Label>(
        kLabelWidth_, kControlHeight_, TSS::labelLookFromSkin(skin),
        PluginDisplayNames::Dialogs::EpromTypePrompt::kEpromTypeLabel);
    epromTypeCombo_ = std::make_unique<TSS::ComboBox>(
        kComboWidth_, kControlHeight_, TSS::comboBoxLookFromSkin(skin));
    epromTypeCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));

    confirmButton_.onClick = [this] { confirm(); };
    specifyLaterButton_.onClick = [this] { dismissAsLater(); };
    specifyLaterButton_.setWantsKeyboardFocus(false);
    specifyLaterButton_.setMouseClickGrabsKeyboardFocus(false);
    confirmButton_.setMouseClickGrabsKeyboardFocus(false);

    wireMidiComboCallbacks();

    epromTypeCombo_->onChange = [this]
    {
        if (! suppressMidiCallbacks_)
            epromComboTouchedByUser_ = true;
    };

    addAndMakeVisible(*midiFromLabel_);
    addAndMakeVisible(*midiFromCombo_);
    addAndMakeVisible(*midiToLabel_);
    addAndMakeVisible(*midiToCombo_);
    addAndMakeVisible(*epromTypeLabel_);
    addAndMakeVisible(*epromTypeCombo_);
    addAndMakeVisible(confirmButton_);
    addAndMakeVisible(specifyLaterButton_);
}

EpromTypePromptDialog::~EpromTypePromptDialog()
{
    stopTimer();
}

void EpromTypePromptDialog::wireMidiComboCallbacks()
{
    midiFromCombo_->onChange = [this]
    {
        if (suppressMidiCallbacks_ || onMidiFromChanged_ == nullptr)
            return;

        onMidiFromChanged_(TSS::MidiPortComboPopulation::selectedPortId(
            *midiFromCombo_, midiFromPortIdentifiers_));
        recomputeDeviceRow();
    };

    midiToCombo_->onChange = [this]
    {
        if (suppressMidiCallbacks_ || onMidiToChanged_ == nullptr)
            return;

        onMidiToChanged_(TSS::MidiPortComboPopulation::selectedPortId(
            *midiToCombo_, midiToPortIdentifiers_));
        recomputeDeviceRow();
    };
}

void EpromTypePromptDialog::prepareForShow(PrepareForShowArgs args)
{
    onConfirm_ = std::move(args.onConfirm);
    onLater_ = std::move(args.onLater);
    onMidiFromChanged_ = std::move(args.onMidiFromChanged);
    onMidiToChanged_ = std::move(args.onMidiToChanged);
    onSearchingWindowStarted_ = std::move(args.onSearchingWindowStarted);
    includeFirmwareSuggestionHint_ = args.includeFirmwareSuggestionHint;
    liveStatus_ = args.deviceStatus;
    searchingWindow_ = {};
    searchingDotFrame_ = 0;
    epromComboTouchedByUser_ = false;
    stopTimer();

    syncPortsFromHost(args.midiFromPortId, args.midiToPortId, true);
    populateComboItems(args.deviceType, args.preferredSelectedId);
    recomputeDeviceRow();
    resized();
    repaint();
}

void EpromTypePromptDialog::updateLiveDeviceStatus(const LiveDeviceStatus& status)
{
    liveStatus_ = status;
    includeFirmwareSuggestionHint_ = status.deviceDetected
        && status.deviceVersion.trim().isNotEmpty();
    recomputeDeviceRow();
}

void EpromTypePromptDialog::refreshEpromSuggestion(MatrixDeviceTypes::Type deviceType,
                                                   int preferredSelectedId)
{
    const int currentId = epromTypeCombo_->getSelectedId();
    populateComboItems(deviceType,
                       Core::nextDeviceSetupEpromPreferredId(
                           epromComboTouchedByUser_, currentId, preferredSelectedId));
}

void EpromTypePromptDialog::syncPortsFromHost(const juce::String& midiFromPortId,
                                              const juce::String& midiToPortId,
                                              bool repopulateLists)
{
    const juce::ScopedValueSetter<bool> guard(suppressMidiCallbacks_, true);
    if (repopulateLists)
        populateMidiPortLists();

    TSS::MidiPortComboPopulation::selectPortInCombo(
        *midiFromCombo_, midiFromPortIdentifiers_, midiFromPortId);
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *midiToCombo_, midiToPortIdentifiers_, midiToPortId);
    recomputeDeviceRow();
}

void EpromTypePromptDialog::populateComboItems(MatrixDeviceTypes::Type deviceType,
                                               int preferredSelectedId)
{
    const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
    const int selectedId = Core::EpromTypePolicy::coerceForDeviceFamily(preferredSelectedId, family);

    const juce::ScopedValueSetter<bool> guard(suppressMidiCallbacks_, true);
    epromTypeCombo_->clear(juce::dontSendNotification);
    Core::EpromTypePolicy::forEachValidItem(family, [this](int id)
    {
        epromTypeCombo_->addItem(Core::EpromTypePolicy::displayNameForId(id), id);
    });
    epromTypeCombo_->setSelectedId(selectedId, juce::dontSendNotification);
}

void EpromTypePromptDialog::populateMidiPortLists()
{
    TSS::MidiPortComboPopulation::populateInputPortCombo(*midiFromCombo_, midiFromPortIdentifiers_);
    TSS::MidiPortComboPopulation::populateOutputPortCombo(*midiToCombo_, midiToPortIdentifiers_);
}

void EpromTypePromptDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    midiFromLabel_->setLook(TSS::labelLookFromSkin(skin));
    midiFromCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    midiFromCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    midiToLabel_->setLook(TSS::labelLookFromSkin(skin));
    midiToCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    midiToCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    epromTypeLabel_->setLook(TSS::labelLookFromSkin(skin));
    epromTypeCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    epromTypeCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    repaint();
}

void EpromTypePromptDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

juce::String EpromTypePromptDialog::bodyText() const
{
    juce::String text(PluginDisplayNames::Dialogs::EpromTypePrompt::kBody);
    if (includeFirmwareSuggestionHint_)
        text += PluginDisplayNames::Dialogs::EpromTypePrompt::kBodySuggestionSuffix;
    return text;
}

void EpromTypePromptDialog::dismissAsLater()
{
    stopTimer();

    if (onLater_)
        onLater_();

    if (onDismissRequested_)
        onDismissRequested_();
}

void EpromTypePromptDialog::confirm()
{
    stopTimer();

    const int selectedId = epromTypeCombo_->getSelectedId();
    if (onConfirm_)
        onConfirm_(selectedId > 0 ? selectedId : PluginIDs::Settings::EpromType::kDefault);

    if (onDismissRequested_)
        onDismissRequested_();
}

void EpromTypePromptDialog::applySearchingWindowUpdate(const Core::DeviceSetupSearchingWindowUpdate& update)
{
    searchingWindow_ = update.state;
    if (update.shouldKickInquiry && onSearchingWindowStarted_ != nullptr)
        onSearchingWindowStarted_();
}

void EpromTypePromptDialog::recomputeDeviceRow()
{
    using namespace TSS::MidiPortComboPopulation;

    const auto midiFromId = selectedPortId(*midiFromCombo_, midiFromPortIdentifiers_);
    const auto midiToId = selectedPortId(*midiToCombo_, midiToPortIdentifiers_);
    const bool identityOk = Core::isDeviceSetupIdentityOk(liveStatus_.deviceDetected,
                                                          liveStatus_.deviceMidiUnresponsive,
                                                          liveStatus_.deviceType);

    applySearchingWindowUpdate(Core::advanceDeviceSetupSearchingWindow(
        searchingWindow_,
        {
            .midiFromId = midiFromId,
            .midiToId = midiToId,
            .identityOk = identityOk,
            .deviceMidiUnresponsive = liveStatus_.deviceMidiUnresponsive,
            .nowMs = juce::Time::getMillisecondCounter(),
        }));

    const auto versionDisplay = TSS::formatDeviceVersionForDisplay(liveStatus_.deviceVersion);
    deviceRowView_ = Core::resolveDeviceSetupDeviceRow({
        .midiFromReady = midiFromId.isNotEmpty(),
        .midiToReady = midiToId.isNotEmpty(),
        .deviceDetected = liveStatus_.deviceDetected,
        .deviceMidiUnresponsive = liveStatus_.deviceMidiUnresponsive,
        .searchingWindowActive = searchingWindow_.active && ! searchingWindow_.exhausted,
        .deviceType = liveStatus_.deviceType,
    }, versionDisplay);

    syncAnimationTimer();
    repaint();
}

void EpromTypePromptDialog::syncAnimationTimer()
{
    if (deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kSearching)
    {
        if (! isTimerRunning())
            startTimer(kSearchingDotsIntervalMs_);
        return;
    }

    stopTimer();
    searchingDotFrame_ = 0;
}

void EpromTypePromptDialog::timerCallback()
{
    if (searchingWindow_.active)
    {
        const auto update = Core::advanceDeviceSetupSearchingWindow(
            searchingWindow_,
            {
                .midiFromId = searchingWindow_.trackedFromId,
                .midiToId = searchingWindow_.trackedToId,
                .identityOk = Core::isDeviceSetupIdentityOk(liveStatus_.deviceDetected,
                                                            liveStatus_.deviceMidiUnresponsive,
                                                            liveStatus_.deviceType),
                .deviceMidiUnresponsive = liveStatus_.deviceMidiUnresponsive,
                .nowMs = juce::Time::getMillisecondCounter(),
            });

        if (update.state.exhausted && ! searchingWindow_.exhausted)
        {
            applySearchingWindowUpdate(update);
            recomputeDeviceRow();
            return;
        }
    }

    if (deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kSearching)
    {
        searchingDotFrame_ = (searchingDotFrame_ + 1) % 3;
        repaint();
        return;
    }

    stopTimer();
}

void EpromTypePromptDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! getDialogBounds().contains(e.getPosition()))
        dismissAsLater();
}

bool EpromTypePromptDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        dismissAsLater();
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        confirm();
        return true;
    }

    return Component::keyPressed(key);
}
