#include "PatchManagerActionHandlerTestSupport.h"

#include "PatchFixturePaths.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerDropLoadReviewTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerDropLoadReviewTests()
        : juce::UnitTest("PatchManagerActionHandlerDropLoadReview")
    {
    }

    void runTest() override
    {
        testVirtualSaveAsOutsideListClearsSelection();
        testGateCancelRestoresVirtualInvalidCount();
        testVirtualInListSavePreservesInvalidCount();
    }

private:
    void testVirtualSaveAsOutsideListClearsSelection()
    {
        beginTest("dropLoad_virtualSaveAsOutsideListClearsSelection");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        juce::File dirA, dirB;
        const auto listBefore = loadTwoFileVirtualList(harness, dirA, dirB);
        expectEquals(listBefore.size(), 2);
        const auto sizeA = listBefore[0].getSize();
        const auto sizeB = listBefore[1].getSize();

        const auto exported = saveAsOutsideVirtualList(harness);
        expectVirtualListUnchanged(harness, listBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     0);

        expect(exported.replaceWithData("x", 1));
        expectEquals(exported.getSize(), (juce::int64) 1);
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(exported.getSize() > 1);
        expectEquals(listBefore[0].getSize(), sizeA);
        expectEquals(listBefore[1].getSize(), sizeB);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     0);

        dirA.deleteRecursively();
        dirB.deleteRecursively();
        exported.getParentDirectory().deleteRecursively();
    }

    void testGateCancelRestoresVirtualInvalidCount()
    {
        beginTest("dropLoad_gateCancelRestoresVirtualInvalidCount");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        const auto priorDir = createTempScanDir();
        expect(priorDir.createDirectory());
        copyFixturePatchToDir(priorDir, "Patch 5.syx");
        setupComputerPatchesScan(harness, priorDir);

        juce::File mixDir;
        const int invalidBefore = installMixVirtualListWithInvalid(harness, mixDir);
        expect(invalidBefore >= 1);

        const auto nextDropDir = createTempScanDir();
        expect(nextDropDir.createDirectory());
        copyFixturePatchToDir(nextDropDir, "Patch 808.syx");
        harness.gateState->allow = false;
        expect(harness.handler.loadDroppedComputerPatchFile(
                   nextDropDir.getChildFile("Patch 808.syx"), harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kCancelled);

        const auto& after = harness.patchFileService.getLastScanResult();
        expect(after.isVirtualList());
        expectEquals(after.invalidCount, invalidBefore);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     FooterMessages::formatScanSummary(after.validCount, after.invalidCount));

        priorDir.deleteRecursively();
        mixDir.deleteRecursively();
        nextDropDir.deleteRecursively();
    }

    void testVirtualInListSavePreservesInvalidCount()
    {
        beginTest("dropLoad_virtualInListSavePreservesInvalidCount");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);

        juce::File mixDir;
        const int invalidBefore = installMixVirtualListWithInvalid(harness, mixDir);
        expect(invalidBefore >= 1);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
                     1);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        const auto& after = harness.patchFileService.getLastScanResult();
        expect(after.isVirtualList());
        expectEquals(after.invalidCount, invalidBefore);
        expect(after.validCount >= 1);

        mixDir.deleteRecursively();
    }

    juce::File saveAsOutsideVirtualList(HandlerHarness& harness)
    {
        const auto exportDir = createTempScanDir();
        expect(exportDir.createDirectory());
        harness.pickSaveFileCallback = [&exportDir](juce::File, juce::String stem) {
            return exportDir.getChildFile(stem + ".syx");
        };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto exportedFiles = exportDir.findChildFiles(juce::File::findFiles, false, "*.syx");
        expectEquals(exportedFiles.size(), 1);
        const auto exported = exportedFiles[0];
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     FooterMessages::formatSaveSuccess(
                         FooterMessages::formatReadablePatchLocation(exported)));
        return exported;
    }

    void expectVirtualListUnchanged(HandlerHarness& harness, const juce::Array<juce::File>& listBefore)
    {
        expect(harness.patchFileService.getLastScanResult().isVirtualList());
        expectEquals(harness.patchFileService.getLastScanResult().sortedValidFiles.size(),
                     listBefore.size());
        for (int i = 0; i < listBefore.size(); ++i)
            expectEquals(listBefore[i].getFullPathName(),
                         harness.patchFileService.getLastScanResult().sortedValidFiles[i].getFullPathName());
    }

    int installMixVirtualListWithInvalid(HandlerHarness& harness, juce::File& outMixDir)
    {
        outMixDir = createTempScanDir();
        expect(outMixDir.createDirectory());
        copyFixturePatchToDir(outMixDir, "Patch 71.syx");
        expect(outMixDir.getChildFile("notes.txt").replaceWithText("not a patch"));

        juce::StringArray mixPaths;
        mixPaths.add(outMixDir.getChildFile("Patch 71.syx").getFullPathName());
        mixPaths.add(outMixDir.getChildFile("notes.txt").getFullPathName());
        expect(harness.handler.loadDroppedComputerPatchSelection(mixPaths, harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);
        return harness.patchFileService.getLastScanResult().invalidCount;
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
};

static PatchManagerActionHandlerDropLoadReviewTests patchManagerActionHandlerDropLoadReviewTests;
