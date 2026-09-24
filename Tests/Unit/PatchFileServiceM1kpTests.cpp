#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "PatchFixturePaths.h"
#include "PatchFileServiceTestSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

class PatchFileServiceM1kpTests : public juce::UnitTest
{
public:
    PatchFileServiceM1kpTests()
        : juce::UnitTest("PatchFileServiceM1kp")
        , decoder_(parser_)
        , service_(decoder_)
    {
    }

    void runTest() override
    {
        scan_validM1kpCountsAndLabels();
        scan_dualStemKeepsBothEntries();
        scan_rejectsWrongSizeM1kp();
        scan_rejectsOversizedM1kp();
        scan_rejectsOutOfRangeM1kp();
        loadPatchSysExFile_validM1kp();
        loadPatchSysExFile_missingM1kpFails();
        loadPatchSysExFile_negativeInt16RoundTrip();
        loadM1kp_thenSaveWritesSyxOnly();
        mergeDroppedSelection_includesM1kp();
        formatOpenListDisplayName_syxAndM1kp();
        hasSupportedPatchExtension_acceptsSyxAndM1kp();
    }

private:
    SysExParser parser_;
    SysExDecoder decoder_;
    SysExEncoder encoder_;
    Core::PatchFileService service_;

    juce::File createTempScanDir()
    {
        return PatchFileServiceTestSupport::createTempDir(*this, "MatrixControlPatchFileServiceM1kp");
    }

    void scan_validM1kpCountsAndLabels()
    {
        beginTest("Valid .m1kp is scanned and labeled with (m1kp)");

        const auto tempDir = createTempScanDir();
        const auto source = PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp");
        expect(source.existsAsFile());
        expect(source.copyFileTo(tempDir.getChildFile("P-Test.m1kp")));

        const auto result = service_.scanFolder(tempDir);

        expect(result.folderUsable);
        expectEquals(result.validCount, 1);
        expectEquals(result.invalidCount, 0);
        expectEquals(result.sortedValidFileNames[0], juce::String("P-Test.m1kp"));
        expectEquals(Core::PatchFileService::formatOpenListDisplayName(result.sortedValidFileNames[0]),
                     juce::String("P-Test (m1kp)"));

        tempDir.deleteRecursively();
    }

