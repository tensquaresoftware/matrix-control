#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Init/InitDefaults.h"
#include "PatchFixturePaths.h"

/**
 * Unit tests for SysExParser
 *
 * Run with: ./Matrix-Control_Standalone --unit-tests
 * Or integrate with JUCE UnitTest framework
 */
class SysExParserTests : public juce::UnitTest
{
public:
    SysExParserTests() : juce::UnitTest("SysExParser Tests") {}

    void runTest() override
    {
        testValidPatchStructure();
        testEditBufferPatchValidation();
        testThirdPartyOpcode01PatchValidation();
        testMissingEndByte();
        testMissingStartByte();
        testWrongManufacturer();
        testDeviceIdMessage();
        testMasterLengthMatrix1000VsMatrix6();
    }

private:
    static void appendByte(juce::MemoryBlock& block, juce::uint8 value)
    {
        block.append(&value, 1);
    }

    void testValidPatchStructure()
    {
        beginTest("Valid patch SysEx structure");

        // Minimal patch header: F0 10 06 01 05 — incomplete payload must fail structure checks.
        juce::MemoryBlock validPatch;
        appendByte(validPatch, SysExConstants::kSysExStart);
        appendByte(validPatch, SysExConstants::kManufacturerIdOberheim);
        appendByte(validPatch, SysExConstants::kDeviceIdMatrix1000);
        appendByte(validPatch, SysExConstants::Opcode::kSinglePatchData);
        appendByte(validPatch, 5);

        SysExParser parser;
        expect(!parser.validateStructure(validPatch),
               "Incomplete message should fail structure validation");
    }

    void testEditBufferPatchValidation()
    {
        beginTest("Edit-buffer patch SysEx (0x0D) validates as patch");

        SysExEncoder encoder;
        const auto message = encoder.encodePatchToEditBufferSysEx(Core::InitDefaults::patchData());

        SysExParser parser;
        const auto result = parser.validateSysEx(message);
        expect(result.isValid, "0x0D patch message should pass validation");
        expect(result.messageType == SysExParser::MessageType::kPatch);

        const auto* data = static_cast<const juce::uint8*>(message.getData());
        expectEquals(static_cast<int>(data[3]),
                     static_cast<int>(SysExConstants::Opcode::kSinglePatchToEditBuffer));
        expectEquals(static_cast<int>(data[4]), 0);
    }

    void testThirdPartyOpcode01PatchValidation()
    {
        beginTest("Third-party patch SysEx (0x01) validates as patch");

        const auto fixture = PatchTestFixtures::resolvePatchFixtureFile("Patch 71.syx");
        juce::MemoryBlock message;
        expect(fixture.loadFileAsData(message), "Patch 71 fixture should load");

        SysExParser parser;
        const auto result = parser.validateSysEx(message);
        expect(result.isValid, "0x01 patch message should pass validation");
        expect(result.messageType == SysExParser::MessageType::kPatch);

        const auto* data = static_cast<const juce::uint8*>(message.getData());
        expectEquals(static_cast<int>(data[3]),
                     static_cast<int>(SysExConstants::Opcode::kSinglePatchData));
    }

    void testMissingEndByte()
    {
        beginTest("Invalid SysEx structure (missing F7)");

        juce::MemoryBlock invalidSysEx;
        appendByte(invalidSysEx, SysExConstants::kSysExStart);
        appendByte(invalidSysEx, SysExConstants::kManufacturerIdOberheim);
        appendByte(invalidSysEx, SysExConstants::kDeviceIdMatrix1000);

        SysExParser parser;
        expect(!parser.validateStructure(invalidSysEx),
               "Message without F7 should fail structure validation");
    }

    void testMissingStartByte()
    {
        beginTest("Invalid SysEx structure (missing F0)");

        juce::MemoryBlock invalidSysEx;
        appendByte(invalidSysEx, SysExConstants::kManufacturerIdOberheim);
        appendByte(invalidSysEx, SysExConstants::kDeviceIdMatrix1000);
        appendByte(invalidSysEx, SysExConstants::kSysExEnd);

        SysExParser parser;
        expect(!parser.validateStructure(invalidSysEx),
               "Message without F0 should fail structure validation");
    }

