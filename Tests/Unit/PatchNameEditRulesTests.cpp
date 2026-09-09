#include <juce_core/juce_core.h>

#include "Core/Services/PatchNameEditRules.h"

class PatchNameEditRulesTests : public juce::UnitTest
{
public:
    PatchNameEditRulesTests() : juce::UnitTest("PatchNameEditRules") {}

    void runTest() override
    {
        resolve_keepsEditedNameWhenNonEmpty();
        resolve_keepsPreviousWhenAllSpaces();
        resolve_keepsPreviousWhenCompletelyEmpty();
        resolve_keepsEditedNameWithInternalSpaces();
        pendingStore_successWhenNonEmpty();
        pendingStore_unsuccessfulWhenEmptyOrSpaces();
        nameRequiredEndState_successInvokesRename();
        nameRequiredEndState_emptyAbortsWithoutRename();
        pendingStore_completeOnlyWhenPendingAndSuccess();
        pendingStore_suppressesRenameAuditionWhenPending();
    }

private:
    void resolve_keepsEditedNameWhenNonEmpty()
    {
        beginTest("resolve_keepsEditedNameWhenNonEmpty");

        const auto result = Core::PatchNameEditRules::resolveCommittedPatchName("COLDPAD ", "WARMPAD ");
        expectEquals(result, juce::String("COLDPAD "));
    }

    void resolve_keepsPreviousWhenAllSpaces()
    {
        beginTest("resolve_keepsPreviousWhenAllSpaces");

        const auto result = Core::PatchNameEditRules::resolveCommittedPatchName("        ", "WARMPAD ");
        expectEquals(result, juce::String("WARMPAD "));
    }

    void resolve_keepsPreviousWhenCompletelyEmpty()
    {
        beginTest("resolve_keepsPreviousWhenCompletelyEmpty");

        const auto result = Core::PatchNameEditRules::resolveCommittedPatchName("", "WARMPAD ");
        expectEquals(result, juce::String("WARMPAD "));
    }

    void resolve_keepsEditedNameWithInternalSpaces()
    {
        beginTest("resolve_keepsEditedNameWithInternalSpaces");

        const auto result = Core::PatchNameEditRules::resolveCommittedPatchName("BS 1    ", "WARMPAD ");
        expectEquals(result, juce::String("BS 1    "));
    }

    void pendingStore_successWhenNonEmpty()
    {
        beginTest("pendingStore_successWhenNonEmpty");

        expect(Core::PatchNameEditRules::isSuccessfulPendingStoreNameCommit("COLDPAD "));
        expect(Core::PatchNameEditRules::isSuccessfulPendingStoreNameCommit("BS 1    "));
    }

    void pendingStore_unsuccessfulWhenEmptyOrSpaces()
    {
        beginTest("pendingStore_unsuccessfulWhenEmptyOrSpaces");

        expect(! Core::PatchNameEditRules::isSuccessfulPendingStoreNameCommit(""));
        expect(! Core::PatchNameEditRules::isSuccessfulPendingStoreNameCommit("        "));
    }

    void nameRequiredEndState_successInvokesRename()
    {
        beginTest("nameRequiredEndState_successInvokesRename");

        const auto endState = Core::PatchNameEditRules::resolveNameRequiredCommitEndState(
            "COLDPAD ",
            "WARMPAD ");

        expect(endState.success);
        expect(endState.shouldInvokeRenameCommit);
        expectEquals(endState.resolvedName, juce::String("COLDPAD "));
    }

    void nameRequiredEndState_emptyAbortsWithoutRename()
    {
        beginTest("nameRequiredEndState_emptyAbortsWithoutRename");

        const auto emptyEnd = Core::PatchNameEditRules::resolveNameRequiredCommitEndState(
            "",
            "WARMPAD ");
        expect(! emptyEnd.success);
        expect(! emptyEnd.shouldInvokeRenameCommit);
        expectEquals(emptyEnd.resolvedName, juce::String("WARMPAD "));

        const auto spacesEnd = Core::PatchNameEditRules::resolveNameRequiredCommitEndState(
            "        ",
            "WARMPAD ");
        expect(! spacesEnd.success);
        expect(! spacesEnd.shouldInvokeRenameCommit);
        expectEquals(spacesEnd.resolvedName, juce::String("WARMPAD "));
    }

    void pendingStore_completeOnlyWhenPendingAndSuccess()
    {
        beginTest("pendingStore_completeOnlyWhenPendingAndSuccess");

        expect(Core::PatchNameEditRules::shouldCompletePendingStore(true, true));
        expect(! Core::PatchNameEditRules::shouldCompletePendingStore(true, false));
        expect(! Core::PatchNameEditRules::shouldCompletePendingStore(false, true));
        expect(! Core::PatchNameEditRules::shouldCompletePendingStore(false, false));
    }

    void pendingStore_suppressesRenameAuditionWhenPending()
    {
        beginTest("pendingStore_suppressesRenameAuditionWhenPending");

        expect(Core::PatchNameEditRules::shouldSuppressRenameAuditionForPendingStore(true));
        expect(! Core::PatchNameEditRules::shouldSuppressRenameAuditionForPendingStore(false));
    }
};

static PatchNameEditRulesTests patchNameEditRulesTests;
