#pragma once

#include <memory>
#include <atomic>
#include <optional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Shared/Definitions/MatrixDeviceTypes.h"

#include "Ports/MidiInputPort.h"
#include "Ports/MidiOutputPort.h"
#include "Transport/MidiSender.h"
#include "Transport/MidiReceiver.h"
#include "SysEx/SysExParser.h"
#include "SysEx/SysExDecoder.h"
#include "SysEx/SysExEncoder.h"
#include "Exceptions/Exceptions.h"
#include "SysEx/SysExConstants.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/Queue/SysExInterMessageDelay.h"

class MidiManager : public juce::Thread
{
public:
    explicit MidiManager(juce::AudioProcessorValueTreeState& apvtsRef,
                         Core::MidiOutboundQueue& outboundQueueRef,
                         Core::MidiActivityTracker& activityTrackerRef);
    ~MidiManager() override;

    bool setMidiInputPort(const juce::String& deviceId, bool reportOpenFailure = true);
    bool setMidiOutputPort(const juce::String& deviceId, bool reportOpenFailure = true);

    bool isInputPortOpenWithDevice(const juce::String& deviceId) const;
    bool isOutputPortOpenWithDevice(const juce::String& deviceId) const;

    void sendPatch(juce::uint8 patchNumber, const juce::uint8* packedData);
    void sendMaster(juce::uint8 version, const juce::uint8* packedData);
    void sendProgramChange(int programNumber, int channel = 1);
    void sendSetBank(int bank);
    void sendUnlockBank();
    void enqueueRemoteParameterEdit(int parameterNumber, juce::uint8 packedValue);
    void enqueueMatrixModBusEdit(juce::uint8 bus,
                                 juce::uint8 source,
                                 juce::uint8 amount,
                                 juce::uint8 destination);

    std::vector<juce::uint8> requestCurrentPatch();
    std::vector<juce::uint8> requestMasterData();
    
    bool performDeviceInquiry();
    void run() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    std::unique_ptr<MidiInputPort> inputMidiPort;
    std::unique_ptr<MidiOutputPort> outputMidiPort;
    std::unique_ptr<MidiSender> midiSender;
    std::unique_ptr<MidiReceiver> midiReceiver;
    std::unique_ptr<SysExParser> sysExParser;
    std::unique_ptr<SysExDecoder> sysExDecoder;
    std::unique_ptr<SysExEncoder> sysExEncoder;
    Core::MidiOutboundQueue& outboundQueue_;
    Core::MidiActivityTracker& activityTracker_;
    Core::EditorPath editorPath_;
    Core::SysExInterMessageDelay sysExDelay_;

    void updateErrorState(const juce::String& errorMessage, const juce::String& errorType);
    void updateDeviceStatus(bool detected,
                            const juce::String& version = {},
                            MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown);
    void handleIncomingSysEx(const juce::MemoryBlock& sysEx);
    
    void stopMidiInputCallbacks();
    void wakeConsumer() noexcept;
    bool processOutboundQueue();
    bool canSendSysExNow() const noexcept;
    void sendQueuedSysEx(const juce::MemoryBlock& sysExMessage, const juce::String& description);
    void dispatchRealtimeMessage(const Core::MidiOutboundQueue::Message& msg);
    void sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description);

    std::optional<Core::MidiOutboundQueue::Message> pendingSysEx_;
    std::vector<juce::uint8> requestSysExData(juce::uint8 requestType, size_t expectedPackedSize, 
                                         const juce::String& requestDescription);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiManager)
};

