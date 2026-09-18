#include "EpromTypePromptDialog.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
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
    buildControls(skin);
    wireMidiComboCallbacks();

    epromTypeCombo_->onChange = [this]
    {
        if (! suppressMidiCallbacks_)
            epromComboTouchedByUser_ = true;
    };

    confirmButton_.onClick = [this] { confirm(); };
    specifyLaterButton_.onClick = [this] { dismissAsLater(); };
    specifyLaterButton_.setWantsKeyboardFocus(false);
    specifyLaterButton_.setMouseClickGrabsKeyboardFocus(false);
    confirmButton_.setMouseClickGrabsKeyboardFocus(false);
}

void EpromTypePromptDialog::buildControls(TSS::ISkin& skin)
{
    const auto comboStyle = TSS::ComboBox::Style::ButtonLike;
    const auto labelLook = TSS::labelLookFromSkin(skin);
    const auto comboLook = TSS::comboBoxLookFromSkin(skin);
    const auto popupLook = TSS::popupMenuLookFromSkin(skin);

    auto makeLabel = [&](const char* text)
    {
        return std::make_unique<TSS::Label>(kLabelWidth_, kControlHeight_, labelLook, text);
    };
    auto makeCombo = [&]()
    {
        auto combo = std::make_unique<TSS::ComboBox>(
            kComboWidth_, kControlHeight_, comboLook, comboStyle);
        combo->setPopupMenuLook(popupLook);
        return combo;
    };

    midiFromLabel_ = makeLabel(PluginDisplayNames::Dialogs::EpromTypePrompt::kMidiFromLabel);
    midiFromCombo_ = makeCombo();
    midiToLabel_ = makeLabel(PluginDisplayNames::Dialogs::EpromTypePrompt::kMidiToLabel);
    midiToCombo_ = makeCombo();
    deviceLabel_ = makeLabel(PluginDisplayNames::Dialogs::EpromTypePrompt::kDeviceLabel);
    deviceValueField_ = std::make_unique<TSS::ReadOnlyValueField>(skin);
    epromTypeLabel_ = makeLabel(PluginDisplayNames::Dialogs::EpromTypePrompt::kEpromTypeLabel);
    epromTypeCombo_ = makeCombo();

    addAndMakeVisible(*midiFromLabel_);
    addAndMakeVisible(*midiFromCombo_);
    addAndMakeVisible(*midiToLabel_);
    addAndMakeVisible(*midiToCombo_);
    addAndMakeVisible(*deviceLabel_);
    addAndMakeVisible(*deviceValueField_);
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
    deviceLabel_->setLook(TSS::labelLookFromSkin(skin));
    deviceValueField_->setSkin(skin);
    epromTypeLabel_->setLook(TSS::labelLookFromSkin(skin));
    epromTypeCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    epromTypeCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    refreshDeviceValueField();
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

    refreshDeviceValueField();
    syncAnimationTimer();
}

void EpromTypePromptDialog::refreshDeviceValueField()
{
    if (deviceValueField_ == nullptr || skin_ == nullptr)
        return;

    const juce::Colour fill { ColourChart::kDarkGrey4 };
    const juce::Colour normalText { ColourChart::kLightGrey2 };
    const juce::Colour errorText { ColourChart::kRed };

    if (deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kSearching)
    {
        deviceValueField_->setColours(fill, normalText);
        deviceValueField_->setText(searchingDetailWithDots());
        return;
    }

    if (deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kNotConnected)
    {
        deviceValueField_->setColours(fill, errorText);
        deviceValueField_->setText(deviceRowView_.detailText);
        return;
    }

    deviceValueField_->setColours(fill, normalText);
    deviceValueField_->setText(deviceRowView_.detailText);
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
        refreshDeviceValueField();
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
