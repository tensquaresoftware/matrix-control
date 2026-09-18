#include "MidiManager.h"

#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MasterEditGate.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace
{
    void seedPatchManagerStateDefaults(juce::ValueTree& state)
    {
        namespace PatchManager = PluginIDs::PatchManagerSection;

        state.setProperty(PatchManager::BankUtilityModule::StateProperties::kSelectedBank,
                          Matrix1000Limits::kMinBankNumber,
                          nullptr);
        state.setProperty(PatchManager::StateProperties::kPatchCoordinatesEstablished, false, nullptr);
        state.setProperty(PatchManager::StateProperties::kNavigationFocus,
                          PatchManager::NavigationFocus::kDefault,
                          nullptr);
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
    apvts.state.setProperty(Core::kDeviceMidiUnresponsiveProperty, false, nullptr);
    apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                            MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kUnknown),
                            nullptr);
    apvts.state.setProperty("deviceVersion", juce::String(), nullptr);
    seedPatchManagerStateDefaults(apvts.state);
    apvts.state.setProperty("lastError", juce::String(), nullptr);
    apvts.state.setProperty("errorType", juce::String(), nullptr);
    apvts.state.setProperty("lastPatchLoaded", juce::String(), nullptr);

    MidiLogger::getInstance().logInfo("MidiManager initialized");

    devicePresenceTimer_ = std::make_unique<DevicePresenceTimer>(*this);
}

MidiManager::~MidiManager()
{
    // Queue outlives MidiManager briefly during PluginProcessor teardown — drop the wake
    // callback so a late enqueue cannot call into a destroyed consumer.
    outboundQueue_.setWakeConsumerCallback(nullptr);

    if (devicePresenceTimer_ != nullptr)
        devicePresenceTimer_->stopTimer();

    cancelPendingSysExRequest();
    stopThread(5000);
    stopMidiInputCallbacks();

    if (outputMidiPort != nullptr)
        outputMidiPort->closePort();
}

int MidiManager::getRequiredSysExDelayMs() const noexcept
{
    return sysExDelay_.getRequiredDelayMs();
}

void MidiManager::refreshSysExDelayFromSettings()
{
    const bool detected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    if (! detected)
    {
        sysExDelay_.setProfile(Core::SysExDelayProfile::stockDefault());
        return;
    }

    const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
        apvts.state.getProperty(PluginIDs::Settings::kEpromType,
                               PluginIDs::Settings::EpromType::kDefault)));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
    sysExDelay_.setProfile(Core::SysExDelayProfile::fromSettings(epromType, family));
}

void MidiManager::sendPatch(juce::uint8 patchNumber, const juce::uint8* packedData)
{
    if (! isEditorOutboundAllowed())
        return;

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
    if (! isEditorOutboundAllowed())
        return;

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

void MidiManager::sendStoreEditBuffer(juce::uint8 patchNumber, juce::uint8 bank, juce::uint8 unitId)
{
    if (! isEditorOutboundAllowed())
        return;

    try
    {
        auto sysExMessage = sysExEncoder->encodeStoreEditBuffer(patchNumber, bank, unitId);
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
    if (! isEditorOutboundAllowed())
        return;

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
    if (! isEditorOutboundAllowed())
        return;

    // FR-46 defense-in-depth: editor outbound alone is not enough for MASTER SysEx.
    if (! isMasterEditOutboundAllowed())
        return;

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
    if (! isEditorOutboundAllowed())
        return;

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

void MidiManager::sendPanic()
{
    const auto enqueuePanicTriple = [this](int channel)
    {
        // Front-insert is LIFO — push 121, then 123, then 120 so dequeue is 120 → 123 → 121.
        outboundQueue_.enqueueRealtimeFront(juce::MidiMessage::controllerEvent(channel, 121, 0));
        outboundQueue_.enqueueRealtimeFront(juce::MidiMessage::controllerEvent(channel, 123, 0));
        outboundQueue_.enqueueRealtimeFront(juce::MidiMessage::controllerEvent(channel, 120, 0));
    };
    int channel = 1;
    bool sendAllChannels = true;
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(
            apvts.getParameter(PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel)))
    {
        const int index = choice->getIndex();
        if (index >= 1 && index <= 16)
        {
            channel = index;
            sendAllChannels = false;
        }
        // Omni (0) and Mono groups: clear every channel — synth may be sounding on any.
    }

    if (sendAllChannels)
    {
        for (int ch = 16; ch >= 1; --ch)
            enqueuePanicTriple(ch);
    }
    else
    {
        enqueuePanicTriple(channel);
    }

    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kInstrument);
}

size_t MidiManager::getRealtimeOutboundDepth() const noexcept
{
    return outboundQueue_.realtimeDepth();
}

void MidiManager::sendSetBank(int bank)
{
    if (! isEditorOutboundAllowed())
        return;

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
    if (! isEditorOutboundAllowed())
        return;

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
    if (! isEditorOutboundAllowed())
        return;

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
    if (! isEditorOutboundAllowed())
        return;

    if (bus >= static_cast<juce::uint8>(Matrix1000Limits::kModulationBusCount))
        return;

    auto sysExMessage = sysExEncoder->encodeMatrixModBusEdit(bus, source, amount, destination);
    editorPath_.enqueueSysEx(sysExMessage);
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

bool MidiManager::isEditorOutboundAllowed() const
{
    const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const bool deviceMidiUnresponsive = static_cast<bool>(
        apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::maySendEditorProgramChange(deviceDetected, deviceType, deviceMidiUnresponsive);
}

bool MidiManager::isMasterEditOutboundAllowed() const
{
    const bool deviceDetected = static_cast<bool>(apvts.state.getProperty("deviceDetected", false));
    const bool deviceMidiUnresponsive = static_cast<bool>(
        apvts.state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    return Core::isMasterEditAllowed(deviceDetected, deviceType) && ! deviceMidiUnresponsive;
}

bool MidiManager::waitUntilOutboundQueueIdle(int timeoutMs)
{
    const auto startMs = juce::Time::getMillisecondCounter();
    const auto deadlineMs = startMs + static_cast<juce::uint32>(juce::jmax(0, timeoutMs));

    wakeConsumer();

    while (! isOutboundQueueIdle())
    {
        if (juce::Time::getMillisecondCounter() >= deadlineMs)
            return isOutboundQueueIdle();

        wakeConsumer();
        juce::Thread::sleep(1);
    }

    return true;
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
        setDeviceMidiUnresponsive(false);
        MidiLogger::getInstance().logInfo(
            "Matrix synth detected (" + MatrixDeviceTypes::toApvtsString(deviceType)
            + "). Version: " + version);
    }
    else
    {
        apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                MatrixDeviceTypes::toApvtsString(MatrixDeviceTypes::Type::kUnknown),
                                nullptr);
        setDeviceMidiUnresponsive(false);
        MidiLogger::getInstance().logWarning("Matrix synth not detected");
    }

    updateDevicePresenceMonitoring();
}

void MidiManager::handleIncomingSysEx(const juce::MemoryBlock& sysEx)
{
    auto validation = sysExParser->validateSysEx(sysEx);

    if (! validation.isValid)
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
