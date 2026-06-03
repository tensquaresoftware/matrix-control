#pragma once

#include <array>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

    // In-memory representation of the Oberheim Matrix-1000 172-byte packed master
    // ("Global Parameters") buffer. Parameter values are read and written by descriptor
    // byte offset, so the SysEx byte layout lives only in PluginDescriptors. Unlike
    // PatchModel there is no name field. Nibble packing, checksum and SysEx framing
    // remain the responsibility of SysExEncoder / SysExDecoder.
    class MasterModel
    {
    public:
        static constexpr size_t kBufferSize = SysExConstants::kMasterPackedDataSize; // 172

        MasterModel() = default;

        // Raw packed-buffer access for interop with SysExEncoder / SysExDecoder.
        const juce::uint8* data() const noexcept { return buffer_.data(); }
        juce::uint8* data() noexcept { return buffer_.data(); }
        void loadFrom(const juce::uint8* packedData) noexcept;

        // Descriptor-driven parameter access. Offsets are sourced only from PluginDescriptors.
        int getValue(const PluginDescriptors::IntParameterDescriptor& descriptor) const;
        void setValue(const PluginDescriptors::IntParameterDescriptor& descriptor, int value);

        // The MIDI Basic Channel choice descriptor spans bytes 11/12/35 on the synth but
        // carries a single sysExOffset (11); this touches byte 11 only. Full Omni/Mono
        // composition is owned by ApvtsMasterMapper (Story 1.4); the 172-byte buffer
        // round-trip preserves all three bytes regardless.
        int getChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor) const;
        void setChoiceIndex(const PluginDescriptors::ChoiceParameterDescriptor& descriptor, int index);

    private:
        std::array<juce::uint8, kBufferSize> buffer_ {};
    };

}
