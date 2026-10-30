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

// Receive-log label for a Matrix-6/6R Master dump (structural match only — no
// checksum verify). Empty when the frame is not that dump; an empty return means
// the caller may fall through to Matrix-1000 validateSysEx (which ERROR-logs on
// length mismatch). A non-empty return must skip validateSysEx to avoid a false ERROR.
inline juce::String matrix6MasterDumpReceiveLogAnalysis(const juce::MemoryBlock& sysex)
{
    if (! isMatrix6MasterParameterDump(sysex))
        return {};

    const auto* data = static_cast<const juce::uint8*>(sysex.getData());
    const auto checksum = data[sysex.getSize() - 2];
    return "Matrix-6/6R Master dump received | Checksum: 0x"
           + juce::String::toHexString(checksum).paddedLeft('0', 2).toUpperCase();
}
} // namespace Core