    void scan_dualStemKeepsBothEntries()
    {
        beginTest("Dual stem keeps .syx and .m1kp as separate list entries");

        const auto tempDir = createTempScanDir();
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx")
                   .copyFileTo(tempDir.getChildFile("NicePad.syx")));
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(tempDir.getChildFile("NicePad.m1kp")));

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.validCount, 2);
        expectEquals(result.invalidCount, 0);
        expectEquals(result.sortedValidFileNames.size(), 2);

        juce::StringArray labels;
        for (const auto& name : result.sortedValidFileNames)
            labels.add(Core::PatchFileService::formatOpenListDisplayName(name));

        expect(labels.contains("NicePad"));
        expect(labels.contains("NicePad (m1kp)"));
        expect(! labels.contains("NicePad.syx"));

        tempDir.deleteRecursively();
    }

    void scan_rejectsWrongSizeM1kp()
    {
        beginTest("Wrong-size .m1kp counts as invalid");

        const auto tempDir = createTempScanDir();
        const juce::uint8 truncated[] = { 0x41, 0x00, 0x42, 0x00 };
        expect(tempDir.getChildFile("short.m1kp").replaceWithData(truncated, sizeof(truncated)));

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.validCount, 0);
        expectEquals(result.invalidCount, 1);

        tempDir.deleteRecursively();
    }

    void scan_rejectsOversizedM1kp()
    {
        beginTest("Oversized 270-byte .m1kp counts as invalid");

        const auto tempDir = createTempScanDir();
        juce::MemoryBlock oversized(270, true);
        const auto file = tempDir.getChildFile("big.m1kp");
        expect(file.replaceWithData(oversized.getData(), oversized.getSize()));

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.validCount, 0);
        expectEquals(result.invalidCount, 1);

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        expect(! service_.loadPatchSysExFile(file, packed).success);

        tempDir.deleteRecursively();
    }

    void scan_rejectsOutOfRangeM1kp()
    {
        beginTest("Out-of-range int16 .m1kp counts as invalid");

        const auto tempDir = createTempScanDir();
        const auto source = PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp");
        juce::MemoryBlock data;
        expect(source.loadFileAsData(data));
        expectEquals(static_cast<int>(data.getSize()), 268);

        auto* bytes = static_cast<juce::uint8*>(data.getData());
        bytes[0] = 200;
        bytes[1] = 0;
        expect(tempDir.getChildFile("oor.m1kp").replaceWithData(data.getData(), data.getSize()));

        const auto result = service_.scanFolder(tempDir);

        expectEquals(result.validCount, 0);
        expectEquals(result.invalidCount, 1);

        tempDir.deleteRecursively();
    }

    void loadPatchSysExFile_validM1kp()
    {
        beginTest("loadPatchSysExFile_validM1kp");

        const auto source = PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp");
        expect(source.existsAsFile());

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        const auto result = service_.loadPatchSysExFile(source, packed);

        expect(result.success);

        Core::PatchModel model;
        model.loadFrom(packed);
        expectEquals(model.getName(), juce::String("BNK2: 02"));
    }

    void loadPatchSysExFile_negativeInt16RoundTrip()
    {
        beginTest("loadPatchSysExFile_negativeInt16RoundTrip");

        const auto tempDir = createTempScanDir();
        juce::MemoryBlock data(268, true);
        auto* bytes = static_cast<juce::uint8*>(data.getData());
        // int16 -9 little-endian = 0xFFF7 → packed byte 0xF7
        bytes[0] = 0xF7;
        bytes[1] = 0xFF;

        const auto file = tempDir.getChildFile("neg.m1kp");
        expect(file.replaceWithData(data.getData(), data.getSize()));

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        expect(service_.loadPatchSysExFile(file, packed).success);
        expectEquals(static_cast<int>(packed[0]), 0xF7);

        tempDir.deleteRecursively();
    }

    void loadPatchSysExFile_missingM1kpFails()
    {
        beginTest("loadPatchSysExFile_missingM1kpFails");

        const auto missing = createTempScanDir().getChildFile("gone.m1kp");
        expect(! missing.existsAsFile());

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        const auto result = service_.loadPatchSysExFile(missing, packed);

        expect(! result.success);
        expectEquals(result.errorMessage, juce::String("File not found"));
    }

    void loadM1kp_thenSaveWritesSyxOnly()
    {
        beginTest("loadM1kp_thenSaveWritesSyxOnly");

        const auto tempDir = createTempScanDir();
        const auto m1kp = tempDir.getChildFile("P-Test.m1kp");
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp").copyFileTo(m1kp));

        juce::MemoryBlock originalM1kp;
        expect(m1kp.loadFileAsData(originalM1kp));

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        expect(service_.loadPatchSysExFile(m1kp, packed).success);

        const auto saved = tempDir.getChildFile("FromM1kp.syx");
        expect(service_.savePatchSysExFile(saved, packed, encoder_).success);
        expect(saved.existsAsFile());
        expect(! tempDir.getChildFile("FromM1kp.m1kp").existsAsFile());

        juce::MemoryBlock afterSave;
        expect(m1kp.loadFileAsData(afterSave));
        expect(afterSave == originalM1kp);

        juce::uint8 reloaded[SysExConstants::kPatchPackedDataSize] = {};
        expect(service_.loadPatchSysExFile(saved, reloaded).success);
        expect(std::memcmp(packed, reloaded, SysExConstants::kPatchPackedDataSize) == 0);

        tempDir.deleteRecursively();
    }

    void mergeDroppedSelection_includesM1kp()
    {
        beginTest("mergeDroppedSelection_includesM1kp");

        const auto dir = createTempScanDir();
        expect(dir.createDirectory());
        expect(PatchTestFixtures::resolvePatchFixtureFile("P-Test.m1kp")
                   .copyFileTo(dir.getChildFile("P-Test.m1kp")));

        juce::Array<juce::File> selection;
        selection.add(dir.getChildFile("P-Test.m1kp"));

        const auto result = service_.mergeDroppedSelection(selection);

        expect(result.isVirtualList());
        expectEquals(result.validCount, 1);
        expectEquals(result.sortedValidFileNames[0], juce::String("P-Test.m1kp"));
        expectEquals(result.footerMessage, FooterMessages::formatScanSummary(1, 0));

        dir.deleteRecursively();
    }

    void formatOpenListDisplayName_syxAndM1kp()
    {
        beginTest("formatOpenListDisplayName_syxAndM1kp");

        expectEquals(Core::PatchFileService::formatOpenListDisplayName("NicePad.syx"),
                     juce::String("NicePad"));
        expectEquals(Core::PatchFileService::formatOpenListDisplayName("NicePad.m1kp"),
                     juce::String("NicePad (m1kp)"));
        expectEquals(Core::PatchFileService::formatOpenListDisplayName("NicePad.M1KP"),
                     juce::String("NicePad (m1kp)"));
    }

    void hasSupportedPatchExtension_acceptsSyxAndM1kp()
    {
        beginTest("hasSupportedPatchExtension_acceptsSyxAndM1kp");

        const auto asFile = [](const char* name) {
            return juce::File::createFileWithoutCheckingPath(name);
        };

        expect(Core::PatchFileService::hasSupportedPatchExtension(asFile("NicePad.syx")));
        expect(Core::PatchFileService::hasSupportedPatchExtension(asFile("NicePad.m1kp")));
        expect(Core::PatchFileService::hasSupportedPatchExtension(asFile("NicePad.M1KP")));
        expect(! Core::PatchFileService::hasSupportedPatchExtension(asFile("notes.txt")));
    }
};

static PatchFileServiceM1kpTests patchFileServiceM1kpTests;
