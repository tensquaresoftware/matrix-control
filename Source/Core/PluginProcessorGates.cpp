// Extracted from PluginProcessor.cpp for modular maintenance.
// Editor-supplied pickers / modal gates and the patch-context confirmation flow.

#include "PluginProcessor.h"

#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/Services/UnsavedEditWarningPolicy.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginProcessor::setPatchFolderPicker(PatchFolderPicker picker)
{
    patchFolderPicker_ = std::move(picker);
}

void PluginProcessor::loadDroppedComputerPatchFile(const juce::File& file)
{
    if (patchManagerActionHandler_ == nullptr)
        return;

    patchManagerActionHandler_->loadDroppedComputerPatchFile(file, getResolvedDeviceMemoryLimits());
}

void PluginProcessor::setMutatorExportFolderPicker(MutatorExportFolderPicker picker)
{
    mutatorExportFolderPicker_ = std::move(picker);
}

void PluginProcessor::setMutatorDefragLimitModalGate(MutatorDefragLimitModalGate gate)
{
    mutatorDefragLimitModalGate_ = std::move(gate);
}

void PluginProcessor::setMutatorExportCollisionModalGate(MutatorExportCollisionModalGate gate)
{
    mutatorExportCollisionModalGate_ = std::move(gate);
}

void PluginProcessor::setMutatorHistoryGateModalGate(MutatorHistoryGateModalGate gate)
{
    mutatorHistoryGateModalGate_ = std::move(gate);
}

void PluginProcessor::setUnsavedEditConfirmModalGate(UnsavedEditConfirmModalGate gate)
{
    unsavedEditConfirmModalGate_ = std::move(gate);
}

void PluginProcessor::setNameRequiredBeforeStoreRequest(NameRequiredBeforeStoreRequest request)
{
    nameRequiredBeforeStoreRequest_ = std::move(request);
}

void PluginProcessor::setMutatorFlushConfirmModalGate(MutatorFlushConfirmModalGate gate)
{
    mutatorFlushConfirmModalGate_ = std::move(gate);
}

void PluginProcessor::setMutatorDeleteConfirmModalGate(MutatorDeleteConfirmModalGate gate)
{
    mutatorDeleteConfirmModalGate_ = std::move(gate);
}

void PluginProcessor::setPatchSaveFilePicker(PatchSaveFilePicker picker)
{
    patchSaveFilePicker_ = std::move(picker);
}

void PluginProcessor::setPatchNameReconciliationPicker(PatchNameReconciliationPicker picker)
{
    patchNameReconciliationPicker_ = std::move(picker);
}

void PluginProcessor::setBankExportFolderPicker(PatchFolderPicker picker)
{
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->setBankExportFolderPicker(std::move(picker));
}

void PluginProcessor::setBankImportFolderPicker(PatchFolderPicker picker)
{
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->setBankImportFolderPicker(std::move(picker));
}

void PluginProcessor::setBankImportConfirmGate(BankImportConfirmGate gate)
{
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->setBankImportConfirmGate(std::move(gate));
}

void PluginProcessor::setBankExportOverwriteConfirmGate(BankImportConfirmGate gate)
{
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->setBankExportOverwriteConfirmGate(std::move(gate));
}

void PluginProcessor::setBankPasteConfirmGate(BankPasteConfirmGate gate)
{
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->setBankPasteConfirmGate(std::move(gate));
}

void PluginProcessor::setBankTransferProgressPresenter(Core::BankTransferProgressPresenter presenter)
{
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->setBankTransferProgressPresenter(std::move(presenter));
}

// AlertWindow / FileChooser require the message thread. Off-thread callers cannot safely
// open a modal; refuse the context change rather than risk a freeze or nested-loop crash.
bool PluginProcessor::requireMessageThreadForModalGate() const
{
    auto* messageManager = juce::MessageManager::getInstanceWithoutCreating();

    if (messageManager == nullptr)
        return false;

    if (! messageManager->isThisTheMessageThread())
    {
        jassertfalse;
        return false;
    }

    return true;
}

bool PluginProcessor::applyUnsavedEditConfirmChoice(Core::UnsavedEditConfirmChoice choice,
                                                    Core::UnsavedEditPersistKind persistKind)
{
    switch (choice)
    {
        case Core::UnsavedEditConfirmChoice::kCancel:
            return false;

        case Core::UnsavedEditConfirmChoice::kDiscard:
            return true;

        case Core::UnsavedEditConfirmChoice::kPersist:
            return patchManagerActionHandler_ != nullptr
                && patchManagerActionHandler_->tryPersistCurrentPatchFromUnsavedGate(persistKind);
    }

    return false;
}

bool PluginProcessor::isCurrentPatchAtRisk()
{
    if (dirtyPatchTracker_ == nullptr || patchModel_ == nullptr || apvtsPatchMapper_ == nullptr
        || patchNameSyncer_ == nullptr)
        return false;

    const bool isDirty = dirtyPatchTracker_->syncApvtsAndIsDirty(
        *apvtsPatchMapper_, *patchNameSyncer_, *patchModel_);
    const bool notStoredInRam = patchManagerActionHandler_ != nullptr
        && patchManagerActionHandler_->isPatchNotStoredInRam();

    return Core::UnsavedEditWarning::isAtRisk(isDirty, notStoredInRam);
}

