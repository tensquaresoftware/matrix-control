#include "GUI/Skins/Dev/DevSkinSsotSeed.h"

#include "GUI/Skins/Dev/DevSkinColourElementList.h"
#include "GUI/Skins/Dev/DevSkinMinimalSeed.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/SkinColours.h"
#include "GUI/Skins/SkinColoursCreamChroma.h"

namespace TSS::DevSkin
{
    namespace
    {
        juce::String pastilleNameForElement(NamedSkin& skin, juce::uint32 argb)
        {
            auto name = pastilleNameForArgb(argb);
            if (name.isEmpty())
                name = juce::String::formatted("custom_%08X", static_cast<unsigned>(argb));
            ensurePastilleOnSkin(skin, name, argb);
            return name;
        }

        juce::uint32 argbForCompiled(const SkinColours::ColourElement& element, SkinKind kind)
        {
            if (kind == SkinKind::kCompiledBlack)
                return element.blackVariant;
            if (kind == SkinKind::kCompiledBlackChroma)
                return element.blackChromaVariant;
            return element.creamVariant;
        }

        void putMapping(NamedSkin& skin,
                        const SkinColours::ColourElement& element,
                        SkinKind kind)
        {
            skin.mappings[element.key] = pastilleNameForElement(skin, argbForCompiled(element, kind));
        }

        void seedFromMinimalEntries(NamedSkin& skin, const MinimalColourEntry* entries, size_t count)
        {
            skin.palette.clear();
            skin.mappings.clear();
            for (size_t i = 0; i < count; ++i)
            {
                const auto& entry = entries[i];
                const auto key = skinColourKey(entry.object, entry.property, entry.state, entry.colourSet);
                skin.mappings[key] = pastilleNameForElement(skin, entry.argb);
            }
        }

        bool isCompiledKind(SkinKind kind) noexcept
        {
            return kind == SkinKind::kCompiledBlack
                || kind == SkinKind::kCompiledBlackChroma
                || kind == SkinKind::kCompiledBlackMinimal
                || kind == SkinKind::kCompiledCream
                || kind == SkinKind::kCompiledCreamMinimal
                || kind == SkinKind::kCompiledCreamChroma;
        }

        void seedCreamChromaAccents(NamedSkin& skin)
        {
            for (const auto& accent : SkinColours::CreamChroma::kAccentOverrides)
                skin.mappings[accent.key] = pastilleNameForElement(skin, accent.argb);
        }
    }

    NamedSkin makeCompiledSkin(SkinKind kind)
    {
        jassert(isCompiledKind(kind));

        NamedSkin skin;
        skin.kind = kind;
        if (kind == SkinKind::kCompiledBlack)
        {
            skin.id = kCompiledBlackId;
            skin.name = "BLACK/GREEN";
        }
        else if (kind == SkinKind::kCompiledBlackChroma)
        {
            skin.id = kCompiledBlackChromaId;
            skin.name = "BLACK/CHROMA";
        }
        else if (kind == SkinKind::kCompiledBlackMinimal)
        {
            skin.id = kCompiledBlackMinimalId;
            skin.name = "BLACK/MINIMAL";
        }
        else if (kind == SkinKind::kCompiledCreamMinimal)
        {
            skin.id = kCompiledCreamMinimalId;
            skin.name = "CREAM/MINIMAL";
        }
        else if (kind == SkinKind::kCompiledCreamChroma)
        {
            skin.id = kCompiledCreamChromaId;
            skin.name = "CREAM/CHROMA";
        }
        else
        {
            skin.id = kCompiledCreamId;
            skin.name = "CREAM/GREEN";
        }
        skin.derivedFrom = {};
        seedFromCompiled(skin, kind);
        return skin;
    }

    void seedFromCompiled(NamedSkin& skin, SkinKind compiledBase)
    {
        jassert(isCompiledKind(compiledBase));

        if (compiledBase == SkinKind::kCompiledBlackMinimal)
        {
            size_t count = 0;
            seedFromMinimalEntries(skin, blackMinimalEntries(count), count);
            return;
        }
        if (compiledBase == SkinKind::kCompiledCreamMinimal)
        {
            size_t count = 0;
            seedFromMinimalEntries(skin, creamMinimalEntries(count), count);
            return;
        }

        skin.palette.clear();
        skin.mappings.clear();
        forEachCompiledColourElement([&](const SkinColours::ColourElement& element)
        {
            putMapping(skin, element, compiledBase);
        });
        if (compiledBase == SkinKind::kCompiledCreamChroma)
            seedCreamChromaAccents(skin);
    }
}
