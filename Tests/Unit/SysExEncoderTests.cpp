#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"

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
    }
};

static SysExEncoderTests sysExEncoderTests;

