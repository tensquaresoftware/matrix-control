#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

class SliderButtonLikeColourTests : public juce::UnitTest
{
public:
    SliderButtonLikeColourTests()
        : juce::UnitTest("SliderButtonLikeColour")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("buttonLikeEnabled_matchesApprovedGreys");

        expect(Slider::ButtonLike::kTrack.blackVariant == ColourChart::kBlack);
        expect(Slider::ButtonLike::kTrack.creamVariant == ColourChart::kBlack);
        expect(Slider::ButtonLike::kFocusBorder.blackVariant == ColourChart::kDarkGrey3);
        expect(Slider::ButtonLike::kFocusBorder.creamVariant == ColourChart::kDarkGrey3);
        expect(Slider::ButtonLike::kValueBar.blackVariant == ColourChart::kDarkGrey5);
        expect(Slider::ButtonLike::kValueBar.creamVariant == ColourChart::kDarkGrey5);
        expect(Slider::ButtonLike::kText.blackVariant == ColourChart::kLightGrey2);
        expect(Slider::ButtonLike::kText.creamVariant == ColourChart::kLightGrey2);

        beginTest("standardSliderEnabled_stillGreen");

        expect(Slider::kTrack.blackVariant == ColourChart::kGreen1);
        expect(Slider::kFocusBorder.blackVariant == ColourChart::kGreen2);
        expect(Slider::kValueBar.blackVariant == ColourChart::kGreen3);
        expect(Slider::kText.blackVariant == ColourChart::kGreen4);

        beginTest("sharedDisabledTokens_unchanged");

        expect(Slider::kValueBarDisabled.blackVariant == ColourChart::kDarkGrey4);
        expect(Slider::kValueBarDisabled.creamVariant == ColourChart::kDarkGrey4);

        beginTest("buttonLikeLookFromSkin_mapsApprovedGreys");

        assertButtonLikeLook(TSS::Skin::ColourVariant::Black);
        assertButtonLikeLook(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertButtonLikeLook(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::sliderLookButtonLikeFromSkin(*skin);

        expect(look.trackEnabled == juce::Colour(ColourChart::kBlack));
        expect(look.focusBorder == juce::Colour(ColourChart::kDarkGrey3));
        expect(look.valueBarEnabled == juce::Colour(ColourChart::kDarkGrey5));
        expect(look.textEnabled == juce::Colour(ColourChart::kLightGrey2));
    }
};

static SliderButtonLikeColourTests sliderButtonLikeColourTests;
