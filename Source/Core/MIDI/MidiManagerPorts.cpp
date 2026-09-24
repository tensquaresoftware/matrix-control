// Extracted from MidiManager.cpp for modular maintenance.
// MIDI input/output port open, reuse, clear and stop paths.

#include "MidiManager.h"
#include "MidiManagerInternal.h"

#include "Core/Loggers/MidiLogger.h"

#include <chrono>
#include <thread>

using MidiManagerInternal::PortOpenFailureArgs;
using MidiManagerInternal::clearFooterThenReassertDeviceLockGuidance;
using MidiManagerInternal::midiInputDeviceNameForId;
using MidiManagerInternal::midiOutputDeviceNameForId;
using MidiManagerInternal::reportPortOpenFailure;

bool MidiManager::setMidiInputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logInfo("Clearing MIDI input port selection");
        stopMidiInputCallbacks();
        clearFooterThenReassertDeviceLockGuidance(apvts);
        return true;
    }

    if (inputMidiPort != nullptr && inputMidiPort->isOpenWithDevice(deviceId))
    {
        if (midiReceiver != nullptr)
            midiReceiver->setMidiInput(inputMidiPort->getMidiInput());

        clearFooterThenReassertDeviceLockGuidance(apvts);
        return true;
    }

    return openMidiInputPort(deviceId, reportOpenFailure);
}

bool MidiManager::openMidiInputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    MidiLogger::getInstance().logInfo(
        "Setting MIDI input port: [" + midiInputDeviceNameForId(deviceId) + "]");

    if (midiReceiver != nullptr)
        midiReceiver->setMidiInput(nullptr);

    const auto openResult = inputMidiPort->openPort(deviceId, midiReceiver.get());
    if (! openResult.succeeded())
    {
        reportPortOpenFailure(apvts, PortOpenFailureArgs{ true, openResult, deviceId, reportOpenFailure });
        return false;
    }

    midiReceiver->setMidiInput(inputMidiPort->getMidiInput());
    clearFooterThenReassertDeviceLockGuidance(apvts);
    MidiLogger::getInstance().logInfo("MIDI input port successfully set");
    return true;
}

bool MidiManager::setMidiOutputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logInfo("Clearing MIDI output port selection");
        if (midiSender != nullptr)
            midiSender->setMidiOutput(nullptr);
        if (outputMidiPort != nullptr)
            outputMidiPort->closePort();
        clearFooterThenReassertDeviceLockGuidance(apvts);
        return true;
    }

    if (outputMidiPort != nullptr && outputMidiPort->isOpenWithDevice(deviceId))
    {
        if (midiSender != nullptr)
            midiSender->setMidiOutput(outputMidiPort->getMidiOutput());

        clearFooterThenReassertDeviceLockGuidance(apvts);
        return true;
    }

    return openMidiOutputPort(deviceId, reportOpenFailure);
}

bool MidiManager::openMidiOutputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    MidiLogger::getInstance().logInfo(
        "Setting MIDI output port: [" + midiOutputDeviceNameForId(deviceId) + "]");

    if (midiSender != nullptr)
        midiSender->setMidiOutput(nullptr);

    const auto openResult = outputMidiPort->openPort(deviceId);
    if (! openResult.succeeded())
    {
        reportPortOpenFailure(apvts, PortOpenFailureArgs{ false, openResult, deviceId, reportOpenFailure });
        return false;
    }

    midiSender->setMidiOutput(outputMidiPort->getMidiOutput());
    clearFooterThenReassertDeviceLockGuidance(apvts);
    MidiLogger::getInstance().logInfo("MIDI output port successfully set");
    wakeConsumer();
    return true;
}

bool MidiManager::forceReopenInputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    if (deviceId.isEmpty())
    {
        stopMidiInputCallbacks();
        clearFooterThenReassertDeviceLockGuidance(apvts);
        return true;
    }

    if (midiReceiver != nullptr)
        midiReceiver->setMidiInput(nullptr);

    if (inputMidiPort != nullptr)
        inputMidiPort->closePort();

    return openMidiInputPort(deviceId, reportOpenFailure);
}

bool MidiManager::forceReopenOutputPort(const juce::String& deviceId, bool reportOpenFailure)
{
    if (deviceId.isEmpty())
    {
        if (midiSender != nullptr)
            midiSender->setMidiOutput(nullptr);
        if (outputMidiPort != nullptr)
            outputMidiPort->closePort();
        clearFooterThenReassertDeviceLockGuidance(apvts);
        return true;
    }

    if (midiSender != nullptr)
        midiSender->setMidiOutput(nullptr);

    if (outputMidiPort != nullptr)
        outputMidiPort->closePort();

    return openMidiOutputPort(deviceId, reportOpenFailure);
}

bool MidiManager::isInputPortOpenWithDevice(const juce::String& deviceId) const
{
    return inputMidiPort != nullptr && inputMidiPort->isOpenWithDevice(deviceId);
}

bool MidiManager::isOutputPortOpenWithDevice(const juce::String& deviceId) const
{
    return outputMidiPort != nullptr && outputMidiPort->isOpenWithDevice(deviceId);
}

juce::String MidiManager::getOpenInputDeviceId() const
{
    return inputMidiPort != nullptr ? inputMidiPort->getOpenDeviceId() : juce::String();
}

juce::String MidiManager::getOpenOutputDeviceId() const
{
    return outputMidiPort != nullptr ? outputMidiPort->getOpenDeviceId() : juce::String();
}

void MidiManager::stopMidiInputCallbacks()
{
    if (inputMidiPort == nullptr || midiReceiver == nullptr)
        return;

    auto* midiInput = inputMidiPort->getMidiInput();
    if (midiInput != nullptr)
    {
        midiInput->stop();
        midiReceiver->setMidiInput(nullptr);
        std::this_thread::sleep_for(std::chrono::milliseconds(SysExConstants::kMidiInputStopDelayMs));
    }
    inputMidiPort->closePort();
}
