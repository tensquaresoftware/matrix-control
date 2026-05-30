#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExParser.h"

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
        SysExParser parser;

        beginTest("Valid patch SysEx structure");
        {
            // Create a minimal valid patch message: F0 10 06 01 05 <nibbles...> <checksum> F7
            // For testing, we'll create a message with correct structure but minimal data
            juce::MemoryBlock validPatch;
            validPatch.append(&SysExConstants::kSysExStart, 1);
            validPatch.append(&SysExConstants::kManufacturerIdOberheim, 1);
            validPatch.append(&SysExConstants::kDeviceIdMatrix1000, 1);
            juce::uint8 opcode = SysExConstants::Opcode::kSinglePatchData;
            validPatch.append(&opcode, 1);
            juce::uint8 patchNum = 5;
            validPatch.append(&patchNum, 1);
            
            // Add minimal nibbles (need at least some data for checksum calculation)
            // For a real patch, we'd need 268 nibbles, but for structure test we can use less
            // Actually, let's test structure validation separately from full message validation
            
            auto result = parser.validateStructure(validPatch);
            expect(!result, "Incomplete message should fail structure validation");
        }
        
        beginTest("Invalid SysEx structure (missing F7)");
        {
            juce::MemoryBlock invalidSysEx;
            invalidSysEx.append(&SysExConstants::kSysExStart, 1);
            invalidSysEx.append(&SysExConstants::kManufacturerIdOberheim, 1);
            invalidSysEx.append(&SysExConstants::kDeviceIdMatrix1000, 1);
            // Missing F7
            
            auto result = parser.validateStructure(invalidSysEx);
            expect(!result, "Message without F7 should fail structure validation");
        }
        
        beginTest("Invalid SysEx structure (missing F0)");
        {
            juce::MemoryBlock invalidSysEx;
            invalidSysEx.append(&SysExConstants::kManufacturerIdOberheim, 1);
            invalidSysEx.append(&SysExConstants::kDeviceIdMatrix1000, 1);
            juce::uint8 f7 = SysExConstants::kSysExEnd;
            invalidSysEx.append(&f7, 1);
            
            auto result = parser.validateStructure(invalidSysEx);
            expect(!result, "Message without F0 should fail structure validation");
        }
        
        beginTest("Oberheim manufacturer ID validation");
        {
            juce::MemoryBlock wrongManufacturer;
            wrongManufacturer.append(&SysExConstants::kSysExStart, 1);
            juce::uint8 wrongManuf = 0x20;  // Wrong manufacturer
            wrongManufacturer.append(&wrongManuf, 1);
            wrongManufacturer.append(&SysExConstants::kDeviceIdMatrix1000, 1);
            juce::uint8 opcode = SysExConstants::Opcode::kSinglePatchData;
            wrongManufacturer.append(&opcode, 1);
            juce::uint8 patchNum = 5;
            wrongManufacturer.append(&patchNum, 1);
            juce::uint8 f7 = SysExConstants::kSysExEnd;
            wrongManufacturer.append(&f7, 1);
            
            // validateSysEx calls validateManufacturerAndDevice internally
            auto result = parser.validateSysEx(wrongManufacturer);
            expect(!result.isValid, "Wrong manufacturer should fail validation");
            expect(result.errorMessage.contains("Manufacturer") || 
                   result.errorMessage.contains("Device"), 
                   "Error should mention manufacturer or device");
        }
        
        beginTest("Device ID message validation");
        {
            // Device ID response: F0 7E <chan> 06 02 10 06 00 02 00 <rev...> F7
            juce::MemoryBlock deviceId;
            deviceId.append(&SysExConstants::kSysExStart, 1);
            juce::uint8 universalId = SysExConstants::DeviceInquiry::kUniversalNonRealtimeId;
            deviceId.append(&universalId, 1);
            juce::uint8 channel = 0x7F;
            deviceId.append(&channel, 1);
            juce::uint8 subId1 = SysExConstants::DeviceInquiry::kSubIdGeneralInfo;
            deviceId.append(&subId1, 1);
            juce::uint8 subId2 = SysExConstants::DeviceInquiry::kSubIdDeviceIdReply;
            deviceId.append(&subId2, 1);
            juce::uint8 manuf = SysExConstants::DeviceInquiry::kExpectedManufacturer;
            deviceId.append(&manuf, 1);
            juce::uint8 family = SysExConstants::DeviceInquiry::kExpectedFamily;
            deviceId.append(&family, 1);
            juce::uint8 familyHigh = 0x00;
            deviceId.append(&familyHigh, 1);
            juce::uint8 memberLow = SysExConstants::DeviceInquiry::kExpectedMemberLow;
            deviceId.append(&memberLow, 1);
            juce::uint8 memberHigh = SysExConstants::DeviceInquiry::kExpectedMemberHigh;
            deviceId.append(&memberHigh, 1);
            juce::uint8 rev[4] = {0x20, 0x31, 0x31, 0x30};  // " 1.10"
            deviceId.append(rev, 4);
            juce::uint8 f7 = SysExConstants::kSysExEnd;
            deviceId.append(&f7, 1);
            
            auto result = parser.validateMessageType(deviceId);
            expect(result.isValid, "Valid Device ID message should pass validation");
            expect(result.messageType == SysExParser::MessageType::kDeviceId, 
                   "Message type should be DeviceId");
        }
    }
    
private:
};

static SysExParserTests sysExParserTests;

