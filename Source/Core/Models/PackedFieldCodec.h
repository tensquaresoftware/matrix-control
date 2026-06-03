#pragma once

#include <cstddef>

#include <juce_core/juce_core.h>

namespace Core
{

    // Shared codec for descriptor-driven access into a Matrix-1000 packed buffer
    // (patch or master). A field is one byte holding a single parameter value,
    // unsigned or 8-bit two's-complement signed (the synth sign-extends bit 6 into
    // bit 7). Offsets and value ranges come from PluginDescriptors; nibble packing,
    // checksum and SysEx framing remain the responsibility of SysExEncoder/Decoder.
    namespace PackedFieldCodec
    {
        // Validates a descriptor sysExOffset against the buffer size and returns it as
        // an index. Out-of-range offsets are a descriptor bug (jassert in debug).
        size_t safeOffset(int sysExOffset, size_t bufferSize) noexcept;

        // Decodes a raw byte into a signed or unsigned parameter value, clamped to
        // [minValue, maxValue]. Signed fields (minValue < 0) are sign-extended from a
        // field width derived from maxValue.
        int decodeField(juce::uint8 raw, int minValue, int maxValue) noexcept;

        // Encodes a parameter value into its canonical packed byte. Signed values are
        // stored as 8-bit two's complement, so a clamped cast reproduces the synth byte.
        juce::uint8 encodeField(int value, int minValue, int maxValue) noexcept;
    }

}
