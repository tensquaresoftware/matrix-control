#pragma once

#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"

#include <PluginFontsData.h>

#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinDesignerChrome
{
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
        label.setColour(juce::Label::textColourId, textBrand());
        label.setJustificationType(juce::Justification::centred);
        preventTextSquish(label);
    }

    inline int measureTextWidth(const juce::Font& font, const juce::String& text)
    {
        return juce::GlyphArrangement::getStringWidthInt(font, text);
    }

    /** Truncate with "..." so text fits maxWidth pixels (bold/regular advance). */
    inline juce::String elideToWidth(const juce::Font& font, juce::String text, int maxWidth)
    {
        if (maxWidth <= 0)
            return {};
        if (measureTextWidth(font, text) <= maxWidth)
            return text;

        constexpr auto kEllipsis = "...";
        const int ellipsisW = measureTextWidth(font, kEllipsis);
        if (ellipsisW > maxWidth)
            return {};

        while (text.isNotEmpty()
               && measureTextWidth(font, text) + ellipsisW > maxWidth)
            text = text.dropLastCharacters(1);
        return text + kEllipsis;
    }

    /**
     * Path-style truncate: keep head + tail around "..." so leaf name + extension stay visible.
     * Same 1/3 head + 2/3 tail balance as TSS::TextFitHelpers (Footer sticky paths).
     */
    inline juce::String elidePathToWidth(const juce::Font& font,
                                        const juce::String& text,
                                        int maxWidth)
    {
        if (maxWidth <= 0)
            return {};
        if (measureTextWidth(font, text) <= maxWidth)
            return text;

        constexpr auto kEllipsis = "...";
        const int ellipsisW = measureTextWidth(font, kEllipsis);
        if (ellipsisW >= maxWidth)
            return kEllipsis;

        const int length = text.length();
        int bestHead = 0;
        int bestTail = 0;
        int lo = 0;
        int hi = length - 1;

        while (lo <= hi)
        {
            const int keep = (lo + hi) / 2;
            const int head = keep / 3;
            const int tail = keep - head;
            const auto candidate = text.substring(0, head) + kEllipsis
                                   + text.substring(length - tail);

            if (measureTextWidth(font, candidate) <= maxWidth)
            {
                bestHead = head;
                bestTail = tail;
                lo = keep + 1;
            }
            else
            {
                hi = keep - 1;
            }
        }

        if (bestHead + bestTail <= 0)
            return kEllipsis;

        return text.substring(0, bestHead) + kEllipsis + text.substring(length - bestTail);
    }

    /**
     * Force typeface load + glyph metrics before the first resized/layout.
     * Avoids truncated chrome (LAST PUSH, …) on cold launch.
     */
    inline void warmFonts()
    {
        constexpr auto kProbe = "LAST PUSH";
        juce::ignoreUnused(measureTextWidth(fontRegular(), kProbe),
                           measureTextWidth(fontBold(kMappingBadgeFont), kProbe),
                           measureTextWidth(fontTitle(), kProbe),
                           measureTextWidth(fontMono(), kProbe));
    }

    inline int measureButtonWidth(const juce::TextButton& button, int pad = 18)
    {
        return juce::jmax(52, measureTextWidth(fontRegular(), button.getButtonText()) + pad);
    }

    /** SESSION short labels — text + pad, no 52 px control floor. */
    inline int sessionCompactButtonWidth(const juce::String& text) noexcept
    {
        constexpr int kPad = 14;
        constexpr int kMin = 36;
        return juce::jmax(kMin, measureTextWidth(fontRegular(), text) + kPad);
    }

    /** SESSION Undo/Redo share one compact width. */
    inline int sessionUndoRedoButtonWidth() noexcept
    {
        return juce::jmax(sessionCompactButtonWidth("Undo"),
                          sessionCompactButtonWidth("Redo"));
    }

    /**
     * Palette New / Dup|Duplicate / Del|Delete: text + pad, no 52 px floor
     * (short labels stay tight at min width).
     */
    inline int measurePaletteActionButtonWidth(const juce::String& label) noexcept
    {
        constexpr int kPad = 16;
        constexpr int kMin = 36;
        return juce::jmax(kMin, measureTextWidth(fontRegular(), label) + kPad);
    }

    inline int measurePaletteActionButtonWidth(const juce::TextButton& button) noexcept
    {
        return measurePaletteActionButtonWidth(button.getButtonText());
    }

    /** Preferred New + Dup + Del row (two gaps); used to floor Palette min width. */
    inline int paletteActionButtonsPreferredWidth() noexcept
    {
        return measurePaletteActionButtonWidth("New")
               + measurePaletteActionButtonWidth("Dup")
               + measurePaletteActionButtonWidth("Del")
               + 2 * kButtonGap;
    }

    /** True when New + Duplicate + Delete (+ gaps) fit the Palette action row. */
    inline bool paletteActionButtonsPreferLongLabels(int actionRowWidth) noexcept
    {
        const int preferred = measurePaletteActionButtonWidth("New")
                              + measurePaletteActionButtonWidth("Duplicate")
                              + measurePaletteActionButtonWidth("Delete")
                              + 2 * kButtonGap;
        return preferred <= actionRowWidth;
    }

    /**
     * List / Mapped-colour paint floor: action row, section headers, and "Mapped colour"
     * legend (header text uses 6 px inset each side).
     */
    inline int paletteListPaintFloor() noexcept
    {
        constexpr int kHeaderInset = 12;
        const int fromActions = paletteActionButtonsPreferredWidth();
        const int fromHeaders = measureTextWidth(fontBold(), "Palette")
                                + kHeaderInset;
        const int fromMappedLegend = measureTextWidth(fontBold(), "Mapped colour");
        return juce::jmax(kPaletteListPaintMinWidth,
                          juce::jmax(fromActions, juce::jmax(fromHeaders, fromMappedLegend)));
    }

    /**
     * Minimum Palette panel width so paintWidth() hosts action + chrome text floors
     * (content pad + scroll-track band reserved like Palette::paintWidth).
     */
    inline int palettePanelWidthForActionButtons() noexcept
    {
        return paletteListPaintFloor() + kPanelContentPad + kScrollTrackWidth;
    }

    /** Effective Palette minimum (token floor vs action/chrome text). */
    inline int palettePanelMinWidth() noexcept
    {
        return juce::jmax(kPalettePanelMinWidth, palettePanelWidthForActionButtons());
    }

    inline int measureLabelWidth(const juce::String& text, const juce::Font& font = fontRegular())
    {
        return juce::jmax(14, measureTextWidth(font, text) + 8);
    }

    inline int titleTabWidth(const juce::String& title, int extraInsideTab = 0)
    {
        return measureTextWidth(fontTitle(), title) + kTitleTabPadH * 2 + extraInsideTab;
    }

    /** SESSION label ahead of the JSON Sync combo. */
    inline int jsonSyncLabelWidth() noexcept
    {
        return measureTextWidth(fontRegular(), "JSON Sync:") + 2;
    }

    /** SESSION JSON Sync combo width (fits Off / Ask / Auto + arrow). */
    inline int jsonSyncComboWidth() noexcept
    {
        return 55;
    }

    /**
     * SESSION well width: title tab + Load/Save/Save as | JSON Sync
     * | Undo/Redo + trailing kGutter (inner right air).
     */
    inline int sessionWellPreferredWidth() noexcept
    {
        const int titleW = titleTabWidth("SESSION");
        const auto measure = [](const char* text)
        {
            return measureButtonWidth(juce::TextButton(text));
        };
        constexpr int kSep = kButtonGap * 2 + 1;
        const int undoRedoW = sessionUndoRedoButtonWidth();
        // Title tab already includes kTitleTabPadH on both sides of the glyph —
        // no extra kButtonGap after SESSION (matches left title air).
        return titleW
               + measure("Load") + kButtonGap + measure("Save")
               + kButtonGap + measure("Save as")
               + kSep
               + jsonSyncLabelWidth() + kButtonGap + jsonSyncComboWidth()
               + kSep
               + undoRedoW + kButtonGap + undoRedoW
               + kGutter;
    }

    /**
     * OPERATION well floor so title chrome equals kWindowContentMinWidth
     * (edges + SESSION + gap + OPERATION). Stretches when the window grows.
     */
    inline int operationWellMinWidth() noexcept
    {
        const int titleWithoutOperation = kWindowEdgeGutter + sessionWellPreferredWidth()
                                          + kSessionOperationGap + kWindowEdgeGutter;
        return juce::jmax(0, kWindowContentMinWidth - titleWithoutOperation);
    }

    /**
     * Edge gutters + SESSION + OPERATION (min) on the title row.
     * Gap matches layoutTitleBar (kSessionOperationGap = Palette|Mappings).
     */
    inline int windowTitleChromeMinWidth() noexcept
    {
        return kWindowEdgeGutter + sessionWellPreferredWidth() + kSessionOperationGap
               + operationWellMinWidth() + kWindowEdgeGutter;
    }

    /**
     * Client width floor: max(token body at Palette min, title chrome, kWindowContentMinWidth).
     * Uses kPalettePanelMinWidth (not the action-button runtime floor) so the window can
     * reach kWindowContentMinWidth; action labels may compress at that size.
     * Widening grows Palette / Mapped and stretches the OPERATION slot.
     */
    inline int windowContentWidth() noexcept
    {
        return juce::jmax(kWindowContentMinWidth,
                          juce::jmax(windowBodyWidthForPalettePanel(kPalettePanelMinWidth),
                                     windowTitleChromeMinWidth()));
    }

    inline int windowMinWidth() noexcept { return windowContentWidth(); }
}
