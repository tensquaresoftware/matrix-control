#pragma once

#include <vector>

#include <juce_core/juce_core.h>

#include "SysExConstants.h"

class SysExEncoder
{
public:
    SysExEncoder() = default;
    ~SysExEncoder() = default;

    juce::MemoryBlock encodePatchSysEx(juce::uint8 patchNumber, const juce::uint8* packedData) const;
    juce::MemoryBlock encodeMasterSysEx(juce::uint8 version, const juce::uint8* packedData) const;
    static juce::MemoryBlock encodeDeviceInquiry();
    juce::MemoryBlock encodeRequestMessage(juce::uint8 requestType, juce::uint8 patchNumber = 0) const;
    static size_t unpackBytes(const juce::uint8* bytes, size_t numBytes, juce::uint8* output);
    static juce::uint8 calculateChecksum(const juce::uint8* data, size_t length);

private:
    std::vector<juce::uint8> buildHeader(juce::uint8 opcode, juce::uint8 headerData = 0) const;
    std::vector<juce::uint8> unpackBytesToNibbles(const juce::uint8* packedData, size_t numBytes) const;
    juce::MemoryBlock buildCompletePatchSysExMessage(
        const std::vector<juce::uint8>& header,
        const std::vector<juce::uint8>& nibbles,
        juce::uint8 checksum) const;
    juce::MemoryBlock buildCompleteMasterSysExMessage(
        const std::vector<juce::uint8>& header,
        const std::vector<juce::uint8>& nibbles,
        juce::uint8 checksum) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysExEncoder)
};

