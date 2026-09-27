#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/StickyInfoMessagePolicy.h"

namespace TSS
{
    /** Unit-square close cross — same geometry as Settings/About close Path. */
    inline juce::Path makeSeverityCloseCrossShape()
    {
        juce::Path shape;
        constexpr float crossThickness = 0.15f;
        shape.addLineSegment({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
        shape.addLineSegment({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);
        return shape;
    }

    /** Info picto: stem + top dot in unit square. */
    inline juce::Path makeSeverityInfoPictoShape()
    {
        juce::Path shape;
        shape.addEllipse(0.38f, 0.12f, 0.24f, 0.24f);
        shape.addRectangle(0.42f, 0.42f, 0.16f, 0.46f);
        return shape;
    }

    /** Warning picto: filled triangle pointing up. */
    inline juce::Path makeSeverityWarningPictoShape()
    {
        juce::Path shape;
        shape.addTriangle(0.5f, 0.08f, 0.92f, 0.92f, 0.08f, 0.92f);
        return shape;
    }

    /** Error picto: octagon outline filled as a regular octagon. */
    inline juce::Path makeSeverityErrorPictoShape()
    {
        juce::Path shape;
        constexpr float a = 0.12f;
        constexpr float b = 0.38f;
        constexpr float c = 0.62f;
        constexpr float d = 0.88f;
        shape.startNewSubPath(b, a);
        shape.lineTo(c, a);
        shape.lineTo(d, b);
        shape.lineTo(d, c);
        shape.lineTo(c, d);
        shape.lineTo(b, d);
        shape.lineTo(a, c);
        shape.lineTo(a, b);
        shape.closeSubPath();
        return shape;
    }

    inline juce::Path severityPictoFor(StickyMessageSeverity severity)
    {
        switch (severity)
        {
            case StickyMessageSeverity::Info:
                return makeSeverityInfoPictoShape();
            case StickyMessageSeverity::Warning:
                return makeSeverityWarningPictoShape();
            case StickyMessageSeverity::Error:
                return makeSeverityErrorPictoShape();
            case StickyMessageSeverity::None:
            default:
                return {};
        }
    }

    /** Metrics for sticky severity badge: [inset + icon square][label + horizontal padding]. */
    struct StickySeverityBadgeMetrics
    {
        int iconSide = 0;
        int iconInset = 0;
        int labelWidth = 0;
        int badgePad = 0;
        int badgeHeight = 0;
    };

    /** Left strip holding the uniform inset + icon square (no right inset before label). */
    inline int stickySeverityIconStripWidth(const StickySeverityBadgeMetrics& metrics)
    {
        return juce::jmax(0, metrics.iconInset) + juce::jmax(0, metrics.iconSide);
    }

    /** Hover changes only the glyph inside the square; label width stays fixed. */
    inline int stickySeverityBadgeWidth(const StickySeverityBadgeMetrics& metrics)
    {
        const int textWidth = juce::jmax(0, metrics.labelWidth)
                              + 2 * juce::jmax(0, metrics.badgePad);
        return stickySeverityIconStripWidth(metrics) + textWidth;
    }

    inline juce::Rectangle<int> stickySeverityBadgeBounds(juce::Rectangle<int> bandBounds,
                                                          const StickySeverityBadgeMetrics& metrics)
    {
        const int width = juce::jmin(bandBounds.getWidth(), stickySeverityBadgeWidth(metrics));
        const int y = bandBounds.getCentreY() - metrics.badgeHeight / 2;
        return { bandBounds.getX(), y, width, metrics.badgeHeight };
    }

    /** Icon square inset from badge left / top / bottom by the same uniform inset. */
    inline juce::Rectangle<int> stickySeverityIconSquareBounds(
        juce::Rectangle<int> badgeBounds,
        const StickySeverityBadgeMetrics& metrics)
    {
        const int inset = juce::jmax(0, metrics.iconInset);
        const int side = juce::jmin(juce::jmax(0, metrics.iconSide),
                                    juce::jmax(0, metrics.badgeHeight - 2 * inset),
                                    juce::jmax(0, badgeBounds.getWidth() - 2 * inset));
        return {
            badgeBounds.getX() + inset,
            badgeBounds.getY() + inset,
            side,
            side
        };
    }

    struct SeverityIconPaintArgs
    {
        juce::Rectangle<int> square;
        const juce::Path& unitGlyph;
        juce::Colour glyphColour;
    };

    /** Paint picto or close cross inside an already-filled square (no second fill). */
    inline void paintSeverityGlyphInSquare(juce::Graphics& g, const SeverityIconPaintArgs& args)
    {
        if (args.unitGlyph.isEmpty() || args.square.isEmpty())
            return;

        auto glyph = args.unitGlyph;
        const float inset = static_cast<float>(args.square.getWidth()) * 0.18f;
        glyph.scaleToFit(static_cast<float>(args.square.getX()) + inset,
                         static_cast<float>(args.square.getY()) + inset,
                         static_cast<float>(args.square.getWidth()) - 2.0f * inset,
                         static_cast<float>(args.square.getHeight()) - 2.0f * inset,
                         true);
        g.setColour(args.glyphColour);
        g.fillPath(glyph);
    }
}
