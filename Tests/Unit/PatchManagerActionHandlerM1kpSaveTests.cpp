#include "PatchManagerActionHandlerTestSupport.h"

#include "PatchFixturePaths.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerM1kpSaveTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerM1kpSaveTests()
        : juce::UnitTest("PatchManagerActionHandlerM1kpSave")
    {
    }

    void runTest() override
    {
        testSave_m1kpWithoutSibling_writesSyxWithoutGate();
        testSave_m1kpWithSiblingSyx_cancelLeavesSyxUntouched();
        testSave_m1kpWithSiblingSyx_continueOverwritesSyx();
    }

private:
    void testSave_m1kpWithoutSibling_writesSyxWithoutGate()
    {
        beginTest("save_m1kpWithoutSibling_writesSyxWithoutGate");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("NICEPAD.m1kp")));

        const auto result = harness.handler.loadDroppedComputerPatchFile(
            tempDir.getChildFile("NICEPAD.m1kp"), harness.limits);
        expect(result == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        bool gateCalled = false;
        harness.handler.setM1kpSiblingSyxOverwriteConfirmGate(
            [&gateCalled](const juce::String&) {
                gateCalled = true;
                return false;
            });

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(! gateCalled);
        expect(tempDir.getChildFile("NICEPAD.syx").existsAsFile());
        expect(tempDir.getChildFile("NICEPAD.m1kp").existsAsFile());

        tempDir.deleteRecursively();
    }

    void testSave_m1kpWithSiblingSyx_cancelLeavesSyxUntouched()
    {
        beginTest("save_m1kpWithSiblingSyx_cancelLeavesSyxUntouched");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("NICEPAD.m1kp")));
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx")
                   .copyFileTo(tempDir.getChildFile("NICEPAD.syx")));

        juce::MemoryBlock syxBefore;
        expect(tempDir.getChildFile("NICEPAD.syx").loadFileAsData(syxBefore));

        expect(harness.handler.loadDroppedComputerPatchFile(
                   tempDir.getChildFile("NICEPAD.m1kp"), harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        juce::String promptedName;
        harness.handler.setM1kpSiblingSyxOverwriteConfirmGate(
            [&promptedName](const juce::String& existingSyxFileName) {
                promptedName = existingSyxFileName;
                return false;
            });

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expectEquals(promptedName, juce::String("NICEPAD.syx"));

        juce::MemoryBlock syxAfter;
        expect(tempDir.getChildFile("NICEPAD.syx").loadFileAsData(syxAfter));
        expect(syxAfter == syxBefore);
        expect(tempDir.getChildFile("NICEPAD.m1kp").existsAsFile());

        tempDir.deleteRecursively();
    }

    void testSave_m1kpWithSiblingSyx_continueOverwritesSyx()
    {
        beginTest("save_m1kpWithSiblingSyx_continueOverwritesSyx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("NICEPAD.m1kp")));
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx")
                   .copyFileTo(tempDir.getChildFile("NICEPAD.syx")));

        juce::MemoryBlock syxBefore;
        expect(tempDir.getChildFile("NICEPAD.syx").loadFileAsData(syxBefore));

        expect(harness.handler.loadDroppedComputerPatchFile(
                   tempDir.getChildFile("NICEPAD.m1kp"), harness.limits)
               == Core::PatchManagerActionHandler::DroppedComputerPatchLoadResult::kLoaded);

        harness.handler.setM1kpSiblingSyxOverwriteConfirmGate(
            [](const juce::String& existingSyxFileName) {
                return existingSyxFileName == "NICEPAD.syx";
            });

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        juce::MemoryBlock syxAfter;
        expect(tempDir.getChildFile("NICEPAD.syx").loadFileAsData(syxAfter));
        expect(syxAfter != syxBefore);
        expect(tempDir.getChildFile("NICEPAD.m1kp").existsAsFile());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "info");

        tempDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerM1kpSaveTests patchManagerActionHandlerM1kpSaveTests;
