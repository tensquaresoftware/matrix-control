#pragma once

#include <juce_core/juce_core.h>

namespace SysExConstants
{
    // SysEx message delimiters
    constexpr juce::uint8 kSysExStart = 0xF0;
    constexpr juce::uint8 kSysExEnd = 0xF7;

    // Manufacturer and Device IDs
    constexpr juce::uint8 kManufacturerIdOberheim = 0x10;
    constexpr juce::uint8 kDeviceIdMatrix1000 = 0x06;

    // SysEx opcodes for Matrix-1000
    namespace Opcode
    {
        constexpr juce::uint8 kSinglePatchData = 0x01;
        constexpr juce::uint8 kSplitPatchData = 0x02;  // Matrix-6 compatibility
        constexpr juce::uint8 kMasterParameterData = 0x03;
        constexpr juce::uint8 kRequestData = 0x04;
        constexpr juce::uint8 kRemoteParameterEdit = 0x06;
        constexpr juce::uint8 kSetGroupMode = 0x07;
        constexpr juce::uint8 kSetBank = 0x0A;
        constexpr juce::uint8 kRemoteParameterEditMatrix = 0x0B;
    }

    // Request types (for opcode 0x04)
    namespace RequestType
    {
        constexpr juce::uint8 kRequestAllBank = 0x00;  // Not implemented in v1.0
        constexpr juce::uint8 kRequestSinglePatch = 0x01;
        constexpr juce::uint8 kRequestSplitPatch = 0x02;  // Matrix-6 compatibility
        constexpr juce::uint8 kRequestMasterParameters = 0x03;
        constexpr juce::uint8 kRequestEditBuffer = 0x04;
    }

    // SysEx message lengths (total transmitted bytes)
    constexpr size_t kPatchMessageLength = 275;  // 134 packed bytes = 268 nibbles + 5 header + 1 checksum + 1 EOX
    constexpr size_t kMasterMessageLength = 351;  // 172 packed bytes = 344 nibbles + 5 header + 1 checksum + 1 EOX
    constexpr size_t kSplitPatchMessageLength = 36 + 5 + 1 + 1;  // 36 bytes + header + checksum + EOX

    // Packed data sizes (before unpacking to nibbles)
    constexpr size_t kPatchPackedDataSize = 134;
    constexpr size_t kMasterPackedDataSize = 172;

    // Minimum SysEx message length (header + checksum + EOX)
    constexpr size_t kMinSysExLength = 7;  // F0 10 06 <opcode> <data...> <checksum> F7

    // Timeout values
    constexpr int kDefaultTimeoutMs = 2000;  // 2 seconds timeout for MIDI operations
    constexpr int kMinSysExDelayMs = 10;     // Minimum delay between SysEx messages
    constexpr int kMidiInputStopDelayMs = 100;  // Delay after stopping MIDI input callbacks before closing port

    // Device Inquiry (Universal SysEx)
    namespace DeviceInquiry
    {
        constexpr juce::uint8 kUniversalNonRealtimeId = 0x7E;
        constexpr juce::uint8 kDeviceIdAll = 0x7F;
        constexpr juce::uint8 kSubIdGeneralInfo = 0x06;
        constexpr juce::uint8 kSubIdDeviceIdRequest = 0x01;
        constexpr juce::uint8 kSubIdDeviceIdReply = 0x02;

        // Device Inquiry request message: F0 7E 7F 06 01 F7
        constexpr juce::uint8 kRequestMessage[] = { 0xF0, 0x7E, 0x7F, 0x06, 0x01, 0xF7 };
        constexpr size_t kRequestMessageLength = 6;

        // Expected response format: F0 7E <chan> 06 02 10 06 00 02 00 <rev-0> <rev-1> <rev-2> <rev-3> F7
        // Manufacturer: 0x10 (Oberheim)
        // Family: 0x06 (Matrix series)
        // Member: 0x10 0x02 (Matrix-1000)
        constexpr juce::uint8 kExpectedManufacturer = 0x10;
        constexpr juce::uint8 kExpectedFamily = 0x06;
        constexpr juce::uint8 kExpectedMemberLow = 0x00;
        constexpr juce::uint8 kExpectedMemberHigh = 0x02;
    }
}

