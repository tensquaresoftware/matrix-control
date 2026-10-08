// DEVICE read-only strip + searching animation for Settings MIDI & DEVICE.

#include "SettingsPanel.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Helpers/DeviceVersionDisplayFormat.h"
#include "GUI/Skins/ColourChart.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

void SettingsPanel::setOnSearchingWindowStarted(std::function<void()> callback)
{
    onSearchingWindowStarted_ = std::move(callback);
}

void SettingsPanel::stopLiveTimers()
{
    stopTimer();
    searchingDotFrame_ = 0;
}

void SettingsPanel::updateLiveDeviceStatus(const LiveDeviceStatus& status)
{
    liveStatus_ = status;
    if (epromTypeCombo_ != nullptr)
    {
        const auto family = Core::EpromTypePolicy::deviceFamilyFromType(status.deviceType);
        epromTypeCombo_->setPopupOnlyMarkedItem(Core::EpromTypePolicy::inquiryPopupMarkerId(
            status.deviceDetected, status.deviceVersion, family));
    }
    recomputeDeviceRow();
}

void SettingsPanel::refreshDeviceRow()
{
    recomputeDeviceRow();
}

void SettingsPanel::applySearchingWindowUpdate(const Core::DeviceSetupSearchingWindowUpdate& update)
{
    searchingWindow_ = update.state;
    if (update.shouldKickInquiry && onSearchingWindowStarted_ != nullptr)
        onSearchingWindowStarted_();
}

void SettingsPanel::recomputeDeviceRow()
{
    const auto midiFromId = midiPage_ != nullptr ? midiPage_->getSelectedSynthFromPortId()
                                                 : juce::String();
    const auto midiToId = midiPage_ != nullptr ? midiPage_->getSelectedSynthToPortId()
                                               : juce::String();
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
    syncDeviceAnimationTimer();
}

void SettingsPanel::refreshDeviceValueField()
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

void SettingsPanel::syncDeviceAnimationTimer()
{
    using namespace PluginIDs::Settings::LastTab;

    const bool searchingVisible = activeTabId_ == kMidiAndDevice
        && deviceRowView_.kind == Core::DeviceSetupDeviceRowKind::kSearching;

    if (searchingVisible)
    {
        if (! isTimerRunning())
            startTimer(kSearchingDotsIntervalMs_);
        return;
    }

    stopTimer();
    searchingDotFrame_ = 0;
}

void SettingsPanel::timerCallback()
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
