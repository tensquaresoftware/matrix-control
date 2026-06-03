#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/MasterModel.h"
#include "Shared/Definitions/PluginDescriptors.h"

// Unit tests for Core::MasterModel — the 172-byte packed master ("Global Parameters")
// buffer. The reference round-trip uses the real Matrix-1000 master dump committed under
// Tests/Fixtures/Masters/ (path injected via MATRIX_TEST_FIXTURES_DIR). Only one master
// dump exists and it carries default (zero) signed values, so signed-field encoding is
// validated separately with synthetic descriptors and explicit on-wire byte assertions.
class MasterModelTests : public juce::UnitTest
{
public:
    MasterModelTests() : juce::UnitTest("MasterModel Tests") {}

    void runTest() override
    {
        runReferenceRoundTrip();
        runSignedFieldCodec();
        runChoiceField();
    }

private:
    static juce::File fixturesMastersDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Masters");
    }

    void runReferenceRoundTrip()
    {
        beginTest("Round-trip Master 1.syx");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;

        juce::MemoryBlock original;
        const bool loaded = fixturesMastersDir().getChildFile("Master 1.syx").loadFileAsData(original);
        expect(loaded, "Fixture should load from disk");
        expectEquals(static_cast<int>(original.getSize()),
                     static_cast<int>(SysExConstants::kMasterMessageLength));

        juce::uint8 decoded[SysExConstants::kMasterPackedDataSize] = {};
        expect(decoder.decodeMasterSysEx(original, decoded), "Decoder should accept the reference master");

        Core::MasterModel model;
        model.loadFrom(decoded);

        expect(std::memcmp(model.data(), decoded, Core::MasterModel::kBufferSize) == 0,
               "MasterModel buffer must be byte-identical to the decoded data");

        const auto version = static_cast<const juce::uint8*>(original.getData())[4];
        const auto reEncoded = encoder.encodeMasterSysEx(version, model.data());
        expect(reEncoded == original, "Re-encoded SysEx must equal the original message (checksum included)");
    }

    void runSignedFieldCodec()
    {
        beginTest("Signed master field two's-complement round-trip");

        Core::MasterModel model;

        // Master Tune: 6-bit signed, byte 8 (range +/-31). Negative values set bit 7.
        const PluginDescriptors::IntParameterDescriptor masterTune {
            .minValue = -31, .maxValue = 31, .sysExOffset = 8
        };
        model.setValue(masterTune, -31);
        expectEquals(model.getValue(masterTune), -31);
        expectEquals(static_cast<int>(model.data()[8]), 0xE1);
        model.setValue(masterTune, 31);
        expectEquals(model.getValue(masterTune), 31);
        model.setValue(masterTune, -1);
        expectEquals(model.getValue(masterTune), -1);
        expectEquals(static_cast<int>(model.data()[8]), 0xFF);

        // Vibrato Speed Mod Amount: 7-bit signed, byte 3 (range +/-63).
        const PluginDescriptors::IntParameterDescriptor speedModAmount {
            .minValue = -63, .maxValue = 63, .sysExOffset = 3
        };
        model.setValue(speedModAmount, -63);
        expectEquals(model.getValue(speedModAmount), -63);
        expectEquals(static_cast<int>(model.data()[3]), 0xC1);
        model.setValue(speedModAmount, 63);
        expectEquals(model.getValue(speedModAmount), 63);

        // Unsigned field: Vibrato Speed, byte 1 (range 0..63).
        const PluginDescriptors::IntParameterDescriptor vibratoSpeed {
            .minValue = 0, .maxValue = 63, .sysExOffset = 1
        };
        model.setValue(vibratoSpeed, 63);
        expectEquals(model.getValue(vibratoSpeed), 63);
        expectEquals(static_cast<int>(model.data()[1]), 63);
    }

    void runChoiceField()
    {
        beginTest("Choice index store/clamp");

        Core::MasterModel model;
        const PluginDescriptors::ChoiceParameterDescriptor onOff {
            .choices = { "OFF", "ON" }, .sysExOffset = 32
        };

        model.setChoiceIndex(onOff, 1);
        expectEquals(model.getChoiceIndex(onOff), 1);
        expectEquals(static_cast<int>(model.data()[32]), 1);

        model.setChoiceIndex(onOff, 99);
        expectEquals(model.getChoiceIndex(onOff), 1);
    }
};

static MasterModelTests masterModelTests;
