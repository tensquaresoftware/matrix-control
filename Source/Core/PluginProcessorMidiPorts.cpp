// Extracted from PluginProcessor.cpp for modular maintenance.
// MIDI port selection, persistence, deferred re-open retries and Keyboard From.

#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Core/MIDI/KeyboardFromMidiInput.h"
#include "Core/MIDI/MidiDevicePresence.h"
#include "Core/MIDI/MidiPortStateCoherence.h"
#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "GUI/PluginEditor.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PluginProcessorInternal;

namespace
{
    void clearDeadMidiInputIfNeeded(MidiManager& midiManager,
                                    juce::ValueTree& state,
                                    const juce::String& desiredId)
    {
        if (! Core::shouldForceReopenForUiRefresh(desiredId, Core::isMidiInputIdentifierLive(desiredId)))
            return;

        if (Core::shouldClearDeadPortAfterReopen(desiredId,
                                                 midiManager.forceReopenInputPort(desiredId, false)))
        {
            midiManager.forceReopenInputPort({}, false);
            state.setProperty("midiInputPortId", juce::String(), nullptr);
        }
    }

    void clearDeadMidiOutputIfNeeded(MidiManager& midiManager,
                                     juce::ValueTree& state,
                                     const juce::String& desiredId)
    {
        if (! Core::shouldForceReopenForUiRefresh(desiredId, Core::isMidiOutputIdentifierLive(desiredId)))
            return;

        if (Core::shouldClearDeadPortAfterReopen(desiredId,
                                                 midiManager.forceReopenOutputPort(desiredId, false)))
        {
            midiManager.forceReopenOutputPort({}, false);
            state.setProperty("midiOutputPortId", juce::String(), nullptr);
        }
    }
}

PluginProcessor::DeferredMidiPortSyncTimer::DeferredMidiPortSyncTimer(PluginProcessor& processorIn)
    : processor(processorIn)
{
}

void PluginProcessor::DeferredMidiPortSyncTimer::startRetrySeries()
{
    stopTimer();
    attemptIndex_ = 0;
    scheduleNextAttempt();
}

int PluginProcessor::DeferredMidiPortSyncTimer::delayMsForAttempt(int attemptIndex)
{
    if (isVst3Wrapper())
    {
        constexpr int vstDelaysMs[] = { 400, 1200, 3000, 6000 };
        return vstDelaysMs[juce::jmin(attemptIndex, kMaxAttempts_ - 1)];
    }

    constexpr int pluginDelaysMs[] = { 300, 800, 2000, 2000 };
    return pluginDelaysMs[juce::jmin(attemptIndex, kMaxAttempts_ - 1)];
}

void PluginProcessor::DeferredMidiPortSyncTimer::scheduleNextAttempt()
{
    startTimer(delayMsForAttempt(attemptIndex_));
}

void PluginProcessor::DeferredMidiPortSyncTimer::timerCallback()
{
    stopTimer();

    const bool isLastAttempt = attemptIndex_ >= kMaxAttempts_ - 1;
    processor.syncMidiPortsFromStateImpl(isLastAttempt);

    if (!processor.arePersistedMidiPortsOpen() && !isLastAttempt)
    {
        ++attemptIndex_;
        scheduleNextAttempt();
    }
}

bool PluginProcessor::setMidiInputPort(const juce::String& deviceId)
{
    if (midiManager == nullptr)
        return false;

    if (isStandaloneWrapper()
        && Core::isMidiFromKeyboardFromConflict(
               deviceId,
               apvts.state.getProperty("keyboardFromPortId", juce::String()).toString()))
    {
        apvts.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter),
            nullptr);
        apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);
        return false;
    }

    if (midiManager->setMidiInputPort(deviceId))
    {
        apvts.state.setProperty("midiInputPortId", deviceId, nullptr);
        Core::DeviceConnectionMachineDefaults::writeMidiInputPort(deviceId);
        Core::clearMidiFromKeyboardFromConflictFooterIfPresent(apvts.state);
        notifyNonParameterStateChanged();
        midiManager->refreshDeviceInquiryAfterPortSync();
        return true;
    }

    return false;
}

bool PluginProcessor::setMidiOutputPort(const juce::String& deviceId)
{
    if (midiManager == nullptr)
        return false;

    if (midiManager->setMidiOutputPort(deviceId))
    {
        apvts.state.setProperty("midiOutputPortId", deviceId, nullptr);
        Core::DeviceConnectionMachineDefaults::writeMidiOutputPort(deviceId);
        notifyNonParameterStateChanged();
        midiManager->refreshDeviceInquiryAfterPortSync();
        return true;
    }

    return false;
}

bool PluginProcessor::isStandalone() const
{
    return isStandaloneWrapper();
}

