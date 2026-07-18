#include "MidiManager.h"

#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/DeviceInquiryTrigger.h"
#include "Core/MIDI/MidiPortOpenFeedback.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace
{
    Core::MidiActivityTracker::Path pathForOutboundMessage(
        const Core::MidiOutboundQueue::Message& msg) noexcept
    {
        if (msg.category == Core::MidiOutboundQueue::MessageCategory::kSysEx)
            return Core::MidiActivityTracker::Path::kEditor;

        if (msg.midiMessage.isProgramChange())
            return Core::MidiActivityTracker::Path::kEditor;

        return Core::MidiActivityTracker::Path::kInstrument;
    }

    void reportPortOpenFailure(juce::AudioProcessorValueTreeState& apvts,
                               bool isInput,
                               const Core::MidiPortOpenResult& result,
                               const juce::String& deviceId,
                               bool reportOpenFailure)
    {
        Core::MidiPortOpenFeedback::logOpenFailure(isInput,
                                                   result.portDisplayName,
                                                   deviceId,
                                                   result.failureReason);
        if (reportOpenFailure)
        {
            Core::MidiPortOpenFeedback::propagateOpenFailure(apvts,
                                                            isInput,
                                                            result.portDisplayName,
                                                            result.failureReason);
        }
    }
}

MidiManager::MidiManager(juce::AudioProcessorValueTreeState& apvtsRef,
                         Core::MidiOutboundQueue& outboundQueueRef,
                         Core::MidiActivityTracker& activityTrackerRef)
    : juce::Thread("MidiManager")
    , apvts(apvtsRef)
    , inputMidiPort(std::make_unique<MidiInputPort>())
    , outputMidiPort(std::make_unique<MidiOutputPort>())
    , midiSender(std::make_unique<MidiSender>())
    , midiReceiver(std::make_unique<MidiReceiver>())
    , sysExParser(std::make_unique<SysExParser>())
    , sysExDecoder(std::make_unique<SysExDecoder>(*sysExParser))
    , sysExEncoder(std::make_unique<SysExEncoder>())
    , outboundQueue_(outboundQueueRef)
    , activityTracker_(activityTrackerRef)
    , editorPath_(outboundQueueRef, activityTrackerRef)
    , sysExDelay_(Core::SysExDelayProfile::stockDefault())
{
    if (midiReceiver != nullptr)
        midiReceiver->setActivityTracker(&activityTrackerRef);

    outboundQueue_.setWakeConsumerCallback([this] { wakeConsumer(); });

    apvts.state.setProperty("deviceDetected", false, nullptr);
    apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                            MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kUnknown),
                            nullptr);
    apvts.state.setProperty("deviceVersion", juce::String(), nullptr);
    apvts.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kSelectedBank,
                            Matrix1000Limits::kMinBankNumber,
                            nullptr);
    apvts.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kBanksLocked,
                            false,
                            nullptr);
    apvts.state.setProperty("lastError", juce::String(), nullptr);
    apvts.state.setProperty("errorType", juce::String(), nullptr);
    apvts.state.setProperty("lastPatchLoaded", juce::String(), nullptr);
    
    MidiLogger::getInstance().logInfo("MidiManager initialized");
}

MidiManager::~MidiManager()
{
    cancelPendingSysExRequest();
    stopThread(5000);
    stopMidiInputCallbacks();
    
    if (outputMidiPort != nullptr)
    {
        outputMidiPort->closePort();
    }
}

bool MidiManager::setMidiInputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logInfo("Clearing MIDI input port selection");
        stopMidiInputCallbacks();
        ExceptionPropagator::clearMessage(apvts);
        return true;
    }

    if (inputMidiPort != nullptr && inputMidiPort->isOpenWithDevice(deviceId))
    {
        if (midiReceiver != nullptr)
            midiReceiver->setMidiInput(inputMidiPort->getMidiInput());

        ExceptionPropagator::clearMessage(apvts);
        return true;
    }

    juce::String deviceName = deviceId;
    auto devices = juce::MidiInput::getAvailableDevices();
    for (const auto& device : devices)
    {
        if (device.identifier == deviceId)
        {
            deviceName = device.name;
            break;
        }
    }
    MidiLogger::getInstance().logInfo("Setting MIDI input port: [" + deviceName + "]");

    if (midiReceiver != nullptr)
    {
        midiReceiver->setMidiInput(nullptr);
    }

    const auto openResult = inputMidiPort->openPort(deviceId, midiReceiver.get());
    if (openResult.succeeded())
    {
        midiReceiver->setMidiInput(inputMidiPort->getMidiInput());
        ExceptionPropagator::clearMessage(apvts);
        MidiLogger::getInstance().logInfo("MIDI input port successfully set");
        return true;
    }

    reportPortOpenFailure(apvts, true, openResult, deviceId, reportOpenFailure);
    return false;
}

