#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/MIDI/MidiManager.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{
namespace UnisonDetuneDispatch
{
    struct QuietGates
    {
        bool suppressMasterParameterSysEx = false;
        bool editorialResyncGranularMidiQuiet = false;
    };

    // Single source of truth for Master Edit UNISON DETUNE CC 94 dispatch gates.
    // Used by PluginProcessor and unit tests (PluginProcessor is not linked into the test runner).
    inline void onParameterChanged(MidiManager* midiManager,
                                   juce::AudioProcessorValueTreeState& apvts,
                                   const juce::String& parameterId,
                                   QuietGates gates)
    {
        if (parameterId != PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonDetune)
            return;

        if (gates.suppressMasterParameterSysEx || gates.editorialResyncGranularMidiQuiet)
            return;

        if (midiManager == nullptr)
            return;

        const auto* raw = apvts.getRawParameterValue(parameterId);
        if (raw == nullptr)
            return;

        midiManager->sendUnisonDetune(juce::roundToInt(raw->load()));
    }
}
} // namespace Core
