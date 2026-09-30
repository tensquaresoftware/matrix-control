// Extracted from PluginEditor.cpp for modular maintenance.
// Construction phase: Bank Utility EXPORT/IMPORT pickers, confirm gates and progress presenter.

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Layout/ScaledLayout.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PluginEditorInternal;

void PluginEditor::wireBankTransferBindings()
{
    setBankExportFolderPickerBinding();
    setBankImportFolderPickerBinding();
    setBankImportConfirmGateBinding();
    setBankExportOverwriteConfirmGateBinding();
    setBankPasteConfirmGateBinding();
    wireBankTransferProgressPresenter();
}

void PluginEditor::setBankExportFolderPickerBinding()
{
    pluginProcessor.setBankExportFolderPicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
        {
            if (safeThis == nullptr)
                return {};

            namespace BankState = PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;

            juce::File startDirectory;
            const auto persistedPath = safeThis->pluginProcessor.getApvts().state.getProperty(
                BankState::kExportParentFolderPath, juce::String()).toString();
            if (persistedPath.isNotEmpty())
            {
                const juce::File persistedFolder(persistedPath);
                if (persistedFolder.isDirectory())
                    startDirectory = persistedFolder;
            }

            const auto chosen = browseForDirectorySync(safeThis.getComponent(),
                                                       "Select bank export folder",
                                                       startDirectory);
            if (chosen == juce::File())
                return {};

            if (chosen.isDirectory())
            {
                safeThis->pluginProcessor.getApvts().state.setProperty(
                    BankState::kExportParentFolderPath,
                    chosen.getFullPathName(),
                    nullptr);
            }

            return chosen;
        });
}

void PluginEditor::setBankImportFolderPickerBinding()
{
    pluginProcessor.setBankImportFolderPicker(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> juce::File
        {
            if (safeThis == nullptr)
                return {};

            namespace BankState = PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;

            juce::File startDirectory;
            const auto persistedPath = safeThis->pluginProcessor.getApvts().state.getProperty(
                BankState::kImportFolderPath, juce::String()).toString();
            if (persistedPath.isNotEmpty())
            {
                const juce::File persistedFolder(persistedPath);
                if (persistedFolder.isDirectory())
                    startDirectory = persistedFolder;
            }

            const auto chosen = browseForDirectorySync(safeThis.getComponent(),
                                                       "Select bank import folder",
                                                       startDirectory);
            if (chosen == juce::File())
                return {};

            if (chosen.isDirectory())
            {
                safeThis->pluginProcessor.getApvts().state.setProperty(
                    BankState::kImportFolderPath,
                    chosen.getFullPathName(),
                    nullptr);
            }

            return chosen;
        });
}

void PluginEditor::setBankImportConfirmGateBinding()
{
    pluginProcessor.setBankImportConfirmGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> bool
        {
            if (! isMessageThread() || safeThis == nullptr)
                return false;

            namespace Dialog = PluginDisplayNames::Dialogs::BankImportConfirm;

            return showOrderedConfirmAlert({
                       juce::MessageBoxIconType::WarningIcon,
                       Dialog::kTitle,
                       Dialog::kBody,
                       Dialog::kCancel,
                       Dialog::kContinue,
                       safeThis.getComponent(),
                       {},
                       {},
                       360
                   })
                   == 1;
        });
}

