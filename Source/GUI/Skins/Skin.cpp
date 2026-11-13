#include "Skin.h"

#include "Dev/DevSkinColourElementList.h"
#include "Dev/DevSkinMinimalSeed.h"
#include "SkinColoursCreamChroma.h"

#include <PluginFontsData.h>

namespace TSS
{
    namespace
    {
        constexpr float kSkinBaseFontHeight_ = 14.0f;
        constexpr float kBrandFontHeight_ = 20.0f;
    }

    Skin::Skin(ColourVariant variant)
        : currentVariant_(variant)
    {
        initializeDefaultColours();
    }

    std::unique_ptr<Skin> Skin::create(ColourVariant variant)
    {
        return std::make_unique<Skin>(variant);
    }

    juce::Colour Skin::getColour(SkinObjectType object,
                                 SkinColourProperty property,
                                 SkinColourState state,
                                 ColourSet colourSet) const
    {
        return getColour(skinColourKey(object, property, state, colourSet));
    }

    juce::Colour Skin::getColour(SkinColourKey key) const
    {
        const auto it = colours_.find(key);
        if (it != colours_.end())
            return it->second;

        if (key.colourSet != ColourSet::k1)
        {
            key.colourSet = ColourSet::k1;
            const auto fallback = colours_.find(key);
            if (fallback != colours_.end())
                return fallback->second;
        }

        jassertfalse;
        DBG("Missing skin colour key - object/property/state/colourSet not in map");
        return juce::Colour(ColourChart::kWhite);
    }

    void Skin::setColour(SkinColourKey key, juce::Colour colour)
    {
        colours_[key] = colour;
    }

    void Skin::clearHigherColourSets()
    {
        for (auto it = colours_.begin(); it != colours_.end();)
        {
            if (it->first.colourSet != ColourSet::k1)
                it = colours_.erase(it);
            else
                ++it;
        }
    }

    juce::Font Skin::getBaseFont() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::PTSansNarrowRegular_ttf,
            static_cast<size_t>(PluginFontsData::PTSansNarrowRegular_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    juce::Font Skin::getBaseFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::PTSansNarrowBold_ttf,
            static_cast<size_t>(PluginFontsData::PTSansNarrowBold_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    juce::Font Skin::getBrandFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::OrbitronBold_ttf,
            static_cast<size_t>(PluginFontsData::OrbitronBold_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kBrandFontHeight_));
    }

    juce::Font Skin::getModalBodyFont() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::MontserratRegular_ttf,
            static_cast<size_t>(PluginFontsData::MontserratRegular_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    juce::Font Skin::getModalBodyFontBold() const
    {
        const auto typeface = juce::Typeface::createSystemTypefaceFor(
            PluginFontsData::MontserratBold_ttf,
            static_cast<size_t>(PluginFontsData::MontserratBold_ttfSize)
        );
        return juce::Font(juce::FontOptions(typeface).withHeight(kSkinBaseFontHeight_));
    }

    void Skin::initializeDefaultColours()
    {
        if (currentVariant_ == ColourVariant::BlackMinimal
            || currentVariant_ == ColourVariant::CreamMinimal)
        {
            size_t count = 0;
            const auto* entries = (currentVariant_ == ColourVariant::BlackMinimal)
                                      ? DevSkin::blackMinimalEntries(count)
                                      : DevSkin::creamMinimalEntries(count);
            for (size_t i = 0; i < count; ++i)
            {
                const auto& entry = entries[i];
                colours_[skinColourKey(entry.object, entry.property, entry.state, entry.colourSet)]
                    = juce::Colour(entry.argb);
            }
            return;
        }

        DevSkin::forEachCompiledColourElement([&](const SkinColours::ColourElement& element)
        {
            juce::uint32 argb = element.creamVariant;
            if (currentVariant_ == ColourVariant::Black)
                argb = element.blackVariant;
            else if (currentVariant_ == ColourVariant::BlackChroma)
                argb = element.blackChromaVariant;
            colours_[element.key] = juce::Colour(argb);
        });

        if (currentVariant_ == ColourVariant::CreamChroma)
            for (const auto& accent : SkinColours::CreamChroma::kAccentOverrides)
                colours_[accent.key] = juce::Colour(accent.argb);
    }
}