bool MidiManager::setMidiOutputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logInfo("Clearing MIDI output port selection");
        if (midiSender != nullptr)
        {
            midiSender->setMidiOutput(nullptr);
        }
        if (outputMidiPort != nullptr)
        {
            outputMidiPort->closePort();
        }
        ExceptionPropagator::clearMessage(apvts);
        return true;
    }

    if (outputMidiPort != nullptr && outputMidiPort->isOpenWithDevice(deviceId))
    {
        if (midiSender != nullptr)
            midiSender->setMidiOutput(outputMidiPort->getMidiOutput());

        ExceptionPropagator::clearMessage(apvts);
        return true;
    }

    juce::String deviceName = deviceId;
    auto devices = juce::MidiOutput::getAvailableDevices();
    for (const auto& device : devices)
    {
        if (device.identifier == deviceId)
        {
            deviceName = device.name;
            break;
        }
    }
    MidiLogger::getInstance().logInfo("Setting MIDI output port: [" + deviceName + "]");

    if (midiSender != nullptr)
    {
        midiSender->setMidiOutput(nullptr);
    }

    const auto openResult = outputMidiPort->openPort(deviceId);
    if (openResult.succeeded())
    {
        midiSender->setMidiOutput(outputMidiPort->getMidiOutput());
        ExceptionPropagator::clearMessage(apvts);
        MidiLogger::getInstance().logInfo("MIDI output port successfully set");
        wakeConsumer();
        return true;
    }

    reportPortOpenFailure(apvts, false, openResult, deviceId, reportOpenFailure);
    return false;
}

bool MidiManager::isInputPortOpenWithDevice(const juce::String& deviceId) const
{
    return inputMidiPort != nullptr && inputMidiPort->isOpenWithDevice(deviceId);
}

bool MidiManager::isOutputPortOpenWithDevice(const juce::String& deviceId) const
{
    return outputMidiPort != nullptr && outputMidiPort->isOpenWithDevice(deviceId);
}

void MidiManager::sendPatch(juce::uint8 patchNumber, const juce::uint8* packedData)
{
    if (packedData == nullptr)
    {
        updateErrorState("Invalid patch data", "SysEx");
        return;
    }

    try
    {
        auto sysExMessage = sysExEncoder->encodePatchSysEx(patchNumber, packedData);
        editorPath_.enqueueSysEx(sysExMessage);
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
    }
}

void MidiManager::sendPatchToEditBuffer(const juce::uint8* packedData)
{
    if (packedData == nullptr)
    {
        updateErrorState("Invalid patch data", "SysEx");
        return;
    }

    try
    {
        auto sysExMessage = sysExEncoder->encodePatchToEditBufferSysEx(packedData);
        editorPath_.enqueueSysEx(sysExMessage);
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
    }
}

void MidiManager::sendFullPatchForAudition(const juce::uint8* packedData,
                                           juce::uint8 patchNumber,
                                           bool deviceHasBankConcept)
{
    if (packedData == nullptr)
    {
        updateErrorState("Invalid patch data", "SysEx");
        return;
    }

    // Matrix-1000: non-destructive edit-buffer dump (opcode 0x0D with literal header byte 0).
    // Matrix-6/6R: no 0x0D — write current slot via 0x01.
    if (deviceHasBankConcept)
        sendPatchToEditBuffer(packedData);
    else
        sendPatch(patchNumber, packedData);
}

void MidiManager::sendMaster(juce::uint8 version, const juce::uint8* packedData)
{
    if (packedData == nullptr)
    {
        updateErrorState("Invalid master data", "SysEx");
        return;
    }

    try
    {
        auto sysExMessage = sysExEncoder->encodeMasterSysEx(version, packedData);
        editorPath_.enqueueSysEx(sysExMessage);
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
    }
}

