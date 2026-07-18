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
    juce::MemoryBlock encodePatchToEditBufferSysEx(const juce::uint8* packedData) const;
    juce::MemoryBlock encodeMasterSysEx(juce::uint8 version, const juce::uint8* packedData) const;
    static juce::MemoryBlock encodeDeviceInquiry();

    /** Universal Device Inquiry reply:
        F0 7E <chan> 06 02 10 06 00 <memb-lo> <memb-hi> <rev0..3> F7.
        Firmware version is clamped/padded to exactly 4 ASCII bytes (7-bit);
        empty input uses default "1.11". */
    static juce::MemoryBlock encodeDeviceInquiryReply(juce::uint8 memberLow,
                                                      juce::uint8 memberHigh,
                                                      juce::StringRef firmwareVersion = "1.11",
                                                      juce::uint8 channel = 0x00);

    juce::MemoryBlock encodeRequestMessage(juce::uint8 requestType, juce::uint8 patchNumber = 0) const;
    juce::MemoryBlock encodeRemoteParameterEdit(juce::uint8 parameterNumber, juce::uint8 value) const;
    juce::MemoryBlock encodeMatrixModBusEdit(juce::uint8 bus,
                                             juce::uint8 source,
                                             juce::uint8 amount,
                                             juce::uint8 destination) const;
    juce::MemoryBlock encodeSetBank(juce::uint8 bank) const;
    static juce::MemoryBlock encodeUnlockBank();
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

