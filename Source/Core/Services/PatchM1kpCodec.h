#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
    // Legacy Max for Live Matrix-1000 Editor .m1kp: 134 little-endian int16 values
    // (268 bytes), no header. Each int16 must be in int8 range [-128, 127]; the
    // packed RAM byte is the low 8 bits (two's complement).
    namespace PatchM1kpCodec
    {
        constexpr const char* kExtension = ".m1kp";
        constexpr size_t kFileByteSize = 268;
        constexpr size_t kInt16Count = SysExConstants::kPatchPackedDataSize;
        constexpr int kMinInt16 = -128;
        constexpr int kMaxInt16 = 127;

        bool hasExtension(const juce::File& file) noexcept;
        bool isValidContents(const juce::MemoryBlock& data) noexcept;
        bool decodeToPacked(const juce::MemoryBlock& data, juce::uint8* packedOut) noexcept;
    }
} // namespace Core
