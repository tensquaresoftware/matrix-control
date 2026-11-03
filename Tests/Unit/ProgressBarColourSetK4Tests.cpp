#include <juce_core/juce_core.h>

#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursCommon.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

using TSS::SkinObjectType;
using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::ColourSet;

class ProgressBarColourSetK4Tests : public juce::UnitTest
{
public:
    ProgressBarColourSetK4Tests()
        : juce::UnitTest("ProgressBarColourSetK4")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("k4SeedElements_matchSliderK4Greys");

        expect(ProgressBarK4::kTrack.blackVariant == SliderK4::kTrack.blackVariant);
        expect(ProgressBarK4::kTrack.creamVariant == SliderK4::kTrack.creamVariant);
        expect(ProgressBarK4::kValueBar.blackVariant == SliderK4::kValueBar.blackVariant);
        expect(ProgressBarK4::kValueBar.creamVariant == SliderK4::kValueBar.creamVariant);
        expect(ProgressBarK4::kText.blackVariant == SliderK4::kText.blackVariant);
        expect(ProgressBarK4::kText.creamVariant == SliderK4::kText.creamVariant);
        expect(ProgressBarK4::kTrack.blackVariant == ColourChart::kBlack);
        expect(ProgressBarK4::kValueBar.blackVariant == ColourChart::kGrey600);
        expect(ProgressBarK4::kText.blackVariant == ColourChart::kGrey200);

        beginTest("baseSeed_copiesSliderEmeraldStops");

        expect(ProgressBar::kTrack.blackVariant == Slider::kTrack.blackVariant);
        expect(ProgressBar::kTrack.creamVariant == Slider::kTrack.creamVariant);
        expect(ProgressBar::kValueBar.blackVariant == Slider::kValueBar.blackVariant);
        expect(ProgressBar::kValueBar.creamVariant == Slider::kValueBar.creamVariant);
        expect(ProgressBar::kText.blackVariant == Slider::kText.blackVariant);
        expect(ProgressBar::kText.creamVariant == Slider::kText.creamVariant);
        expect(ProgressBar::kTrackDisabled.blackVariant == Slider::kTrackDisabled.blackVariant);
        expect(ProgressBar::kTrackDisabled.creamVariant == Slider::kTrackDisabled.creamVariant);
        expect(ProgressBar::kValueBarDisabled.blackVariant == Slider::kValueBarDisabled.blackVariant);
        expect(ProgressBar::kValueBarDisabled.creamVariant == Slider::kValueBarDisabled.creamVariant);
        expect(ProgressBar::kTextDisabled.blackVariant == Slider::kTextDisabled.blackVariant);
        expect(ProgressBar::kTextDisabled.creamVariant == Slider::kTextDisabled.creamVariant);

        beginTest("modalPaintContract_k4GreysNotSliderEmerald");

        assertModalPaintContract(TSS::Skin::ColourVariant::Black);
        assertModalPaintContract(TSS::Skin::ColourVariant::Cream);

        beginTest("k1StaysEmerald_afterK4Overlay");

        assertK1StaysEmerald(TSS::Skin::ColourVariant::Black);
        assertK1StaysEmerald(TSS::Skin::ColourVariant::Cream);

        beginTest("disabledPhaseLaneContract_k4UsesDisabledSeeds");

        assertDisabledPhaseLaneContract(TSS::Skin::ColourVariant::Black);
        assertDisabledPhaseLaneContract(TSS::Skin::ColourVariant::Cream);
    }

