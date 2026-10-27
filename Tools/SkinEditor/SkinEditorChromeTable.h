#pragma once

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"
#include "Tools/SkinEditor/SkinEditorChromeTokens.h"

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinEditorChrome
{
    struct TableRowFill
    {
        juce::Rectangle<float> area;
        juce::Colour fill;
        bool roundTop = false;
        bool roundBottom = false;
    };

    /** Fill a table row; only outer corners of the whole table are rounded. */
    void fillTableRow(juce::Graphics& g, TableRowFill spec);

    /** Shared folder-tab metrics for section header fill + object focus outline. */
    struct TableSectionGeom
    {
        float left = 0;
        float right = 0;
        float top = 0;
        float sectionBottom = 0;
        float railTop = 0;
        float tabW = 0;
        float tabRight = 0;
        float bevelEndX = 0;
        float r = kControlCorner;
    };

    TableSectionGeom makeTableSectionGeom(juce::Rectangle<float> sectionRow,
                                          const juce::String& title);

    /** Cubic kappa for a circular quarter-arc (matches button / fillTableRow corners). */
    inline constexpr float kCircleBez = 0.5522847498f;

    void cubicCornerTopLeft(juce::Path& path, float L, float T, float r);
    void cubicCornerTopRight(juce::Path& path, float R, float top, float r);
    void cubicCornerBottomRight(juce::Path& path, float R, float B, float r);
    void cubicCornerBottomLeft(juce::Path& path, float L, float B, float r);

    /**
     * Full object silhouette: folder-tab + body.
     * `outset` > 0 builds a parallel outer rim (corner radius grows by outset).
     */
    juce::Path makeObjectTableSilhouettePath(juce::Rectangle<float> sectionRow,
                                             float tableBottom,
                                             const juce::String& title,
                                             float outset = 0.0f);

    /** Header-only fill (tab + rail), same outer rim as makeObjectTableSilhouettePath. */
    void paintTableSectionHeader(juce::Graphics& g,
                                 juce::Rectangle<float> rowArea,
                                 const juce::String& title);

    /**
     * Focus outline: 1 px stroked path centred on a half-pixel outset of the silhouette.
     * Covers exactly one logical pixel outside the fill (Photocraft stroke-outside), with
     * uniform H/V weight — avoids the double-edge fringe of an outer−inner annulus.
     */
    void strokeObjectTableFocusOutside(juce::Graphics& g,
                                       juce::Rectangle<float> sectionRow,
                                       float tableBottom,
                                       const juce::String& title);

    /** Classic transparency checkerboard (light / mid grey cells). */
    void paintCheckerboard(juce::Graphics& g, juce::Rectangle<int> area, int cell = 3);

    /** Checkerboard then colour on top — shows alpha / fully transparent as checkerboard. */
    void paintColourSwatch(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour colour);

    /** Empty circle for pending (uninitialized) pastilles — 1 px stroke in logo grey. */
    void paintPendingColourSwatch(juce::Graphics& g, juce::Rectangle<int> area);

    /** Count badge: same light grey as button / panel-title text, dark glyph. */
    void drawCountBadge(juce::Graphics& g,
                        juce::Rectangle<int> area,
                        int count);

    int countBadgeWidth(int count);

    /**
     * Chrome text badge - Inter rectangle (MAPPING / BRIDGE / LAST PUSH / APP).
     * Do not reuse plugin FooterPanel badge APIs or count-capsule badges.
     */
    void drawChromeBadge(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& text);

    int chromeBadgeWidth(const juce::String& text);

    inline void drawMappingModeBadge(juce::Graphics& g, juce::Rectangle<int> area)
    {
        drawChromeBadge(g, area, "MAPPING");
    }

    inline int mappingModeBadgeWidth() { return chromeBadgeWidth("MAPPING"); }
}
