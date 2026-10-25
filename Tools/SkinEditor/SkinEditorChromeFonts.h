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
        label.setColour(juce::Label::textColourId, textPrimary());
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

    /** SESSION well width: title tab + Undo/Redo | Compare/Push | Export/Import. */
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
               + measure("Export") + kButtonGap + measure("Import");
    }

    /** Measured Orbitron title advance (+ layout pad matching layoutTitleBar). */
    inline int appTitlePreferredWidth() noexcept
    {
        return measureTextWidth(fontAppTitle(), kAppTitleText) + 8;
    }

    /** Title-bar pads + measured app title + SESSION well. */
    inline int windowTitleSessionMinWidth() noexcept
    {
        return kPanelPad + appTitlePreferredWidth() + kButtonGap
               + sessionWellPreferredWidth() + kPanelPad;
    }

    /** Constrainer / normalize floor: body columns vs title + SESSION (whichever is wider). */
    inline int windowMinWidth() noexcept
    {
        return juce::jmax(kWindowBodyMinWidth, windowTitleSessionMinWidth());
    }
}
