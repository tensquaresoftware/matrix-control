#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

using TSS::SkinObjectType;
using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::ColourSet;
using TSS::SkinZone;

class SliderZoneSeedTests : public juce::UnitTest
{
public:
    SliderZoneSeedTests()
        : juce::UnitTest("SliderZoneSeed")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("blackDuo_zoneAccentsFollowBlueOrangeYellow");

        expect(Slider::kTrack.blackDuoVariant == ColourChart::kBlue950);
        expect(Slider::kValueBar.blackDuoVariant == ColourChart::kBlue600);
        expect(Slider::kText.blackDuoVariant == ColourChart::kBlue400);
        expect(SliderK2::kTrack.blackDuoVariant == ColourChart::kOrange950);
        expect(SliderK2::kValueBar.blackDuoVariant == ColourChart::kOrange700);
        expect(SliderK2::kText.blackDuoVariant == ColourChart::kOrange400);
        expect(SliderK3::kTrack.blackDuoVariant == ColourChart::kYellow950);
        expect(SliderK3::kValueBar.blackDuoVariant == ColourChart::kYellow700);
        expect(SliderK3::kText.blackDuoVariant == ColourChart::kYellow400);

        beginTest("modales_monoGreysOnBlackDuoAndCafeOnCream");

        expect(SliderK4::kTrack.blackDuoVariant == ColourChart::kBlack);
        expect(SliderK4::kFocusBorder.blackDuoVariant == ColourChart::kGrey800);
        expect(SliderK4::kValueBar.blackDuoVariant == ColourChart::kGrey600);
        expect(SliderK4::kText.blackDuoVariant == ColourChart::kGrey200);
        expect(SliderK4::kTrack.creamVariant == ColourChart::kCream);
        expect(SliderK4::kValueBar.creamVariant == ColourChart::kCafe800);
        expect(SliderK4::kText.creamVariant == ColourChart::kCafe800);

        beginTest("cream_patchMasterHeaderShareGreens");

        expect(Slider::kTrack.creamVariant == ColourChart::kGreen600);
        expect(SliderK2::kTrack.creamVariant == Slider::kTrack.creamVariant);
        expect(SliderK3::kTrack.creamVariant == Slider::kTrack.creamVariant);
        expect(Slider::kText.creamVariant == ColourChart::kGreen100);

        beginTest("skinLoaded_patchSliderMatchesBlueStops");

        assertPatchSliderFromSkin();

        beginTest("sliderLookForZone_modalesMapsMonoGreys");

        assertModalesLook(TSS::Skin::ColourVariant::BlackDuo);
        assertModalesLook(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertPatchSliderFromSkin()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        expect(skin->getSliderTrackColour(true) == juce::Colour(ColourChart::kBlue950));
        expect(skin->getSliderValueBarColour(true) == juce::Colour(ColourChart::kBlue600));
        expect(skin->getSliderTextColour(true) == juce::Colour(ColourChart::kBlue400));
    }

    void assertModalesLook(TSS::Skin::ColourVariant variant)
    {
        using namespace TSS::SkinColours::Widgets;

        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::sliderLookForZone(*skin, SkinZone::kModales);
        auto seed = [variant](const TSS::SkinColours::ColourElement& element)
        {
            if (variant == TSS::Skin::ColourVariant::BlackDuo)
                return juce::Colour(element.blackDuoVariant);
            if (variant == TSS::Skin::ColourVariant::Black)
                return juce::Colour(element.blackVariant);
            return juce::Colour(element.creamVariant);
        };

        expect(look.trackEnabled == seed(SliderK4::kTrack));
        expect(look.focusBorder == seed(SliderK4::kFocusBorder));
        expect(look.valueBarEnabled == seed(SliderK4::kValueBar));
        expect(look.textEnabled == seed(SliderK4::kText));
        expect(look.trackDisabled == skin->getSliderTrackColour(false, ColourSet::k4));
        expect(look.backgroundDisabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDisabled,
                                  ColourSet::k4));
        expect(look.valueBarDisabled == skin->getSliderValueBarColour(false, ColourSet::k4));
        expect(look.textDisabled == skin->getSliderTextColour(false, ColourSet::k4));
        expect(look.editorText == skin->getColour(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText));
    }
};

static SliderZoneSeedTests sliderZoneSeedTests;
