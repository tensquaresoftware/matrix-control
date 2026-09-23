// Extracted from PluginEditor.cpp for modular maintenance.
// Construction phase: OS pickers + confirm gates for Computer Patches / Patch Mutator.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Core/Services/MutatorDeleteWarningPolicy.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PluginEditorInternal;

void PluginEditor::wirePatchAndMutatorBindings()
{
    setPatchFolderPickerBinding();
    setMutatorExportFolderPickerBinding();
    setMutatorDefragLimitGateBinding();
    setMutatorExportCollisionGateBinding();
    setMutatorHistoryGateBinding();
    setUnsavedEditConfirmGateBinding();
    setMutatorFlushConfirmGateBinding();
    setMutatorDeleteConfirmGateBinding();
    setPatchSaveFilePickerBinding();
    setPatchNameReconciliationPickerBinding();
}

void PluginEditor::setPatchFolderPickerBinding()
{
    pluginProcessor.setPatchFolderPicker([safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
    {
        if (safeThis == nullptr)
            return {};

        juce::File startDirectory;
        const auto persistedPath = safeThis->pluginProcessor.getApvts().state.getProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String()).toString();

        if (persistedPath.isNotEmpty())
        {
            const juce::File persistedFolder(persistedPath);
            if (persistedFolder.isDirectory())
                startDirectory = persistedFolder;
        }

        // OPEN may auto-load and show ASK ONCE next — helper raises before/after picker.
        return browseForDirectorySync(safeThis.getComponent(),
                                      "Select patch folder",
                                      startDirectory);
    });
}

void PluginEditor::setMutatorExportFolderPickerBinding()
{
    pluginProcessor.setMutatorExportFolderPicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
        {
            if (safeThis == nullptr)
                return {};

            return browseForDirectorySync(safeThis.getComponent(),
                                          "Select mutation export folder",
                                          {});
        });
}

void PluginEditor::setMutatorDefragLimitGateBinding()
{
    pluginProcessor.setMutatorDefragLimitModalGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](std::function<void()> onConfirmed)
        {
            if (! isMessageThread() || safeThis == nullptr)
                return;

            safeThis->openMutatorHistoryDefragConfirmDialog(std::move(onConfirmed));
        });
}

void PluginEditor::setMutatorPanelDefragRecoveryBinding()
{
    if (mainComponent_ == nullptr)
        return;

    auto& mutatorPanel = mainComponent_->getBodyPanel()
                             .getSharedPanel()
                             .getPatchManagerPanel()
                             .getPatchMutatorPanel();

    mutatorPanel.setDefragRecoveryRequestHandler(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
        {
            if (safeThis == nullptr)
                return;

            safeThis->openMutatorHistoryDefragConfirmDialog([safeThis]
            {
                if (safeThis == nullptr)
                    return;

                safeThis->pluginProcessor.defragMutationHistory();
            });
        });
}

void PluginEditor::setMutatorExportCollisionGateBinding()
{
    pluginProcessor.setMutatorExportCollisionModalGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
            std::function<void(Core::ExportCollisionResolution)> onResolved)
        {
            if (! onResolved)
                return;

            if (! isMessageThread() || safeThis == nullptr)
            {
                onResolved(Core::ExportCollisionResolution::kCancel);
                return;
            }

            namespace Msg = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

            switch (showOrderedConfirmAlert({
                juce::MessageBoxIconType::QuestionIcon,
                Msg::kExportCollisionTitle,
                Msg::kExportCollisionMessage,
                Msg::kExportCollisionCancel,
                Msg::kExportCollisionOverwrite,
                safeThis.getComponent(),
                Msg::kExportCollisionKeep
            }))
            {
                case 1: onResolved(Core::ExportCollisionResolution::kOverwrite); break;
                case 2: onResolved(Core::ExportCollisionResolution::kKeep); break;
                default: onResolved(Core::ExportCollisionResolution::kCancel); break;
            }
        });
}

void PluginEditor::setMutatorHistoryGateBinding()
{
    pluginProcessor.setMutatorHistoryGateModalGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> Core::MutatorHistoryGateChoice
        {
            if (! isMessageThread() || safeThis == nullptr)
                return Core::MutatorHistoryGateChoice::kCancel;

            namespace Msg = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

            switch (showOrderedConfirmAlert({
                juce::MessageBoxIconType::QuestionIcon,
                Msg::kHistoryGateTitle,
                Msg::kHistoryGateMessage,
                Msg::kHistoryGateCancel,
                Msg::kHistoryGateExport,
                safeThis.getComponent(),
                Msg::kHistoryGateDiscard
            }))
            {
                case 1: return Core::MutatorHistoryGateChoice::kExport;
                case 2: return Core::MutatorHistoryGateChoice::kDiscard;
                default: return Core::MutatorHistoryGateChoice::kCancel;
            }
        });
}

