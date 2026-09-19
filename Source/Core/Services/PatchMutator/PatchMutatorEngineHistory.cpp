// Extracted from PatchMutatorEngine.cpp for modular maintenance.
// Export, defrag, history list mirrors, and selection navigation.

#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/Services/PatchMutator/PatchMutatorEngineInternal.h"

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/HistoryDefragService.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using namespace PatchMutatorEngineInternal;

namespace Core
{

MutatorActionResult PatchMutatorEngine::exportHistory(const juce::File& destinationFolder)
{
    if (historyStore_.isEmpty())
        return makeExportWarningResult(kEmptyHistoryFooterMessage);

    if (patchFileService_ == nullptr || sysExEncoder_ == nullptr)
        return Core::MutatorActionResult{};

    if (! destinationFolder.isDirectory() || ! destinationFolder.hasWriteAccess())
        return makeExportWarningResult(kExportFolderNotWritableFooterMessage);

    if (! historyStore_.hasFrozenExportBasename())
        return makeExportHistoryResult(
            patchFileService_->exportMutatorHistory(
                destinationFolder, historyStore_, *sysExEncoder_, patchModel_->getName()),
            destinationFolder);

    const auto sessionFolder = destinationFolder.getChildFile(historyStore_.getFrozenExportBasename());
    if (sessionFolder.exists())
    {
        MutatorActionResult result;
        result.exportCollisionModalRequested = true;
        return result;
    }

    return runSessionExport(sessionFolder, false);
}

MutatorActionResult PatchMutatorEngine::exportHistoryResolved(const juce::File& destinationFolder,
                                                              ExportCollisionResolution resolution)
{
    if (resolution == ExportCollisionResolution::kCancel)
    {
        MutatorActionResult cancelled;
        cancelled.footerMessage = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages::kExportCancelledFooter;
        cancelled.footerSeverity = kFooterSeverityInfo;
        return cancelled;
    }

    if (historyStore_.isEmpty())
        return makeExportWarningResult(kEmptyHistoryFooterMessage);

    if (patchFileService_ == nullptr || sysExEncoder_ == nullptr || ! historyStore_.hasFrozenExportBasename())
        return Core::MutatorActionResult{};

    if (! destinationFolder.isDirectory() || ! destinationFolder.hasWriteAccess())
        return makeExportWarningResult(kExportFolderNotWritableFooterMessage);

    const auto basename = historyStore_.getFrozenExportBasename();
    const auto sessionFolder = resolution == ExportCollisionResolution::kKeep
        ? Core::PatchFileService::resolveKeepSessionFolder(destinationFolder, basename)
        : destinationFolder.getChildFile(basename);

    return runSessionExport(sessionFolder, resolution == ExportCollisionResolution::kOverwrite);
}

MutatorActionResult PatchMutatorEngine::runSessionExport(const juce::File& sessionFolder, bool clearExisting)
{
    const Core::MutatorHistorySessionExportArgs args {
        sessionFolder,
        historyStore_,
        *sysExEncoder_,
        clearExisting,
        patchModel_->getName()
    };
    return makeExportHistoryResult(patchFileService_->exportMutatorHistorySession(args),
                                   sessionFolder);
}

MutatorActionResult PatchMutatorEngine::defragHistory()
{
    if (historyStore_.isEmpty())
        return makeWarningResult(kEmptyHistoryFooterMessage);

    forceExitCompare();

    const auto selection = std::make_pair(selectedRootIndex_, selectedRetryIndex_);
    const auto defragResult = HistoryDefragService::defrag(historyStore_, selection);

    if (! defragResult.success)
        return MutatorActionResult{};

    selectedRootIndex_ = defragResult.remappedRootIndex;
    selectedRetryIndex_ = defragResult.remappedRetryIndex;
    syncHistoryUiProperties(apvts_);
    auditionAfterHistoryMutation();

    const int mutationsUsed = historyStore_.rootCount();
    const int mutationsAvailable = MutationHistoryStore::kMaxRoots - mutationsUsed;
    int retriesUsed = 0;
    int retriesAvailable = 0;
    for (const int rootIndex : historyStore_.getSortedRootIndices())
    {
        const int rootRetries = historyStore_.retryCount(rootIndex);
        retriesUsed += rootRetries;
        retriesAvailable += MutationHistoryStore::kMaxRetriesPerRoot - rootRetries;
    }

    namespace DefragMessages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

    MutatorActionResult result = makeSuccessResult();
    result.footerMessage = DefragMessages::formatDefragCompleteFooterMessage(
        mutationsUsed, mutationsAvailable, retriesUsed, retriesAvailable);
    result.footerSeverity = kFooterSeverityInfo;
    return result;
}

void PatchMutatorEngine::rebuildHistoryListMirrors()
{
    applySelectionFromApvts();
    syncHistoryUiProperties(apvts_);
}

juce::Array<PatchMutatorEngine::FlatHistoryEntry> PatchMutatorEngine::buildFlatHistoryEntries() const
{
    juce::Array<FlatHistoryEntry> flat;

    // INITIAL leads the walk whenever HISTORY offers it (same gate as the combo row).
    if (canOfferInitialSelection())
        flat.add({ kInitialFlatRootIndex, MutationHistoryStore::kRootOnly });

    for (const int rootIndex : historyStore_.getSortedRootIndices())
    {
        flat.add({ rootIndex, MutationHistoryStore::kRootOnly });
        for (const int retryIndex : historyStore_.getSortedRetryIndices(rootIndex))
            flat.add({ rootIndex, retryIndex });
    }
    return flat;
}

int PatchMutatorEngine::indexOfFlatHistoryEntry(const juce::Array<FlatHistoryEntry>& flat,
                                                int rootIndex,
                                                int retryIndex)
{
    for (int i = 0; i < flat.size(); ++i)
    {
        if (flat.getReference(i).rootIndex == rootIndex
            && flat.getReference(i).retryIndex == retryIndex)
        {
            return i;
        }
    }

    return -1;
}

void PatchMutatorEngine::commitHistorySelectionToApvts()
{
    // INITIAL first: when it changes without touching M/R it is the only notification the
    // panel and the audition dispatch get.
    auto& state = apvts_.state;
    state.setProperty(MutatorState::kInitialSelected, initialSelected_, nullptr);
    // Match panel write order so existing selection listeners rebuild + audition.
    state.setProperty(MutatorState::kSelectedRetryIndex, selectedRetryIndex_, nullptr);
    state.setProperty(MutatorState::kSelectedMutateRootIndex, selectedRootIndex_, nullptr);
}

void PatchMutatorEngine::advanceHistorySelection(bool isNext)
{
    if (readBoolProperty(apvts_.state, MutatorState::kCompareActive, false))
        return;

    applySelectionFromApvts();

    const auto flat = buildFlatHistoryEntries();
    if (flat.size() < 2)
        return;

    const int currentIndex = initialSelected_
                                 ? indexOfFlatHistoryEntry(flat,
                                                           kInitialFlatRootIndex,
                                                           MutationHistoryStore::kRootOnly)
                                 : indexOfFlatHistoryEntry(flat, selectedRootIndex_, selectedRetryIndex_);
    // Stale / unknown selection — do not pretend we are at flat[0] and jump.
    if (currentIndex < 0)
        return;

    const int count = flat.size();
    const int nextIndex = isNext
                              ? (currentIndex + 1) % count
                              : (currentIndex - 1 + count) % count;
    const auto& next = flat.getReference(nextIndex);

    if (next.isInitial())
    {
        // Keep the mutation selection so leaving INITIAL lands back on a real entry.
        initialSelected_ = true;
        commitHistorySelectionToApvts();
        return;
    }

    if (! initialSelected_ && next.rootIndex == selectedRootIndex_ && next.retryIndex == selectedRetryIndex_)
        return;

    initialSelected_ = false;
    selectedRootIndex_ = next.rootIndex;
    selectedRetryIndex_ = next.retryIndex;
    commitHistorySelectionToApvts();
}

void PatchMutatorEngine::auditionSelectedHistoryEntry()
{
    if (readBoolProperty(apvts_.state, MutatorState::kCompareActive, false))
        return;

    applySelectionFromApvts();

    if (historyStore_.isEmpty() || (selectedRootIndex_ < 0 && ! initialSelected_))
        return;

    const PatchModel auditionModel = resolveAuditionBuffer();

    if (! candidateDiffersFromLive(auditionModel))
        return;

    pushResultToEditorAndSynth(auditionModel);
}

void PatchMutatorEngine::clearEmptyHistoryUi(juce::AudioProcessorValueTreeState& apvts)
{
    auto& state = apvts.state;
    state.setProperty(MutatorState::kHistoryMutateList, juce::String(), nullptr);
    state.setProperty(MutatorState::kHistoryRetryList, juce::String(), nullptr);
    state.setProperty(MutatorState::kHistoryRetryListsByRoot, juce::String(), nullptr);
    state.setProperty(MutatorState::kSelectedMutateRootIndex, -1, nullptr);
    state.setProperty(MutatorState::kSelectedRetryIndex, MutationHistoryStore::kRootOnly, nullptr);
    // No mutations left — HISTORY hides INITIAL even when the snapshot survives (CLEAR).
    state.setProperty(MutatorState::kInitialSelected, false, nullptr);
    selectedRootIndex_ = -1;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    initialSelected_ = false;
    refreshActionEnabledMirrors(apvts);
}

void PatchMutatorEngine::writeHistoryListMirrors(juce::ValueTree& state)
{
    juce::StringArray mutateLabels;
    juce::StringArray retryListsByRoot;
    for (const int rootIndex : historyStore_.getSortedRootIndices())
    {
        mutateLabels.add(MutationNaming::formatRootLabel(rootIndex));

        juce::StringArray rootRetryLabels;
        rootRetryLabels.add(historyRootSentinelLabel());
        for (const int retryIndex : historyStore_.getSortedRetryIndices(rootIndex))
            rootRetryLabels.add(MutationNaming::formatRetryLabel(retryIndex));
        retryListsByRoot.add(juce::String(rootIndex) + "=" + joinLabels(rootRetryLabels));
    }
    state.setProperty(MutatorState::kHistoryMutateList, joinLabels(mutateLabels), nullptr);
    state.setProperty(MutatorState::kHistoryRetryListsByRoot, retryListsByRoot.joinIntoString(";"), nullptr);
}

void PatchMutatorEngine::writeSelectedHistoryUi(juce::ValueTree& state, const juce::Array<int>& roots)
{
    int selectedMutateRootIndex = selectedRootIndex_;
    if (selectedMutateRootIndex < 0 || ! historyStore_.hasRoot(selectedMutateRootIndex))
        selectedMutateRootIndex = roots.getLast();

    selectedRootIndex_ = selectedMutateRootIndex;

    const auto retries = historyStore_.getSortedRetryIndices(selectedMutateRootIndex);
    if (selectedRetryIndex_ != MutationHistoryStore::kRootOnly
        && ! historyStore_.hasRetry(selectedMutateRootIndex, selectedRetryIndex_))
    {
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    }

    // Write selection before the retry list so any listener that rebuilds on
    // kHistoryRetryList already sees the matching selected mutate root.
    state.setProperty(MutatorState::kSelectedMutateRootIndex, selectedMutateRootIndex, nullptr);

    juce::StringArray retryLabels;
    retryLabels.add(historyRootSentinelLabel());
    for (const int retryIndex : retries)
        retryLabels.add(MutationNaming::formatRetryLabel(retryIndex));
    state.setProperty(MutatorState::kHistoryRetryList, joinLabels(retryLabels), nullptr);
    state.setProperty(MutatorState::kSelectedRetryIndex, selectedRetryIndex_, nullptr);
}

void PatchMutatorEngine::syncHistoryUiProperties(juce::AudioProcessorValueTreeState& apvts)
{
    SuppressMutatorHistorySelectionDebounceGuard suppressGuard(hooks_);

    auto& state = apvts.state;

    if (! state.hasProperty(MutatorState::kCompareActive))
        state.setProperty(MutatorState::kCompareActive, false, nullptr);

    state.setProperty(MutatorState::kInitialSnapshotAvailable,
                      historyStore_.hasInitialSnapshot(),
                      nullptr);

    // Selection → list rebuild goes through rebuildHistoryListMirrors() which calls
    // applySelectionFromApvts() first. Do not self-heal here: mutate/retry/delete set
    // engine members then sync while APVTS may still hold the previous selection.

    const auto roots = historyStore_.getSortedRootIndices();
    if (roots.isEmpty())
    {
        clearEmptyHistoryUi(apvts);
        return;
    }

    // Engine members are the truth here — publish INITIAL before the enabled mirrors read
    // the selection back from APVTS. Compare shows the origin too, so it keeps the row.
    state.setProperty(MutatorState::kInitialSelected,
                      initialSelected_ || readBoolProperty(state, MutatorState::kCompareActive, false),
                      nullptr);
    writeHistoryListMirrors(state);
    writeSelectedHistoryUi(state, roots);
    refreshActionEnabledMirrors(apvts);
}

void PatchMutatorEngine::refreshActionEnabledMirrors(juce::AudioProcessorValueTreeState& apvts)
{
    applySelectionFromApvts();

    const auto recipe = buildRecipeFromApvts();
    auto& state = apvts.state;
    state.setProperty(MutatorState::kMutateEnabled,
                      computeMutateEnabled(historyStore_, recipe),
                      nullptr);
    // INITIAL is the origin, not a history entry: RETRY / DELETE have nothing to act on.
    const int selectedEntryRootIndex = initialSelected_ ? -1 : selectedRootIndex_;
    state.setProperty(MutatorState::kRetryEnabled,
                      computeRetryEnabled(historyStore_, selectedEntryRootIndex),
                      nullptr);
    state.setProperty(MutatorState::kExportEnabled, computeExportEnabled(historyStore_), nullptr);
    state.setProperty(MutatorState::kDeleteEnabled,
                      computeDeleteEnabled(historyStore_, selectedEntryRootIndex),
                      nullptr);
    state.setProperty(MutatorState::kClearEnabled, computeExportEnabled(historyStore_), nullptr);
}

std::pair<int, int> PatchMutatorEngine::resolveSelectionAfterDelete(int rootIndex,
                                                                    int retryIndex,
                                                                    bool isRetryDelete)
{
    if (isRetryDelete)
    {
        const auto retries = historyStore_.getSortedRetryIndices(rootIndex);
        for (int i = 0; i < retries.size(); ++i)
        {
            if (retries[i] != retryIndex)
                continue;

            if (i > 0)
                return { rootIndex, retries[i - 1] };

            return { rootIndex, MutationHistoryStore::kRootOnly };
        }

        return { rootIndex, MutationHistoryStore::kRootOnly };
    }

    const auto roots = historyStore_.getSortedRootIndices();
    for (int i = 0; i < roots.size(); ++i)
    {
        if (roots[i] != rootIndex)
            continue;

        if (i > 0)
            return { roots[i - 1], MutationHistoryStore::kRootOnly };

        return { -1, MutationHistoryStore::kRootOnly };
    }

    return { -1, MutationHistoryStore::kRootOnly };
}

} // namespace Core
