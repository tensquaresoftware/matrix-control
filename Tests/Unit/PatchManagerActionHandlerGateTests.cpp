#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerGateTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerGateTests() : juce::UnitTest("PatchManagerActionHandlerGate") {}

    void runTest() override
    {
        testHistoryGate_cancelAbortsNavigation();
        testHistoryGate_proceedAllowsNavigation();
        testHistoryGate_cancelAbortsInit();
        testUnsavedGate_initIncludesUnsavedWarningFlag();
        testUnsavedGate_initMarksNotStoredInRam();
        testUnsavedGate_storeClearsNotStoredInRam();
        testUnsavedGate_pasteSkipsContextGate();
        testUnsavedGate_secondInitWhileNotStoredStillGates();
        testUnsavedGate_tryPersistStoreClearsNotStored();
        testUnsavedGate_tryPersistStoreBlockedKeepsNotStored();
        testUnsavedGate_loadCancelRevertsSelection();
        testUnsavedGate_prevNextCancelDoesNotDoubleLoad();
        testUnsavedGate_openCancelRestoresPriorBrowser();
        testUnsavedGate_captureAfterLoadLeavesClean();
        testUnsavedGate_captureAfterSaveLeavesClean();
        testUnsavedGate_captureAfterStoreLeavesClean();
        testUnsavedGate_storeBlockedKeepsDirty();
        testUnsavedGate_captureAfterInitThenEditIsDirty();
    }

