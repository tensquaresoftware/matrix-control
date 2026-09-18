// Extracted from MidiManager.cpp for modular maintenance.
// Universal Device Inquiry arm / poll / finish paths.

#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/DeviceInquiryCaptureFilter.h"
#include "Core/MIDI/DeviceInquiryTrigger.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MasterPullOnConnectPolicy.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

void MidiManager::clearDeviceDetectionAfterPortLoss()
{
    const bool hadInquiryPair = lastInquiryInputId_.isNotEmpty() || lastInquiryOutputId_.isNotEmpty();
    const bool wasDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));

    cancelPendingSysExRequest();
    clearLastInquiryPortPair();
    forceMasterPullOnNextInquirySuccess_ = false;

    if (! wasDetected && ! hadInquiryPair)
    {
        setDeviceMidiUnresponsive(false);
        updateDevicePresenceMonitoring();
        return;
    }

    sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
    updateDeviceStatus(false);
}

void MidiManager::clearLastInquiryPortPair() noexcept
{
    lastInquiryInputId_.clear();
    lastInquiryOutputId_.clear();
}

void MidiManager::updateDevicePresenceMonitoring()
{
    if (devicePresenceTimer_ == nullptr)
        return;

    const auto inputId = inputMidiPort != nullptr ? inputMidiPort->getOpenDeviceId() : juce::String();
    const auto outputId = outputMidiPort != nullptr ? outputMidiPort->getOpenDeviceId() : juce::String();

    if (! Core::shouldForceDeviceInquiryForPresence(isDeviceDumpAvailable(), inputId, outputId))
    {
        devicePresenceTimer_->stopTimer();
        return;
    }

    const bool detected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const int intervalMs = detected ? Core::MidiRequestTiming::kDevicePresenceHeartbeatDetectedMs
                                    : Core::MidiRequestTiming::kDevicePresenceRetryUndetectedMs;

    if (! devicePresenceTimer_->isTimerRunning()
        || devicePresenceTimer_->getTimerInterval() != intervalMs)
    {
        devicePresenceTimer_->startTimer(intervalMs);
    }
}

void MidiManager::onDevicePresenceTimer()
{
    const auto inputId = inputMidiPort != nullptr ? inputMidiPort->getOpenDeviceId() : juce::String();
    const auto outputId = outputMidiPort != nullptr ? outputMidiPort->getOpenDeviceId() : juce::String();

    if (! Core::shouldForceDeviceInquiryForPresence(isDeviceDumpAvailable(), inputId, outputId))
    {
        updateDevicePresenceMonitoring();
        return;
    }

    // Skip while any shared async SysEx capture is active — performDeviceInquiry cancels it.
    if (asyncSysExCaptureActive_.load(std::memory_order_acquire) || pendingAsyncCallback_ != nullptr)
        return;

    lastInquiryInputId_ = inputId;
    lastInquiryOutputId_ = outputId;
    performDeviceInquiry();
}

void MidiManager::refreshDeviceInquiryAfterPortSync()
{
    if (! isDeviceDumpAvailable())
    {
        clearDeviceDetectionAfterPortLoss();
        return;
    }

    const auto inputId = inputMidiPort != nullptr ? inputMidiPort->getOpenDeviceId() : juce::String();
    const auto outputId = outputMidiPort != nullptr ? outputMidiPort->getOpenDeviceId() : juce::String();

    if (inputId.isEmpty() || outputId.isEmpty())
    {
        clearDeviceDetectionAfterPortLoss();
        return;
    }

    if (Core::shouldStartDeviceInquiry(true,
                                       inputId,
                                       outputId,
                                       lastInquiryInputId_,
                                       lastInquiryOutputId_))
    {
        // Port pair changed while both ports are open: detection may still say "connected".
        // Force Master pull on the coming success so MIDI TO reconnect mirrors a fresh detect.
        forceMasterPullOnNextInquirySuccess_ = true;
        lastInquiryInputId_ = inputId;
        lastInquiryOutputId_ = outputId;
        performDeviceInquiry();
    }

    updateDevicePresenceMonitoring();
}

