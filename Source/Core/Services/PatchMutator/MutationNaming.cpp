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

    int MutationNaming::parseEngineRetryToken(const juce::String& token)
    {
        if (! token.startsWithChar('R') || token.length() < 3)
            return -1;

        const int retryIndex = token.substring(1, 3).getIntValue();
        if (! isValidRetryIndex(retryIndex))
            return -1;

        // Reject non-canonical forms (e.g. "R0x", "R00extra") that still parse digits.
        if (formatRetryLabel(retryIndex) != token)
            return -1;

        return retryIndex;
    }

    HistorySubmenuDisplay MutationNaming::buildHistorySubmenuDisplay(
        int rootIndex,
        const juce::StringArray& engineRetryLabels)
    {
        HistorySubmenuDisplay result;
        if (! isValidRootIndex(rootIndex) || engineRetryLabels.size() <= 1)
            return result;

        const auto rootOnlyLabel = formatPatchName(rootIndex, MutationHistoryStore::kRootOnly);
        if (rootOnlyLabel.isEmpty())
            return result;

        result.labels.add(rootOnlyLabel);
        result.retryIndices.add(MutationHistoryStore::kRootOnly);

        for (int i = 1; i < engineRetryLabels.size(); ++i)
        {
            const int retryIndex = parseEngineRetryToken(engineRetryLabels[i]);
            if (retryIndex < 0)
                continue;

            const auto label = formatPatchName(rootIndex, retryIndex);
            if (label.isEmpty())
                continue;

            result.labels.add(label);
            result.retryIndices.add(retryIndex);
        }

        if (result.labels.size() <= 1)
            return {};

        return result;
    }

    juce::StringArray MutationNaming::buildHistorySubmenuDisplayLabels(
        int rootIndex,
        const juce::StringArray& engineRetryLabels)
    {
        return buildHistorySubmenuDisplay(rootIndex, engineRetryLabels).labels;
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
