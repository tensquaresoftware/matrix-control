#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Init/InitDefaults.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

/**
 * Unit tests for SysExEncoder
 */
class SysExEncoderTests : public juce::UnitTest
{
public:
    SysExEncoderTests() : juce::UnitTest("SysExEncoder Tests") {}
    
    void runTest() override
    {
        SysExEncoder encoder;

        beginTest("Device Inquiry message encoding");
        {
            auto inquiry = SysExEncoder::encodeDeviceInquiry();
            expect(inquiry.getSize() == SysExConstants::DeviceInquiry::kRequestMessageLength,
                   "Device Inquiry should have correct length");
            
            const auto* data = static_cast<const juce::uint8*>(inquiry.getData());
            for (size_t i = 0; i < SysExConstants::DeviceInquiry::kRequestMessageLength; ++i)
            {
                expect(data[i] == SysExConstants::DeviceInquiry::kRequestMessage[i],
                       juce::String("Device Inquiry byte ") + juce::String(i) + " should match");
            }
        }

        beginTest("Device Inquiry reply encoding — Matrix-1000 golden bytes");
        {
            const juce::uint8 expected[] = {
                0xF0, 0x7E, 0x00, 0x06, 0x02,
                0x10, 0x06, 0x00, 0x02, 0x00,
                '1', '.', '1', '1',
                0xF7
            };
            const auto reply = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh);

            expectEquals(static_cast<int>(reply.getSize()), static_cast<int>(sizeof(expected)));
            const auto* data = static_cast<const juce::uint8*>(reply.getData());
            for (size_t i = 0; i < sizeof(expected); ++i)
                expect(data[i] == expected[i],
                       juce::String("M-1000 reply byte ") + juce::String(static_cast<int>(i)));
        }

        beginTest("Device Inquiry reply encoding — provisional Matrix-6 golden bytes");
        {
            const juce::uint8 expected[] = {
                0xF0, 0x7E, 0x00, 0x06, 0x02,
                0x10, 0x06, 0x00, 0x01, 0x00,
                '1', '.', '1', '1',
                0xF7
            };
            const auto reply = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kMatrix6MemberLow,
                SysExConstants::DeviceInquiry::kMatrix6MemberHigh);

