#pragma once

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"

namespace Core
{

    struct MutationNaming
    {
        static juce::String formatRootLabel(int rootIndex);
        static juce::String formatRetryLabel(int retryIndex);
        static juce::String formatPatchName(int rootIndex,
                                            int retryIndex = MutationHistoryStore::kRootOnly);
        static juce::String formatExportStem(int rootIndex,
                                             int retryIndex = MutationHistoryStore::kRootOnly);

        static bool applyPatchName(PatchModel& model,
                                   int rootIndex,
                                   int retryIndex = MutationHistoryStore::kRootOnly);

    private:
        static bool isValidRootIndex(int rootIndex) noexcept;
        static bool isValidRetryIndex(int retryIndex) noexcept;
        static juce::String formatTwoDigit(int index);
    };

} // namespace Core
