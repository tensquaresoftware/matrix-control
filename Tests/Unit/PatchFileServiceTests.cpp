#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

namespace
{
    juce::File fixturesPatchesDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Patches");
    }

    juce::File fixturesMastersDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Masters");
    }
}

class PatchFileServiceTests : public juce::UnitTest
{
public:
    PatchFileServiceTests()
        : juce::UnitTest("PatchFileService")
        , decoder_(parser_)
        , service_(decoder_)
    {
    }

    void runTest() override
    {
        scan_committedPatchFixtures_allValid();
        scan_rejectsMasterDump();
        scan_mixedValidInvalid();
        scan_emptyFolder();
        scan_nonexistentFolder();
        scan_sortOrder();
        scan_readFailure_countsInvalid();
        scan_sortOrder_patchFixtures();
        scan_uppercaseSyxExtension_countsValid();
        savePatchSysExFile_validRoundTrip();
        savePatchSysExFile_writeFailureHandled();
        loadPatchSysExFile_validFixture();
        loadPatchSysExFile_invalid();

        exportMutatorHistory_emptyStore_fails();
        exportMutatorHistory_writesInitialAndRoot();
        exportMutatorHistory_writesRetries();
        exportMutatorHistory_gapIndices();
        exportMutatorHistory_nameBytesMatch();
        exportMutatorHistory_initialKeepsOriginalName();
        exportMutatorHistory_nonWritableFolder_fails();
        exportMutatorHistory_roundTripValidates();

        exportSession_createsFolderWithLayout();
        resolveKeepSessionFolder_indexesSuffix();
        exportSession_overwriteClearsExisting();
    }

private:
    SysExParser parser_;
    SysExDecoder decoder_;
    SysExEncoder encoder_;
    Core::PatchFileService service_;

