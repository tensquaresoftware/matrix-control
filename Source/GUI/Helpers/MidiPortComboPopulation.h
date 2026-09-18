#pragma once

#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

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

    inline void populateInputPortCombo(ComboBox& combo, std::vector<juce::String>& identifiers)
    {
        const juce::String previousIdentifier = selectedPortId(combo, identifiers);

        combo.clear(juce::dontSendNotification);
        identifiers.clear();
        combo.addItem(PluginDisplayNames::HeaderPanel::kNoInputSentinel, kPortSentinelItemId);

        const auto devices = juce::MidiInput::getAvailableDevices();
        for (int i = 0; i < devices.size(); ++i)
        {
            const auto& device = devices.getReference(i);
            const int itemId = i + kFirstDeviceItemId;
            combo.addItem(device.name.toUpperCase(), itemId);
            identifiers.push_back(device.identifier);
        }

        selectPortInCombo(combo, identifiers, previousIdentifier);
    }

    inline void populateOutputPortCombo(ComboBox& combo, std::vector<juce::String>& identifiers)
    {
        const juce::String previousIdentifier = selectedPortId(combo, identifiers);

        combo.clear(juce::dontSendNotification);
        identifiers.clear();
        combo.addItem(PluginDisplayNames::HeaderPanel::kNoOutputSentinel, kPortSentinelItemId);

        const auto devices = juce::MidiOutput::getAvailableDevices();
        for (int i = 0; i < devices.size(); ++i)
        {
            const auto& device = devices.getReference(i);
            const int itemId = i + kFirstDeviceItemId;
            combo.addItem(device.name.toUpperCase(), itemId);
            identifiers.push_back(device.identifier);
        }

        selectPortInCombo(combo, identifiers, previousIdentifier);
    }
}
