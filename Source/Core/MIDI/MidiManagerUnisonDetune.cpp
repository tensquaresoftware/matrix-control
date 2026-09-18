// Extracted from MidiManager.cpp for modular maintenance.
// Unison Detune (MIDI CC 94) outbound gated by Settings EPROM TYPE.

#include "MidiManager.h"

#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/PluginIDs.h"

void MidiManager::sendUnisonDetune(int value)
{
    if (! isMasterEditOutboundAllowed())
        return;

    const int epromType = Core::EpromTypePolicy::normalize(static_cast<int>(
        apvts.state.getProperty(PluginIDs::Settings::kEpromType,
                                PluginIDs::Settings::EpromType::kDefault)));
    if (! Core::EpromTypePolicy::supportsUnisonDetune(epromType))
        return;

    constexpr int kUnisonDetuneController = 94;
    const int clampedValue = juce::jlimit(0, 127, value);

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
    }

    const auto enqueueCc = [this, clampedValue](int ch)
    {
        outboundQueue_.enqueueRealtime(
            juce::MidiMessage::controllerEvent(ch, kUnisonDetuneController, clampedValue));
    };

    if (sendAllChannels)
    {
        for (int ch = 1; ch <= 16; ++ch)
            enqueueCc(ch);
    }
    else
    {
        enqueueCc(channel);
    }

    activityTracker_.notifyActivity(Core::MidiActivityTracker::Path::kEditor);
}
