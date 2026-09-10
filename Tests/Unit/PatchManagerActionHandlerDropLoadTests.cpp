#include "PatchManagerActionHandlerTestSupport.h"

#include "PatchFixturePaths.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerDropLoadTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerDropLoadTests()
        : juce::UnitTest("PatchManagerActionHandlerDropLoad")
    {
    }

    void runTest() override
    {
        testDropLoad_validSelectsAndLoads();
        testDropLoad_rejectBankOrMulti();
        testDropLoad_rejectNonSyx();
        testDropLoad_rejectInvalid();
        testDropLoad_gateCancelRestoresBrowser();
        testDropLoad_gateCancelAfterSamePathReload();
        testDropLoad_folderDropRemembersFolder();
        testDropLoad_zeroValidRejectsWithoutVirtualInstall();
    }

private:
    void testDropLoad_validSelectsAndLoads()
    {
        beginTest("dropLoad_validSelectsAndLoads");

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

        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        copyFixturePatchToDir(dropDir, "Patch 71.syx");
        const auto dropped = dropDir.getChildFile("Patch 71.syx");

        const auto result = harness.handler.loadDroppedComputerPatchFile(dropped, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     dropDir.getFullPathName());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice(
                      FooterMessages::formatReadablePatchLocation(dropped), false)
               || harness.proc.apvts.state.getProperty("uiMessageText").toString()
                      == FooterMessages::formatLoadSuccess(
                             FooterMessages::formatReadablePatchLocation(dropped)));
        expect(scanQueue(harness.queue).editBufferPatch);

        priorDir.deleteRecursively();
        dropDir.deleteRecursively();
    }

    void testDropLoad_rejectBankOrMulti()
    {
        beginTest("dropLoad_rejectBankOrMulti");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);

        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        const auto a = PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx");
        const auto b = PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx");
        juce::MemoryBlock combined;
        expect(a.loadFileAsData(combined));
        juce::MemoryBlock second;
        expect(b.loadFileAsData(second));
        combined.append(second.getData(), second.getSize());
        const auto dropped = dropDir.getChildFile("bankish.syx");
        expect(dropped.replaceWithData(combined.getData(), combined.getSize()));

        const auto folderBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kFolderPath).toString();

        const auto result = harness.handler.loadDroppedComputerPatchFile(dropped, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kRejected);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     folderBefore);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kDropRejectedBankOrMulti));
        expect(harness.queue.isEmpty());

        priorDir.deleteRecursively();
        dropDir.deleteRecursively();
    }

    void testDropLoad_rejectNonSyx()
    {
        beginTest("dropLoad_rejectNonSyx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        const auto dropped = dropDir.getChildFile("notes.txt");
        expect(dropped.replaceWithText("hello"));

        const auto result = harness.handler.loadDroppedComputerPatchFile(dropped, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kRejected);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kDropRejectedNotSyx));
        expect(harness.queue.isEmpty());

        dropDir.deleteRecursively();
    }

    void testDropLoad_rejectInvalid()
    {
        beginTest("dropLoad_rejectInvalid");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());

        const auto valid = PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx");
        juce::MemoryBlock truncated;
        expect(valid.loadFileAsData(truncated));
        expect(truncated.getSize() > 40);
        truncated.setSize(40, true);

        const auto dropped = dropDir.getChildFile("corrupt.syx");
        expect(dropped.replaceWithData(truncated.getData(), truncated.getSize()));

        const auto result = harness.handler.loadDroppedComputerPatchFile(dropped, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kRejected);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kDropRejectedInvalid));
        expect(harness.queue.isEmpty());

        dropDir.deleteRecursively();
    }

    void testDropLoad_gateCancelRestoresBrowser()
    {
        beginTest("dropLoad_gateCancelRestoresBrowser");

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

        const auto folderBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kFolderPath).toString();
        const int selectBefore = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile));

        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        copyFixturePatchToDir(dropDir, "Patch 71.syx");
        harness.gateState->allow = false;

        const auto result = harness.handler.loadDroppedComputerPatchFile(
            dropDir.getChildFile("Patch 71.syx"), harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kCancelled);
        expect(harness.gateState->calls >= 1);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     folderBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     selectBefore);
        // Set Bank may already be queued while pinning the destination; the .syx must not load.
        const auto queued = scanQueue(harness.queue);
        expect(! queued.editBufferPatch);
        expect(! queued.patchData);

        priorDir.deleteRecursively();
        dropDir.deleteRecursively();
    }

    void testDropLoad_gateCancelAfterSamePathReload()
    {
        beginTest("dropLoad_gateCancelAfterSamePathReload");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        copyFixturePatchToDir(dropDir, "Patch 71.syx");
        const auto dropped = dropDir.getChildFile("Patch 71.syx");

        const auto first = harness.handler.loadDroppedComputerPatchFile(dropped, harness.limits);
        expect(first == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        harness.gateState->allow = false;
        const int gateCallsBefore = harness.gateState->calls;

        const auto second = harness.handler.loadDroppedComputerPatchFile(dropped, harness.limits);

        expect(second == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kCancelled);
        expect(harness.gateState->calls > gateCallsBefore);

        dropDir.deleteRecursively();
    }

    void testDropLoad_folderDropRemembersFolder()
    {
        beginTest("dropLoad_folderDropRemembersFolder");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);

        const auto dropDir = createTempScanDir();
        expect(dropDir.createDirectory());
        copyFixturePatchToDir(dropDir, "Patch 71.syx");
        copyFixturePatchToDir(dropDir, "Patch 66.syx");

        juce::StringArray paths;
        paths.add(dropDir.getFullPathName());
        const auto result = harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     dropDir.getFullPathName());
        expect(! harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.patchFileService.getLastScanResult().validCount, 2);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);

        priorDir.deleteRecursively();
        dropDir.deleteRecursively();
    }

    void testDropLoad_zeroValidRejectsWithoutVirtualInstall()
    {
        beginTest("dropLoad_zeroValidRejectsWithoutVirtualInstall");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);
        const auto folderBefore = priorDir.getFullPathName();

        const auto junkDir = createTempScanDir();
        expect(junkDir.createDirectory());
        expect(junkDir.getChildFile("notes.txt").replaceWithText("x"));

        juce::StringArray paths;
        paths.add(junkDir.getChildFile("notes.txt").getFullPathName());
        paths.add(junkDir.getFullPathName());

        const auto result = harness.handler.loadDroppedComputerPatchSelection(paths, harness.limits);

        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kRejected);
        expectEquals(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StateProperties::kFolderPath).toString(),
                     folderBefore);
        expect(! harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kDropRejectedNoValid));

        priorDir.deleteRecursively();
        junkDir.deleteRecursively();
    }

};

static PatchManagerActionHandlerDropLoadTests patchManagerActionHandlerDropLoadTests;
