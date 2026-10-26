#pragma once

#include "Tools/SkinEditor/SkinEditorChromeTokens.h"

#include <PluginFontsData.h>

#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinEditorChrome
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
     * Avoids truncated chrome (Push, LAST PUSH, …) on cold launch.
     */
    inline void warmFonts()
    {
        constexpr auto kProbe = "LAST PUSH";
        juce::ignoreUnused(measureTextWidth(fontRegular(), kProbe),
                           measureTextWidth(fontBold(kMappingBadgeFont), kProbe),
                           measureTextWidth(fontTitle(), kProbe),
                           measureTextWidth(fontAppTitle(), kProbe),
                           measureTextWidth(fontMono(), kProbe));
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

    /**
     * SESSION well width: title tab + Undo/Redo | Compare/Push | Export/Import
     * + trailing kGutter after Import (inner right air).
     */
    inline int sessionWellPreferredWidth() noexcept
    {
        const int titleW = titleTabWidth("SESSION");
        const auto measure = [](const char* text)
        {
            return measureButtonWidth(juce::TextButton(text));
        };
        constexpr int kSep = kButtonGap * 4 + 1;
        return titleW + kButtonGap
               + measure("Undo") + kButtonGap + measure("Redo")
               + kSep
               + measure("Compare") + kButtonGap + measure("Push")
               + kSep
               + measure("Export") + kButtonGap + measure("Import")
               + kGutter;
    }

    /** Measured Orbitron title advance (+ layout pad matching layoutTitleBar). */
    inline int appTitlePreferredWidth() noexcept
    {
        return measureTextWidth(fontAppTitle(), kAppTitleText) + 8;
    }

    /** Edge gutters + app title + gap + SESSION controls. */
    inline int windowTitleSessionMinWidth() noexcept
    {
        return kWindowEdgeGutter + appTitlePreferredWidth() + kButtonGap
               + sessionWellPreferredWidth() + kWindowEdgeGutter;
    }

    /**
     * Fitted client width: fixed body columns vs measured title + SESSION.
     * Mappings keeps kMappingsPanelWidth; the window shrinks instead of stretching Mappings.
     */
    inline int windowContentWidth() noexcept
    {
        return juce::jmax(kWindowBodyWidth, windowTitleSessionMinWidth());
    }

    inline int windowMinWidth() noexcept { return windowContentWidth(); }

    inline int windowMaxWidth() noexcept { return windowContentWidth(); }
}
