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

Core::MidiPortOpenResult MidiOutputPort::openPort(const juce::String& deviceId)
{
    Core::MidiPortOpenResult result;
    if (deviceId.isEmpty())
    {
        MidiLogger::getInstance().logWarning("MidiOutputPort::openPort: empty device ID");
        result.failureReason = Core::MidiPortOpenFailureReason::kNotFound;
        return result;
    }

    if (isOpenWithDevice(deviceId) && midiOutput != nullptr)
    {
        const auto devices = juce::MidiOutput::getAvailableDevices();
        for (const auto& device : devices)
        {
            if (device.identifier == deviceId)
            {
                result.portDisplayName = device.name;
                break;
            }
        }

        return result;
    }

    closePort();

    const auto devices = juce::MidiOutput::getAvailableDevices();
    for (const auto& device : devices)
    {
        if (device.identifier != deviceId)
            continue;

        result.portDisplayName = device.name;
        midiOutput = juce::MidiOutput::openDevice(device.identifier);
        if (midiOutput != nullptr)
        {
            portIsOpen = true;
            openDeviceId_ = deviceId;
            MidiLogger::getInstance().logInfo("MIDI output port opened: [" + device.name + "]");
            return result;
        }

        result.failureReason = Core::MidiPortOpenFailureReason::kOpenRejected;
        return result;
    }

    result.portDisplayName = deviceId;
    result.failureReason = Core::MidiPortOpenFailureReason::kNotFound;
    return result;
}

void MidiOutputPort::closePort()
{
    if (midiOutput != nullptr)
    {
        MidiLogger::getInstance().logInfo("MIDI output port closed");
        midiOutput.reset();
    }
    portIsOpen = false;
    openDeviceId_.clear();
}

bool MidiOutputPort::isOpenWithDevice(const juce::String& deviceId) const noexcept
{
    return portIsOpen && midiOutput != nullptr && openDeviceId_ == deviceId;
}

juce::String MidiOutputPort::getOpenDeviceId() const noexcept
{
    return openDeviceId_;
}

bool MidiOutputPort::isOpen() const noexcept
{
    return portIsOpen && midiOutput != nullptr;
}

juce::MidiOutput* MidiOutputPort::getMidiOutput() const noexcept
{
    return midiOutput.get();
}
