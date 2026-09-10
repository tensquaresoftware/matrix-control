#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerBrowserTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerBrowserTests() : juce::UnitTest("PatchManagerActionHandlerBrowser") {}

    void runTest() override
    {
        testOpenPersistsFolderPath();
        testOpenCancelledDoesNotPersist();
        testOpenAutoSelectsAndLoadsFirst();
        testOpenEmptyFolderNoLoad();
        testOpenUndefinedCoordinates_establishesDestinationWithoutDeviceLoad();
        testOpenEstablishedCoordinates_keepsThem();
        testOpenEmptyFolder_leavesCoordinatesUndefined();
        testOpenEmptyFolder_preservesInternalFocus();
        testOpenEmptyFolder_clearsComputerFocus();
        testOpenUndefined_cancelLeavesCoordinatesUnestablished();
        testOpenAlreadySelectedFirstReloads();
        testSessionLoadResetsBrowserThenRescansPersistedFolder();
        testRescanPersistedFolderMissingPathWarningFooter();
        testRescanPersistedFolderEmptyPathNoOp();
        testRescanPersistedFolderEmptyPathClearsStaleCache();
        testRescanPersistedFolderNoSysEx();
        testFolderPathSessionXmlRoundTrip();
        testNavigationFocus_switchesBetweenComputerAndInternal();
    }

