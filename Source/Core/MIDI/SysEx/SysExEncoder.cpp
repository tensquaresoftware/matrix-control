#include <cstring>

#include "SysExEncoder.h"

#include "Core/Loggers/MidiLogger.h"

juce::MemoryBlock SysExEncoder::encodePatchSysEx(juce::uint8 patchNumber, const juce::uint8* packedData) const
{
    if (packedData == nullptr)
    {
        MidiLogger::getInstance().logError("encodePatchSysEx: null packedData pointer");
        return {};
    }

    auto header = buildHeader(SysExConstants::Opcode::kSinglePatchData, patchNumber);
    std::vector<juce::uint8> nibbles = unpackBytesToNibbles(packedData, SysExConstants::kPatchPackedDataSize);
    juce::uint8 checksum = calculateChecksum(packedData, SysExConstants::kPatchPackedDataSize);

    return buildCompletePatchSysExMessage(header, nibbles, checksum);
}

juce::MemoryBlock SysExEncoder::encodePatchToEditBufferSysEx(const juce::uint8* packedData) const
{
    if (packedData == nullptr)
    {
        MidiLogger::getInstance().logError("encodePatchToEditBufferSysEx: null packedData pointer");
        return {};
    }

    auto header = buildHeader(SysExConstants::Opcode::kSinglePatchToEditBuffer, 0);
    std::vector<juce::uint8> nibbles = unpackBytesToNibbles(packedData, SysExConstants::kPatchPackedDataSize);
    juce::uint8 checksum = calculateChecksum(packedData, SysExConstants::kPatchPackedDataSize);

    return buildCompletePatchSysExMessage(header, nibbles, checksum);
}

juce::MemoryBlock SysExEncoder::encodeMasterSysEx(juce::uint8 version, const juce::uint8* packedData) const
{
    if (packedData == nullptr)
    {
        MidiLogger::getInstance().logError("encodeMasterSysEx: null packedData pointer");
        return {};
    }

    auto header = buildHeader(SysExConstants::Opcode::kMasterParameterData, version);
    std::vector<juce::uint8> nibbles = unpackBytesToNibbles(packedData, SysExConstants::kMasterPackedDataSize);
    juce::uint8 checksum = calculateChecksum(packedData, SysExConstants::kMasterPackedDataSize);

    return buildCompleteMasterSysExMessage(header, nibbles, checksum);
}

juce::MemoryBlock SysExEncoder::encodeDeviceInquiry()
{
    juce::MemoryBlock message;
    message.append(SysExConstants::DeviceInquiry::kRequestMessage,
                   SysExConstants::DeviceInquiry::kRequestMessageLength);
    return message;
}

juce::MemoryBlock SysExEncoder::encodeDeviceInquiryReply(juce::uint8 memberLow,
                                                         juce::uint8 memberHigh,
                                                         juce::StringRef firmwareVersion,
                                                         juce::uint8 channel)
{
    juce::uint8 versionBytes[4] = { ' ', ' ', ' ', ' ' };
    auto version = juce::String(firmwareVersion);
    if (version.isEmpty())
        version = "1.11";

    const int copyLen = juce::jmin(4, version.length());
    for (int i = 0; i < copyLen; ++i)
        versionBytes[static_cast<size_t>(i)] = static_cast<juce::uint8>(version[i] & 0x7f);

    const juce::uint8 reply[] = {
        SysExConstants::kSysExStart,
        SysExConstants::DeviceInquiry::kUniversalNonRealtimeId,
        channel,
        SysExConstants::DeviceInquiry::kSubIdGeneralInfo,
        SysExConstants::DeviceInquiry::kSubIdDeviceIdReply,
        SysExConstants::DeviceInquiry::kExpectedManufacturer,
        SysExConstants::DeviceInquiry::kExpectedFamily,
        SysExConstants::DeviceInquiry::kExpectedFamilyHigh,
        memberLow,
        memberHigh,
        versionBytes[0],
        versionBytes[1],
        versionBytes[2],
        versionBytes[3],
        SysExConstants::kSysExEnd
    };

    return juce::MemoryBlock(reply, sizeof(reply));
}

juce::MemoryBlock SysExEncoder::encodeRequestMessage(juce::uint8 requestType, juce::uint8 patchNumber) const
{
    auto header = buildHeader(SysExConstants::Opcode::kRequestData, requestType);

    juce::MemoryBlock message;
    message.append(header.data(), header.size());
    message.append(&patchNumber, 1);
    message.append(&SysExConstants::kSysExEnd, 1);

    return message;
}