void MidiManager::sendProgramChange(int programNumber, int channel)
{
    try
    {
        editorPath_.enqueueProgramChange(programNumber, channel);
        MidiLogger::getInstance().logProgramChange(static_cast<juce::uint8>(programNumber), "QUEUED");
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "ProgramChange");
    }
}

void MidiManager::sendSetBank(int bank)
{
    try
    {
        auto sysExMessage = sysExEncoder->encodeSetBank(static_cast<juce::uint8>(bank));
        editorPath_.enqueueSysEx(sysExMessage);
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
    }
}

void MidiManager::sendUnlockBank()
{
    try
    {
        auto sysExMessage = SysExEncoder::encodeUnlockBank();
        editorPath_.enqueueSysEx(sysExMessage);
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
    }
}

void MidiManager::enqueueRemoteParameterEdit(int parameterNumber, juce::uint8 packedValue)
{
    if (parameterNumber < 0 || parameterNumber > 127)
        return;

    auto sysExMessage = sysExEncoder->encodeRemoteParameterEdit(
        static_cast<juce::uint8>(parameterNumber),
        packedValue);
    editorPath_.enqueueSysEx(sysExMessage);
}

void MidiManager::enqueueMatrixModBusEdit(juce::uint8 bus,
                                          juce::uint8 source,
                                          juce::uint8 amount,
                                          juce::uint8 destination)
{
    if (bus >= static_cast<juce::uint8>(Matrix1000Limits::kModulationBusCount))
        return;

    auto sysExMessage = sysExEncoder->encodeMatrixModBusEdit(bus, source, amount, destination);
    editorPath_.enqueueSysEx(sysExMessage);
}

void MidiManager::sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    sysExDelay_.waitUntilReady();
    midiSender->sendSysEx(sysExMessage);
    MidiLogger::getInstance().logSysExSent(sysExMessage, description);
    sysExDelay_.recordSysExSent(static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes()));
}

std::vector<juce::uint8> MidiManager::requestCurrentPatch()
{
    return requestSysExData(SysExConstants::RequestType::kRequestEditBuffer,
                           SysExConstants::kPatchPackedDataSize,
                           "patch");
}

std::vector<juce::uint8> MidiManager::requestSinglePatch(juce::uint8 patchNumber)
{
    return requestSysExData(SysExConstants::RequestType::kRequestSinglePatch,
                           SysExConstants::kPatchPackedDataSize,
                           "single patch",
                           patchNumber);
}

void MidiManager::cancelPendingSysExRequest() noexcept
{
    const auto token = asyncRequestToken_.fetch_add(1, std::memory_order_acq_rel) + 1;
    juce::ignoreUnused(token);

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    pendingAsyncCallback_ = nullptr;
}

void MidiManager::finishAsyncPackedPatch(std::uint64_t token, std::vector<juce::uint8> packed)
{
    // First finisher for this token wins (success or timeout); invalidates the other.
    auto expected = token;
    if (! asyncRequestToken_.compare_exchange_strong(expected, token + 1, std::memory_order_acq_rel))
        return;

    if (midiReceiver != nullptr)
        midiReceiver->cancelOneShotSysExCapture();

    auto callback = std::move(pendingAsyncCallback_);
    pendingAsyncCallback_ = nullptr;

    if (callback)
        callback(std::move(packed));
}

std::vector<juce::uint8> MidiManager::decodePackedPatchResponse(const juce::MemoryBlock& response,
                                                                const juce::String& requestDescription)
{
    if (response.getSize() == 0)
        return {};

    MidiLogger::getInstance().logSysExReceived(response, requestDescription + " response");

    std::vector<juce::uint8> packedData(SysExConstants::kPatchPackedDataSize);
    if (sysExDecoder->decodePatchSysEx(response, packedData.data()))
        return packedData;

    updateErrorState("Failed to decode " + requestDescription + " response", "SysEx");
    return {};
}

