#include "Tools/SkinEditor/SkinEditorChromeTable.h"

namespace SkinEditorChrome
{
    void fillTableRow(juce::Graphics& g, TableRowFill spec)
    {
        juce::Path path;
        path.addRoundedRectangle(spec.area.getX(), spec.area.getY(),
                                 spec.area.getWidth(), spec.area.getHeight(),
                                 kControlCorner, kControlCorner,
                                 spec.roundTop, spec.roundTop, spec.roundBottom, spec.roundBottom);
        g.setColour(spec.fill);
        g.fillPath(path);
    }

    TableSectionGeom makeTableSectionGeom(juce::Rectangle<float> sectionRow,
                                          const juce::String& title)
    {
        TableSectionGeom g;
        g.r = kControlCorner;
        g.left = sectionRow.getX();
        g.right = sectionRow.getRight();
        g.top = sectionRow.getY();
        g.sectionBottom = sectionRow.getBottom();
        g.railTop = g.sectionBottom - (float) kTableSectionRailHeight;
        const float padH = (float) kTableSectionTabPadH;
        g.tabW = juce::jmin(sectionRow.getWidth(),
                            (float) measureTextWidth(fontBold(), title) + padH * 2.0f);
        const float drop = g.railTop - g.top;
        g.tabRight = g.left + juce::jmax(g.r * 2.0f, g.tabW - kTabBevelStartEarly);
        g.bevelEndX = g.tabRight + drop;
        return g;
    }

    void cubicCornerTopLeft(juce::Path& path, float L, float T, float r)
    {
        // Arriving up the left edge → leaving along the top edge.
        path.cubicTo(L, T + r * (1.0f - kCircleBez),
                     L + r * (1.0f - kCircleBez), T,
                     L + r, T);
    }

    void cubicCornerTopRight(juce::Path& path, float R, float top, float r)
    {
        // Arriving along the top → leaving down the right edge.
        path.cubicTo(R - r * (1.0f - kCircleBez), top,
                     R, top + r * (1.0f - kCircleBez),
                     R, top + r);
    }

    void cubicCornerBottomRight(juce::Path& path, float R, float B, float r)
    {
        path.cubicTo(R, B - r * (1.0f - kCircleBez),
                     R - r * (1.0f - kCircleBez), B,
                     R - r, B);
    }

    void cubicCornerBottomLeft(juce::Path& path, float L, float B, float r)
    {
        path.cubicTo(L + r * (1.0f - kCircleBez), B,
                     L, B - r * (1.0f - kCircleBez),
                     L, B - r);
    }

    juce::Path makeObjectTableSilhouettePath(juce::Rectangle<float> sectionRow,
                                             float tableBottom,
                                             const juce::String& title,
                                             float outset)
    {
        const auto g = makeTableSectionGeom(sectionRow, title);
        const float L = g.left - outset;
        const float R = g.right + outset;
        const float T = g.top - outset;
        const float B = tableBottom + outset;
        const float r = g.r + outset;
        const float railTop = g.railTop - outset;
        const float tabRight = g.tabRight;
        const float bevelEndX = g.bevelEndX;

        juce::Path path;
        path.startNewSubPath(L, T + r);
        cubicCornerTopLeft(path, L, T, r);
        path.lineTo(tabRight - r, T);
        path.quadraticTo(tabRight, T, tabRight + r, T + r);
        if (bevelEndX + r < R)
        {
            path.lineTo(bevelEndX - r, railTop - r); // 45° (Δx == Δy)
            path.quadraticTo(bevelEndX, railTop, bevelEndX + r, railTop);
            path.lineTo(R - r, railTop);
            cubicCornerTopRight(path, R, railTop, r);
        }
        else
        {
            path.lineTo(R, railTop + r);
        }
        path.lineTo(R, B - r);
        cubicCornerBottomRight(path, R, B, r);
        path.lineTo(L + r, B);
        cubicCornerBottomLeft(path, L, B, r);
        path.closeSubPath();
        return path;
    }