private:
    static juce::Colour progressColour(const TSS::Skin& skin,
                                       SkinColourProperty property,
                                       ColourSet colourSet)
    {
        return skin.getColour(SkinObjectType::kProgressBar,
                              property,
                              SkinColourState::kDefault,
                              colourSet);
    }

    static juce::Colour sliderColour(const TSS::Skin& skin,
                                     SkinColourProperty property,
                                     ColourSet colourSet)
    {
        return skin.getColour(SkinObjectType::kSlider,
                              property,
                              SkinColourState::kDefault,
                              colourSet);
    }

    void assertModalPaintContract(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto track = progressColour(*skin, SkinColourProperty::kTrack, ColourSet::k4);
        const auto valueBar = progressColour(*skin, SkinColourProperty::kValueBar, ColourSet::k4);
        const auto text = progressColour(*skin, SkinColourProperty::kText, ColourSet::k4);

        expect(track == juce::Colour(ColourChart::kBlack));
        expect(valueBar == juce::Colour(ColourChart::kGrey600));
        expect(text == juce::Colour(ColourChart::kGrey200));
        expect(track == sliderColour(*skin, SkinColourProperty::kTrack, ColourSet::k4));
        expect(valueBar == sliderColour(*skin, SkinColourProperty::kValueBar, ColourSet::k4));
        expect(text == sliderColour(*skin, SkinColourProperty::kText, ColourSet::k4));
        expect(track != sliderColour(*skin, SkinColourProperty::kTrack, ColourSet::k1));
        expect(valueBar != sliderColour(*skin, SkinColourProperty::kValueBar, ColourSet::k1));
        expect(text != sliderColour(*skin, SkinColourProperty::kText, ColourSet::k1));
        expect(track != juce::Colour(ColourChart::kEmerald950));
        expect(valueBar != juce::Colour(ColourChart::kEmerald700));
        expect(text != juce::Colour(ColourChart::kEmerald400));
    }

    void assertK1StaysEmerald(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        expect(progressColour(*skin, SkinColourProperty::kTrack, ColourSet::k1)
               == juce::Colour(ColourChart::kEmerald950));
        expect(progressColour(*skin, SkinColourProperty::kValueBar, ColourSet::k1)
               == juce::Colour(ColourChart::kEmerald700));
        expect(progressColour(*skin, SkinColourProperty::kText, ColourSet::k1)
               == juce::Colour(ColourChart::kEmerald400));
        expect(progressColour(*skin, SkinColourProperty::kTrack, ColourSet::k1)
               == sliderColour(*skin, SkinColourProperty::kTrack, ColourSet::k1));
        expect(progressColour(*skin, SkinColourProperty::kValueBar, ColourSet::k1)
               == sliderColour(*skin, SkinColourProperty::kValueBar, ColourSet::k1));
        expect(progressColour(*skin, SkinColourProperty::kText, ColourSet::k1)
               == sliderColour(*skin, SkinColourProperty::kText, ColourSet::k1));
        expect(progressColour(*skin, SkinColourProperty::kTrack, ColourSet::k1)
               != progressColour(*skin, SkinColourProperty::kTrack, ColourSet::k4));
    }

    void assertDisabledPhaseLaneContract(TSS::Skin::ColourVariant variant)
    {
        using namespace TSS::SkinColours;

        const auto skin = TSS::Skin::create(variant);

        expect(skin->getColour(SkinObjectType::kProgressBar,
                               SkinColourProperty::kTrack,
                               SkinColourState::kDisabled,
                               ColourSet::k4)
               == juce::Colour(Common::kDecorationDisabled));
        expect(skin->getColour(SkinObjectType::kProgressBar,
                               SkinColourProperty::kText,
                               SkinColourState::kDisabled,
                               ColourSet::k4)
               == juce::Colour(Common::kContentDisabled));
        expect(skin->getColour(SkinObjectType::kProgressBar,
                               SkinColourProperty::kValueBar,
                               SkinColourState::kDisabled,
                               ColourSet::k4)
               == juce::Colour(Widgets::ProgressBar::kValueBarDisabled.blackVariant));
        expect(Widgets::ProgressBar::kValueBarDisabled.blackVariant == ColourChart::kGrey700);
        expect(Widgets::ProgressBar::kValueBarDisabled.creamVariant == ColourChart::kGrey700);
    }
};

static ProgressBarColourSetK4Tests progressBarColourSetK4Tests;
