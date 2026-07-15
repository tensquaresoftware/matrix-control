#pragma once

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"

namespace Core
{

    /** Parallel History N2 presentation rows from engine pipe tokens ("—|R00|…"). */
    struct HistorySubmenuDisplay
    {
        juce::StringArray labels;
        juce::Array<int> retryIndices; // parallel to labels; [0] is kRootOnly when non-empty
    };

    struct MutationNaming
    {
        static juce::String formatRootLabel(int rootIndex);
        static juce::String formatRetryLabel(int retryIndex);
        static juce::String formatPatchName(int rootIndex,
                                            int retryIndex = MutationHistoryStore::kRootOnly);
        static juce::String formatExportStem(int rootIndex,
                                             int retryIndex = MutationHistoryStore::kRootOnly);

        /** Presentation labels + retry indices for History N2.
            Returns empty when there are no real retries (empty or sentinel-only).
            Otherwise: Mxx (@ kRootOnly), then Mxx-Ryy for each valid retry token. */
        static HistorySubmenuDisplay buildHistorySubmenuDisplay(
            int rootIndex,
            const juce::StringArray& engineRetryLabels);

        /** Labels only — same contract as buildHistorySubmenuDisplay().labels. */
        static juce::StringArray buildHistorySubmenuDisplayLabels(
            int rootIndex,
            const juce::StringArray& engineRetryLabels);

        static bool applyPatchName(PatchModel& model,
                                   int rootIndex,
                                   int retryIndex = MutationHistoryStore::kRootOnly);

    private:
        static bool isValidRootIndex(int rootIndex) noexcept;
        static bool isValidRetryIndex(int retryIndex) noexcept;
        static juce::String formatTwoDigit(int index);
        static int parseEngineRetryToken(const juce::String& token);
    };

} // namespace Core
