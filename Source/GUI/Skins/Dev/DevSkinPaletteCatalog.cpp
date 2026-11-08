#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

#include "GUI/Skins/ColourChart.h"

namespace TSS::DevSkin
{
    namespace
    {
        std::vector<Pastille> zoneAccentPastilles()
        {
            return {
                { "kBlue950", ColourChart::kBlue950 },
                { "kBlue800", ColourChart::kBlue800 },
                { "kBlue600", ColourChart::kBlue600 },
                { "kBlue400", ColourChart::kBlue400 },
                { "kOrange950", ColourChart::kOrange950 },
                { "kOrange800", ColourChart::kOrange800 },
                { "kOrange700", ColourChart::kOrange700 },
                { "kOrange400", ColourChart::kOrange400 },
                { "kYellow950", ColourChart::kYellow950 },
                { "kYellow800", ColourChart::kYellow800 },
                { "kYellow700", ColourChart::kYellow700 },
                { "kYellow400", ColourChart::kYellow400 },
                { "kCream400", ColourChart::kCream400 },
                { "kCafe400", ColourChart::kCafe400 },
                { "kCafe600", ColourChart::kCafe600 },
                { "kCafe700", ColourChart::kCafe700 },
                { "kCafe800", ColourChart::kCafe800 },
                { "kCafe900", ColourChart::kCafe900 },
                { "kCafe950", ColourChart::kCafe950 },
                { "kGreen100", ColourChart::kGreen100 },
                { "kGreen200", ColourChart::kGreen200 },
                { "kGreen400", ColourChart::kGreen400 },
                { "kGreen500", ColourChart::kGreen500 },
                { "kGreen600", ColourChart::kGreen600 },
                { "kGreen700", ColourChart::kGreen700 },
                { "kGreen800", ColourChart::kGreen800 },
            };
        }
    }

    std::vector<Pastille> colourChartPastilles()
    {
        std::vector<Pastille> pastilles {
            { "kTransparent", ColourChart::kTransparent },
            { "kBlack", ColourChart::kBlack },
            { "kWhite", ColourChart::kWhite },
            { "kCream", ColourChart::kCream },
            { "kGrey950", ColourChart::kGrey950 },
            { "kGrey900", ColourChart::kGrey900 },
            { "kGrey800", ColourChart::kGrey800 },
            { "kGrey700", ColourChart::kGrey700 },
            { "kGrey600", ColourChart::kGrey600 },
            { "kGrey525", ColourChart::kGrey525 },
            { "kGrey500", ColourChart::kGrey500 },
            { "kGrey400", ColourChart::kGrey400 },
            { "kGrey350", ColourChart::kGrey350 },
            { "kGrey300", ColourChart::kGrey300 },
            { "kGrey200", ColourChart::kGrey200 },
            { "kGrey100", ColourChart::kGrey100 },
            { "kRed900", ColourChart::kRed900 },
            { "kRed800", ColourChart::kRed800 },
            { "kRed500", ColourChart::kRed500 },
            { "kOrange", ColourChart::kOrange },
            { "kYellow", ColourChart::kYellow },
            { "kBlue", ColourChart::kBlue },
            { "kEmerald950", ColourChart::kEmerald950 },
            { "kEmerald900", ColourChart::kEmerald900 },
            { "kEmerald700", ColourChart::kEmerald700 },
            { "kEmerald400", ColourChart::kEmerald400 },
        };
        const auto accents = zoneAccentPastilles();
        pastilles.insert(pastilles.end(), accents.begin(), accents.end());
        return pastilles;
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
            if (pastille.name == name)
                return pastille.argb;
        return std::nullopt;
    }
}
