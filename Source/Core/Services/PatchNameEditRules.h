#pragma once

#include <juce_core/juce_core.h>

namespace Core
{

    // Small, pure commit rules extracted from the Patch Name inline editor so they are
    // unit-testable without a Component: an empty or all-spaces edit keeps the previous
    // name instead of committing a blank patch name. Name-required (pending STORE) uses
    // the same empty/trim gate for success vs abort.
    struct PatchNameEditRules
    {
        // End-state for a name-required Return: trim-non-empty is the success gate
        // (charset filtering happens earlier in the editor; this helper does not re-check it).
        struct NameRequiredCommitEndState
        {
            bool success = false;
            bool shouldInvokeRenameCommit = false;
            juce::String resolvedName;
        };

        static juce::String resolveCommittedPatchName(const juce::String& editedName,
                                                       const juce::String& previousName);

        // True when the edit buffer is non-empty after trim — pending STORE may proceed.
        static bool isSuccessfulPendingStoreNameCommit(const juce::String& editedName);

        static NameRequiredCommitEndState resolveNameRequiredCommitEndState(
            const juce::String& editedName,
            const juce::String& previousName);
    };

} // namespace Core
