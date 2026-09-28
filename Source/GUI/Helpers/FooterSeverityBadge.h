#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/Design/DesignPanels.h"

namespace TSS
{
    /** Unit-square close cross — slightly thicker than Settings/About for footer readability. */
    inline juce::Path makeSeverityCloseCrossShape()
    {
        juce::Path shape;
        constexpr float crossThickness = 0.22f;
        shape.addLineSegment({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
        shape.addLineSegment({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);
        return shape;
    }

    /**
     * Discrete separator thickness by UI Scale preset (not a floating stroke scale):
     * 50%/75% → 1 px, 100%/125% → design base (2 px), 150%/175% → 3 px, 200% → 4 px.
     */
    inline int stickySeveritySeparatorThickness(float uiScale)
    {
        if (uiScale >= 2.0f)
            return 4;
        if (uiScale >= 1.5f)
            return 3;
        if (uiScale >= 1.0f)
            return Design::Panels::Footer::kSeverityBadgeSeparatorThickness;
        return 1;
    }

    /** Metrics for sticky severity chrome: [close square][separator][label + pad]. */
    struct StickySeverityBadgeMetrics
    {
        int closeSide = 0;
        int separatorThickness = 0;
        int labelWidth = 0;
        int badgePad = 0;
        int badgeHeight = 0;
    };

    inline int stickySeverityCloseStripWidth(const StickySeverityBadgeMetrics& metrics)
    {
        return juce::jmax(0, metrics.closeSide) + juce::jmax(0, metrics.separatorThickness);
    }

    /** Layout width is hover-stable (no CLOSE label swap). */
    inline int stickySeverityBadgeWidth(const StickySeverityBadgeMetrics& metrics)
    {
        const int textWidth = juce::jmax(0, metrics.labelWidth)
                              + 2 * juce::jmax(0, metrics.badgePad);
        return stickySeverityCloseStripWidth(metrics) + textWidth;
    }

    inline juce::Rectangle<int> stickySeverityBadgeBounds(juce::Rectangle<int> bandBounds,
                                                          const StickySeverityBadgeMetrics& metrics)
    {
        const int width = juce::jmin(bandBounds.getWidth(), stickySeverityBadgeWidth(metrics));
        const int y = bandBounds.getCentreY() - metrics.badgeHeight / 2;
        return { bandBounds.getX(), y, width, metrics.badgeHeight };
    }

    /** Close square — left of chrome, side = badge height when space allows. */
    inline juce::Rectangle<int> stickySeverityCloseSquareBounds(
        juce::Rectangle<int> badgeBounds,
        const StickySeverityBadgeMetrics& metrics)
    {
        const int side = juce::jmin(juce::jmax(0, metrics.closeSide),
                                    juce::jmax(0, metrics.badgeHeight),
                                    juce::jmax(0, badgeBounds.getWidth()));
        return {
            badgeBounds.getX(),
            badgeBounds.getY() + (metrics.badgeHeight - side) / 2,
            side,
            side
        };
    }

    inline juce::Rectangle<int> stickySeveritySeparatorBounds(
        juce::Rectangle<int> badgeBounds,
        const StickySeverityBadgeMetrics& metrics)
    {
        const auto square = stickySeverityCloseSquareBounds(badgeBounds, metrics);
        const int thickness = juce::jmin(juce::jmax(0, metrics.separatorThickness),
                                         juce::jmax(0, badgeBounds.getRight() - square.getRight()));
        return {
            square.getRight(),
            badgeBounds.getY(),
            thickness,
            metrics.badgeHeight
        };
    }

    struct SeverityCloseGlyphPaintArgs
    {
        juce::Rectangle<int> square;
        const juce::Path& unitGlyph;
        juce::Colour glyphColour;
    };

    /** Paint permanent close cross inside an already-filled square (no second fill). */
    inline void paintSeverityCloseGlyphInSquare(juce::Graphics& g,
                                                const SeverityCloseGlyphPaintArgs& args)
    {
        if (args.unitGlyph.isEmpty() || args.square.isEmpty())
            return;

        auto glyph = args.unitGlyph;
        // Slightly smaller than the former 0.18 inset so the bold cross reads with margin.
        const float inset = static_cast<float>(args.square.getWidth()) * 0.26f;
        glyph.scaleToFit(static_cast<float>(args.square.getX()) + inset,
                         static_cast<float>(args.square.getY()) + inset,
                         static_cast<float>(args.square.getWidth()) - 2.0f * inset,
                         static_cast<float>(args.square.getHeight()) - 2.0f * inset,
                         true);
        g.setColour(args.glyphColour);
        g.fillPath(glyph);
    }
}
