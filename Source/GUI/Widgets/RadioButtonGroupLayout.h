#pragma once

#include <juce_core/juce_core.h>

namespace TSS
{
    /** Pure layout helpers for exclusive radio options that wrap in a width. */
    namespace RadioButtonGroupLayout
    {
        struct OptionMetrics
        {
            int optionWidth = 0;
            int optionHeight = 0;
            int gapX = 0;
            int gapY = 0;
        };

        inline int preferredHeight(int optionCount, int availableWidth, const OptionMetrics& metrics) noexcept
        {
            if (optionCount <= 0 || availableWidth <= 0 || metrics.optionWidth <= 0
                || metrics.optionHeight <= 0)
            {
                return 0;
            }

            const int stride = metrics.optionWidth + metrics.gapX;
            int perRow = 1;
            if (stride > 0)
            {
                perRow = juce::jmax(1, (availableWidth + metrics.gapX) / stride);
                if (perRow * metrics.optionWidth + (perRow - 1) * metrics.gapX > availableWidth)
                    perRow = juce::jmax(1, perRow - 1);
            }

            const int rows = (optionCount + perRow - 1) / perRow;
            return rows * metrics.optionHeight + juce::jmax(0, rows - 1) * metrics.gapY;
        }

        inline juce::Rectangle<int> optionBounds(int optionIndex,
                                                 int optionCount,
                                                 juce::Rectangle<int> area,
                                                 const OptionMetrics& metrics) noexcept
        {
            juce::ignoreUnused(optionCount);

            if (optionIndex < 0 || area.isEmpty() || metrics.optionWidth <= 0 || metrics.optionHeight <= 0)
                return {};

            const int stride = metrics.optionWidth + metrics.gapX;
            int perRow = 1;
            if (stride > 0)
            {
                perRow = juce::jmax(1, (area.getWidth() + metrics.gapX) / stride);
                if (perRow * metrics.optionWidth + (perRow - 1) * metrics.gapX > area.getWidth())
                    perRow = juce::jmax(1, perRow - 1);
            }

            const int row = optionIndex / perRow;
            const int col = optionIndex % perRow;
            const int x = area.getX() + col * (metrics.optionWidth + metrics.gapX);
            const int y = area.getY() + row * (metrics.optionHeight + metrics.gapY);
            return { x, y, metrics.optionWidth, metrics.optionHeight };
        }

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
    }
}