bool PluginProcessor::setKeyboardFromPort(const juce::String& deviceId)
{
    if (!isStandaloneWrapper() || keyboardFromMidiInput_ == nullptr)
        return false;

    if (deviceId.isEmpty())
    {
        keyboardFromMidiInput_->closePort();
        apvts.state.setProperty("keyboardFromEnabled", false, nullptr);
        apvts.state.setProperty("keyboardFromPortId", juce::String(), nullptr);
        Core::clearMidiFromKeyboardFromConflictFooterIfPresent(apvts.state);
        return true;
    }

    if (Core::isMidiFromKeyboardFromConflict(
            apvts.state.getProperty("midiInputPortId", juce::String()).toString(),
            deviceId))
    {
        apvts.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter),
            nullptr);
        apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);
        return false;
    }

    // Match MIDI From: leave APVTS unchanged on open failure so the editor can restore the
    // previous selection without racing a premature empty-id combo resync.
    if (!keyboardFromMidiInput_->setPort(deviceId))
        return false;

    apvts.state.setProperty("keyboardFromEnabled", true, nullptr);
    apvts.state.setProperty("keyboardFromPortId", deviceId, nullptr);
    Core::clearMidiFromKeyboardFromConflictFooterIfPresent(apvts.state);
    return true;
}

void PluginProcessor::revalidateOpenMidiPortsForUiRefresh()
{
    if (midiManager == nullptr)
        return;

    clearDeadMidiInputIfNeeded(
        *midiManager,
        apvts.state,
        apvts.state.getProperty("midiInputPortId", juce::String()).toString());
    clearDeadMidiOutputIfNeeded(
        *midiManager,
        apvts.state,
        apvts.state.getProperty("midiOutputPortId", juce::String()).toString());

    if (! isStandaloneWrapper() || keyboardFromMidiInput_ == nullptr)
        return;

    const auto desiredKeyboard =
        apvts.state.getProperty("keyboardFromPortId", juce::String()).toString();
    if (! Core::shouldForceReopenForUiRefresh(desiredKeyboard,
                                              Core::isMidiInputIdentifierLive(desiredKeyboard)))
    {
        return;
    }

    const bool reopenSucceeded = setKeyboardFromPort(desiredKeyboard);
    // Conflict returns false without closing; only clear when we are not holding that id.
    if (Core::shouldClearDeadPortAfterReopen(desiredKeyboard, reopenSucceeded)
        && getKeyboardFromOpenDeviceId() != desiredKeyboard)
    {
        setKeyboardFromPort({});
    }
}

juce::String PluginProcessor::getKeyboardFromOpenDeviceId() const
{
    if (keyboardFromMidiInput_ == nullptr || ! keyboardFromMidiInput_->isPortOpen())
        return {};

    return apvts.state.getProperty("keyboardFromPortId", juce::String()).toString();
}

void PluginProcessor::syncMidiPortsFromStateImpl(bool reportOpenFailures)
{
    if (midiManager == nullptr)
        return;

    syncMidiInputPortFromState(reportOpenFailures);
    syncMidiOutputPortFromState(reportOpenFailures);

    midiManager->refreshDeviceInquiryAfterPortSync();
}

void PluginProcessor::syncMidiInputPortFromState(bool reportOpenFailures)
{
    auto inputPortId = apvts.state.getProperty("midiInputPortId", juce::String()).toString();
    const auto sanitizedInputPortId = sanitizePersistedMidiInputPortId(inputPortId);

    if (sanitizedInputPortId != inputPortId)
        apvts.state.setProperty("midiInputPortId", sanitizedInputPortId, nullptr);

    const auto keyboardFromId = apvts.state.getProperty("keyboardFromPortId", juce::String()).toString();
    const bool midiFromConflictsWithKeyboard = isStandaloneWrapper()
        && Core::isMidiFromKeyboardFromConflict(sanitizedInputPortId, keyboardFromId);

    if (! midiFromConflictsWithKeyboard)
    {
        const bool inputOpened = midiManager->setMidiInputPort(sanitizedInputPortId, reportOpenFailures);
        // Option 2: align APVTS only when reporting (final attempt / standalone). Soft intermediate
        // plugin retries keep the desired id so deferred reopen can retry the same target.
        Core::maybeAlignApvtsPortIdAfterOpenAttempt(
            apvts.state,
            "midiInputPortId",
            reportOpenFailures,
            inputOpened,
            sanitizedInputPortId,
            midiManager->getOpenInputDeviceId());
        return;
    }

    // Refuse double-open on sync (interactive setters already guard). Clear MIDI From —
    // retries cannot succeed while Keyboard From holds the same device.
    midiManager->setMidiInputPort(juce::String(), reportOpenFailures);
    apvts.state.setProperty("midiInputPortId", juce::String(), nullptr);

    if (reportOpenFailures)
    {
        apvts.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter),
            nullptr);
        apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);
    }
}

