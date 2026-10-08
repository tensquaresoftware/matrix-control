#include "GettingStartedWizardDialog.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Skins/ColourChart.h"
#include "Shared/Definitions/PluginDisplayNames.h"

void GettingStartedWizardDialog::updateLiveDeviceStatus(const LiveDeviceStatus& status)
{
    liveStatus_ = status;
    includeFirmwareSuggestionHint_ = status.deviceDetected
        && status.deviceVersion.trim().isNotEmpty();
    if (epromTypeCombo_ != nullptr)
    {
        const auto family = Core::EpromTypePolicy::deviceFamilyFromType(status.deviceType);
        epromTypeCombo_->setPopupOnlyMarkedItem(Core::EpromTypePolicy::inquiryPopupMarkerId(
            status.deviceDetected, status.deviceVersion, family));
    }
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
    ensureAudioPage();
    if (audioPage_ == nullptr)
        return;

    audioPage_->populateSynthFromCombo(names, ids);
    audioPage_->selectSynthFromSourceId(selectedId);
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