private:
    void testOpenPersistsFolderPath()
    {
        beginTest("open_persistsFolderPath");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == tempDir.getFullPathName());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            1);

        tempDir.deleteRecursively();
    }

    void testOpenCancelledDoesNotPersist()
    {
        beginTest("open_cancelled_doesNotPersist");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const juce::String keepPath = "/tmp/keep";
        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, keepPath, nullptr);
        harness.pickFolderCallback = []() { return juce::File(); };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == keepPath);
    }

    void testOpenAutoSelectsAndLoadsFirst()
    {
        beginTest("open_autoSelectsAndLoadsFirst");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            1);
        expect(harness.patchFileService.getLastScanResult().sortedValidFileNames[0]
               == "Patch 5.syx");
        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(queued.editBufferPatch);
        expect(harness.patchLoadHookState->invoked);

        tempDir.deleteRecursively();
    }

    void testOpenEmptyFolderNoLoad()
    {
        beginTest("open_emptyFolder_noLoad");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            3,
            nullptr);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            0);
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        expect(harness.queue.isEmpty());
        expect(! harness.patchLoadHookState->invoked);

        tempDir.deleteRecursively();
    }

    void testOpenUndefinedCoordinates_establishesDestinationWithoutDeviceLoad()
    {
        beginTest("open_undefinedCoordinates_establishesDestinationWithoutDeviceLoad");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)),
                     Matrix1000Limits::kMinBankNumber);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         BankUtility::StateProperties::kSelectedBank)),
                     Matrix1000Limits::kMinBankNumber);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kComputer);

        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, Matrix1000Limits::kMinBankNumber);
        // Device patch 00 must not be pulled into the editor before the .syx is applied.
        expectEquals(harness.dumpFakeState->requestCount, 0);
        expect(harness.patchLoadHookState->invoked);

        tempDir.deleteRecursively();
    }

    void testOpenEstablishedCoordinates_keepsThem()
    {
        beginTest("open_establishedCoordinates_keepsThem");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 34, true);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 34);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kComputer);

        tempDir.deleteRecursively();
    }

    void testOpenEmptyFolder_leavesCoordinatesUndefined()
    {
        beginTest("open_emptyFolder_leavesCoordinatesUndefined");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kNone);

        tempDir.deleteRecursively();
    }

    void testOpenEmptyFolder_preservesInternalFocus()
    {
        beginTest("open_emptyFolder_preservesInternalFocus");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 2, 10, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kInternal,
            nullptr);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);

        tempDir.deleteRecursively();
    }

    void testOpenEmptyFolder_clearsComputerFocus()
    {
        beginTest("open_emptyFolder_clearsComputerFocus");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 5, true);
        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kNone);

        tempDir.deleteRecursively();
    }

    void testOpenUndefined_cancelLeavesCoordinatesUnestablished()
    {
        beginTest("open_undefined_cancelLeavesCoordinatesUnestablished");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");

        SelectPatchFileLoadDispatcher dispatcher(harness);
        harness.gateState->allow = false;
        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expect(! static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kNone);

        tempDir.deleteRecursively();
    }

    void testOpenAlreadySelectedFirstReloads()
    {
        beginTest("open_alreadySelectedFirst_reloads");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);
        expect(harness.patchLoadHookState->invoked);

        harness.patchLoadHookState->invoked = false;
        while (! harness.queue.isEmpty())
            (void) harness.queue.dequeue();

        fireOpenAndDispatchLoad(harness);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            1);
        expect(harness.patchLoadHookState->invoked);
        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(queued.editBufferPatch);

        tempDir.deleteRecursively();
    }

    void testSessionLoadResetsBrowserThenRescansPersistedFolder()
    {
        beginTest("sessionLoad_resetsBrowser_thenRescansPersistedFolder");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);
        expect(harness.patchFileService.getLastScanResult().validCount > 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            1);

        const auto pathBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kFolderPath).toString();

        // Install a virtual list on top of the remembered real folder (session-only browsing).
        const auto otherDir = createTempScanDir();
        expect(otherDir.createDirectory());
        copyFixturePatchToDir(otherDir, "Patch 66.syx");
        juce::StringArray virtualPaths;
        virtualPaths.add(otherDir.getChildFile("Patch 66.syx").getFullPathName());
        virtualPaths.add(tempDir.getChildFile("Patch 71.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(virtualPaths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expect(harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString(),
                     pathBefore);

        // Same production entry as PluginProcessor::applyRestoredPluginState.
        harness.handler.applyComputerPatchesBrowserAfterSessionLoad();

        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString()
               == pathBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            0);
        expect(! harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 1);
        expect(harness.patchFileService.getLastScanResult().folderUsable);

        otherDir.deleteRecursively();
        tempDir.deleteRecursively();
    }

    void testRescanPersistedFolderMissingPathWarningFooter()
    {
        beginTest("rescanPersistedFolder_missingPath_warningFooter");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            "/nonexistent/path",
            nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto& scan = harness.patchFileService.getLastScanResult();
        expect(!scan.folderUsable);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kFolderNotFound));
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
    }

    void testRescanPersistedFolderEmptyPathNoOp()
    {
        beginTest("rescanPersistedFolder_emptyPath_noOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, juce::String(), nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        expect(!harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));
    }

    void testRescanPersistedFolderEmptyPathClearsStaleCache()
    {
        beginTest("rescanPersistedFolder_emptyPath_clearsStaleCache");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        expect(harness.patchFileService.getLastScanResult().validCount > 0);

        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, juce::String(), nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        expect(!harness.patchFileService.getLastScanResult().folderUsable);
        expect(harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));

        tempDir.deleteRecursively();
    }

    void testRescanPersistedFolderNoSysEx()
    {
        beginTest("rescanPersisted_noSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);

        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testFolderPathSessionXmlRoundTrip()
    {
        beginTest("folderPath_sessionXmlRoundTrip");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const juce::String path = "/tmp/persisted/folder";
        harness.proc.apvts.state.setProperty(ComputerPatches::StateProperties::kFolderPath, path, nullptr);

        juce::MemoryBlock destData;
        if (auto xml = harness.proc.apvts.copyState().createXml())
            juce::AudioProcessor::copyXmlToBinary(*xml, destData);

        TestAudioProcessorPatchManager restored;
        if (auto xmlState = juce::AudioProcessor::getXmlFromBinary(destData.getData(),
                                                                   static_cast<int>(destData.getSize())))
            restored.apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        expect(restored.apvts.state.getProperty(ComputerPatches::StateProperties::kFolderPath).toString() == path);
    }

    void testNavigationFocus_switchesBetweenComputerAndInternal()
    {
        beginTest("navigationFocus_switchesBetweenComputerAndInternal");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 10, true);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 5.syx");

        harness.pickFolderCallback = [&tempDir]() { return tempDir; };
        fireOpenAndDispatchLoad(harness);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kComputer);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 10);

        while (! harness.queue.isEmpty())
            (void) harness.queue.dequeue();

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 11);

        tempDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerBrowserTests patchManagerActionHandlerBrowserTests;
