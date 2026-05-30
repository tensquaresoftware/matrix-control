#pragma once

// ScaledLayout.h
// Helpers for consistent integer bounds from design pixels and UI display scale.
// Convention: scaledInt uses roundToInt(designPx * uiScale).

#include <juce_core/juce_core.h>

#include <vector>

namespace tss
{
namespace ScaledLayout
{
    inline float uiScaleFromEditorBounds(int editorWidth, int designWidth)
    {
        if (designWidth <= 0)
            return 1.0f;

        return static_cast<float>(editorWidth) / static_cast<float>(designWidth);
    }

    inline int scaledInt(float designPixels, float uiScale)
    {
        return juce::roundToInt(designPixels * uiScale);
    }

    /** Split totalHeight into count integer row heights that sum exactly; adjacent rows differ by at most 1 px. */
    inline std::vector<int> equalSplitHeights(int totalHeight, size_t count)
    {
        if (count == 0)
            return {};

        jassert(totalHeight >= 0);
        std::vector<int> out(count);
        const int n = static_cast<int>(count);

        for (size_t i = 0; i < count; ++i)
        {
            const int start = (totalHeight * static_cast<int>(i)) / n;
            const int end = (totalHeight * static_cast<int>(i + 1)) / n;
            out[i] = end - start;
        }

        return out;
    }

    /**
     * Rows 0..n-2 use scaledInt(designRowHeight, uiScale); last row absorbs the remainder (sum == totalHeight).
     * Matches mockup alignment when paired sections share the same design row height (e.g. ParameterCell vs ModulationBusCell).
     * Falls back to equalSplitHeights only when the last row would be < 1 px (cannot place n rows with fixed heads).
     */
    inline std::vector<int> distributeFixedDesignRowsWithRemainderOnLast(
        int totalHeight,
        size_t rowCount,
        int designRowHeight,
        float uiScale)
    {
        if (rowCount == 0)
            return {};
        if (rowCount == 1)
            return { totalHeight };

        jassert(totalHeight >= 0);
        const int hRow = scaledInt(static_cast<float>(designRowHeight), uiScale);
        const int fixedPart = static_cast<int>(rowCount - 1) * hRow;
        const int lastHeight = totalHeight - fixedPart;
        if (lastHeight < 1)
            return equalSplitHeights(totalHeight, rowCount);

        std::vector<int> out(rowCount);
        for (size_t i = 0; i < rowCount - 1; ++i)
            out[i] = hRow;
        out[rowCount - 1] = lastHeight;
        return out;
    }

    /**
     * Every row uses the same scaled design height; extra space stays at the bottom (sum of heights may be < totalHeight).
     * Use for module parameter stacks where the last row must not stretch (Patch Edit modules).
     * If totalHeight is too small for n scaled rows, falls back to equalSplitHeights.
     */
    inline std::vector<int> distributeFixedDesignRowsWithRemainderAtBottom(
        int totalHeight,
        size_t rowCount,
        int designRowHeight,
        float uiScale)
    {
        if (rowCount == 0)
            return {};

        jassert(totalHeight >= 0);
        const int hRow = scaledInt(static_cast<float>(designRowHeight), uiScale);
        const int fixedTotal = static_cast<int>(rowCount) * hRow;

        if (totalHeight < fixedTotal)
            return equalSplitHeights(totalHeight, rowCount);

        if (rowCount == 1)
            return { hRow };

        std::vector<int> out(rowCount);
        for (size_t i = 0; i < rowCount; ++i)
            out[i] = hRow;

        return out;
    }

    /** Fixed spans are rounded; flexibleIndex absorbs remainder so sum == totalHeight. */
    inline std::vector<int> distributeHeights(
        int totalHeight,
        const std::vector<int>& designHeights,
        float uiScale,
        size_t flexibleIndex)
    {
        jassert(!designHeights.empty());
        jassert(flexibleIndex < designHeights.size());

        std::vector<int> out(designHeights.size());
        int fixedSum = 0;

        for (size_t i = 0; i < designHeights.size(); ++i)
        {
            if (i == flexibleIndex)
                continue;

            const int h = scaledInt(static_cast<float>(designHeights[i]), uiScale);
            out[i] = h;
            fixedSum += h;
        }

        out[flexibleIndex] = totalHeight - fixedSum;
        jassert(out[flexibleIndex] >= 0);
        return out;
    }

    /** Fixed spans are rounded; flexibleIndex absorbs remainder so sum == totalWidth. */
    inline std::vector<int> distributeWidths(
        int totalWidth,
        const std::vector<int>& designWidths,
        float uiScale,
        size_t flexibleIndex)
    {
        jassert(!designWidths.empty());
        jassert(flexibleIndex < designWidths.size());

        std::vector<int> out(designWidths.size());
        int fixedSum = 0;

        for (size_t i = 0; i < designWidths.size(); ++i)
        {
            if (i == flexibleIndex)
                continue;

            const int w = scaledInt(static_cast<float>(designWidths[i]), uiScale);
            out[i] = w;
            fixedSum += w;
        }

        out[flexibleIndex] = totalWidth - fixedSum;
        jassert(out[flexibleIndex] >= 0);
        return out;
    }
}
}
