// Extracted from MidiManager.cpp for modular maintenance.
// PANIC: Note Off spray + CC 120/123/121 on the active channel (or all 16 for Omni/Mono).

#include "MidiManager.h"

#include <vector>

#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr int kNotesPerChannel = 128;
    constexpr int kMessagesPerChannel = kNotesPerChannel + 3;
    constexpr int kMidiChannelCount = 16;

    void appendControllersForChannel(std::vector<juce::MidiMessage>& burst, int channel)
    {
        burst.push_back(juce::MidiMessage::controllerEvent(channel, 120, 0));
        burst.push_back(juce::MidiMessage::controllerEvent(channel, 123, 0));
        burst.push_back(juce::MidiMessage::controllerEvent(channel, 121, 0));
    }

    void appendPanicForChannel(std::vector<juce::MidiMessage>& burst, int channel)
    {
        for (int note = 0; note < kNotesPerChannel; ++note)
            burst.push_back(juce::MidiMessage::noteOff(channel, note));

        appendControllersForChannel(burst, channel);
    }

    void appendInterleavedOmniPanic(std::vector<juce::MidiMessage>& burst)
    {
        // Interleave Note Offs across channels so every channel starts clearing promptly
        // (channel-major order would delay ch 16 by ~1s of DIN MIDI under a full spray).
        for (int note = 0; note < kNotesPerChannel; ++note)
            for (int ch = 1; ch <= kMidiChannelCount; ++ch)
                burst.push_back(juce::MidiMessage::noteOff(ch, note));

        for (int ch = 1; ch <= kMidiChannelCount; ++ch)
            appendControllersForChannel(burst, ch);
    }
}

void MidiManager::sendPanic()
{
    std::vector<juce::MidiMessage> burst;
    burst.reserve(static_cast<size_t>(kMidiChannelCount * kMessagesPerChannel));

    int channel = 1;
    bool sendAllChannels = true;
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(
            apvts.getParameter(PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel)))
    {
        const int index = choice->getIndex();
        if (index >= 1 && index <= 16)
        {
            channel = index;
            sendAllChannels = false;
        }
        // Omni (0) and Mono groups: clear every channel — synth may be sounding on any.
    }

    if (sendAllChannels)
        appendInterleavedOmniPanic(burst);
    else
        appendPanicForChannel(burst, channel);

    outboundQueue_.enqueueRealtimeFrontMany(std::move(burst));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kInstrument);
}
