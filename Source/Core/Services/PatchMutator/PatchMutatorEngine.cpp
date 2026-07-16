#include "Core/Services/PatchMutator/PatchMutatorEngine.h"

#include <cstring>

#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/HistoryDefragService.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    constexpr const char* kNoOpRecipeFooterMessage = "Set Amount and Random above 0 to mutate.";
    constexpr const char* kNoModuleScopeFooterMessage = "Enable at least one module to mutate.";
    constexpr const char* kNoMutationChangeFooterMessage =
        "Mutation made no changes. Try higher Amount or Random.";
    constexpr const char* kHistoryLimitFooterMessage = "Mutation history is full. Defrag to continue.";
    constexpr const char* kEmptyHistoryFooterMessage = "Mutation history is empty.";
    constexpr const char* kNoSelectionFooterMessage = "No valid mutation history entry selected.";
    constexpr const char* kNoInitialSnapshotFooterMessage =
        "No initial patch snapshot available for compare.";
    constexpr const char* kRootDeleteCascadeFooterPrefix = "Deleted ";
    constexpr const char* kRootDeleteCascadeFooterSuffix = " and all retries.";
    constexpr const char* kDefragCompleteFooterMessage = "Mutation history renumbered.";
    constexpr const char* kExportFolderNotWritableFooterMessage = "Export folder is not writable.";
    constexpr const char* kExportFailedFooterMessage = "Mutation export failed.";
    constexpr const char* kFooterSeverityWarning = "warning";
    constexpr const char* kFooterSeverityInfo = "info";

    namespace CompareMessages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

    void clearCompareLockedFooterIfPresent(juce::AudioProcessorValueTreeState& apvts)
    {
        if (apvts.state.getProperty("uiMessageText").toString()
            == juce::String(CompareMessages::kCompareLockedFooter))
        {
            apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
            apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
        }
    }

    juce::String formatExportCompleteFooterMessage(int filesWritten)
    {
        return "Exported " + juce::String(filesWritten) + " mutation file(s).";
    }

    Core::MutatorActionResult makeExportWarningResult(const char* message)
    {
        Core::MutatorActionResult result;
        result.footerMessage = message;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    Core::MutatorActionResult makeExportHistoryResult(const Core::PatchFileExportResult& exportResult)
    {
        Core::MutatorActionResult result;

        if (! exportResult.success)
        {
            result.footerMessage = exportResult.errorMessage.isNotEmpty()
                ? exportResult.errorMessage
                : juce::String(kExportFailedFooterMessage);
            result.footerSeverity = kFooterSeverityWarning;
            return result;
        }

        result.success = true;
        result.footerMessage = formatExportCompleteFooterMessage(exportResult.filesWritten);
        result.footerSeverity = kFooterSeverityInfo;
        return result;
    }

    class SuppressMutatorHistorySelectionDebounceGuard
    {
    public:
        explicit SuppressMutatorHistorySelectionDebounceGuard(Core::ActionExecutionHooks& hooks)
            : hooks_(hooks)
        {
            if (hooks_.setSuppressMutatorHistorySelectionDebounce)
                hooks_.setSuppressMutatorHistorySelectionDebounce(true);
        }

        ~SuppressMutatorHistorySelectionDebounceGuard()
        {
            if (hooks_.setSuppressMutatorHistorySelectionDebounce)
                hooks_.setSuppressMutatorHistorySelectionDebounce(false);
        }

    private:
        Core::ActionExecutionHooks& hooks_;
    };

    void setPatchLoadSuppressHooks(Core::ActionExecutionHooks& hooks, bool suppress)
    {
        if (hooks.setSuppressPatchSysEx)
            hooks.setSuppressPatchSysEx(suppress);

        if (hooks.setSuppressMatrixModSysEx)
            hooks.setSuppressMatrixModSysEx(suppress);
    }

    void flushDeferredApvtsParameterSync(juce::AudioProcessorValueTreeState& apvts)
    {
        (void) apvts.copyState();
    }

    // Pipe-separated root/retry labels — e.g. "M00|M02|M05", "—|R00|R02".
    juce::String joinLabels(const juce::StringArray& labels)
    {
        return labels.joinIntoString("|");
    }

    juce::String historyRootSentinelLabel()
    {
        return PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryRootSentinel;
    }

    int readPatchMutatorPercent(juce::AudioProcessorValueTreeState& apvts,
                                const juce::Identifier& propertyId,
                                int defaultValue)
    {
        auto* rawValue = apvts.getRawParameterValue(propertyId);
        if (rawValue != nullptr)
            return juce::jlimit(0, 100, juce::roundToInt(rawValue->load()));

        return juce::jlimit(0, 100, static_cast<int>(apvts.state.getProperty(propertyId, defaultValue)));
    }

    void pushPatchModelToApvtsWithSuppress(juce::AudioProcessorValueTreeState& apvts,
                                           Core::ActionExecutionHooks& hooks,
                                           Core::ApvtsPatchMapper& mapper,
                                           Core::PatchNameSyncer* patchNameSyncer)
    {
        setPatchLoadSuppressHooks(hooks, true);
        mapper.bufferToApvts();

        if (patchNameSyncer != nullptr)
            patchNameSyncer->bufferToApvts();

        flushDeferredApvtsParameterSync(apvts);
        setPatchLoadSuppressHooks(hooks, false);
    }
} // namespace

