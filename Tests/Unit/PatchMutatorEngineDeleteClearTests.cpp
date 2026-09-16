#include "PatchMutatorEngineTestSupport.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineDeleteClearTests : public juce::UnitTest
{
public:
    PatchMutatorEngineDeleteClearTests() : juce::UnitTest("PatchMutatorEngineDeleteClear") {}

    void runTest() override
    {
        delete_emptyHistory_blocked();
        delete_noSelection_blocked();
        delete_retry_removesSingleEntry();
        delete_root_cascadesRetries();
        delete_retry_selectionMovesToPrevious();
        delete_retry_firstRetry_fallsBackToRootOnly();
        delete_root_selectionMovesToPreviousRoot();
        delete_lastEntry_emptyHistory_auditionsInitialSnapshot();
        delete_lastEntry_disablesCompare();
        delete_whileCompareActive_noStaleRestore();
        delete_syncsApvtsAfterSuccess();
        clear_purgesHistory_emptySentinel();
        clear_emptyHistory_blocked();
        clear_disablesCompare();
        clear_auditionsInitialSnapshot();
        clear_keepsInitialSnapshot();
    }

private:
    void delete_emptyHistory_blocked()
    {
        beginTest("delete_emptyHistory_blocked");

        EngineHarness harness;

        const auto result = harness.engine.deleteSelected();
        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Mutation history is empty."));
        expectEquals(result.footerSeverity, juce::String("warning"));
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void delete_noSelection_blocked()
    {
        beginTest("delete_noSelection_blocked");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(901);
        auto m00Parent = makeDistinctBuffer(902);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, -1, nullptr);

        const auto result = harness.engine.deleteSelected();
        expect(! result.success);
        expectEquals(result.footerMessage,
                     juce::String("PATCH MUTATOR: No valid mutation history entry selected."));
        expectEquals(result.footerSeverity, juce::String("warning"));
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void delete_retry_removesSingleEntry()
    {
        beginTest("delete_retry_removesSingleEntry");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(911);
        auto m00Parent = makeDistinctBuffer(912);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(913);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        auto r02 = makeDistinctBuffer(914);
        Core::MutationNaming::applyPatchName(r02, 0, 2);
        expect(harness.store().insertRetry(0, 2, r02, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex, 2, nullptr);

        const auto result = harness.engine.deleteSelected();
        expect(result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Deleted M00-R02."));
        expectEquals(result.footerSeverity, juce::String("info"));
        expect(! harness.store().hasRetry(0, 2));
        expect(harness.store().hasRetry(0, 0));
        expect(! harness.store().hasRetry(0, 1));
    }

    void delete_root_cascadesRetries()
    {
        beginTest("delete_root_cascadesRetries");

        EngineHarness harness;

        auto m05 = makeDistinctBuffer(921);
        auto m05Parent = makeDistinctBuffer(922);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        auto r00 = makeDistinctBuffer(923);
        Core::MutationNaming::applyPatchName(r00, 5, 0);
        expect(harness.store().insertRetry(5, 0, r00, m05Parent));

        auto r01 = makeDistinctBuffer(924);
        Core::MutationNaming::applyPatchName(r01, 5, 1);
        expect(harness.store().insertRetry(5, 1, r01, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        const auto result = harness.engine.deleteSelected();
        expect(result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Deleted M05 and all retries."));
        expectEquals(result.footerSeverity, juce::String("info"));
        expect(! harness.store().hasRoot(5));
        expectEquals(harness.store().retryCount(5), 0);
    }

    void delete_retry_selectionMovesToPrevious()
    {
        beginTest("delete_retry_selectionMovesToPrevious");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(931);
        auto m00Parent = makeDistinctBuffer(932);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(933);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        auto r02 = makeDistinctBuffer(934);
        Core::MutationNaming::applyPatchName(r02, 0, 2);
        expect(harness.store().insertRetry(0, 2, r02, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex, 2, nullptr);

        std::memcpy(harness.model.data(), r02.data(), Core::PatchModel::kBufferSize);
        const auto liveNameBeforeDelete = harness.model.getName();

        expect(harness.engine.deleteSelected().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        // Re-audition after delete stamps the name that was live before the delete.
        Core::PatchModel expectedAfterDelete;
        expectedAfterDelete.loadFrom(r00.data());
        expectedAfterDelete.setName(liveNameBeforeDelete);
        expect(std::memcmp(harness.model.data(), expectedAfterDelete.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void delete_retry_firstRetry_fallsBackToRootOnly()
    {
        beginTest("delete_retry_firstRetry_fallsBackToRootOnly");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(941);
        auto m00Parent = makeDistinctBuffer(942);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(943);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex, 0, nullptr);

        expect(harness.engine.deleteSelected().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void delete_root_selectionMovesToPreviousRoot()
    {
        beginTest("delete_root_selectionMovesToPreviousRoot");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(951);
        auto m00Parent = makeDistinctBuffer(952);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(953);
        auto m05Parent = makeDistinctBuffer(954);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        expect(harness.engine.deleteSelected().success);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
    }

    void delete_lastEntry_emptyHistory_auditionsInitialSnapshot()
    {
        beginTest("delete_lastEntry_emptyHistory_auditionsInitialSnapshot");

        EngineHarness harness;

        const auto snapshot = makeDistinctBuffer(961);
        harness.store().setInitialSnapshot(snapshot);

        auto m00 = makeDistinctBuffer(962);
        auto m00Parent = makeDistinctBuffer(963);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        std::memcpy(harness.model.data(), m00.data(), Core::PatchModel::kBufferSize);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);

        expect(harness.engine.deleteSelected().success);
        expect(harness.store().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty());
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), snapshot.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void delete_lastEntry_disablesCompare()
    {
        beginTest("delete_lastEntry_disablesCompare");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(971);
        auto m00Parent = makeDistinctBuffer(972);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        expect(harness.engine.deleteSelected().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void delete_whileCompareActive_noStaleRestore()
    {
        beginTest("delete_whileCompareActive_noStaleRestore");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(981);
        auto m00Parent = makeDistinctBuffer(982);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(983);
        auto m05Parent = makeDistinctBuffer(984);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));
        harness.store().setInitialSnapshot(m00Parent);

        auto r00 = makeDistinctBuffer(985);
        Core::MutationNaming::applyPatchName(r00, 5, 0);
        expect(harness.store().insertRetry(5, 0, r00, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.applySelectionFromApvts();

        expect(harness.engine.toggleCompare().success);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));

        expect(harness.engine.deleteSelected().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)),
                     Core::MutationHistoryStore::kRootOnly);
    }

    void delete_syncsApvtsAfterSuccess()
    {
        beginTest("delete_syncsApvtsAfterSuccess");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(991);
        auto m00Parent = makeDistinctBuffer(992);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(993);
        auto m05Parent = makeDistinctBuffer(994);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 5, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kSelectedRetryIndex,
                                             Core::MutationHistoryStore::kRootOnly,
                                             nullptr);
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        const auto listBefore = harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString();
        expect(listBefore.contains("M00"));
        expect(listBefore.contains("M05"));

        expect(harness.engine.deleteSelected().success);

        const auto listAfter = harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString();
        expect(listAfter.contains("M00"));
        expect(! listAfter.contains("M05"));

        const auto retryLabelListAfter = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRetryList).toString();
        expectEquals(retryLabelListAfter, MutatorDisplayNames::kHistoryRootSentinel);
    }

    void clear_purgesHistory_emptySentinel()
    {
        beginTest("clear_purgesHistory_emptySentinel");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1001);
        auto m00Parent = makeDistinctBuffer(1002);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto m05 = makeDistinctBuffer(1003);
        auto m05Parent = makeDistinctBuffer(1004);
        Core::MutationNaming::applyPatchName(m05, 5);
        expect(harness.store().insertRoot(5, m05, m05Parent));

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);

        const auto result = harness.engine.clearHistory();
        expect(result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Mutation history flushed."));
        expectEquals(result.footerSeverity, juce::String("info"));
        expectEquals(harness.store().rootCount(), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedMutateRootIndex)), -1);
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty(MutatorState::kHistoryRetryList).toString().isEmpty());
    }

    void clear_emptyHistory_blocked()
    {
        beginTest("clear_emptyHistory_blocked");

        EngineHarness harness;

        const auto result = harness.engine.clearHistory();
        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Mutation history is empty."));
        expectEquals(result.footerSeverity, juce::String("warning"));
    }

    void clear_disablesCompare()
    {
        beginTest("clear_disablesCompare");

        EngineHarness harness;

        auto m00 = makeDistinctBuffer(1011);
        auto m00Parent = makeDistinctBuffer(1012);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.store().setInitialSnapshot(m00Parent);

        harness.proc.apvts.state.setProperty(MutatorState::kSelectedMutateRootIndex, 0, nullptr);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        expect(harness.engine.clearHistory().success);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(MutatorState::kCompareActive, false)));
    }

    void clear_auditionsInitialSnapshot()
    {
        beginTest("clear_auditionsInitialSnapshot");

        EngineHarness harness;

        const auto snapshot = makeDistinctBuffer(1021);
        harness.store().setInitialSnapshot(snapshot);

        auto m00 = makeDistinctBuffer(1022);
        auto m00Parent = makeDistinctBuffer(1023);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        std::memcpy(harness.model.data(), m00.data(), Core::PatchModel::kBufferSize);

        expect(harness.engine.clearHistory().success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
        expect(std::memcmp(harness.model.data(), snapshot.data(), Core::PatchModel::kBufferSize) == 0);
    }

    void clear_keepsInitialSnapshot()
    {
        beginTest("clear_keepsInitialSnapshot");

        EngineHarness harness;

        const auto snapshot = makeDistinctBuffer(1031);
        harness.store().setInitialSnapshot(snapshot);

        auto m00 = makeDistinctBuffer(1032);
        auto m00Parent = makeDistinctBuffer(1033);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        expect(harness.engine.clearHistory().success);
        expect(harness.store().hasInitialSnapshot());
    }

};

static PatchMutatorEngineDeleteClearTests patchMutatorEngineDeleteClearTests;
