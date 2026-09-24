#pragma once

#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/ComboBoxLiveRefresh.h"
#include "GUI/Widgets/ComboBox.h"
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
        const ComboBox& combo,
        const std::vector<juce::String>& identifiers,
        const juce::Array<juce::MidiDeviceInfo>& devices)
    {
        if (! identifiersMatchMidiDevices(identifiers, devices))
            return false;

        if (combo.getNumItems() != static_cast<int>(identifiers.size()) + 1)
            return false;

        for (int i = 0; i < devices.size(); ++i)
        {
            if (combo.getItemText(i + 1) != devices.getReference(i).name.toUpperCase())
                return false;
        }

        return true;
    }

    inline void populateInputPortCombo(ComboBox& combo, std::vector<juce::String>& identifiers)
    {
        const juce::String previousIdentifier = selectedPortId(combo, identifiers);
        const auto devices = juce::MidiInput::getAvailableDevices();
        const bool itemSetUnchanged = midiPortItemSetUnchanged(combo, identifiers, devices);
        const auto action = ComboBoxLiveRefresh::planRefresh(combo.isPopupOpen(), itemSetUnchanged);

        if (action == ComboBoxLiveRefresh::Action::kSkipRebuild)
        {
            selectPortInCombo(combo, identifiers, previousIdentifier);
            return;
        }

        const auto rebuild = [&combo, &identifiers, &devices, &previousIdentifier]()
        {
            combo.clear(juce::dontSendNotification);
            identifiers.clear();
            combo.addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel, kPortSentinelItemId);

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

    inline void populateOutputPortCombo(ComboBox& combo, std::vector<juce::String>& identifiers)
    {
        const juce::String previousIdentifier = selectedPortId(combo, identifiers);
        const auto devices = juce::MidiOutput::getAvailableDevices();
        const bool itemSetUnchanged = midiPortItemSetUnchanged(combo, identifiers, devices);
        const auto action = ComboBoxLiveRefresh::planRefresh(combo.isPopupOpen(), itemSetUnchanged);

        if (action == ComboBoxLiveRefresh::Action::kSkipRebuild)
        {
            selectPortInCombo(combo, identifiers, previousIdentifier);
            return;
        }

        const auto rebuild = [&combo, &identifiers, &devices, &previousIdentifier]()
        {
            combo.clear(juce::dontSendNotification);
            identifiers.clear();
            combo.addItem(PluginDisplayNames::HeaderPanel::kNoOutputSentinel, kPortSentinelItemId);

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
}