namespace Core
{

PatchMutatorEngine::PatchMutatorEngine(PatchModel* patchModel,
                                       ApvtsPatchMapper* apvtsPatchMapper,
                                       PatchNameSyncer* patchNameSyncer,
                                       MidiManager* midiManager,
                                       juce::AudioProcessorValueTreeState& apvts,
                                       ActionExecutionHooks hooks,
                                       std::function<int()> getCurrentPatchNumber,
                                       PatchFileService* patchFileService,
                                       SysExEncoder* sysExEncoder)
    : patchModel_(patchModel)
    , apvtsPatchMapper_(apvtsPatchMapper)
    , patchNameSyncer_(patchNameSyncer)
    , midiManager_(midiManager)
    , apvts_(apvts)
    , hooks_(std::move(hooks))
    , getCurrentPatchNumber_(std::move(getCurrentPatchNumber))
    , patchFileService_(patchFileService)
    , sysExEncoder_(sysExEncoder)
{
    jassert(patchModel_ != nullptr);
    jassert(apvtsPatchMapper_ != nullptr);
}

MutatorActionResult PatchMutatorEngine::mutate()
{
    applySelectionFromApvts();

    if (historyStore_.isRootSlotsFull() || historyStore_.isRootIndexExhausted())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const auto recipe = buildRecipeFromApvts();
    if (recipe.amountPercent == 0 || recipe.randomPercent == 0)
    {
        MutatorActionResult result;
        result.footerMessage = kNoOpRecipeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    if (! recipe.hasAnyModuleEnabled())
    {
        MutatorActionResult result;
        result.footerMessage = kNoModuleScopeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

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

    rng_.setSeedRandomly();
    JuceRandomSource rngSource(rng_);
    const bool mutated = algorithm_.apply(working, recipe, rngSource);
    if (! mutated)
    {
        MutatorActionResult result;
        result.footerMessage = kNoMutationChangeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto rootIndexOpt = historyStore_.peekNextRootIndex();
    if (! rootIndexOpt.has_value())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const int rootIndex = *rootIndexOpt;
    MutationNaming::applyPatchName(working, rootIndex);

    if (! historyStore_.insertRoot(rootIndex, working, parentSnapshot))
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    pushResultToEditorAndSynth(working);

    selectedRootIndex_ = rootIndex;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    syncHistoryUiProperties(apvts_);

    MutatorActionResult result;
    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::retry()
{
    applySelectionFromApvts();

    if (historyStore_.isEmpty())
    {
        MutatorActionResult result;
        result.footerMessage = kEmptyHistoryFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto rootIndexOpt = resolveSelectedRootIndex();
    if (! rootIndexOpt.has_value() || ! historyStore_.hasRoot(*rootIndexOpt))
    {
        MutatorActionResult result;
        result.footerMessage = kNoSelectionFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const int rootIndex = *rootIndexOpt;

    if (historyStore_.isRetrySlotsFull(rootIndex) || historyStore_.isRetryIndexExhausted(rootIndex))
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const auto selectedEntry = resolveSelectedEntryForRetry(rootIndex);
    if (! selectedEntry.has_value())
    {
        MutatorActionResult result;
        result.footerMessage = kNoSelectionFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto recipe = buildRecipeFromApvts();
    if (recipe.amountPercent == 0 || recipe.randomPercent == 0)
    {
        MutatorActionResult result;
        result.footerMessage = kNoOpRecipeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    if (! recipe.hasAnyModuleEnabled())
    {
        MutatorActionResult result;
        result.footerMessage = kNoModuleScopeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    PatchModel parentSnapshot;
    parentSnapshot.loadFrom(selectedEntry->parentSnapshot.data());

    PatchModel working;
    working.loadFrom(selectedEntry->parentSnapshot.data());

    rng_.setSeedRandomly();
    JuceRandomSource rngSource(rng_);
    const bool mutated = algorithm_.apply(working, recipe, rngSource);
    if (! mutated)
    {
        MutatorActionResult result;
        result.footerMessage = kNoMutationChangeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto retryIndexOpt = historyStore_.peekNextRetryIndex(rootIndex);
    if (! retryIndexOpt.has_value())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const int retryIndex = *retryIndexOpt;
    MutationNaming::applyPatchName(working, rootIndex, retryIndex);

    if (! historyStore_.insertRetry(rootIndex, retryIndex, working, parentSnapshot))
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    pushResultToEditorAndSynth(working);

    selectedRetryIndex_ = retryIndex;
    syncHistoryUiProperties(apvts_);

    MutatorActionResult result;
    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::toggleCompare()
{
    applySelectionFromApvts();

    auto& state = apvts_.state;
    const bool currentlyActive = readBoolProperty(state, MutatorState::kCompareActive, false);

    if (currentlyActive)
    {
        state.setProperty(MutatorState::kCompareActive, false, nullptr);
        {
            SuppressMutatorHistorySelectionDebounceGuard suppressGuard(hooks_);
            state.setProperty(MutatorState::kSelectedMutateRootIndex, compareSavedMutateRootIndex_, nullptr);
            state.setProperty(MutatorState::kSelectedRetryIndex, compareSavedRetryIndex_, nullptr);
        }
        applySelectionFromApvts();

        const PatchModel auditionModel = resolveAuditionBuffer();
        if (std::memcmp(auditionModel.data(), patchModel_->data(), PatchModel::kBufferSize) != 0)
            pushResultToEditorAndSynth(auditionModel);

        clearCompareLockedFooterIfPresent(apvts_);

        MutatorActionResult result;
        result.success = true;
        return result;
    }

    if (historyStore_.isEmpty())
    {
        MutatorActionResult result;
        result.footerMessage = kEmptyHistoryFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    if (! historyStore_.hasInitialSnapshot())
    {
        MutatorActionResult result;
        result.footerMessage = kNoInitialSnapshotFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    compareSavedMutateRootIndex_ = selectedRootIndex_;
    compareSavedRetryIndex_ = selectedRetryIndex_;

    state.setProperty(MutatorState::kCompareActive, true, nullptr);

    const PatchModel initialSnapshot = historyStore_.getInitialSnapshot();
    if (std::memcmp(initialSnapshot.data(), patchModel_->data(), PatchModel::kBufferSize) != 0)
        pushResultToEditorAndSynth(initialSnapshot);

    MutatorActionResult result;
    result.success = true;
    result.footerMessage = CompareMessages::kCompareLockedFooter;
    result.footerSeverity = kFooterSeverityInfo;
    return result;
}

MutatorActionResult PatchMutatorEngine::deleteSelected()
{
    applySelectionFromApvts();

    if (historyStore_.isEmpty())
    {
        MutatorActionResult result;
        result.footerMessage = kEmptyHistoryFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const int mutateRootIndex = selectedRootIndex_;
    if (mutateRootIndex < 0 || ! historyStore_.hasRoot(mutateRootIndex))
    {
        MutatorActionResult result;
        result.footerMessage = kNoSelectionFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    forceExitCompare();

    const int retryIndex = selectedRetryIndex_;
    int newMutateRootIndex = mutateRootIndex;
    int newRetryIndex = MutationHistoryStore::kRootOnly;
    MutatorActionResult result;

    if (retryIndex != MutationHistoryStore::kRootOnly && historyStore_.hasRetry(mutateRootIndex, retryIndex))
    {
        std::tie(newMutateRootIndex, newRetryIndex) = resolveSelectionAfterDelete(mutateRootIndex, retryIndex, true);

        if (! historyStore_.deleteRetry(mutateRootIndex, retryIndex))
        {
            result.footerMessage = kNoSelectionFooterMessage;
            result.footerSeverity = kFooterSeverityWarning;
            return result;
        }
    }
    else
    {
        std::tie(newMutateRootIndex, newRetryIndex) = resolveSelectionAfterDelete(mutateRootIndex,
                                                                                  MutationHistoryStore::kRootOnly,
                                                                                  false);

        if (! historyStore_.deleteRoot(mutateRootIndex))
        {
            result.footerMessage = kNoSelectionFooterMessage;
            result.footerSeverity = kFooterSeverityWarning;
            return result;
        }

        result.footerMessage = kRootDeleteCascadeFooterPrefix
                               + MutationNaming::formatRootLabel(mutateRootIndex)
                               + kRootDeleteCascadeFooterSuffix;
        result.footerSeverity = kFooterSeverityInfo;
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
    forceExitCompare();
    historyStore_.clear();
    selectedRootIndex_ = -1;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    syncHistoryUiProperties(apvts_);
    auditionAfterHistoryMutation();

    MutatorActionResult result;
    result.success = true;
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

    MutatorActionResult result;
    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::exportHistory(const juce::File& destinationFolder)
{
    if (historyStore_.isEmpty())
        return makeExportWarningResult(kEmptyHistoryFooterMessage);

    if (patchFileService_ == nullptr || sysExEncoder_ == nullptr)
        return Core::MutatorActionResult{};

    if (! destinationFolder.isDirectory() || ! destinationFolder.hasWriteAccess())
        return makeExportWarningResult(kExportFolderNotWritableFooterMessage);

    if (! historyStore_.hasFrozenExportBasename())
        return makeExportHistoryResult(patchFileService_->exportMutatorHistory(
            destinationFolder, historyStore_, *sysExEncoder_));

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
    return makeExportHistoryResult(patchFileService_->exportMutatorHistorySession(
        sessionFolder, historyStore_, *sysExEncoder_, clearExisting));
}

void PatchMutatorEngine::freezeExportBasename(const PatchModel& snapshot)
{
    if (! patchLoadContextProvider_)
        return;

    const auto context = patchLoadContextProvider_();
    historyStore_.setFrozenExportBasename(context.computeExportBasename(snapshot.getName()));
}

void PatchMutatorEngine::setPatchLoadContextProvider(std::function<PatchLoadContext()> provider)
{
    patchLoadContextProvider_ = std::move(provider);
}

MutatorActionResult PatchMutatorEngine::defragHistory()
{
    MutatorActionResult result;

    if (historyStore_.isEmpty())
    {
        result.footerMessage = kEmptyHistoryFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    forceExitCompare();

    const auto selection = std::make_pair(selectedRootIndex_, selectedRetryIndex_);
    const auto defragResult = HistoryDefragService::defrag(historyStore_, selection);

    if (! defragResult.success)
        return result;

    selectedRootIndex_ = defragResult.remappedRootIndex;
    selectedRetryIndex_ = defragResult.remappedRetryIndex;
    syncHistoryUiProperties(apvts_);
    auditionAfterHistoryMutation();

    result.success = true;
    result.footerMessage = kDefragCompleteFooterMessage;
    result.footerSeverity = kFooterSeverityInfo;
    return result;
}

void PatchMutatorEngine::rebuildHistoryListMirrors()
{
    applySelectionFromApvts();
    syncHistoryUiProperties(apvts_);
}

void PatchMutatorEngine::advanceHistorySelection(bool isNext)
{
    if (readBoolProperty(apvts_.state, MutatorState::kCompareActive, false))
        return;

    applySelectionFromApvts();

    struct FlatEntry
    {
        int rootIndex = -1;
        int retryIndex = MutationHistoryStore::kRootOnly;
    };

    juce::Array<FlatEntry> flat;
    for (const int rootIndex : historyStore_.getSortedRootIndices())
    {
        flat.add({ rootIndex, MutationHistoryStore::kRootOnly });
        for (const int retryIndex : historyStore_.getSortedRetryIndices(rootIndex))
            flat.add({ rootIndex, retryIndex });
    }

    if (flat.isEmpty())
        return;

    // Circular nav is a no-op with a single entry — keep selection unchanged.
    if (flat.size() < 2)
        return;

    int currentIndex = -1;
    for (int i = 0; i < flat.size(); ++i)
    {
        if (flat.getReference(i).rootIndex == selectedRootIndex_
            && flat.getReference(i).retryIndex == selectedRetryIndex_)
        {
            currentIndex = i;
            break;
        }
    }

    // Stale / unknown selection — do not pretend we are at flat[0] and jump.
    if (currentIndex < 0)
        return;

    const int count = flat.size();
    const int nextIndex = isNext
                              ? (currentIndex + 1) % count
                              : (currentIndex - 1 + count) % count;
    const auto& next = flat.getReference(nextIndex);

    if (next.rootIndex == selectedRootIndex_ && next.retryIndex == selectedRetryIndex_)
        return;

    selectedRootIndex_ = next.rootIndex;
    selectedRetryIndex_ = next.retryIndex;

    // Match panel write order so existing selection listeners rebuild + audition.
    auto& state = apvts_.state;
    state.setProperty(MutatorState::kSelectedRetryIndex, selectedRetryIndex_, nullptr);
    state.setProperty(MutatorState::kSelectedMutateRootIndex, selectedRootIndex_, nullptr);
}

void PatchMutatorEngine::auditionSelectedHistoryEntry()
{
    if (readBoolProperty(apvts_.state, MutatorState::kCompareActive, false))
        return;

    applySelectionFromApvts();

    if (historyStore_.isEmpty() || selectedRootIndex_ < 0)
        return;

    const PatchModel auditionModel = resolveAuditionBuffer();

    if (std::memcmp(auditionModel.data(), patchModel_->data(), PatchModel::kBufferSize) == 0)
        return;

    pushResultToEditorAndSynth(auditionModel);
}

void PatchMutatorEngine::syncHistoryUiProperties(juce::AudioProcessorValueTreeState& apvts)
{
    SuppressMutatorHistorySelectionDebounceGuard suppressGuard(hooks_);

    auto& state = apvts.state;

    if (! state.hasProperty(MutatorState::kCompareActive))
        state.setProperty(MutatorState::kCompareActive, false, nullptr);

    // Selection → list rebuild goes through rebuildHistoryListMirrors() which calls
    // applySelectionFromApvts() first. Do not self-heal here: mutate/retry/delete set
    // engine members then sync while APVTS may still hold the previous selection.

    const auto roots = historyStore_.getSortedRootIndices();
    if (roots.isEmpty())
    {
        state.setProperty(MutatorState::kHistoryMutateList, juce::String(), nullptr);
        state.setProperty(MutatorState::kHistoryRetryList, juce::String(), nullptr);
        state.setProperty(MutatorState::kHistoryRetryListsByRoot, juce::String(), nullptr);
        state.setProperty(MutatorState::kSelectedMutateRootIndex, -1, nullptr);
        state.setProperty(MutatorState::kSelectedRetryIndex, MutationHistoryStore::kRootOnly, nullptr);
        selectedRootIndex_ = -1;
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
        refreshActionEnabledMirrors(apvts);
        return;
    }

    juce::StringArray mutateLabels;
    juce::StringArray retryListsByRoot;
    for (const int rootIndex : roots)
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
    refreshActionEnabledMirrors(apvts);
}

namespace
{
    bool computeMutateEnabled(const Core::MutationHistoryStore& store,
                              const Core::MutationRecipe& recipe)
    {
        return store.peekNextRootIndex().has_value() && recipe.hasAnyModuleEnabled();
    }

    bool computeExportEnabled(const Core::MutationHistoryStore& store)
    {
        return ! store.isEmpty();
    }

    bool computeDeleteEnabled(const Core::MutationHistoryStore& store, int selectedRoot)
    {
        return ! store.isEmpty() && selectedRoot >= 0;
    }

    bool computeRetryEnabled(const Core::MutationHistoryStore& store, int selectedRoot)
    {
        if (store.isEmpty() || selectedRoot < 0)
            return false;

        return store.peekNextRetryIndex(selectedRoot).has_value();
    }
} // namespace

void PatchMutatorEngine::refreshActionEnabledMirrors(juce::AudioProcessorValueTreeState& apvts)
{
    applySelectionFromApvts();

    const auto recipe = buildRecipeFromApvts();
    auto& state = apvts.state;
    state.setProperty(MutatorState::kMutateEnabled,
                      computeMutateEnabled(historyStore_, recipe),
                      nullptr);
    state.setProperty(MutatorState::kRetryEnabled,
                      computeRetryEnabled(historyStore_, selectedRootIndex_),
                      nullptr);
    state.setProperty(MutatorState::kExportEnabled, computeExportEnabled(historyStore_), nullptr);
    state.setProperty(MutatorState::kDeleteEnabled,
                      computeDeleteEnabled(historyStore_, selectedRootIndex_),
                      nullptr);
    state.setProperty(MutatorState::kClearEnabled, computeExportEnabled(historyStore_), nullptr);
}

void PatchMutatorEngine::forceExitCompare()
{
    auto& state = apvts_.state;
    if (! readBoolProperty(state, MutatorState::kCompareActive, false))
        return;

    state.setProperty(MutatorState::kCompareActive, false, nullptr);
    compareSavedMutateRootIndex_ = -1;
    compareSavedRetryIndex_ = MutationHistoryStore::kRootOnly;
    clearCompareLockedFooterIfPresent(apvts_);
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

void PatchMutatorEngine::auditionAfterHistoryMutation()
{
    PatchModel buffer;

    if (! historyStore_.isEmpty())
        buffer = resolveAuditionBuffer();
    else if (historyStore_.hasInitialSnapshot())
        buffer = historyStore_.getInitialSnapshot();
    else
        buffer = *patchModel_;

    if (std::memcmp(buffer.data(), patchModel_->data(), PatchModel::kBufferSize) != 0)
        pushResultToEditorAndSynth(buffer);
}

void PatchMutatorEngine::applySelectionFromApvts()
{
    const auto& state = apvts_.state;
    if (! state.hasProperty(MutatorState::kSelectedMutateRootIndex))
        return;

    const int mutateRootIndex = static_cast<int>(state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
    const int retryIndex = static_cast<int>(state.getProperty(MutatorState::kSelectedRetryIndex,
                                                               MutationHistoryStore::kRootOnly));

    if (mutateRootIndex < 0 || ! historyStore_.hasRoot(mutateRootIndex))
    {
        selectedRootIndex_ = -1;
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
        return;
    }

    selectedRootIndex_ = mutateRootIndex;

    if (retryIndex == MutationHistoryStore::kRootOnly)
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    else if (historyStore_.hasRetry(mutateRootIndex, retryIndex))
        selectedRetryIndex_ = retryIndex;
    else
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
}

void PatchMutatorEngine::setAuditionSelection(int rootIndex, int retryIndex)
{
    selectedRootIndex_ = rootIndex;
    selectedRetryIndex_ = retryIndex;
}

int PatchMutatorEngine::rootCount() const noexcept
{
    return historyStore_.rootCount();
}

std::optional<MutationEntry> PatchMutatorEngine::getEntry(int rootIndex, int retryIndex) const
{
    return historyStore_.getEntry(rootIndex, retryIndex);
}

MutationRecipe PatchMutatorEngine::buildRecipeFromApvts() const
{
    const auto& state = apvts_.state;

    MutationRecipe recipe;
    recipe.amountPercent = readPatchMutatorPercent(apvts_, PatchMutator::kAmount, 50);
    recipe.randomPercent = readPatchMutatorPercent(apvts_, PatchMutator::kRandom, 25);
    recipe.enableDco1 = readBoolProperty(state, PatchMutator::kEnableDco1, false);
    recipe.enableDco2 = readBoolProperty(state, PatchMutator::kEnableDco2, false);
    recipe.enableVcfVca = readBoolProperty(state, PatchMutator::kEnableVcfVca, false);
    recipe.enableFmTrack = readBoolProperty(state, PatchMutator::kEnableFmTrack, false);
    recipe.enableRampPortamento = readBoolProperty(state, PatchMutator::kEnableRampPortamento, false);
    recipe.enableEnvelope1 = readBoolProperty(state, PatchMutator::kEnableEnvelope1, false);
    recipe.enableEnvelope2 = readBoolProperty(state, PatchMutator::kEnableEnvelope2, false);
    recipe.enableEnvelope3 = readBoolProperty(state, PatchMutator::kEnableEnvelope3, false);
    recipe.enableLfo1 = readBoolProperty(state, PatchMutator::kEnableLfo1, false);
    recipe.enableLfo2 = readBoolProperty(state, PatchMutator::kEnableLfo2, false);
    recipe.enableMatrixMod = readBoolProperty(state, PatchMutator::kEnableMatrixMod, false);
    return recipe;
}

PatchModel PatchMutatorEngine::resolveAuditionBuffer()
{
    applySelectionFromApvts();

    if (historyStore_.isEmpty())
        return *patchModel_;

    int rootIndex = selectedRootIndex_;
    int retryIndex = selectedRetryIndex_;

    if (rootIndex < 0)
    {
        const auto sortedRoots = historyStore_.getSortedRootIndices();
        if (sortedRoots.isEmpty())
            return *patchModel_;

        rootIndex = sortedRoots.getLast();
        retryIndex = MutationHistoryStore::kRootOnly;
    }

    if (const auto entry = historyStore_.getEntry(rootIndex, retryIndex))
    {
        PatchModel model;
        model.loadFrom(entry->result.data());
        return model;
    }

    if (const auto rootEntry = historyStore_.getEntry(rootIndex, MutationHistoryStore::kRootOnly))
    {
        PatchModel model;
        model.loadFrom(rootEntry->result.data());
        return model;
    }

    return *patchModel_;
}

std::optional<int> PatchMutatorEngine::resolveSelectedRootIndex() const
{
    if (historyStore_.isEmpty())
        return std::nullopt;

    if (selectedRootIndex_ >= 0)
        return selectedRootIndex_;

    const auto sortedRoots = historyStore_.getSortedRootIndices();
    if (sortedRoots.isEmpty())
        return std::nullopt;

    return sortedRoots.getLast();
}

std::optional<MutationEntry> PatchMutatorEngine::resolveSelectedEntryForRetry(int rootIndex) const
{
    if (selectedRetryIndex_ == MutationHistoryStore::kRootOnly)
        return historyStore_.getEntry(rootIndex, MutationHistoryStore::kRootOnly);

    if (const auto retryEntry = historyStore_.getEntry(rootIndex, selectedRetryIndex_))
        return retryEntry;

    return historyStore_.getEntry(rootIndex, MutationHistoryStore::kRootOnly);
}

void PatchMutatorEngine::pushResultToEditorAndSynth(const PatchModel& mutatedModel)
{
    std::memcpy(patchModel_->data(), mutatedModel.data(), PatchModel::kBufferSize);
    pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);

    if (midiManager_ != nullptr && getCurrentPatchNumber_)
    {
        const int patchNumber = getCurrentPatchNumber_();
        midiManager_->sendPatch(static_cast<juce::uint8>(juce::jlimit(0, 255, patchNumber)),
                               patchModel_->data());
    }
}

bool PatchMutatorEngine::readBoolProperty(const juce::ValueTree& state,
                                          const juce::Identifier& propertyId,
                                          bool defaultValue)
{
    if (! state.hasProperty(propertyId))
        return defaultValue;

    return static_cast<bool>(state.getProperty(propertyId, defaultValue));
}

} // namespace Core
