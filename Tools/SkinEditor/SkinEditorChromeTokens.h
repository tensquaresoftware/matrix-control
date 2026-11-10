#pragma once

#include <cmath>

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
    /** Selected mapping row — lift above surfaceTable (#1c1c1c). */
    inline juce::Colour surfaceTableRowHighlight() { return juce::Colour(0xff272727); }
    /** HeldColour Property hover fallback (pending pastille / no colour yet). */
    inline juce::Colour surfaceTablePropertyBindHover() { return juce::Colour(0xff353535); }
    /**
     * Property label on a light / mid bind-hover fill (kWhite, kCream, kGrey350, …).
     * Same ink as surfaceSection / app chrome.
     */
    inline juce::Colour textOnLightPropertyBindHover() { return juce::Colour(0xff151515); }
    /** Horizontal inset so highlights float inside cut column slabs. */
    inline constexpr int kMappingsHighlightInsetX = 4;
    /** Vertical inset (keeps highlight thinner than the cell band). */
    inline constexpr int kMappingsHighlightInsetY = 2;
    /** Selection highlight corners (Mappings + Palette) — quieter than kControlCorner. */
    inline constexpr float kMappingsHighlightCorner = 1.5f;
    /** Raised tab chip (Photocraft active tab). */
    inline juce::Colour surfaceTab() { return juce::Colour(0xff323232); }
    inline juce::Colour textPrimary() { return juce::Colour(0xffdddddd); }
    inline juce::Colour textMuted() { return juce::Colour(0xff999999); }
    /** Plugin logo / ChromeText — ColourChart::kGrey200. */
    inline juce::Colour textBrand() { return juce::Colour(0xffb9b9b9); }
    /** Section titles + table column headers (lighter than textMuted). */
    inline juce::Colour textTableHeader() { return juce::Colour(0xffbcbcbc); }
    /** Table data-row body text (darker than textPrimary). */
    inline juce::Colour textTableBody() { return juce::Colour(0xffc4c4c4); }

    /** WCAG relative luminance (sRGB), 0 = black … 1 = white. */
    inline float relativeLuminance(juce::Colour c) noexcept
    {
        auto linearise = [](float u) noexcept
        {
            return u <= 0.04045f ? (u / 12.92f)
                                 : std::pow((u + 0.055f) / 1.055f, 2.4f);
        };
        return 0.2126f * linearise(c.getFloatRed())
             + 0.7152f * linearise(c.getFloatGreen())
             + 0.0722f * linearise(c.getFloatBlue());
    }

    /** Contrast ratio of two opaque colours (≥ 1). */
    inline float contrastRatio(juce::Colour a, juce::Colour b) noexcept
    {
        const float l1 = relativeLuminance(a);
        const float l2 = relativeLuminance(b);
        const float lighter = juce::jmax(l1, l2);
        const float darker = juce::jmin(l1, l2);
        return (lighter + 0.05f) / (darker + 0.05f);
    }

    /**
     * True when #151515 reads better than textTableBody on this pastille fill
     * (covers kWhite / kCream / kGrey350 and other mid-light fills).
     */
    inline bool propertyBindHoverNeedsDarkText(juce::Colour fill) noexcept
    {
        return contrastRatio(fill, textOnLightPropertyBindHover())
             > contrastRatio(fill, textTableBody());
    }

    inline juce::Colour accent() { return juce::Colour(0xff00ddad); }
    /** Attention / coaching chrome (same amber as Mappings "Unbound"). */
    inline juce::Colour attention() { return juce::Colour(0xffe0a020); }
    /** Danger fill (Compare blink; SAME COLOUR / pending MAPPING / disconnected BRIDGE badges). */
    inline juce::Colour danger() { return juce::Colour(0xffff3b3b); }
    inline juce::Colour separatorLine() { return juce::Colour(0xff3a3a3a); }
    /** Gap between cut column slabs (panel shows through = cutter). */
    inline constexpr int kMappingsColumnDividerThickness = 4;
    inline juce::Colour controlFill() { return surfacePanel(); }
    inline juce::Colour controlOutline() { return juce::Colour(0xff555555); }
    inline juce::Colour controlFillHover() { return juce::Colour(0xff4a4a4a); }
    inline juce::Colour controlFillDown() { return juce::Colour(0xff3a3a3a); }

    /** Window / inter-panel gutters only — never reuse as in-panel content inset. */
    inline constexpr int kGutter = 6;
    /** Stack gap between chrome rows / panels (title↔body, SKINS↔COLOR, …). */
    inline constexpr int kChromeStackGap = kGutter * 2; // 12
    /** Left/right margin of the app window (logo / OPERATION / body / status). */
    inline constexpr int kWindowEdgeGutter = kChromeStackGap; // 12
    /** Gap on both sides of Palette (Color|Palette and Palette|Mappings). */
    inline constexpr int kPaletteSideGap = kChromeStackGap; // 12
    /** Title-bar chrome inset (not panel body content). */
    inline constexpr int kPanelPad = 8;
    inline constexpr int kButtonGap = 8;
    /**
     * ComboBox text vs arrow: LookAndFeel arrow sits near width-16; label ends before this reserve.
     * Keep in sync with SkinEditorLookAndFeel::positionComboBoxText.
     */
    inline constexpr int kComboArrowReserve = 18;
    /** Mappings Object combo — fits TrackGeneratorDisplay with air before the arrow (+8 vs prior). */
    inline constexpr int kObjectComboWidth = 149;
    /** Mappings Object combo first item — show every object table. */
    inline constexpr const char* kAllObjectsComboText = "- All objects -";
    /**
     * Hold colour toggle left edge: flush with Colour set column when possible,
     * otherwise Object combo (or its clear badge) + kButtonGap.
     */
    inline int mappingsHoldLeft(int holdAnchorLeft, int colourSetColumnLeft) noexcept
    {
        return juce::jmax(holdAnchorLeft + kButtonGap, colourSetColumnLeft);
    }
    /**
     * In-panel content inset on all four sides of the body (below the title tab,
     * and left / right / bottom inside the silhouette). Do not use kGutter here.
     */
    inline constexpr int kPanelContentPad = 10;
    /**
     * Vertical scroll track width when centred in the panel's right content-pad band:
     * former default track (8) + right content pad.
     */
    inline constexpr int kScrollTrackWidth = 8 + kPanelContentPad;
    /** Fixed Color column — does not grow with the window. */
    inline constexpr int kColorPanelWidth = 260;
    /**
     * Palette column minimum width. Extra window width is shared with Mappings
     * Mapped colour so both can show the same colour names.
     * Sized so Color + Palette + Mappings fit kWindowContentMinWidth (1000).
     * Floored at runtime with New/Dup/Del + chrome text via palettePanelMinWidth().
     */
    inline constexpr int kPalettePanelMinWidth = 125;
    /**
     * Palette list paint floor at min panel width (before scroll-track band).
     * Live paint width = palettePanelW - kPanelContentPad - kScrollTrackWidth.
     */
    inline constexpr int kPaletteListPaintMinWidth = kPalettePanelMinWidth
                                                     - kPanelContentPad
                                                     - kScrollTrackWidth; // 97
    /**
     * Mappings column widths (multiples of 5 where fixed). Each column insets content by
     * kMappingsColumnPad on left and right so text/chips clear the dividers.
     */
    inline constexpr int kMappingsPropertyColumnWidth = 160;
    inline constexpr int kMappingsStateColumnWidth = 85;
    /** Colour set: [csN] badge + free label (up to 16 chars) + header pictos. */
    inline constexpr int kMappingsColourSetColumnWidth = 140;
    /**
     * Link: chain + fixed count chip only (no title).
     * Visible slab = 2×pad + chain + chip gap + count; column adds cutter thickness.
     */
    inline constexpr int kMappingsLinkColumnWidth = 55; // slab 51 + cutter 4 (6+12+4+22+6 +1)
    /** Fixed Mappings columns (everything except Mapped colour). */
    inline constexpr int kMappingsFixedColumnsWidth = kMappingsPropertyColumnWidth
                                                      + kMappingsStateColumnWidth
                                                      + kMappingsColourSetColumnWidth
                                                      + kMappingsLinkColumnWidth; // 440
    /**
     * Mapped colour minimum = Palette list paint at min Palette width (name parity).
     * Live Mapped width absorbs the remainder of the Mappings table paint width.
     */
    inline constexpr int kMappingsMappedColumnMinWidth = kPaletteListPaintMinWidth; // 97
    /** Horizontal inset inside every Mappings column (left and right). */
    inline constexpr int kMappingsColumnPad = 6;
    /** Min table paint width = fixed columns + min Mapped. */
    inline constexpr int kMappingsTablePaintMinWidth = kMappingsFixedColumnsWidth
                                                       + kMappingsMappedColumnMinWidth; // 537
    inline constexpr int kMappingsFocusPad = 1;
    /**
     * Mappings panel width for a given Palette panel width.
     * Identity: panel = fixedCols + palettePanelW + 2×focus
     * (Mapped paint tracks Palette list paint; pad/scroll terms cancel).
     */
    inline constexpr int mappingsPanelWidthForPalettePanel(int palettePanelW) noexcept
    {
        return kMappingsFixedColumnsWidth + palettePanelW + 2 * kMappingsFocusPad;
    }
    /** Min Mappings panel width (Palette at minimum). */
    inline constexpr int kMappingsPanelMinWidth =
        mappingsPanelWidthForPalettePanel(kPalettePanelMinWidth); // 567
    /** Body width for a given Palette panel width (edges + Color + gaps + Mappings). */
    inline constexpr int windowBodyWidthForPalettePanel(int palettePanelW) noexcept
    {
        return kWindowEdgeGutter + kColorPanelWidth
               + kPaletteSideGap + palettePanelW
               + kPaletteSideGap + mappingsPanelWidthForPalettePanel(palettePanelW)
               + kWindowEdgeGutter;
    }
    /** Constexpr body floor at minimum Palette (matches kWindowContentMinWidth). */
    inline constexpr int kWindowBodyMinWidth =
        windowBodyWidthForPalettePanel(kPalettePanelMinWidth); // 1000
    /**
     * Palette width from body remainder after Color (includes leading Palette gap).
     * remainder = palette + gap + mappings(palette) → palette = (remainder - offset) / 2.
     */
    inline int palettePanelWidthFromBodyRemainder(int remainderAfterColor) noexcept
    {
        const int offset = kPaletteSideGap + kMappingsFixedColumnsWidth
                           + 2 * kMappingsFocusPad;
        return juce::jmax(kPalettePanelMinWidth, (remainderAfterColor - offset) / 2);
    }
    /** Palette list paint width for a live panel width. */
    inline int paletteListPaintWidthForPanel(int palettePanelW) noexcept
    {
        return juce::jmax(1, palettePanelW - kPanelContentPad - kScrollTrackWidth);
    }
    /**
     * SESSION well: kButtonHeight with kGutter air above/below (was 38px SKINS strip;
     * 36px aligns the air with standard gutters).
     */
    inline constexpr int kButtonHeight = 24;
    inline constexpr int kSessionWellHeight = kGutter + kButtonHeight + kGutter; // 36
    /**
     * Title bar: kChromeStackGap above SESSION/OPERATION wells + well + kGutter below
     * (bottom gutter + following strip = kChromeStackGap before the body).
     */
    inline constexpr int kTitleBarHeight = kChromeStackGap + kSessionWellHeight + kGutter; // 54
    /** Bottom status bar — same height as SESSION / OPERATION wells. */
    inline constexpr int kStatusBarHeight = kSessionWellHeight; // 36
    /**
     * OPERATION well on the title row (right of SESSION) — same height as SESSION
     * (title + dark content pocket for the mapping witness; idle = empty).
     */
    inline constexpr int kMappingBandHeight = kSessionWellHeight;
    /**
     * Client width floor for the Skin Editor window.
     * OPERATION's minimum is derived so title chrome (SESSION + OPERATION) hits this;
     * Palette min is sized so the body columns fit the same floor.
     */
    inline constexpr int kWindowContentMinWidth = 1000;
    /** Badge → label gap for BRIDGE / MAPPING / LAST PUSH chrome. */
    inline constexpr int kChromeBadgeGap = 8;
    /** Chrome text badges (MAPPING / BRIDGE / …) — thinner than TextButton. */
    inline constexpr int kMappingBadgeHeight = 16;
    inline constexpr float kMappingBadgeFont = 11.0f;
    inline constexpr int kMappingBadgePadH = 4;
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
    /** Geometric centre in the 36px status bar. */
    inline constexpr int kStatusChromeNudgeY = 0;
    inline constexpr int kPanelTitleHeight = 26;
    inline constexpr int kTitleTabPadH = 10;
    /** Sticky column-legend / table content row height. */
    inline constexpr int kColumnHeaderHeight = 22;
    /** Thin rail extending right of a Mappings object-header tab (and cut-column continuity). */
    inline constexpr int kTableSectionRailHeight = 6;
    /** Horizontal pad inside the object-header tab chip. */
    inline constexpr int kTableSectionTabPadH = 8;
    /** Begin the 45° folder-tab shoulder this many px earlier than the measured title width. */
    inline constexpr float kTabBevelStartEarly = 5.0f;
    /** Gap under the sticky Mappings legend, included in the sticky band. */
    inline constexpr int kColumnHeaderGap = 11;
    /** Empty space between object tables (ListBox slot = half a content row). */
    inline constexpr int kTableGapHeight = 11;
    inline constexpr float kPanelTitleFont = 15.0f;
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
    /** Count badge square / capsule height (Link, Unbound, Palette counts). */
    inline constexpr int kCountBadgeDiameter = 12;
    inline constexpr float kCountBadgeFont = 11.0f;
    inline constexpr int kCountBadgeCapsulePad = 10;
    /** Mappings column-filter clear chip: square NxN (matches count / Link chain). */
    inline constexpr int kClearBadgeSize = 12;
    /** Mappings column-sort chip: same footprint as the clear badge. */
    inline constexpr int kSortBadgeSize = kClearBadgeSize;
    /** Link chain chip — same footprint as sort / clear badges. */
    inline constexpr int kLinkChainBadgeSize = kSortBadgeSize;
    /** Link header count chip — fixed width for two digits ("99"), centred glyph. */
    inline constexpr int kLinkCountBadgeWidth = kCountBadgeDiameter + kCountBadgeCapsulePad; // 22
    /**
     * Sticky-legend gap between adjacent header chips
     * (sort ↔ filter clear, Link chain ↔ count badge).
     */
    inline constexpr int kLegendChipGap = 4;
    /** Colour set row index chip (`cs1`…`cs4`) — same type size as count badges. */
    inline constexpr float kColourSetIndexBadgeFont = kCountBadgeFont; // 11
    /** Chip height — matches colour swatch (kColourSwatchSize). */
    inline constexpr int kColourSetIndexBadgeHeight = 12;
    /** Min width floor; paint grows with measured `csN` text (+ tight side air). */
    inline constexpr int kColourSetIndexBadgeMinWidth = 18;
    /** Horizontal pad inside the csN chip (each side, total = 2×). */
    inline constexpr int kColourSetIndexBadgePadH = 1;
}
