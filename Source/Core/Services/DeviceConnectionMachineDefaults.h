#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core::DeviceConnectionMachineDefaults
{
    /** Machine-global device-connection defaults (AD-7 narrow exception). */
    struct Values
    {
        bool promptDone = false;
        juce::String midiInputPortId;
        juce::String midiOutputPortId;
        int epromTypeId = PluginIDs::Settings::EpromType::kDefault;
    };

    /** Pure: CONFIRM writes promptDone + ports + EPROM. */
    inline Values applyConfirm(Values base,
                               int selectedEpromTypeId,
                               const juce::String& midiInputPortId,
                               const juce::String& midiOutputPortId) noexcept
    {
        base.promptDone = true;
        base.epromTypeId = EpromTypePolicy::normalize(selectedEpromTypeId);
        base.midiInputPortId = midiInputPortId;
        base.midiOutputPortId = midiOutputPortId;
        return base;
    }

    /** Pure: SPECIFY LATER writes promptDone + ports; EPROM unchanged. */
    inline Values applySpecifyLater(Values base,
                                    const juce::String& midiInputPortId,
                                    const juce::String& midiOutputPortId) noexcept
    {
        base.promptDone = true;
        base.midiInputPortId = midiInputPortId;
        base.midiOutputPortId = midiOutputPortId;
        return base;
    }

    /** Pure: update MIDI FROM only (empty clears that side; TO untouched). */
    inline Values applyMidiInputPort(Values base, const juce::String& midiInputPortId) noexcept
    {
        base.midiInputPortId = midiInputPortId;
        return base;
    }

    /** Pure: update MIDI TO only (empty clears that side; FROM untouched). */
    inline Values applyMidiOutputPort(Values base, const juce::String& midiOutputPortId) noexcept
    {
        base.midiOutputPortId = midiOutputPortId;
        return base;
    }

    /** Pure: Settings EPROM change — EPROM only. */
    inline Values applyEpromType(Values base, int epromTypeId) noexcept
    {
        base.epromTypeId = EpromTypePolicy::normalize(epromTypeId);
        return base;
    }

    /** Pure: seed live session APVTS from machine defaults (fresh construction). */
    inline void seedApvtsState(juce::ValueTree& state, const Values& values)
    {
        state.setProperty(PluginIDs::Settings::kEpromTypePromptDone, values.promptDone, nullptr);
        state.setProperty("midiInputPortId", values.midiInputPortId, nullptr);
        state.setProperty("midiOutputPortId", values.midiOutputPortId, nullptr);
        state.setProperty(PluginIDs::Settings::kEpromType,
                          EpromTypePolicy::normalize(values.epromTypeId),
                          nullptr);
    }

    /** True when a factory/empty host chunk should inherit machine connection defaults. */
    inline bool shouldReseedAfterHostRestore(bool machinePromptDone,
                                             bool sessionPromptDone) noexcept
    {
        return machinePromptDone && ! sessionPromptDone;
    }

    Values load();
    void save(const Values& values);

    void writeAfterConfirm(int selectedEpromTypeId,
                           const juce::String& midiInputPortId,
                           const juce::String& midiOutputPortId);
    void writeAfterSpecifyLater(const juce::String& midiInputPortId,
                                const juce::String& midiOutputPortId);
    void writeMidiInputPort(const juce::String& midiInputPortId);
    void writeMidiOutputPort(const juce::String& midiOutputPortId);
    void writeEpromType(int epromTypeId);

    /** Re-seed APVTS when machine promptDone is true and the restored session is still false. */
    void reseedApvtsAfterHostRestoreIfNeeded(juce::ValueTree& state);

    inline void writeAfterConfirmFromState(const juce::ValueTree& state, int selectedEpromTypeId)
    {
        writeAfterConfirm(selectedEpromTypeId,
                          state.getProperty("midiInputPortId", juce::String()).toString(),
                          state.getProperty("midiOutputPortId", juce::String()).toString());
    }

    inline void writeAfterSpecifyLaterFromState(const juce::ValueTree& state)
    {
        writeAfterSpecifyLater(state.getProperty("midiInputPortId", juce::String()).toString(),
                               state.getProperty("midiOutputPortId", juce::String()).toString());
    }
}
