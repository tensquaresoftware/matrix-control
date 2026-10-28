#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
// True for an Oberheim Matrix-6/6R Master Parameter dump used as discovery proof.
// Accepts wire length 478 or 479 only — rejects Matrix-1000 Master (351) and the
// youngmonkey printed total 477 alone. Discovery does not decode the payload.
inline bool isMatrix6MasterParameterDump(const juce::MemoryBlock& sysex) noexcept
{
    const auto size = sysex.getSize();
    if (size < SysExConstants::kMatrix6MasterMessageLengthMin
        || size > SysExConstants::kMatrix6MasterMessageLengthMax)
        return false;

    const auto* data = static_cast<const juce::uint8*>(sysex.getData());
    return data[0] == SysExConstants::kSysExStart
           && data[1] == SysExConstants::kManufacturerIdOberheim
           && data[2] == SysExConstants::kDeviceIdMatrix1000
           && data[3] == SysExConstants::Opcode::kMasterParameterData
           && data[size - 1] == SysExConstants::kSysExEnd;
}
} // namespace Core
