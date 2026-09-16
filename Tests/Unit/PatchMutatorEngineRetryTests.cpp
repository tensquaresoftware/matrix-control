#include "PatchMutatorEngineTestSupport.h"
#include "MutationCalibrationTestSupport.h"

#include "Core/Services/PatchMutator/PatchMutatorEngineInternal.h"

using namespace PatchMutatorEngineTestSupport;
using namespace MutationCalibrationTestSupport;

class PatchMutatorEngineRetryTests : public juce::UnitTest
{
public:
    PatchMutatorEngineRetryTests() : juce::UnitTest("PatchMutatorEngineRetry") {}

    void runTest() override
    {
        retry_emptyHistory_blocked();
        retry_firstRetry_preservesParentPatchName();
        retry_usesParentSnapshot_notResult();
        retry_fromSelectedRetry_usesThatEntryParentSnapshot();
        retry_gapAllocation();
        retry_limitBlocks();
        retry_legacyZeroAmount_doesNotOverrideMode();
        retry_consecutiveRetries_areNotIdentical();
        retry_tooSimilar_warnsWithoutHistory();
        retry_sendsSysExOnce();
        retry_matrix1000_sendsEditBuffer();
        retry_neverDeletesExistingRetries();
        retry_staysUnderSameRoot();
        retry_success_updatesApvtsHistory();
    }

private:
    void retry_emptyHistory_blocked()
    {
        beginTest("retry_emptyHistory_blocked");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.retry();
        expect(! result.success);
        expectEquals(harness.engine.rootCount(), 0);
        expectEquals(harness.store().retryCount(0), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void retry_firstRetry_preservesParentPatchName()
    {
        beginTest("retry_firstRetry_preservesParentPatchName");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(41);
        auto m00Parent = makeDistinctBuffer(42);
        m00Parent.setName("COLDPAD");
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expect(harness.store().hasRetry(0, 0));

        const auto retryEntry = harness.store().getEntry(0, 0);
        expect(retryEntry.has_value());

        Core::PatchModel retryResultModel;
        retryResultModel.loadFrom(retryEntry->result.data());
        expectEquals(retryResultModel.getName(), juce::String("COLDPAD"));
    }

    void retry_usesParentSnapshot_notResult()
    {
        beginTest("retry_usesParentSnapshot_notResult");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(51);
        auto m00Parent = makeDistinctBuffer(52);
        m00Parent.data()[8] = static_cast<juce::uint8>(0xAA);
        m00.data()[8] = static_cast<juce::uint8>(0xBB);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);

        const auto retryEntry = harness.store().getEntry(0, 0);
        expect(retryEntry.has_value());
        expectEquals(static_cast<int>(retryEntry->parentSnapshot[8]), 0xAA);
    }

    void retry_fromSelectedRetry_usesThatEntryParentSnapshot()
    {
        beginTest("retry_fromSelectedRetry_usesThatEntryParentSnapshot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(61);
        auto rootParent = makeDistinctBuffer(62);
        rootParent.data()[8] = static_cast<juce::uint8>(0x11);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, rootParent));

        auto retryResult = makeDistinctBuffer(63);
        auto retryParent = makeDistinctBuffer(64);
        retryParent.data()[8] = static_cast<juce::uint8>(0x22);
        Core::MutationNaming::applyPatchName(retryResult, 0, 0);
        expect(harness.store().insertRetry(0, 0, retryResult, retryParent));

        harness.engine.setAuditionSelection(0, 0);

        const auto result = harness.engine.retry();
        expect(result.success);

        const auto newRetry = harness.store().getEntry(0, 1);
        expect(newRetry.has_value());
        expectEquals(static_cast<int>(newRetry->parentSnapshot[8]), 0x22);
    }

    void retry_gapAllocation()
    {
        beginTest("retry_gapAllocation");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(71);
        auto m00Parent = makeDistinctBuffer(72);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto retryResult = makeDistinctBuffer(73);
        Core::MutationNaming::applyPatchName(retryResult, 0, 0);
        expect(harness.store().insertRetry(0, 0, retryResult, m00Parent));
        expect(harness.store().deleteRetry(0, 0));
        expect(harness.store().peekNextRetryIndex(0).value_or(-1) == 0);

        const auto result = harness.engine.retry();
        expect(result.success);
        expect(harness.store().hasRetry(0, 0));
    }

    void retry_limitBlocks()
    {
        beginTest("retry_limitBlocks");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(81);
        auto m00Parent = makeDistinctBuffer(82);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRetriesPerRoot; ++i)
        {
            auto resultPatch = makeDistinctBuffer(100 + i);
            const auto parentPatch = makeDistinctBuffer(200 + i);
            Core::MutationNaming::applyPatchName(resultPatch, 0, i);
            expect(harness.store().insertRetry(0, i, resultPatch, parentPatch));
        }

