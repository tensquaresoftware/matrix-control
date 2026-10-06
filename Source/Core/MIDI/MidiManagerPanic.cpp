// Extracted from MidiManager.cpp for modular maintenance.
// PANIC: Note Offs for held notes + CC 123/121 (Matrix-era channel modes; no CC 120).

#include "MidiManager.h"

#include <utility>
#include <vector>

#include "Core/MIDI/ActiveNoteRegistry.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    constexpr int kNotesPerChannel = 128;
    constexpr int kMidiChannelCount = 16;
    // Skip stacking another Panic while a prior burst (or heavy realtime) is still draining —
    // Omni 16×128 floods wedged Device Inquiry and locked the UI (2026-10-06 UAT).
    constexpr std::size_t kPanicCoalesceRealtimeDepth = 32;

    struct PanicChannelTarget
    {
        int channel = 1;
        bool sendAllChannels = true;
    };

    void appendControllersForChannel(std::vector<juce::MidiMessage>& burst, int channel)
    {
        // Matrix-1000 MIDI SUMMARY lists All Notes Off + Reset All Controllers, not All Sound Off.
        burst.push_back(juce::MidiMessage::controllerEvent(channel, 123, 0));
        burst.push_back(juce::MidiMessage::controllerEvent(channel, 121, 0));
    }

    void appendFallbackNoteOffSpray(std::vector<juce::MidiMessage>& burst, int channel)
    {
        for (int note = 0; note < kNotesPerChannel; ++note)
            burst.push_back(juce::MidiMessage::noteOff(channel, note));
    }

    void appendPanicControllers(std::vector<juce::MidiMessage>& burst, const PanicChannelTarget& target)
    {
        if (target.sendAllChannels)
        {
            for (int ch = 1; ch <= kMidiChannelCount; ++ch)
                appendControllersForChannel(burst, ch);
            return;
        }

        appendControllersForChannel(burst, target.channel);
    }

    PanicChannelTarget resolvePanicChannelTarget(juce::AudioProcessorValueTreeState& apvts)
    {
        PanicChannelTarget target;
        if (auto* choice = dynamic_cast<juce::AudioParameterChoice*>(
                apvts.getParameter(PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel)))
        {
            const int index = choice->getIndex();
            if (index >= 1 && index <= 16)
            {
                target.channel = index;
                target.sendAllChannels = false;
            }
            // Omni (0) and Mono groups: clear every channel — synth may be sounding on any.
        }
        return target;
    }

    void appendHeldOrFallbackNoteOffs(std::vector<juce::MidiMessage>& burst,
                                      Core::ActiveNoteRegistry* registry,
                                      const PanicChannelTarget& target)
    {
        if (registry != nullptr)
        {
            auto heldNoteOffs = registry->takeNoteOffsAndClear();
            burst.insert(burst.end(),
                         std::make_move_iterator(heldNoteOffs.begin()),
                         std::make_move_iterator(heldNoteOffs.end()));
        }

        if (! burst.empty())
            return;

        // Nothing tracked (ghost / non-instrument path): single-channel 0-127 only —
        // never 16×128 (that flooded DIN MIDI and tripped presence lock).
        appendFallbackNoteOffSpray(burst, target.sendAllChannels ? 1 : target.channel);
    }
}

void MidiManager::setActiveNoteRegistry(Core::ActiveNoteRegistry* registry) noexcept
{
    activeNoteRegistry_ = registry;
}

void MidiManager::sendPanic()
{
    if (outboundQueue_.realtimeDepth() >= kPanicCoalesceRealtimeDepth)
        return;

    std::vector<juce::MidiMessage> burst;
    burst.reserve(160);

    const auto target = resolvePanicChannelTarget(apvts);
    appendHeldOrFallbackNoteOffs(burst, activeNoteRegistry_, target);
    appendPanicControllers(burst, target);

    outboundQueue_.enqueueRealtimeFrontMany(std::move(burst));
    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kInstrument);
}