            expectEquals(static_cast<int>(reply.getSize()), static_cast<int>(sizeof(expected)));
            const auto* data = static_cast<const juce::uint8*>(reply.getData());
            for (size_t i = 0; i < sizeof(expected); ++i)
                expect(data[i] == expected[i],
                       juce::String("M-6 reply byte ") + juce::String(static_cast<int>(i)));
        }

        beginTest("Device Inquiry reply — round-trip decode + registry");
        {
            SysExParser parser;
            SysExDecoder decoder(parser);

            const auto m1000 = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh);
            const auto m1000Info = decoder.decodeDeviceId(m1000);
            expect(m1000Info.isValid);
            expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(m1000Info)),
                         static_cast<int>(MatrixDeviceTypes::Type::kMatrix1000));
            expectEquals(m1000Info.version, juce::String("1.11"));

            const auto m6 = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kMatrix6MemberLow,
                SysExConstants::DeviceInquiry::kMatrix6MemberHigh);
            const auto m6Info = decoder.decodeDeviceId(m6);
            expect(m6Info.isValid);
            expectEquals(static_cast<int>(Core::DeviceTypeRegistry::fromDeviceInquiry(m6Info)),
                         static_cast<int>(MatrixDeviceTypes::Type::kMatrix6));
        }

        beginTest("Device Inquiry reply — version padded/truncated to 4 ASCII chars");
        {
            const auto shortVer = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh,
                "1.1");
            const auto* shortData = static_cast<const juce::uint8*>(shortVer.getData());
            expect(shortData[10] == '1');
            expect(shortData[11] == '.');
            expect(shortData[12] == '1');
            expect(shortData[13] == ' ');

            const auto longVer = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh,
                "12.34x");
            const auto* longData = static_cast<const juce::uint8*>(longVer.getData());
            expect(longData[10] == '1');
            expect(longData[11] == '2');
            expect(longData[12] == '.');
            expect(longData[13] == '3');

            const auto emptyVer = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh,
                "");
            const auto* emptyData = static_cast<const juce::uint8*>(emptyVer.getData());
            expect(emptyData[10] == '1');
            expect(emptyData[11] == '.');
            expect(emptyData[12] == '1');
            expect(emptyData[13] == '1');

            juce::String highBit;
            highBit << juce::juce_wchar(0xE9); // 'é' — must not emit a data byte >= 0x80
            const auto masked = SysExEncoder::encodeDeviceInquiryReply(
                SysExConstants::DeviceInquiry::kExpectedMemberLow,
                SysExConstants::DeviceInquiry::kExpectedMemberHigh,
                highBit);
            const auto* maskedData = static_cast<const juce::uint8*>(masked.getData());
            expect(maskedData[10] <= 0x7F);
            expect(maskedData[11] == ' ');
            expect(maskedData[12] == ' ');
            expect(maskedData[13] == ' ');
        }
        
        beginTest("Request message encoding");
        {
            auto request = encoder.encodeRequestMessage(
                SysExConstants::RequestType::kRequestEditBuffer, 0);
            
            expect(request.getSize() >= 7, "Request message should have minimum length");
            
            const auto* data = static_cast<const juce::uint8*>(request.getData());
            expect(data[0] == SysExConstants::kSysExStart, "Should start with F0");
            expect(data[1] == SysExConstants::kManufacturerIdOberheim, "Should have Oberheim ID");
            expect(data[2] == SysExConstants::kDeviceIdMatrix1000, "Should have Matrix-1000 ID");
            expect(data[3] == SysExConstants::Opcode::kRequestData, "Should have request opcode");
            expect(data[4] == SysExConstants::RequestType::kRequestEditBuffer, 
                   "Should have edit buffer request type");
            expect(data[request.getSize() - 1] == SysExConstants::kSysExEnd, "Should end with F7");
        }
        
        beginTest("Checksum calculation");
        {
            juce::uint8 testData[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
            juce::uint8 checksum = SysExEncoder::calculateChecksum(testData, 5);
            expect(checksum <= 0x7F, "Checksum should be 7-bit");
            expect(checksum == 0x0F, "Checksum should be sum & 0x7F (1+2+3+4+5=15=0x0F)");
        }
        
        beginTest("Remote Parameter Edit (0x06) encoding");
        {
            const juce::uint8 param = 12;
            const juce::uint8 value = 37;
            auto message = encoder.encodeRemoteParameterEdit(param, value);

            expectEquals(static_cast<int>(message.getSize()), 7);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[0] == SysExConstants::kSysExStart);
            expect(data[1] == SysExConstants::kManufacturerIdOberheim);
            expect(data[2] == SysExConstants::kDeviceIdMatrix1000);
            expect(data[3] == SysExConstants::Opcode::kRemoteParameterEdit);
            expect(data[4] == param);
            expect(data[5] == value);
            expect(data[6] == SysExConstants::kSysExEnd);
        }

        beginTest("Remote Parameter Edit (0x06) — parameter number masked to 7 bits");
        {
            const juce::uint8 paramWithHighBit = 200;
            auto message = encoder.encodeRemoteParameterEdit(paramWithHighBit, 0);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[4] == static_cast<juce::uint8>(paramWithHighBit & 0x7F));
        }

        beginTest("Remote Parameter Edit (0x06) — packed value above 127 passed through");
        {
            const juce::uint8 signedPackedByte = 251;
            auto message = encoder.encodeRemoteParameterEdit(1, signedPackedByte);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[5] == signedPackedByte);
        }

        beginTest("Matrix Mod Bus Edit (0x0B) encoding");
        {
            const juce::uint8 bus = 3;
            const juce::uint8 source = 5;
            const juce::uint8 amount = 43;
            const juce::uint8 destination = 12;
            auto message = encoder.encodeMatrixModBusEdit(bus, source, amount, destination);

            expectEquals(static_cast<int>(message.getSize()),
                         static_cast<int>(SysExConstants::kMatrixModBusMessageLength));

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[0] == SysExConstants::kSysExStart);
            expect(data[1] == SysExConstants::kManufacturerIdOberheim);
            expect(data[2] == SysExConstants::kDeviceIdMatrix1000);
            expect(data[3] == SysExConstants::Opcode::kRemoteParameterEditMatrix);
            expect(data[4] == bus);
            expect(data[5] == source);
            expect(data[6] == amount);
            expect(data[7] == destination);
            expect(data[8] == SysExConstants::kSysExEnd);
        }

        beginTest("Matrix Mod Bus Edit (0x0B) — signed amount above 127 passed through");
        {
            const juce::uint8 signedPackedByte = 251;
            auto message = encoder.encodeMatrixModBusEdit(0, 1, signedPackedByte, 2);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[6] == signedPackedByte);
        }

        beginTest("Set Bank (0x0A) encoding");
        {
            auto message = encoder.encodeSetBank(7);
            expectEquals(static_cast<int>(message.getSize()), 6);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[0] == SysExConstants::kSysExStart);
            expect(data[3] == SysExConstants::Opcode::kSetBank);
            expect(data[4] == 7);
            expect(data[5] == SysExConstants::kSysExEnd);
        }

        beginTest("Unlock Bank (0x0C) encoding");
        {
            auto message = SysExEncoder::encodeUnlockBank();
            expectEquals(static_cast<int>(message.getSize()), 5);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[0] == SysExConstants::kSysExStart);
            expect(data[3] == SysExConstants::Opcode::kUnlockBank);
            expect(data[4] == SysExConstants::kSysExEnd);
        }

        beginTest("Unpack bytes to nibbles");
        {
            juce::uint8 packedBytes[] = { 0x12, 0x34, 0x56 };
            juce::uint8 nibbles[6];
            size_t numNibbles = SysExEncoder::unpackBytes(packedBytes, 3, nibbles);
            
            expect(numNibbles == 6, "Should produce 6 nibbles from 3 bytes");
            expect(nibbles[0] == 0x02, "Low nibble of 0x12 should be 0x02");
            expect(nibbles[1] == 0x01, "High nibble of 0x12 should be 0x01");
            expect(nibbles[2] == 0x04, "Low nibble of 0x34 should be 0x04");
            expect(nibbles[3] == 0x03, "High nibble of 0x34 should be 0x03");
        }

        beginTest("Single Patch to Edit Buffer (0x0D) encoding");
        {
            const auto* packedData = Core::InitDefaults::patchData();
            auto message = encoder.encodePatchToEditBufferSysEx(packedData);

            expectEquals(static_cast<int>(message.getSize()),
                         static_cast<int>(SysExConstants::kPatchToEditBufferMessageLength));
            expectEquals(static_cast<int>(message.getSize()), 275);

            const auto* data = static_cast<const juce::uint8*>(message.getData());
            expect(data[0] == SysExConstants::kSysExStart);
            expect(data[1] == SysExConstants::kManufacturerIdOberheim);
            expect(data[2] == SysExConstants::kDeviceIdMatrix1000);
            expect(data[3] == SysExConstants::Opcode::kSinglePatchToEditBuffer);
            expect(data[4] == 0x00); // literal header byte required by Oberheim 0DH format
            expect(data[message.getSize() - 2]
                   == SysExEncoder::calculateChecksum(packedData, SysExConstants::kPatchPackedDataSize));
            expect(data[message.getSize() - 1] == SysExConstants::kSysExEnd);
        }
    }
};

static SysExEncoderTests sysExEncoderTests;