        const auto result = harness.engine.retry();
        expect(! result.success);
        expect(result.defragModalRequested);
    }

    // Legacy Amount / Random may still sit in a restored session; MODE owns the curve inputs.
    void retry_legacyZeroAmount_doesNotOverrideMode()
    {
        beginTest("retry_legacyZeroAmount_doesNotOverrideMode");

        EngineHarness harness;
        harness.setRecipe(0, 0, true);
        harness.setMode(Core::MutationMode::kWild, Core::MutationPitchMode::kFree);

        auto m00 = makeDistinctBuffer(91);
        auto m00Parent = makeDistinctBuffer(92);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expectEquals(harness.store().retryCount(0), 1);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
    }

    // Two RETRY presses on the same parent must not hand back the same patch twice.
    void retry_consecutiveRetries_areNotIdentical()
    {
        beginTest("retry_consecutiveRetries_areNotIdentical");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(131);
        auto m00Parent = makeDistinctBuffer(132);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        expect(harness.engine.retry().success);
        expect(harness.engine.retry().success);

        const auto first = harness.engine.getEntry(0, 0);
        const auto second = harness.engine.getEntry(0, 1);
        expect(first.has_value());
        expect(second.has_value());
        if (! first.has_value() || ! second.has_value())
            return;

        expect(std::memcmp(first->result.data(),
                           second->result.data(),
                           Core::PatchModel::kBufferSize)
               != 0);

        // The diversity re-roll aims for a few mutable bytes of distance, not just any
        // single byte, so a second RETRY reads as a genuinely different patch.
        int changedMutableBytes = 0;
        for (size_t offset = PatchMutatorEngineInternal::kMutableByteRangeStart;
             offset < Core::PatchModel::kBufferSize;
             ++offset)
        {
            if (first->result.data()[offset] != second->result.data()[offset])
                ++changedMutableBytes;
        }

        expectGreaterOrEqual(changedMutableBytes,
                             PatchMutatorEngineInternal::kRetryDiversityMinChangedBytes);
    }

    // Kindred + Matrix Modulation alone often moves only one Amount byte — below the
    // diversity bar — so RETRY must warn instead of filing a near-clone.
    void retry_tooSimilar_warnsWithoutHistory()
    {
        beginTest("retry_tooSimilar_warnsWithoutHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, false);
        harness.setMode(Core::MutationMode::kKindred, Core::MutationPitchMode::kFree);
        harness.proc.apvts.state.setProperty(PatchMutator::kEnableMatrixMod, true, nullptr);

        auto parent = makeInitPatchModel();
        clearAllMatrixModBuses(parent);
        writeMatrixModBus(parent, { 0, SourceNames::kLfo1, DestinationNames::kDco1PulseWidth, 0 });

        auto resultPatch = parent;
        Core::MutationNaming::applyPatchName(resultPatch, 0);
        expect(harness.store().insertRoot(0, resultPatch, parent));

        const auto result = harness.engine.retry();
        expect(! result.success);
        expectEquals(result.footerMessage,
                     juce::String("PATCH MUTATOR: RETRY too similar. Try a wider MODE or more modules."));
        expectEquals(harness.store().retryCount(0), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void retry_sendsSysExOnce()
    {
        beginTest("retry_sendsSysExOnce");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(101);
        auto m00Parent = makeDistinctBuffer(102);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
    }

    void retry_matrix1000_sendsEditBuffer()
    {
        beginTest("retry_matrix1000_ram_sendsEditBuffer");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(111);
        auto m00Parent = makeDistinctBuffer(112);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        const auto opcodes = countFullPatchOpcodes(harness.queue);
        expectEquals(opcodes.slotWrite, 0);
        expectEquals(opcodes.editBuffer, 1);
    }

    void retry_neverDeletesExistingRetries()
    {
        beginTest("retry_neverDeletesExistingRetries");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(111);
        auto m00Parent = makeDistinctBuffer(112);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto r00 = makeDistinctBuffer(113);
        Core::MutationNaming::applyPatchName(r00, 0, 0);
        expect(harness.store().insertRetry(0, 0, r00, m00Parent));

        auto r02 = makeDistinctBuffer(114);
        Core::MutationNaming::applyPatchName(r02, 0, 2);
        expect(harness.store().insertRetry(0, 2, r02, m00Parent));

        const auto result = harness.engine.retry();
        expect(result.success);
        expect(harness.store().hasRetry(0, 0));
        expect(harness.store().hasRetry(0, 2));
    }

    void retry_staysUnderSameRoot()
    {
        beginTest("retry_staysUnderSameRoot");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(121);
        auto m00Parent = makeDistinctBuffer(122);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        const int rootsBefore = harness.engine.rootCount();
        expect(harness.store().hasRoot(0));

        const auto result = harness.engine.retry();
        expect(result.success);
        expectEquals(harness.engine.rootCount(), rootsBefore);
        expect(harness.store().hasRoot(0));
    }

    void retry_success_updatesApvtsHistory()
    {
        beginTest("retry_success_updatesApvtsHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(601);
        auto m00Parent = makeDistinctBuffer(602);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));
        harness.engine.syncHistoryUiProperties(harness.proc.apvts);

        const auto result = harness.engine.retry();
        expect(result.success);

        const auto retryLabelList = harness.proc.apvts.state.getProperty(MutatorState::kHistoryRetryList).toString();
        expect(retryLabelList.contains("R00"));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(MutatorState::kSelectedRetryIndex)), 0);
    }

};

static PatchMutatorEngineRetryTests patchMutatorEngineRetryTests;
