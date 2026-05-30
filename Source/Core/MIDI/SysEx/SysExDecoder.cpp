#include "SysExDecoder.h"

#include "Core/Loggers/MidiLogger.h"
#include "SysExConstants.h"

SysExDecoder::SysExDecoder(SysExParser& parserRef)
    : parser(parserRef)
{
}

bool SysExDecoder::decodePatchSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const
{
    if (output == nullptr)
    {
        MidiLogger::getInstance().logError("decodePatchSysEx: null output pointer");
        return false;
    }

    if (!validatePatchSysExMessage(sysEx))
        return false;

    bool success = extractPackedDataFromPatchSysEx(sysEx, output);
    if (success)
    {
        MidiLogger::getInstance().logInfo("Successfully decoded patch SysEx");
    }
    else
    {
        MidiLogger::getInstance().logError("Failed to extract packed data from patch SysEx");
    }
    return success;
}

bool SysExDecoder::decodeMasterSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const
{
    if (output == nullptr)
    {
        MidiLogger::getInstance().logError("decodeMasterSysEx: null output pointer");
        return false;
    }

    if (!validateMasterSysExMessage(sysEx))
        return false;

    bool success = extractPackedDataFromMasterSysEx(sysEx, output);
    if (success)
    {
        MidiLogger::getInstance().logInfo("Successfully decoded master SysEx");
    }
    else
    {
        MidiLogger::getInstance().logError("Failed to extract packed data from master SysEx");
    }
    return success;
}

DeviceIdInfo SysExDecoder::decodeDeviceId(const juce::MemoryBlock& sysEx) const
{
    DeviceIdInfo info = {};
    info.isValid = false;

    if (sysEx.getSize() < 15)
    {
        MidiLogger::getInstance().logError("decodeDeviceId: message too short");
        return info;
    }

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());

    if (!validateDeviceInquiryStructure(data))
        return info;

    extractDeviceInformation(data, info);
    extractDeviceVersion(data, sysEx.getSize(), info);
    validateMatrix1000Device(info);

    if (info.isValid)
    {
        MidiLogger::getInstance().logInfo("Device ID decoded: Matrix-1000, version: " + info.version);
    }
    else
    {
        MidiLogger::getInstance().logWarning("Device ID validation failed - not a Matrix-1000");
    }

    return info;
}

size_t SysExDecoder::packNibbles(const juce::uint8* nibbles, size_t numNibbles, juce::uint8* output)
{
    if (nibbles == nullptr || output == nullptr || numNibbles % 2 != 0)
    {
        return 0;
    }

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

bool SysExDecoder::extractPackedData(const juce::MemoryBlock& sysEx,
                                     size_t dataStartIndex,
                                     size_t expectedPackedSize,
                                     juce::uint8* output) const
{
    if (output == nullptr || sysEx.getSize() < dataStartIndex + expectedPackedSize * 2 + 2)
    {
        return false;
    }

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    size_t totalSize = sysEx.getSize();

    size_t checksumIndex = getChecksumIndex(totalSize);
    size_t numNibbles = checksumIndex - dataStartIndex;

    if (numNibbles != expectedPackedSize * 2)
    {
        return false;
    }

    size_t packedBytes = packNibbles(&data[dataStartIndex], numNibbles, output);
    return packedBytes == expectedPackedSize;
}

bool SysExDecoder::validatePatchSysExMessage(const juce::MemoryBlock& sysEx) const
{
    auto validation = parser.validateSysEx(sysEx);
    if (!validation.isValid || validation.messageType != SysExParser::MessageType::kPatch)
    {
        MidiLogger::getInstance().logError("decodePatchSysEx: validation failed");
        return false;
    }
    return true;
}

bool SysExDecoder::extractPackedDataFromPatchSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const
{
    return extractPackedData(sysEx, 5, SysExConstants::kPatchPackedDataSize, output);
}

bool SysExDecoder::validateMasterSysExMessage(const juce::MemoryBlock& sysEx) const
{
    auto validation = parser.validateSysEx(sysEx);
    if (!validation.isValid || validation.messageType != SysExParser::MessageType::kMaster)
    {
        MidiLogger::getInstance().logError("decodeMasterSysEx: validation failed");
        return false;
    }
    return true;
}

bool SysExDecoder::extractPackedDataFromMasterSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const
{
    return extractPackedData(sysEx, 5, SysExConstants::kMasterPackedDataSize, output);
}

bool SysExDecoder::validateDeviceInquiryStructure(const juce::uint8* data) const
{
    return data[0] == SysExConstants::kSysExStart &&
           data[1] == SysExConstants::DeviceInquiry::kUniversalNonRealtimeId &&
           data[3] == SysExConstants::DeviceInquiry::kSubIdGeneralInfo &&
           data[4] == SysExConstants::DeviceInquiry::kSubIdDeviceIdReply;
}

void SysExDecoder::extractDeviceInformation(const juce::uint8* data, DeviceIdInfo& info) const
{
    info.manufacturerId = data[5];
    info.familyLow = data[6];
    info.familyHigh = data[7];
    info.memberLow = data[8];
    info.memberHigh = data[9];
}

void SysExDecoder::extractDeviceVersion(const juce::uint8* data, size_t messageSize, DeviceIdInfo& info) const
{
    if (messageSize >= 15)
    {
        char versionStr[5] = {0};
        versionStr[0] = static_cast<char>(data[10]);
        versionStr[1] = static_cast<char>(data[11]);
        versionStr[2] = static_cast<char>(data[12]);
        versionStr[3] = static_cast<char>(data[13]);
        info.version = juce::String(versionStr).trim();
    }
}

void SysExDecoder::validateMatrix1000Device(DeviceIdInfo& info) const
{
    info.isValid = (info.manufacturerId == SysExConstants::DeviceInquiry::kExpectedManufacturer &&
                    info.familyLow == SysExConstants::DeviceInquiry::kExpectedFamily &&
                    info.memberLow == SysExConstants::DeviceInquiry::kExpectedMemberLow &&
                    info.memberHigh == SysExConstants::DeviceInquiry::kExpectedMemberHigh);
}

size_t SysExDecoder::getChecksumIndex(size_t totalSize) const
{
    return totalSize - 2;
}

