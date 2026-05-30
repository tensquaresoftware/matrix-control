#pragma once

#include <vector>

#include <juce_core/juce_core.h>

#include "SysExParser.h"

struct DeviceIdInfo
{
    juce::uint8 manufacturerId;
    juce::uint8 familyLow;
    juce::uint8 familyHigh;
    juce::uint8 memberLow;
    juce::uint8 memberHigh;
    juce::String version;
    bool isValid;
};

class SysExDecoder
{
public:
    explicit SysExDecoder(SysExParser& parserRef);
    ~SysExDecoder() = default;

    bool decodePatchSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const;
    bool decodeMasterSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const;
    DeviceIdInfo decodeDeviceId(const juce::MemoryBlock& sysEx) const;
    static size_t packNibbles(const juce::uint8* nibbles, size_t numNibbles, juce::uint8* output);

private:
    SysExParser& parser;

    bool extractPackedData(const juce::MemoryBlock& sysEx,
                          size_t dataStartIndex,
                          size_t expectedPackedSize,
                          juce::uint8* output) const;
    
    bool validatePatchSysExMessage(const juce::MemoryBlock& sysEx) const;
    bool extractPackedDataFromPatchSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const;
    bool validateMasterSysExMessage(const juce::MemoryBlock& sysEx) const;
    bool extractPackedDataFromMasterSysEx(const juce::MemoryBlock& sysEx, juce::uint8* output) const;
    bool validateDeviceInquiryStructure(const juce::uint8* data) const;
    void extractDeviceInformation(const juce::uint8* data, DeviceIdInfo& info) const;
    void extractDeviceVersion(const juce::uint8* data, size_t messageSize, DeviceIdInfo& info) const;
    void validateMatrix1000Device(DeviceIdInfo& info) const;
    size_t getChecksumIndex(size_t totalSize) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysExDecoder)
};

