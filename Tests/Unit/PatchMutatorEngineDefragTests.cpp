#include "PatchMutatorEngineTestSupport.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineDefragTests : public juce::UnitTest
{
public:
    PatchMutatorEngineDefragTests() : juce::UnitTest("PatchMutatorEngineDefrag") {}

    void runTest() override
    {
        defrag_emptyHistory_blocked();
        defrag_gapExhaustion_thenMutateUnblocked();
        defrag_remapsSelectedRoot();
        defrag_remapsSelectedRetry();
        defrag_clampsEmptySelection();
        defrag_syncsApvtsLists();
        defrag_disablesCompare();
        defrag_auditionsRemappedSelection();
        defrag_successFooter();
    }

private:
    void defrag_emptyHistory_blocked()
    {
        beginTest("defrag_emptyHistory_blocked");

        EngineHarness harness;

        const auto result = harness.engine.defragHistory();
        expect(! result.success);
        expectEquals(result.footerSeverity, juce::String("warning"));
        expect(result.footerMessage.isNotEmpty());
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void defrag_gapExhaustion_thenMutateUnblocked()
    {
        beginTest("defrag_gapExhaustion_thenMutateUnblocked");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m99 = makeDistinctBuffer(1101);
        auto m99Parent = makeDistinctBuffer(1102);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        const auto blocked = harness.engine.mutate();
        expect(! blocked.success);
        expect(blocked.defragModalRequested);

        const auto defrag = harness.engine.defragHistory();
        expect(defrag.success);
        expect(harness.store().hasRoot(0));
        expectEquals(harness.store().peekNextRootIndex().value_or(-1), 1);

        const auto mutate = harness.engine.mutate();
        expect(mutate.success);
        expect(harness.store().hasRoot(1));
    }

    void defrag_remapsSelectedRoot()
    {
        beginTest("defrag_remapsSelectedRoot");

        EngineHarness harness;

        auto m99 = makeDistinctBuffer(1111);
        auto m99Parent = makeDistinctBuffer(1112);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 99, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.applySelectionFromApvts();

        expect(harness.engine.defragHistory().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void defrag_remapsSelectedRetry()
    {
        beginTest("defrag_remapsSelectedRetry");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1121);
        auto m00Parent = makeDistinctBuffer(1122);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r99 = makeDistinctBuffer(1123);
        Core::MutationNaming::applyPatchName(r99, 0, 99);
        expect(harness.store().insertRetry(0, 99, r99, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex, 99, nullptr);
        harness.applySelectionFromApvts();

        expect(harness.engine.defragHistory().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)), 0);
    }

    void defrag_clampsEmptySelection()
    {
        beginTest("defrag_clampsEmptySelection");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1171);
        auto m00Parent = makeDistinctBuffer(1172);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(1173);
        auto m05Parent = makeDistinctBuffer(1174);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        auto m99 = makeDistinctBuffer(1175);
        auto m99Parent = makeDistinctBuffer(1176);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, -1, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.applySelectionFromApvts();

        expect(harness.engine.defragHistory().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void defrag_syncsApvtsLists()
    {
        beginTest("defrag_syncsApvtsLists");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1131);
        auto m00Parent = makeDistinctBuffer(1132);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(1133);
        auto m05Parent = makeDistinctBuffer(1134);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        auto m99 = makeDistinctBuffer(1135);
        auto m99Parent = makeDistinctBuffer(1136);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        expect(harness.engine.defragHistory().success);

        const auto mutateLabelList = harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString();
        expectEquals(mutateLabelList, juce::String("M00|M01|M02"));
    }

    void defrag_disablesCompare()
    {
        beginTest("defrag_disablesCompare");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1141);
        auto m00Parent = makeDistinctBuffer(1142);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        expect(harness.engine.defragHistory().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void defrag_auditionsRemappedSelection()
    {
        beginTest("defrag_auditionsRemappedSelection");

        EngineHarness harness;

        auto m99 = makeDistinctBuffer(1151);
        auto m99Parent = makeDistinctBuffer(1152);
        Core::MutationNaming::applyPatchName(m99, 99);
        expect(harness.store().insertRoot(99, m99, m99Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 99, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.applySelectionFromApvts();

        std::memcpy(harness.model.data(), m99Parent.data(), Core::PatchModel::kBufferSize);
        const auto liveNameBeforeDefrag = harness.model.getName();

        expect(harness.engine.defragHistory().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);

        const auto entry = harness.store().getEntry(0);
        expect(entry.has_value());
        // Re-audition after defrag stamps the name that was live before the defrag.
        Core::PatchModel expectedAfterDefrag;
        expectedAfterDefrag.loadFrom(entry->result.data());
        expectedAfterDefrag.setName(liveNameBeforeDefrag);
        expect(std::memcmp(harness.model.data(), expectedAfterDefrag.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void defrag_successFooter()
    {
        beginTest("defrag_successFooter");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1161);
        auto m00Parent = makeDistinctBuffer(1162);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.defragHistory();
        expect(result.success);
        expectEquals(result.footerSeverity, juce::String("info"));
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Mutation history renumbered."));
    }

};

static PatchMutatorEngineDefragTests patchMutatorEngineDefragTests;
