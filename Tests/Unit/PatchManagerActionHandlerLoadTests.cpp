#include "PatchManagerActionHandlerTestSupport.h"

#include "Core/Services/Matrix1000FactoryPatchNames.h"
#include "Core/Services/PatchFileNameSanitizer.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerLoadTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerLoadTests() : juce::UnitTest("PatchManagerActionHandlerLoad") {}

    void runTest() override
    {
        testLoadSelected_enqueuesSysEx();
        testLoadSelected_matrix6_sendsPatchSlot();
        testLoadSelected_sentinelNoOp();
        testLoadSelected_staleScanNoOp();
        testLoadSelected_outOfRangeWarning();
        testLoadSelected_invalidFileWarning();
        testLoad_askOnceCancelRestoresModel();
        testLoad_mismatch_preferInternalFooter();
        testLoad_mismatch_preferFilenameFooter();
        testReapplyComputerDisplay_togglesSysexAndFileNames();
        testReapplyMatrixDisplay_noDumpCache_togglesRomFactoryAndBnk();
    }

private:
    void testLoadSelected_enqueuesSysEx()
    {
        beginTest("loadSelected_enqueuesSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(queued.editBufferPatch);
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "info");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice(
                      FooterMessages::formatReadablePatchLocation(
                          tempDir.getChildFile("Patch 71.syx")),
                      false));

        tempDir.deleteRecursively();
    }

    void testLoadSelected_matrix6_sendsPatchSlot()
    {
        beginTest("loadSelected_matrix6_sendsPatchSlot");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6));
        initializePatchManagerState(harness.proc.apvts.state, 0, 12, false);
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        const auto queued = scanQueue(harness.queue);
        expect(queued.patchData);
        expect(!queued.editBufferPatch);
        expectEquals(queued.patchNumber, 12);

        tempDir.deleteRecursively();
    }

    void testLoadSelected_sentinelNoOp()
    {
        beginTest("loadSelected_sentinelNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        expect(harness.queue.isEmpty());
        expect(! harness.proc.apvts.state.getProperty("uiMessageText").toString().startsWith("Loaded "));

        tempDir.deleteRecursively();
    }

    void testLoadSelected_staleScanNoOp()
    {
        beginTest("loadSelected_staleScanNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        expect(tempDir.deleteRecursively());

        simulateSelectPatchFileDispatch(harness);

        expect(harness.queue.isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::kLoadSelectionStale);

        tempDir.deleteRecursively();
    }

    void testLoadSelected_outOfRangeWarning()
    {
        beginTest("loadSelected_outOfRangeWarning");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            99,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        expect(harness.queue.isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::kLoadSelectionStale);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            0);

        tempDir.deleteRecursively();
    }

    void testLoadSelected_invalidFileWarning()
    {
        beginTest("loadSelected_invalidFileWarning");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        const auto patchNameBefore = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();
        const juce::uint8 garbage[] = { 0x00, 0x01, 0x02, 0x03 };
        expect(tempDir.getChildFile("Patch 71.syx").replaceWithData(garbage, sizeof(garbage)));

        simulateSelectPatchFileDispatch(harness);

        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "warning");
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == patchNameBefore);
        expect(harness.queue.isEmpty());
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            0);

        tempDir.deleteRecursively();
    }

    void testLoad_askOnceCancelRestoresModel()
    {
        beginTest("load_askOnceCancelRestoresModel");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.mapper.apvtsToBuffer();
        harness.model.setName("KEEP-ME");
        harness.mapper.bufferToApvts();
        const auto patchNameBefore = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kAskOncePerLoad,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        harness.pickReconciliationCallback =
            [](juce::String, juce::String) -> std::optional<Core::NameReconciliationChoice>
            {
                return std::nullopt;
            };

        simulateSelectPatchFileDispatch(harness);

        expect(harness.model.getName() == "KEEP-ME");
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == patchNameBefore);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testLoad_mismatch_preferInternalFooter()
    {
        beginTest("load_mismatch_preferInternalFooter");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("OUTSIDE.syx"),
            exportModel.data(),
            harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            Policy::kDisplaySysexNames,
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            1,
            nullptr);

        simulateSelectPatchFileDispatch(harness);

        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice(
                      FooterMessages::formatReadablePatchLocation(
                          tempDir.getChildFile("OUTSIDE.syx")),
                      false));
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == "INSIDE");

        tempDir.deleteRecursively();
    }

    void testLoad_mismatch_preferFilenameFooter()
    {
        beginTest("load_mismatch_preferFilenameFooter");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("OUTSIDE.syx"),
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

        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatReconciliationNotice(
                      FooterMessages::formatReadablePatchLocation(
                          tempDir.getChildFile("OUTSIDE.syx")),
                      true));
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == "OUTSIDE");

        tempDir.deleteRecursively();
    }

    void testReapplyComputerDisplay_togglesSysexAndFileNames()
    {
        beginTest("reapplyComputerDisplay_togglesSysexAndFileNames");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        Core::PatchModel exportModel;
        exportModel.loadFrom(Core::InitDefaults::patchData());
        exportModel.setName("INSIDE");
        expect(harness.patchFileService.savePatchSysExFile(
            tempDir.getChildFile("OUTSIDE.syx"), exportModel.data(), harness.sysExEncoder).success);

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("INSIDE"));

        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplayFileNames, nullptr);
        harness.handler.reapplyComputerPatchDisplayedName();
        expectEquals(harness.model.getName(), juce::String("OUTSIDE"));

        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy, Policy::kDisplaySysexNames, nullptr);
        harness.handler.reapplyComputerPatchDisplayedName();
        expectEquals(harness.model.getName(), juce::String("INSIDE"));

        tempDir.deleteRecursively();
    }

    void testReapplyMatrixDisplay_noDumpCache_togglesRomFactoryAndBnk()
    {
        beginTest("reapplyMatrixDisplay_noDumpCache_togglesRomFactoryAndBnk");

        constexpr int kRomBank = 2;
        constexpr int kPatch = 5;
        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, kRomBank, kPatch, false);

        const auto factoryName = Core::Matrix1000FactoryPatchNames::nameFor(kRomBank, kPatch);
        expect(factoryName.isNotEmpty());

        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kMatrix1000PatchesNamesMode,
            PluginIDs::Settings::Matrix1000PatchesNamesMode::kDisplayMusicalNames,
            nullptr);
        harness.handler.reapplyDisplayedPatchName();
        expectEquals(harness.model.getName(), factoryName);

        harness.proc.apvts.state.setProperty(
            PluginIDs::Settings::kMatrix1000PatchesNamesMode,
            PluginIDs::Settings::Matrix1000PatchesNamesMode::kDisplayHardwareNames,
            nullptr);
        harness.handler.reapplyDisplayedPatchName();
        expectEquals(harness.model.getName(),
                     Core::PatchFileNameSanitizer::formatOberheimBankPlaceholderName(kRomBank, kPatch));
    }
};

static PatchManagerActionHandlerLoadTests patchManagerActionHandlerLoadTests;