std::vector<juce::uint8> MidiManager::tryDecodeAsyncPatchResponse(const juce::MemoryBlock& response)
{
    if (response.getSize() == 0 || sysExParser == nullptr || sysExDecoder == nullptr)
        return {};

    // Ignore non-patch SysEx (Device ID, Master, noise) without failing the pending request.
    const auto validation = sysExParser->validateSysEx(response);
    if (! validation.isValid || validation.messageType != SysExParser::MessageType::kPatch)
        return {};

    std::vector<juce::uint8> packedData(SysExConstants::kPatchPackedDataSize);
    if (! sysExDecoder->decodePatchSysEx(response, packedData.data()))
        return {};

    MidiLogger::getInstance().logSysExReceived(response, "single patch response");
    return packedData;
}

void MidiManager::armAsyncSinglePatchCapture(std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (midiReceiver == nullptr)
    {
        finishAsyncPackedPatch(token, {});
        return;
    }

    midiReceiver->armOneShotSysExCapture(
        [this, token](const juce::MemoryBlock& response)
        {
            // MIDI input thread: marshal decode + callback to the message thread.
            juce::MessageManager::callAsync(
                [this, token, response]
                {
                    if (token != asyncRequestToken_.load(std::memory_order_acquire))
                        return;

                    auto packed = tryDecodeAsyncPatchResponse(response);
                    if (! packed.empty())
                    {
                        finishAsyncPackedPatch(token, std::move(packed));
                        return;
                    }

                    // Parasitic / non-patch SysEx consumed the one-shot — keep listening.
                    armAsyncSinglePatchCapture(token);
                });
        });
}

void MidiManager::sendArmedSinglePatchRequest(juce::uint8 patchNumber, std::uint64_t token)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    if (midiReceiver == nullptr || midiSender == nullptr || ! midiSender->isOutputAvailable())
    {
        finishAsyncPackedPatch(token, {});
        return;
    }

    try
    {
        auto requestMessage = sysExEncoder->encodeRequestMessage(
            SysExConstants::RequestType::kRequestSinglePatch, patchNumber);

        armAsyncSinglePatchCapture(token);
        sendSysExWithDelay(requestMessage, "single patch request");

        juce::Timer::callAfterDelay(
            SysExConstants::kDefaultTimeoutMs,
            [this, token]
            {
                if (token != asyncRequestToken_.load(std::memory_order_acquire))
                    return;

                MidiLogger::getInstance().logWarning(
                    "Timeout waiting for SysEx response ("
                    + juce::String(SysExConstants::kDefaultTimeoutMs) + "ms)");
                updateErrorState("Timeout waiting for single patch response", "Timeout");
                finishAsyncPackedPatch(token, {});
            });
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
        finishAsyncPackedPatch(token, {});
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
        finishAsyncPackedPatch(token, {});
    }
}

void MidiManager::requestSinglePatchAsync(juce::uint8 patchNumber,
                                          PackedPatchCallback callback,
                                          int settleMs,
                                          int outboundIdleTimeoutMs)
{
    cancelPendingSysExRequest();

    const auto token = asyncRequestToken_.load(std::memory_order_acquire);
    pendingAsyncCallback_ = std::move(callback);

    wakeConsumer();
    pollOutboundIdleThenRequest(patchNumber,
                                token,
                                juce::jmax(0, settleMs),
                                juce::Time::getMillisecondCounter(),
                                juce::jmax(0, outboundIdleTimeoutMs));
}

void MidiManager::pollOutboundIdleThenRequest(juce::uint8 patchNumber,
                                              std::uint64_t token,
                                              int settleMs,
                                              juce::uint32 idleStartMs,
                                              int outboundIdleTimeoutMs)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    const bool isIdle = outboundQueue_.isEmpty()
                        && ! hasPendingSysEx_.load(std::memory_order_acquire);

    if (isIdle)
    {
        if (settleMs <= 0)
        {
            sendArmedSinglePatchRequest(patchNumber, token);
            return;
        }

        juce::Timer::callAfterDelay(settleMs,
                                    [this, patchNumber, token]
                                    {
                                        sendArmedSinglePatchRequest(patchNumber, token);
                                    });
        return;
    }

    if (juce::Time::getMillisecondCounter() - idleStartMs
        >= static_cast<juce::uint32>(outboundIdleTimeoutMs))
    {
        MidiLogger::getInstance().logWarning("Timeout waiting for outbound MIDI queue to go idle");
        updateErrorState("Timeout waiting for outbound MIDI queue to go idle", "Timeout");
        finishAsyncPackedPatch(token, {});
        return;
    }

    wakeConsumer();
    juce::Timer::callAfterDelay(1,
                                [this, patchNumber, token, settleMs, idleStartMs, outboundIdleTimeoutMs]
                                {
                                    pollOutboundIdleThenRequest(patchNumber,
                                                                token,
                                                                settleMs,
                                                                idleStartMs,
                                                                outboundIdleTimeoutMs);
                                });
}

