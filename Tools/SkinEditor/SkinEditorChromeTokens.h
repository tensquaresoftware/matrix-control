#pragma once

#include <juce_graphics/juce_graphics.h>

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

    /** Window / inter-panel gutters only — never reuse as in-panel content inset. */
    inline constexpr int kGutter = 6;
    /** Title-bar chrome inset (not panel body content). */
    inline constexpr int kPanelPad = 8;
    inline constexpr int kButtonGap = 8;
    /** Fixed left-column and Palette widths — Mappings takes the leftover. */
    inline constexpr int kColorPanelWidth = 260;
    inline constexpr int kPalettePanelWidth = 150;
    /** Practical Mappings floor so object tables stay readable when the window shrinks. */
    inline constexpr int kMappingsMinWidth = 320;
    /**
     * Body-column floor: gutters + Color + Palette + Mappings min.
     * Live window min = max(body, measured title + SESSION) — see windowMinWidth().
     */
    inline constexpr int kWindowBodyMinWidth = kGutter + kColorPanelWidth + kGutter
                                              + kPalettePanelWidth + kGutter
                                              + kMappingsMinWidth + kGutter;
    /** Document window maximum width (height unconstrained here). Default launch uses max. */
    inline constexpr int kWindowMaxWidth = 1210;
    /** Orbitron logo title in the Skin Editor title bar. */
    inline constexpr const char* kAppTitleText = "MATRIX-CONTROL SKIN EDITOR";
    /**
     * In-panel content inset on all four sides of the body (below the title tab,
     * and left / right / bottom inside the silhouette). Do not use kGutter here.
     */
    inline constexpr int kPanelContentPad = 10;
    inline constexpr int kTitleBarHeight = 32;
    /** Bottom status bar - same height / fill spirit as the logo title bar. */
    inline constexpr int kStatusBarHeight = kTitleBarHeight;
    /** Nude mapping-help band under the logo (status-bar spirit, no SKINS well). */
    inline constexpr int kMappingBandHeight = kStatusBarHeight;
    /** Badge → label gap for CONNECTION / MAPPING / LAST PUSH chrome. */
    inline constexpr int kChromeBadgeGap = 8;
    inline constexpr int kButtonHeight = 24;
    /** Chrome text badges (MAPPING / CONNECTION / …) — thinner than TextButton. */
    inline constexpr int kMappingBadgeHeight = 16;
    inline constexpr float kMappingBadgeFont = 11.0f;
    inline constexpr int kMappingBadgePadH = 8;
    /** Palette / Mappings / MAPPING witness colour square. */
    inline constexpr int kColourSwatchSize = 12;
    /** Checker cells across one swatch side (12 / 4 = 3 px cells, exact 4x4). */
    inline constexpr int kColourSwatchCheckerDivisions = 4;
    /** Integer optical nudges (barY + (barH - rowH) / 2 + nudge) — keep even/odd consistent. */
    inline constexpr int kLogoTitleNudgeY = 2;
    inline constexpr int kLogoChromeNudgeY = 3;
    inline constexpr int kStatusChromeNudgeY = -6;
    inline constexpr int kPanelTitleHeight = 26;
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
    /**
     * Badge corner — shorter than buttons (16 vs 24), so use a smaller radius
     * to keep the same optical roundness as kControlCorner on controls.
     */
    inline constexpr float kChromeBadgeCorner = 2.0f;
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
}
