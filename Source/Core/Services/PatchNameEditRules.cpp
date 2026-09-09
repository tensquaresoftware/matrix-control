#include "Core/Services/PatchNameEditRules.h"

namespace Core
{

    juce::String PatchNameEditRules::resolveCommittedPatchName(const juce::String& editedName,
                                                                const juce::String& previousName)
    {
        if (editedName.trim().isEmpty())
            return previousName;

        return editedName;
    }

    bool PatchNameEditRules::isSuccessfulPendingStoreNameCommit(const juce::String& editedName)
    {
        return editedName.trim().isNotEmpty();
    }

    PatchNameEditRules::NameRequiredCommitEndState PatchNameEditRules::resolveNameRequiredCommitEndState(
        const juce::String& editedName,
        const juce::String& previousName)
    {
        NameRequiredCommitEndState endState;
        endState.success = isSuccessfulPendingStoreNameCommit(editedName);
        endState.shouldInvokeRenameCommit = endState.success;
        endState.resolvedName = resolveCommittedPatchName(editedName, previousName);
        return endState;
    }

} // namespace Core
