#include "Core/Services/PatchMutator/MutationNaming.h"

namespace Core
{

    juce::String MutationNaming::formatRootLabel(int rootIndex)
    {
        if (! isValidRootIndex(rootIndex))
            return {};

        return "M" + formatTwoDigit(rootIndex);
    }

    juce::String MutationNaming::formatRetryLabel(int retryIndex)
    {
        if (! isValidRetryIndex(retryIndex))
            return {};

        return "R" + formatTwoDigit(retryIndex);
    }

    juce::String MutationNaming::formatPatchName(int rootIndex, int retryIndex)
    {
        if (! isValidRootIndex(rootIndex))
            return {};

        const auto rootLabel = formatRootLabel(rootIndex);
        if (rootLabel.isEmpty())
            return {};

        if (retryIndex == MutationHistoryStore::kRootOnly)
            return rootLabel;

        if (! isValidRetryIndex(retryIndex))
            return {};

        return rootLabel + "-R" + formatTwoDigit(retryIndex);
    }

    juce::String MutationNaming::formatExportStem(int rootIndex, int retryIndex)
    {
        return formatPatchName(rootIndex, retryIndex);
    }

    bool MutationNaming::applyPatchName(PatchModel& model, int rootIndex, int retryIndex)
    {
        const auto name = formatPatchName(rootIndex, retryIndex);
        if (name.isEmpty())
            return false;

        model.setName(name);
        return true;
    }

    bool MutationNaming::isValidRootIndex(int rootIndex) noexcept
    {
        return rootIndex >= 0 && rootIndex < MutationHistoryStore::kMaxRoots;
    }

    bool MutationNaming::isValidRetryIndex(int retryIndex) noexcept
    {
        return retryIndex >= 0 && retryIndex < MutationHistoryStore::kMaxRetriesPerRoot;
    }

    juce::String MutationNaming::formatTwoDigit(int index)
    {
        return juce::String::formatted("%02d", index);
    }

} // namespace Core
