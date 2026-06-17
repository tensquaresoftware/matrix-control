#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"

class InitTemplateLoaderTests : public juce::UnitTest
{
public:
    InitTemplateLoaderTests() : juce::UnitTest("InitTemplateLoader Tests") {}

    void runTest() override
    {
        runResolveHelpers();
        runLoadPatchFromFixtureFile();
        runLoadMasterFromFixtureFile();
        runEmptyTemplatesFolderFallback();
        runMissingPatchFileFallback();
        runMissingMasterFileFallback();
        runInvalidPatchFileFallback();
        runInvalidMasterFileFallback();
    }

private:
    static juce::File fixturesInitDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Init");
    }

    juce::File createTempTemplatesDir()
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                       .getNonexistentChildFile("MatrixControlInitTemplateLoader", "", false);
        expect(dir.createDirectory(), "Temp templates dir should be created");
        return dir;
    }

    void copyFixtureToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = fixturesInitDir().getChildFile(fileName);
        expect(source.existsAsFile(), fileName + " fixture should exist");
        expect(source.copyFileTo(dir.getChildFile(fileName)), "Fixture copy should succeed");
    }

    void runResolveHelpers()
    {
        beginTest("Resolve helpers return fixed filenames");

        const juce::File folder("/tmp/templates");
        expectEquals(Core::InitTemplateLoader::resolvePatchInitFile(folder).getFileName(),
                     juce::String(Core::InitTemplateLoader::kPatchInitFileName));
        expectEquals(Core::InitTemplateLoader::resolveMasterInitFile(folder).getFileName(),
                     juce::String(Core::InitTemplateLoader::kMasterInitFileName));
    }

    void runLoadPatchFromFixtureFile()
    {
        beginTest("Load patch from user PatchInit.syx");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kPatchInitFileName);

        Core::PatchModel model;
        const auto result = loader.loadPatch(model, tempDir);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kUserFile);
        expect(result.infoMessage.isEmpty());

        expect(std::memcmp(model.data(), Core::InitDefaults::patchData(), Core::InitDefaults::kPatchSize) == 0,
               "Loaded patch should match InitDefaults fixture bytes");

        tempDir.deleteRecursively();
    }

    void runLoadMasterFromFixtureFile()
    {
        beginTest("Load master from user MasterInit.syx");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        Core::MasterModel model;
        const auto result = loader.loadMaster(model, tempDir);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kUserFile);
        expect(result.infoMessage.isEmpty());

        expect(std::memcmp(model.data(), Core::InitDefaults::masterData(), Core::InitDefaults::kMasterSize) == 0,
               "Loaded master should match InitDefaults fixture bytes");

        tempDir.deleteRecursively();
    }

    void runMissingPatchFileFallback()
    {
        beginTest("Missing patch file falls back to InitDefaults");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempTemplatesDir();
        Core::PatchModel model;
        const auto result = loader.loadPatch(model, tempDir);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.fallbackReason == Core::InitTemplateFallbackReason::kFileMissing);
        expect(result.infoMessage.isNotEmpty());
        expect(result.infoMessage.contains("not found"));

        expect(std::memcmp(model.data(), Core::InitDefaults::patchData(), Core::InitDefaults::kPatchSize) == 0,
               "Fallback patch should match InitDefaults");

        tempDir.deleteRecursively();
    }

    void runEmptyTemplatesFolderFallback()
    {
        beginTest("Empty templates folder falls back to InitDefaults");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        Core::PatchModel patchModel;
        const auto patchResult = loader.loadPatch(patchModel, juce::File());

        expect(patchResult.success);
        expect(patchResult.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(patchResult.fallbackReason == Core::InitTemplateFallbackReason::kFileMissing);
        expect(patchResult.infoMessage.contains("not found"));
        expect(std::memcmp(patchModel.data(), Core::InitDefaults::patchData(), Core::InitDefaults::kPatchSize) == 0,
               "Empty folder patch fallback should match InitDefaults");

        Core::MasterModel masterModel;
        const auto masterResult = loader.loadMaster(masterModel, juce::File());

        expect(masterResult.success);
        expect(masterResult.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(masterResult.fallbackReason == Core::InitTemplateFallbackReason::kFileMissing);
        expect(masterResult.infoMessage.contains("not found"));
        expect(std::memcmp(masterModel.data(), Core::InitDefaults::masterData(), Core::InitDefaults::kMasterSize) == 0,
               "Empty folder master fallback should match InitDefaults");
    }

    void runMissingMasterFileFallback()
    {
        beginTest("Missing master file falls back to InitDefaults");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempTemplatesDir();
        Core::MasterModel model;
        const auto result = loader.loadMaster(model, tempDir);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.fallbackReason == Core::InitTemplateFallbackReason::kFileMissing);
        expect(result.infoMessage.isNotEmpty());
        expect(result.infoMessage.contains("not found"));

        expect(std::memcmp(model.data(), Core::InitDefaults::masterData(), Core::InitDefaults::kMasterSize) == 0,
               "Fallback master should match InitDefaults");

        tempDir.deleteRecursively();
    }

    void runInvalidMasterFileFallback()
    {
        beginTest("Invalid master file falls back to InitDefaults");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempTemplatesDir();
        tempDir.getChildFile(Core::InitTemplateLoader::kMasterInitFileName)
            .replaceWithText("not valid sysex");

        Core::MasterModel model;
        const auto result = loader.loadMaster(model, tempDir);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.fallbackReason == Core::InitTemplateFallbackReason::kFileInvalid);
        expect(result.infoMessage.isNotEmpty());
        expect(result.infoMessage.contains("invalid"));

        expect(std::memcmp(model.data(), Core::InitDefaults::masterData(), Core::InitDefaults::kMasterSize) == 0,
               "Invalid master file fallback should match InitDefaults");

        tempDir.deleteRecursively();
    }

    void runInvalidPatchFileFallback()
    {
        beginTest("Invalid patch file falls back to InitDefaults");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempTemplatesDir();
        tempDir.getChildFile(Core::InitTemplateLoader::kPatchInitFileName)
            .replaceWithText("not valid sysex");

        Core::PatchModel model;
        const auto result = loader.loadPatch(model, tempDir);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.fallbackReason == Core::InitTemplateFallbackReason::kFileInvalid);
        expect(result.infoMessage.isNotEmpty());
        expect(result.infoMessage.contains("invalid"));

        expect(std::memcmp(model.data(), Core::InitDefaults::patchData(), Core::InitDefaults::kPatchSize) == 0,
               "Invalid file fallback should match InitDefaults");

        tempDir.deleteRecursively();
    }
};

static InitTemplateLoaderTests initTemplateLoaderTests;
