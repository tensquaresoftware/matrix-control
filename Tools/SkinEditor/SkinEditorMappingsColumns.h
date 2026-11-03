#pragma once

#include "Tools/SkinEditor/SkinEditorChromeTokens.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"

/** Shared Mappings table column geometry (Property|State|Colour set|Link|Mapped). */
namespace SkinEditorMappingsColumns
{
    struct Layout
    {
        int propertyW = 0;
        int stateW = 0;
        int colourSetW = 0;
        int linkW = 0;
        int mappedX = 0;
        int mappedW = 0;

        int colourSetLeft() const noexcept
        {
            return SkinEditorChrome::kMappingsColumnPad + propertyW + stateW;
        }

        int linkLeft() const noexcept
        {
            return colourSetLeft() + colourSetW;
        }
    };

    inline Layout forWidth(int totalWidth) noexcept
    {
        const int mappedSpan = juce::jmin(SkinEditorChrome::kPaletteListPaintWidth,
                                          juce::jmax(0, totalWidth));
        return { SkinEditorChrome::kMappingsPropertyColumnWidth,
                 SkinEditorChrome::kMappingsStateColumnWidth,
                 SkinEditorChrome::kMappingsColourSetColumnWidth,
                 SkinEditorChrome::kMappingsLinkColumnWidth,
                 totalWidth - mappedSpan,
                 mappedSpan };
    }

    inline bool isColourSetColumnX(int x, const Layout& cols) noexcept
    {
        return SkinEditorMappingPolicy::isColourSetColumn(x, cols.colourSetLeft(), cols.colourSetW);
    }

    inline bool isLinkColumnX(int x, const Layout& cols) noexcept
    {
        return SkinEditorMappingPolicy::isLinkColumn(x, cols.linkLeft(), cols.linkW);
    }
}
