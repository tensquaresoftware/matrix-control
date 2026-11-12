#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

#include "GUI/Skins/ColourChart.h"

namespace TSS::DevSkin
{
    namespace
    {
        std::vector<Pastille> zoneAccentPastilles()
        {
            // Indexed families: lightest → darkest within each hue.
            return {
                { "kBlue400", ColourChart::kBlue400 },
                { "kBlue600", ColourChart::kBlue600 },
                { "kBlue800", ColourChart::kBlue800 },
                { "kBlue950", ColourChart::kBlue950 },
                { "kOrange400", ColourChart::kOrange400 },
                { "kOrange500", ColourChart::kOrange500 },
                { "kOrange700", ColourChart::kOrange700 },
                { "kOrange800", ColourChart::kOrange800 },
                { "kOrange950", ColourChart::kOrange950 },
                { "kYellow400", ColourChart::kYellow400 },
                { "kYellow700", ColourChart::kYellow700 },
                { "kYellow800", ColourChart::kYellow800 },
                { "kYellow950", ColourChart::kYellow950 },
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
        // Product colours: Brand*, White→Grey→Black, then accents (no Transparent).
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
            { "kRed800", ColourChart::kRed800 },
            { "kRed900", ColourChart::kRed900 },
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
