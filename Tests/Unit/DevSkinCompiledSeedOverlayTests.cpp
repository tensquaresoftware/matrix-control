#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinSsotSeed.h"
#include "GUI/Skins/SkinValues.h"

class DevSkinCompiledSeedOverlayTests : public juce::UnitTest
{
public:
    DevSkinCompiledSeedOverlayTests()
        : juce::UnitTest("DevSkinCompiledSeedOverlay")
    {
    }

    void runTest() override
    {
        beginTest("compiledSeed_overlayFormerButtonLikeAtK4");
        assertCompiledSeedOverlaysFormerButtonLikeAtK4();
    }

private:
    void assertCompiledSeedOverlaysFormerButtonLikeAtK4()
    {
        const auto assertOverlay = [this](TSS::DevSkin::SkinKind kind)
        {
            const auto skin = TSS::DevSkin::makeCompiledSkin(kind);

            const auto comboBgK4 = TSS::skinColourKey(TSS::SkinObjectType::kComboBox,
                                                      TSS::SkinColourProperty::kBackground,
                                                      TSS::SkinColourState::kDefault,
                                                      TSS::ColourSet::k4);
            const auto sliderTrackK4 = TSS::skinColourKey(TSS::SkinObjectType::kSlider,
                                                          TSS::SkinColourProperty::kTrack,
                                                          TSS::SkinColourState::kDefault,
                                                          TSS::ColourSet::k4);
            const auto popupBorderK4 = TSS::skinColourKey(TSS::SkinObjectType::kPopupMenu,
                                                          TSS::SkinColourProperty::kBorder,
                                                          TSS::SkinColourState::kDefault,
                                                          TSS::ColourSet::k4);
            expectEquals(skin.mappings.at(comboBgK4), juce::String("kBlack"));
            expectEquals(skin.mappings.at(sliderTrackK4), juce::String("kBlack"));
            expectEquals(skin.mappings.at(popupBorderK4), juce::String("kGrey600"));

            auto comboBgK1 = comboBgK4;
            comboBgK1.colourSet = TSS::ColourSet::k1;
            expectEquals(skin.mappings.at(comboBgK1), juce::String("kEmerald950"));
        };

        assertOverlay(TSS::DevSkin::SkinKind::kCompiledBlack);
        assertOverlay(TSS::DevSkin::SkinKind::kCompiledCream);
    }
};

static DevSkinCompiledSeedOverlayTests devSkinCompiledSeedOverlayTests;
