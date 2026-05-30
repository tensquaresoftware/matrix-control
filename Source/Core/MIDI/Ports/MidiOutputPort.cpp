#include "MidiOutputPort.h"
#include "Core/Loggers/MidiLogger.h"

MidiOutputPort::MidiOutputPort()
    : portIsOpen(false)
{
}

MidiOutputPort::~MidiOutputPort()
{
    closePort();
}

bool MidiOutputPort::openPort(const juce::String& deviceId)
{
    closePort();

    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logWarning("MidiOutputPort::openPort: empty device ID");
        return false;
    }

    auto devices = juce::MidiOutput::getAvailableDevices();
    for (const auto& device : devices)
    {
        if (device.identifier == deviceId)
        {
            midiOutput = juce::MidiOutput::openDevice(device.identifier);
            if (midiOutput != nullptr)
            {
                portIsOpen = true;
                MidiLogger::getInstance().logInfo("MIDI output port opened: [" + device.name + "]");
                return true;
            }
            else
            {
                MidiLogger::getInstance().logError("Failed to open MIDI output device: [" + device.name + "]");
            }
            break;
        }
    }

    MidiLogger::getInstance().logError("MIDI output device not found: " + deviceId);
    return false;
}

void MidiOutputPort::closePort()
{
    if (midiOutput != nullptr)
    {
        MidiLogger::getInstance().logInfo("MIDI output port closed");
        midiOutput.reset();
    }
    portIsOpen = false;
}

bool MidiOutputPort::isOpen() const noexcept
{
    return portIsOpen && midiOutput != nullptr;
}

juce::MidiOutput* MidiOutputPort::getMidiOutput() const noexcept
{
    return midiOutput.get();
}

