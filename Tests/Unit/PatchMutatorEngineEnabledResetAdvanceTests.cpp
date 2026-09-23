#include "PatchMutatorEngineTestSupport.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineEnabledResetAdvanceTests : public juce::UnitTest
{
public:
    PatchMutatorEngineEnabledResetAdvanceTests() : juce::UnitTest("PatchMutatorEngineEnabledResetAdvance") {}

    void runTest() override
    {
        enabled_emptyHistory();
        enabled_emptyHistory_noModuleToggle_disabled();
        enabled_afterFirstMutate();
        enabled_afterFirstMutate_clearLastToggle_disablesMutate();
        enabled_rootLimit();
        enabled_retryLimit();
        enabled_afterDeleteLast();
        enabled_afterDefrag();
        reset_afterMutate_clearsHistoryAndSnapshot();
        reset_afterCompareActive();
        reset_preservesRecipe();
        reset_noAuditionSysEx();
        reset_enabledMirrorsEmptyHistory();
    }

private:
    void enabled_emptyHistory()
    {
        beginTest("enabled_emptyHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.refreshActionEnabledMirrors(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = false,
            .exportEnabled = false,
            .deleteEnabled = false,
            .clear = false });
    }

    void enabled_emptyHistory_noModuleToggle_disabled()
    {
        beginTest("enabled_emptyHistory_noModuleToggle_disabled");

        EngineHarness harness;
        harness.setRecipe(100, 100, false);
        harness.engine.refreshActionEnabledMirrors(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = false,
            .retry = false,
            .exportEnabled = false,
            .deleteEnabled = false,
            .clear = false });
    }

    void enabled_afterFirstMutate()
    {
        beginTest("enabled_afterFirstMutate");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true });
    }

    void enabled_afterFirstMutate_clearLastToggle_disablesMutate()
    {
        beginTest("enabled_afterFirstMutate_clearLastToggle_disablesMutate");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true });

        harness.proc.apvts.state.setProperty(PatchMutator::kEnableDco1, false, nullptr);
        harness.engine.refreshActionEnabledMirrors(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = false,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true });
    }

    void enabled_rootLimit()
    {
        beginTest("enabled_rootLimit");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRoots; ++i)
        {
            const auto resultPatch = makeDistinctBuffer(i + 10);
            const auto parentPatch = makeDistinctBuffer(i + 110);
            expect(harness.store().insertRoot(i, resultPatch, parentPatch));
        }

        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true,
            .mutateAllocationBlocked = true,
            .mutateDefragRecovery = true });
    }

    void enabled_retryLimit()
    {
        beginTest("enabled_retryLimit");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(1801);
        auto m00Parent = makeDistinctBuffer(1802);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRetriesPerRoot; ++i)
        {
            auto retryPatch = makeDistinctBuffer(i + 1900);
            Core::MutationNaming::applyPatchName(retryPatch, 0, i);
            expect(harness.store().insertRetry(0, i, retryPatch, m00Parent));
        }

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true,
            .retryAllocationBlocked = true,
            .retryDefragRecovery = true });
    }

    void enabled_afterDeleteLast()
    {
        beginTest("enabled_afterDeleteLast");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.engine.deleteSelected().success);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = false,
            .exportEnabled = false,
            .deleteEnabled = false,
            .clear = false });
    }

    void enabled_afterDefrag()
    {
        beginTest("enabled_afterDefrag");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m99 = makeDistinctBuffer(2001);
        auto m99Parent = makeDistinctBuffer(2002);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true,
            .mutateAllocationBlocked = true,
            .mutateDefragRecovery = true });

        expect(harness.engine.defragHistory().success);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true });
    }

    void reset_afterMutate_clearsHistoryAndSnapshot()
    {
        beginTest("reset_afterMutate_clearsHistoryAndSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.store().hasInitialSnapshot());
        expectEquals(harness.engine.rootCount(), 1);

        expect(harness.engine.resetSessionForPatchLoad().success);
        expectEquals(harness.store().rootCount(), 0);
        expect(! harness.store().hasInitialSnapshot());
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryRetryList).toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryRetryListsByRoot).toString().isEmpty());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), -1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void reset_afterCompareActive()
    {
        beginTest("reset_afterCompareActive");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.engine.toggleCompare().success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));

        expect(harness.engine.resetSessionForPatchLoad().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(harness.store().rootCount(), 0);
    }

    void reset_preservesRecipe()
    {
        beginTest("reset_preservesRecipe");

        EngineHarness harness;
        harness.setRecipe(75, 50, true);
        harness.proc.apvts.state.setProperty(PatchMutator::kEnableDco2, true, nullptr);
        harness.proc.apvts.state.setProperty(PatchMutator::kEnableVcfVca, true, nullptr);
        expect(harness.engine.mutate().success);

        const auto amountBefore = static_cast<int>(harness.proc.apvts.state.getProperty(PatchMutator::kAmount, 0));
        const auto randomBefore = static_cast<int>(harness.proc.apvts.state.getProperty(PatchMutator::kRandom, 0));
        const auto dco1Before = static_cast<bool>(harness.proc.apvts.state.getProperty(PatchMutator::kEnableDco1, false));
        const auto dco2Before = static_cast<bool>(harness.proc.apvts.state.getProperty(PatchMutator::kEnableDco2, false));
        const auto vcfBefore = static_cast<bool>(harness.proc.apvts.state.getProperty(PatchMutator::kEnableVcfVca, false));

        expect(harness.engine.resetSessionForPatchLoad().success);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(PatchMutator::kAmount, 0)), amountBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(PatchMutator::kRandom, 0)), randomBefore);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(PatchMutator::kEnableDco1, false)) == dco1Before);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(PatchMutator::kEnableDco2, false)) == dco2Before);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(PatchMutator::kEnableVcfVca, false)) == vcfBefore);
    }

    void reset_noAuditionSysEx()
    {
        beginTest("reset_noAuditionSysEx");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        (void) countPatchSysExMessages(harness.queue);

        expect(harness.engine.resetSessionForPatchLoad().success);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void reset_enabledMirrorsEmptyHistory()
    {
        beginTest("reset_enabledMirrorsEmptyHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.engine.resetSessionForPatchLoad().success);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = false,
            .exportEnabled = false,
            .deleteEnabled = false,
            .clear = false });
    }

};

static PatchMutatorEngineEnabledResetAdvanceTests patchMutatorEngineEnabledResetAdvanceTests;