void MidiManager::handleAsyncDeviceInquiryResponse(std::uint64_t token,
                                                   const juce::MemoryBlock& response)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (sysExParser == nullptr)
    {
        finishAsyncDeviceInquiryFailure(token, "SysEx parser unavailable", "SysEx");
        return;
    }

    const auto validation = sysExParser->validateSysEx(response);
    if (! validation.isValid || validation.messageType != SysExParser::MessageType::kDeviceId)
    {
        armAsyncDeviceInquiryCapture(token);
        return;
    }

    if (sysExDecoder == nullptr)
    {
        finishAsyncDeviceInquiryFailure(token, "SysEx decoder unavailable", "SysEx");
        return;
    }

    MidiLogger::getInstance().logSysExReceived(response, "Device ID response");
    const DeviceIdInfo deviceInfo = sysExDecoder->decodeDeviceId(response);

    if (deviceInfo.isValid)
    {
        const auto deviceType = Core::DeviceTypeRegistry::fromDeviceInquiry(deviceInfo);
        finishAsyncDeviceInquirySuccess(token, deviceInfo, deviceType);
        return;
    }

    finishAsyncDeviceInquiryFailure(token,
                                    "Connected device is not a supported Oberheim Matrix synth",
                                    "Device");
}

bool MidiManager::armAsyncDeviceInquiryCapture(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return false;

    if (midiReceiver == nullptr)
    {
        finishAsyncDeviceInquiryFailure(token, "MIDI input not available for Device Inquiry", "Connection");
        return false;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    midiReceiver->armOneShotSysExCapture(
        [weakThis, token](const juce::MemoryBlock& response)
        {
            juce::MessageManager::callAsync(
                [weakThis, token, response]
                {
                    if (auto* self = weakThis.get())
                        self->handleAsyncDeviceInquiryResponse(token, response);
                });
        },
        [](const juce::MemoryBlock& sysex)
        {
            return Core::isDeviceInquiryIdentityReply(sysex);
        });

    return true;
}

void MidiManager::finishAsyncDeviceInquirySuccess(std::uint64_t token,
                                                  const DeviceIdInfo& info,
                                                  MatrixDeviceTypes::Type deviceType)
{
    auto expected = token;
    if (! asyncRequestToken_.compare_exchange_strong(expected, token + 1, std::memory_order_acq_rel))
        return;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    asyncSysExCaptureActive_.store(false, std::memory_order_release);

    const bool wasDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const auto previousType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));

    const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
        apvts.state.getProperty(PluginIDs::Settings::kEpromType,
                               PluginIDs::Settings::EpromType::kDefault)));
    sysExDelay_.setProfile(Core::SysExDelayProfile::fromDeviceInquiry(info, epromType));
    updateDeviceStatus(true, info.version, deviceType);

    const bool promptDone = static_cast<bool>(
        apvts.state.getProperty(PluginIDs::Settings::kEpromTypePromptDone, false));
    if (! promptDone)
        apvts.state.setProperty(PluginIDs::Settings::kEpromTypePromptPending, true, nullptr);

    const auto pullSnapshot = Core::consumeMasterPullInquirySnapshot(
        wasDetected, previousType, forceMasterPullOnNextInquirySuccess_);
    maybePullMasterAfterInquirySuccess(pullSnapshot.wasDetectedBeforeSuccess,
                                       pullSnapshot.previousType,
                                       deviceType,
                                       pullSnapshot.forceBecausePortPairChanged);
}

void MidiManager::finishAsyncDeviceInquiryFailure(std::uint64_t token,
                                                  const juce::String& errorMessage,
                                                  const juce::String& errorType)
{
    auto expected = token;
    if (! asyncRequestToken_.compare_exchange_strong(expected, token + 1, std::memory_order_acq_rel))
        return;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    asyncSysExCaptureActive_.store(false, std::memory_order_release);
    forceMasterPullOnNextInquirySuccess_ = false;
    clearLastInquiryPortPair();
    sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
    updateDeviceStatus(false);
    updateErrorState(errorMessage, errorType);
}

void MidiManager::softAbortDeviceInquiryOutboundBusy(std::uint64_t token)
{
    auto expected = token;
    if (! asyncRequestToken_.compare_exchange_strong(expected, token + 1, std::memory_order_acq_rel))
        return;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    asyncSysExCaptureActive_.store(false, std::memory_order_release);

    // Keep deviceDetected / lastInquiry pair / delay profile — a busy wire is not a missing synth.
    // Presence timer will retry on the next cadence tick.
    MidiLogger::getInstance().logInfo(
        "Device Inquiry deferred: outbound MIDI queue still busy (presence will retry)");
}

void MidiManager::setDeviceMidiUnresponsive(bool unresponsive)
{
    const bool previous = static_cast<bool>(
        apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false));
    if (previous == unresponsive)
        return;

    apvts.state.setProperty(Core::kDeviceMidiUnresponsiveProperty, unresponsive, nullptr);

    if (unresponsive)
    {
        apvts.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::FooterPanel::kDeviceUnresponsiveGuidance),
            nullptr);
        apvts.state.setProperty("uiMessageSeverity", "error", nullptr);
        MidiLogger::getInstance().logWarning(
            "Matrix synth not responding to Device Inquiry — editor SysEx paused; power-cycle if notes stick");
        return;
    }

    if (apvts.state.getProperty("uiMessageText").toString()
        == PluginDisplayNames::FooterPanel::kDeviceUnresponsiveGuidance)
    {
        apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
        apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
    }
}

