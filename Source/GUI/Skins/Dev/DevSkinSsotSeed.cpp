#include "GUI/Skins/Dev/DevSkinSsotSeed.h"

#include "GUI/Skins/Dev/DevSkinColourElementList.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/SkinColours.h"

namespace TSS::DevSkin
{
    namespace
    {
        juce::String pastilleNameForElement(NamedSkin& skin, juce::uint32 argb)
        {
            auto name = pastilleNameForArgb(argb);
            if (name.isEmpty())
            {
                name = juce::String::formatted("custom_%08X", static_cast<unsigned>(argb));
                bool found = false;
                for (const auto& pastille : skin.palette)
                {
                    if (pastille.name == name)
                    {
                        found = true;
                        break;
                    }
                }
                if (! found)
                    skin.palette.push_back({ name, argb });
            }
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

        bool isCompiledKind(SkinKind kind) noexcept
        {
            return kind == SkinKind::kCompiledBlack
                || kind == SkinKind::kCompiledBlackChroma
                || kind == SkinKind::kCompiledCream;
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

        skin.palette = colourChartPastilles();
        skin.mappings.clear();
        forEachCompiledColourElement([&](const SkinColours::ColourElement& element)
        {
            putMapping(skin, element, compiledBase);
        });
    }
}
