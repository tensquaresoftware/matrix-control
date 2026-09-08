// Extracted from PluginEditor.cpp for modular maintenance.
// Settings panel restore-from-state + control wiring.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Services/PatchNameDisplayMode.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    int normalizeComputerPatchesNamesPolicy(int policyRaw)
    {
        using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;

        if (policyRaw == kDisplaySysexNames || policyRaw == kDisplayFileNames
            || policyRaw == kAskOncePerLoad)
            return policyRaw;

        return kDefault;
    }

    int normalizeUnsavedStatePolicy(int policyRaw)
    {
        using namespace PluginIDs::Settings::UnsavedStatePolicy;

        if (policyRaw == kAlwaysWarn || policyRaw == kNeverWarn)
            return policyRaw;

        return kDefault;
    }

    int normalizeDeleteWarningPolicy(int policyRaw)
    {
        using namespace PluginIDs::Settings::DeleteWarningPolicy;

        if (policyRaw == kAlwaysWarn || policyRaw == kNeverWarn)
            return policyRaw;

        return kDefault;
    }

    int readNormalizedProperty(juce::ValueTree& state,
                               const char* propertyId,
                               int defaultValue,
                               const std::function<int(int)>& normalize)
    {
        const int raw = static_cast<int>(state.getProperty(propertyId, defaultValue));
        const int normalized = normalize(raw);
        if (normalized != raw)
            state.setProperty(propertyId, normalized, nullptr);
        return normalized;
    }
}

void PluginEditor::restoreSettingsPanelFromState(SettingsPanel& panel)
{
    auto& state = pluginProcessor.getApvts().state;

    if (!pluginProcessor.isStandalone())
        panel.getHardwareLatencySlider().setValue(pluginProcessor.getHardwareLatencyMs(), juce::dontSendNotification);

    panel.getMatrix1000PatchesCombo().setSelectedId(
        readNormalizedProperty(state,
                               PluginIDs::Settings::kMatrix1000PatchesNamesMode,
                               PluginIDs::Settings::Matrix1000PatchesNamesMode::kDefault,
                               [](int raw) { return Core::PatchNameDisplay::normalize(raw); }),
        juce::dontSendNotification);

    panel.getComputerPatchesCombo().setSelectedId(
        readNormalizedProperty(state,
                               PluginIDs::Settings::kComputerPatchesNamesPolicy,
                               PluginIDs::Settings::ComputerPatchesNamesPolicy::kDefault,
                               normalizeComputerPatchesNamesPolicy),
        juce::dontSendNotification);

    panel.getUnsavedStateCombo().setSelectedId(
        readNormalizedProperty(state,
                               PluginIDs::Settings::kUnsavedStatePolicy,
                               PluginIDs::Settings::UnsavedStatePolicy::kDefault,
                               normalizeUnsavedStatePolicy),
        juce::dontSendNotification);

    panel.getDeleteWarningCombo().setSelectedId(
        readNormalizedProperty(state,
                               PluginIDs::Settings::kDeleteWarningPolicy,
                               PluginIDs::Settings::DeleteWarningPolicy::kDefault,
                               normalizeDeleteWarningPolicy),
        juce::dontSendNotification);

    refreshInitTemplateDeleteButtons(panel);
}

void PluginEditor::refreshInitTemplateDeleteButtons(SettingsPanel& panel)
{
    panel.refreshInitTemplateDeleteEnablement(pluginProcessor.patchInitTemplateExists(),
                                              pluginProcessor.masterInitTemplateExists());
}

bool PluginEditor::confirmDeleteInitTemplate(const juce::String& bodyMessage)
{
    namespace Dialog = PluginDisplayNames::Dialogs::DeleteInitTemplateConfirm;

    return PluginEditorInternal::showOrderedConfirmAlert({
               juce::MessageBoxIconType::WarningIcon,
               Dialog::kTitle,
               bodyMessage,
               Dialog::kCancel,
               Dialog::kDelete,
               this
           })
           == 1;
}

