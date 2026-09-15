// Extracted from PatchMutatorEngine.cpp for modular maintenance.
// MUTATE / RETRY / COMPARE / DELETE / CLEAR / RESET session actions.

#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/Services/PatchMutator/PatchMutatorEngineInternal.h"

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/MutationNaming.h"

using namespace PatchMutatorEngineInternal;

namespace Core
{

namespace
{
void assignDeleteSuccessFooter(MutatorActionResult& result,
                               int rootIndex,
                               int retryIndex,
                               bool isRetryDelete)
{
    result.footerMessage = isRetryDelete
        ? formatRetryDeleteFooterMessage(MutationNaming::formatPatchName(rootIndex, retryIndex))
        : formatRootDeleteCascadeFooterMessage(MutationNaming::formatRootLabel(rootIndex));
    result.footerSeverity = kFooterSeverityInfo;
}
} // namespace

const char* PatchMutatorEngine::footerForDiverseMutation(DiverseMutationOutcome outcome) noexcept
{
    switch (outcome)
    {
        case DiverseMutationOutcome::kApplied:
            return nullptr;
        case DiverseMutationOutcome::kTooSimilar:
            return kRetryTooSimilarFooterMessage;
        case DiverseMutationOutcome::kNoUsableRoll:
            return kNoMutationChangeFooterMessage;
    }

    return kNoMutationChangeFooterMessage;
}

MutatorActionResult PatchMutatorEngine::mutate()
{
    applySelectionFromApvts();

    if (historyStore_.isRootSlotsFull() || historyStore_.isRootIndexExhausted())
        return makeHistoryLimitResult();

    const auto recipe = buildRecipeFromApvts();
    if (const auto recipeError = validateMutationRecipe(recipe))
        return *recipeError;

    const PatchModel auditionBuffer = resolveAuditionBuffer();

    if (! historyStore_.hasInitialSnapshot())
    {
        historyStore_.setInitialSnapshot(auditionBuffer);
        freezeExportBasename(auditionBuffer);
    }

    PatchModel parentSnapshot;
    parentSnapshot.loadFrom(auditionBuffer.data());

    PatchModel working;
    working.loadFrom(auditionBuffer.data());

    if (! applyRecipeMutation(working, recipe))
        return makeWarningResult(kNoMutationChangeFooterMessage);

    const auto rootIndexOpt = historyStore_.peekNextRootIndex();
    if (! rootIndexOpt.has_value())
        return makeHistoryLimitResult();

    const int rootIndex = *rootIndexOpt;

    if (! historyStore_.insertRoot(rootIndex, working, parentSnapshot))
        return makeHistoryLimitResult();

    pushResultToEditorAndSynth(working);

    if (hooks_.onEditorialCheckpoint)
        hooks_.onEditorialCheckpoint();

    selectedRootIndex_ = rootIndex;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    initialSelected_ = false;
    syncHistoryUiProperties(apvts_);

    return makeSuccessResult();
}

MutatorActionResult PatchMutatorEngine::retry()
{
    applySelectionFromApvts();

    int rootIndex = -1;
    if (const auto preError = validateRetryPreconditions(rootIndex))
        return *preError;

    const auto selectedEntry = resolveSelectedEntryForRetry(rootIndex);
    if (! selectedEntry.has_value())
        return makeWarningResult(kNoSelectionFooterMessage);

    const auto recipe = buildRecipeFromApvts();
    if (const auto recipeError = validateMutationRecipe(recipe))
        return *recipeError;

    PatchModel parentSnapshot;
    parentSnapshot.loadFrom(selectedEntry->parentSnapshot.data());

    PatchModel working;
    working.loadFrom(selectedEntry->parentSnapshot.data());

    PatchModel previousResult;
    previousResult.loadFrom(selectedEntry->result.data());

    if (const auto* diversityFooter = footerForDiverseMutation(
            applyDiverseRecipeMutation(working, recipe, previousResult)))
    {
        return makeWarningResult(diversityFooter);
    }

    const auto retryIndexOpt = historyStore_.peekNextRetryIndex(rootIndex);
    if (! retryIndexOpt.has_value())
        return makeHistoryLimitResult();

    const int retryIndex = *retryIndexOpt;

    if (! historyStore_.insertRetry(rootIndex, retryIndex, working, parentSnapshot))
        return makeHistoryLimitResult();

    pushResultToEditorAndSynth(working);

    if (hooks_.onEditorialCheckpoint)
        hooks_.onEditorialCheckpoint();

    selectedRetryIndex_ = retryIndex;
    initialSelected_ = false;
    syncHistoryUiProperties(apvts_);

    return makeSuccessResult();
}

MutatorActionResult PatchMutatorEngine::exitCompareMode()
{
    auto& state = apvts_.state;
    state.setProperty(MutatorState::kCompareActive, false, nullptr);
    {
        SuppressMutatorHistorySelectionDebounceGuard suppressGuard(hooks_);
        state.setProperty(MutatorState::kInitialSelected, false, nullptr);
        state.setProperty(MutatorState::kSelectedMutateRootIndex, compareSavedMutateRootIndex_, nullptr);
        state.setProperty(MutatorState::kSelectedRetryIndex, compareSavedRetryIndex_, nullptr);
    }
    applySelectionFromApvts();

    const PatchModel auditionModel = resolveAuditionBuffer();
    if (candidateDiffersFromLive(auditionModel))
        pushResultToEditorAndSynth(auditionModel);

    clearCompareLockedFooterIfPresent(apvts_);
    return makeSuccessResult();
}

MutatorActionResult PatchMutatorEngine::enterCompareMode()
{
    if (historyStore_.isEmpty())
        return makeWarningResult(kEmptyHistoryFooterMessage);

    if (! historyStore_.hasInitialSnapshot())
        return makeWarningResult(kNoInitialSnapshotFooterMessage);

    // Manual INITIAL already auditions the origin — Compare would be a no-op lock.
    if (initialSelected_)
        return makeWarningResult(kNoSelectionFooterMessage);

    compareSavedMutateRootIndex_ = selectedRootIndex_;
    compareSavedRetryIndex_ = selectedRetryIndex_;

    apvts_.state.setProperty(MutatorState::kCompareActive, true, nullptr);
    {
        // Compare shows the origin, so HISTORY must display INITIAL too. Guarded: the
        // Compare lock already drives the panel refresh — no extra audition dispatch.
        SuppressMutatorHistorySelectionDebounceGuard suppressGuard(hooks_);
        apvts_.state.setProperty(MutatorState::kInitialSelected, true, nullptr);
    }

    const PatchModel initialSnapshot = historyStore_.getInitialSnapshot();
    if (candidateDiffersFromLive(initialSnapshot))
        pushResultToEditorAndSynth(initialSnapshot);

    MutatorActionResult result = makeSuccessResult();
    result.footerMessage = CompareMessages::kCompareLockedFooter;
    result.footerSeverity = kFooterSeverityInfo;
    return result;
}

MutatorActionResult PatchMutatorEngine::toggleCompare()
{
    applySelectionFromApvts();

    if (readBoolProperty(apvts_.state, MutatorState::kCompareActive, false))
        return exitCompareMode();

    return enterCompareMode();
}

MutatorActionResult PatchMutatorEngine::deleteSelected()
{
    applySelectionFromApvts();

    if (historyStore_.isEmpty())
        return makeWarningResult(kEmptyHistoryFooterMessage);

    // The origin snapshot is not a history entry — DELETE never applies to it.
    if (initialSelected_)
        return makeWarningResult(kNoSelectionFooterMessage);

    const int mutateRootIndex = selectedRootIndex_;
    if (mutateRootIndex < 0 || ! historyStore_.hasRoot(mutateRootIndex))
        return makeWarningResult(kNoSelectionFooterMessage);

    forceExitCompare();

    const int retryIndex = selectedRetryIndex_;
    int newMutateRootIndex = mutateRootIndex;
    int newRetryIndex = MutationHistoryStore::kRootOnly;
    MutatorActionResult result;

    if (retryIndex != MutationHistoryStore::kRootOnly && historyStore_.hasRetry(mutateRootIndex, retryIndex))
    {
        std::tie(newMutateRootIndex, newRetryIndex) = resolveSelectionAfterDelete(mutateRootIndex, retryIndex, true);

        if (! historyStore_.deleteRetry(mutateRootIndex, retryIndex))
            return makeWarningResult(kNoSelectionFooterMessage);

        assignDeleteSuccessFooter(result, mutateRootIndex, retryIndex, true);
    }
    else
    {
        std::tie(newMutateRootIndex, newRetryIndex) = resolveSelectionAfterDelete(mutateRootIndex,
                                                                                  MutationHistoryStore::kRootOnly,
                                                                                  false);

        if (! historyStore_.deleteRoot(mutateRootIndex))
            return makeWarningResult(kNoSelectionFooterMessage);

        assignDeleteSuccessFooter(result, mutateRootIndex, MutationHistoryStore::kRootOnly, false);
    }

    selectedRootIndex_ = newMutateRootIndex;
    selectedRetryIndex_ = newRetryIndex;
    syncHistoryUiProperties(apvts_);
    auditionAfterHistoryMutation();

    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::clearHistory()
{
    if (historyStore_.isEmpty())
        return makeWarningResult(kEmptyHistoryFooterMessage);

    forceExitCompare();
    historyStore_.clear();
    selectedRootIndex_ = -1;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    syncHistoryUiProperties(apvts_);
    auditionAfterHistoryMutation();

    MutatorActionResult result = makeSuccessResult();
    result.footerMessage = kFlushSuccessFooterMessage;
    result.footerSeverity = kFooterSeverityInfo;
    return result;
}

MutatorActionResult PatchMutatorEngine::resetSessionForPatchLoad()
{
    forceExitCompare();
    historyStore_.clear();
    historyStore_.clearInitialSnapshot();
    selectedRootIndex_ = -1;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    syncHistoryUiProperties(apvts_);
    return makeSuccessResult();
}

} // namespace Core
