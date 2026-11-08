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
    static TSS::SkinColourKey keyAtK4(TSS::SkinObjectType object, TSS::SkinColourProperty property)
    {
        return TSS::skinColourKey(object, property, TSS::SkinColourState::kDefault, TSS::ColourSet::k4);
    }

    void assertProgressBarK4MatchesSliderK4(const TSS::DevSkin::NamedSkin& skin)
    {
        const auto progressTrack = keyAtK4(TSS::SkinObjectType::kProgressBar, TSS::SkinColourProperty::kTrack);
        const auto progressValue = keyAtK4(TSS::SkinObjectType::kProgressBar, TSS::SkinColourProperty::kValueBar);
        const auto progressText = keyAtK4(TSS::SkinObjectType::kProgressBar, TSS::SkinColourProperty::kText);
        const auto sliderValue = keyAtK4(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kValueBar);
        const auto sliderText = keyAtK4(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);

        expectEquals(skin.mappings.at(progressTrack), juce::String("kBlack"));
        expectEquals(skin.mappings.at(progressValue), skin.mappings.at(sliderValue));
        expectEquals(skin.mappings.at(progressText), skin.mappings.at(sliderText));
        expectEquals(skin.mappings.at(progressValue), juce::String("kGrey600"));
        expectEquals(skin.mappings.at(progressText), juce::String("kGrey200"));
    }

    void assertCompiledSeedOverlaysFormerButtonLikeAtK4()
    {
        const auto assertOverlay = [this](TSS::DevSkin::SkinKind kind)
        {
            const auto skin = TSS::DevSkin::makeCompiledSkin(kind);
            const auto comboBgK4 = keyAtK4(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kBackground);
            const auto sliderTrackK4 = keyAtK4(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kTrack);
            const auto popupBorderK4 = keyAtK4(TSS::SkinObjectType::kPopupMenu, TSS::SkinColourProperty::kBorder);

            expectEquals(skin.mappings.at(comboBgK4), juce::String("kBlack"));
            expectEquals(skin.mappings.at(sliderTrackK4), juce::String("kBlack"));
            assertProgressBarK4MatchesSliderK4(skin);
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
