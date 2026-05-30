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

bool MidiInputPort::openPort(const juce::String& deviceId, juce::MidiInputCallback* callback)
{
    closePort();

    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logWarning("MidiInputPort::openPort: empty device ID");
        return false;
    }

    auto devices = juce::MidiInput::getAvailableDevices();
    for (const auto& device : devices)
    {
        if (device.identifier == deviceId)
        {
            midiInput = juce::MidiInput::openDevice(device.identifier, callback);
            if (midiInput != nullptr)
            {
                if (callback != nullptr)
                {
                    midiInput->start();
                }
                portIsOpen = true;
                MidiLogger::getInstance().logInfo("MIDI input port opened: [" + device.name + "]");
                return true;
            }
            else
            {
                MidiLogger::getInstance().logError("Failed to open MIDI input device: [" + device.name + "]");
            }
            break;
        }
    }

    MidiLogger::getInstance().logError("MIDI input device not found: " + deviceId);
    return false;
}

void MidiInputPort::closePort()
{
    if (midiInput != nullptr)
    {
        midiInput->stop();
        // Give time for any in-flight callbacks to complete
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

