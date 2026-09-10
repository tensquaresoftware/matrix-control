#include "PatchManagerActionHandlerTestSupport.h"

#include "PatchFixturePaths.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerDropLoadExtrasTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerDropLoadExtrasTests()
        : juce::UnitTest("PatchManagerActionHandlerDropLoadExtras")
    {
    }

    void runTest() override
    {
        testDropLoad_virtualHomonymsLoadDistinctFiles();
        testDropLoad_rescanPersistedClearsVirtualList();
        testDropLoad_virtualNextNavigatesAbsoluteFiles();
        testDropLoad_virtualSaveOverwritesOrigin();
        testDropLoad_mixMergeSortsWithPathTieBreak();
        testDropLoad_openAfterVirtualUsesRealFolder();
        testDropLoad_multiBuildsVirtualListKeepsFolderPath();
        testDropLoad_gateCancelRestoresVirtualList();
        testDropLoad_emptyFolderDropKeepsFolderClearsSelection();
    }

private:
    void testDropLoad_virtualHomonymsLoadDistinctFiles()
    {
        beginTest("dropLoad_virtualHomonymsLoadDistinctFiles");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        expect(dirA.getFullPathName() != dirB.getFullPathName());
        copyFixturePatchToDir(dirA, "Patch 71.syx");

        const auto patch5 = PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx");
        expect(patch5.existsAsFile()
               && patch5.copyFileTo(dirB.getChildFile("Patch 71.syx")));

        juce::StringArray paths;
        paths.add(dirA.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dirB.getChildFile("Patch 71.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        const auto& scan = harness.patchFileService.getLastScanResult();
        expect(scan.isVirtualList() && scan.validCount == 2);
        expectEquals(scan.sortedValidFiles.size(), 2);
        expectEquals(scan.sortedValidFileNames[0], juce::String("Patch 71.syx"));
        expectEquals(scan.sortedValidFileNames[1], juce::String("Patch 71.syx"));
        expect(scan.sortedValidFiles[0].getFullPathName()
               != scan.sortedValidFiles[1].getFullPathName());
        expect(matchesCombinedFirstLoadFooter(
            harness.proc.apvts.state.getProperty("uiMessageText").toString(),
            scan.validCount,
            scan.invalidCount,
            scan.sortedValidFiles[0]));

        const auto nameAfterFirst = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 2, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expect(nameAfterFirst
               != harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString());

        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    void testDropLoad_rescanPersistedClearsVirtualList()
    {
        beginTest("dropLoad_rescanPersistedClearsVirtualList");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto realDir = createTempScanDir();
        expect(realDir.createDirectory());
        copyFixturePatchToDir(realDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, realDir);

        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        copyFixturePatchToDir(dropDir, "Patch 71.syx");
        copyFixturePatchToDir(dropDir, "Patch 66.syx");

        juce::StringArray paths;
        paths.add(dropDir.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dropDir.getChildFile("Patch 66.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expect(harness.patchFileService.getLastScanResult().isVirtualList());

        harness.handler.rescanPersistedComputerPatchesFolder();

        expect(! harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     realDir.getFullPathName());
        expectEquals(harness.patchFileService.getLastScanResult().sortedValidFileNames[0],
                     juce::String("Patch 5.syx"));

        realDir.deleteRecursively();
        dropDir.deleteRecursively();
    }

    void testDropLoad_virtualNextNavigatesAbsoluteFiles()
    {
        beginTest("dropLoad_virtualNextNavigatesAbsoluteFiles");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        copyFixturePatchToDir(dirA, "Patch 5.syx");
        copyFixturePatchToDir(dirB, "Patch 71.syx");

        juce::StringArray paths;
        paths.add(dirA.getChildFile("Patch 5.syx").getFullPathName());
        paths.add(dirB.getChildFile("Patch 71.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);

        const auto nameFirst = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();
        fireAdjacentNavigation(harness, ComputerPatches::StandaloneWidgets::kLoadNextPatchFile);
        const auto nameSecond = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     2);
        expect(nameFirst != nameSecond);
        expect(harness.patchFileService.getLastScanResult().isVirtualList());

        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    void testDropLoad_virtualSaveOverwritesOrigin()
    {
        beginTest("dropLoad_virtualSaveOverwritesOrigin");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        copyFixturePatchToDir(dirA, "Patch 71.syx");
        copyFixturePatchToDir(dirB, "Patch 66.syx");

        juce::StringArray paths;
        paths.add(dirA.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dirB.getChildFile("Patch 66.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        const auto origin = harness.patchFileService.getLastScanResult().sortedValidFiles[0];
        const auto sizeBefore = origin.getSize();
        expect(sizeBefore > 0);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        // SAVE injects an uppercase stem into the on-disk filename (same as folder mode).
        const auto written = origin.getSiblingFile("PATCH 66.syx");
        expect(written.existsAsFile());
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     FooterMessages::formatSaveSuccess(
                         FooterMessages::formatReadablePatchLocation(written)));
        expect(harness.patchFileService.getLastScanResult().isVirtualList());

        const int selectedId = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile));
        expect(selectedId >= 1);
        expectEquals(
            harness.patchFileService.getLastScanResult().sortedValidFiles[selectedId - 1].getFullPathName(),
            written.getFullPathName());

        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    void testDropLoad_mixMergeSortsWithPathTieBreak()
    {
        beginTest("dropLoad_mixMergeSortsWithPathTieBreak");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        expect(dirA.getFullPathName() != dirB.getFullPathName());
        copyFixturePatchToDir(dirA, "Patch 71.syx");
        copyFixturePatchToDir(dirB, "Patch 71.syx");
        copyFixturePatchToDir(dirA, "Patch 5.syx");

        expect(dirA.getChildFile("Patch 71.syx").existsAsFile());
        expect(dirB.getChildFile("Patch 71.syx").existsAsFile());
        expect(dirA.getChildFile("Patch 5.syx").existsAsFile());

        juce::StringArray paths;
        paths.add(dirA.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dirB.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dirA.getChildFile("Patch 5.syx").getFullPathName());

        const auto result = harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits);
        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        const auto& scan = harness.patchFileService.getLastScanResult();
        expect(scan.isVirtualList());
        expectEquals(scan.validCount, 3);
        expectEquals(scan.sortedValidFiles.size(), 3);

        // Basename order with path tie-break for identical "Patch 71.syx".
        expectEquals(scan.sortedValidFileNames[0], juce::String("Patch 5.syx"));
        expectEquals(scan.sortedValidFileNames[1], juce::String("Patch 71.syx"));
        expectEquals(scan.sortedValidFileNames[2], juce::String("Patch 71.syx"));
        expect(scan.sortedValidFiles[1].getFullPathName().compareIgnoreCase(
                   scan.sortedValidFiles[2].getFullPathName())
               < 0);

        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    void testDropLoad_openAfterVirtualUsesRealFolder()
    {
        beginTest("dropLoad_openAfterVirtualUsesRealFolder");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto realDir = createTempScanDir();
        expect(realDir.createDirectory());
        copyFixturePatchToDir(realDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, realDir);

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        copyFixturePatchToDir(dirA, "Patch 71.syx");
        copyFixturePatchToDir(dirB, "Patch 66.syx");

        juce::StringArray paths;
        paths.add(dirA.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dirB.getChildFile("Patch 66.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expect(harness.patchFileService.getLastScanResult().isVirtualList());

        const auto openDir = createTempScanDir();
        expect(openDir.createDirectory());
        copyFixturePatchToDir(openDir, "Patch 808.syx");
        harness.pickFolderCallback = [openDir]() { return openDir; };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kOpenPatchFolder, juce::var());

        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     openDir.getFullPathName());
        expect(! harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 1);
        expectEquals(harness.patchFileService.getLastScanResult().sortedValidFileNames[0],
                     juce::String("Patch 808.syx"));

        realDir.deleteRecursively();
        dirA.deleteRecursively();
        dirB.deleteRecursively();
        openDir.deleteRecursively();
    }

    void testDropLoad_multiBuildsVirtualListKeepsFolderPath()
    {
        beginTest("dropLoad_multiBuildsVirtualListKeepsFolderPath");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);
        const auto folderBefore = priorDir.getFullPathName();

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        copyFixturePatchToDir(dirA, "Patch 71.syx");
        copyFixturePatchToDir(dirB, "Patch 66.syx");

        juce::StringArray paths;
        paths.add(dirA.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(dirB.getChildFile("Patch 66.syx").getFullPathName());

        const auto result = harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     folderBefore);
        expect(harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 2);
        expectEquals(harness.patchFileService.getLastScanResult().sortedValidFiles.size(), 2);

        priorDir.deleteRecursively();
        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    juce::Array<juce::File> loadTwoFileVirtualList(HandlerHarness& harness,
                                                   juce::File& outDirA,
                                                   juce::File& outDirB)
    {
        outDirA = createTempScanDir();
        expect(outDirA.createDirectory());
        outDirB = createTempScanDir();
        expect(outDirB.createDirectory());
        copyFixturePatchToDir(outDirA, "Patch 71.syx");
        copyFixturePatchToDir(outDirB, "Patch 66.syx");

        juce::StringArray paths;
        paths.add(outDirA.getChildFile("Patch 71.syx").getFullPathName());
        paths.add(outDirB.getChildFile("Patch 66.syx").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        return harness.patchFileService.getLastScanResult().sortedValidFiles;
    }

    void testDropLoad_gateCancelRestoresVirtualList()
    {
        beginTest("dropLoad_gateCancelRestoresVirtualList");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);

        juce::File dirA, dirB;
        const auto virtualBefore = loadTwoFileVirtualList(harness, dirA, dirB);
        const int selectBefore = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile));

        const auto nextDropDir = createTempScanDir();
        expect(nextDropDir.createDirectory());
        copyFixturePatchToDir(nextDropDir, "Patch 808.syx");
        harness.gateState->allow = false;
        expect(harness.handler.loadDroppedComputerPatchFile(
                   nextDropDir.getChildFile("Patch 808.syx"), harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kCancelled);

        const auto& scan = harness.patchFileService.getLastScanResult();
        expect(scan.isVirtualList());
        expectEquals(scan.sortedValidFiles.size(), 2);
        expectEquals(scan.sortedValidFiles[0].getFullPathName(), virtualBefore[0].getFullPathName());
        expectEquals(scan.sortedValidFiles[1].getFullPathName(), virtualBefore[1].getFullPathName());
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     priorDir.getFullPathName());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     selectBefore);

        priorDir.deleteRecursively();
        dirA.deleteRecursively();
        dirB.deleteRecursively();
        nextDropDir.deleteRecursively();
    }

    void testDropLoad_emptyFolderDropKeepsFolderClearsSelection()
    {
        beginTest("dropLoad_emptyFolderDropKeepsFolderClearsSelection");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);
        harness.handler.flushComputerSelectDebouncerForTests();

        const auto emptyDir = createTempScanDir();
        expect(emptyDir.createDirectory());
        expect(emptyDir.getChildFile("notes.txt").replaceWithText("x"));

        juce::StringArray paths;
        paths.add(emptyDir.getFullPathName());
        const auto result = harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kRejected);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     emptyDir.getFullPathName());
        expect(! harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     0);

        priorDir.deleteRecursively();
        emptyDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerDropLoadExtrasTests patchManagerActionHandlerDropLoadExtrasTests;
