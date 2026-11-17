#include "Skin.h"

#include "Dev/DevSkinColourElementList.h"
#include "Dev/DevSkinMinimalSeed.h"
#include "SkinColoursCreamChroma.h"
#include "SkinColoursTones.h"

#include <PluginFontsData.h>

namespace TSS
{
    namespace
    {
        constexpr float kSkinBaseFontHeight_ = 14.0f;
        constexpr float kBrandFontHeight_ = 20.0f;

        constexpr Skin::ColourVariant kTonesVariants_[] = {
            Skin::ColourVariant::TonesCherry,
            Skin::ColourVariant::TonesCoral,
            Skin::ColourVariant::TonesSaffron,
            Skin::ColourVariant::TonesOchre,
            Skin::ColourVariant::TonesGold,
            Skin::ColourVariant::TonesOlive,
            Skin::ColourVariant::TonesLime,
            Skin::ColourVariant::TonesEmerald,
            Skin::ColourVariant::TonesKigee,
            Skin::ColourVariant::TonesPetrol,
            Skin::ColourVariant::TonesAzur,
            Skin::ColourVariant::TonesMarine,
            Skin::ColourVariant::TonesPurple,
            Skin::ColourVariant::TonesIndigo,
            Skin::ColourVariant::TonesFuchsia
        };

        struct BaseSkinPair
        {
            int itemId;
            Skin::ColourVariant variant;
        };

        constexpr BaseSkinPair kBaseSkinPairs_[] = {
            { static_cast<int>(Skin::SkinComboBoxItemId::kBlack), Skin::ColourVariant::Black },
            { static_cast<int>(Skin::SkinComboBoxItemId::kCream), Skin::ColourVariant::Cream },
            { static_cast<int>(Skin::SkinComboBoxItemId::kBlackChroma), Skin::ColourVariant::BlackChroma },
            { static_cast<int>(Skin::SkinComboBoxItemId::kBlackMinimal), Skin::ColourVariant::BlackMinimal },
            { static_cast<int>(Skin::SkinComboBoxItemId::kCreamMinimal), Skin::ColourVariant::CreamMinimal },
            { static_cast<int>(Skin::SkinComboBoxItemId::kCreamChroma), Skin::ColourVariant::CreamChroma }
        };
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

    Skin::SkinFamily Skin::familyOf(ColourVariant variant) noexcept
    {
        if (isTonesVariant(variant))
            return SkinFamily::Tones;
        if (variant == ColourVariant::Cream
            || variant == ColourVariant::CreamMinimal
            || variant == ColourVariant::CreamChroma)
            return SkinFamily::Cream;
        return SkinFamily::Black;
    }

    bool Skin::isTonesVariant(ColourVariant variant) noexcept
    {
        return variant >= ColourVariant::TonesCherry && variant <= ColourVariant::TonesFuchsia;
    }

    Skin::ColourVariant Skin::tonesVariantFromToneId(ColourChartTones::ToneId tone) noexcept
    {
        const int index = static_cast<int>(tone);
        if (index < 0 || index >= ColourChartTones::kToneCount)
            return ColourVariant::TonesCherry;
        return kTonesVariants_[static_cast<size_t>(index)];
    }

    ColourChartTones::ToneId Skin::toneIdFromColourVariant(ColourVariant variant) noexcept
    {
        if (! isTonesVariant(variant))
            return ColourChartTones::ToneId::kCherry;
        return static_cast<ColourChartTones::ToneId>(
            static_cast<int>(variant) - static_cast<int>(ColourVariant::TonesCherry));
    }

    Skin::ColourVariant Skin::colourVariantFromItemId(int skinItemId) noexcept
    {
        const int tonesFirst = static_cast<int>(SkinComboBoxItemId::kTonesCherry);
        const int tonesLast = static_cast<int>(SkinComboBoxItemId::kTonesFuchsia);
        if (skinItemId >= tonesFirst && skinItemId <= tonesLast)
            return tonesVariantFromToneId(
                static_cast<ColourChartTones::ToneId>(skinItemId - tonesFirst));

        for (const auto& pair : kBaseSkinPairs_)
            if (pair.itemId == skinItemId)
                return pair.variant;

        return ColourVariant::Black;
    }

    int Skin::itemIdFromColourVariant(ColourVariant variant) noexcept
    {
        if (isTonesVariant(variant))
            return static_cast<int>(SkinComboBoxItemId::kTonesCherry)
                + static_cast<int>(toneIdFromColourVariant(variant));

        for (const auto& pair : kBaseSkinPairs_)
            if (pair.variant == variant)
                return pair.itemId;

        return static_cast<int>(SkinComboBoxItemId::kBlack);
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
        if (isTonesVariant(currentVariant_))
        {
            SkinColours::Tones::applyToneColours(
                toneIdFromColourVariant(currentVariant_),
                [this](SkinColourKey key, juce::Colour colour)
                {
                    colours_[key] = colour;
                });
            return;
        }

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