void PluginProcessor::syncMidiOutputPortFromState(bool reportOpenFailures)
{
    auto outputPortId = apvts.state.getProperty("midiOutputPortId", juce::String()).toString();
    const auto sanitizedOutputPortId = sanitizePersistedMidiOutputPortId(outputPortId);

    if (sanitizedOutputPortId != outputPortId)
        apvts.state.setProperty("midiOutputPortId", sanitizedOutputPortId, nullptr);

    const bool outputOpened = midiManager->setMidiOutputPort(sanitizedOutputPortId, reportOpenFailures);
    Core::maybeAlignApvtsPortIdAfterOpenAttempt(
        apvts.state,
        "midiOutputPortId",
        reportOpenFailures,
        outputOpened,
        sanitizedOutputPortId,
        midiManager->getOpenOutputDeviceId());
}

void PluginProcessor::syncMidiPortsFromState(bool reportOpenFailures)
{
    if (midiManager == nullptr)
        return;

    if (isStandaloneWrapper())
    {
        syncMidiPortsFromStateImpl(reportOpenFailures);
        return;
    }

    runSyncOnMessageThread([this, reportOpenFailures]()
    {
        syncMidiPortsFromStateImpl(reportOpenFailures);
    });
}

bool PluginProcessor::arePersistedMidiPortsOpen() const
{
    if (midiManager == nullptr)
        return true;

    const auto inputPortId = apvts.state.getProperty("midiInputPortId", juce::String()).toString();
    if (inputPortId.isNotEmpty() && !midiManager->isInputPortOpenWithDevice(inputPortId))
        return false;

    const auto outputPortId = apvts.state.getProperty("midiOutputPortId", juce::String()).toString();
    if (outputPortId.isNotEmpty() && !midiManager->isOutputPortOpenWithDevice(outputPortId))
        return false;

    return true;
}

void PluginProcessor::restoreMidiPortsForHost()
{
    if (isStandaloneWrapper())
    {
        syncMidiPortsFromStateImpl(true);
        return;
    }

    runSyncOnMessageThread([this]()
    {
        syncMidiPortsFromStateImpl(false);

        if (!arePersistedMidiPortsOpen())
            scheduleDeferredMidiPortSyncForPluginHost();
    });
}

void PluginProcessor::scheduleDeferredMidiPortSyncForPluginHost()
{
    if (isStandaloneWrapper() || deferredMidiPortSyncTimer_ == nullptr)
        return;

    runSyncOnMessageThread([this]()
    {
        if (deferredMidiPortSyncTimer_ != nullptr)
            deferredMidiPortSyncTimer_->startRetrySeries();
    });
}

void PluginProcessor::installMidiDeviceListConnection()
{
    juce::WeakReference<PluginProcessor> weakThis(this);
    midiDeviceListConnection_ = juce::MidiDeviceListConnection::make(
        [weakThis]
        {
            juce::MessageManager::callAsync(
                [weakThis]
                {
                    if (auto* self = weakThis.get())
                        self->handleMidiDeviceListChanged();
                });
        });
}

void PluginProcessor::handleMidiDeviceListChanged()
{
    // Soft open reporting: avoid permanently clearing persisted From/To on transient hot-plug
    // open failures (same rationale as deferred host MIDI reopen retries).
    syncMidiPortsFromState(false);

    if (isStandaloneWrapper())
    {
        const auto keyboardFromId =
            apvts.state.getProperty("keyboardFromPortId", juce::String()).toString();
        if (keyboardFromId.isNotEmpty() && ! setKeyboardFromPort(keyboardFromId))
            setKeyboardFromPort({});
    }

    if (auto* editor = dynamic_cast<PluginEditor*>(getActiveEditor()))
        editor->refreshMidiPortListsFromOsChange();
}

void PluginProcessor::initializeMidiPortProperties()
{
    if (!apvts.state.hasProperty("midiInputPortId"))
    {
        apvts.state.setProperty("midiInputPortId", juce::String(), nullptr);
    }
    if (!apvts.state.hasProperty("midiOutputPortId"))
    {
        apvts.state.setProperty("midiOutputPortId", juce::String(), nullptr);
    }

    if (!apvts.state.hasProperty("keyboardFromEnabled"))
    {
        apvts.state.setProperty("keyboardFromEnabled", false, nullptr);
    }

    if (!apvts.state.hasProperty("keyboardFromPortId"))
    {
        apvts.state.setProperty("keyboardFromPortId", juce::String(), nullptr);
    }

    if (apvts.state.hasProperty("guiZoomLevelId"))
    {
        const auto oldValue = apvts.state.getProperty("guiZoomLevelId");
        apvts.state.setProperty(PluginIDs::Settings::kGuiScale, oldValue, nullptr);
        apvts.state.removeProperty("guiZoomLevelId", nullptr);
    }

    if (!apvts.state.hasProperty(PluginIDs::Settings::kGuiScale))
    {
        apvts.state.setProperty(PluginIDs::Settings::kGuiScale,
                                PluginIDs::Settings::ScaleLevels::kDefault,
                                nullptr);
    }

    if (!apvts.state.hasProperty(PluginIDs::Settings::kSkinVariant))
    {
        apvts.state.setProperty(PluginIDs::Settings::kSkinVariant,
                                PluginIDs::Settings::SkinVariants::kDefault,
                                nullptr);
    }
}
