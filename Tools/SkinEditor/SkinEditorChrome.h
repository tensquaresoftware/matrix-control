#pragma once

#include <PluginFontsData.h>

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinEditorChrome
{
    // Photocraft / Skin Studio gutter spirit (neutral greys).
    inline juce::Colour surfaceApp() { return juce::Colour(0xff151515); }
    inline juce::Colour surfacePanel() { return juce::Colour(0xff262626); }
    inline juce::Colour surfaceGutter() { return juce::Colour(0xff101010); }
    inline juce::Colour surfaceBand() { return juce::Colour(0xff1a1a1a); }
    /** Dark band for section titles (Product colours, sticky legend, …). */
    inline juce::Colour surfaceSection() { return juce::Colour(0xff151515); }
    /**
     * Object / table headers inside Mappings (e.g. envelope group title row).
     * Lighter than data rows; between the previous 0x33 step and the old 0x45.
     */
    inline juce::Colour surfaceTableHeader() { return juce::Colour(0xff3a3a3a); }
    /**
     * Mapping table data rows — darker than panel, lighter than surfaceSection.
     */
    inline juce::Colour surfaceTable() { return juce::Colour(0xff1c1c1c); }
    /** Selected mapping row — slight lift above surfaceTable. */
    inline juce::Colour surfaceTableRowHighlight() { return juce::Colour(0xff252525); }
    /** Raised tab chip (Photocraft active tab). */
    inline juce::Colour surfaceTab() { return juce::Colour(0xff323232); }
    inline juce::Colour textPrimary() { return juce::Colour(0xffdddddd); }
    inline juce::Colour textMuted() { return juce::Colour(0xff999999); }
    /** Plugin logo / DarkPanel text — ColourChart::kGrey200. */
    inline juce::Colour textBrand() { return juce::Colour(0xffb9b9b9); }
    /** Section titles + table column headers (lighter than textMuted). */
    inline juce::Colour textTableHeader() { return juce::Colour(0xffb3b3b3); }
    /** Table data-row body text (darker than textPrimary). */
    inline juce::Colour textTableBody() { return juce::Colour(0xffc4c4c4); }
    inline juce::Colour accent() { return juce::Colour(0xff00ddad); }
    inline juce::Colour separatorLine() { return juce::Colour(0xff3a3a3a); }
    inline juce::Colour controlFill() { return surfacePanel(); }
    inline juce::Colour controlOutline() { return juce::Colour(0xff555555); }
    inline juce::Colour controlFillHover() { return juce::Colour(0xff4a4a4a); }
    inline juce::Colour controlFillDown() { return juce::Colour(0xff3a3a3a); }

    /** Skin Editor marketing / chrome version label (keep in sync with project VERSION). */
    inline constexpr const char* kAppVersionLabel = "v1.0.0";

    /** Window / inter-panel gutters only — never reuse as in-panel content inset. */
    inline constexpr int kGutter = 6;
    /** Fixed body-column widths — Mappings / Unbound share the leftover. */
    inline constexpr int kColorPanelWidth = 260;
    inline constexpr int kPalettePanelWidth = 150;
    /** Document window maximum width (height unconstrained here). */
    inline constexpr int kWindowMaxWidth = 1210;
    /** Title-bar chrome inset (not panel body content). */
    inline constexpr int kPanelPad = 8;
    /**
     * In-panel content inset on all four sides of the body (below the title créneau,
     * and left / right / bottom inside the silhouette). Do not use kGutter here.
     */
    inline constexpr int kPanelContentPad = 10;
    inline constexpr int kTitleBarHeight = 32;
    inline constexpr int kButtonHeight = 24;
    inline constexpr int kButtonGap = 8;
    inline constexpr int kPanelTitleHeight = 26;
    /** Top gutter + title band + bottom gutter — matches other panels' title offset. */
    inline constexpr int kSkinsPanelHeight = kGutter + kPanelTitleHeight + kGutter;
    inline constexpr int kTitleTabPadH = 10;
    /** Sticky column-legend / table content row height. */
    inline constexpr int kColumnHeaderHeight = 22;
    /** Thin rail extending right of a Mappings object-header tab. */
    inline constexpr int kTableSectionRailHeight = 7;
    /** Horizontal pad inside the object-header tab chip. */
    inline constexpr int kTableSectionTabPadH = 8;
    /** Begin the 45° folder-tab shoulder this many px earlier than the measured title width. */
    inline constexpr float kTabBevelStartEarly = 5.0f;
    /** Gap under the sticky Mappings legend, included in the sticky band. */
    inline constexpr int kColumnHeaderGap = 11;
    /** Empty space between object tables (ListBox slot = half a content row). */
    inline constexpr int kTableGapHeight = 11;
    inline constexpr float kPanelTitleFont = 15.0f;
    /** App chrome title — same face as plugin logo (Orbitron Bold), smaller than kLogoFontHeight 27. */
    inline constexpr float kAppTitleFont = 18.0f;
    inline constexpr float kBodyFont = 13.0f;
    /** Match ComboBox corner radius (LookAndFeel_V4 default is 3). */
    inline constexpr float kControlCorner = 3.0f;
    /** Visible scrollbar thumb thickness (centred in the track). */
    inline constexpr int kScrollThumbThickness = 6;
    /**
     * Vertical scroll track width when centred in the panel's right content-pad band:
     * former default track (8) + right content pad.
     */
    inline constexpr int kScrollTrackWidth = 8 + kPanelContentPad;
    /** Count badge circle / capsule height (also used for multi-digit width floor). */
    inline constexpr int kCountBadgeDiameter = 15;
    inline constexpr float kCountBadgeFont = 11.0f;
    inline constexpr int kCountBadgeCapsulePad = 10;

    inline juce::Typeface::Ptr interRegularTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::InterRegular_ttf,
            static_cast<size_t>(PluginFontsData::InterRegular_ttfSize));
        return tf;
    }

    inline juce::Typeface::Ptr interMediumTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::InterMedium_ttf,
            static_cast<size_t>(PluginFontsData::InterMedium_ttfSize));
        return tf;
    }

    inline juce::Typeface::Ptr interBoldTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::InterBold_ttf,
            static_cast<size_t>(PluginFontsData::InterBold_ttfSize));
        return tf;
    }

    /** Same brand face as the plugin logo (Skin::getBrandFontBold). */
    inline juce::Typeface::Ptr orbitronBoldTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::OrbitronBold_ttf,
            static_cast<size_t>(PluginFontsData::OrbitronBold_ttfSize));
        return tf;
    }

    inline juce::Typeface::Ptr monoTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::JetBrainsMonoRegular_ttf,
            static_cast<size_t>(PluginFontsData::JetBrainsMonoRegular_ttfSize));
        return tf;
    }

    inline juce::Font fontRegular(float height = kBodyFont)
    {
        return juce::Font(juce::FontOptions(interRegularTypeface()).withHeight(height));
    }

    inline juce::Font fontBold(float height = kBodyFont)
    {
        return juce::Font(juce::FontOptions(interBoldTypeface()).withHeight(height));
    }

    /** Panel titles — Inter Medium (500). */
    inline juce::Font fontTitle(float height = kPanelTitleFont)
    {
        return juce::Font(juce::FontOptions(interMediumTypeface()).withHeight(height));
    }

    /** App window title — Orbitron Bold, smaller than the plugin logo. */
    inline juce::Font fontAppTitle(float height = kAppTitleFont)
    {
        return juce::Font(juce::FontOptions(orbitronBoldTypeface()).withHeight(height));
    }

    inline juce::Font fontMono(float height = kBodyFont)
    {
        return juce::Font(juce::FontOptions(monoTypeface()).withHeight(height));
    }

    /**
     * Prevent JUCE from horizontally compressing glyphs when bounds are tight.
     * Also clears the default Label border — otherwise measured widths look fine
     * but the text is clipped by ~5px padding on each side.
     */
    inline void preventTextSquish(juce::Label& label)
    {
        label.setMinimumHorizontalScale(1.0f);
        label.setBorderSize({});
    }

    inline void stylePanelTitle(juce::Label& label, const juce::String& titleUpper)
    {
        label.setText(titleUpper, juce::dontSendNotification);
        label.setFont(fontTitle());
        label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        label.setColour(juce::Label::textColourId, textPrimary());
        label.setJustificationType(juce::Justification::centred);
        preventTextSquish(label);
    }

    inline int measureTextWidth(const juce::Font& font, const juce::String& text)
    {
        return juce::GlyphArrangement::getStringWidthInt(font, text);
    }

    inline int measureButtonWidth(const juce::TextButton& button, int pad = 18)
    {
        return juce::jmax(52, measureTextWidth(fontRegular(), button.getButtonText()) + pad);
    }

    inline int measureLabelWidth(const juce::String& text, const juce::Font& font = fontRegular())
    {
        return juce::jmax(14, measureTextWidth(font, text) + 8);
    }

    inline int titleTabWidth(const juce::String& title, int extraInsideTab = 0)
    {
        return measureTextWidth(fontTitle(), title) + kTitleTabPadH * 2 + extraInsideTab;
    }

    struct PanelChrome
    {
        juce::Rectangle<int> tab;
        juce::Rectangle<int> content;
    };

    /**
     * Outer kGutter above/below the silhouette (window bg). Créneau flush left.
     * Body content inset with kPanelContentPad on top (under title), left, right, bottom.
     */
    inline PanelChrome layoutPanel(juce::Rectangle<int> bounds, int tabWidth)
    {
        bounds.removeFromBottom(kGutter);
        bounds.removeFromTop(kGutter);
        auto titleRow = bounds.removeFromTop(kPanelTitleHeight);
        bounds.removeFromTop(kPanelContentPad);
        bounds.removeFromBottom(kPanelContentPad);
        const int w = juce::jlimit(1, titleRow.getWidth(), tabWidth);
        auto tab = titleRow.removeFromLeft(w); // flush left — no side offset
        bounds.removeFromLeft(kPanelContentPad);
        bounds.removeFromRight(kPanelContentPad);
        return { tab, bounds };
    }

    /**
     * Panel silhouette: folder-tab title (45° shoulder, same language as table section
     * headers), then body corners. Top/bottom gutters stay window bg (parent fill).
     */
    inline void paintPanelFrame(juce::Graphics& g,
                                juce::Rectangle<int> panelBounds,
                                juce::Rectangle<int> tab)
    {
        const float r = kControlCorner;
        const float left = (float) panelBounds.getX();
        const float right = (float) panelBounds.getRight();
        const float top = (float) (panelBounds.getY() + kGutter);
        const float bottom = (float) (panelBounds.getBottom() - kGutter);
        // Same rhythm as table section tabs: rail top (= body shoulder) lines up with
        // the title baseline, not the full title-band bottom.
        const float titleBottom = top + (float) kPanelTitleHeight
                                  - (float) kTableSectionRailHeight;
        const float drop = titleBottom - top; // 45° run equals drop
        const float tabRight = juce::jmax(left + r * 2.0f,
                                          (float) tab.getRight() - kTabBevelStartEarly);
        const float bevelEndX = tabRight + drop;

        juce::Path silhouette;
        silhouette.startNewSubPath(left + r, top);
        silhouette.lineTo(tabRight - r, top);
        silhouette.quadraticTo(tabRight, top, tabRight + r, top + r);             // into 45°
        if (bevelEndX + r < right)
        {
            silhouette.lineTo(bevelEndX - r, titleBottom - r);                    // 45° mid-run
            silhouette.quadraticTo(bevelEndX, titleBottom, bevelEndX + r, titleBottom);
        }
        else
        {
            silhouette.lineTo(tabRight, titleBottom);
        }
        silhouette.lineTo(right - r, titleBottom);
        silhouette.quadraticTo(right, titleBottom, right, titleBottom + r);       // body top-right
        silhouette.lineTo(right, bottom - r);
        silhouette.quadraticTo(right, bottom, right - r, bottom);
        silhouette.lineTo(left + r, bottom);
        silhouette.quadraticTo(left, bottom, left, bottom - r);
        silhouette.lineTo(left, top + r);
        silhouette.quadraticTo(left, top, left + r, top);                         // tab top-left
        silhouette.closeSubPath();

        g.setColour(surfacePanel());
        g.fillPath(silhouette);
    }

    inline void paintRoundedWell(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour fill)
    {
        g.setColour(fill);
        g.fillRoundedRectangle(area.toFloat(), kControlCorner);
    }

    /** Fill a table row; only outer corners of the whole table are rounded. */
    inline void fillTableRow(juce::Graphics& g,
                             juce::Rectangle<float> area,
                             juce::Colour fill,
                             bool roundTop,
                             bool roundBottom)
    {
        juce::Path path;
        path.addRoundedRectangle(area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                                 kControlCorner, kControlCorner,
                                 roundTop, roundTop, roundBottom, roundBottom);
        g.setColour(fill);
        g.fillPath(path);
    }

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

    inline TableSectionGeom makeTableSectionGeom(juce::Rectangle<float> sectionRow,
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

    /** Cubic kappa for a circular quarter-arc (matches button / fillTableRow corners). */
    inline constexpr float kCircleBez = 0.5522847498f;

    inline void cubicCornerTopLeft(juce::Path& path, float L, float T, float r)
    {
        // Arriving up the left edge → leaving along the top edge.
        path.cubicTo(L, T + r * (1.0f - kCircleBez),
                     L + r * (1.0f - kCircleBez), T,
                     L + r, T);
    }

    inline void cubicCornerTopRight(juce::Path& path, float R, float top, float r)
    {
        // Arriving along the top → leaving down the right edge.
        path.cubicTo(R - r * (1.0f - kCircleBez), top,
                     R, top + r * (1.0f - kCircleBez),
                     R, top + r);
    }

    inline void cubicCornerBottomRight(juce::Path& path, float R, float B, float r)
    {
        path.cubicTo(R, B - r * (1.0f - kCircleBez),
                     R - r * (1.0f - kCircleBez), B,
                     R - r, B);
    }

    inline void cubicCornerBottomLeft(juce::Path& path, float L, float B, float r)
    {
        path.cubicTo(L + r * (1.0f - kCircleBez), B,
                     L, B - r * (1.0f - kCircleBez),
                     L, B - r);
    }

    /**
     * Full object silhouette: folder-tab + body.
     * `outset` > 0 builds a parallel outer rim (corner radius grows by outset).
     */
    inline juce::Path makeObjectTableSilhouettePath(juce::Rectangle<float> sectionRow,
                                                    float tableBottom,
                                                    const juce::String& title,
                                                    float outset = 0.0f)
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

    /** Header-only fill (tab + rail), same outer rim as makeObjectTableSilhouettePath. */
    inline void paintTableSectionHeader(juce::Graphics& g,
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

    /**
     * Focus outline: 1 px stroked path centred on a half-pixel outset of the silhouette.
     * Covers exactly one logical pixel outside the fill (Photocraft stroke-outside), with
     * uniform H/V weight — avoids the double-edge fringe of an outer−inner annulus.
     */
    inline void strokeObjectTableFocusOutside(juce::Graphics& g,
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

    /** Classic transparency checkerboard (light / mid grey cells). */
    inline void paintCheckerboard(juce::Graphics& g, juce::Rectangle<int> area, int cell = 4)
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

    /** Checkerboard then colour on top — shows alpha / fully transparent as damier. */
    inline void paintColourSwatch(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour colour)
    {
        paintCheckerboard(g, area);
        g.setColour(colour);
        g.fillRect(area);
    }

    /** Count badge: same light grey as button / panel-title text, dark glyph. */
    inline void drawCountBadge(juce::Graphics& g,
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

    inline int countBadgeWidth(int count)
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
