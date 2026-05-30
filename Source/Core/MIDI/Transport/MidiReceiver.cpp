#include <thread>

#include "MidiReceiver.h"

#include "Core/Loggers/MidiLogger.h"

MidiReceiver::MidiReceiver()
    : midiInput(nullptr)
    , isDestroying(false)
    , isReceivingSysEx(false)
    , responseReceived(false)
{
}

MidiReceiver::~MidiReceiver()
{
    // Mark as destroying to prevent callbacks from accessing members
    isDestroying = true;
    
    // Stop MIDI input to prevent new callbacks
    if (midiInput != nullptr)
    {
        midiInput->stop();
    }
    
    reset();
}

void MidiReceiver::setMidiInput(juce::MidiInput* input)
{
    if (midiInput != nullptr)
    {
        midiInput->stop();
    }

    midiInput = input;

    // Note: The callback must be set when opening the port via MidiInputPort::openPort()
    // The MidiInput is already started if callback was provided during opening
}

void MidiReceiver::handleIncomingMidiMessage(juce::MidiInput* source,
                                             const juce::MidiMessage& message)
{
    juce::ignoreUnused(source);

    // Safety check: ensure we're not being destroyed and still valid
    if (isDestroying.load() || midiInput == nullptr)
    {
        return;
    }

    // Log all incoming MIDI messages for debugging
    MidiLogger::getInstance().logInfo("MIDI message received: " + 
                                       juce::String(message.getRawDataSize()) + " bytes");

    if (message.isSysEx())
    {
        const juce::uint8* sysExData = message.getSysExData();
        int sysExSize = message.getSysExDataSize();

        MidiLogger::getInstance().logInfo("SysEx message detected: " + 
                                          juce::String(sysExSize) + " bytes");

        if (sysExSize > 0 && sysExData != nullptr)
        {
            juce::MemoryBlock completeSysEx(sysExData, static_cast<size_t>(sysExSize));
            MidiLogger::getInstance().logSysExReceived(completeSysEx);
            processCompleteSysEx(completeSysEx);
        }
        else
        {
            MidiLogger::getInstance().logWarning("SysEx message has null data or zero size");
        }
    }
    else if (message.isProgramChange())
    {
        MidiLogger::getInstance().logProgramChange(
            static_cast<juce::uint8>(message.getProgramChangeNumber()), "RECEIVED");
    }
    else
    {
        // Log other MIDI message types for debugging
        juce::String msgType = "Unknown";
        if (message.isNoteOn()) msgType = "NoteOn";
        else if (message.isNoteOff()) msgType = "NoteOff";
        else if (message.isController()) msgType = "CC";
        else if (message.isPitchWheel()) msgType = "PitchWheel";
        else if (message.isAftertouch()) msgType = "Aftertouch";
        
        MidiLogger::getInstance().logInfo("MIDI message type: " + msgType + 
                                          ", channel: " + juce::String(message.getChannel()));
    }
}

juce::MemoryBlock MidiReceiver::waitForSysExResponse(int timeoutMs)
{
    reset();

    auto startTime = std::chrono::steady_clock::now();
    
    while (true)
    {
        if (checkIfResponseReceived())
        {
            return getReceivedSysEx();
        }

        if (hasTimeoutElapsed(startTime, timeoutMs))
        {
            logTimeoutAndReset(timeoutMs);
            return {};
        }

        sleepToAvoidBusyWaiting();
    }
}

void MidiReceiver::reset()
{
    std::lock_guard<std::mutex> bufferLock(bufferMutex);
    std::lock_guard<std::mutex> responseLock(responseMutex);
    
    isReceivingSysEx = false;
    sysExBuffer.clear();
    responseReceived = false;
    receivedSysEx.reset();
}

bool MidiReceiver::isInputAvailable() const noexcept
{
    return midiInput != nullptr;
}

void MidiReceiver::processCompleteSysEx(const juce::MemoryBlock& completeSysEx)
{
    if (isDestroying.load())
        return;
    
    storeReceivedSysExAndNotify(completeSysEx);
}

bool MidiReceiver::checkIfResponseReceived()
{
    std::lock_guard<std::mutex> lock(responseMutex);
    if (responseReceived.load())
    {
        responseReceived = false;
        return true;
    }
    return false;
}

juce::MemoryBlock MidiReceiver::getReceivedSysEx()
{
    std::lock_guard<std::mutex> lock(responseMutex);
    return receivedSysEx;
}

bool MidiReceiver::hasTimeoutElapsed(
    const std::chrono::steady_clock::time_point& startTime,
    int timeoutMs) const
{
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - startTime).count();
    return elapsed >= timeoutMs;
}

void MidiReceiver::logTimeoutAndReset(int timeoutMs)
{
    MidiLogger::getInstance().logWarning("Timeout waiting for SysEx response (" + 
                                          juce::String(timeoutMs) + "ms)");
    reset();
}

void MidiReceiver::sleepToAvoidBusyWaiting()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void MidiReceiver::storeReceivedSysExAndNotify(const juce::MemoryBlock& completeSysEx)
{
    std::lock_guard<std::mutex> lock(responseMutex);
    if (!isDestroying.load())
    {
        receivedSysEx = completeSysEx;
        responseReceived = true;
    }
}

