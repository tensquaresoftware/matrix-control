#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Shared/Definitions/Matrix1000Limits.h"

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

    apvts.state.setProperty("deviceDetected", false, nullptr);
    apvts.state.setProperty("deviceVersion", juce::String(), nullptr);
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

bool MidiManager::setMidiInputPort(const juce::String& deviceId)
{
    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logInfo("Clearing MIDI input port selection");
        stopMidiInputCallbacks();
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

    if (inputMidiPort->openPort(deviceId, midiReceiver.get()))
    {
        midiReceiver->setMidiInput(inputMidiPort->getMidiInput());
        MidiLogger::getInstance().logInfo("MIDI input port successfully set");
        return true;
    }

    MidiLogger::getInstance().logError("Failed to set MIDI input port");
    return false;
}

bool MidiManager::setMidiOutputPort(const juce::String& deviceId)
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

    if (outputMidiPort->openPort(deviceId))
    {
        midiSender->setMidiOutput(outputMidiPort->getMidiOutput());
        MidiLogger::getInstance().logInfo("MIDI output port successfully set");
        return true;
    }
    MidiLogger::getInstance().logError("Failed to set MIDI output port");
    return false;
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
            sysExDelay_.setProfile(Core::SysExDelayProfile::fromDeviceInquiry(deviceInfo));
            updateDeviceStatus(true, deviceInfo.version);
            return true;
        }
        else
        {
            sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
            updateDeviceStatus(false);
            updateErrorState("Connected device is not a Matrix-1000", "Device");
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

void MidiManager::run()
{
    while (!threadShouldExit())
    {
        if (!midiSender->isOutputAvailable())
        {
            wait(5);
            continue;
        }

        if (auto msg = outboundQueue_.dequeue())
        {
            dispatchOutboundMessage(*msg);
            continue;
        }

        wait(1);
    }
}

void MidiManager::dispatchOutboundMessage(const Core::MidiOutboundQueue::Message& msg)
{
    try
    {
        if (msg.category == Core::MidiOutboundQueue::MessageCategory::kRealtime)
        {
            midiSender->sendMidiMessage(msg.midiMessage);
            activityTracker_.notifyActivity(pathForOutboundMessage(msg));
            activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
            return;
        }

        if (msg.sysExData.getSize() == 0)
            return;

        sendSysExWithDelay(msg.sysExData, "QUEUED");
        activityTracker_.notifyActivity(pathForOutboundMessage(msg));
        activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);
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

void MidiManager::updateErrorState(const juce::String& errorMessage, const juce::String& errorType)
{
    apvts.state.setProperty("lastError", errorMessage, nullptr);
    apvts.state.setProperty("errorType", errorType, nullptr);
    
    MidiLogger::getInstance().logError(errorMessage);
}

void MidiManager::updateDeviceStatus(bool detected, const juce::String& version)
{
    apvts.state.setProperty("deviceDetected", detected, nullptr);
    apvts.state.setProperty("deviceVersion", version, nullptr);
    
    if (detected)
    {
        MidiLogger::getInstance().logInfo("Matrix-1000 detected. Version: " + version);
    }
    else
    {
        MidiLogger::getInstance().logWarning("Matrix-1000 not detected");
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

