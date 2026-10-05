#pragma once

#include <juce_core/juce_core.h>

namespace TSS
{
    /** Exclusive stereo I/O pair helpers shared by Settings AUDIO and device setup sync. */
    namespace RadioButtonGroupLayout
    {
        inline juce::String stereoPairLabel(int pairIndex) noexcept
        {
            const int left = pairIndex * 2 + 1;
            const int right = left + 1;
            return juce::String(left) + " + " + juce::String(right);
        }

        inline int stereoPairCount(int channelCount) noexcept
        {
            return juce::jmax(0, channelCount / 2);
        }

        inline juce::BigInteger stereoPairMask(int pairIndex) noexcept
        {
            juce::BigInteger mask;
            if (pairIndex >= 0)
            {
                mask.setBit(pairIndex * 2);
                mask.setBit(pairIndex * 2 + 1);
            }
            return mask;
        }

        /** Returns pair index when `channels` matches that exclusive stereo mask; else -1. */
        inline int selectedStereoPairIndex(const juce::BigInteger& channels, int pairCount) noexcept
        {
            for (int i = 0; i < pairCount; ++i)
            {
                if (channels == stereoPairMask(i))
                    return i;
            }
            return -1;
        }

        /** First ComboBox item id for exclusive stereo pairs (Settings AUDIO channel combos). */
        inline constexpr int kFirstChannelPairComboItemId = 1;

        inline int channelPairComboItemId(int pairIndex) noexcept
        {
            return pairIndex + kFirstChannelPairComboItemId;
        }

        /** Inverse of `channelPairComboItemId`. Caller must validate with `isChannelPairComboItemId`. */
        inline int channelPairIndexFromComboItemId(int itemId) noexcept
        {
            return itemId - kFirstChannelPairComboItemId;
        }

        inline bool isChannelPairComboItemId(int itemId) noexcept
        {
            return itemId >= kFirstChannelPairComboItemId;
        }
    }
}
