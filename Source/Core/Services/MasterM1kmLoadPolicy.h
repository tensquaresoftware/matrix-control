#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{
    class MasterModel;

    // User choice after a successful .m1km decode, before commit to the live master.
    enum class MasterM1kmGroupsPolicy
    {
        kMasterSettingsOnly, // keep mapped params; reset Groups + cascade from InitDefaults
        kFullMaster          // commit all 172 decoded bytes as-is
    };

    namespace MasterM1kmLoadPolicy
    {
        // Inclusive Groups range G000–G124 in the packed master buffer.
        constexpr size_t kGroupsOffset = 36;
        constexpr size_t kGroupsLastInclusive = 160;
        constexpr size_t kGroupsCount = kGroupsLastInclusive - kGroupsOffset + 1;

        // Cascade fields in the packed master buffer.
        constexpr size_t kCascadeOffset = 166;
        constexpr size_t kCascadeLastInclusive = 168;
        constexpr size_t kCascadeCount = kCascadeLastInclusive - kCascadeOffset + 1;

        static_assert(kGroupsCount == 125);
        static_assert(kCascadeCount == 3);
        static_assert(kCascadeLastInclusive < SysExConstants::kMasterPackedDataSize);

        // Applies policy in place. kFullMaster is a no-op. initDefaults must be 172 bytes.
        void apply(juce::uint8* packed172,
                   MasterM1kmGroupsPolicy policy,
                   const juce::uint8* initDefaults) noexcept;

        // Copies packed172, applies policy, then loadFrom into model (Settings commit SSOT).
        void loadPackedIntoModel(MasterModel& model,
                                 const juce::uint8* packed172,
                                 MasterM1kmGroupsPolicy policy,
                                 const juce::uint8* initDefaults) noexcept;
    }
} // namespace Core
