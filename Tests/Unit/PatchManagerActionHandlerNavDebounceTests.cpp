#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerNavDebounceTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerNavDebounceTests() : juce::UnitTest("PatchManagerActionHandlerNavDebounce") {}

    void runTest() override
    {
        internalNav_burst_coalescesToFinalPatch();
        internalNav_wrapDuringBurst_loadsFinalOnly();
        internalNav_cancelAtSettle_restoresBaselineWithoutAbandonedMidi();
        internalNav_burst_confirmOnlyAtSettle();
        internalNav_newBurst_supersedesPendingDump();
        computerNav_burst_coalescesToFinalFile();
        computerSelect_burst_coalescesLoad();
        computerNav_cancelAtSettle_revertsWithoutSysEx();
        crossPath_internalThenComputerSelect_clearsInternalBaseline();
        crossPath_internalClaim_cancelsPendingComputerSettle();
        crossPath_dumpFailAfterFurtherInternalSteps_keepsLatestIndex();
    }

private:
    void internalNav_burst_coalescesToFinalPatch()
    {
        beginTest("internalNav_burst_coalescesToFinalPatch");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();

        for (int i = 0; i < 5; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 10);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(0));
        expect(harness.queue.isEmpty());

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 10);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(10));
        const auto queued = scanQueue(harness.queue);
        expectEquals(queued.programChangeCount, 1);
        expectEquals(queued.lastProgramChange, 10);
    }

    void internalNav_wrapDuringBurst_loadsFinalOnly()
    {
        beginTest("internalNav_wrapDuringBurst_loadsFinalOnly");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state,
                                    0,
                                    Matrix1000Limits::kMaxPatchNumber,
                                    true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();

        for (int i = 0; i < 3; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 2);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(2));
        const auto queued = scanQueue(harness.queue);
        expectEquals(queued.programChangeCount, 1);
        expectEquals(queued.lastProgramChange, 2);
    }

    void internalNav_cancelAtSettle_restoresBaselineWithoutAbandonedMidi()
    {
        beginTest("internalNav_cancelAtSettle_restoresBaselineWithoutAbandonedMidi");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.gateState->allow = false;

        for (int i = 0; i < 4; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 9);

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(harness.gateState->calls, 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 5);
        const auto queued = scanQueue(harness.queue);
        expectEquals(queued.programChangeCount, 1);
        expectEquals(queued.lastProgramChange, 5);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(0));
    }

    void internalNav_burst_confirmOnlyAtSettle()
    {
        beginTest("internalNav_burst_confirmOnlyAtSettle");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();

        for (int i = 0; i < 4; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(harness.gateState->calls, 0);

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(harness.gateState->calls, 1);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(9));
    }

    void internalNav_newBurst_supersedesPendingDump()
    {
        beginTest("internalNav_newBurst_supersedesPendingDump");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 8, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 9);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(9));
        auto firstCallback = std::move(harness.dumpFakeState->pendingCallback);
        expect(firstCallback != nullptr);

        for (int i = 0; i < 3; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 12);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(12));

        // Stale dump for the abandoned target must not apply.
        firstCallback(harness.dumpFakeState->response);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 12);
        expect(! harness.patchLoadHookState->invoked);

        harness.fireDeferredDump();
        expect(harness.patchLoadHookState->invoked);
    }

    void computerNav_burst_coalescesToFinalFile()
    {
        beginTest("computerNav_burst_coalescesToFinalFile");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);
        simulateSelectPatchFileDispatch(harness);
        harness.patchLoadHookState->invoked = false;

        while (! harness.queue.isEmpty())
            (void) harness.queue.dequeue();

        for (int i = 0; i < 2; ++i)
            harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kLoadNextPatchFile, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     3);
        expect(harness.queue.isEmpty());
        expect(! harness.patchLoadHookState->invoked);

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     3);
        expect(harness.patchLoadHookState->invoked);
        expect(scanQueue(harness.queue).editBufferPatch);

        tempDir.deleteRecursively();
    }

    void computerSelect_burst_coalescesLoad()
    {
        beginTest("computerSelect_burst_coalescesLoad");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            2,
            nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            3,
            nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        expect(! harness.patchLoadHookState->invoked);
        expect(harness.queue.isEmpty());

        harness.handler.flushComputerSelectDebouncerForTests();

        expect(harness.patchLoadHookState->invoked);
        expect(scanQueue(harness.queue).editBufferPatch);
        expectEquals(harness.gateState->calls, 1);

        tempDir.deleteRecursively();
    }

    void computerNav_cancelAtSettle_revertsWithoutSysEx()
    {
        beginTest("computerNav_cancelAtSettle_revertsWithoutSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);
        simulateSelectPatchFileDispatch(harness);
        harness.patchLoadHookState->invoked = false;
        harness.gateState->calls = 0;
        harness.gateState->allow = false;

        while (! harness.queue.isEmpty())
            (void) harness.queue.dequeue();

        for (int i = 0; i < 2; ++i)
            harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kLoadNextPatchFile, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     3);

        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(harness.gateState->calls, 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);
        expect(! harness.patchLoadHookState->invoked);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void crossPath_internalThenComputerSelect_clearsInternalBaseline()
    {
        beginTest("crossPath_internalThenComputerSelect_clearsInternalBaseline");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        setupComputerPatchesScan(harness, tempDir);

        for (int i = 0; i < 3; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 8);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            2,
            nullptr);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSelectPatchFile, juce::var());

        // Internal settle must have been abandoned with coords restored to baseline.
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 5);

        harness.handler.flushPatchNavDebouncerForTests();
        harness.handler.flushComputerSelectDebouncerForTests();

        expect(harness.patchLoadHookState->invoked);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(0));

        tempDir.deleteRecursively();
    }

    static int selectedComputerPatchId(HandlerHarness& harness)
    {
        return static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile));
    }

    static int currentInternalPatch(HandlerHarness& harness)
    {
        return static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber));
    }

    juce::File prepareThreeFileComputerBrowser(HandlerHarness& harness)
    {
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        return tempDir;
    }

    void crossPath_internalClaim_cancelsPendingComputerSettle()
    {
        beginTest("crossPath_internalClaim_cancelsPendingComputerSettle");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();

        const auto tempDir = prepareThreeFileComputerBrowser(harness);
        harness.patchLoadHookState->invoked = false;
        while (! harness.queue.isEmpty())
            (void) harness.queue.dequeue();

        for (int i = 0; i < 2; ++i)
            harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kLoadNextPatchFile, juce::var());
        expectEquals(selectedComputerPatchId(harness), 3);

        harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());
        expectEquals(currentInternalPatch(harness), 6);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        expectEquals(selectedComputerPatchId(harness), 1);
        expect(! harness.patchLoadHookState->invoked);

        harness.handler.flushPatchNavDebouncerForTests();
        harness.handler.flushComputerSelectDebouncerForTests();
        expectEquals(currentInternalPatch(harness), 6);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(6));
        expectEquals(selectedComputerPatchId(harness), 1);
        expect(harness.patchLoadHookState->invoked);

        tempDir.deleteRecursively();
    }

    void crossPath_dumpFailAfterFurtherInternalSteps_keepsLatestIndex()
    {
        beginTest("crossPath_dumpFailAfterFurtherInternalSteps_keepsLatestIndex");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.useSuccessfulDeviceDump();
        harness.dumpFakeState->deferCallback = true;

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 6);
        expectEquals(harness.dumpFakeState->lastRequestedPatch, static_cast<juce::uint8>(6));
        auto pendingDump = std::move(harness.dumpFakeState->pendingCallback);
        expect(pendingDump != nullptr);

        // User keeps stepping while the dump for 6 is still outstanding.
        for (int i = 0; i < 3; ++i)
            harness.handler.handleAction(InternalPatches::kLoadNextPatch, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 9);

        // Failed/empty dump for the abandoned target must not yank NumberBoxes backward.
        pendingDump({});
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 9);
        expect(! harness.patchLoadHookState->invoked);

        // Settle the newer burst with a failing dump — still KEEP_DISPLAYED at latest index.
        harness.dumpFakeState->response.clear();
        harness.dumpFakeState->deferCallback = false;
        harness.handler.flushPatchNavDebouncerForTests();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 9);
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(! harness.patchLoadHookState->invoked);
    }
};

static PatchManagerActionHandlerNavDebounceTests patchManagerActionHandlerNavDebounceTests;
