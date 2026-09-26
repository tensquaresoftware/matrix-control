#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Services/MasterFileAssess.h"
#include "Core/Services/MasterM1kmCodec.h"
#include "PatchFixturePaths.h"
#include "PatchFileServiceTestSupport.h"

class MasterFileAssessTests : public juce::UnitTest
{
public:
    MasterFileAssessTests()
        : juce::UnitTest("MasterFileAssess")
        , decoder_(parser_)
    {
    }

    void runTest() override
    {
        assess_validMasterSyx();
        assess_validMasterM1km();
        assess_validMasterUppercaseSyx();
        assess_validMasterUppercaseM1km();
        assess_rejectsPatchSyxSize();
        assess_rejectsWrongSizeM1km();
        assess_rejectsWrongSizeSyx();
        assess_rejectsCorrectSizeNonMasterSyx();
        assess_rejectsCorrectSizeOutOfRangeM1km();
        assess_rejectsNonMasterExtension();
        assess_rejectsMissingFile();
    }

private:
    SysExParser parser_;
    SysExDecoder decoder_;

    juce::File createTempDir(const char* prefix)
    {
        return PatchFileServiceTestSupport::createTempDir(*this, prefix);
    }

    static juce::MemoryBlock makeM1kmWithInt16(int16_t fillValue)
    {
        juce::MemoryBlock data(Core::MasterM1kmCodec::kFileByteSize, true);
        auto* bytes = static_cast<juce::uint8*>(data.getData());

        for (size_t i = 0; i < Core::MasterM1kmCodec::kInt16Count; ++i)
        {
            bytes[i * 2] = static_cast<juce::uint8>(fillValue & 0xff);
            bytes[i * 2 + 1] = static_cast<juce::uint8>((fillValue >> 8) & 0xff);
        }

        return data;
    }

    void assess_validMasterSyx()
    {
        beginTest("assess_validMasterSyx");

        const auto file = PatchTestFixtures::matrixControlMastersDir().getChildFile("Master 1.syx");
        expect(file.existsAsFile());

        const auto assessment = Core::MasterFileAssess::assess(file, decoder_);
        expect(assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kSyx);
    }

    void assess_validMasterM1km()
    {
        beginTest("assess_validMasterM1km");

        const auto file = PatchTestFixtures::matrix1000EditorMastersDir().getChildFile("Master 1.m1km");
        expect(file.existsAsFile());

        const auto assessment = Core::MasterFileAssess::assess(file, decoder_);
        expect(assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kM1km);
    }

    void assess_validMasterUppercaseSyx()
    {
        beginTest("assess_validMasterUppercaseSyx");

        const auto source = PatchTestFixtures::matrixControlMastersDir().getChildFile("Master 1.syx");
        expect(source.existsAsFile());

        const auto dir = createTempDir("MatrixControlMasterFileAssessUpperSyx");
        const auto target = dir.getChildFile("Master 1.SYX");
        expect(source.copyFileTo(target));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kSyx);

        dir.deleteRecursively();
    }

    void assess_validMasterUppercaseM1km()
    {
        beginTest("assess_validMasterUppercaseM1km");

        const auto source = PatchTestFixtures::matrix1000EditorMastersDir().getChildFile("Master 1.m1km");
        expect(source.existsAsFile());

        const auto dir = createTempDir("MatrixControlMasterFileAssessUpperM1km");
        const auto target = dir.getChildFile("Master 1.M1KM");
        expect(source.copyFileTo(target));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kM1km);

        dir.deleteRecursively();
    }

    void assess_rejectsPatchSyxSize()
    {
        beginTest("assess_rejectsPatchSyxSize");

        const auto file = PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx");
        expect(file.existsAsFile());
        expectEquals(static_cast<int>(file.getSize()),
                     static_cast<int>(SysExConstants::kPatchMessageLength));

        const auto assessment = Core::MasterFileAssess::assess(file, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kSyx);
    }

    void assess_rejectsWrongSizeM1km()
    {
        beginTest("assess_rejectsWrongSizeM1km");

        const auto dir = createTempDir("MatrixControlMasterFileAssessM1km");
        const auto target = dir.getChildFile("bad.m1km");
        expect(target.replaceWithData("too-short", 9));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kM1km);

        dir.deleteRecursively();
    }

    void assess_rejectsWrongSizeSyx()
    {
        beginTest("assess_rejectsWrongSizeSyx");

        const auto dir = createTempDir("MatrixControlMasterFileAssessSyx");
        const auto target = dir.getChildFile("bad-master.syx");
        juce::MemoryBlock junk(400, true);
        expect(target.replaceWithData(junk.getData(), junk.getSize()));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kSyx);

        dir.deleteRecursively();
    }

    void assess_rejectsCorrectSizeNonMasterSyx()
    {
        beginTest("assess_rejectsCorrectSizeNonMasterSyx");

        juce::MemoryBlock data;
        expect(PatchTestFixtures::matrixControlMastersDir()
                   .getChildFile("Master 1.syx")
                   .loadFileAsData(data));
        expectEquals(static_cast<int>(data.getSize()),
                     static_cast<int>(SysExConstants::kMasterMessageLength));

        // F0 10 06 <opcode> … — flip Master opcode 0x03 to patch 0x01.
        auto* bytes = static_cast<juce::uint8*>(data.getData());
        expectEquals(static_cast<int>(bytes[3]),
                     static_cast<int>(SysExConstants::Opcode::kMasterParameterData));
        bytes[3] = SysExConstants::Opcode::kSinglePatchData;

        const auto dir = createTempDir("MatrixControlMasterFileAssessNonMasterSyx");
        const auto target = dir.getChildFile("non-master-351.syx");
        expect(target.replaceWithData(data.getData(), data.getSize()));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kSyx);

        dir.deleteRecursively();
    }

    void assess_rejectsCorrectSizeOutOfRangeM1km()
    {
        beginTest("assess_rejectsCorrectSizeOutOfRangeM1km");

        auto data = makeM1kmWithInt16(0);
        auto* bytes = static_cast<juce::uint8*>(data.getData());
        // 256 as int16 LE — outside MasterM1kmCodec [-128, 255].
        bytes[0] = 0x00;
        bytes[1] = 0x01;
        expectEquals(static_cast<int>(data.getSize()),
                     static_cast<int>(Core::MasterM1kmCodec::kFileByteSize));

        const auto dir = createTempDir("MatrixControlMasterFileAssessOutOfRangeM1km");
        const auto target = dir.getChildFile("oor.m1km");
        expect(target.replaceWithData(data.getData(), data.getSize()));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kM1km);

        dir.deleteRecursively();
    }

    void assess_rejectsNonMasterExtension()
    {
        beginTest("assess_rejectsNonMasterExtension");

        const auto dir = createTempDir("MatrixControlMasterFileAssessExt");
        const auto target = dir.getChildFile("notes.txt");
        expect(target.replaceWithText("hello"));

        const auto assessment = Core::MasterFileAssess::assess(target, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kNone);

        dir.deleteRecursively();
    }

    void assess_rejectsMissingFile()
    {
        beginTest("assess_rejectsMissingFile");

        const juce::File missing("/tmp/matrix-control-missing-master-assess.syx");
        expect(! missing.existsAsFile());

        const auto assessment = Core::MasterFileAssess::assess(missing, decoder_);
        expect(! assessment.isValidMaster);
        expect(assessment.format == Core::MasterFileAssess::Format::kNone);
    }
};

static MasterFileAssessTests masterFileAssessTests;
