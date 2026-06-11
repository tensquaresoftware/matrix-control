#include "Core/MIDI/KeyboardFromMidiInput.h"

#include <chrono>
#include <thread>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
    namespace
    {
        bool isAllowedInstrumentMessage(const juce::MidiMessage& msg) noexcept
        {
            return msg.isNoteOnOrOff() || msg.isController() || msg.isPitchWheel();
        }
    }

    KeyboardFromMidiInput::KeyboardFromMidiInput(MidiOutboundQueue& queue, MidiActivityTracker& tracker)
        : queue_(queue)
        , tracker_(tracker)
    {
    }

    KeyboardFromMidiInput::~KeyboardFromMidiInput()
    {
        closePort();
    }

    bool KeyboardFromMidiInput::setPort(const juce::String& deviceId)
    {
        closePort();

        if (deviceId.isEmpty())
            return true;

        auto devices = juce::MidiInput::getAvailableDevices();
        for (const auto& device : devices)
        {
            if (device.identifier != deviceId)
                continue;

            midiInput_ = juce::MidiInput::openDevice(device.identifier, this);
            if (midiInput_ == nullptr)
                return false;

            midiInput_->start();
            return true;
        }

        return false;
    }

    void KeyboardFromMidiInput::closePort()
    {
        if (midiInput_ != nullptr)
        {
            midiInput_->stop();
            std::this_thread::sleep_for(
                std::chrono::milliseconds(SysExConstants::kMidiInputStopDelayMs));
            midiInput_.reset();
        }
    }

    bool KeyboardFromMidiInput::isPortOpen() const noexcept
    {
        return midiInput_ != nullptr;
    }

    void KeyboardFromMidiInput::processIncomingMessage(const juce::MidiMessage& message)
    {
        if (isAllowedInstrumentMessage(message))
        {
            queue_.enqueueRealtime(message);
            tracker_.notifyActivity(MidiActivityTracker::Path::kInstrument);
        }
    }

    void KeyboardFromMidiInput::handleIncomingMidiMessage(juce::MidiInput* source,
                                                          const juce::MidiMessage& message)
    {
        juce::ignoreUnused(source);
        processIncomingMessage(message);
    }
}
