#pragma once

#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/ComboBoxLiveRefresh.h"
#include "GUI/Widgets/ComboBox.h"
#include "Core/MIDI/MidiDevicePresence.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS::MidiPortComboPopulation
{
    inline constexpr int kPortSentinelItemId = 1;
    inline constexpr int kFirstDeviceItemId = 2;

    inline int findItemIdForPortIdentifier(const std::vector<juce::String>& identifiers,
                                           const juce::String& deviceId) noexcept
    {
        if (deviceId.isEmpty())
            return kPortSentinelItemId;

        for (size_t i = 0; i < identifiers.size(); ++i)
        {
            if (identifiers[i] == deviceId)
                return static_cast<int>(i) + kFirstDeviceItemId;
        }

        return kPortSentinelItemId;
    }

    inline juce::String getPortIdentifierForItemId(const std::vector<juce::String>& identifiers,
                                                   int itemId)
    {
        if (itemId < kFirstDeviceItemId)
            return {};

        const auto index = static_cast<size_t>(itemId - kFirstDeviceItemId);
        if (index >= identifiers.size())
            return {};

        return identifiers[index];
    }

    inline void selectPortInCombo(ComboBox& combo,
                                  const std::vector<juce::String>& identifiers,
                                  const juce::String& portId)
    {
        combo.setSelectedId(findItemIdForPortIdentifier(identifiers, portId),
                            juce::dontSendNotification);
    }

    inline juce::String selectedPortId(const ComboBox& combo,
                                       const std::vector<juce::String>& identifiers)
    {
        return getPortIdentifierForItemId(identifiers, combo.getSelectedId());
    }

    /** Sort by display name (case-insensitive); stable for equal names via identifier. */
    inline void sortMidiDevicesByName(juce::Array<juce::MidiDeviceInfo>& devices)
    {
        struct NameThenIdComparator
        {
            static int compareElements(const juce::MidiDeviceInfo& a,
                                       const juce::MidiDeviceInfo& b) noexcept
            {
                const int byName = a.name.compareIgnoreCase(b.name);
                if (byName != 0)
                    return byName;

                return a.identifier.compare(b.identifier);
            }
        };

        NameThenIdComparator comparator;
        devices.sort(comparator);
    }

    [[nodiscard]] inline juce::Array<juce::MidiDeviceInfo> sortedAvailableInputDevices()
    {
        auto devices = juce::MidiInput::getAvailableDevices();
        sortMidiDevicesByName(devices);
        return devices;
    }

    [[nodiscard]] inline juce::Array<juce::MidiDeviceInfo> sortedAvailableOutputDevices()
    {
        auto devices = juce::MidiOutput::getAvailableDevices();
        sortMidiDevicesByName(devices);
        return devices;
    }

    /** Drop powered-off devices that JUCE may still enumerate (CoreMIDI live sources on Apple). */
    [[nodiscard]] inline juce::Array<juce::MidiDeviceInfo> filterReachableInputDevices(
        const juce::Array<juce::MidiDeviceInfo>& devices,
        const juce::String& keepOpenDeviceId)
    {
        auto live = Core::filterLiveMidiInputs(devices);

        // A port we successfully hold may briefly vanish from the live scan during reopen;
        // keep it only when CoreMIDI still reports it live.
        if (keepOpenDeviceId.isNotEmpty()
            && Core::isMidiInputIdentifierLive(keepOpenDeviceId))
        {
            bool alreadyPresent = false;
            for (const auto& device : live)
            {
                if (device.identifier == keepOpenDeviceId)
                {
                    alreadyPresent = true;
                    break;
                }
            }

            if (! alreadyPresent)
            {
                for (const auto& device : devices)
                {
                    if (device.identifier == keepOpenDeviceId)
                    {
                        live.add(device);
                        break;
                    }
                }
            }
        }

        return live;
    }

    [[nodiscard]] inline juce::Array<juce::MidiDeviceInfo> filterReachableOutputDevices(
        const juce::Array<juce::MidiDeviceInfo>& devices,
        const juce::String& keepOpenDeviceId)
    {
        auto live = Core::filterLiveMidiOutputs(devices);

        if (keepOpenDeviceId.isNotEmpty()
            && Core::isMidiOutputIdentifierLive(keepOpenDeviceId))
        {
            bool alreadyPresent = false;
            for (const auto& device : live)
            {
                if (device.identifier == keepOpenDeviceId)
                {
                    alreadyPresent = true;
                    break;
                }
            }

            if (! alreadyPresent)
            {
                for (const auto& device : devices)
                {
                    if (device.identifier == keepOpenDeviceId)
                    {
                        live.add(device);
                        break;
                    }
                }
            }
        }

        return live;
    }

    [[nodiscard]] inline bool identifiersMatchMidiDevices(
        const std::vector<juce::String>& identifiers,
        const juce::Array<juce::MidiDeviceInfo>& devices) noexcept
    {
        if (static_cast<size_t>(devices.size()) != identifiers.size())
            return false;

        for (int i = 0; i < devices.size(); ++i)
        {
            if (identifiers[static_cast<size_t>(i)] != devices.getReference(i).identifier)
                return false;
        }

        return true;
    }

    [[nodiscard]] inline bool midiPortItemSetUnchanged(
        const std::vector<juce::String>& identifiers,
        const juce::Array<juce::MidiDeviceInfo>& devices,
        int comboNumItems,
        const std::vector<juce::String>& comboItemTexts) noexcept
    {
        if (! identifiersMatchMidiDevices(identifiers, devices))
            return false;

        if (comboNumItems != static_cast<int>(identifiers.size()) + 1)
            return false;

        if (static_cast<int>(comboItemTexts.size()) != comboNumItems)
            return false;

        for (int i = 0; i < devices.size(); ++i)
        {
            if (comboItemTexts[static_cast<size_t>(i + 1)]
                != devices.getReference(i).name.toUpperCase())
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] inline bool midiPortItemSetUnchanged(
        const ComboBox& combo,
        const std::vector<juce::String>& identifiers,
        const juce::Array<juce::MidiDeviceInfo>& devices)
    {
        std::vector<juce::String> texts;
        texts.reserve(static_cast<size_t>(combo.getNumItems()));
        for (int i = 0; i < combo.getNumItems(); ++i)
            texts.push_back(combo.getItemText(i));

        return midiPortItemSetUnchanged(identifiers, devices, combo.getNumItems(), texts);
    }

    inline void populatePortCombo(ComboBox& combo,
                                  std::vector<juce::String>& identifiers,
                                  const juce::Array<juce::MidiDeviceInfo>& devices,
                                  const juce::String& sentinelLabel)
    {
        const juce::String previousIdentifier = selectedPortId(combo, identifiers);
        const bool itemSetUnchanged = midiPortItemSetUnchanged(combo, identifiers, devices);
        const auto action = ComboBoxLiveRefresh::planRefresh(combo.isPopupOpen(), itemSetUnchanged);

        if (action == ComboBoxLiveRefresh::Action::kSkipRebuild)
        {
            selectPortInCombo(combo, identifiers, previousIdentifier);
            return;
        }

        const auto rebuild = [&combo, &identifiers, &devices, &previousIdentifier, &sentinelLabel]()
        {
            combo.clear(juce::dontSendNotification);
            identifiers.clear();
            combo.addItem(sentinelLabel, kPortSentinelItemId);

            for (int i = 0; i < devices.size(); ++i)
            {
                const auto& device = devices.getReference(i);
                const int itemId = i + kFirstDeviceItemId;
                combo.addItem(device.name.toUpperCase(), itemId);
                identifiers.push_back(device.identifier);
            }

            selectPortInCombo(combo, identifiers, previousIdentifier);
        };

        if (action == ComboBoxLiveRefresh::Action::kDismissRebuildReopen)
            ComboBoxLiveRefresh::rebuildPreservingOpenPopup(combo, rebuild);
        else
            rebuild();
    }

    inline void populateInputPortCombo(ComboBox& combo,
                                       std::vector<juce::String>& identifiers,
                                       const juce::String& keepOpenDeviceId = {})
    {
        populatePortCombo(combo,
                          identifiers,
                          filterReachableInputDevices(sortedAvailableInputDevices(), keepOpenDeviceId),
                          PluginDisplayNames::HeaderPanel::kNoInputSentinel);
    }

    inline void populateOutputPortCombo(ComboBox& combo,
                                        std::vector<juce::String>& identifiers,
                                        const juce::String& keepOpenDeviceId = {})
    {
        populatePortCombo(combo,
                          identifiers,
                          filterReachableOutputDevices(sortedAvailableOutputDevices(), keepOpenDeviceId),
                          PluginDisplayNames::HeaderPanel::kNoOutputSentinel);
    }
}
