#pragma once

#include <cstddef>

#include <juce_core/juce_core.h>

namespace Core
{

    // Encodes / decodes the Master Edit CHANNEL combo (OMNI / CHANNEL 1–16 / MONO G1–9)
    // into the three Oberheim master bytes: basic channel (11), Omni (12), Mono (35).
    // MasterModel keeps generic single-offset accessors; callers that understand the
    // composite field use this codec (ApvtsMasterMapper, init copy).
    namespace MidiChannelMasterCodec
    {
        constexpr int kBasicChannelOffset = 11;
        constexpr int kOmniOffset         = 12;
        constexpr int kMonoOffset         = 35;

        constexpr int kOmniComboIndex       = 0;
        constexpr int kFirstChannelIndex    = 1;
        constexpr int kLastChannelIndex     = 16;
        constexpr int kFirstMonoIndex       = 17;
        constexpr int kLastMonoIndex        = 25;
        constexpr int kMaxBasicChannel      = 16;
        constexpr int kMaxMonoGroup         = 9;

        // Writes bytes 11 / 12 / 35 from a clamped combo index. bufferSize must cover
        // kMonoOffset (MasterModel::kBufferSize).
        void applyComboIndex(juce::uint8* buffer, size_t bufferSize, int comboIndex) noexcept;

        // Reads bytes 11 / 12 / 35 into a combo index. Mono wins over Omni; Mono group
        // channels above 9 clamp to MONO G9 for display.
        int readComboIndex(const juce::uint8* buffer, size_t bufferSize) noexcept;
    }

} // namespace Core
