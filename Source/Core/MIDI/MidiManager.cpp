#include "MidiManager.h"

#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/Loggers/MidiLogger.h"
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
    apvts.state.setProperty(PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties::kBankLock,
                            false,
                            nullptr);
    apvts.state.setProperty("lastError", juce::String(), nullptr);
    apvts.state.setProperty("errorType", juce::String(), nullptr);
    apvts.state.setProperty("lastPatchLoaded", juce::String(), nullptr);
    
    MidiLogger::getInstance().logInfo("MidiManager initialized");
}

MidiManager::~MidiManager()
{
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

std::vector<juce::uint8> MidiManager::requestMasterData()
{
    return requestSysExData(SysExConstants::RequestType::kRequestMasterParameters,
                           SysExConstants::kMasterPackedDataSize,
                           "master");
}

bool MidiManager::performDeviceInquiry()
{
    try
    {
        auto inquiryMessage = SysExEncoder::encodeDeviceInquiry();
        sendSysExWithDelay(inquiryMessage, "Device Inquiry");

        auto response = midiReceiver->waitForSysExResponse(SysExConstants::kDefaultTimeoutMs);
        
        if (response.getSize() == 0)
        {
            sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
            updateDeviceStatus(false);
            updateErrorState("Timeout waiting for Device ID response", "Timeout");
            return false;
        }

        MidiLogger::getInstance().logSysExReceived(response, "Device ID response");

        DeviceIdInfo deviceInfo = sysExDecoder->decodeDeviceId(response);
        
        if (deviceInfo.isValid)
        {
            const auto deviceType = Core::DeviceTypeRegistry::fromDeviceInquiry(deviceInfo);
            sysExDelay_.setProfile(Core::SysExDelayProfile::fromDeviceInquiry(deviceInfo));
            updateDeviceStatus(true, deviceInfo.version, deviceType);
            return true;
        }
        else
        {
            sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
            updateDeviceStatus(false);
            updateErrorState("Connected device is not a supported Oberheim Matrix synth", "Device");
            return false;
        }
    }
    catch (const MidiConnectionException& e)
    {
        sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
        updateDeviceStatus(false);
        updateErrorState(e.getMessage(), "Connection");
        return false;
    }
    catch (const std::exception& e)
    {
        sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
        updateDeviceStatus(false);
        updateErrorState(e.what(), "SysEx");
        return false;
    }
}

std::vector<juce::uint8> MidiManager::requestSysExData(juce::uint8 requestType, size_t expectedPackedSize, 
                                                    const juce::String& requestDescription)
{
    try
    {
        auto requestMessage = sysExEncoder->encodeRequestMessage(requestType, 0);
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
        
        if (requestType == SysExConstants::RequestType::kRequestEditBuffer)
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

