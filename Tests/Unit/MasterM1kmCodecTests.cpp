#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/InitTemplateWriter.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/MasterModel.h"
#include "Core/Services/MasterM1kmCodec.h"
#include "Core/Services/MasterM1kmLoadPolicy.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "PatchFixturePaths.h"

class MasterM1kmCodecTests : public juce::UnitTest
{
public:
    MasterM1kmCodecTests() : juce::UnitTest("MasterM1kmCodec") {}

    void runTest() override
    {
        decode_validStudyFixture();
        decode_rejectsWrongSize();
        decode_rejectsOversized();
        decode_rejectsOutOfRangeHigh();
        decode_rejectsOutOfRangeLow();
        decode_acceptsSignedAndGroupsMaskRange();
        policy_masterSettingsOnlyResetsGroupsAndCascade();
        policy_fullMasterLeavesBufferUnchanged();
        loadPackedIntoModel_masterSettingsOnly();
        loadPackedIntoModel_fullMaster();
        loader_validM1kmReturnsUserFile();
        loader_invalidM1kmRejectsBeforeSuccess();
        writer_m1kmWithoutPolicyFails();
        writer_m1kmMasterSettingsOnlyAppliesPolicy();
        writer_m1kmFullMasterPreservesGroups();
        writer_m1kmThenSaveWritesSyxOnly();
        decodeUserFile_missingFails();
        decodeWithoutCommit_leavesLiveMasterUnchanged();
        helpCopy_mentionsSyxAndM1km();
    }

private:
    static juce::File fixturesMastersDir()
    {
        return PatchTestFixtures::matrix1000EditorMastersDir();
    }

    static juce::File studyFixture(const juce::String& fileName)
    {
        return fixturesMastersDir().getChildFile(fileName);
    }

    juce::File createTempDir(const juce::String& prefix)
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                       .getNonexistentChildFile(prefix, "", true);
        expect(dir.createDirectory());
        return dir;
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

    void decode_validStudyFixture()
    {
        beginTest("decode_validStudyFixture");

        const auto file = studyFixture("Master 1.m1km");
        expect(file.existsAsFile());
        expectEquals(static_cast<int>(file.getSize()), static_cast<int>(Core::MasterM1kmCodec::kFileByteSize));

        juce::MemoryBlock data;
        expect(file.loadFileAsData(data));

        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(Core::MasterM1kmCodec::decodeToPacked(data, packed));
        expect(Core::MasterM1kmCodec::isValidContents(data));
        expect(Core::MasterM1kmCodec::hasExtension(file));
    }

    void decode_rejectsWrongSize()
    {
        beginTest("decode_rejectsWrongSize");

        const juce::uint8 truncated[] = { 0x01, 0x00, 0x02, 0x00 };
        juce::MemoryBlock data(truncated, sizeof(truncated));
        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count];
        std::memset(packed, 0xAB, sizeof(packed));

