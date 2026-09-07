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
        testBankSelect_reclickCurrentBank_whileComputerFocus_isNoOp();
        testBankSelect_reclickCurrentBank_skipsUnsavedGate();
        testBankSelect_differentBank_stillLoadsPatch00();
    }

private:
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
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        expectEquals(harness.dumpFakeState->requestCount, 0);
        expectEquals(harness.gateState->calls, 0);
        expect(harness.queue.isEmpty());
    }

    void testBankSelect_reclickCurrentBank_whileComputerFocus_isNoOp()
    {
        beginTest("bankSelect_reclickCurrentBank_whileComputerFocus_isNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 17, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        harness.useSuccessfulDeviceDump();

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank2, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 17);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kComputer);
        expectEquals(harness.dumpFakeState->requestCount, 0);
        expect(harness.queue.isEmpty());
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
        expectEquals(harness.dumpFakeState->requestCount, 1);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 4);
        expect(queued.programChangeCount >= 1);
        expectEquals(queued.lastProgramChange, Matrix1000Limits::kMinPatchNumber);
    }
};

static PatchManagerActionHandlerBankReclickTests patchManagerActionHandlerBankReclickTests;
