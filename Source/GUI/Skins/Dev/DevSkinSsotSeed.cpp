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

        void putMapping(NamedSkin& skin,
                        const SkinColours::ColourElement& element,
                        bool useBlack)
        {
            const auto argb = useBlack ? element.blackVariant : element.creamVariant;
            const auto name = pastilleNameForElement(skin, argb);
            if (! supportsMultipleColourSets(element.key.object))
            {
                skin.mappings[element.key] = name;
                return;
            }

            for (const auto colourSet : kAllColourSets)
            {
                auto key = element.key;
                key.colourSet = colourSet;
                skin.mappings[key] = name;
            }
        }

        void putMappingExact(NamedSkin& skin,
                             const SkinColours::ColourElement& element,
                             bool useBlack)
        {
            const auto argb = useBlack ? element.blackVariant : element.creamVariant;
            skin.mappings[element.key] = pastilleNameForElement(skin, argb);
        }

        void overlayFormerButtonLikeAtK4(NamedSkin& skin, bool useBlack)
        {
            using namespace SkinColours;

            putMappingExact(skin, Widgets::SliderK4::kTrack, useBlack);
            putMappingExact(skin, Widgets::SliderK4::kFocusBorder, useBlack);
            putMappingExact(skin, Widgets::SliderK4::kValueBar, useBlack);
            putMappingExact(skin, Widgets::SliderK4::kText, useBlack);

            putMappingExact(skin, Widgets::ComboBoxK4::kBackground, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kBackgroundDisabled, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kBorder, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kBorderDisabled, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kFocusBorder, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kText, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kTextDisabled, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kTriangle, useBlack);
            putMappingExact(skin, Widgets::ComboBoxK4::kTriangleDisabled, useBlack);

            putMappingExact(skin, Widgets::PopupMenuK4::kBackground, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kBorder, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kSeparator, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kText, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kTextDisabled, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kBackgroundHover, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kTextHover, useBlack);
            putMappingExact(skin, Widgets::PopupMenuK4::kScrollbar, useBlack);
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
        overlayFormerButtonLikeAtK4(skin, useBlack);
    }
}
