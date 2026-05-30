#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/Exceptions/Exceptions.h"
#include "Core/MIDI/SysEx/SysExConstants.h"

class MidiReceiver : public juce::MidiInputCallback
{
public:
    MidiReceiver();
    ~MidiReceiver() override;

    void setMidiInput(juce::MidiInput* midiInput);
    void handleIncomingMidiMessage(juce::MidiInput* source,
                                   const juce::MidiMessage& message) override;
    juce::MemoryBlock waitForSysExResponse(int timeoutMs = SysExConstants::kDefaultTimeoutMs);
    void reset();
    bool isInputAvailable() const noexcept;

private:
    juce::MidiInput* midiInput;
    std::atomic<bool> isDestroying;
    
    std::atomic<bool> isReceivingSysEx;
    std::vector<juce::uint8> sysExBuffer;
    std::mutex bufferMutex;
    
    std::atomic<bool> responseReceived;
    juce::MemoryBlock receivedSysEx;
    std::mutex responseMutex;

    void processCompleteSysEx(const juce::MemoryBlock& completeSysEx);
    bool checkIfResponseReceived();
    juce::MemoryBlock getReceivedSysEx();
    bool hasTimeoutElapsed(const std::chrono::steady_clock::time_point& startTime, int timeoutMs) const;
    void logTimeoutAndReset(int timeoutMs);
    void sleepToAvoidBusyWaiting();
    void storeReceivedSysExAndNotify(const juce::MemoryBlock& completeSysEx);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiReceiver)
};

