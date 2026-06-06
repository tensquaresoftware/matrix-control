#include <thread>
#include <chrono>

#include "MidiInputPort.h"
#include "Core/Loggers/MidiLogger.h"

MidiInputPort::MidiInputPort()
    : portIsOpen(false)
{
}

MidiInputPort::~MidiInputPort()
{
    closePort();
}

Core::MidiPortOpenResult MidiInputPort::openPort(const juce::String& deviceId,
                                                 juce::MidiInputCallback* callback)
{
    Core::MidiPortOpenResult result;
    closePort();

    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logWarning("MidiInputPort::openPort: empty device ID");
        result.failureReason = Core::MidiPortOpenFailureReason::kNotFound;
        return result;
    }

    const auto devices = juce::MidiInput::getAvailableDevices();
    for (const auto& device : devices)
    {
        if (device.identifier != deviceId)
            continue;

        result.portDisplayName = device.name;
        midiInput = juce::MidiInput::openDevice(device.identifier, callback);
        if (midiInput != nullptr)
        {
            if (callback != nullptr)
                midiInput->start();

            portIsOpen = true;
            MidiLogger::getInstance().logInfo("MIDI input port opened: [" + device.name + "]");
            return result;
        }

        result.failureReason = Core::MidiPortOpenFailureReason::kOpenRejected;
        return result;
    }

    result.portDisplayName = deviceId;
    result.failureReason = Core::MidiPortOpenFailureReason::kNotFound;
    return result;
}

void MidiInputPort::closePort()
{
    if (midiInput != nullptr)
    {
        midiInput->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        MidiLogger::getInstance().logInfo("MIDI input port closed");
        midiInput.reset();
    }
    portIsOpen = false;
}

bool MidiInputPort::isOpen() const noexcept
{
    return portIsOpen && midiInput != nullptr;
}

juce::MidiInput* MidiInputPort::getMidiInput() const noexcept
{
    return midiInput.get();
}