std::vector<juce::uint8> MidiManager::requestMasterData()
{
    return requestSysExData(SysExConstants::RequestType::kRequestMasterParameters,
                           SysExConstants::kMasterPackedDataSize,
                           "master");
}

bool MidiManager::isDeviceDumpAvailable() const
{
    return midiSender != nullptr
        && midiSender->isOutputAvailable()
        && midiReceiver != nullptr
        && midiReceiver->isInputAvailable();
}

bool MidiManager::waitUntilOutboundQueueIdle(int timeoutMs)
{
    const auto isIdle = [this]
    {
        return outboundQueue_.isEmpty() && ! hasPendingSysEx_.load(std::memory_order_acquire);
    };

    const auto startMs = juce::Time::getMillisecondCounter();
    const auto deadlineMs = startMs + static_cast<juce::uint32>(juce::jmax(0, timeoutMs));

    wakeConsumer();

    while (! isIdle())
    {
        if (juce::Time::getMillisecondCounter() >= deadlineMs)
            return isIdle();

        wakeConsumer();
        juce::Thread::sleep(1);
    }

    return true;
}

void MidiManager::clearDeviceDetectionAfterPortLoss()
{
    const bool hadInquiryPair = lastInquiryInputId_.isNotEmpty() || lastInquiryOutputId_.isNotEmpty();
    const bool wasDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));

    cancelPendingSysExRequest();
    clearLastInquiryPortPair();

    if (! wasDetected && ! hadInquiryPair)
        return;

    sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
    updateDeviceStatus(false);
}

