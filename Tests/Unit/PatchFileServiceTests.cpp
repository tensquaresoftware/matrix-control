#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Services/PatchFileService.h"
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
};

static PatchFileServiceTests patchFileServiceTests;
