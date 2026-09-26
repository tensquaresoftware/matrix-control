#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/PatchModel.h"
#include "PatchFixturePaths.h"
#include "Shared/Definitions/PluginDescriptors.h"

// Unit tests for Core::PatchModel — the 134-byte packed single-patch buffer.
// Round-trip cases use real Matrix-1000 reference patches committed under
// Tests/Fixtures/Matrix-Control/Patches/ (path injected via MATRIX_TEST_FIXTURES_DIR).
class PatchModelTests : public juce::UnitTest
{
public:
    PatchModelTests() : juce::UnitTest("PatchModel Tests") {}

    void runTest() override
    {
        runReferenceRoundTrip();
        runNameCodec();
        runSixBitNameNormalization();
        runSignedFieldCodec();
        runSignedReferenceValidation();
        runChoiceField();
    }

private:
    struct ReferencePatch
    {
        const char* fileName;
        const char* expectedName;
    };

    void runReferenceRoundTrip()
    {
        const ReferencePatch patches[] = {
            { "Patch 5.syx",   "BNK4: 05" },
            { "Patch 66.syx",  "BNK4: 66" },
            { "Patch 71.syx",  "BNK2: 71" },
            { "Patch 808.syx", "BNK4: 40" }
        };

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;

        for (const auto& patch : patches)
        {
            beginTest(juce::String("Round-trip ") + patch.fileName);

            juce::MemoryBlock original;
            const bool loaded = PatchTestFixtures::resolvePatchFixtureFile(patch.fileName).loadFileAsData(original);
            expect(loaded, "Fixture should load from disk");

            juce::uint8 decoded[SysExConstants::kPatchPackedDataSize] = {};
            expect(decoder.decodePatchSysEx(original, decoded), "Decoder should accept the reference patch");

            Core::PatchModel model;
            model.loadFrom(decoded);

            expect(std::memcmp(model.data(), decoded, Core::PatchModel::kBufferSize) == 0,
                   "PatchModel buffer must be byte-identical to the decoded data");

            expectEquals(model.getName(), juce::String(patch.expectedName));

            const auto patchNumber = static_cast<const juce::uint8*>(original.getData())[4];
            const auto reEncoded = encoder.encodePatchSysEx(patchNumber, model.data());
            expect(reEncoded == original, "Re-encoded SysEx must equal the original message");
        }
    }

    void runNameCodec()
    {
        beginTest("Name set/get round-trip and length enforcement");

        Core::PatchModel model;

        model.setName("SYNTHBAS");
        expectEquals(model.getName(), juce::String("SYNTHBAS"));

        model.setName("TOOLONGNAME");
        expectEquals(model.getName(), juce::String("TOOLONGN"));

        model.setName("HI");
        expectEquals(model.getName(), juce::String("HI"));

        // Matrix display charset is uppercase-only: setName folds input to uppercase.
        model.setName("synthbas");
        expectEquals(model.getName(), juce::String("SYNTHBAS"));

        model.setName("Hi There!");
        expectEquals(model.getName(), juce::String("HI THERE"));
    }

    void runSixBitNameNormalization()
    {
        beginTest("6-bit wire name normalizes to setName storage");

        Core::PatchModel model;
        juce::uint8 packed[Core::PatchModel::kBufferSize] = {};

        // Wire-style 6-bit codes for "ABCD" (0x01..0x04 → decode to 'A'..'D').
        packed[0] = 0x01;
        packed[1] = 0x02;
        packed[2] = 0x03;
        packed[3] = 0x04;
        for (int i = 4; i < Core::PatchModel::kNameLength; ++i)
            packed[static_cast<size_t>(i)] = 0x20; // space in either encoding

        model.loadFrom(packed);
        expectEquals(model.getName(), juce::String("ABCD"));
        expect(model.data()[0] == 0x01);

        model.normalizeNameEncoding();
        expectEquals(model.getName(), juce::String("ABCD"));
        expectEquals(static_cast<int>(model.data()[0]), static_cast<int>('A'));
        expectEquals(static_cast<int>(model.data()[1]), static_cast<int>('B'));
        expectEquals(static_cast<int>(model.data()[2]), static_cast<int>('C'));
        expectEquals(static_cast<int>(model.data()[3]), static_cast<int>('D'));
    }

    void runSignedFieldCodec()
    {
        beginTest("Signed field two's-complement round-trip");

        Core::PatchModel model;

        // Signed fields are stored as 8-bit two's complement (synth sign-extends bit 6
        // into bit 7), so negative values set bit 7 — see byte-value assertions below.
        const PluginDescriptors::IntParameterDescriptor signed7 {
            .minValue = -63, .maxValue = 63, .sysExOffset = 86
        };
        model.setValue(signed7, -63);
        expectEquals(model.getValue(signed7), -63);
        expectEquals(static_cast<int>(model.data()[86]), 0xC1);
        model.setValue(signed7, 63);
        expectEquals(model.getValue(signed7), 63);
        model.setValue(signed7, -1);
        expectEquals(model.getValue(signed7), -1);
        expectEquals(static_cast<int>(model.data()[86]), 0xFF);

        const PluginDescriptors::IntParameterDescriptor signed6 {
            .minValue = -31, .maxValue = 31, .sysExOffset = 19
        };
        model.setValue(signed6, -31);
        expectEquals(model.getValue(signed6), -31);
        expectEquals(static_cast<int>(model.data()[19]), 0xE1);
        model.setValue(signed6, 31);
        expectEquals(model.getValue(signed6), 31);

        const PluginDescriptors::IntParameterDescriptor unsigned7 {
            .minValue = 0, .maxValue = 127, .sysExOffset = 26
        };
        model.setValue(unsigned7, 127);
        expectEquals(model.getValue(unsigned7), 127);
        expectEquals(static_cast<int>(model.data()[26]), 127);
    }

    void runSignedReferenceValidation()
    {
        // Patch 71 (from the Oberheim Max-for-Live capture) carries a real negative
        // signed value: byte 90 = 212 (VCF Frequency by Env 1 Amount, param 22) = -44.
        beginTest("Signed field decode validated against reference Patch 71");

        SysExParser parser;
        SysExDecoder decoder(parser);

        juce::MemoryBlock original;
        expect(PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx").loadFileAsData(original),
               "Patch 71 fixture should load");

        juce::uint8 decoded[SysExConstants::kPatchPackedDataSize] = {};
        expect(decoder.decodePatchSysEx(original, decoded), "Patch 71 should decode");

        Core::PatchModel model;
        model.loadFrom(decoded);

        const PluginDescriptors::IntParameterDescriptor vcfFreqByEnv1 {
            .minValue = -63, .maxValue = 63, .sysExOffset = 90
        };
        expectEquals(static_cast<int>(model.data()[90]), 212);
        expectEquals(model.getValue(vcfFreqByEnv1), -44);

        model.setValue(vcfFreqByEnv1, -44);
        expectEquals(static_cast<int>(model.data()[90]), 212);
    }

    void runChoiceField()
    {
        beginTest("Choice index store/clamp");

        Core::PatchModel model;
        const PluginDescriptors::ChoiceParameterDescriptor choice {
            .choices = { "A", "B", "C", "D" }, .sysExOffset = 8
        };

        model.setChoiceIndex(choice, 2);
        expectEquals(model.getChoiceIndex(choice), 2);
        expectEquals(static_cast<int>(model.data()[8]), 2);

        model.setChoiceIndex(choice, 99);
        expectEquals(model.getChoiceIndex(choice), 3);
    }
};

static PatchModelTests patchModelTests;
