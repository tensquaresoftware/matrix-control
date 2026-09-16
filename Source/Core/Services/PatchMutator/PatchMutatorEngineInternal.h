#pragma once

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

// Free helpers shared by PatchMutatorEngine.cpp and its companion .cpp files.
namespace PatchMutatorEngineInternal
{
    namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
    namespace CompareMessages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

    // Shipped defaults: a recognizable drift, with the patch's tuning left alone.
    constexpr int kDefaultModeIndex = static_cast<int>(Core::MutationMode::kDrift);
    constexpr int kDefaultPitchIndex = static_cast<int>(Core::MutationPitchMode::kPreserve);

    // RETRY diversity heuristic: a handful of re-rolls, and "different enough" means a few
    // mutable bytes apart from the previous mutation.
    constexpr int kRetryDiversityAttempts = 4;
    constexpr int kRetryDiversityMinChangedBytes = 3;
    constexpr size_t kMutableByteRangeStart = 8;

    // All Patch Mutator sticky footers share this module prefix (badge stays separate).
    constexpr const char* kStickyModulePrefix = "PATCH MUTATOR: ";

    constexpr const char* kNoModuleScopeFooterMessage =
        "PATCH MUTATOR: Enable at least one module to mutate.";
    constexpr const char* kNoMutationChangeFooterMessage =
        "PATCH MUTATOR: No changes. Try a wider MODE or more modules.";
    constexpr const char* kRetryTooSimilarFooterMessage =
        "PATCH MUTATOR: RETRY too similar. Try a wider MODE or more modules.";
    constexpr const char* kHistoryLimitFooterMessage =
        "PATCH MUTATOR: Mutation history is full. Defrag to continue.";
    constexpr const char* kEmptyHistoryFooterMessage = "PATCH MUTATOR: Mutation history is empty.";
    constexpr const char* kNoSelectionFooterMessage =
        "PATCH MUTATOR: No valid mutation history entry selected.";
    constexpr const char* kNoInitialSnapshotFooterMessage =
        "PATCH MUTATOR: No initial patch snapshot available for compare.";
    constexpr const char* kFlushSuccessFooterMessage = "PATCH MUTATOR: Mutation history flushed.";
    constexpr const char* kDefragCompleteFooterMessage = "PATCH MUTATOR: Mutation history renumbered.";
    constexpr const char* kExportFolderNotWritableFooterMessage =
        "PATCH MUTATOR: Export folder is not writable.";
    constexpr const char* kExportFailedFooterMessage = "PATCH MUTATOR: Mutation export failed.";
    constexpr const char* kFooterSeverityWarning = "warning";
    constexpr const char* kFooterSeverityInfo = "info";

    inline void clearCompareLockedFooterIfPresent(juce::AudioProcessorValueTreeState& apvts)
    {
        if (apvts.state.getProperty("uiMessageText").toString()
            == juce::String(CompareMessages::kCompareLockedFooter))
        {
            apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
            apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
        }
    }

    inline juce::String formatRetryDeleteFooterMessage(const juce::String& patchName)
    {
        return juce::String(kStickyModulePrefix) + "Deleted " + patchName + ".";
    }

    inline juce::String formatRootDeleteCascadeFooterMessage(const juce::String& rootLabel)
    {
        return juce::String(kStickyModulePrefix) + "Deleted " + rootLabel + " and all retries.";
    }

    inline juce::String formatExportCompleteFooterMessage(int filesWritten,
                                                          const juce::File& destinationFolder)
    {
        return juce::String(CompareMessages::kExportCompleteFooterStem)
               + juce::String(filesWritten)
               + " mutation file(s) to " + destinationFolder.getFullPathName() + ".";
    }

    inline Core::MutatorActionResult makeExportWarningResult(const char* message)
    {
        Core::MutatorActionResult result;
        result.footerMessage = message;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    inline Core::MutatorActionResult makeExportHistoryResult(const Core::PatchFileExportResult& exportResult,
                                                            const juce::File& destinationFolder)
    {
        Core::MutatorActionResult result;

        if (! exportResult.success)
        {
            result.footerMessage = exportResult.errorMessage.isNotEmpty()
                ? juce::String(kStickyModulePrefix) + exportResult.errorMessage
                : juce::String(kExportFailedFooterMessage);
            result.footerSeverity = kFooterSeverityWarning;
            return result;
        }

        result.success = true;
        result.footerMessage = formatExportCompleteFooterMessage(exportResult.filesWritten,
                                                                destinationFolder);
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

    inline void setPatchLoadSuppressHooks(Core::ActionExecutionHooks& hooks, bool suppress)
    {
        if (hooks.setSuppressPatchSysEx)
            hooks.setSuppressPatchSysEx(suppress);

        if (hooks.setSuppressMatrixModSysEx)
            hooks.setSuppressMatrixModSysEx(suppress);
    }

    inline void flushDeferredApvtsParameterSync(juce::AudioProcessorValueTreeState& apvts)
    {
        (void) apvts.copyState();
    }

    // Pipe-separated root/retry labels — e.g. "M00|M02|M05", "—|R00|R02".
    inline juce::String joinLabels(const juce::StringArray& labels)
    {
        return labels.joinIntoString("|");
    }

    inline juce::String historyRootSentinelLabel()
    {
        return PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryRootSentinel;
    }

    inline int readMutatorStateIndex(const juce::ValueTree& state,
                                     const juce::Identifier& propertyId,
                                     int defaultValue)
    {
        if (! state.hasProperty(propertyId))
            return defaultValue;

        return static_cast<int>(state.getProperty(propertyId, defaultValue));
    }

    inline int countDifferingMutableBytes(const Core::PatchModel& left, const Core::PatchModel& right)
    {
        int differing = 0;

        for (size_t i = kMutableByteRangeStart; i < Core::PatchModel::kBufferSize; ++i)
        {
            if (left.data()[i] != right.data()[i])
                ++differing;
        }

        return differing;
    }

    inline void pushPatchModelToApvtsWithSuppress(juce::AudioProcessorValueTreeState& apvts,
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

    inline bool computeMutateEnabled(const Core::MutationHistoryStore& store,
                                     const Core::MutationRecipe& recipe)
    {
        return store.peekNextRootIndex().has_value() && recipe.hasAnyModuleEnabled();
    }

    inline bool computeExportEnabled(const Core::MutationHistoryStore& store)
    {
        return ! store.isEmpty();
    }

    inline bool computeDeleteEnabled(const Core::MutationHistoryStore& store, int selectedRoot)
    {
        return ! store.isEmpty() && selectedRoot >= 0;
    }

    inline bool computeRetryEnabled(const Core::MutationHistoryStore& store, int selectedRoot)
    {
        if (store.isEmpty() || selectedRoot < 0)
            return false;

        return store.peekNextRetryIndex(selectedRoot).has_value();
    }
} // namespace PatchMutatorEngineInternal
