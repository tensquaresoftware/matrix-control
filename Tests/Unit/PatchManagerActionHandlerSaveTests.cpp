#include "PatchManagerActionHandlerTestSupport.h"

#include "Core/MIDI/SysEx/SysExConstants.h"

#include "PatchFixturePaths.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerSaveTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerSaveTests() : juce::UnitTest("PatchManagerActionHandlerSave") {}

    void runTest() override
    {
        testSaveAs_writesAndRescans();
        testSave_overwritesSelectedFile();
        testSave_selectSentinelNoOp();
        testSaveAs_cancelledNoWrite();
        testSave_noSysEx();
        testSave_unusableFolderNoOp();
        testSaveAs_noSysEx();
        testSaveAs_injectsUppercaseStemIntoNameBytes();
        testSave_injectsSanitizedStemIntoNameBytes();
        testSaveAs_fromComputerOrigin_updatesLivePatchName();
        testSaveAs_illegalStem_noWrite();
        testSaveAs_suggestedStem_isMatrixSanitized();
        testSave_illegalDiskStem_refusesWrite();
    }

private:
    static juce::String loadSavedPatchName(HandlerHarness& harness, const juce::File& file)
    {
        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        if (! harness.patchFileService.loadPatchSysExFile(file, packed).success)
            return {};

        Core::PatchModel decoded;
        decoded.loadFrom(packed);
        return decoded.getName();
    }

    juce::File bindTempComputerPatchesFolder(HandlerHarness& harness)
    {
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath, tempDir.getFullPathName(), nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        return tempDir;
    }

    void testSaveAs_writesAndRescans()
    {
        beginTest("saveAs_writesAndRescans");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        const auto revisionBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kScanRevision);

        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String stem) {
            return tempDir.getChildFile(stem + ".syx");
        };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto savedFile = tempDir.getChildFile("PATCH.syx");
        expect(savedFile.existsAsFile());
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatSaveSuccess(
                      FooterMessages::formatReadablePatchLocation(savedFile)));
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString() == "info");
        expect(harness.proc.apvts.state.hasProperty(ComputerPatches::StateProperties::kScanRevision));
        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kScanRevision)
               != revisionBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            1);

        juce::MemoryBlock savedSysEx;
        expect(savedFile.loadFileAsData(savedSysEx));
        expect(harness.decoder.validatePatchSysExMessage(savedSysEx));

        const auto* data = static_cast<const juce::uint8*>(savedSysEx.getData());
        expectEquals(static_cast<int>(data[3]),
                     static_cast<int>(SysExConstants::Opcode::kSinglePatchToEditBuffer));
        expectEquals(static_cast<int>(data[4]), 0);

        tempDir.deleteRecursively();
    }

    void testSave_overwritesSelectedFile()
    {
        beginTest("save_overwritesSelectedFile");

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
        simulateSelectPatchFileDispatch(harness);

        const auto target = tempDir.getChildFile("Patch 71.syx");
        const auto sizeBefore = target.getSize();

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        const auto written = tempDir.getChildFile("PATCH 71.syx");
        expect(written.existsAsFile());
        // Case-sensitive volumes must not keep "Patch 71.syx" beside "PATCH 71.syx".
        expectEquals(tempDir.getNumberOfChildFiles(juce::File::findFiles, "*.syx"), 1);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatSaveSuccess(
                      FooterMessages::formatReadablePatchLocation(written)));
        expectEquals(static_cast<int>(harness.patchFileService.getLastScanResult().validCount), 1);
        expect(sizeBefore > 0);

        juce::MemoryBlock savedSysEx;
        expect(written.loadFileAsData(savedSysEx));
        const auto* data = static_cast<const juce::uint8*>(savedSysEx.getData());
        expectEquals(static_cast<int>(data[3]),
                     static_cast<int>(SysExConstants::Opcode::kSinglePatchToEditBuffer));
        expectEquals(static_cast<int>(data[4]), 0);
        expectEquals(harness.model.getName(), juce::String("PATCH 71"));
        expectEquals(loadSavedPatchName(harness, written), juce::String("PATCH 71"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("PATCH 71"));

        tempDir.deleteRecursively();
    }

    void testSave_selectSentinelNoOp()
    {
        beginTest("save_selectSentinelNoOp");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile,
            0,
            nullptr);

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expectEquals(tempDir.getNumberOfChildFiles(juce::File::findFiles), 0);
        expect(! harness.proc.apvts.state.getProperty("uiMessageText").toString().startsWith("Saved "));

        tempDir.deleteRecursively();
    }

    void testSaveAs_cancelledNoWrite()
    {
        beginTest("saveAs_cancelledNoWrite");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        const auto revisionBefore = harness.proc.apvts.state.getProperty(
            ComputerPatches::StateProperties::kScanRevision);
        const auto patchNameBefore = harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString();
        const auto selectBefore = static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile));
        harness.pickSaveFileCallback = [](juce::File, juce::String) { return juce::File(); };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        expectEquals(tempDir.getNumberOfChildFiles(juce::File::findFiles), 1);
        expect(harness.proc.apvts.state.getProperty(ComputerPatches::StateProperties::kScanRevision)
               == revisionBefore);
        expect(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString() == patchNameBefore);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile)),
            selectBefore);

        tempDir.deleteRecursively();
    }

    void testSave_noSysEx()
    {
        beginTest("save_noSysEx");

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

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(! queued.patchData);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testSave_unusableFolderNoOp()
    {
        beginTest("save_unusableFolderNoOp");

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

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(! harness.proc.apvts.state.getProperty("uiMessageText").toString().startsWith("Saved "));
    }

    void testSaveAs_noSysEx()
    {
        beginTest("saveAs_noSysEx");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());

        harness.proc.apvts.state.setProperty(
            ComputerPatches::StateProperties::kFolderPath,
            tempDir.getFullPathName(),
            nullptr);
        harness.handler.rescanPersistedComputerPatchesFolder();

        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String stem) {
            return tempDir.getChildFile(stem + ".syx");
        };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(! queued.patchData);
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testSaveAs_injectsUppercaseStemIntoNameBytes()
    {
        beginTest("saveAs_injectsUppercaseStemIntoNameBytes");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        // Device/ROM origin: file gets TEST; live Patch Name stays the ROM name.
        harness.model.setName("*'CANOPY");
        harness.proc.apvts.state.setProperty(PatchNameIds::kPatchName, "*'CANOPY", nullptr);
        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String) {
            return tempDir.getChildFile("test.syx");
        };

        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto savedFile = tempDir.getChildFile("TEST.syx");
        expect(savedFile.existsAsFile());
        expectEquals(harness.model.getName(), juce::String("*'CANOPY"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("*'CANOPY"));
        expectEquals(loadSavedPatchName(harness, savedFile), juce::String("TEST"));
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString()
               == FooterMessages::formatSaveSuccess(
                      FooterMessages::formatReadablePatchLocation(savedFile)));

        tempDir.deleteRecursively();
    }

    void testSave_injectsSanitizedStemIntoNameBytes()
    {
        beginTest("save_injectsSanitizedStemIntoNameBytes");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        const auto warmFile = tempDir.getChildFile("WARM.syx");
        {
            Core::PatchModel seed;
            seed.setName("OLDNAME");
            expect(harness.patchFileService.savePatchSysExFile(
                       warmFile, seed.data(), harness.sysExEncoder)
                       .success);
        }

        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);

        harness.model.setName("*'CANOPY");
        harness.proc.apvts.state.setProperty(PatchNameIds::kPatchName, "*'CANOPY", nullptr);
        harness.patchNameSyncer.apvtsToBuffer();
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expect(warmFile.existsAsFile());
        expectEquals(harness.model.getName(), juce::String("WARM"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("WARM"));
        expectEquals(loadSavedPatchName(harness, warmFile), juce::String("WARM"));

        tempDir.deleteRecursively();
    }

    void testSaveAs_fromComputerOrigin_updatesLivePatchName()
    {
        beginTest("saveAs_fromComputerOrigin_updatesLivePatchName");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        {
            Core::PatchModel seed;
            seed.setName("SEEDNAME");
            expect(harness.patchFileService.savePatchSysExFile(
                       tempDir.getChildFile("SEED.syx"), seed.data(), harness.sysExEncoder)
                       .success);
        }

        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);

        harness.pickSaveFileCallback = [&tempDir](juce::File, juce::String) {
            return tempDir.getChildFile("test.syx");
        };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        const auto savedFile = tempDir.getChildFile("TEST.syx");
        expect(savedFile.existsAsFile());
        expectEquals(harness.model.getName(), juce::String("TEST"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("TEST"));
        expectEquals(loadSavedPatchName(harness, savedFile), juce::String("TEST"));

        tempDir.deleteRecursively();
    }

    void testSaveAs_illegalStem_noWrite()
    {
        beginTest("saveAs_illegalStem_noWrite");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        harness.model.setName("KEEPNAME");
        harness.proc.apvts.state.setProperty(PatchNameIds::kPatchName, "KEEPNAME", nullptr);

        const auto accentedStem = juce::String::fromUTF8("r\xc3\xa9so");
        harness.pickSaveFileCallback = [&tempDir, accentedStem](juce::File, juce::String) {
            return tempDir.getChildFile(accentedStem + ".syx");
        };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        expectEquals(tempDir.getNumberOfChildFiles(juce::File::findFiles), 0);
        expectEquals(harness.model.getName(), juce::String("KEEPNAME"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("KEEPNAME"));
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kInvalidSaveStem));
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String("warning"));

        tempDir.deleteRecursively();
    }

    void testSaveAs_suggestedStem_isMatrixSanitized()
    {
        beginTest("saveAs_suggestedStem_isMatrixSanitized");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        harness.model.setName("*'CANOPY");
        harness.proc.apvts.state.setProperty(PatchNameIds::kPatchName, "*'CANOPY", nullptr);

        juce::String suggestedStemSeen;
        harness.pickSaveFileCallback = [&tempDir, &suggestedStemSeen](juce::File, juce::String stem) {
            suggestedStemSeen = stem;
            return tempDir.getChildFile("CANOPY.syx");
        };
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchAs, juce::var());

        expectEquals(suggestedStemSeen, juce::String("CANOPY"));
        expect(tempDir.getChildFile("CANOPY.syx").existsAsFile());

        tempDir.deleteRecursively();
    }

    void testSave_illegalDiskStem_refusesWrite()
    {
        beginTest("save_illegalDiskStem_refusesWrite");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = bindTempComputerPatchesFolder(harness);

        const auto accentedStem = juce::String::fromUTF8("r\xc3\xa9so");
        const auto oddFile = tempDir.getChildFile(accentedStem + ".syx");
        {
            Core::PatchModel seed;
            seed.setName("OLDNAME");
            expect(harness.patchFileService.savePatchSysExFile(
                       oddFile, seed.data(), harness.sysExEncoder)
                       .success);
        }

        harness.handler.rescanPersistedComputerPatchesFolder();
        harness.proc.apvts.state.setProperty(
            ComputerPatches::StandaloneWidgets::kSelectPatchFile, 1, nullptr);

        harness.model.setName("KEEPNAME");
        harness.proc.apvts.state.setProperty(PatchNameIds::kPatchName, "KEEPNAME", nullptr);
        const auto sizeBefore = oddFile.getSize();
        harness.handler.handleAction(ComputerPatches::StandaloneWidgets::kSavePatchFile, juce::var());

        expectEquals(oddFile.getSize(), sizeBefore);
        expectEquals(harness.model.getName(), juce::String("KEEPNAME"));
        expectEquals(harness.proc.apvts.state.getProperty(PatchNameIds::kPatchName).toString(),
                     juce::String("KEEPNAME"));
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(FooterMessages::kInvalidSaveStem));

        tempDir.deleteRecursively();
    }
};

static PatchManagerActionHandlerSaveTests patchManagerActionHandlerSaveTests;