    void paintTableSectionHeader(juce::Graphics& g,
                                 juce::Rectangle<float> rowArea,
                                 const juce::String& title)
    {
        const auto geom = makeTableSectionGeom(rowArea, title);
        juce::Path header;
        header.startNewSubPath(geom.left, geom.top + geom.r);
        cubicCornerTopLeft(header, geom.left, geom.top, geom.r);
        header.lineTo(geom.tabRight - geom.r, geom.top);
        header.quadraticTo(geom.tabRight, geom.top, geom.tabRight + geom.r, geom.top + geom.r);
        if (geom.bevelEndX + geom.r < geom.right)
        {
            header.lineTo(geom.bevelEndX - geom.r, geom.railTop - geom.r);
            header.quadraticTo(geom.bevelEndX, geom.railTop, geom.bevelEndX + geom.r, geom.railTop);
            header.lineTo(geom.right - geom.r, geom.railTop);
            cubicCornerTopRight(header, geom.right, geom.railTop, geom.r);
            header.lineTo(geom.right, geom.sectionBottom);
        }
        else
        {
            header.lineTo(geom.right, geom.sectionBottom);
        }
        header.lineTo(geom.left, geom.sectionBottom);
        header.closeSubPath();

        g.setColour(surfaceTableHeader());
        g.fillPath(header);

        g.setColour(textTableHeader());
        g.setFont(fontBold());
        g.drawText(title,
                   juce::Rectangle<float>(geom.left, geom.top, geom.tabW, rowArea.getHeight())
                       .toNearestIntEdges(),
                   juce::Justification::centred, false);
    }

    void strokeObjectTableFocusOutside(juce::Graphics& g,
                                       juce::Rectangle<float> sectionRow,
                                       float tableBottom,
                                       const juce::String& title)
    {
        const auto snapped = juce::Rectangle<float>((float) juce::roundToInt(sectionRow.getX()),
                                                    (float) juce::roundToInt(sectionRow.getY()),
                                                    (float) juce::roundToInt(sectionRow.getWidth()),
                                                    (float) juce::roundToInt(sectionRow.getHeight()));
        const float bottom = (float) juce::roundToInt(tableBottom);

        // Midline of a 1 px outside stroke = silhouette outset by 0.5.
        const auto mid = makeObjectTableSilhouettePath(snapped, bottom, title, 0.5f);
        juce::Path band;
        juce::PathStrokeType(1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded)
            .createStrokedPath(band, mid);

        g.setColour(controlOutline());
        g.fillPath(band);
    }

    void paintCheckerboard(juce::Graphics& g, juce::Rectangle<int> area, int cell)
    {
        g.setColour(juce::Colour(0xffd0d0d0));
        g.fillRect(area);
        g.setColour(juce::Colour(0xffa8a8a8));
        for (int y = area.getY(); y < area.getBottom(); y += cell)
            for (int x = area.getX(); x < area.getRight(); x += cell)
                if (((x - area.getX()) / cell + (y - area.getY()) / cell) & 1)
                    g.fillRect(x, y,
                               juce::jmin(cell, area.getRight() - x),
                               juce::jmin(cell, area.getBottom() - y));
    }

    void paintColourSwatch(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour colour)
    {
        paintCheckerboard(g, area);
        g.setColour(colour);
        g.fillRect(area);
    }

    void drawCountBadge(juce::Graphics& g,
                        juce::Rectangle<int> area,
                        int count)
    {
        if (count < 0)
            count = 0;
        const auto text = juce::String(count);
        const auto font = fontBold(kCountBadgeFont);
        const int textW = measureTextWidth(font, text);
        const int diameter = kCountBadgeDiameter;
        const int width = (text.length() <= 1) ? diameter
                                              : juce::jmax(diameter, textW + kCountBadgeCapsulePad);
        auto badge = juce::Rectangle<int>(area.getX(),
                                          area.getCentreY() - diameter / 2,
                                          width,
                                          diameter);
        g.setColour(textPrimary());
        g.fillRoundedRectangle(badge.toFloat(), (float) diameter * 0.5f);
        g.setColour(surfacePanel());
        g.setFont(font);
        g.drawText(text, badge, juce::Justification::centred, false);
    }

    int countBadgeWidth(int count)
    {
        if (count < 0)
            count = 0;
        const auto text = juce::String(count);
        const int textW = measureTextWidth(fontBold(kCountBadgeFont), text);
        const int diameter = kCountBadgeDiameter;
        return (text.length() <= 1) ? diameter
                                    : juce::jmax(diameter, textW + kCountBadgeCapsulePad);
    }
}
