#pragma once

#include <cmath>

namespace PluginAudioConstants
{
    /// Sentinel for −∞ display; maps to linear gain 0.0f.
    inline constexpr float kSilenceInputGainDb = -120.0f;
    /// Lowest finite gain step above silence.
    inline constexpr float kMinInputGainDb = -70.0f;
    inline constexpr float kMaxInputGainDb = 12.0f;

    inline constexpr int kInputGainSilenceIndex = 0;
    inline constexpr int kInputGainMaxIndex = 1 + static_cast<int>(kMaxInputGainDb - kMinInputGainDb);
    inline constexpr int kInputGainDefaultIndex = 1 + static_cast<int>(0.0f - kMinInputGainDb);

    inline constexpr float snapInputGainDb(float gainDb) noexcept
    {
        if (!std::isfinite(gainDb))
            return 0.0f;

        if (gainDb <= kSilenceInputGainDb + 0.5f)
            return kSilenceInputGainDb;

        if (gainDb < kMinInputGainDb)
            return kMinInputGainDb;

        return gainDb > kMaxInputGainDb ? kMaxInputGainDb : gainDb;
    }

    inline int inputGainDbToIndex(float gainDb) noexcept
    {
        const float snappedDb = snapInputGainDb(gainDb);

        if (snappedDb <= kSilenceInputGainDb + 0.5f)
            return kInputGainSilenceIndex;

        return 1 + static_cast<int>(std::round(snappedDb - kMinInputGainDb));
    }

    inline float inputGainIndexToDb(int index) noexcept
    {
        if (index <= kInputGainSilenceIndex)
            return kSilenceInputGainDb;

        return kMinInputGainDb + static_cast<float>(index - 1);
    }

    inline float inputGainIndexToNormalizedFill(int index) noexcept
    {
        const int clampedIndex = index < kInputGainSilenceIndex ? kInputGainSilenceIndex
                            : (index > kInputGainMaxIndex ? kInputGainMaxIndex : index);

        return static_cast<float>(clampedIndex) / static_cast<float>(kInputGainMaxIndex);
    }
}