        expect(! Core::MasterM1kmCodec::decodeToPacked(data, packed));
        expect(packed[0] == 0xAB);
    }

    void decode_rejectsOversized()
    {
        beginTest("decode_rejectsOversized");

        auto data = makeM1kmWithInt16(0);
        data.append("xx", 2);
        expectEquals(static_cast<int>(data.getSize()), static_cast<int>(Core::MasterM1kmCodec::kFileByteSize + 2));

        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(! Core::MasterM1kmCodec::decodeToPacked(data, packed));
    }

    void decode_rejectsOutOfRangeHigh()
    {
        beginTest("decode_rejectsOutOfRangeHigh");

        auto data = makeM1kmWithInt16(0);
        auto* bytes = static_cast<juce::uint8*>(data.getData());
        // 256 as int16 LE
        bytes[0] = 0x00;
        bytes[1] = 0x01;

        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(! Core::MasterM1kmCodec::decodeToPacked(data, packed));
    }

    void decode_rejectsOutOfRangeLow()
    {
        beginTest("decode_rejectsOutOfRangeLow");

        auto data = makeM1kmWithInt16(0);
        auto* bytes = static_cast<juce::uint8*>(data.getData());
        // -129 as int16 LE
        bytes[0] = 0x7F;
        bytes[1] = 0xFF;

        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(! Core::MasterM1kmCodec::decodeToPacked(data, packed));
    }

    void decode_acceptsSignedAndGroupsMaskRange()
    {
        beginTest("decode_acceptsSignedAndGroupsMaskRange");

        // Store signed -128 and unsigned 255 as little-endian int16 values.
        juce::MemoryBlock data(Core::MasterM1kmCodec::kFileByteSize, true);
        auto* bytes = static_cast<juce::uint8*>(data.getData());
        bytes[0] = 0x80; // -128 LE
        bytes[1] = 0xFF;
        bytes[2] = 0xFF; // 255 LE
        bytes[3] = 0x00;

        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(Core::MasterM1kmCodec::decodeToPacked(data, packed));
        expectEquals(static_cast<int>(packed[0]), 0x80);
        expectEquals(static_cast<int>(packed[1]), 0xFF);
    }

    void policy_masterSettingsOnlyResetsGroupsAndCascade()
    {
        beginTest("policy_masterSettingsOnlyResetsGroupsAndCascade");

        juce::uint8 packed[SysExConstants::kMasterPackedDataSize];
        std::memset(packed, 0x5A, sizeof(packed));

        Core::MasterM1kmLoadPolicy::apply(packed,
                                          Core::MasterM1kmGroupsPolicy::kMasterSettingsOnly,
                                          Core::InitDefaults::masterData());

        const auto* defaults = Core::InitDefaults::masterData();
        expect(std::memcmp(packed + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           Core::MasterM1kmLoadPolicy::kGroupsCount)
               == 0);
        expect(std::memcmp(packed + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                           Core::MasterM1kmLoadPolicy::kCascadeCount)
               == 0);
        expectEquals(static_cast<int>(packed[0]), 0x5A);
        expectEquals(static_cast<int>(packed[35]), 0x5A);
        expectEquals(static_cast<int>(packed[165]), 0x5A);
        expectEquals(static_cast<int>(packed[169]), 0x5A);
    }

    void policy_fullMasterLeavesBufferUnchanged()
    {
        beginTest("policy_fullMasterLeavesBufferUnchanged");

        juce::uint8 packed[SysExConstants::kMasterPackedDataSize];
        std::memset(packed, 0xA5, sizeof(packed));
        juce::uint8 before[SysExConstants::kMasterPackedDataSize];
        std::memcpy(before, packed, sizeof(before));

        Core::MasterM1kmLoadPolicy::apply(packed,
                                          Core::MasterM1kmGroupsPolicy::kFullMaster,
                                          Core::InitDefaults::masterData());

        expect(std::memcmp(packed, before, sizeof(packed)) == 0);
    }

    void loadPackedIntoModel_masterSettingsOnly()
    {
        beginTest("loadPackedIntoModel_masterSettingsOnly");

        juce::uint8 packed[SysExConstants::kMasterPackedDataSize];
        std::memset(packed, 0x5A, sizeof(packed));

        Core::MasterModel model;
        Core::MasterM1kmLoadPolicy::loadPackedIntoModel(
            model, packed, Core::MasterM1kmGroupsPolicy::kMasterSettingsOnly,
            Core::InitDefaults::masterData());

        const auto* defaults = Core::InitDefaults::masterData();
        expect(std::memcmp(model.data() + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           Core::MasterM1kmLoadPolicy::kGroupsCount)
               == 0);
        expect(std::memcmp(model.data() + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                           Core::MasterM1kmLoadPolicy::kCascadeCount)
               == 0);
        expectEquals(static_cast<int>(model.data()[0]), 0x5A);
        expectEquals(static_cast<int>(model.data()[169]), 0x5A);
        // Input buffer must stay untouched (helper copies first).
        expectEquals(static_cast<int>(packed[Core::MasterM1kmLoadPolicy::kGroupsOffset]), 0x5A);
    }

    void loadPackedIntoModel_fullMaster()
    {
        beginTest("loadPackedIntoModel_fullMaster");

        juce::uint8 packed[SysExConstants::kMasterPackedDataSize];
        std::memset(packed, 0xA5, sizeof(packed));

        Core::MasterModel model;
        Core::MasterM1kmLoadPolicy::loadPackedIntoModel(
            model, packed, Core::MasterM1kmGroupsPolicy::kFullMaster,
            Core::InitDefaults::masterData());

        expect(std::memcmp(model.data(), packed, sizeof(packed)) == 0);
    }

    void loader_validM1kmReturnsUserFile()
    {
        beginTest("loader_validM1kmReturnsUserFile");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmLoaderValid");
        const auto target = tempDir.getChildFile("Master 1.m1km");
        expect(studyFixture("Master 1.m1km").copyFileTo(target));

        Core::MasterModel model;
        const auto result = loader.loadMasterFile(model, target);
        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kUserFile);

        juce::MemoryBlock data;
        expect(target.loadFileAsData(data));
        juce::uint8 expected[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(Core::MasterM1kmCodec::decodeToPacked(data, expected));
        expect(std::memcmp(model.data(), expected, Core::MasterM1kmCodec::kInt16Count) == 0);

        tempDir.deleteRecursively();
    }

    void loader_invalidM1kmRejectsBeforeSuccess()
    {
        beginTest("loader_invalidM1kmRejectsBeforeSuccess");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmLoaderInvalid");
        const auto target = tempDir.getChildFile("bad.m1km");
        const juce::uint8 truncated[] = { 0x01, 0x00 };
        expect(target.replaceWithData(truncated, sizeof(truncated)));

        Core::MasterModel live;
        live.loadFrom(Core::InitDefaults::masterData());
        juce::uint8 before[Core::MasterModel::kBufferSize];
        std::memcpy(before, live.data(), sizeof(before));

        Core::MasterModel scratch;
        scratch.loadFrom(before);
        const auto result = loader.loadMasterFile(scratch, target);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.fallbackReason == Core::InitTemplateFallbackReason::kFileInvalid);

        // Live model was never passed to the loader — unchanged by construction.
        expect(std::memcmp(live.data(), before, sizeof(before)) == 0);

        tempDir.deleteRecursively();
    }

    void writer_m1kmWithoutPolicyFails()
    {
        beginTest("writer_m1kmWithoutPolicyFails");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmWriterNoPolicy");
        const auto target = tempDir.getChildFile("Master 1.m1km");
        expect(studyFixture("Master 1.m1km").copyFileTo(target));

        Core::MasterModel live;
        live.loadFrom(Core::InitDefaults::masterData());
        juce::uint8 before[Core::MasterModel::kBufferSize];
        std::memcpy(before, live.data(), sizeof(before));

        const auto result = Core::InitTemplateWriter::loadMasterFromUserFile(live, target, loader);
        expect(! result.success);
        expectEquals(result.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed));
        expect(std::memcmp(live.data(), before, sizeof(before)) == 0);

        tempDir.deleteRecursively();
    }

    void writer_m1kmMasterSettingsOnlyAppliesPolicy()
    {
        beginTest("writer_m1kmMasterSettingsOnlyAppliesPolicy");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmWriterSettingsOnly");
        const auto target = tempDir.getChildFile("All Groups On.m1km");
        expect(studyFixture("All Groups On.m1km").copyFileTo(target));

        juce::MemoryBlock data;
        expect(target.loadFileAsData(data));
        juce::uint8 decoded[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(Core::MasterM1kmCodec::decodeToPacked(data, decoded));

        Core::MasterModel live;
        const auto result = Core::InitTemplateWriter::loadMasterFromUserFile(
            live, target, loader, Core::MasterM1kmGroupsPolicy::kMasterSettingsOnly);
        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kUserFile);

        const auto* defaults = Core::InitDefaults::masterData();
        expect(std::memcmp(live.data() + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           Core::MasterM1kmLoadPolicy::kGroupsCount)
               == 0);
        expect(std::memcmp(live.data() + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                           Core::MasterM1kmLoadPolicy::kCascadeCount)
               == 0);

        // Non-Groups / non-cascade bytes follow the file.
        expectEquals(static_cast<int>(live.data()[0]), static_cast<int>(decoded[0]));
        expectEquals(static_cast<int>(live.data()[169]), static_cast<int>(decoded[169]));

        tempDir.deleteRecursively();
    }

    void writer_m1kmFullMasterPreservesGroups()
    {
        beginTest("writer_m1kmFullMasterPreservesGroups");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmWriterFull");
        const auto target = tempDir.getChildFile("All Groups On.m1km");
        expect(studyFixture("All Groups On.m1km").copyFileTo(target));

        juce::MemoryBlock data;
        expect(target.loadFileAsData(data));
        juce::uint8 decoded[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(Core::MasterM1kmCodec::decodeToPacked(data, decoded));

        Core::MasterModel live;
        const auto result = Core::InitTemplateWriter::loadMasterFromUserFile(
            live, target, loader, Core::MasterM1kmGroupsPolicy::kFullMaster);
        expect(result.success);
        expect(std::memcmp(live.data(), decoded, Core::MasterM1kmCodec::kInt16Count) == 0);

        tempDir.deleteRecursively();
    }

    void writer_m1kmThenSaveWritesSyxOnly()
    {
        beginTest("writer_m1kmThenSaveWritesSyxOnly");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmSaveAsSyx");
        const auto m1km = tempDir.getChildFile("Master 1.m1km");
        expect(studyFixture("Master 1.m1km").copyFileTo(m1km));

        Core::MasterModel live;
        expect(Core::InitTemplateWriter::loadMasterFromUserFile(
                   live, m1km, loader, Core::MasterM1kmGroupsPolicy::kFullMaster)
                   .success);

        const auto saveTarget = tempDir.getChildFile("Exported.m1km");
        const auto write = Core::InitTemplateWriter::writeMasterToFile(live, saveTarget, encoder);
        expect(write.success);

        const auto syx = tempDir.getChildFile("Exported.syx");
        expect(syx.existsAsFile());
        expect(! saveTarget.existsAsFile() || saveTarget.getFileExtension().equalsIgnoreCase(".syx"));
        expect(m1km.existsAsFile());
        expectEquals(static_cast<int>(m1km.getSize()), static_cast<int>(Core::MasterM1kmCodec::kFileByteSize));

        tempDir.deleteRecursively();
    }

    void decodeUserFile_missingFails()
    {
        beginTest("decodeUserFile_missingFails");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto missing = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("MatrixControlMissingMaster.m1km");
        expect(! missing.existsAsFile());

        Core::MasterModel scratch;
        const auto result = Core::InitTemplateWriter::decodeMasterUserFile(scratch, missing, loader);
        expect(! result.success);
        expectEquals(result.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterFileFailed));
    }

    // Cancel on the Groups/cascade modal = successful decode into a scratch buffer with no commit.
    void decodeWithoutCommit_leavesLiveMasterUnchanged()
    {
        beginTest("decodeWithoutCommit_leavesLiveMasterUnchanged");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        const auto tempDir = createTempDir("MatrixControlM1kmCancelEquivalent");
        const auto target = tempDir.getChildFile("Master 1.m1km");
        expect(studyFixture("Master 1.m1km").copyFileTo(target));

        Core::MasterModel live;
        live.loadFrom(Core::InitDefaults::masterData());
        juce::uint8 before[Core::MasterModel::kBufferSize];
        std::memcpy(before, live.data(), sizeof(before));

        Core::MasterModel scratch;
        const auto result = Core::InitTemplateWriter::decodeMasterUserFile(scratch, target, loader);
        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kUserFile);
        expect(std::memcmp(live.data(), before, sizeof(before)) == 0);
        expect(std::memcmp(scratch.data(), before, sizeof(before)) != 0);

        tempDir.deleteRecursively();
    }

    void helpCopy_mentionsSyxAndM1km()
    {
        beginTest("helpCopy_mentionsSyxAndM1km");

        const juce::String help(PluginDisplayNames::Settings::ContextualHelp::kMasterLoad);
        expect(help.containsIgnoreCase(".syx"));
        expect(help.containsIgnoreCase(".m1km"));
    }
};

static MasterM1kmCodecTests masterM1kmCodecTests;
