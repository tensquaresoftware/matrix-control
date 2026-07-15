#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Services/PatchMutator/MutationAlgorithm.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"

class MidiManager;
class SysExEncoder;

class PatchMutatorEngineTests;

namespace Core
{
    class ApvtsPatchMapper;
    class PatchFileService;
    class PatchModel;
    class PatchNameSyncer;

    struct MutatorActionResult
    {
        bool success = false;
        juce::String footerMessage;
        juce::String footerSeverity;
        bool defragModalRequested = false;
    };

    class PatchMutatorEnginePort
    {
    public:
        virtual ~PatchMutatorEnginePort() = default;

        virtual MutatorActionResult mutate() = 0;
        virtual MutatorActionResult retry() = 0;
        virtual MutatorActionResult toggleCompare() = 0;
        virtual MutatorActionResult deleteSelected() = 0;
        virtual MutatorActionResult clearHistory() = 0;
        virtual MutatorActionResult exportHistory(const juce::File& destinationFolder) = 0;
        virtual MutatorActionResult defragHistory() = 0;
        virtual void auditionSelectedHistoryEntry() = 0;
        virtual void rebuildHistoryListMirrors() = 0;
    };

    class PatchMutatorEngine final : public PatchMutatorEnginePort
    {
    public:
        PatchMutatorEngine(PatchModel* patchModel,
                           ApvtsPatchMapper* apvtsPatchMapper,
                           PatchNameSyncer* patchNameSyncer,
                           MidiManager* midiManager,
                           juce::AudioProcessorValueTreeState& apvts,
                           ActionExecutionHooks hooks,
                           std::function<int()> getCurrentPatchNumber,
                           PatchFileService* patchFileService = nullptr,
                           SysExEncoder* sysExEncoder = nullptr);

        MutatorActionResult mutate() override;
        MutatorActionResult retry() override;
        MutatorActionResult toggleCompare() override;
        MutatorActionResult deleteSelected() override;
        MutatorActionResult clearHistory() override;
        MutatorActionResult resetSessionForPatchLoad();
        MutatorActionResult exportHistory(const juce::File& destinationFolder) override;
        MutatorActionResult defragHistory() override;

        void auditionSelectedHistoryEntry() override;
        void rebuildHistoryListMirrors() override;
        void syncHistoryUiProperties(juce::AudioProcessorValueTreeState& apvts); // Story 6.6
        void refreshActionEnabledMirrors(juce::AudioProcessorValueTreeState& apvts); // Story 6.12

        void setAuditionSelection(int rootIndex, int retryIndex = MutationHistoryStore::kRootOnly);

        int rootCount() const noexcept;
        std::optional<MutationEntry> getEntry(int rootIndex,
                                              int retryIndex = MutationHistoryStore::kRootOnly) const;

    private:
        friend class ::PatchMutatorEngineTests;

        MutationRecipe buildRecipeFromApvts() const;
        // resolveAuditionBuffer: empty history -> live editor; selectedRootIndex_ < 0 -> highest sorted root
        // (root-only); missing entry at selection -> root-only entry or live editor fallback.
        // MUTATE-only — returns selected entry result, not parentSnapshot (D-083).
        PatchModel resolveAuditionBuffer();
        // RETRY-only — resolves Mi from selection with same root fallback as resolveAuditionBuffer.
        std::optional<int> resolveSelectedRootIndex() const;
        // RETRY-only — returns selected entry for parentSnapshot input; differs from audition semantics (D-083).
        std::optional<MutationEntry> resolveSelectedEntryForRetry(int rootIndex) const;
        void pushResultToEditorAndSynth(const PatchModel& mutatedModel);
        void applySelectionFromApvts();
        void forceExitCompare();
        std::pair<int, int> resolveSelectionAfterDelete(int rootIndex,
                                                        int retryIndex,
                                                        bool isRetryDelete);
        void auditionAfterHistoryMutation();
        static bool readBoolProperty(const juce::ValueTree& state,
                                     const juce::Identifier& propertyId,
                                     bool defaultValue);

        PatchModel* patchModel_;
        ApvtsPatchMapper* apvtsPatchMapper_;
        PatchNameSyncer* patchNameSyncer_;
        MidiManager* midiManager_;
        juce::AudioProcessorValueTreeState& apvts_;
        ActionExecutionHooks hooks_;
        std::function<int()> getCurrentPatchNumber_;
        PatchFileService* patchFileService_ = nullptr;
        SysExEncoder* sysExEncoder_ = nullptr;

        MutationHistoryStore historyStore_;
        MutationAlgorithm algorithm_;
        juce::Random rng_;
        int selectedRootIndex_ = -1;
        int selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
        int compareSavedMutateRootIndex_ = -1;
        int compareSavedRetryIndex_ = MutationHistoryStore::kRootOnly;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchMutatorEngine)
    };

} // namespace Core