    juce::File createTempScanDir()
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                       .getNonexistentChildFile("MatrixControlPatchFileService", "", false);
        expect(dir.createDirectory(), "Temp scan dir should be created");
        return dir;
    }

    static Core::PatchModel makeDistinctBuffer(int seed)
    {
        Core::PatchModel model;
        const auto marker = static_cast<juce::uint8>(seed & 0xFF);
        std::memset(model.data(), marker, Core::PatchModel::kBufferSize);
        model.data()[8] = marker;
        return model;
    }

    static Core::PatchModel makeParentBuffer(int seed)
    {
        Core::PatchModel model;
        const auto marker = static_cast<juce::uint8>((seed + 100) & 0xFF);
        std::memset(model.data(), marker, Core::PatchModel::kBufferSize);
        model.data()[9] = marker;
        return model;
    }

    static Core::PatchModel namedResult(int rootIndex, int retryIndex, int seed)
    {
        auto model = makeDistinctBuffer(seed);
        Core::MutationNaming::applyPatchName(model, rootIndex, retryIndex);
        return model;
    }

    juce::String decodedPatchName(const juce::File& file)
    {
        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        expect(service_.loadPatchSysExFile(file, packed).success);
        Core::PatchModel model;
        model.loadFrom(packed);
        return model.getName();
    }

    void copyFixturePatchToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = fixturesPatchesDir().getChildFile(fileName);
        expect(source.existsAsFile(), fileName + " patch fixture should exist");
        expect(source.copyFileTo(dir.getChildFile(fileName)), "Patch fixture copy should succeed");
    }

    void copyFixtureMasterToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = fixturesMastersDir().getChildFile(fileName);
        expect(source.existsAsFile(), fileName + " master fixture should exist");
        expect(source.copyFileTo(dir.getChildFile(fileName)), "Master fixture copy should succeed");
    }

    void scan_committedPatchFixtures_allValid()
    {
        beginTest("All committed patch fixtures validate");

        const auto tempDir = createTempScanDir();
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        copyFixturePatchToDir(tempDir, "Patch 808.syx");

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 4);
        expectEquals(result.invalidCount, 0);
        expectEquals(result.sortedValidFileNames.size(), 4);
        expectEquals(result.footerMessage, FooterMessages::formatScanSummary(4, 0));

        tempDir.deleteRecursively();
    }

    void scan_rejectsMasterDump()
    {
        beginTest("Master dump is rejected");

        const auto tempDir = createTempScanDir();
        copyFixtureMasterToDir(tempDir, "Master 1.syx");

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 0);
        expectEquals(result.invalidCount, 1);
        expectEquals(result.footerMessage, FooterMessages::formatScanSummary(0, 1));

        tempDir.deleteRecursively();
    }

    void scan_mixedValidInvalid()
    {
        beginTest("Mixed valid patch and garbage file");

        const auto tempDir = createTempScanDir();
        copyFixturePatchToDir(tempDir, "Patch 71.syx");

        const juce::uint8 garbage[] = { 0x00, 0x01, 0x02, 0x03 };
        expect(tempDir.getChildFile("garbage.syx").replaceWithData(garbage, sizeof(garbage)));

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 1);
        expectEquals(result.invalidCount, 1);
        expectEquals(result.footerMessage, FooterMessages::formatScanSummary(1, 1));

        tempDir.deleteRecursively();
    }

    void scan_emptyFolder()
    {
        beginTest("Empty folder reports zero files");

        const auto tempDir = createTempScanDir();

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 0);
        expectEquals(result.invalidCount, 0);
        expectEquals(result.footerMessage, juce::String(FooterMessages::kEmptyFolder));

        tempDir.deleteRecursively();
    }

    void scan_nonexistentFolder()
    {
        beginTest("Nonexistent folder is unusable");

        const auto missing = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("MatrixControlMissingPatchFolder");
        const auto result = service_.scanFolder(missing);

        expect(! result.folderUsable);
        expectEquals(result.footerMessage, juce::String(FooterMessages::kFolderNotFound));
        expectEquals(result.footerSeverity, juce::String("warning"));
    }

    void scan_sortOrder()
    {
        beginTest("Valid filenames are sorted lexicographically");

        const auto tempDir = createTempScanDir();
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        expect(fixturesPatchesDir().getChildFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("Z.syx")));
        expect(fixturesPatchesDir().getChildFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("A.syx")));
        expect(fixturesPatchesDir().getChildFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("M.syx")));

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.validCount, 4);
        expectEquals(result.sortedValidFileNames[0], juce::String("A.syx"));
        expectEquals(result.sortedValidFileNames[1], juce::String("M.syx"));
        expectEquals(result.sortedValidFileNames[2], juce::String("Patch 71.syx"));
        expectEquals(result.sortedValidFileNames[3], juce::String("Z.syx"));

        tempDir.deleteRecursively();
    }

    void scan_readFailure_countsInvalid()
    {
        beginTest("Zero-byte syx file counts as invalid");

        const auto tempDir = createTempScanDir();
        expect(tempDir.getChildFile("empty.syx").create());

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 0);
        expectEquals(result.invalidCount, 1);

        tempDir.deleteRecursively();
    }

    void scan_sortOrder_patchFixtures()
    {
        beginTest("Patch fixture filenames sort for Story 4.6 order");

        const auto tempDir = createTempScanDir();
        copyFixturePatchToDir(tempDir, "Patch 808.syx");
        copyFixturePatchToDir(tempDir, "Patch 5.syx");
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        copyFixturePatchToDir(tempDir, "Patch 66.syx");

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.sortedValidFileNames[0], juce::String("Patch 5.syx"));
        expectEquals(result.sortedValidFileNames[1], juce::String("Patch 66.syx"));
        expectEquals(result.sortedValidFileNames[2], juce::String("Patch 71.syx"));
        expectEquals(result.sortedValidFileNames[3], juce::String("Patch 808.syx"));

        tempDir.deleteRecursively();
    }

    void scan_uppercaseSyxExtension_countsValid()
    {
        beginTest("Uppercase .SYX extension is discovered");

        const auto tempDir = createTempScanDir();
        expect(fixturesPatchesDir().getChildFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("Patch 71.SYX")));

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 1);
        expectEquals(result.invalidCount, 0);
        expectEquals(result.sortedValidFileNames[0], juce::String("Patch 71.SYX"));

        tempDir.deleteRecursively();
    }

    void savePatchSysExFile_validRoundTrip()
    {
        beginTest("savePatchSysExFile_validRoundTrip");

        const auto tempDir = createTempScanDir();
        const auto target = tempDir.getChildFile("SavedPatch.syx");

        const auto result = service_.savePatchSysExFile(
            target,
            Core::InitDefaults::patchData(),
            encoder_);

        expect(result.success);
        expect(target.existsAsFile());
        expectEquals(service_.scanFolder(tempDir).validCount, 1);

        tempDir.deleteRecursively();
    }

    void savePatchSysExFile_writeFailureHandled()
    {
        beginTest("savePatchSysExFile_writeFailureHandled");

        const auto missingParent = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                       .getChildFile("MatrixControlMissingParent")
                                       .getChildFile("nested/SavedPatch.syx");

        const auto result = service_.savePatchSysExFile(
            missingParent,
            Core::InitDefaults::patchData(),
            encoder_);

        expect(! result.success);
        expect(result.errorMessage.isNotEmpty());
    }

    void loadPatchSysExFile_validFixture()
    {
        beginTest("loadPatchSysExFile_validFixture");

        const auto source = fixturesPatchesDir().getChildFile("Patch 71.syx");
        expect(source.existsAsFile());

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        const auto result = service_.loadPatchSysExFile(source, packed);

        expect(result.success);

        bool hasNonZero = false;
        for (auto byte : packed)
        {
            if (byte != 0)
            {
                hasNonZero = true;
                break;
            }
        }

        expect(hasNonZero);
    }

    void loadPatchSysExFile_invalid()
    {
        beginTest("loadPatchSysExFile_invalid");

        const auto source = fixturesMastersDir().getChildFile("Master 1.syx");
        expect(source.existsAsFile());

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        const auto result = service_.loadPatchSysExFile(source, packed);

        expect(! result.success);
        expect(result.errorMessage.isNotEmpty());
    }

    void exportMutatorHistory_emptyStore_fails()
    {
        beginTest("exportMutatorHistory_emptyStore_fails");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        const auto result = service_.exportMutatorHistory(tempDir, store, encoder_);

        expect(! result.success);
        expect(result.errorMessage.isNotEmpty());

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_writesInitialAndRoot()
    {
        beginTest("exportMutatorHistory_writesInitialAndRoot");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        auto initial = makeDistinctBuffer(301);
        initial.setName("MY PATCH");
        store.setInitialSnapshot(initial);
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 302),
                                makeParentBuffer(302)));

        const auto result = service_.exportMutatorHistory(tempDir, store, encoder_);

        expect(result.success);
        expect(result.filesWritten >= 2);
        expect(tempDir.getChildFile("Initial.syx").existsAsFile());
        expect(tempDir.getChildFile("M00").isDirectory());
        expect(tempDir.getChildFile("M00").getChildFile("M00.syx").existsAsFile());

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_writesRetries()
    {
        beginTest("exportMutatorHistory_writesRetries");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 310),
                                makeParentBuffer(310)));
        expect(store.insertRetry(0, 0, namedResult(0, 0, 311), makeParentBuffer(311)));
        expect(store.insertRetry(0, 1, namedResult(0, 1, 312), makeParentBuffer(312)));

        const auto result = service_.exportMutatorHistory(tempDir, store, encoder_);

        expect(result.success);
        expect(tempDir.getChildFile("M00").getChildFile("M00-R00.syx").existsAsFile());
        expect(tempDir.getChildFile("M00").getChildFile("M00-R01.syx").existsAsFile());

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_gapIndices()
    {
        beginTest("exportMutatorHistory_gapIndices");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 320),
                                makeParentBuffer(320)));
        expect(store.insertRoot(5, namedResult(5, Core::MutationHistoryStore::kRootOnly, 325),
                                makeParentBuffer(325)));
        expect(store.insertRoot(99, namedResult(99, Core::MutationHistoryStore::kRootOnly, 399),
                                makeParentBuffer(399)));

        const auto result = service_.exportMutatorHistory(tempDir, store, encoder_);

        expect(result.success);
        expect(tempDir.getChildFile("M00").isDirectory());
        expect(tempDir.getChildFile("M05").isDirectory());
        expect(tempDir.getChildFile("M99").isDirectory());
        expect(! tempDir.getChildFile("M01").exists());
        expect(! tempDir.getChildFile("M02").exists());

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_nameBytesMatch()
    {
        beginTest("exportMutatorHistory_nameBytesMatch");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        expect(store.insertRoot(5, namedResult(5, Core::MutationHistoryStore::kRootOnly, 335),
                                makeParentBuffer(335)));
        expect(store.insertRetry(5, 2, namedResult(5, 2, 352), makeParentBuffer(352)));

        const auto result = service_.exportMutatorHistory(tempDir, store, encoder_);
        expect(result.success);

        expectEquals(decodedPatchName(tempDir.getChildFile("M05").getChildFile("M05.syx")),
                     Core::MutationNaming::formatPatchName(5));
        expectEquals(decodedPatchName(tempDir.getChildFile("M05").getChildFile("M05-R02.syx")),
                     Core::MutationNaming::formatPatchName(5, 2));

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_initialKeepsOriginalName()
    {
        beginTest("exportMutatorHistory_initialKeepsOriginalName");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        auto initial = makeDistinctBuffer(340);
        initial.setName("MY PATCH");
        store.setInitialSnapshot(initial);
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 341),
                                makeParentBuffer(341)));

        const auto result = service_.exportMutatorHistory(tempDir, store, encoder_);
        expect(result.success);

        expectEquals(decodedPatchName(tempDir.getChildFile("Initial.syx")), juce::String("MY PATCH"));

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_nonWritableFolder_fails()
    {
        beginTest("exportMutatorHistory_nonWritableFolder_fails");

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 350),
                                makeParentBuffer(350)));

        const auto missing = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("MatrixControlMissingExportFolder");

        const auto result = service_.exportMutatorHistory(missing, store, encoder_);

        expect(! result.success);
        expect(result.errorMessage.isNotEmpty());
    }

    void exportSession_createsFolderWithLayout()
    {
        beginTest("exportSession_createsFolderWithLayout");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;
        auto initial = makeDistinctBuffer(401);
        initial.setName("MY PATCH");
        store.setInitialSnapshot(initial);
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 402),
                                makeParentBuffer(402)));

        const auto sessionFolder = tempDir.getChildFile("B08-P25-OB-VOX");
        const auto result = service_.exportMutatorHistorySession(sessionFolder, store, encoder_, false);

        expect(result.success);
        expect(sessionFolder.getChildFile("Initial.syx").existsAsFile());
        expect(sessionFolder.getChildFile("M00").getChildFile("M00.syx").existsAsFile());

        tempDir.deleteRecursively();
    }

    void resolveKeepSessionFolder_indexesSuffix()
    {
        beginTest("resolveKeepSessionFolder_indexesSuffix");

        const auto tempDir = createTempScanDir();
        const juce::String basename = "B03-P38-OB-VOX";

        expectEquals(Core::PatchFileService::resolveKeepSessionFolder(tempDir, basename).getFileName(),
                     basename);

        expect(tempDir.getChildFile(basename).createDirectory());
        expectEquals(Core::PatchFileService::resolveKeepSessionFolder(tempDir, basename).getFileName(),
                     basename + "-2");

        expect(tempDir.getChildFile(basename + "-2").createDirectory());
        expectEquals(Core::PatchFileService::resolveKeepSessionFolder(tempDir, basename).getFileName(),
                     basename + "-3");

        tempDir.deleteRecursively();
    }

    void exportSession_overwriteClearsExisting()
    {
        beginTest("exportSession_overwriteClearsExisting");

        const auto tempDir = createTempScanDir();
        const auto sessionFolder = tempDir.getChildFile("B00-P00");
        expect(sessionFolder.createDirectory());
        const auto stale = sessionFolder.getChildFile("stale.txt");
        expect(stale.replaceWithText("stale"));

        Core::MutationHistoryStore store;
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 410),
                                makeParentBuffer(410)));

        const auto result = service_.exportMutatorHistorySession(sessionFolder, store, encoder_, true);

        expect(result.success);
        expect(! stale.existsAsFile());
        expect(sessionFolder.getChildFile("M00").getChildFile("M00.syx").existsAsFile());

        tempDir.deleteRecursively();
    }

    void exportMutatorHistory_roundTripValidates()
    {
        beginTest("exportMutatorHistory_roundTripValidates");

        const auto tempDir = createTempScanDir();
        Core::MutationHistoryStore store;

        auto initial = makeDistinctBuffer(360);
        initial.setName("INITNAME");
        store.setInitialSnapshot(initial);
        expect(store.insertRoot(0, namedResult(0, Core::MutationHistoryStore::kRootOnly, 361),
                                makeParentBuffer(361)));
        expect(store.insertRetry(0, 0, namedResult(0, 0, 362), makeParentBuffer(362)));

        const auto exportResult = service_.exportMutatorHistory(tempDir, store, encoder_);
        expect(exportResult.success);
        expectEquals(exportResult.filesWritten, 3);

        const auto rootScan = service_.scanFolder(tempDir);
        expect(rootScan.folderUsable);
        expectEquals(rootScan.validCount, 1);
        expectEquals(rootScan.invalidCount, 0);

        const auto m00Scan = service_.scanFolder(tempDir.getChildFile("M00"));
        expect(m00Scan.folderUsable);
        expectEquals(m00Scan.validCount, 2);
        expectEquals(m00Scan.invalidCount, 0);

        tempDir.deleteRecursively();
    }
};

static PatchFileServiceTests patchFileServiceTests;