juce::MemoryBlock SysExEncoder::encodeRemoteParameterEdit(juce::uint8 parameterNumber, juce::uint8 value) const
{
    const juce::uint8 message[] {
        SysExConstants::kSysExStart,
        SysExConstants::kManufacturerIdOberheim,
        SysExConstants::kDeviceIdMatrix1000,
        SysExConstants::Opcode::kRemoteParameterEdit,
        static_cast<juce::uint8>(parameterNumber & 0x7F),
        value,
        SysExConstants::kSysExEnd
    };

    juce::MemoryBlock block;
    block.append(message, sizeof(message));
    return block;
}

juce::MemoryBlock SysExEncoder::encodeSetBank(juce::uint8 bank) const
{
    const juce::uint8 message[] {
        SysExConstants::kSysExStart,
        SysExConstants::kManufacturerIdOberheim,
        SysExConstants::kDeviceIdMatrix1000,
        SysExConstants::Opcode::kSetBank,
        static_cast<juce::uint8>(bank & 0x7F),
        SysExConstants::kSysExEnd
    };

    juce::MemoryBlock block;
    block.append(message, sizeof(message));
    return block;
}

juce::MemoryBlock SysExEncoder::encodeUnlockBank()
{
    const juce::uint8 message[] {
        SysExConstants::kSysExStart,
        SysExConstants::kManufacturerIdOberheim,
        SysExConstants::kDeviceIdMatrix1000,
        SysExConstants::Opcode::kUnlockBank,
        SysExConstants::kSysExEnd
    };

    juce::MemoryBlock block;
    block.append(message, sizeof(message));
    return block;
}

juce::MemoryBlock SysExEncoder::encodeMatrixModBusEdit(juce::uint8 bus,
                                                       juce::uint8 source,
                                                       juce::uint8 amount,
                                                       juce::uint8 destination) const
{
    const juce::uint8 message[] {
        SysExConstants::kSysExStart,
        SysExConstants::kManufacturerIdOberheim,
        SysExConstants::kDeviceIdMatrix1000,
        SysExConstants::Opcode::kRemoteParameterEditMatrix,
        bus,
        source,
        amount,
        destination,
        SysExConstants::kSysExEnd
    };

    juce::MemoryBlock block;
    block.append(message, sizeof(message));
    return block;
}

size_t SysExEncoder::unpackBytes(const juce::uint8* bytes, size_t numBytes, juce::uint8* output)
{
    for (size_t i = 0; i < numBytes; ++i)
    {
        output[i * 2] = bytes[i] & 0x0F;
        output[i * 2 + 1] = (bytes[i] >> 4) & 0x0F;
    }
    return numBytes * 2;
}

std::vector<juce::uint8> SysExEncoder::unpackBytesToNibbles(const juce::uint8* packedData, size_t numBytes) const
{
    std::vector<juce::uint8> nibbles(numBytes * 2);
    unpackBytes(packedData, numBytes, nibbles.data());
    return nibbles;
}

juce::MemoryBlock SysExEncoder::buildCompletePatchSysExMessage(
    const std::vector<juce::uint8>& header,
    const std::vector<juce::uint8>& nibbles,
    juce::uint8 checksum) const
{
    juce::MemoryBlock message;
    message.append(header.data(), header.size());
    message.append(nibbles.data(), nibbles.size());
    message.append(&checksum, 1);
    message.append(&SysExConstants::kSysExEnd, 1);
    return message;
}

juce::MemoryBlock SysExEncoder::buildCompleteMasterSysExMessage(
    const std::vector<juce::uint8>& header,
    const std::vector<juce::uint8>& nibbles,
    juce::uint8 checksum) const
{
    juce::MemoryBlock message;
    message.append(header.data(), header.size());
    message.append(nibbles.data(), nibbles.size());
    message.append(&checksum, 1);
    message.append(&SysExConstants::kSysExEnd, 1);
    return message;
}

juce::uint8 SysExEncoder::calculateChecksum(const juce::uint8* data, size_t length)
{
    juce::uint8 checksum = 0;
    for (size_t i = 0; i < length; ++i)
    {
        checksum += data[i];
    }
    return checksum & 0x7F;
}

std::vector<juce::uint8> SysExEncoder::buildHeader(juce::uint8 opcode, juce::uint8 headerData) const
{
    std::vector<juce::uint8> header;
    header.push_back(SysExConstants::kSysExStart);
    header.push_back(SysExConstants::kManufacturerIdOberheim);
    header.push_back(SysExConstants::kDeviceIdMatrix1000);
    header.push_back(opcode);
    if (headerData != 0 || opcode == SysExConstants::Opcode::kSinglePatchData ||
        opcode == SysExConstants::Opcode::kSinglePatchToEditBuffer ||
        opcode == SysExConstants::Opcode::kMasterParameterData ||
        opcode == SysExConstants::Opcode::kRequestData)
    {
        header.push_back(headerData);
    }
    return header;
}

