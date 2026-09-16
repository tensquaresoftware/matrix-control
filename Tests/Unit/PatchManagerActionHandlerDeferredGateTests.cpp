#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerDeferredGateTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerDeferredGateTests() : juce::UnitTest("PatchManagerActionHandlerDeferredGate") {}

    void runTest() override
    {
        testUnsavedGate_continueDumpUnavailableRollsBackCoords();
        testUnsavedGate_continueAsyncEmptyDumpRollsBackCoords();
        testUnsavedGate_continueEditBeforeDumpAbortsApply();
        testUnsavedGate_bootstrapCancelBeforeFirstCommit();
        testUnsavedGate_comboCancelUsesBaselineBeforeFirstCommit();
        testUnsavedGate_bankDumpUnavailableRollsBackCoords();
        testUnsavedGate_numberBoxPriorSnapshotRestoresUndefinedCoords();
        testUnsavedGate_computerLoadAbandonsPendingDeviceDump();
        testUnsavedGate_deferredHistoryDiscardSurvivesReconcileCancel();
        testUnsavedGate_openCancelAfterHistoryGateCancel();
    }

private:
    void testUnsavedGate_continueDumpUnavailableRollsBackCoords()
    {
        beginTest("unsavedGate_continueDumpUnavailableRollsBackCoords");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 10, false);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);
        harness.model.setName("DIRTY!!!");
        harness.patchNameSyncer.bufferToApvts();
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));

        harness.dumpFakeState->available = false;
        harness.patchLoadHookState->invoked = false;
        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 10);
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(! harness.patchLoadHookState->invoked);
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String("PATCH MUTATOR: Could not read the patch from the synth. Keeping the current editor buffer. "
                                  "Check that MIDI FROM is the synth MIDI OUT."));
        expect(harness.model.getName() == "DIRTY!!!");
    }

    void testUnsavedGate_continueAsyncEmptyDumpRollsBackCoords()
    {
        beginTest("unsavedGate_continueAsyncEmptyDumpRollsBackCoords");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);
        harness.model.setName("KEEPEDIT");
        harness.patchNameSyncer.bufferToApvts();

        harness.dumpFakeState->available = true;
        harness.dumpFakeState->response.clear();
        harness.patchLoadHookState->invoked = false;
        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 4);
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(! harness.patchLoadHookState->invoked);
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.model.getName() == "KEEPEDIT");
    }

    void testUnsavedGate_continueEditBeforeDumpAbortsApply()
    {
        beginTest("unsavedGate_continueEditBeforeDumpAbortsApply");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 8, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);
        harness.mapper.apvtsToBuffer();
        harness.model.setName("BASEEDIT");
        harness.patchNameSyncer.bufferToApvts();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);
        harness.model.setName("DIRTYPRE");
        harness.patchNameSyncer.bufferToApvts();

        harness.dumpFakeState->deferCallback = true;
        harness.patchLoadHookState->invoked = false;
        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 9);

        harness.model.setName("MIDEDIT!");
        harness.patchNameSyncer.bufferToApvts();
        harness.fireDeferredDump();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 8);
        expect(harness.model.getName() == "MIDEDIT!");
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(! harness.patchLoadHookState->invoked);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String("PATCH MUTATOR: Synth patch load cancelled because the editor changed while waiting. "
                                  "Bank and patch numbers were restored; your edits were kept."));
    }

    void testUnsavedGate_bootstrapCancelBeforeFirstCommit()
    {
        beginTest("unsavedGate_bootstrapCancelBeforeFirstCommit");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        // Selection present in APVTS but never remembered via a successful load.
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 2, nullptr);

        SelectPatchFileLoadDispatcher dispatcher(harness);
        harness.gateState->allow = false;
        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadNextPatchFile);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 2);

        tempDir.deleteRecursively();
    }

    void testUnsavedGate_comboCancelUsesBaselineBeforeFirstCommit()
    {
        beginTest("unsavedGate_comboCancelUsesBaselineBeforeFirstCommit");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 2, nullptr);

        SelectPatchFileLoadDispatcher dispatcher(harness);
        harness.gateState->allow = false;
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kSelectPatchCancelBaseline, 2, nullptr);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 2);

        tempDir.deleteRecursively();
    }

    void testUnsavedGate_bankDumpUnavailableRollsBackCoords()
    {
        beginTest("unsavedGate_bankDumpUnavailableRollsBackCoords");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 10, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(1);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);
        harness.model.setName("BANKKEEP");
        harness.patchNameSyncer.bufferToApvts();

        harness.dumpFakeState->available = false;
        harness.patchLoadHookState->invoked = false;
        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 10);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kNone);
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(! harness.patchLoadHookState->invoked);
        expect(harness.model.getName() == "BANKKEEP");
    }

    void testUnsavedGate_numberBoxPriorSnapshotRestoresUndefinedCoords()
    {
        beginTest("unsavedGate_numberBoxPriorSnapshotRestoresUndefinedCoords");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 10, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(1);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);
        harness.model.setName("NUMBKEEP");
        harness.patchNameSyncer.bufferToApvts();

        // Simulate NumberBox: advance patch + establish, then load with a true pre-nav snapshot.
        harness.proc.apvts.state.setProperty(InternalPatches::kCurrentPatchNumber, 11, nullptr);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished, true, nullptr);
        harness.dumpFakeState->available = false;
        harness.patchLoadHookState->invoked = false;
        harness.handler.loadCurrentPatchFromDevice(
            harness.limits,
            Core::PatchManagerActionHandler::InternalCoordinatesSnapshot {
                1, 10, 1, false, PatchManager::NavigationFocus::kNone });

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 10);
        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kNone);
        expect(harness.dirtyPatchTracker.syncApvtsAndIsDirty(
            harness.mapper, harness.patchNameSyncer, harness.model));
        expect(! harness.patchLoadHookState->invoked);
        expect(harness.model.getName() == "NUMBKEEP");
    }

    void testUnsavedGate_computerLoadAbandonsPendingDeviceDump()
    {
        beginTest("unsavedGate_computerLoadAbandonsPendingDeviceDump");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, true);
        harness.mapper.apvtsToBuffer();
        harness.dirtyPatchTracker.captureSnapshot(harness.model);

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        setupComputerPatchesScan(harness, tempDir);

        harness.dumpFakeState->available = true;
        harness.dumpFakeState->deferCallback = true;
        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 5);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expect(harness.patchLoadHookState->invoked);

        const auto nameAfterComputerLoad = harness.model.getName();
        harness.fireDeferredDump();

        expect(harness.model.getName() == nameAfterComputerLoad);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 5);

        tempDir.deleteRecursively();
    }

    bool seedOutsideAndFixturePatches(HandlerHarness& harness, const juce::File& tempDir)
    {
        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        const bool saved = harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("OUTSIDE.syx"),
            exportModel.data(),
            harness.sysExEncoder).success;
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        return saved;
    }

    juce::String loadSecondPatchPreferInternal(HandlerHarness& harness, const juce::File& tempDir)
    {
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kDisplaySysexNames,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        // Sorted: OUTSIDE.syx = 1, Patch 71.syx = 2. Load 2 first (no ask).
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 2, nullptr);
        simulateSelectPatchFileDispatch(harness);
        return harness.model.getName();
    }

    void armAskOnceCancelAndSelectFirst(HandlerHarness& harness)
    {
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kAskOncePerLoad,
            nullptr);
        harness.patchLoadHookState->invoked = false;
        harness.pickReconciliationCallback =
            [](juce::String, juce::String) -> std::optional<Core::NameReconciliationChoice>
            {
                return std::nullopt;
            };

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
    }

    void testUnsavedGate_deferredHistoryDiscardSurvivesReconcileCancel()
    {
        beginTest("unsavedGate_deferredHistoryDiscardSurvivesReconcileCancel");

        // Gate Discard clears Mutator history only via onPatchLoaded after a successful load.
        // Reconcile Cancel must restore the prior packed model, revert Computer selection, and
        // must not invoke onPatchLoaded (so deferred Discard leaves history intact).
        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(seedOutsideAndFixturePatches(harness, tempDir));

        const auto nameAfterFirstLoad = loadSecondPatchPreferInternal(harness, tempDir);
        expect(harness.patchLoadHookState->invoked);

        armAskOnceCancelAndSelectFirst(harness);

        expect(! harness.patchLoadHookState->invoked);
        expect(harness.model.getName() == nameAfterFirstLoad);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 2);

        tempDir.deleteRecursively();
    }

    void testUnsavedGate_openCancelAfterHistoryGateCancel()
    {
        beginTest("unsavedGate_openCancelAfterHistoryGateCancel");

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

        SelectPatchFileLoadDispatcher dispatcher(harness);
        harness.gateState->allow = false;
        harness.pickFolderCallback = [&folderB]() { return folderB; };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == folderA.getFullPathName());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)), 2);

        folderA.deleteRecursively();
        folderB.deleteRecursively();
    }
};

static PatchManagerActionHandlerDeferredGateTests patchManagerActionHandlerDeferredGateTests;
