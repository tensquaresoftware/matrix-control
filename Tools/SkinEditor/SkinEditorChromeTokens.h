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
     * Lighter than data rows; slight lift above the previous 0x3a step.
     */
    inline juce::Colour surfaceTableHeader() { return juce::Colour(0xff424242); }
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
    inline juce::Colour textTableHeader() { return juce::Colour(0xffbcbcbc); }
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
    /** Stack gap between chrome wells / panels (SESSION↔OPERATION, OPERATION↔SKINS, SKINS↔COLOR). */
    inline constexpr int kChromeStackGap = kGutter * 2; // 12
    /** Left/right margin of the app window (logo / OPERATION / body / status). */
    inline constexpr int kWindowEdgeGutter = kChromeStackGap; // 12
    /** Gap on both sides of Palette (Color|Palette and Palette|Mappings). */
    inline constexpr int kPaletteSideGap = kChromeStackGap; // 12
    /** Title-bar chrome inset (not panel body content). */
    inline constexpr int kPanelPad = 8;
    inline constexpr int kButtonGap = 8;
    /** Fixed body-column widths — window width fits these; Mappings does not grow. */
    inline constexpr int kColorPanelWidth = 260;
    /** Wider than the original 150 so OPERATION Hold-colour coaching fits more often. */
    inline constexpr int kPalettePanelWidth = 210;
    /**
     * Former Mappings half-share when Color|Palette|Mappings|Unbound filled the
     * legacy 1210-wide window (Unbound removed; Mappings keeps this width).
     * Frozen at the palette=150 share so widening Palette grows the window, not
     * shrinks Mappings.
     */
    inline constexpr int kLegacyWindowMaxWidth = 1210;
    inline constexpr int kMappingsPanelWidth = 385;
    /**
     * Exact body width: edge gutters + Color + Palette side gaps + fixed Mappings.
     * Live window width = max(body, measured title + SESSION) — see windowContentWidth().
     */
    inline constexpr int kWindowBodyWidth = kWindowEdgeGutter + kColorPanelWidth
                                            + kPaletteSideGap + kPalettePanelWidth
                                            + kPaletteSideGap + kMappingsPanelWidth
                                            + kWindowEdgeGutter;
    /** Constexpr body floor; runtime max/min prefer windowContentWidth(). */
    inline constexpr int kWindowMaxWidth = kWindowBodyWidth;
    /** Orbitron logo title in the Skin Editor title bar. */
    inline constexpr const char* kAppTitleText = "MATRIX-CONTROL SKIN EDITOR";
    /**
     * In-panel content inset on all four sides of the body (below the title tab,
     * and left / right / bottom inside the silhouette). Do not use kGutter here.
     */
    inline constexpr int kPanelContentPad = 10;
    /**
     * SESSION well: kButtonHeight with kGutter air above/below (was 38px SKINS strip;
     * 36px aligns the air with standard gutters).
     */
    inline constexpr int kButtonHeight = 24;
    inline constexpr int kSessionWellHeight = kGutter + kButtonHeight + kGutter; // 36
    /**
     * Logo bar: kChromeStackGap above SESSION + well + kGutter below
     * (bottom gutter + following strip = kChromeStackGap before OPERATION).
     */
    inline constexpr int kTitleBarHeight = kChromeStackGap + kSessionWellHeight + kGutter; // 54
    /** Bottom status bar — same height as SESSION / OPERATION wells. */
    inline constexpr int kStatusBarHeight = kSessionWellHeight; // 36
    /**
     * OPERATION well under the logo — same height as SESSION (title + dark content
     * pocket for the mapping witness; idle = empty dark rect).
     */
    inline constexpr int kMappingBandHeight = kSessionWellHeight;
    /** Badge → label gap for BRIDGE / MAPPING / LAST PUSH chrome. */
    inline constexpr int kChromeBadgeGap = 8;
    /** Chrome text badges (MAPPING / BRIDGE / …) — thinner than TextButton. */
    inline constexpr int kMappingBadgeHeight = 16;
    inline constexpr float kMappingBadgeFont = 11.0f;
    inline constexpr int kMappingBadgePadH = 8;
    /** Inset from OPERATION dark pocket to MAPPING badge — equals vertical centre air. */
    inline constexpr int kOperationWitnessPad = (kButtonHeight - kMappingBadgeHeight) / 2; // 4
    /** Painted thick next-step arrow between identity and coaching text. */
    inline constexpr int kOperationArrowWidth = 14;
    /** Tight gap after role colon (role string already ends with a space). */
    inline constexpr int kOperationTightGap = 0;
    /** Gap on each side of the OPERATION arrow. */
    inline constexpr int kOperationArrowGap = 3;
    /** Palette / Mappings / MAPPING witness colour square. */
    inline constexpr int kColourSwatchSize = 12;
    /** Checker cells across one swatch side (12 / 4 = 3 px cells, exact 4x4). */
    inline constexpr int kColourSwatchCheckerDivisions = 4;
    /** Integer optical nudges (barY + (barH - rowH) / 2 + nudge) — keep even/odd consistent. */
    /** Logo dropped 3px from geometric centre in the 54px logo bar. */
    inline constexpr int kLogoTitleNudgeY = 3;
    /** Geometric centre in the 36px status bar. */
    inline constexpr int kStatusChromeNudgeY = 0;
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
    inline constexpr float kAppTitleFont = 20.0f;
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
