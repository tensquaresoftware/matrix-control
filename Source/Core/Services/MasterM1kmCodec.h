#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
    // Legacy Max for Live Matrix-1000 Editor .m1km: 172 little-endian int16 values
    // (344 bytes), no header. Each int16 must be in [-128, 255] (signed Master Tune /
    // Transpose plus Groups masks 0..255); the packed RAM byte is the low 8 bits.
    namespace MasterM1kmCodec
    {
        constexpr const char* kExtension = ".m1km";
        constexpr size_t kFileByteSize = 344;
        constexpr size_t kInt16Count = SysExConstants::kMasterPackedDataSize;
        constexpr int kMinInt16 = -128;
        constexpr int kMaxInt16 = 255;

        static_assert(kFileByteSize == kInt16Count * 2);

        bool hasExtension(const juce::File& file) noexcept;
        bool isValidContents(const juce::MemoryBlock& data) noexcept;
        bool decodeToPacked(const juce::MemoryBlock& data, juce::uint8* packedOut) noexcept;
    }
} // namespace Core
