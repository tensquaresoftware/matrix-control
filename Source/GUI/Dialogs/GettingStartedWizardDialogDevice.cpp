#include "GettingStartedWizardDialog.h"

#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Settings/AudioDeviceSetupSync.h"
#include "GUI/Skins/ColourChart.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    int deviceComboIdForName(const juce::StringArray& names, const juce::String& deviceName)
    {
        if (deviceName.isEmpty())
            return 1;

        const int index = names.indexOf(deviceName);
        if (index >= 0)
            return index + 2;

        return names.size() + 2;
    }

    void populateDeviceCombo(TSS::ComboBox& combo,
                             const juce::StringArray& names,
                             const juce::String& selectedName)
    {
        combo.clear(juce::dontSendNotification);
        combo.addItem(PluginDisplayNames::Settings::kNoDeviceSentinel, 1);
        for (int i = 0; i < names.size(); ++i)
            combo.addItem(names[i].toUpperCase(), i + 2);

        if (selectedName.isNotEmpty() && ! names.contains(selectedName))
            combo.addItem(selectedName.toUpperCase(), names.size() + 2);

        combo.setSelectedId(deviceComboIdForName(names, selectedName), juce::dontSendNotification);
    }

    juce::String selectedDeviceName(const TSS::ComboBox& combo,
                                    const juce::StringArray& scannedNames,
                                    const juce::String& liveSetupName)
    {
        const int id = combo.getSelectedId();
        if (id <= 1)
            return {};

        if (id >= 2 && id - 2 < scannedNames.size())
            return scannedNames[id - 2];

        // Orphan combo item is display-uppercased; keep the live setup spelling for apply.
        return liveSetupName;
    }
}

void GettingStartedWizardDialog::updateLiveDeviceStatus(const LiveDeviceStatus& status)
{
    liveStatus_ = status;
    includeFirmwareSuggestionHint_ = status.deviceDetected
        && status.deviceVersion.trim().isNotEmpty();
    recomputeDeviceRow();
    resized();
    repaint();
}

void GettingStartedWizardDialog::refreshEpromSuggestion(MatrixDeviceTypes::Type deviceType,
                                                        int preferredSelectedId)
{
    const int currentId = epromTypeCombo_->getSelectedId();
    populateEpromItems(deviceType,
                       Core::nextDeviceSetupEpromPreferredId(
                           epromComboTouchedByUser_, currentId, preferredSelectedId));
}

void GettingStartedWizardDialog::populateSynthFromChannels(const juce::StringArray& names,
                                                           const juce::StringArray& ids,
                                                           const juce::String& selectedId)
{
    if (synthFromCombo_ == nullptr)
        return;

    const juce::ScopedValueSetter<bool> guard(suppressControlCallbacks_, true);
    synthFromCombo_->clear(juce::dontSendNotification);
    synthFromSourceIdentifiers_.clear();
    synthFromCombo_->addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel,
                             TSS::MidiPortComboPopulation::kPortSentinelItemId);

    const int count = juce::jmin(names.size(), ids.size());
    for (int i = 0; i < count; ++i)
    {
        const int itemId = i + TSS::MidiPortComboPopulation::kFirstDeviceItemId;
        synthFromCombo_->addItem(names[i].toUpperCase(), itemId);
        synthFromSourceIdentifiers_.push_back(ids[i]);
    }

    TSS::MidiPortComboPopulation::selectPortInCombo(
        *synthFromCombo_, synthFromSourceIdentifiers_, selectedId);
}

void GettingStartedWizardDialog::applySearchingWindowUpdate(
    const Core::DeviceSetupSearchingWindowUpdate& update)
{
    searchingWindow_ = update.state;
    if (update.shouldKickInquiry && bindings_.onSearchingWindowStarted != nullptr)
        bindings_.onSearchingWindowStarted();
}

void GettingStartedWizardDialog::recomputeDeviceRow()
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

void GettingStartedWizardDialog::refreshDeviceValueField()
{
    if (deviceValueField_ == nullptr || skin_ == nullptr)
        return;

    const juce::Colour fill { ColourChart::kDarkGrey4 };
    const juce::Colour normalText { ColourChart::kLightGrey2 };
    const juce::Colour errorText { ColourChart::kRed };

    if (deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kSearching)
    {
        static constexpr const char* kFrames[] = { ".", "..", "..." };
        deviceValueField_->setColours(fill, normalText);
        deviceValueField_->setText(
            juce::String(PluginDisplayNames::Dialogs::EpromTypePrompt::kSearching)
            + kFrames[juce::jlimit(0, 2, searchingDotFrame_)]);
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

void GettingStartedWizardDialog::syncAnimationTimer()
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

void GettingStartedWizardDialog::timerCallback()
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

void GettingStartedWizardDialog::refreshDigesteAudioFromDeviceManager()
{
    if (bindings_.audioDeviceManager == nullptr)
        return;

    const juce::ScopedValueSetter<bool> guard(updatingAudioUi_, true);
    auto& manager = *bindings_.audioDeviceManager;

    driverTypeCombo_->clear(juce::dontSendNotification);
    const auto& types = manager.getAvailableDeviceTypes();
    const auto current = manager.getCurrentAudioDeviceType();
    int selectedTypeId = 0;
    for (int i = 0; i < types.size(); ++i)
    {
        const int id = i + 1;
        driverTypeCombo_->addItem(types.getUnchecked(i)->getTypeName().toUpperCase(), id);
        if (types.getUnchecked(i)->getTypeName() == current)
            selectedTypeId = id;
    }
    if (selectedTypeId > 0)
        driverTypeCombo_->setSelectedId(selectedTypeId, juce::dontSendNotification);

    auto* type = manager.getCurrentDeviceTypeObject();
    inputDeviceCombo_->clear(juce::dontSendNotification);
    if (type != nullptr)
    {
        type->scanForDevices();
        const auto inputs = type->getDeviceNames(true);
        populateDeviceCombo(*inputDeviceCombo_, inputs, manager.getAudioDeviceSetup().inputDeviceName);
    }
}

void GettingStartedWizardDialog::applyDigesteAudioFromUi(bool preferOutputEndpoint)
{
    if (bindings_.audioDeviceManager == nullptr || updatingAudioUi_)
        return;

    auto& manager = *bindings_.audioDeviceManager;
    auto* type = manager.getCurrentDeviceTypeObject();
    if (type == nullptr)
        return;

    auto setup = manager.getAudioDeviceSetup();
    const auto inputs = type->getDeviceNames(true);
    setup.inputDeviceName = selectedDeviceName(
        *inputDeviceCombo_, inputs, setup.inputDeviceName);
    AudioDeviceSetupSync::resolveEndpointNamesForApply(
        manager.getCurrentAudioDeviceType(),
        setup.inputDeviceName,
        setup.outputDeviceName,
        preferOutputEndpoint);

    manager.setAudioDeviceSetup(setup, true);
    refreshDigesteAudioFromDeviceManager();
}
