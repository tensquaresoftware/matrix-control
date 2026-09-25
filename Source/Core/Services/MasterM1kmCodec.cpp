#include "Core/Services/MasterM1kmCodec.h"

#include <cstring>

namespace Core
{
    namespace MasterM1kmCodec
    {
        bool hasExtension(const juce::File& file) noexcept
        {
            return file.getFileExtension().equalsIgnoreCase(kExtension);
        }

        bool decodeToPacked(const juce::MemoryBlock& data, juce::uint8* packedOut) noexcept
        {
            if (packedOut == nullptr || data.getSize() != kFileByteSize)
                return false;

            const auto* bytes = static_cast<const juce::uint8*>(data.getData());
            juce::uint8 packed[kInt16Count];

            for (size_t i = 0; i < kInt16Count; ++i)
            {
                const auto low = bytes[i * 2];
                const auto high = bytes[i * 2 + 1];
                const auto value = static_cast<int16_t>(
                    static_cast<uint16_t>(low) | (static_cast<uint16_t>(high) << 8));

                if (value < kMinInt16 || value > kMaxInt16)
                    return false;

                packed[i] = static_cast<juce::uint8>(value & 0xff);
            }

            std::memcpy(packedOut, packed, kInt16Count);
            return true;
        }

        bool isValidContents(const juce::MemoryBlock& data) noexcept
        {
            juce::uint8 packed[kInt16Count];
            return decodeToPacked(data, packed);
        }
    }
} // namespace Core