void PluginEditor::wireSettingsInitAndMasterActions(SettingsPanel& panel)
{
    panel.getPatchSaveAsInitButton().onClick = [this, &panel]
    {
        pluginProcessor.savePatchAsInitTemplate();
        refreshInitTemplateDeleteButtons(panel);
    };

    panel.getMasterSaveAsInitButton().onClick = [this, &panel]
    {
        pluginProcessor.saveMasterAsInitTemplate();
        refreshInitTemplateDeleteButtons(panel);
    };

    panel.getPatchDeleteInitButton().onClick = [this, &panel]
    {
        if (! confirmDeleteInitTemplate(PluginDisplayNames::Dialogs::DeleteInitTemplateConfirm::kBodyPatch))
            return;

        pluginProcessor.deletePatchInitTemplate();
        refreshInitTemplateDeleteButtons(panel);
    };

    panel.getMasterDeleteInitButton().onClick = [this, &panel]
    {
        if (! confirmDeleteInitTemplate(PluginDisplayNames::Dialogs::DeleteInitTemplateConfirm::kBodyMaster))
            return;

        pluginProcessor.deleteMasterInitTemplate();
        refreshInitTemplateDeleteButtons(panel);
    };

    panel.getMasterInitButton().onClick = [this]
    {
        openMasterGlobalInitConfirmDialog([this]
        {
            pluginProcessor.initAllMasterModulesFromTemplate();
        });
    };

    wireSettingsMasterFileActions(panel);
}

void PluginEditor::wireSettingsMasterFileActions(SettingsPanel& panel)
{
    panel.getMasterLoadButton().onClick = [this]
    {
        const auto file = PluginEditorInternal::browseForFileToOpenSync(
            this, PluginDisplayNames::Settings::kLoadMasterDialogTitle, {}, "*.syx");

        if (file.getFullPathName().isNotEmpty())
            pluginProcessor.loadMasterFromUserFile(file);
    };

    panel.getMasterSaveAsButton().onClick = [this]
    {
        const auto file = PluginEditorInternal::browseForFileToSaveSync(
            this,
            PluginDisplayNames::Settings::kSaveMasterAsDialogTitle,
            juce::File().getChildFile("Master.syx"),
            "*.syx");

        if (file.getFullPathName().isNotEmpty())
            pluginProcessor.saveMasterToUserFile(file);
    };
}

void PluginEditor::wireSettingsPanel(SettingsPanel& panel)
{
    panel.getHardwareLatencySlider().onValueChange = [this, &panel]
    {
        pluginProcessor.setHardwareLatencyMs(static_cast<float>(panel.getHardwareLatencySlider().getValue()));
    };

    panel.getMatrix1000PatchesCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::Matrix1000PatchesNamesMode;
        const int selectedId = panel.getMatrix1000PatchesCombo().getSelectedId();
        if (selectedId != kDisplayMusicalNames && selectedId != kDisplayHardwareNames)
            return;
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kMatrix1000PatchesNamesMode, selectedId, nullptr);
        pluginProcessor.refreshPatchNameDisplayForSettingsMode();
    };

    panel.getComputerPatchesCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::ComputerPatchesNamesPolicy;
        const int selectedId = panel.getComputerPatchesCombo().getSelectedId();
        if (selectedId != kDisplaySysexNames && selectedId != kDisplayFileNames
            && selectedId != kAskOncePerLoad)
            return;
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, selectedId, nullptr);

        // DISPLAY SYSEX / FILE NAMES: refresh PATCH NAME for an already-loaded computer patch.
        // ASK ONCE PER LOAD is load-time only — do not prompt from Settings.
        if (selectedId == kDisplaySysexNames || selectedId == kDisplayFileNames)
            pluginProcessor.refreshComputerPatchNameDisplayForSettingsPolicy();
    };

    panel.getUnsavedStateCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::UnsavedStatePolicy;
        const int selectedId = panel.getUnsavedStateCombo().getSelectedId();
        if (selectedId != kAlwaysWarn && selectedId != kNeverWarn)
            return;
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kUnsavedStatePolicy, selectedId, nullptr);
    };

    panel.getDeleteWarningCombo().onChange = [this, &panel]
    {
        using namespace PluginIDs::Settings::DeleteWarningPolicy;
        const int selectedId = panel.getDeleteWarningCombo().getSelectedId();
        if (selectedId != kAlwaysWarn && selectedId != kNeverWarn)
            return;
        pluginProcessor.getApvts().state.setProperty(
            PluginIDs::Settings::kDeleteWarningPolicy, selectedId, nullptr);
    };

    wireSettingsInitAndMasterActions(panel);
}
