#include <juce_core/juce_core.h>

#include <cstring>

#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "PatchFixturePaths.h"
#include "PatchFileServiceTestSupport.h"

class PatchFileServiceAssessTests : public juce::UnitTest
{
public:
    PatchFileServiceAssessTests()
        : juce::UnitTest("PatchFileServiceAssess")
        , decoder_(parser_)
        , service_(decoder_)
    {
    }

    void runTest() override
    {
        assessSinglePatch_validFixture();
        assessSinglePatch_rejectsMasterAsBankOrMulti();
        assessSinglePatch_rejectsNonSyx();
        assessSinglePatch_rejectsMultiMessageConcat();
        assessSinglePatch_bnkNameFallsBackToStem();
        assessSinglePatch_usableInternalNameShown();
    }

private:
    SysExParser parser_;
    SysExDecoder decoder_;
    SysExEncoder encoder_;
    Core::PatchFileService service_;

    juce::File createTempScanDir()
    {
        return PatchFileServiceTestSupport::createTempDir(*this, "MatrixControlPatchFileServiceAssess");
    }

    juce::File fixturesMastersDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Masters");
    }

    void assessSinglePatch_validFixture()
    {
        beginTest("assessSinglePatch_validFixture");

        const auto file = PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx");
        expect(file.existsAsFile());

        const auto assessment = service_.assessSinglePatchSyxFile(file);
        expect(assessment.isValidSinglePatch);
        expect(assessment.rejectKind == Core::SinglePatchSyxRejectKind::kNone);
        expect(assessment.previewPrimaryName.isNotEmpty());
        expect(service_.isValidSinglePatchSyxFile(file));
    }

    void assessSinglePatch_rejectsMasterAsBankOrMulti()
    {
        beginTest("assessSinglePatch_rejectsMasterAsBankOrMulti");

        const auto file = fixturesMastersDir().getChildFile("Master 1.syx");
        expect(file.existsAsFile());

        const auto assessment = service_.assessSinglePatchSyxFile(file);
        expect(! assessment.isValidSinglePatch);
        expect(assessment.rejectKind == Core::SinglePatchSyxRejectKind::kBankOrMultiMessage);
        expect(! service_.isValidSinglePatchSyxFile(file));
    }

    void assessSinglePatch_rejectsNonSyx()
    {
        beginTest("assessSinglePatch_rejectsNonSyx");

        const auto tempDir = createTempScanDir();
        const auto file = tempDir.getChildFile("notes.txt");
        expect(file.replaceWithText("not a patch"));

        const auto assessment = service_.assessSinglePatchSyxFile(file);
        expect(! assessment.isValidSinglePatch);
        expect(assessment.rejectKind == Core::SinglePatchSyxRejectKind::kNotSyx);

        tempDir.deleteRecursively();
    }

    void assessSinglePatch_rejectsMultiMessageConcat()
    {
        beginTest("assessSinglePatch_rejectsMultiMessageConcat");

        const auto tempDir = createTempScanDir();
        const auto a = PatchTestFixtures::resolvePatchFixtureFile("Patch 5.syx");
        const auto b = PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx");
        expect(a.existsAsFile());
        expect(b.existsAsFile());

        juce::MemoryBlock combined;
        expect(a.loadFileAsData(combined));
        juce::MemoryBlock second;
        expect(b.loadFileAsData(second));
        combined.append(second.getData(), second.getSize());

        const auto file = tempDir.getChildFile("two-patches.syx");
        expect(file.replaceWithData(combined.getData(), combined.getSize()));

        const auto assessment = service_.assessSinglePatchSyxFile(file);
        expect(! assessment.isValidSinglePatch);
        expect(assessment.rejectKind == Core::SinglePatchSyxRejectKind::kBankOrMultiMessage);

        tempDir.deleteRecursively();
    }

    void assessSinglePatch_bnkNameFallsBackToStem()
    {
        beginTest("assessSinglePatch_bnkNameFallsBackToStem");

        const auto file = PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx");
        expect(file.existsAsFile());

        const auto assessment = service_.assessSinglePatchSyxFile(file);
        expect(assessment.isValidSinglePatch);
        expectEquals(assessment.previewPrimaryName, juce::String("Patch 71"));
    }

    void assessSinglePatch_usableInternalNameShown()
    {
        beginTest("assessSinglePatch_usableInternalNameShown");

        const auto tempDir = createTempScanDir();
        const auto target = tempDir.getChildFile("DROPSTEM.syx");

        Core::PatchModel model;
        std::memcpy(model.data(),
                    Core::InitDefaults::patchData(),
                    SysExConstants::kPatchPackedDataSize);
        model.setName("NYLON 12");

        expect(service_.savePatchSysExFile(target, model.data(), encoder_).success);

        const auto assessment = service_.assessSinglePatchSyxFile(target);
        expect(assessment.isValidSinglePatch);
        expectEquals(assessment.previewPrimaryName, juce::String("NYLON 12"));

        tempDir.deleteRecursively();
    }
};

static PatchFileServiceAssessTests patchFileServiceAssessTests;
