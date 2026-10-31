#pragma once

/** Shared Palette row paint / hit geometry (count column + name band). */
namespace SkinEditorPaletteGeometry
{
    constexpr int kSwatchX = 4;
    constexpr int kNameGap = 8;
    constexpr int kNameCountGap = 6;
    constexpr int kCountRightInset = 4;

    inline int mappingCountLeft(int paintW, int countColumnW) noexcept
    {
        return paintW - kCountRightInset - countColumnW;
    }
}
