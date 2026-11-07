#pragma once

/** Shared Palette row paint / hit geometry (count column + name band). */
namespace SkinEditorPaletteGeometry
{
    constexpr int kSwatchX = 4;
    constexpr int kNameGap = 8;
    constexpr int kNameCountGap = 6;
    /** Flush count badges to a shared right edge (before scroll-track band). */
    constexpr int kCountRightInset = 2;

    inline int mappingCountRight(int paintW) noexcept
    {
        return paintW - kCountRightInset;
    }

    inline int mappingCountLeft(int paintW, int countColumnW) noexcept
    {
        return mappingCountRight(paintW) - countColumnW;
    }
}
