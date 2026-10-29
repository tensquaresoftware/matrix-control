// MIDI & DEVICE Settings wiring: PRESENCE CHECK, EPROM TYPE, HARDWARE LATENCY.

#include "PluginEditor.h"

#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/PresenceCheckPolicy.h"
#include "Core/Services/DeviceConnectionMachineDefaults.h"
#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::restoreSettingsPresenceCheckFromState(SettingsPanel& panel)
{
    auto& state = pluginProcessor.getApvts().state;
    const int raw = static_cast<int>(state.getProperty(
        PluginIDs::Settings::kPresenceCheck,
        PluginIDs::Settings::PresenceCheck::kDefault));
    const int normalized = Core::PresenceCheckPolicy::normalize(raw);
    if (normalized != raw)
        state.setProperty(PluginIDs::Settings::kPresenceCheck, normalized, nullptr);

    panel.getPresenceCheckCombo().setSelectedId(normalized, juce::dontSendNotification);
}

void PluginEditor::wireSettingsEpromAndLatency(SettingsPanel& panel)
{
    panel.getHardwareLatencySlider().onValueChange = [this, &panel]
    {
        pluginProcessor.setHardwareLatencyMs(static_cast<float>(panel.getHardwareLatencySlider().getValue()));
    };

    panel.getPresenceCheckCombo().onChange = [this, &panel]
    {
        const int selectedId = panel.getPresenceCheckCombo().getSelectedId();
        const int normalized = Core::PresenceCheckPolicy::normalize(selectedId);
        if (selectedId != normalized)
            return;

        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kPresenceCheck, normalized, nullptr);
        Core::DeviceConnectionMachineDefaults::writePresenceCheck(normalized);
        pluginProcessor.getMidiManager().refreshDevicePresenceMonitoringFromSettings();
    };

    panel.registerPresenceCheckContextualHelp([this]() -> FooterPanel*
    {
        return mainComponent_ != nullptr ? &mainComponent_->getFooterPanel() : nullptr;
    });

    panel.getEpromTypeCombo().onChange = [this, &panel]
    {
        const int selectedId = panel.getEpromTypeCombo().getSelectedId();
        const int normalized = Core::EpromTypePolicy::normalize(selectedId);
        if (selectedId != normalized)
            return;

        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kEpromType, normalized, nullptr);
        Core::DeviceConnectionMachineDefaults::writeEpromType(normalized);
        pluginProcessor.getMidiManager().refreshSysExDelayFromSettings();
    };
}
