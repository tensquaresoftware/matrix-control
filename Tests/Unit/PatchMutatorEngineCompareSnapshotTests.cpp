#include "PatchMutatorEngineTestSupport.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineCompareSnapshotTests : public juce::UnitTest
{
public:
    PatchMutatorEngineCompareSnapshotTests() : juce::UnitTest("PatchMutatorEngineCompareSnapshot") {}

    void runTest() override
    {
        mutate_firstRoot_capturesInitialSnapshot();
        mutate_secondRoot_doesNotOverwriteInitialSnapshot();
        compare_emptyHistory_blocked();
        compare_noInitialSnapshot_blocked();
        compare_enter_auditionsInitialSnapshot();
        compare_enter_setsCompareActive();
        compare_exit_clearsLockedFooterExactMatch();
        compare_exit_leavesUnrelatedFooter();
        compare_exit_restoresSelection();
        compare_exit_auditionsRestoredEntry();
        compare_auditionBlockedWhileActive();
        compare_enter_selectsInitialInHistory();
        compare_exit_clearsInitialSelection();
        initialSelected_auditionsOriginSnapshot();
        initialSelected_blocksRetryAndDelete();
        mutate_fromInitial_keepsSnapshotAndSelectsNewEntry();
        clear_hidesInitialSelection();
    }

private:
    void mutate_firstRoot_capturesInitialSnapshot()
    {
        beginTest("mutate_firstRoot_capturesInitialSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto preMutateBuffer = makeDistinctBuffer(801);
        std::memcpy(harness.model.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasInitialSnapshot());

        const auto snapshot = harness.store().getInitialSnapshot();
        expect(std::memcmp(snapshot.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize) == 0);

        const auto entry = harness.engine.getEntry(0);
        expect(entry.has_value());
        expect(std::memcmp(entry->result.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize) != 0);
    }

    void mutate_secondRoot_doesNotOverwriteInitialSnapshot()
    {
        beginTest("mutate_secondRoot_doesNotOverwriteInitialSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto preFirstMutate = makeDistinctBuffer(811);
        std::memcpy(harness.model.data(), preFirstMutate.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.mutate().success);
        const auto firstSnapshot = harness.store().getInitialSnapshot();

        expect(harness.engine.mutate().success);
        const auto secondSnapshot = harness.store().getInitialSnapshot();
        expect(std::memcmp(firstSnapshot.data(), secondSnapshot.data(), Core::PatchModel::kBufferSize) == 0);
        expect(std::memcmp(secondSnapshot.data(), preFirstMutate.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void compare_emptyHistory_blocked()
    {
        beginTest("compare_emptyHistory_blocked");

        EngineHarness harness;

        const auto result = harness.engine.toggleCompare();
        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("Patch Mutator: Mutation history is empty."));
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void compare_noInitialSnapshot_blocked()
    {
        beginTest("compare_noInitialSnapshot_blocked");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(815);
        auto m00Parent = makeDistinctBuffer(816);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);

        const auto result = harness.engine.toggleCompare();
        expect(! result.success);
        expectEquals(result.footerMessage,
                     juce::String("Patch Mutator: No initial patch snapshot available for compare."));
        expectEquals(result.footerSeverity, juce::String("warning"));
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void compare_enter_auditionsInitialSnapshot()
    {
        beginTest("compare_enter_auditionsInitialSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto preMutateBuffer = makeDistinctBuffer(821);
        std::memcpy(harness.model.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.mutate().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);

        const auto result = harness.engine.toggleCompare();
        expect(result.success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), preMutateBuffer.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void compare_enter_setsCompareActive()
    {
        beginTest("compare_enter_setsCompareActive");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        expect(harness.engine.mutate().success);

        const auto result = harness.engine.toggleCompare();
        expect(result.success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(result.footerMessage,
                     juce::String(PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages::kCompareLockedFooter));
        expectEquals(result.footerSeverity, juce::String("info"));
    }

    void compare_exit_clearsLockedFooterExactMatch()
    {
        beginTest("compare_exit_clearsLockedFooterExactMatch");

        namespace MutatorMessages = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages;

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.engine.toggleCompare().success);

        harness.proc.apvts.state.setProperty("uiMessageText",
                                             juce::String(MutatorMessages::kCompareLockedFooter),
                                             nullptr);
        harness.proc.apvts.state.setProperty("uiMessageSeverity", "info", nullptr);

        expect(harness.engine.toggleCompare().success);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString().isEmpty());
    }

    void compare_exit_leavesUnrelatedFooter()
    {
        beginTest("compare_exit_leavesUnrelatedFooter");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.engine.toggleCompare().success);

        harness.proc.apvts.state.setProperty("uiMessageText", "Unrelated sticky", nullptr);
        harness.proc.apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);

        expect(harness.engine.toggleCompare().success);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String("Unrelated sticky"));
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String("warning"));
    }

    void compare_exit_restoresSelection()
    {
        beginTest("compare_exit_restoresSelection");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(831);
        auto m00Parent = makeDistinctBuffer(832);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        auto r00 = makeDistinctBuffer(833);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex, 0, nullptr);
        harness.applySelectionFromApvts();

        expect(harness.engine.toggleCompare().success);
        expect(harness.engine.toggleCompare().success);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)), 0);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void compare_exit_auditionsRestoredEntry()
    {
        beginTest("compare_exit_auditionsRestoredEntry");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(841);
        auto m00Parent = makeDistinctBuffer(842);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        auto r00 = makeDistinctBuffer(843);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        std::memcpy(harness.model.data(), r00.data(), Core::PatchModel::kBufferSize);
        const auto liveNameBeforeCompare = harness.model.getName();

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex, 0, nullptr);
        harness.applySelectionFromApvts();

        expect(harness.engine.toggleCompare().success);
        // Entering Compare swaps in the initial snapshot's params but keeps the
        // currently-displayed name (name SSOT is the live model, not the historical entry).
        Core::PatchModel expectedOnEnter;
        expectedOnEnter.loadFrom(m00Parent.data());
        expectedOnEnter.setName(liveNameBeforeCompare);
        expect(std::memcmp(harness.model.data(), expectedOnEnter.data(), Core::PatchModel::kBufferSize) == 0);

        expect(harness.engine.toggleCompare().success);
        expect(std::memcmp(harness.model.data(), r00.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void compare_auditionBlockedWhileActive()
    {
        beginTest("compare_auditionBlockedWhileActive");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(851);
        auto m00Parent = makeDistinctBuffer(852);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        expect(harness.engine.toggleCompare().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);

        harness.engine.auditionSelectedHistoryEntry();
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void compare_enter_selectsInitialInHistory()
    {
        beginTest("compare_enter_selectsInitialInHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));

        expect(harness.engine.toggleCompare().success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));
        expect(static_cast<bool>(
            harness.proc.apvts.state.getProperty(MutatorState::kInitialSnapshotAvailable, false)));
    }

    void compare_exit_clearsInitialSelection()
    {
        beginTest("compare_exit_clearsInitialSelection");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);
        expect(harness.engine.retry().success);

        const int rootBeforeCompare = static_cast<int>(
            harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex));
        const int retryBeforeCompare = static_cast<int>(
            harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex));

        expect(harness.engine.toggleCompare().success);
        expect(harness.engine.toggleCompare().success);

        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)),
                     rootBeforeCompare);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     retryBeforeCompare);
    }

    void initialSelected_auditionsOriginSnapshot()
    {
        beginTest("initialSelected_auditionsOriginSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto origin = makeDistinctBuffer(861);
        std::memcpy(harness.model.data(), origin.data(), Core::PatchModel::kBufferSize);
        expect(harness.engine.mutate().success);
        (void) countPatchSysExMessages(harness.queue);

        harness.proc.apvts.state.setProperty(MutatorState::kInitialSelected, true, nullptr);
        harness.engine.auditionSelectedHistoryEntry();

        expectEquals(countPatchSysExMessages(harness.queue), 1);
        Core::PatchModel expectedLive;
        expectedLive.loadFrom(origin.data());
        expectedLive.setName(harness.model.getName());
        expect(std::memcmp(harness.model.data(), expectedLive.data(), Core::PatchModel::kBufferSize) == 0);

        // Manual INITIAL is not the Compare lock.
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void initialSelected_blocksRetryAndDelete()
    {
        beginTest("initialSelected_blocksRetryAndDelete");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);

        harness.proc.apvts.state.setProperty(MutatorState::kInitialSelected, true, nullptr);
        harness.engine.refreshActionEnabledMirrors(harness.proc.apvts);

        expectActionEnabledMirrors(*this, harness, ActionEnabledExpectations {
            .mutate = true,
            .retry = false,
            .exportEnabled = true,
            .deleteEnabled = false,
            .clear = true });

        expect(! harness.engine.retry().success);
        expect(! harness.engine.deleteSelected().success);
        expect(! harness.engine.toggleCompare().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(harness.engine.rootCount(), 1);
    }

    void mutate_fromInitial_keepsSnapshotAndSelectsNewEntry()
    {
        beginTest("mutate_fromInitial_keepsSnapshotAndSelectsNewEntry");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto origin = makeDistinctBuffer(871);
        std::memcpy(harness.model.data(), origin.data(), Core::PatchModel::kBufferSize);
        expect(harness.engine.mutate().success);
        const auto snapshotBefore = harness.store().getInitialSnapshot();

        harness.proc.apvts.state.setProperty(MutatorState::kInitialSelected, true, nullptr);

        expect(harness.engine.mutate().success);
        const auto snapshotAfter = harness.store().getInitialSnapshot();
        expect(std::memcmp(snapshotBefore.data(), snapshotAfter.data(), Core::PatchModel::kBufferSize) == 0);

        expectEquals(harness.engine.rootCount(), 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 1);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));

        // The new mutation branched from the origin, not from M00.
        const auto entry = harness.engine.getEntry(1);
        expect(entry.has_value());
        expect(std::memcmp(entry->parentSnapshot.data(), snapshotBefore.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void clear_hidesInitialSelection()
    {
        beginTest("clear_hidesInitialSelection");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        expect(harness.engine.mutate().success);

        harness.proc.apvts.state.setProperty(MutatorState::kInitialSelected, true, nullptr);

        expect(harness.engine.clearHistory().success);
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty());
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kInitialSelected, false)));
        // CLEAR keeps the RAM snapshot; only the empty history hides the row.
        expect(harness.store().hasInitialSnapshot());
    }

};

static PatchMutatorEngineCompareSnapshotTests patchMutatorEngineCompareSnapshotTests;