bool PluginProcessor::confirmSessionCloseGateIfNeeded()
{
    return confirmUnsavedEditGateIfNeeded();
}

bool PluginProcessor::confirmUnsavedEditGateIfNeeded()
{
    using namespace PluginIDs::Settings::UnsavedStatePolicy;

    const int policy = static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kUnsavedStatePolicy,
        kDefault));

    if (dirtyPatchTracker_ == nullptr || patchModel_ == nullptr || apvtsPatchMapper_ == nullptr
        || patchNameSyncer_ == nullptr)
        return true;

    const bool isDirty = dirtyPatchTracker_->syncApvtsAndIsDirty(
        *apvtsPatchMapper_, *patchNameSyncer_, *patchModel_);
    const bool notStoredInRam = patchManagerActionHandler_ != nullptr
        && patchManagerActionHandler_->isPatchNotStoredInRam();

    if (! Core::UnsavedEditWarning::shouldPrompt(policy, isDirty, notStoredInRam))
        return true;

    // No UI gate wired (headless / tests) -> proceed without blocking.
    if (! unsavedEditConfirmModalGate_)
        return true;

    if (! requireMessageThreadForModalGate())
        return false;

    const auto persistKind = patchManagerActionHandler_ != nullptr
        ? patchManagerActionHandler_->resolveUnsavedEditPersistKind(isDirty)
        : Core::UnsavedEditPersistKind::kSaveAs;

    return applyUnsavedEditConfirmChoice(unsavedEditConfirmModalGate_(persistKind), persistKind);
}

bool PluginProcessor::confirmPatchContextChangeGate(bool includeUnsavedEditWarning)
{
    if (includeUnsavedEditWarning && ! confirmUnsavedEditGateIfNeeded())
        return false;

    // No mutations to lose -> proceed silently.
    if (patchMutatorEngine_ == nullptr || patchMutatorEngine_->rootCount() == 0)
        return true;

    // No UI gate wired (headless / tests) -> proceed without blocking.
    if (! mutatorHistoryGateModalGate_)
        return true;

    if (! requireMessageThreadForModalGate())
        return false;

    return runMutatorHistoryGateChoice();
}

bool PluginProcessor::runMutatorHistoryGateChoice()
{
    switch (mutatorHistoryGateModalGate_())
    {
        case Core::MutatorHistoryGateChoice::kCancel:
            return false;

        case Core::MutatorHistoryGateChoice::kDiscard:
            // Defer history clear until load fully succeeds (onPatchLoaded). Cancel at
            // name reconciliation must leave Mutator history intact.
            return true;

        case Core::MutatorHistoryGateChoice::kExport:
            // Only proceed when export completes; history clear still waits for onPatchLoaded.
            return runMutatorExportForGate();
    }

    return false;
}

void PluginProcessor::publishMutatorExportCancelledFooter()
{
    namespace Messages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

    apvts.state.setProperty("uiMessageText", juce::String(Messages::kExportCancelledFooter), nullptr);
    apvts.state.setProperty("uiMessageSeverity", juce::String("info"), nullptr);
}

bool PluginProcessor::resolveMutatorExportCollision(const juce::File& folder,
                                                    Core::MutatorActionResult& result)
{
    if (! mutatorExportCollisionModalGate_)
    {
        publishMutatorExportCancelledFooter();
        return false;
    }

    auto resolution = Core::ExportCollisionResolution::kCancel;
    bool resolved = false;
    // PluginEditor's collision gate runs its modal loop synchronously and invokes the
    // callback before returning, so capturing the resolution here is safe.
    mutatorExportCollisionModalGate_([&resolution, &resolved](Core::ExportCollisionResolution chosen)
    {
        resolution = chosen;
        resolved = true;
    });

    if (! resolved || resolution == Core::ExportCollisionResolution::kCancel)
    {
        publishMutatorExportCancelledFooter();
        return false;
    }

    result = patchMutatorEngine_->exportHistoryResolved(folder, resolution);
    return true;
}

bool PluginProcessor::runMutatorExportForGate()
{
    if (patchMutatorEngine_ == nullptr || ! mutatorExportFolderPicker_)
    {
        publishMutatorExportCancelledFooter();
        return false;
    }

    const juce::File folder = mutatorExportFolderPicker_();

    if (! folder.isDirectory())
    {
        publishMutatorExportCancelledFooter();
        return false;
    }

    auto result = patchMutatorEngine_->exportHistory(folder);

    if (result.exportCollisionModalRequested && ! resolveMutatorExportCollision(folder, result))
        return false;

    if (result.footerMessage.isNotEmpty())
    {
        apvts.state.setProperty("uiMessageText", result.footerMessage, nullptr);
        apvts.state.setProperty("uiMessageSeverity", result.footerSeverity, nullptr);
    }

    return result.success;
}