    void testWrongManufacturer()
    {
        beginTest("Oberheim manufacturer ID validation");

        juce::MemoryBlock wrongManufacturer;
        appendByte(wrongManufacturer, SysExConstants::kSysExStart);
        appendByte(wrongManufacturer, 0x20);  // Wrong manufacturer
        appendByte(wrongManufacturer, SysExConstants::kDeviceIdMatrix1000);
        appendByte(wrongManufacturer, SysExConstants::Opcode::kSinglePatchData);
        appendByte(wrongManufacturer, 5);
        appendByte(wrongManufacturer, SysExConstants::kSysExEnd);

        SysExParser parser;
        const auto result = parser.validateSysEx(wrongManufacturer);
        expect(!result.isValid, "Wrong manufacturer should fail validation");
        expect(result.errorMessage.contains("Manufacturer")
                   || result.errorMessage.contains("Device"),
               "Error should mention manufacturer or device");
    }

    void testDeviceIdMessage()
    {
        beginTest("Device ID message validation");

        // Device ID response: F0 7E <chan> 06 02 10 06 00 02 00 <rev...> F7
        juce::MemoryBlock deviceId;
        appendByte(deviceId, SysExConstants::kSysExStart);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kUniversalNonRealtimeId);
        appendByte(deviceId, 0x7F);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kSubIdGeneralInfo);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kSubIdDeviceIdReply);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kExpectedManufacturer);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kExpectedFamily);
        appendByte(deviceId, 0x00);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kExpectedMemberLow);
        appendByte(deviceId, SysExConstants::DeviceInquiry::kExpectedMemberHigh);
        const juce::uint8 revision[4] = {0x20, 0x31, 0x31, 0x30};  // " 1.10"
        deviceId.append(revision, 4);
        appendByte(deviceId, SysExConstants::kSysExEnd);

        SysExParser parser;
        const auto result = parser.validateMessageType(deviceId);
        expect(result.isValid, "Valid Device ID message should pass validation");
        expect(result.messageType == SysExParser::MessageType::kDeviceId,
               "Message type should be DeviceId");
    }

    void testMasterLengthMatrix1000VsMatrix6()
    {
        beginTest("Master length — M-1000 351 still type-valid; Matrix-6 478 fails M-1000 length");

        auto makeMasterFrame = [](size_t length)
        {
            juce::MemoryBlock block(length, true);
            auto* data = static_cast<juce::uint8*>(block.getData());
            data[0] = SysExConstants::kSysExStart;
            data[1] = SysExConstants::kManufacturerIdOberheim;
            data[2] = SysExConstants::kDeviceIdMatrix1000;
            data[3] = SysExConstants::Opcode::kMasterParameterData;
            data[length - 1] = SysExConstants::kSysExEnd;
            return block;
        };

        SysExParser parser;

        const auto m1000 = makeMasterFrame(SysExConstants::kMasterMessageLength);
        const auto m1000Type = parser.validateMessageType(m1000);
        expect(m1000Type.isValid, "Matrix-1000 Master length 351 must remain type-valid");
        expect(m1000Type.messageType == SysExParser::MessageType::kMaster);

        const auto matrix6Min = makeMasterFrame(SysExConstants::kMatrix6MasterMessageLengthMin);
        const auto matrix6MinType = parser.validateMessageType(matrix6Min);
        expect(! matrix6MinType.isValid,
               "Matrix-6 Master 478 must not pass Matrix-1000 length validation");
        expect(matrix6MinType.errorMessage.contains("Invalid message length"));

        const auto matrix6Max = makeMasterFrame(SysExConstants::kMatrix6MasterMessageLengthMax);
        const auto matrix6MaxType = parser.validateMessageType(matrix6Max);
        expect(! matrix6MaxType.isValid,
               "Matrix-6 Master 479 must not pass Matrix-1000 length validation");
        expect(matrix6MaxType.errorMessage.contains("Invalid message length"));
    }
};

static SysExParserTests sysExParserTests;
