#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerBankReclickTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerBankReclickTests()
        : juce::UnitTest("PatchManagerActionHandlerBankReclick")
    {
    }

    void runTest() override
    {
        testBankSelect_reclickCurrentBank_isNoOp();
        testBankSelect_reclickCurrentBank_whileComputerFocus_reclaimsInternal();
        testBankSelect_reclickCurrentBank_whileComputerFocus_dumpFailKeepsInternal();
        testBankSelect_reclickCurrentBank_skipsUnsavedGate();
        testBankSelect_differentBank_stillLoadsPatch00();
    }

private:
    static int navigationFocus(const HandlerHarness& harness)
    {
        return static_cast<int>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kDefault));
    }

    juce::File armPendingComputerSelectSettle(HandlerHarness& harness)
    {
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            2,
            nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());
        return tempDir;
    }

    void expectInternalSlotReloaded(HandlerHarness& harness, int bank, int patch)
    {
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)),
                     bank);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     patch);
        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.gateState->calls, 0);
        expectEquals(harness.dumpFakeState->requestCount, 1);
        expectEquals(static_cast<int>(harness.dumpFakeState->lastRequestedPatch), patch);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, bank);
        expect(queued.programChangeCount >= 1);
        expectEquals(queued.lastProgramChange, patch);
        expect(! queued.editBufferPatch);
    }

    void testBankSelect_reclickCurrentBank_isNoOp()
    {
        beginTest("bankSelect_reclickCurrentBank_isNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kInternal,
            nullptr);
        harness.patchSelectionMidiSync.resetLastSyncedBank(2);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank2, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 17);
        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.dumpFakeState->requestCount, 0);
        expectEquals(harness.gateState->calls, 0);
        expect(harness.queue.isEmpty());
    }

    void testBankSelect_reclickCurrentBank_whileComputerFocus_reclaimsInternal()
    {
        beginTest("bankSelect_reclickCurrentBank_whileComputerFocus_reclaimsInternal");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        harness.patchSelectionMidiSync.resetLastSyncedBank(2);
        harness.gateState->allow = false;
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        // Arm a pending Computer select settle; reclaim must cancel it before it can audition.
        const auto tempDir = armPendingComputerSelectSettle(harness);
        expect(! harness.patchLoadHookState->invoked);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank2, juce::var());

        harness.handler.flushComputerSelectDebouncerForTests();
        expect(! harness.patchLoadHookState->invoked);
        expectInternalSlotReloaded(harness, 2, 17);

        tempDir.deleteRecursively();
    }

    void testBankSelect_reclickCurrentBank_whileComputerFocus_dumpFailKeepsInternal()
    {
        beginTest("bankSelect_reclickCurrentBank_whileComputerFocus_dumpFailKeepsInternal");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        harness.patchSelectionMidiSync.resetLastSyncedBank(2);
        harness.dumpFakeState->available = false;

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank2, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 17);
        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.dumpFakeState->requestCount, 0);
    }

    void testBankSelect_reclickCurrentBank_skipsUnsavedGate()
    {
        beginTest("bankSelect_reclickCurrentBank_skipsUnsavedGate");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.gateState->allow = false;

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank2, juce::var());

        expectEquals(harness.gateState->calls, 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 17);
        expect(harness.queue.isEmpty());
    }

    void testBankSelect_differentBank_stillLoadsPatch00()
    {
        beginTest("bankSelect_differentBank_stillLoadsPatch00");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(2);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank4, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 4);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        expectEquals(navigationFocus(harness), PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.dumpFakeState->requestCount, 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 4);
        expect(queued.programChangeCount >= 1);
        expectEquals(queued.lastProgramChange, Matrix1000Limits::kMinPatchNumber);
    }
};

static PatchManagerActionHandlerBankReclickTests patchManagerActionHandlerBankReclickTests;