void PluginEditor::setBankExportOverwriteConfirmGateBinding()
{
    pluginProcessor.setBankExportOverwriteConfirmGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)]() -> bool
        {
            if (! isMessageThread() || safeThis == nullptr)
                return false;

            namespace Dialog = PluginDisplayNames::Dialogs::BankExportOverwriteConfirm;

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

void PluginEditor::setBankPasteConfirmGateBinding()
{
    pluginProcessor.setBankPasteConfirmGate(
        [safeThis = juce::Component::SafePointer<PluginEditor>(this)](int sourceBank, int targetBank) -> bool
        {
            if (! isMessageThread() || safeThis == nullptr)
                return false;

            namespace Dialog = PluginDisplayNames::Dialogs::BankPasteConfirm;

            return showOrderedConfirmAlert({
                       juce::MessageBoxIconType::WarningIcon,
                       Dialog::kTitle,
                       Dialog::formatBody(sourceBank, targetBank),
                       Dialog::kCancel,
                       Dialog::kContinue,
                       safeThis.getComponent()
                   })
                   == 1;
        });
}

void PluginEditor::wireBankTransferProgressPresenter()
{
    Core::BankTransferProgressPresenter presenter;
    configureBankTransferProgressShowAndUpdate(presenter);
    configureBankTransferProgressMessaging(presenter);
    pluginProcessor.setBankTransferProgressPresenter(std::move(presenter));
}

void PluginEditor::configureBankTransferProgressShowAndUpdate(Core::BankTransferProgressPresenter& presenter)
{
    presenter.show = [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
        const juce::String& title,
        const juce::String& message,
        const juce::String& detail,
        int totalSteps,
        std::function<void()> onCancel)
    {
        if (safeThis == nullptr)
            return;

        using namespace PluginDisplayNames::Dialogs::BankTransferProgress;

        const auto layout = (title == juce::String(kExportTitle) || title == juce::String(kCopyTitle))
            ? BankTransferProgressDialog::ContentLayout::SingleLane
            : BankTransferProgressDialog::ContentLayout::DualLane;

        safeThis->showBankTransferProgressDialog(
            BankTransferProgressShowRequest {
                title, message, detail, totalSteps, std::move(onCancel), layout });
    };

    presenter.update = [safeThis = juce::Component::SafePointer<PluginEditor>(this)](int completedSteps)
    {
        if (safeThis != nullptr && safeThis->bankTransferProgressDialog_ != nullptr)
            safeThis->bankTransferProgressDialog_->setProgress(completedSteps);
    };

    presenter.beginSecondaryPhase = [safeThis = juce::Component::SafePointer<PluginEditor>(this)](
        const juce::String& message, int totalSteps)
    {
        if (safeThis != nullptr && safeThis->bankTransferProgressDialog_ != nullptr)
        {
            safeThis->bankTransferProgressDialog_->beginSecondaryPhase(message, totalSteps);
            safeThis->updateBankTransferProgressDialogLayout(
                (safeThis->layoutDimensions_.editor.width > 0)
                    ? TSS::ScaledLayout::uiScaleFromEditorBounds(
                          safeThis->getWidth(), safeThis->layoutDimensions_.editor.width)
                    : 1.0f);
        }
    };
}

void PluginEditor::configureBankTransferProgressMessaging(Core::BankTransferProgressPresenter& presenter)
{
    presenter.setMessage = [safeThis = juce::Component::SafePointer<PluginEditor>(this)](const juce::String& message)
    {
        if (safeThis != nullptr && safeThis->bankTransferProgressDialog_ != nullptr)
            safeThis->bankTransferProgressDialog_->setMessage(message);
    };

    presenter.setDetail = [safeThis = juce::Component::SafePointer<PluginEditor>(this)](const juce::String& detail)
    {
        if (safeThis != nullptr && safeThis->bankTransferProgressDialog_ != nullptr)
            safeThis->bankTransferProgressDialog_->setDetail(detail);
    };

    presenter.setCancelEnabled = [safeThis = juce::Component::SafePointer<PluginEditor>(this)](bool enabled)
    {
        if (safeThis != nullptr && safeThis->bankTransferProgressDialog_ != nullptr)
            safeThis->bankTransferProgressDialog_->setCancelEnabled(enabled);
    };

    presenter.hide = [safeThis = juce::Component::SafePointer<PluginEditor>(this)]
    {
        if (safeThis != nullptr)
            safeThis->hideBankTransferProgressDialog();
    };
}
