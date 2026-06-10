#pragma once

// ScaledLayout.h
// Helpers for consistent integer bounds from design pixels and UI display scale.
// Convention: scaledInt uses roundToInt(designPx * uiScale).
// Plugin shell layout uses fixed Figma design dimensions only — no flexible distribution.

#include <juce_core/juce_core.h>

namespace TSS
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
}
}
