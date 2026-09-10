#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "PatchFixturePaths.h"
#include "PatchFileServiceTestSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

namespace
{
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
        scan_sortOrder_mutatorExportNames();
        scan_readFailure_countsInvalid();
        scan_sortOrder_patchFixtures();
        scan_uppercaseSyxExtension_countsValid();
        savePatchSysExFile_validRoundTrip();
        savePatchSysExFile_bankExportSlotIgnoredInHeader();
        savePatchSysExFile_bankExportStemWritesEditBufferHeader();
        savePatchSysExFile_writeFailureHandled();
        loadPatchSysExFile_validFixture();
        loadPatchSysExFile_thirdPartyOpcode01StillLoadable();
        loadPatchSysExFile_invalid();
        mergeDroppedSelection_mergesFoldersAndFiles();
        mergeDroppedSelection_sortTieBreakByPath();
        mergeDroppedSelection_zeroValidStillVirtualCache();
    }

private:
    SysExParser parser_;
    SysExDecoder decoder_;
    SysExEncoder encoder_;
    Core::PatchFileService service_;

    juce::File createTempScanDir()
    {
        return PatchFileServiceTestSupport::createTempDir(*this, "MatrixControlPatchFileService");
    }

    void copyFixturePatchToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = PatchTestFixtures::resolvePatchFixtureFile(fileName);
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
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("Z.syx")));
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("A.syx")));
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("M.syx")));

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.validCount, 4);
        expectEquals(result.sortedValidFileNames[0], juce::String("A.syx"));
        expectEquals(result.sortedValidFileNames[1], juce::String("M.syx"));
        expectEquals(result.sortedValidFileNames[2], juce::String("Patch 71.syx"));
        expectEquals(result.sortedValidFileNames[3], juce::String("Z.syx"));

        tempDir.deleteRecursively();
    }

    void scan_sortOrder_mutatorExportNames()
    {
        beginTest("Mutator export filenames sort root before retries");

        const auto tempDir = createTempScanDir();
        const auto model = PatchFileServiceTestSupport::makeDistinctBuffer(71);
        const juce::StringArray names {
            "M01-R00.syx",
            "M02.syx",
            "INITIAL.syx",
            "M01.syx",
            "M01-R01.syx",
            "M00.syx"
        };

        for (const auto& name : names)
            expect(service_.savePatchSysExFile(tempDir.getChildFile(name), model.data(), encoder_).success);

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.sortedValidFileNames.size(), 6);
        expectEquals(result.sortedValidFileNames[0], juce::String("INITIAL.syx"));
        expectEquals(result.sortedValidFileNames[1], juce::String("M00.syx"));
        expectEquals(result.sortedValidFileNames[2], juce::String("M01.syx"));
        expectEquals(result.sortedValidFileNames[3], juce::String("M01-R00.syx"));
        expectEquals(result.sortedValidFileNames[4], juce::String("M01-R01.syx"));
        expectEquals(result.sortedValidFileNames[5], juce::String("M02.syx"));

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
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("Patch 71.SYX")));

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 1);
        expectEquals(result.invalidCount, 0);
        expectEquals(result.sortedValidFileNames[0], juce::String("Patch 71.SYX"));

        tempDir.deleteRecursively();
    }

    static void expectEditBufferPatchHeader(juce::UnitTest& test, const juce::MemoryBlock& sysEx)
    {
        PatchFileServiceTestSupport::expectEditBufferPatchHeader(test, sysEx);
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

        juce::MemoryBlock savedSysEx;
        expect(target.loadFileAsData(savedSysEx));
        expectEditBufferPatchHeader(*this, savedSysEx);

        juce::uint8 loaded[SysExConstants::kPatchPackedDataSize] = {};
        expect(service_.loadPatchSysExFile(target, loaded).success);
        expect(std::memcmp(loaded,
                           Core::InitDefaults::patchData(),
                           SysExConstants::kPatchPackedDataSize) == 0);

        tempDir.deleteRecursively();
    }

    void savePatchSysExFile_bankExportStemWritesEditBufferHeader()
    {
        beginTest("savePatchSysExFile_bankExportStemWritesEditBufferHeader");

        const auto tempDir = createTempScanDir();
        const auto stem = Core::PatchFileNameSanitizer::bankExportFileStem(42, "TEST");
        const auto target = tempDir.getChildFile(Core::PatchFileNameSanitizer::ensureSyxExtension(stem));

        const auto result = service_.savePatchSysExFile(
            target,
            Core::InitDefaults::patchData(),
            encoder_,
            42);

        expect(result.success);
        expectEquals(target.getFileName(), juce::String("P42. TEST.syx"));

        juce::MemoryBlock savedSysEx;
        expect(target.loadFileAsData(savedSysEx));
        expectEditBufferPatchHeader(*this, savedSysEx);

        tempDir.deleteRecursively();
    }

    void savePatchSysExFile_bankExportSlotIgnoredInHeader()
    {
        beginTest("savePatchSysExFile_bankExportSlotIgnoredInHeader");

        const auto tempDir = createTempScanDir();
        const auto target = tempDir.getChildFile("P42. TEST.syx");

        const auto result = service_.savePatchSysExFile(
            target,
            Core::InitDefaults::patchData(),
            encoder_,
            42);

        expect(result.success);
        juce::MemoryBlock savedSysEx;
        expect(target.loadFileAsData(savedSysEx));
        expectEditBufferPatchHeader(*this, savedSysEx);

        tempDir.deleteRecursively();
    }

    void loadPatchSysExFile_thirdPartyOpcode01StillLoadable()
    {
        beginTest("loadPatchSysExFile_thirdPartyOpcode01StillLoadable");

        const auto source = PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx");
        expect(source.existsAsFile());

        juce::MemoryBlock sysEx;
        expect(source.loadFileAsData(sysEx));
        const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
        expectEquals(static_cast<int>(data[3]),
                     static_cast<int>(SysExConstants::Opcode::kSinglePatchData));

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        expect(service_.loadPatchSysExFile(source, packed).success);
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

        const auto source = PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx");
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

    void mergeDroppedSelection_mergesFoldersAndFiles()
    {
        beginTest("mergeDroppedSelection_mergesFoldersAndFiles");

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        copyFixturePatchToDir(dirA, "Patch 71.syx");
        copyFixturePatchToDir(dirB, "Patch 5.syx");
        copyFixturePatchToDir(dirB, "Patch 66.syx");

        juce::Array<juce::File> selection;
        selection.add(dirA);
        selection.add(dirB.getChildFile("Patch 66.syx"));

        const auto result = service_.mergeDroppedSelection(selection);

        expect(result.isVirtualList());
        expect(result.folderUsable);
        expectEquals(result.validCount, 2);
        expectEquals(result.sortedValidFiles.size(), 2);
        expectEquals(result.sortedValidFileNames[0], juce::String("Patch 66.syx"));
        expectEquals(result.sortedValidFileNames[1], juce::String("Patch 71.syx"));
        expectEquals(result.footerMessage, FooterMessages::formatScanSummary(2, 0));

        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    void mergeDroppedSelection_sortTieBreakByPath()
    {
        beginTest("mergeDroppedSelection_sortTieBreakByPath");

        const auto dirA = createTempScanDir();
        expect(dirA.createDirectory());
        const auto dirB = createTempScanDir();
        expect(dirB.createDirectory());
        copyFixturePatchToDir(dirA, "Patch 71.syx");
        copyFixturePatchToDir(dirB, "Patch 71.syx");

        juce::Array<juce::File> selection;
        selection.add(dirB.getChildFile("Patch 71.syx"));
        selection.add(dirA.getChildFile("Patch 71.syx"));

        const auto result = service_.mergeDroppedSelection(selection);

        expectEquals(result.validCount, 2);
        expectEquals(result.sortedValidFileNames[0], juce::String("Patch 71.syx"));
        expectEquals(result.sortedValidFileNames[1], juce::String("Patch 71.syx"));
        expect(result.sortedValidFiles[0].getFullPathName().compareIgnoreCase(
                   result.sortedValidFiles[1].getFullPathName())
               < 0);

        dirA.deleteRecursively();
        dirB.deleteRecursively();
    }

    void mergeDroppedSelection_zeroValidStillVirtualCache()
    {
        beginTest("mergeDroppedSelection_zeroValidStillVirtualCache");

        const auto junkDir = createTempScanDir();
        expect(junkDir.createDirectory());
        expect(junkDir.getChildFile("notes.txt").replaceWithText("x"));

        juce::Array<juce::File> selection;
        selection.add(junkDir);
        selection.add(junkDir.getChildFile("notes.txt"));

        const auto result = service_.mergeDroppedSelection(selection);

        expect(result.isVirtualList());
        expectEquals(result.validCount, 0);
        expect(result.invalidCount >= 1);
        expectEquals(service_.getLastScanResult().validCount, 0);

        junkDir.deleteRecursively();
    }
};

static PatchFileServiceTests patchFileServiceTests;