private:
    void testHistoryGate_cancelAbortsNavigation()
    {
        beginTest("historyGate_cancelAbortsNavigation");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.gateState->allow = false;

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        // Gate was consulted and, on Cancel, coordinates stay put with no MIDI emitted.
        expectEquals(harness.gateState->calls, 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 5);
        const auto queued = scanQueue(harness.queue);
        expect(!queued.setBank);
        expect(queued.patchSysExCount == 0);
    }

    void testHistoryGate_proceedAllowsNavigation()
    {
        beginTest("historyGate_proceedAllowsNavigation");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.gateState->allow = true;

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(harness.gateState->calls, 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 6);
    }

    void testHistoryGate_cancelAbortsInit()
    {
        beginTest("historyGate_cancelAbortsInit");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 12, false);
        harness.gateState->allow = false;

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        // On Cancel the patch load hook (history reset) must not fire.
        expectEquals(harness.gateState->calls, 1);
        expect(harness.gateState->lastIncludeUnsavedEditWarning);
        expect(!harness.patchLoadHookState->invoked);
    }

    void testUnsavedGate_initIncludesUnsavedWarningFlag()
    {
        beginTest("unsavedGate_initIncludesUnsavedWarningFlag");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expectEquals(harness.gateState->calls, 1);
        expect(harness.gateState->lastIncludeUnsavedEditWarning);
    }

    void testUnsavedGate_initMarksNotStoredInRam()
    {
        beginTest("unsavedGate_initMarksNotStoredInRam");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expect(harness.dirtyPatchTracker.hasSnapshot());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(harness.handler.isPatchNotStoredInRam());
    }

    void testUnsavedGate_storeClearsNotStoredInRam()
    {
        beginTest("unsavedGate_storeClearsNotStoredInRam");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());
        expect(harness.handler.isPatchNotStoredInRam());
        scanQueue(harness.queue); // drain INIT audition SysEx before STORE assertions

        // INIT leaves `* INIT *` — STORE defers (name-required); notStored stays until a real name.
        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());
        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        expect(harness.queue.isEmpty());
        expect(harness.handler.isPatchNotStoredInRam());

        harness.model.setName("NEWNAMED");
        harness.patchNameSyncer.bufferToApvts();
        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        expect(! harness.handler.isPatchNotStoredInRam());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
    }

    void testUnsavedGate_pasteSkipsContextGate()
    {
        beginTest("unsavedGate_pasteSkipsContextGate");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyFullPatch(harness.model, "BANK 0 / PATCH 0");
        harness.gateState->allow = false;
        harness.gateState->calls = 0;

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        expectEquals(harness.gateState->calls, 0);
        expect(harness.patchLoadHookState->invoked);
    }

    void testUnsavedGate_secondInitWhileNotStoredStillGates()
    {
        beginTest("unsavedGate_secondInitWhileNotStoredStillGates");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());
        expect(harness.handler.isPatchNotStoredInRam());
        expectEquals(harness.gateState->calls, 1);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expectEquals(harness.gateState->calls, 2);
        expect(harness.gateState->lastIncludeUnsavedEditWarning);
        expect(harness.handler.isPatchNotStoredInRam());
    }

    void testUnsavedGate_tryPersistStoreClearsNotStored()
    {
        beginTest("unsavedGate_tryPersistStoreClearsNotStored");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());
        expect(harness.handler.isPatchNotStoredInRam());
        scanQueue(harness.queue); // drain INIT audition SysEx before Persist assertions

        // Persist→STORE while sentinel: same deferral as button STORE — persist fails until renamed.
        expect(! harness.handler.tryPersistCurrentPatchFromUnsavedGate(
            Core::UnsavedEditPersistKind::kStore));
        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        expect(harness.queue.isEmpty());
        expect(harness.handler.isPatchNotStoredInRam());

        harness.model.setName("NEWNAMED");
        harness.patchNameSyncer.bufferToApvts();
        expect(harness.handler.tryPersistCurrentPatchFromUnsavedGate(
            Core::UnsavedEditPersistKind::kStore));
        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 1);
        expect(! harness.handler.isPatchNotStoredInRam());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
    }

    void testUnsavedGate_tryPersistStoreBlockedKeepsNotStored()
    {
        beginTest("unsavedGate_tryPersistStoreBlockedKeepsNotStored");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());
        expect(harness.handler.isPatchNotStoredInRam());

        harness.proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        expect(! harness.handler.tryPersistCurrentPatchFromUnsavedGate(
            Core::UnsavedEditPersistKind::kStore));
        expect(harness.handler.isPatchNotStoredInRam());
    }




    void testUnsavedGate_loadCancelRevertsSelection()
    {
        beginTest("unsavedGate_loadCancelRevertsSelection");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 1);

        harness.gateState->allow = false;
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 2, nullptr);
        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.gateState->calls, 2);
        expect(harness.gateState->lastIncludeUnsavedEditWarning);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 1);

        tempDir.deleteRecursively();
    }

    void testUnsavedGate_prevNextCancelDoesNotDoubleLoad()
    {
        beginTest("unsavedGate_prevNextCancelDoesNotDoubleLoad");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expectEquals(harness.gateState->calls, 1);

        SelectPatchFileLoadDispatcher dispatcher(harness);
        harness.gateState->allow = false;
        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadNextPatchFile);

        expectEquals(harness.gateState->calls, 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 1);

        tempDir.deleteRecursively();
    }

    void testUnsavedGate_openCancelRestoresPriorBrowser()
    {
        beginTest("unsavedGate_openCancelRestoresPriorBrowser");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto folderA = createTempScanDir();
        const auto folderB = createTempScanDir();
        expect(folderA.createDirectory());
        expect(folderB.createDirectory());
        copyFixturePatchToDir(folderA, "Patch 5.syx");
        copyFixturePatchToDir(folderA, "Patch 71.syx");
        copyFixturePatchToDir(folderB, "Patch 71.syx");

        setupComputerPatchesScan(harness, folderA);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 2, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 2);

        SelectPatchFileLoadDispatcher dispatcher(harness);
        harness.gateState->allow = false;
        harness.pickFolderCallback = [&folderB]() { return folderB; };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expectEquals(harness.gateState->calls, 2);
        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == folderA.getFullPathName());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 2);
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 2);

        folderA.deleteRecursively();
        folderB.deleteRecursively();
    }

    void testUnsavedGate_captureAfterLoadLeavesClean()
    {
        beginTest("unsavedGate_captureAfterLoadLeavesClean");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);

        expect(harness.dirtyPatchTracker.hasSnapshot());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));

        tempDir.deleteRecursively();
    }

    void testUnsavedGate_captureAfterSaveLeavesClean()
    {
        beginTest("unsavedGate_captureAfterSaveLeavesClean");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);

        harness.model.setName("EDITED!!");
        harness.patchNameSyncer.bufferToApvts();
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(harness.dirtyPatchTracker.hasSnapshot());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(! harness.handler.isPatchNotStoredInRam());

        tempDir.deleteRecursively();
    }


    void testUnsavedGate_captureAfterStoreLeavesClean()
    {
        beginTest("unsavedGate_captureAfterStoreLeavesClean");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);

        harness.model.setName("EDITED!!");
        harness.patchNameSyncer.bufferToApvts();
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expect(harness.dirtyPatchTracker.hasSnapshot());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
    }

    void testUnsavedGate_storeBlockedKeepsDirty()
    {
        beginTest("unsavedGate_storeBlockedKeepsDirty");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);

        harness.model.setName("EDITED!!");
        harness.patchNameSyncer.bufferToApvts();
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));

        harness.proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
    }

    void testUnsavedGate_captureAfterInitThenEditIsDirty()
    {
        beginTest("unsavedGate_captureAfterInitThenEditIsDirty");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 3, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expect(harness.dirtyPatchTracker.hasSnapshot());
        expect(! harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));

        harness.model.setName("DIRTYINI");
        harness.patchNameSyncer.bufferToApvts();
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
    }
};

static PatchManagerActionHandlerGateTests patchManagerActionHandlerGateTests;
