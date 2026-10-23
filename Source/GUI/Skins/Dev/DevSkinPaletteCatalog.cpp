#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

#include "GUI/Skins/ColourChart.h"

namespace TSS::DevSkin
{
    std::vector<Pastille> colourChartPastilles()
    {
        return {
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