void MidiManager::softAbortDeviceInquiryUnresponsive(std::uint64_t token)
{
    auto expected = token;
    if (! asyncRequestToken_.compare_exchange_strong(expected, token + 1, std::memory_order_acq_rel))
        return;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    asyncSysExCaptureActive_.store(false, std::memory_order_release);

    // Keep deviceDetected / lastInquiry pair / delay profile — silence ≠ unplugged cables.
    setDeviceMidiUnresponsive(true);
}

void MidiManager::scheduleDeviceInquiryTimeout(std::uint64_t token)
{
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(
        SysExConstants::kDefaultTimeoutMs,
        [weakThis, token]
        {
            if (auto* self = weakThis.get())
            {
                if (token != self->asyncRequestToken_.load(std::memory_order_acquire))
                    return;

                MidiLogger::getInstance().logWarning(
                    "Timeout waiting for SysEx response ("
                    + juce::String(SysExConstants::kDefaultTimeoutMs) + "ms)");

                const bool wasDetected = static_cast<bool>(
                    self->apvts.state.getProperty("deviceDetected", false));
                if (wasDetected)
                {
                    self->softAbortDeviceInquiryUnresponsive(token);
                    return;
                }

                self->finishAsyncDeviceInquiryFailure(token,
                                                      "Timeout waiting for Device ID response",
                                                      "Timeout");
            }
        });
}

void MidiManager::sendArmedDeviceInquiry(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (! isDeviceDumpAvailable() || midiSender == nullptr || ! midiSender->isOutputAvailable())
    {
        finishAsyncDeviceInquiryFailure(token, "MIDI ports not available for Device Inquiry", "Connection");
        return;
    }

    try
    {
        auto inquiryMessage = SysExEncoder::encodeDeviceInquiry();
        if (! armAsyncDeviceInquiryCapture(token))
            return;

        sendSysExWithDelay(inquiryMessage, "Device Inquiry");
        scheduleDeviceInquiryTimeout(token);
    }
    catch (const MidiConnectionException& e)
    {
        finishAsyncDeviceInquiryFailure(token, e.getMessage(), "Connection");
    }
    catch (const std::exception& e)
    {
        finishAsyncDeviceInquiryFailure(token, e.what(), "SysEx");
    }
}

void MidiManager::scheduleOrSendArmedDeviceInquiry(const OutboundIdlePollArgs& args)
{
    if (args.settleMs <= 0)
    {
        sendArmedDeviceInquiry(args.token);
        return;
    }

    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(args.settleMs,
                                [weakThis, token = args.token]
                                {
                                    if (auto* self = weakThis.get())
                                        self->sendArmedDeviceInquiry(token);
                                });
}

void MidiManager::pollOutboundIdleThenDeviceInquiry(OutboundIdlePollArgs args)
{
    if (args.token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (isOutboundQueueIdle())
    {
        scheduleOrSendArmedDeviceInquiry(args);
        return;
    }

    if (hasOutboundIdleTimedOut(args))
    {
        softAbortDeviceInquiryOutboundBusy(args.token);
        return;
    }

    wakeConsumer();
    juce::WeakReference<MidiManager> weakThis(this);
    juce::Timer::callAfterDelay(1,
                                [weakThis, args]
                                {
                                    if (auto* self = weakThis.get())
                                        self->pollOutboundIdleThenDeviceInquiry(args);
                                });
}

void MidiManager::performDeviceInquiry()
{
    cancelPendingSysExRequest();

    if (! isDeviceDumpAvailable() || midiSender == nullptr || ! midiSender->isOutputAvailable())
    {
        clearLastInquiryPortPair();
        sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
        updateDeviceStatus(false);
        updateErrorState("MIDI ports not available for Device Inquiry", "Connection");
        return;
    }

    asyncSysExCaptureActive_.store(true, std::memory_order_release);

    const auto token = asyncRequestToken_.load(std::memory_order_acquire);
    const int profileDelayMs = sysExDelay_.getRequiredDelayMs();
    wakeConsumer();
    pollOutboundIdleThenDeviceInquiry({ token,
                                        Core::MidiRequestTiming::deviceSettleMs(profileDelayMs),
                                        juce::Time::getMillisecondCounter(),
                                        Core::MidiRequestTiming::outboundIdleTimeoutMs(profileDelayMs) });
}
