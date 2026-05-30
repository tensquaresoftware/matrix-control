#include <vector>

#include "SysExParser.h"

#include "Core/Loggers/MidiLogger.h"
#include "SysExConstants.h"

SysExParser::ValidationResult SysExParser::validateSysEx(const juce::MemoryBlock& sysEx) const
{
    if (!validateStructure(sysEx))
    {
        MidiLogger::getInstance().logError("Invalid SysEx structure: missing F0 or F7");
        return ValidationResult::failure("Invalid SysEx structure: missing F0 or F7");
    }

    if (!validateManufacturerAndDevice(sysEx))
    {
        MidiLogger::getInstance().logError("Invalid Manufacturer ID or Device ID");
        return ValidationResult::failure("Invalid Manufacturer ID or Device ID");
    }

    auto typeResult = validateMessageType(sysEx);
    if (!typeResult.isValid)
    {
        MidiLogger::getInstance().logError("SysEx validation failed! " + typeResult.errorMessage);
        return typeResult;
    }

    if (!validateChecksum(sysEx))
    {
        MidiLogger::getInstance().logError("Invalid SysEx checksum");
        return ValidationResult::failure("Invalid checksum");
    }

    return ValidationResult::success(typeResult.messageType);
}

bool SysExParser::validateStructure(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < SysExConstants::kMinSysExLength)
    {
        return false;
    }

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    
    if (data[0] != SysExConstants::kSysExStart)
    {
        return false;
    }

    if (data[sysEx.getSize() - 1] != SysExConstants::kSysExEnd)
    {
        return false;
    }

    return true;
}

bool SysExParser::validateChecksum(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < SysExConstants::kMinSysExLength)
    {
        return false;
    }

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    
    if (isDeviceInquiryMessage(data))
    {
        return true;
    }

    size_t dataStartIndex = 5;
    size_t checksumIndex = sysEx.getSize() - 2;
    
    if (sysEx.getSize() <= dataStartIndex + 2)
    {
        return false;
    }

    juce::uint8 receivedChecksum = data[checksumIndex];
    size_t numNibbles = checksumIndex - dataStartIndex;
    
    if (numNibbles == 0 || (numNibbles % 2 != 0))
    {
        return false;
    }

    std::vector<juce::uint8> packedData = packNibblesToBytes(&data[dataStartIndex], numNibbles);
    juce::uint8 calculatedChecksum = calculateChecksum(packedData.data(), packedData.size());

    return (calculatedChecksum & 0x7F) == receivedChecksum;
}

SysExParser::ValidationResult SysExParser::validateMessageType(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < 4)
    {
        return ValidationResult::failure("Message too short");
    }

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());

    if (isDeviceIdResponse(data))
    {
        if (sysEx.getSize() < 15)
        {
            return ValidationResult::failure("Device ID message too short");
        }
        return ValidationResult::success(MessageType::kDeviceId);
    }

    if (!isOberheimMatrix1000Message(data))
    {
        return ValidationResult::failure("Not an Oberheim Matrix-1000 message");
    }

    juce::uint8 opcode = data[3];
    MessageType messageType = getMessageTypeFromOpcode(opcode);

    if (messageType == MessageType::kUnknown)
    {
        return ValidationResult::failure("Unknown message type");
    }

    size_t expectedLength = getExpectedMessageLength(messageType);
    if (sysEx.getSize() != expectedLength)
    {
        return ValidationResult::failure(
            juce::String("Invalid message length: expected ") + juce::String(expectedLength) +
            ", got " + juce::String(sysEx.getSize()));
    }

    return ValidationResult::success(messageType);
}

juce::uint8 SysExParser::calculateChecksum(const juce::uint8* data, size_t length)
{
    juce::uint8 checksum = 0;
    for (size_t i = 0; i < length; ++i)
    {
        checksum += data[i];
    }
    return checksum & 0x7F;
}

size_t SysExParser::packNibbles(const juce::uint8* nibbles, size_t numNibbles, juce::uint8* output)
{
    size_t numBytes = numNibbles / 2;
    for (size_t i = 0; i < numBytes; ++i)
    {
        // Low nibble first, then high nibble (Oberheim format)
        juce::uint8 lowNibble = nibbles[i * 2] & 0x0F;
        juce::uint8 highNibble = nibbles[i * 2 + 1] & 0x0F;
        output[i] = static_cast<juce::uint8>(lowNibble | (highNibble << 4));
    }
    return numBytes;
}

bool SysExParser::validateManufacturerAndDevice(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < 4)
    {
        return false;
    }

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());

    if (isDeviceInquiryMessage(data))
    {
        return true;
    }

    if (data[1] != SysExConstants::kManufacturerIdOberheim)
    {
        return false;
    }

    if (data[2] != SysExConstants::kDeviceIdMatrix1000)
    {
        return false;
    }

    return true;
}

SysExParser::MessageType SysExParser::getMessageTypeFromOpcode(juce::uint8 opcode)
{
    switch (opcode)
    {
        case SysExConstants::Opcode::kSinglePatchData:
            return MessageType::kPatch;
        case SysExConstants::Opcode::kMasterParameterData:
            return MessageType::kMaster;
        case SysExConstants::Opcode::kSplitPatchData:
            return MessageType::kSplitPatch;
        default:
            return MessageType::kUnknown;
    }
}

bool SysExParser::isDeviceInquiryMessage(const juce::uint8* data)
{
    return data[0] == SysExConstants::kSysExStart &&
           data[1] == SysExConstants::DeviceInquiry::kUniversalNonRealtimeId;
}

std::vector<juce::uint8> SysExParser::packNibblesToBytes(const juce::uint8* nibbles, size_t numNibbles)
{
    size_t numBytes = numNibbles / 2;
    std::vector<juce::uint8> packedData(numBytes);
    packNibbles(nibbles, numNibbles, packedData.data());
    return packedData;
}

bool SysExParser::isDeviceIdResponse(const juce::uint8* data)
{
    return data[0] == SysExConstants::kSysExStart &&
           data[1] == SysExConstants::DeviceInquiry::kUniversalNonRealtimeId &&
           data[3] == SysExConstants::DeviceInquiry::kSubIdGeneralInfo &&
           data[4] == SysExConstants::DeviceInquiry::kSubIdDeviceIdReply;
}

bool SysExParser::isOberheimMatrix1000Message(const juce::uint8* data)
{
    return data[1] == SysExConstants::kManufacturerIdOberheim &&
           data[2] == SysExConstants::kDeviceIdMatrix1000;
}

size_t SysExParser::getExpectedMessageLength(MessageType messageType)
{
    switch (messageType)
    {
        case MessageType::kPatch:
            return SysExConstants::kPatchMessageLength;
        case MessageType::kMaster:
            return SysExConstants::kMasterMessageLength;
        case MessageType::kSplitPatch:
            return SysExConstants::kSplitPatchMessageLength;
        case MessageType::kDeviceId:
        case MessageType::kUnknown:
            return 0;
    }
    return 0;
}

