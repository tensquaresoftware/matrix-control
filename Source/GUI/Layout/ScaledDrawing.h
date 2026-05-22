#pragma once

#include <cmath>
#include <juce_gui_basics/juce_gui_basics.h>

namespace tss
{
namespace ScaledDrawing
{
    enum class StrokeSnapPolicy
    {
        kFloor,
        kRound,
        kCeil
    };

    inline float systemDisplayScaleForComponent(const juce::Component& component)
    {
        if (const auto* display = juce::Desktop::getInstance().getDisplays().getDisplayForRect(component.getScreenBounds()))
            return juce::jmax(1.0f, static_cast<float>(display->scale));

        return 1.0f;
    }

    inline float snappedStrokeThicknessFromDesign(
        float designThickness,
        float uiScale,
        float systemDisplayScale,
        StrokeSnapPolicy policy)
    {
        const float safeUiScale = juce::jmax(0.0f, uiScale);
        const float safeDisplayScale = juce::jmax(1.0e-6f, systemDisplayScale);
        const float desiredLogical = juce::jmax(0.0f, designThickness * safeUiScale);
        const float desiredPhysical = desiredLogical * safeDisplayScale;
        float snappedPhysical = 0.0f;

        switch (policy)
        {
            case StrokeSnapPolicy::kFloor:
                snappedPhysical = std::floor(desiredPhysical);
                break;

            case StrokeSnapPolicy::kCeil:
                snappedPhysical = std::ceil(desiredPhysical);
                break;

            case StrokeSnapPolicy::kRound:
            default:
                snappedPhysical = static_cast<float>(juce::roundToInt(desiredPhysical));
                break;
        }

        snappedPhysical = juce::jmax(1.0f, snappedPhysical);
        return snappedPhysical / safeDisplayScale;
    }

    /**
     * Logical pixel size from design dimensions: design * uiScale * systemDisplayScale (rounded).
     * Prefer using this only for special cases; normal Component bounds should use uiScale alone
     * (see ScaledLayout) so Retina does not double logical widget sizes.
     */
    inline int scaledDesignPixels(float designPixels, float uiScale, float systemDisplayScale)
    {
        return juce::roundToInt(designPixels * uiScale * systemDisplayScale);
    }

    /**
     * Inset per side (logical pixels) from a design inset, for padding inside filled rects.
     * Uses floor of (designInset * uiScale * displayScale), minimum 1 logical pixel per side.
     */
    inline int logicalInsetPixelsFromDesign(float designInsetPixels, float uiScale, float systemDisplayScale)
    {
        const float safeUi = juce::jmax(0.0f, uiScale);
        const float safeDisplay = juce::jmax(1.0e-6f, systemDisplayScale);
        const float v = designInsetPixels * safeUi * safeDisplay;
        return juce::jmax(1, static_cast<int>(std::floor(v)));
    }
}
}
