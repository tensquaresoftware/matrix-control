#include "PackedFieldCodec.h"

namespace Core
{

    namespace
    {
        bool isSignedField(int minValue) noexcept { return minValue < 0; }

        // Field width (bit count) of a signed range expressed as its sign-bit index:
        // maxValue 63 -> bit 6 (7-bit field), 31 -> bit 5 (6-bit field).
        int signBitPosition(int maxValue) noexcept
        {
            int position = 0;
            while ((1 << (position + 1)) <= maxValue)
                ++position;

            return position + 1;
        }
    }

    size_t PackedFieldCodec::safeOffset(int sysExOffset, size_t bufferSize) noexcept
    {
        jassert(sysExOffset >= 0 && static_cast<size_t>(sysExOffset) < bufferSize);
        juce::ignoreUnused(bufferSize);
        return static_cast<size_t>(sysExOffset);
    }

    int PackedFieldCodec::decodeField(juce::uint8 raw, int minValue, int maxValue) noexcept
    {
        if (! isSignedField(minValue))
            return juce::jlimit(minValue, maxValue, static_cast<int>(raw));

        const int signBit = signBitPosition(maxValue);
        const int fieldMask = (1 << (signBit + 1)) - 1;
        int value = raw & fieldMask;
        if ((value & (1 << signBit)) != 0)
            value -= (1 << (signBit + 1));

        return juce::jlimit(minValue, maxValue, value);
    }

    juce::uint8 PackedFieldCodec::encodeField(int value, int minValue, int maxValue) noexcept
    {
        return static_cast<juce::uint8>(juce::jlimit(minValue, maxValue, value));
    }

}
