#pragma once

#include <array>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

    // In-memory representation of the Oberheim Matrix-1000 134-byte packed single-patch
    // buffer. Parameter values are read and written by descriptor byte offset, so the
    // SysEx byte layout lives only in PluginDescriptors. Nibble packing, checksum and
    // SysEx framing remain the responsibility of SysExEncoder / SysExDecoder.
    class PatchModel
    {
    public:
        static constexpr size_t kBufferSize = SysExConstants::kPatchPackedDataSize; // 134
        static constexpr int kNameLength = 8;                                       // bytes 0-7

        PatchModel() = default;

        // Raw packed-buffer access for interop with SysExEncoder / SysExDecoder.
        const juce::uint8* data() const noexcept { return buffer_.data(); }
        juce::uint8* data() noexcept { return buffer_.data(); }
        void loadFrom(const juce::uint8* packedData) noexcept;

        // Descriptor-driven parameter access. Offsets are sourced only from PluginDescriptors.
        int getValue(const PluginDescriptors::IntParameterDescriptor& descriptor) const;
        void setValue(const PluginDescriptors::IntParameterDescriptor& descriptor, int value);

        int getChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor) const;
        void setChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor, int index);

        // Patch name (bytes 0-7, Matrix 6-bit ASCII charset). Limited to kNameLength characters.
        juce::String getName() const;
        void setName(const juce::String& name);

    private:
        static bool isSignedField(int minValue) noexcept { return minValue < 0; }
        static int signBitPosition(int maxValue) noexcept;
        static int decodeField(juce::uint8 raw, int minValue, int maxValue);
        static juce::uint8 encodeField(int value, int minValue, int maxValue);
        static juce::juce_wchar decodeNameChar(juce::uint8 raw) noexcept;

        std::array<juce::uint8, kBufferSize> buffer_ {};
    };

}
