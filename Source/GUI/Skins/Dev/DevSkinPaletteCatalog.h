#pragma once

#include <vector>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    /**
     * Full ColourChart dictionary for ARGB → stable pastille name during seed.
     * Not injected wholesale into skin palettes (palettes are slim / skin-owned).
     */
    std::vector<Pastille> colourChartPastilles();

    /** Fixed starter pastilles for new Designer skins (Brand* + Black + White). */
    std::vector<Pastille> designerStarterKitPastilles();

    /** Resolve ARGB to a ColourChart pastille name, or empty if unknown. */
    juce::String pastilleNameForArgb(juce::uint32 argb);

    std::optional<juce::uint32> argbForPastilleName(const std::vector<Pastille>& palette,
                                                    const juce::String& name);

    /** Ensure name+argb exists on skin.palette (no-op if name already present). */
    void ensurePastilleOnSkin(NamedSkin& skin, const juce::String& name, juce::uint32 argb);

    /** Drop pastilles not referenced by mappings. */
    void prunePaletteToMappedNames(NamedSkin& skin);

    /** Ensure starter-kit pastilles exist (does not remove others). */
    void ensureDesignerStarterKit(NamedSkin& skin);
}
