// Settings SUPPORT — diagnostic logging combo + open log folder.
#include "PluginEditor.h"

#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginEditor::wireSettingsDiagnosticLogging(SettingsPanel& panel)
{
    panel.getDiagnosticLoggingCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::DiagnosticLogging;
        const int selectedId = panel.getDiagnosticLoggingCombo().getSelectedId();
        if (selectedId != kOff && selectedId != kOn)
            return;
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kDiagnosticLogging, selectedId, nullptr);
        pluginProcessor.setDiagnosticLoggingEnabled(selectedId == kOn);

        // Enable may force the property back to OFF if file open fails — keep the combo honest.
        const int applied = static_cast<int>(pluginProcessor.getApvts().state.getProperty(
            PluginIDs::Settings::kDiagnosticLogging, kOff));
        panel.getDiagnosticLoggingCombo().setSelectedId(
            applied == kOn ? kOn : kOff, juce::dontSendNotification);
    };

    panel.getOpenLogFolderButton().onClick = [this]
    {
        pluginProcessor.revealDiagnosticLogsFolder();
    };
}

