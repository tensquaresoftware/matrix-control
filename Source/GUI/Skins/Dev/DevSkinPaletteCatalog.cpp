#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

#include <algorithm>
#include <unordered_set>

#include "GUI/Skins/ColourChart.h"

namespace TSS::DevSkin
{
    namespace
    {
        // Indexed families in chart step order within each hue.
        std::vector<Pastille> chromaAccentPastilles()
        {
            return {
                { "kBlue100", ColourChart::kBlue100 },
                { "kBlue200", ColourChart::kBlue200 },
                { "kBlue400", ColourChart::kBlue400 },
                { "kBlue450", ColourChart::kBlue450 },
                { "kBlue500", ColourChart::kBlue500 },
                { "kBlue600", ColourChart::kBlue600 },
                { "kBlue650", ColourChart::kBlue650 },
                { "kBlue700", ColourChart::kBlue700 },
                { "kBlue750", ColourChart::kBlue750 },
                { "kBlue800", ColourChart::kBlue800 },
                { "kBlue950", ColourChart::kBlue950 },
                { "kOrange100", ColourChart::kOrange100 },
                { "kOrange200", ColourChart::kOrange200 },
                { "kOrange350", ColourChart::kOrange350 },
                { "kOrange400", ColourChart::kOrange400 },
                { "kOrange500", ColourChart::kOrange500 },
                { "kOrange550", ColourChart::kOrange550 },
                { "kOrange600", ColourChart::kOrange600 },
                { "kOrange650", ColourChart::kOrange650 },
                { "kOrange700", ColourChart::kOrange700 },
                { "kOrange750", ColourChart::kOrange750 },
                { "kOrange800", ColourChart::kOrange800 },
                { "kOrange950", ColourChart::kOrange950 },
                { "kYellow100", ColourChart::kYellow100 },
                { "kYellow200", ColourChart::kYellow200 },
                { "kYellow300", ColourChart::kYellow300 },
                { "kYellow400", ColourChart::kYellow400 },
                { "kYellow500", ColourChart::kYellow500 },
                { "kYellow600", ColourChart::kYellow600 },
                { "kYellow650", ColourChart::kYellow650 },
                { "kYellow700", ColourChart::kYellow700 },
                { "kYellow800", ColourChart::kYellow800 },
                { "kYellow850", ColourChart::kYellow850 },
                { "kYellow950", ColourChart::kYellow950 },
            };
        }

        std::vector<Pastille> cafeAndGreenPastilles()
        {
            return {
                { "kCafe50", ColourChart::kCafe50 },
                { "kCafe150", ColourChart::kCafe150 },
                { "kCafe200", ColourChart::kCafe200 },
                { "kCafe300", ColourChart::kCafe300 },
                { "kCafe400", ColourChart::kCafe400 },
                { "kCafe500", ColourChart::kCafe500 },
                { "kCafe600", ColourChart::kCafe600 },
                { "kCafe700", ColourChart::kCafe700 },
                { "kCafe750", ColourChart::kCafe750 },
                { "kCafe850", ColourChart::kCafe850 },
                { "kCafe950", ColourChart::kCafe950 },
                { "kGreen100", ColourChart::kGreen100 },
                { "kGreen200", ColourChart::kGreen200 },
                { "kGreen300", ColourChart::kGreen300 },
                { "kGreen400", ColourChart::kGreen400 },
                { "kGreen500", ColourChart::kGreen500 },
                { "kGreen600", ColourChart::kGreen600 },
                { "kGreen700", ColourChart::kGreen700 },
                { "kGreen750", ColourChart::kGreen750 },
                { "kGreen800", ColourChart::kGreen800 },
                { "kGreen900", ColourChart::kGreen900 },
                { "kGreen950", ColourChart::kGreen950 },
            };
        }
    }

    std::vector<Pastille> colourChartPastilles()
    {
        // Name dictionary only (Shared → greys → reds → Chroma → Cafe/Green).
        std::vector<Pastille> pastilles {
            { "kBrandCream", ColourChart::kBrandCream },
            { "kBrandBlue", ColourChart::kBrandBlue },
            { "kBrandOrange", ColourChart::kBrandOrange },
            { "kWhite", ColourChart::kWhite },
            { "kGrey100", ColourChart::kGrey100 },
            { "kGrey200", ColourChart::kGrey200 },
            { "kGrey300", ColourChart::kGrey300 },
            { "kGrey350", ColourChart::kGrey350 },
            { "kGrey400", ColourChart::kGrey400 },
            { "kGrey500", ColourChart::kGrey500 },
            { "kGrey550", ColourChart::kGrey550 },
            { "kGrey600", ColourChart::kGrey600 },
            { "kGrey700", ColourChart::kGrey700 },
            { "kGrey800", ColourChart::kGrey800 },
            { "kGrey900", ColourChart::kGrey900 },
            { "kGrey950", ColourChart::kGrey950 },
            { "kBlack", ColourChart::kBlack },
            { "kRed500", ColourChart::kRed500 },
            { "kRed600", ColourChart::kRed600 },
            { "kRed700", ColourChart::kRed700 },
            { "kRed800", ColourChart::kRed800 },
            { "kRed900", ColourChart::kRed900 },
        };
        for (const auto& accents : { chromaAccentPastilles(), cafeAndGreenPastilles() })
            pastilles.insert(pastilles.end(), accents.begin(), accents.end());
        return pastilles;
    }

    std::vector<Pastille> designerStarterKitPastilles()
    {
        return {
            { "kBrandCream", ColourChart::kBrandCream },
            { "kBrandBlue", ColourChart::kBrandBlue },
            { "kBrandOrange", ColourChart::kBrandOrange },
            { "kWhite", ColourChart::kWhite },
            { "kBlack", ColourChart::kBlack },
        };
    }

    juce::String pastilleNameForArgb(juce::uint32 argb)
    {
        for (const auto& pastille : colourChartPastilles())
            if (pastille.argb == argb)
                return pastille.name;
        return {};
    }

    std::optional<juce::uint32> argbForPastilleName(const std::vector<Pastille>& palette,
                                                    const juce::String& name)
    {
        for (const auto& pastille : palette)
            if (pastille.name.equalsIgnoreCase(name))
                return pastille.argb;
        return std::nullopt;
    }

    void ensurePastilleOnSkin(NamedSkin& skin, const juce::String& name, juce::uint32 argb)
    {
        for (const auto& pastille : skin.palette)
            if (pastille.name.equalsIgnoreCase(name))
                return;
        skin.palette.push_back({ name, argb, false });
    }

    void prunePaletteToMappedNames(NamedSkin& skin)
    {
        std::unordered_set<std::string> keep;
        for (const auto& [key, name] : skin.mappings)
        {
            juce::ignoreUnused(key);
            keep.insert(name.toLowerCase().toStdString());
        }
        skin.palette.erase(std::remove_if(skin.palette.begin(), skin.palette.end(),
                                          [&](const Pastille& pastille)
                                          {
                                              return keep.find(pastille.name.toLowerCase().toStdString())
                                                     == keep.end();
                                          }),
                           skin.palette.end());
    }

    void ensureDesignerStarterKit(NamedSkin& skin)
    {
        for (const auto& pastille : designerStarterKitPastilles())
            ensurePastilleOnSkin(skin, pastille.name, pastille.argb);
    }
}