void MidiManager::clearLastInquiryPortPair() noexcept
{
    lastInquiryInputId_.clear();
    lastInquiryOutputId_.clear();
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

    if (! Core::shouldStartDeviceInquiry(true,
                                         inputId,
                                         outputId,
                                         lastInquiryInputId_,
                                         lastInquiryOutputId_))
        return;

    lastInquiryInputId_ = inputId;
    lastInquiryOutputId_ = outputId;
    performDeviceInquiry();
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

    midiReceiver->armOneShotSysExCapture(
        [this, token](const juce::MemoryBlock& response)
        {
            juce::MessageManager::callAsync(
                [this, token, response]
                {
                    if (token != asyncRequestToken_.load(std::memory_order_acquire))
                        return;

                    if (sysExParser == nullptr)
                    {
                        finishAsyncDeviceInquiryFailure(token, "SysEx parser unavailable", "SysEx");
                        return;
                    }

                    const auto validation = sysExParser->validateSysEx(response);
                    if (! validation.isValid
                        || validation.messageType != SysExParser::MessageType::kDeviceId)
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

                    finishAsyncDeviceInquiryFailure(
                        token,
                        "Connected device is not a supported Oberheim Matrix synth",
                        "Device");
                });
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

    sysExDelay_.setProfile(Core::SysExDelayProfile::fromDeviceInquiry(info));
    updateDeviceStatus(true, info.version, deviceType);
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

    clearLastInquiryPortPair();
    sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
    updateDeviceStatus(false);
    updateErrorState(errorMessage, errorType);
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

        juce::Timer::callAfterDelay(
            SysExConstants::kDefaultTimeoutMs,
            [this, token]
            {
                if (token != asyncRequestToken_.load(std::memory_order_acquire))
                    return;

                MidiLogger::getInstance().logWarning(
                    "Timeout waiting for SysEx response ("
                    + juce::String(SysExConstants::kDefaultTimeoutMs) + "ms)");
                finishAsyncDeviceInquiryFailure(token,
                                                "Timeout waiting for Device ID response",
                                                "Timeout");
            });
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

void MidiManager::pollOutboundIdleThenDeviceInquiry(std::uint64_t token,
                                                    int settleMs,
                                                    juce::uint32 idleStartMs,
                                                    int outboundIdleTimeoutMs)
{
    if (token != asyncRequestToken_.load(std::memory_order_acquire))
        return;

    const bool isIdle = outboundQueue_.isEmpty()
                        && ! hasPendingSysEx_.load(std::memory_order_acquire);

    if (isIdle)
    {
        if (settleMs <= 0)
        {
            sendArmedDeviceInquiry(token);
            return;
        }

        juce::Timer::callAfterDelay(settleMs,
                                    [this, token]
                                    {
                                        sendArmedDeviceInquiry(token);
                                    });
        return;
    }

    if (juce::Time::getMillisecondCounter() - idleStartMs
        >= static_cast<juce::uint32>(outboundIdleTimeoutMs))
    {
        MidiLogger::getInstance().logWarning("Timeout waiting for outbound MIDI queue to go idle");
        finishAsyncDeviceInquiryFailure(token,
                                        "Timeout waiting for outbound MIDI queue to go idle",
                                        "Timeout");
        return;
    }

    wakeConsumer();
    juce::Timer::callAfterDelay(1,
                                [this, token, settleMs, idleStartMs, outboundIdleTimeoutMs]
                                {
                                    pollOutboundIdleThenDeviceInquiry(token,
                                                                      settleMs,
                                                                      idleStartMs,
                                                                      outboundIdleTimeoutMs);
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

    const auto token = asyncRequestToken_.load(std::memory_order_acquire);
    wakeConsumer();
    pollOutboundIdleThenDeviceInquiry(token,
                                      50,
                                      juce::Time::getMillisecondCounter(),
                                      500);
}

std::vector<juce::uint8> MidiManager::requestSysExData(juce::uint8 requestType,
                                                       size_t expectedPackedSize,
                                                       const juce::String& requestDescription,
                                                       juce::uint8 patchNumber)
{
    try
    {
        auto requestMessage = sysExEncoder->encodeRequestMessage(requestType, patchNumber);
        sendSysExWithDelay(requestMessage, requestDescription + " request");

        auto response = midiReceiver->waitForSysExResponse(SysExConstants::kDefaultTimeoutMs);
        
        if (response.getSize() == 0)
        {
            updateErrorState("Timeout waiting for " + requestDescription + " response", "Timeout");
            return {};
        }

        MidiLogger::getInstance().logSysExReceived(response, requestDescription + " response");

        std::vector<juce::uint8> packedData(expectedPackedSize);
        bool decodeSuccess = false;
        
        if (requestType == SysExConstants::RequestType::kRequestEditBuffer
            || requestType == SysExConstants::RequestType::kRequestSinglePatch)
        {
            decodeSuccess = sysExDecoder->decodePatchSysEx(response, packedData.data());
        }
        else if (requestType == SysExConstants::RequestType::kRequestMasterParameters)
        {
            decodeSuccess = sysExDecoder->decodeMasterSysEx(response, packedData.data());
        }
        
        if (decodeSuccess)
        {
            return packedData;
        }
        else
        {
            updateErrorState("Failed to decode " + requestDescription + " response", "SysEx");
            return {};
        }
    }
    catch (const MidiConnectionException& e)
    {
        updateErrorState(e.getMessage(), "Connection");
        return {};
    }
    catch (const std::exception& e)
    {
        updateErrorState(e.what(), "SysEx");
        return {};
    }
}

void MidiManager::wakeConsumer() noexcept
{
    notify();
}

bool MidiManager::canSendSysExNow() const noexcept
{
    const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
    return sysExDelay_.millisUntilNextAllowed(nowMs) == 0;
}

void MidiManager::sendQueuedSysEx(const juce::MemoryBlock& sysExMessage, const juce::String& description)
{
    midiSender->sendSysEx(sysExMessage);
    MidiLogger::getInstance().logSysExSent(sysExMessage, description);
    sysExDelay_.recordSysExSent(static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes()));
}

void MidiManager::dispatchRealtimeMessage(const Core::MidiOutboundQueue::Message& msg)
{
    midiSender->sendMidiMessage(msg.midiMessage);
    activityTracker_.notifyActivity(pathForOutboundMessage(msg));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
}

bool MidiManager::processOutboundQueue()
{
    bool didWork = false;

    while (true)
    {
        auto msg = outboundQueue_.dequeue();
        if (!msg.has_value())
            break;

        didWork = true;

        try
        {
            if (msg->category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
            {
                dispatchRealtimeMessage(*msg);
                continue;
            }

            if (msg->sysExData.getSize() == 0)
                continue;

            if (pendingSysEx_.has_value())
            {
                outboundQueue_.enqueueSysEx(std::move(msg->sysExData));
                break;
            }

            if (canSendSysExNow())
            {
                sendQueuedSysEx(msg->sysExData, "QUEUED");
                activityTracker_.notifyActivity(pathForOutboundMessage(*msg));
                activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
                continue;
            }

            pendingSysEx_ = std::move(*msg);
            break;
        }
        catch (const MidiConnectionException& e)
        {
            updateErrorState(e.getMessage(), "Connection");
            break;
        }
        catch (const std::exception& e)
        {
            updateErrorState(e.what(), "MIDI");
            break;
        }
    }

    if (pendingSysEx_.has_value() && canSendSysExNow())
    {
        try
        {
            sendQueuedSysEx(pendingSysEx_->sysExData, "QUEUED");
            activityTracker_.notifyActivity(pathForOutboundMessage(*pendingSysEx_));
            activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
            pendingSysEx_.reset();
            didWork = true;
        }
        catch (const MidiConnectionException& e)
        {
            updateErrorState(e.getMessage(), "Connection");
        }
        catch (const std::exception& e)
        {
            updateErrorState(e.what(), "MIDI");
        }
    }

    hasPendingSysEx_.store(pendingSysEx_.has_value(), std::memory_order_release);

    return didWork;
}

void MidiManager::run()
{
    while (!threadShouldExit())
    {
        if (!midiSender->isOutputAvailable())
        {
            wait(1);
            continue;
        }

        if (processOutboundQueue())
            continue;

        int sleepMs = 1;
        if (pendingSysEx_.has_value())
        {
            const auto nowMs = static_cast<juce::int64>(juce::Time::getMillisecondCounterHiRes());
            sleepMs = juce::jmax(1, sysExDelay_.millisUntilNextAllowed(nowMs));
        }

        wait(sleepMs);
    }
}

void MidiManager::updateErrorState(const juce::String& errorMessage, const juce::String& errorType)
{
    apvts.state.setProperty("lastError", errorMessage, nullptr);
    apvts.state.setProperty("errorType", errorType, nullptr);
    
    MidiLogger::getInstance().logError(errorMessage);
}

void MidiManager::updateDeviceStatus(bool detected,
                                     const juce::String& version,
                                     MatrixDeviceTypes::Type deviceType)
{
    apvts.state.setProperty("deviceDetected", detected, nullptr);
    apvts.state.setProperty("deviceVersion", version, nullptr);

    if (detected)
    {
        apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                MatrixDeviceTypes::toApvtsString(deviceType),
                                nullptr);
        MidiLogger::getInstance().logInfo(
            "Matrix synth detected (" + MatrixDeviceTypes::toApvtsString(deviceType)
            + "). Version: " + version);
    }
    else
    {
        apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kUnknown),
                                nullptr);
        MidiLogger::getInstance().logWarning("Matrix synth not detected");
    }
}

void MidiManager::stopMidiInputCallbacks()
{
    if (inputMidiPort == nullptr || midiReceiver == nullptr)
    {
        return;
    }
    
    auto* midiInput = inputMidiPort->getMidiInput();
    if (midiInput != nullptr)
    {
        midiInput->stop();
        midiReceiver->setMidiInput(nullptr);
        std::this_thread::sleep_for(std::chrono::milliseconds(SysExConstants::kMidiInputStopDelayMs));
    }
    inputMidiPort->closePort();
}

void MidiManager::handleIncomingSysEx(const juce::MemoryBlock& sysEx)
{
    auto validation = sysExParser->validateSysEx(sysEx);
    
    if (!validation.isValid)
    {
        updateErrorState(validation.errorMessage, "SysEx");
        return;
    }

    switch (validation.messageType)
    {
        case SysExParser::MessageType::kPatch:
        case SysExParser::MessageType::kMaster:
        case SysExParser::MessageType::kDeviceId:
        case SysExParser::MessageType::kUnknown:
        case SysExParser::MessageType::kSplitPatch:
        default:
            break;
    }
}

