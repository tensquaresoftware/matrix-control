#include "PatchManagerActionHandlerTestSupport.h"

#include "Core/Services/PatchFileNameSanitizer.h"
#include "PatchFixturePaths.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerM1kpNameTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerM1kpNameTests()
        : juce::UnitTest("PatchManagerActionHandlerM1kpName")
    {
    }

    void runTest() override
    {
        testLoad_m1kp_forcesFilenameUnderSysexPolicy();
        testLoad_m1kp_askOnceSkipsPicker();
        testLoad_m1kp_bankExportLikeStemKeepsFullSanitize();
        testLoad_syx_keepsSysexUnderDefaultPolicy();
        testReapplyComputerDisplay_m1kpStaysFilenameUnderSysex();
    }

private:
    void testLoad_m1kp_forcesFilenameUnderSysexPolicy()
    {
        beginTest("load_m1kp_forcesFilenameUnderSysexPolicy");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("CleanBss.m1kp")));

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);

        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.model.getName(), juce::String("CLEANBSS"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("CLEANBSS"));

        tempDir.deleteRecursively();
    }

    void testLoad_m1kp_askOnceSkipsPicker()
    {
        beginTest("load_m1kp_askOnceSkipsPicker");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("Rich Pad.m1kp")));

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kAskOncePerLoad, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);

        int pickerCalls = 0;
        harness.pickReconciliationCallback =
            [&pickerCalls](juce::String, juce::String) -> std::optional<Core::NameReconciliationChoice>
            {
                ++pickerCalls;
                return Core::NameReconciliationChoice::kInternal;
            };

        simulateSelectPatchFileDispatch(harness);

        expectEquals(pickerCalls, 0);
        expectEquals(harness.model.getName(), juce::String("RICH PAD"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("RICH PAD"));

        tempDir.deleteRecursively();
    }

    void testLoad_m1kp_bankExportLikeStemKeepsFullSanitize()
    {
        beginTest("load_m1kp_bankExportLikeStemKeepsFullSanitize");

        // Frozen intent: .m1kp uses getFileNameWithoutExtension only — do not strip "Pxx. ".
        const auto expected = Core::PatchFileNameSanitizer::sanitizeFileStem("P10. Nylon");
        expectEquals(expected, juce::String("P10 NYLO"));
        expect(expected != juce::String("NYLON"));

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("P10. Nylon.m1kp")));

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);

        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.model.getName(), expected);
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     expected);

        tempDir.deleteRecursively();
    }

    void testLoad_syx_keepsSysexUnderDefaultPolicy()
    {
        beginTest("load_syx_keepsSysexUnderDefaultPolicy");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("CleanBss.syx"), exportModel.data(), harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);

        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.model.getName(), juce::String("INSIDE"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("INSIDE"));

        tempDir.deleteRecursively();
    }

    void testReapplyComputerDisplay_m1kpStaysFilenameUnderSysex()
    {
        beginTest("reapplyComputerDisplay_m1kpStaysFilenameUnderSysex");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("CleanBss.m1kp")));

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplayFileNames, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expectEquals(harness.model.getName(), juce::String("CLEANBSS"));

        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.reapplyComputerPatchDisplayedName();
        expectEquals(harness.model.getName(), juce::String("CLEANBSS"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("CLEANBSS"));

        tempDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerM1kpNameTests patchManagerActionHandlerM1kpNameTests;
