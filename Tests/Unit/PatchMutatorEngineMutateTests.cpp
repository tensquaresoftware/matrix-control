#include "PatchMutatorEngineTestSupport.h"

#include "Core/Services/PatchMutator/PatchMutatorEngineInternal.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineMutateTests : public juce::UnitTest
{
public:
    PatchMutatorEngineMutateTests() : juce::UnitTest("PatchMutatorEngineMutate") {}

    void runTest() override
    {
        mutate_preservesUserPatchName();
        mutate_gapAllocation();
        mutate_limitBlocks();
        mutate_legacyZeroAmount_doesNotOverrideMode();
        mutate_noModuleToggle_blocked();
        mutate_fromAuditionedRetry();
        mutate_sendsSysExOnce();
        mutate_matrix1000_sendsEditBuffer();
        mutate_matrix1000_rom_sendsRemoteEdits();
        mutate_matrix6_sendsPatchSlot();
        mutate_neverDeletesRoots();
        mutate_success_updatesApvtsHistory();
        footerForDiverseMutation_noUsableRoll_usesPrefixedNoChangeSticky();
    }

private:
    void mutate_preservesUserPatchName()
    {
        beginTest("mutate_preservesUserPatchName");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.model.setName("WARMPAD");

        const auto result = harness.engine.mutate();
        expect(result.success);
        expectEquals(harness.engine.rootCount(), 1);

        const auto entry = harness.engine.getEntry(0);
        expect(entry.has_value());

        Core::PatchModel resultModel;
        resultModel.loadFrom(entry->result.data());
        expectEquals(resultModel.getName(), juce::String("WARMPAD"));
    }

    void mutate_gapAllocation()
    {
        beginTest("mutate_gapAllocation");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto m00 = makeDistinctBuffer(1);
        const auto parent = makeDistinctBuffer(2);
        expect(harness.store().insertRoot(0, m00, parent));
        expect(harness.store().deleteRoot(0));
        expect(harness.store().peekNextRootIndex().value_or(-1) == 0);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasRoot(0));
    }

    void mutate_limitBlocks()
    {
        beginTest("mutate_limitBlocks");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        for (int i = 0; i < Core::MutationHistoryStore::kMaxRoots; ++i)
        {
            const auto resultPatch = makeDistinctBuffer(i + 10);
            const auto parentPatch = makeDistinctBuffer(i + 110);
            expect(harness.store().insertRoot(i, resultPatch, parentPatch));
        }

        const auto result = harness.engine.mutate();
        expect(! result.success);
        expect(result.defragModalRequested);
        expectEquals(result.footerMessage,
                     juce::String("Patch Mutator: Mutation history is full. Defrag to continue."));
        expectEquals(result.footerSeverity, juce::String("warning"));
    }

    // Legacy Amount / Random may still sit in a restored session; MODE owns the curve inputs.
    void mutate_legacyZeroAmount_doesNotOverrideMode()
    {
        beginTest("mutate_legacyZeroAmount_doesNotOverrideMode");

        EngineHarness harness;
        harness.setRecipe(0, 0, true);
        harness.setMode(Core::MutationMode::kWild, Core::MutationPitchMode::kFree);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expectEquals(harness.engine.rootCount(), 1);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
    }

    void mutate_noModuleToggle_blocked()
    {
        beginTest("mutate_noModuleToggle_blocked");

        EngineHarness harness;
        harness.setRecipe(100, 100, false);

        const auto result = harness.engine.mutate();
        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("Patch Mutator: Enable at least one module to mutate."));
        expectEquals(harness.engine.rootCount(), 0);
        expectEquals(countPatchSysExMessages(harness.queue), 0);
    }

    void mutate_fromAuditionedRetry()
    {
        beginTest("mutate_fromAuditionedRetry");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        auto m00 = makeDistinctBuffer(11);
        auto m00Parent = makeDistinctBuffer(12);
        Core::MutationNaming::applyPatchName(m00, 0);
        expect(harness.store().insertRoot(0, m00, m00Parent));

        auto retryResult = makeDistinctBuffer(21);
        retryResult.data()[9] = static_cast<juce::uint8>(0xAB);
        Core::MutationNaming::applyPatchName(retryResult, 0, 0);
        expect(harness.store().insertRetry(0, 0, retryResult, m00Parent));

        harness.engine.setAuditionSelection(0, 0);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasRoot(1));

        const auto parentEntry = harness.store().getEntry(1);
        expect(parentEntry.has_value());
        expectEquals(static_cast<int>(parentEntry->parentSnapshot[9]), static_cast<int>(retryResult.data()[9]));
    }

    void mutate_sendsSysExOnce()
    {
        beginTest("mutate_sendsSysExOnce");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expectEquals(countPatchSysExMessages(harness.queue), 1);
    }

    void mutate_matrix1000_sendsEditBuffer()
    {
        beginTest("mutate_matrix1000_ram_sendsEditBuffer");

        EngineHarness harness;
        harness.deviceLimits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        const auto opcodes = countFullPatchOpcodes(harness.queue);
        expectEquals(opcodes.slotWrite, 0);
        expectEquals(opcodes.editBuffer, 1);
        expectEquals(opcodes.remoteParamEdit, 0);
    }

    void mutate_matrix1000_rom_sendsRemoteEdits()
    {
        beginTest("mutate_matrix1000_rom_sendsEditBuffer");

        EngineHarness harness;
        harness.deviceLimits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000);
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        const auto opcodes = countFullPatchOpcodes(harness.queue);
        expectEquals(opcodes.slotWrite, 0);
        expectEquals(opcodes.editBuffer, 1);
        expectEquals(opcodes.remoteParamEdit, 0);
        expectEquals(opcodes.matrixModEdit, 0);
    }

    void mutate_matrix6_sendsPatchSlot()
    {
        beginTest("mutate_matrix6_sendsPatchSlot");

        EngineHarness harness;
        harness.deviceLimits = Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6);
        harness.currentPatchNumber = 17;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        const auto opcodes = countFullPatchOpcodes(harness.queue);
        expectEquals(opcodes.slotWrite, 1);
        expectEquals(opcodes.editBuffer, 0);
        expectEquals(opcodes.remoteParamEdit, 0);
    }

    void mutate_neverDeletesRoots()
    {
        beginTest("mutate_neverDeletesRoots");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto m00 = makeDistinctBuffer(31);
        const auto m02 = makeDistinctBuffer(32);
        const auto parent = makeDistinctBuffer(33);
        expect(harness.store().insertRoot(0, m00, parent));
        expect(harness.store().insertRoot(2, m02, parent));

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(harness.store().hasRoot(0));
        expect(harness.store().hasRoot(2));
    }

    void mutate_success_updatesApvtsHistory()
    {
        beginTest("mutate_success_updatesApvtsHistory");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);

        const auto result = harness.engine.mutate();
        expect(result.success);
        expect(! harness.proc.apvts.state.getProperty(MutatorState::kHistoryMutateList).toString().isEmpty());
    }

    void footerForDiverseMutation_noUsableRoll_usesPrefixedNoChangeSticky()
    {
        beginTest("footerForDiverseMutation_noUsableRoll_usesPrefixedNoChangeSticky");

        expectEquals(juce::String(PatchMutatorEngineInternal::kNoMutationChangeFooterMessage),
                     juce::String("Patch Mutator: No changes. Try a wider MODE or more modules."));
    }

};

static PatchMutatorEngineMutateTests patchMutatorEngineMutateTests;
