#pragma once

#include <array>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TestScaleColumns
{
    struct ColumnSpec
    {
        float scale;
        const char* label;
    };

    inline constexpr int kGap = 12;
    inline constexpr int kPanelGapMultiplier = 2;
    inline constexpr int kScaleLabelHeight = 20;

    // Minimum column width @ 100 % so scale labels ("200%") are not clipped.
    inline constexpr int kScaleLabelColumnDesignWidth = 56;

    inline constexpr auto kSpecs = std::array<ColumnSpec, 7>
    {{
        { 0.5f, "50%" },
        { 0.75f, "75%" },
        { 1.0f, "100%" },
        { 1.25f, "125%" },
        { 1.5f, "150%" },
        { 1.75f, "175%" },
        { 2.0f, "200%" }
    }};

    namespace detail
    {
        constexpr float sumScaleFactors()
        {
            float sum = 0.0f;
            for (const auto& spec : kSpecs)
                sum += spec.scale;
            return sum;
        }
    }

    inline constexpr float kScaleFactorSum = detail::sumScaleFactors();

    inline int bandWidthForTestRow(int availableRowWidth)
    {
        const int panelGap = kGap * kPanelGapMultiplier;
        const int gapTotal = (static_cast<int>(kSpecs.size()) - 1) * panelGap;
        return juce::jmax(0, juce::roundToInt(static_cast<float>(availableRowWidth - gapTotal) / kScaleFactorSum));
    }

    inline int scaledSize(int value, float scale)
    {
        return juce::roundToInt(static_cast<float>(value) * scale);
    }

    template <typename PanelRange, typename GetWidthFn>
    inline int sumPanelRowWidth(const PanelRange& panels, int panelGap, GetWidthFn getWidth)
    {
        int totalWidth = 0;
        int index = 0;

        for (const auto& panel : panels)
        {
            if (index > 0)
                totalWidth += panelGap;

            totalWidth += getWidth(panel);
            ++index;
        }

        return totalWidth;
    }
}
