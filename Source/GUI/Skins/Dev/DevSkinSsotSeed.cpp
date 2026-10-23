#include "GUI/Skins/Dev/DevSkinSsotSeed.h"

#include "GUI/Skins/Dev/DevSkinColourElementList.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

namespace TSS::DevSkin
{
    namespace
    {
        void putMapping(NamedSkin& skin,
                        const SkinColours::ColourElement& element,
                        bool useBlack)
        {
            const auto argb = useBlack ? element.blackVariant : element.creamVariant;
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
            skin.mappings[element.key] = name;
        }
    }

    NamedSkin makeCompiledSkin(SkinKind kind)
    {
        jassert(kind == SkinKind::kCompiledBlack || kind == SkinKind::kCompiledCream);

        NamedSkin skin;
        skin.kind = kind;
        skin.id = (kind == SkinKind::kCompiledBlack) ? kCompiledBlackId : kCompiledCreamId;
        skin.name = (kind == SkinKind::kCompiledBlack) ? "Black" : "Cream";
        skin.derivedFrom = {};
        seedFromCompiled(skin, kind);
        return skin;
    }

    void seedFromCompiled(NamedSkin& skin, SkinKind compiledBase)
    {
        jassert(compiledBase == SkinKind::kCompiledBlack || compiledBase == SkinKind::kCompiledCream);

        skin.palette = colourChartPastilles();
        skin.mappings.clear();
        const bool useBlack = compiledBase == SkinKind::kCompiledBlack;
        forEachCompiledColourElement([&](const SkinColours::ColourElement& element)
        {
            putMapping(skin, element, useBlack);
        });
    }
}
