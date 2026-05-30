#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/Exceptions/Exceptions.h"
#include "SysExConstants.h"

class SysExParser
{
public:
    enum class MessageType
    {
        kUnknown,
        kPatch,
        kMaster,
        kDeviceId,
        kSplitPatch
    };

    struct ValidationResult
    {
        bool isValid;
        MessageType messageType;
        juce::String errorMessage;

        static ValidationResult success(MessageType type)
        {
            return { true, type, {} };
        }

        static ValidationResult failure(const juce::String& error)
        {
            return { false, MessageType::kUnknown, error };
        }
    };

    SysExParser() = default;
    ~SysExParser() = default;

    ValidationResult validateSysEx(const juce::MemoryBlock& sysEx) const;
    bool validateStructure(const juce::MemoryBlock& sysEx) const;
    bool validateChecksum(const juce::MemoryBlock& sysEx) const;
    ValidationResult validateMessageType(const juce::MemoryBlock& sysEx) const;
    static juce::uint8 calculateChecksum(const juce::uint8* data, size_t length);
    static size_t packNibbles(const juce::uint8* nibbles, size_t numNibbles, juce::uint8* output);

private:
    bool validateManufacturerAndDevice(const juce::MemoryBlock& sysEx) const;
    static MessageType getMessageTypeFromOpcode(juce::uint8 opcode);
    static bool isDeviceInquiryMessage(const juce::uint8* data);
    static std::vector<juce::uint8> packNibblesToBytes(const juce::uint8* nibbles, size_t numNibbles);
    static bool isDeviceIdResponse(const juce::uint8* data);
    static bool isOberheimMatrix1000Message(const juce::uint8* data);
    static size_t getExpectedMessageLength(MessageType messageType);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysExParser)
};

