#include "PatchManagerActionHandlerTestSupport.h"

#include "Core/Services/PatchFileNameSanitizer.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerBankExportFilenameTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerBankExportFilenameTests()
        : juce::UnitTest("PatchManagerActionHandlerBankExportFilename")
    {
    }

    void runTest() override
    {
        testLoad_bankExportStem_fileNamesUsesStrippedName();
        testLoad_bankExportStemWithArtisticInnerName_fileNamesUsesInner();
        testLoad_artisticHyphenStem_fileNamesKeepsFullName();
        testReapplyComputerDisplay_bankExportStemStripsPrefix();
    }

private:
    void testLoad_bankExportStem_fileNamesUsesStrippedName()
    {
        beginTest("load_bankExportStem_fileNamesUsesStrippedName");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("NYLON 12");
        const auto stem = Core::PatchFileNameSanitizer::bankExportFileStem(10, "NYLON 12");
        expectEquals(stem, juce::String("P10. NYLON 12"));
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile(Core::PatchFileNameSanitizer::ensureSyxExtension(stem)),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kDisplayFileNames,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("NYLON 12"));
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatLoadSuccess(
                      FooterMessages::formatReadablePatchLocation(
                          tempDir.getChildFile(
                              Core::PatchFileNameSanitizer::ensureSyxExtension(stem)))));

        tempDir.deleteRecursively();
    }

    void testLoad_bankExportStemWithArtisticInnerName_fileNamesUsesInner()
    {
        beginTest("load_bankExportStemWithArtisticInnerName_fileNamesUsesInner");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        const auto stem = Core::PatchFileNameSanitizer::bankExportFileStem(5, "P99 - DJ");
        expectEquals(stem, juce::String("P05. P99 - DJ"));
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile(Core::PatchFileNameSanitizer::ensureSyxExtension(stem)),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kDisplayFileNames,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("P99 - DJ"));

        tempDir.deleteRecursively();
    }

    void testLoad_artisticHyphenStem_fileNamesKeepsFullName()
    {
        beginTest("load_artisticHyphenStem_fileNamesKeepsFullName");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("P99 - DJ.syx"),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kDisplayFileNames,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("P99 - DJ"));

        tempDir.deleteRecursively();
    }

    void testReapplyComputerDisplay_bankExportStemStripsPrefix()
    {
        beginTest("reapplyComputerDisplay_bankExportStemStripsPrefix");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        // SysEx payload name intentionally differs from the bank-export filename stem so
        // FILE NAMES reapply must strip "P10. " — a matching SysEx/file pair would pass even
        // if the strip path were broken.
        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        const auto stem = Core::PatchFileNameSanitizer::bankExportFileStem(10, "NYLON 12");
        expectEquals(stem, juce::String("P10. NYLON 12"));
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile(Core::PatchFileNameSanitizer::ensureSyxExtension(stem)),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expectEquals(harness.model.getName(), juce::String("INSIDE"));

        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplayFileNames, nullptr);
        harness.handler.reapplyComputerPatchDisplayedName();
        expectEquals(harness.model.getName(), juce::String("NYLON 12"));

        tempDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerBankExportFilenameTests patchManagerActionHandlerBankExportFilenameTests;
