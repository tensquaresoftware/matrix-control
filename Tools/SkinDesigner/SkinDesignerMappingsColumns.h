#pragma once

#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"
#include "Tools/SkinDesigner/SkinDesignerMappingModePolicy.h"

/** Shared Mappings table column geometry (Property|State|Colour set|Link|Mapped). */
namespace SkinDesignerMappingsColumns
{
    struct Layout
    {
        int propertyW = 0;
        int stateW = 0;
        int colourSetW = 0;
        int linkW = 0;
        int mappedX = 0;
        int mappedW = 0;

        int propertyLeft() const noexcept { return 0; }

        int stateLeft() const noexcept { return propertyW; }

        int colourSetLeft() const noexcept { return propertyW + stateW; }

        int linkLeft() const noexcept { return colourSetLeft() + colourSetW; }

        /** Left edge for text / chips inside a column (after uniform cell pad). */
        int contentLeft(int columnLeft) const noexcept
        {
            return columnLeft + SkinDesignerChrome::kMappingsColumnPad;
        }

        /** Usable width inside a column (minus left + right cell pad). */
        int contentWidth(int columnW) const noexcept
        {
            return juce::jmax(0, columnW - 2 * SkinDesignerChrome::kMappingsColumnPad);
        }
    };

    inline Layout forWidth(int totalWidth) noexcept
    {
        // Fixed leading columns; Mapped colour absorbs the remainder (tracks Palette width).
        const int mappedX = SkinDesignerChrome::kMappingsFixedColumnsWidth;
        const int mappedSpan = juce::jmax(0, totalWidth - mappedX);
        return { SkinDesignerChrome::kMappingsPropertyColumnWidth,
                 SkinDesignerChrome::kMappingsStateColumnWidth,
                 SkinDesignerChrome::kMappingsColourSetColumnWidth,
                 SkinDesignerChrome::kMappingsLinkColumnWidth,
                 mappedX,
                 mappedSpan };
    }

    inline bool isPropertyColumnX(int x, const Layout& cols) noexcept
    {
        return SkinDesignerMappingPolicy::isPropertyColumn(x, cols.propertyW);
    }

    inline bool isColourSetColumnX(int x, const Layout& cols) noexcept
    {
        return SkinDesignerMappingPolicy::isColourSetColumn(x, cols.colourSetLeft(), cols.colourSetW);
    }

    inline bool isLinkColumnX(int x, const Layout& cols) noexcept
    {
        return SkinDesignerMappingPolicy::isLinkColumn(x, cols.linkLeft(), cols.linkW);
    }

    /** Args for fillCutColumnSlabs (keeps the call under the parameter gate). */
    struct CutColumnFill
    {
        juce::Rectangle<int> area;
        juce::Colour fill;
        bool roundTop = false;
        bool roundBottom = false;
        /** Inclusive start index (0 = Property … 4 = Mapped). */
        int firstColumn = 0;
        /** Inclusive end index (default = Mapped). */
        int lastColumn = 4;
        /** Horizontal / vertical inset (row / Property bind highlights). */
        int insetX = 0;
        int insetY = 0;
        /** Corner radius (body slabs use control corner; highlights use a quieter one). */
        float cornerRadius = SkinDesignerChrome::kControlCorner;
    };

    /**
     * Fill each column as its own rounded slab. Trailing cutter thickness is left
     * empty (panel shows through) so the next column keeps its full leading pad.
     */
    inline void fillCutColumnSlabs(juce::Graphics& g,
                                   const Layout& cols,
                                   CutColumnFill spec) noexcept
    {
        if (spec.area.getHeight() <= 0)
            return;

        const int thickness = SkinDesignerChrome::kMappingsColumnDividerThickness;
        const float r = juce::jmax(0.0f, spec.cornerRadius);
        const int lefts[5] = {
            cols.propertyLeft(),
            cols.stateLeft(),
            cols.colourSetLeft(),
            cols.linkLeft(),
            cols.mappedX
        };
        const int rights[5] = {
            cols.stateLeft() - thickness,
            cols.colourSetLeft() - thickness,
            cols.linkLeft() - thickness,
            cols.mappedX - thickness,
            spec.area.getWidth()
        };

        g.setColour(spec.fill);
        const int start = juce::jlimit(0, 4, spec.firstColumn);
        const int end = juce::jlimit(start, 4, spec.lastColumn);
        const int insetX = juce::jmax(0, spec.insetX);
        const int insetY = juce::jmax(0, spec.insetY);
        for (int i = start; i <= end; ++i)
        {
            const int w = rights[i] - lefts[i] - 2 * insetX;
            const int h = spec.area.getHeight() - 2 * insetY;
            if (w <= 0 || h <= 0)
                continue;
            juce::Path path;
            path.addRoundedRectangle((float) (spec.area.getX() + lefts[i] + insetX),
                                     (float) (spec.area.getY() + insetY),
                                     (float) w,
                                     (float) h,
                                     r, r,
                                     spec.roundTop, spec.roundTop,
                                     spec.roundBottom, spec.roundBottom);
            g.fillPath(path);
        }
    }

    /** Property cut-slab horizontal span (local X / width within table width). */
    inline juce::Rectangle<int> propertyCutSlab(const Layout& cols) noexcept
    {
        const int thickness = SkinDesignerChrome::kMappingsColumnDividerThickness;
        return { cols.propertyLeft(), 0,
                 juce::jmax(0, cols.stateLeft() - thickness - cols.propertyLeft()), 0 };
    }
}
