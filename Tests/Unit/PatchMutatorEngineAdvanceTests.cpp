#include <array>

#include "PatchMutatorEngineTestSupport.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineAdvanceTests : public juce::UnitTest
{
public:
    PatchMutatorEngineAdvanceTests() : juce::UnitTest("PatchMutatorEngineAdvance") {}

    void runTest() override
    {
        advance_emptyHistory_noOp();
        advance_singleEntry_noOp();
        advance_next_flatOrderMultiRootRetry();
        advance_previous_wrapsFromFirstToLast();
        advance_compareActive_noOp();
        advance_unknownSelection_noOp();
        advance_withInitialSnapshot_walksInitialFirst();
        advance_singleMutationWithInitial_togglesInitial();
    }

private:
    void advance_emptyHistory_noOp()
    {
        beginTest("advance_emptyHistory_noOp");

        EngineHarness harness;
        harness.engine.advanceHistorySelection(true);
        harness.engine.advanceHistorySelection(false);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex, -1)),
                     -1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex,
                                                                         Core::MutationHistoryStore::kRootOnly)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void advance_singleEntry_noOp()
    {
        beginTest("advance_singleEntry_noOp");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        const auto m00 = makeDistinctBuffer(100);
        expect(harness.store().insertRoot(0, m00, parent));
        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        harness.engine.advanceHistorySelection(true);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);

        harness.engine.advanceHistorySelection(false);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void advance_next_flatOrderMultiRootRetry()
    {
        beginTest("advance_next_flatOrderMultiRootRetry");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        expect(harness.store().insertRoot(0, makeDistinctBuffer(100), parent));
        expect(harness.store().insertRetry(0, 0, makeDistinctBuffer(110), parent));
        expect(harness.store().insertRetry(0, 1, makeDistinctBuffer(111), parent));
        expect(harness.store().insertRoot(1, makeDistinctBuffer(200), parent));
        expect(harness.store().insertRetry(1, 0, makeDistinctBuffer(210), parent));

        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        // M00 → M00-R00 → M00-R01 → M01 → M01-R00 → wrap M00
        const std::array<std::pair<int, int>, 5> expected {
            std::pair { 0, 0 },
            std::pair { 0, 1 },
            std::pair { 1, Core::MutationHistoryStore::kRootOnly },
            std::pair { 1, 0 },
            std::pair { 0, Core::MutationHistoryStore::kRootOnly },
        };

        for (const auto& [root, retry] : expected)
        {
            harness.engine.advanceHistorySelection(true);
            expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)),
                         root);
            expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                         retry);
        }
    }

    void advance_previous_wrapsFromFirstToLast()
    {
        beginTest("advance_previous_wrapsFromFirstToLast");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        expect(harness.store().insertRoot(0, makeDistinctBuffer(100), parent));
        expect(harness.store().insertRetry(0, 0, makeDistinctBuffer(110), parent));
        expect(harness.store().insertRoot(1, makeDistinctBuffer(200), parent));

        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        harness.engine.advanceHistorySelection(false);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void advance_compareActive_noOp()
    {
        beginTest("advance_compareActive_noOp");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        expect(harness.store().insertRoot(0, makeDistinctBuffer(100), parent));
        expect(harness.store().insertRoot(1, makeDistinctBuffer(200), parent));
        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        harness.engine.advanceHistorySelection(true);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void advance_unknownSelection_noOp()
    {
        beginTest("advance_unknownSelection_noOp");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        expect(harness.store().insertRoot(0, makeDistinctBuffer(100), parent));
        expect(harness.store().insertRoot(1, makeDistinctBuffer(200), parent));
        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        // Stale APVTS root not in the store — applySelection clears engine members; advance must not jump.
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 5, nullptr);

        harness.engine.advanceHistorySelection(true);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 5);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);

        harness.engine.advanceHistorySelection(false);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 5);
    }

    void advance_withInitialSnapshot_walksInitialFirst()
    {
        beginTest("advance_withInitialSnapshot_walksInitialFirst");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        expect(harness.store().insertRoot(0, makeDistinctBuffer(100), parent));
        expect(harness.store().insertRoot(1, makeDistinctBuffer(200), parent));
        harness.store().setInitialSnapshot(parent);

        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        const auto expectInitialSelected = [this, &harness](bool selected)
        {
            expect(static_cast<bool>(
                       harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false))
                   == selected);
        };

        // INITIAL leads the flat walk: M00 -> M01 -> INITIAL -> M00.
        harness.engine.advanceHistorySelection(true);
        expectInitialSelected(false);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 1);

        harness.engine.advanceHistorySelection(true);
        expectInitialSelected(true);
        // M / R stay put so leaving INITIAL lands back on a real entry.
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 1);

        harness.engine.advanceHistorySelection(true);
        expectInitialSelected(false);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);

        // Backwards from the first mutation lands on INITIAL.
        harness.engine.advanceHistorySelection(false);
        expectInitialSelected(true);
    }

    void advance_singleMutationWithInitial_togglesInitial()
    {
        beginTest("advance_singleMutationWithInitial_togglesInitial");

        EngineHarness harness;
        const auto parent = makeDistinctBuffer(1);
        expect(harness.store().insertRoot(0, makeDistinctBuffer(100), parent));
        harness.store().setInitialSnapshot(parent);

        harness.engine.setAuditionSelection(0, Core::MutationHistoryStore::kRootOnly);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        // Two flat slots now (INITIAL + M00) — nav is no longer a no-op.
        harness.engine.advanceHistorySelection(true);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));

        harness.engine.advanceHistorySelection(true);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
    }
};

static PatchMutatorEngineAdvanceTests patchMutatorEngineAdvanceTests;
