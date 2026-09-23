#include "PatchMutatorEngineTestSupport.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineDefragRecoveryEnabledTests : public juce::UnitTest
{
public:
    PatchMutatorEngineDefragRecoveryEnabledTests()
        : juce::UnitTest("PatchMutatorEngineDefragRecoveryEnabled")
    {
    }

    void runTest() override
    {
        enabled_rootGapExhaustion_recovery();
        enabled_retryGapExhaustion_recovery();
        enabled_rootLimit_noModule_staysDisabled();
        enabled_afterDeleteLeavesRecovery();
        enabled_retryRecovery_togglesWithHistorySelection();
        enabled_retryFull_initialSelected_noRecovery();
        enabled_rootLimit_retryStillNormal();
        enabled_compareToggle_clearsAndRestoresRecovery();
    }

private:
    void enabled_rootGapExhaustion_recovery()
    {
        beginTest("enabled_rootGapExhaustion_recovery");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m99 = makeDistinctBuffer(2101);
        auto m99Parent = makeDistinctBuffer(2102);
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
    }

    void enabled_retryGapExhaustion_recovery()
    {
        beginTest("enabled_retryGapExhaustion_recovery");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(2201);
        auto m00Parent = makeDistinctBuffer(2202);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r99 = makeDistinctBuffer(2203);
        Core::MutationNaming::applyPatchName(r99, 0, 99);
        expect(harness.store().insertRetry(0, 99, r99, m00Parent));

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

    void enabled_rootLimit_noModule_staysDisabled()
    {
        beginTest("enabled_rootLimit_noModule_staysDisabled");

        EngineHarness harness;
        harness.setRecipe(100, 100, false);

        auto m99 = makeDistinctBuffer(2301);
        auto m99Parent = makeDistinctBuffer(2302);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = false,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true,
            .mutateAllocationBlocked = true,
            .mutateDefragRecovery = false });
    }

    void enabled_afterDeleteLeavesRecovery()
    {
        beginTest("enabled_afterDeleteLeavesRecovery");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m98 = makeDistinctBuffer(2401);
        auto m98Parent = makeDistinctBuffer(2402);
        auto m99 = makeDistinctBuffer(2403);
        auto m99Parent = makeDistinctBuffer(2404);
        Core::MutationNaming::applyPatchName(m98, 98);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(98, m98, m98Parent));
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.engine.setAuditionSelection(99, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expect(static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kMutateDefragRecovery, false)));

        expect(harness.engine.deleteSelected().success);
        expect(! static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kMutateDefragRecovery, false)));
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kMutateEnabled)));
    }

    void enabled_retryRecovery_togglesWithHistorySelection()
    {
        beginTest("enabled_retryRecovery_togglesWithHistorySelection");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(2501);
        auto m00Parent = makeDistinctBuffer(2502);
        auto m01 = makeDistinctBuffer(2503);
        auto m01Parent = makeDistinctBuffer(2504);
        Core::MutationNaming::applyPatchName(m00, 0);
        Core::MutationNaming::applyPatchName(m01, 1);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        expect(harness.store().insertRoot(1, m01, m01Parent));

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRetriesPerRoot; ++i)
        {
            auto retryPatch = makeDistinctBuffer(i + 2600);
            Core::MutationNaming::applyPatchName(retryPatch, 0, i);
            expect(harness.store().insertRetry(0, i, retryPatch, m00Parent));
        }

        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true,
            .retryAllocationBlocked = true,
            .retryDefragRecovery = true });

        harness.engine.setAuditionSelection(1, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = true,
            .exportEnabled = true,
            .deleteEnabled = true,
            .clear = true });
    }

    void enabled_retryFull_initialSelected_noRecovery()
    {
        beginTest("enabled_retryFull_initialSelected_noRecovery");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);

        const int root = static_cast<int>(
            harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1));
        expect(root >= 0);

        const auto entry = harness.store().getEntry(root);
        expect(entry.has_value());
        Core::PatchModel parent;
        std::memcpy(parent.data(), entry->parentSnapshot.data(), Core::PatchModel::kBufferSize);

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRetriesPerRoot; ++i)
        {
            auto retryPatch = makeDistinctBuffer(i + 2700);
            Core::MutationNaming::applyPatchName(retryPatch, root, i);
            expect(harness.store().insertRetry(root, i, retryPatch, parent));
        }

        harness.engine.setAuditionSelection(root, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expect(static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kRetryDefragRecovery, false)));

        harness.proc.apvts.state.setProperty(MutatorState::kInitialSelected, true, nullptr);
        harness.engine.refreshActionEnabledMirrors(harness.proc.apvts);
        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = false,
            .exportEnabled = true,
            .deleteEnabled = false,
            .clear = true });
    }

    void enabled_rootLimit_retryStillNormal()
    {
        beginTest("enabled_rootLimit_retryStillNormal");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRoots; ++i)
        {
            const auto resultPatch = makeDistinctBuffer(i + 2800);
            const auto parentPatch = makeDistinctBuffer(i + 2900);
            expect(harness.store().insertRoot(i, resultPatch, parentPatch));
        }

        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
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

    void enabled_compareToggle_clearsAndRestoresRecovery()
    {
        beginTest("enabled_compareToggle_clearsAndRestoresRecovery");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        harness.store().setInitialSnapshot(makeDistinctBuffer(3000));

        auto m99 = makeDistinctBuffer(3001);
        auto m99Parent = makeDistinctBuffer(3002);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.engine.setAuditionSelection(99, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        expect(static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kMutateDefragRecovery, false)));

        expect(harness.engine.toggleCompare().success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expect(! static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kMutateDefragRecovery, false)));

        harness.engine.refreshActionEnabledMirrors(harness.proc.apvts);
        expect(! static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kMutateDefragRecovery, false)));

        expect(harness.engine.toggleCompare().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expect(static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kMutateDefragRecovery, false)));
    }
};

static PatchMutatorEngineDefragRecoveryEnabledTests patchMutatorEngineDefragRecoveryEnabledTests;
