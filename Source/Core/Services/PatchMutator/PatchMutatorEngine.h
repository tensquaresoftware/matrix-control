#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Services/PatchMutator/MutationAlgorithm.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"

class MidiManager;

class PatchMutatorEngineTests;

namespace Core
{
    class ApvtsPatchMapper;
    class PatchModel;
    class PatchNameSyncer;

    struct MutatorActionResult
    {
        bool success = false;
        juce::String footerMessage;
        juce::String footerSeverity;
        bool defragModalRequested = false;
    };

    class PatchMutatorEngine
    {
    public:
        PatchMutatorEngine(PatchModel* patchModel,
                           ApvtsPatchMapper* apvtsPatchMapper,
                           PatchNameSyncer* patchNameSyncer,
                           MidiManager* midiManager,
                           juce::AudioProcessorValueTreeState& apvts,
                           ActionExecutionHooks hooks,
                           std::function<int()> getCurrentPatchNumber);

        MutatorActionResult mutate();
        MutatorActionResult retry();                       // Story 6.5
        MutatorActionResult toggleCompare();               // Story 6.8
        MutatorActionResult deleteSelected();              // Story 6.9
        MutatorActionResult clearHistory();                // Story 6.9
        MutatorActionResult exportHistory(const juce::File& destinationFolder); // Story 6.11
        MutatorActionResult defragHistory();               // Story 6.10

        void auditionSelectedHistoryEntry();               // Story 6.7
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
        PatchModel resolveAuditionBuffer() const;
        // RETRY-only — resolves Mi from selection with same root fallback as resolveAuditionBuffer.
        std::optional<int> resolveSelectedRootIndex() const;
        // RETRY-only — returns selected entry for parentSnapshot input; differs from audition semantics (D-083).
        std::optional<MutationEntry> resolveSelectedEntryForRetry(int rootIndex) const;
        void pushResultToEditorAndSynth(const PatchModel& mutatedModel);
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

        MutationHistoryStore historyStore_;
        MutationAlgorithm algorithm_;
        juce::Random rng_;
        int selectedRootIndex_ = -1;
        int selectedRetryIndex_ = MutationHistoryStore::kRootOnly;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchMutatorEngine)
    };

} // namespace Core
