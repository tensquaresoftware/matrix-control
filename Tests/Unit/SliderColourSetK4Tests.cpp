#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

using TSS::SkinObjectType;
using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::ColourSet;

class SliderColourSetK4Tests : public juce::UnitTest
{
public:
    SliderColourSetK4Tests()
        : juce::UnitTest("SliderColourSetK4")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("k4SeedElements_matchApprovedGreys");

        expect(SliderK4::kTrack.blackVariant == ColourChart::kBlack);
        expect(SliderK4::kTrack.creamVariant == ColourChart::kBlack);
        expect(SliderK4::kFocusBorder.blackVariant == ColourChart::kGrey800);
        expect(SliderK4::kFocusBorder.creamVariant == ColourChart::kGrey800);
        expect(SliderK4::kValueBar.blackVariant == ColourChart::kGrey600);
        expect(SliderK4::kValueBar.creamVariant == ColourChart::kGrey600);
        expect(SliderK4::kText.blackVariant == ColourChart::kGrey200);
        expect(SliderK4::kText.creamVariant == ColourChart::kGrey200);

        beginTest("standardSliderEnabled_stillGreen");

        expect(Slider::kTrack.blackVariant == ColourChart::kEmerald950);
        expect(Slider::kFocusBorder.blackVariant == ColourChart::kEmerald900);
        expect(Slider::kValueBar.blackVariant == ColourChart::kEmerald700);
        expect(Slider::kText.blackVariant == ColourChart::kEmerald400);

        beginTest("skinLoaded_standardSliderEnabledMatchesEmeraldStops");

        assertStandardSliderEmeraldFromSkin(TSS::Skin::ColourVariant::Black);
        assertStandardSliderEmeraldFromSkin(TSS::Skin::ColourVariant::Cream);

        beginTest("sharedDisabledTokens_unchanged");

        expect(Slider::kValueBarDisabled.blackVariant == ColourChart::kGrey700);
        expect(Slider::kValueBarDisabled.creamVariant == ColourChart::kGrey700);

        beginTest("sliderLookFromSkin_k4_mapsApprovedGreys");

        assertK4Look(TSS::Skin::ColourVariant::Black);
        assertK4Look(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertStandardSliderEmeraldFromSkin(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        expect(skin->getSliderTrackColour(true) == juce::Colour(ColourChart::kEmerald950));
        expect(skin->getSliderValueBarColour(true) == juce::Colour(ColourChart::kEmerald700));
        expect(skin->getSliderTextColour(true) == juce::Colour(ColourChart::kEmerald400));
    }

    void assertK4Look(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::sliderLookFromSkin(*skin, ColourSet::k4);

        expect(look.trackEnabled == juce::Colour(ColourChart::kBlack));
        expect(look.focusBorder == juce::Colour(ColourChart::kGrey800));
        expect(look.valueBarEnabled == juce::Colour(ColourChart::kGrey600));
        expect(look.textEnabled == juce::Colour(ColourChart::kGrey200));

        // Disabled roles reuse the shared slider tokens; editor stays high-contrast white.
        expect(look.trackDisabled == skin->getSliderTrackColour(false, ColourSet::k4));
        expect(look.backgroundDisabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDisabled,
                                  ColourSet::k4));
        expect(look.valueBarDisabled == skin->getSliderValueBarColour(false, ColourSet::k4));
        expect(look.textDisabled == skin->getSliderTextColour(false, ColourSet::k4));
        expect(look.editorText
               == skin->getColour(SkinObjectType::kNumberBox,
                                  SkinColourProperty::kEditorText,
                                  SkinColourState::kDefault,
                                  ColourSet::k4));
        expect(look.editorCaret
               == skin->getColour(SkinObjectType::kNumberBox,
                                  SkinColourProperty::kEditorText,
                                  SkinColourState::kDefault,
                                  ColourSet::k4));
    }
};

static SliderColourSetK4Tests sliderColourSetK4Tests;
