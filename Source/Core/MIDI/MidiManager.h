#pragma once

#include <memory>
#include <atomic>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Ports/MidiInputPort.h"
#include "Ports/MidiOutputPort.h"
#include "Transport/MidiSender.h"
#include "Transport/MidiReceiver.h"
#include "SysEx/SysExParser.h"
#include "SysEx/SysExDecoder.h"
#include "SysEx/SysExEncoder.h"
#include "Exceptions/Exceptions.h"
#include "SysEx/SysExConstants.h"

class MidiManager : public juce::Thread
{
public:
    explicit MidiManager(juce::AudioProcessorValueTreeState& apvtsRef);
    ~MidiManager() override;

    bool setMidiInputPort(const juce::String& deviceId);
    bool setMidiOutputPort(const juce::String& deviceId);

    void sendPatch(juce::uint8 patchNumber, const juce::uint8* packedData);
    void sendMaster(juce::uint8 version, const juce::uint8* packedData);
    void sendProgramChange(int programNumber, int channel = 1);

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

    void updateErrorState(const juce::String& errorMessage, const juce::String& errorType);
    void updateDeviceStatus(bool detected, const juce::String& version = {});
    void handleIncomingSysEx(const juce::MemoryBlock& sysEx);
    
    void stopMidiInputCallbacks();
    void sendSysExWithDelay(const juce::MemoryBlock& sysExMessage, const juce::String& description);
    std::vector<juce::uint8> requestSysExData(juce::uint8 requestType, size_t expectedPackedSize, 
                                         const juce::String& requestDescription);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiManager)
};