void PluginEditor::setUnsavedEditConfirmGateBinding()
{
    pluginProcessor.setUnsavedEditConfirmModalGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
            Core::UnsavedEditPersistKind persistKind) -> Core::UnsavedEditConfirmChoice
        {
            if (! isMessageThread() || safeThis == nullptr)
                return Core::UnsavedEditConfirmChoice::kCancel;

            namespace Dialog = PluginDisplayNames::Dialogs::UnsavedEditConfirm;

            const char* body = Dialog::kBodySaveAs;
            const char* persistLabel = Dialog::kSaveAs;

            switch (persistKind)
            {
                case Core::UnsavedEditPersistKind::kStore:
                    body = Dialog::kBodyStore;
                    persistLabel = Dialog::kStore;
                    break;
                case Core::UnsavedEditPersistKind::kSave:
                    body = Dialog::kBodySave;
                    persistLabel = Dialog::kSave;
                    break;
                case Core::UnsavedEditPersistKind::kSaveAs:
                    break;
            }

            const int code = showOrderedConfirmAlert({
                juce::MessageBoxIconType::WarningIcon,
                Dialog::kTitle,
                body,
                Dialog::kCancel,
                persistLabel,
                safeThis.getComponent(),
                Dialog::kDiscard
            });

            switch (code)
            {
                case 1: return Core::UnsavedEditConfirmChoice::kPersist;
                case 2: return Core::UnsavedEditConfirmChoice::kDiscard;
                default: return Core::UnsavedEditConfirmChoice::kCancel;
            }
        });
}

void PluginEditor::setMutatorFlushConfirmGateBinding()
{
    pluginProcessor.setMutatorFlushConfirmModalGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> bool
        {
            if (! isMessageThread() || safeThis == nullptr)
            {
                jassertfalse;
                return false;
            }

            namespace Dialog = PluginDisplayNames::Dialogs::MutatorFlushConfirm;

            return showOrderedConfirmAlert({
                       juce::MessageBoxIconType::WarningIcon,
                       Dialog::kTitle,
                       Dialog::kBody,
                       Dialog::kCancel,
                       Dialog::kContinue,
                       safeThis.getComponent()
                   })
                   == 1;
        });
}

void PluginEditor::setMutatorDeleteConfirmGateBinding()
{
    pluginProcessor.setMutatorDeleteConfirmModalGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> bool
        {
            if (! isMessageThread() || safeThis == nullptr)
            {
                jassertfalse;
                return false;
            }

            using namespace PluginIDs::Settings::DeleteWarningPolicy;

            const int policyRaw = static_cast<int>(safeThis->pluginProcessor.getApvts().state.getProperty(
                PluginIDs::Settings::kDeleteWarningPolicy,
                kDefault));
            const int policy =
                (policyRaw == kAlwaysWarn || policyRaw == kNeverWarn) ? policyRaw : kDefault;

            if (! Core::MutatorDeleteWarning::shouldPrompt(policy))
                return true;

            const auto result = showMutatorDeleteConfirmAlert(safeThis.getComponent());

            if (result.confirmed && result.dontAskAgain)
            {
                safeThis->pluginProcessor.getApvts().state.setProperty(
                    PluginIDs::Settings::kDeleteWarningPolicy,
                    kNeverWarn,
                    nullptr);

                if (auto* settingsPanel = safeThis->getSettingsPanelIfOpen())
                    settingsPanel->getDeleteWarningCombo().setSelectedId(
                        kNeverWarn, juce::dontSendNotification);
            }

            return result.confirmed;
        });
}

void PluginEditor::setPatchSaveFilePickerBinding()
{
    pluginProcessor.setPatchSaveFilePicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
            juce::File suggestedFolder, juce::String suggestedStem) -> juce::File
        {
            if (safeThis == nullptr)
                return {};

            namespace Dialog = PluginDisplayNames::Dialogs::InvalidSaveAsPatchName;
            auto folder = suggestedFolder;
            auto stemForDialog = suggestedStem;

            for (;;)
            {
                if (safeThis == nullptr)
                    return {};

                const auto chosen = browseForFileToSaveSync(
                    safeThis.getComponent(),
                    "Save patch as",
                    folder.getChildFile(stemForDialog + ".syx"),
                    "*.syx");

                if (chosen == juce::File())
                    return {};

                if (safeThis == nullptr)
                    return {};

                const auto normalized = Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty(
                    chosen.getFileNameWithoutExtension());

                if (normalized.isNotEmpty())
                    return chosen.getSiblingFile(
                        Core::PatchFileNameSanitizer::ensureSyxExtension(normalized));

                raiseUiBeforeModalDialog(safeThis.getComponent());

                juce::NativeMessageBox::show(juce::MessageBoxOptions()
                                                .withIconType(juce::MessageBoxIconType::WarningIcon)
                                                .withTitle(Dialog::kTitle)
                                                .withMessage(Dialog::kBody)
                                                .withButton(Dialog::kOk)
                                                .withAssociatedComponent(safeThis.getComponent()));

                if (safeThis == nullptr)
                    return {};

                raiseUiBeforeModalDialog(safeThis.getComponent());
                folder = chosen.getParentDirectory();
                stemForDialog = chosen.getFileNameWithoutExtension();
            }
        });
}

void PluginEditor::setPatchNameReconciliationPickerBinding()
{
    pluginProcessor.setPatchNameReconciliationPicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
            juce::String internalSanitized, juce::String fileSanitized)
            -> std::optional<Core::NameReconciliationChoice>
        {
            if (! isMessageThread() || safeThis == nullptr)
                return std::nullopt;

            namespace Dialog = PluginDisplayNames::Dialogs::PatchNameReconciliation;
            const auto body = juce::String(Dialog::kBodyTemplate)
                                  .replace("{INTERNAL}", internalSanitized)
                                  .replace("{FILENAME}", fileSanitized);

            switch (showOrderedConfirmAlert({
                juce::MessageBoxIconType::QuestionIcon,
                Dialog::kTitle,
                body,
                Dialog::kCancel,
                Dialog::kFilename,
                safeThis.getComponent(),
                Dialog::kInternal
            }))
            {
                case 1: return Core::NameReconciliationChoice::kFilename;
                case 2: return Core::NameReconciliationChoice::kInternal;
                default: return std::nullopt;
            }
        });
}
