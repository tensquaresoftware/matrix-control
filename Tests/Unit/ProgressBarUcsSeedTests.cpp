#include <juce_core/juce_core.h>

#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursCommon.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

using TSS::SkinObjectType;
using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::ColourSet;

class ProgressBarUcsSeedTests : public juce::UnitTest
{
public:
    ProgressBarUcsSeedTests()
        : juce::UnitTest("ProgressBarUcsSeed")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("ucsSeed_matchesModalesSliderOnBothSkins");

        expect(ProgressBar::kTrack.blackChromaVariant == SliderK4::kTrack.blackChromaVariant);
        expect(ProgressBar::kTrack.creamVariant == SliderK4::kTrack.creamVariant);
        expect(ProgressBar::kValueBar.blackChromaVariant == SliderK4::kValueBar.blackChromaVariant);
        expect(ProgressBar::kValueBar.creamVariant == SliderK4::kValueBar.creamVariant);
        expect(ProgressBar::kText.blackChromaVariant == SliderK4::kText.blackChromaVariant);
        expect(ProgressBar::kText.creamVariant == SliderK4::kText.creamVariant);
        expect(ProgressBar::kTrack.blackChromaVariant == ColourChart::kBlack);
        expect(ProgressBar::kValueBar.blackChromaVariant == ColourChart::kGrey600);
        expect(ProgressBar::kText.blackChromaVariant == ColourChart::kGrey200);

        beginTest("ucs_onlyK1IsSeeded");

        expect(! TSS::supportsColourSet(SkinObjectType::kProgressBar, ColourSet::k2));
        assertHigherSetFallsBackToK1(TSS::Skin::ColourVariant::BlackChroma);
        assertHigherSetFallsBackToK1(TSS::Skin::ColourVariant::Cream);

        beginTest("disabledContract_pinsBlackChromaSharedAndCreamCafeSeeds");

        assertDisabledContract();
    }

private:
    static juce::Colour progressColour(const TSS::Skin& skin,
                                       SkinColourProperty property,
                                       ColourSet colourSet = ColourSet::k1)
    {
        return skin.getColour(SkinObjectType::kProgressBar,
                              property,
                              SkinColourState::kDefault,
                              colourSet);
    }

    void assertHigherSetFallsBackToK1(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        for (const auto property : { SkinColourProperty::kTrack,
                                     SkinColourProperty::kValueBar,
                                     SkinColourProperty::kText })
            expect(progressColour(*skin, property, ColourSet::k4) == progressColour(*skin, property));
    }

    void assertDisabledContract()
    {
        using namespace TSS::SkinColours;

        const auto blackDuo = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        expect(blackDuo->getColour(SkinObjectType::kProgressBar,
                                   SkinColourProperty::kTrack,
                                   SkinColourState::kDisabled)
               == juce::Colour(Common::kDecorationDisabled));
        expect(blackDuo->getColour(SkinObjectType::kProgressBar,
                                   SkinColourProperty::kText,
                                   SkinColourState::kDisabled)
               == juce::Colour(Common::kContentDisabled));
        expect(blackDuo->getColour(SkinObjectType::kProgressBar,
                                   SkinColourProperty::kValueBar,
                                   SkinColourState::kDisabled)
               == juce::Colour(ColourChart::kGrey700));

        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        expect(cream->getColour(SkinObjectType::kProgressBar,
                                SkinColourProperty::kTrack,
                                SkinColourState::kDisabled)
               == juce::Colour(ColourChart::kCream));
        expect(cream->getColour(SkinObjectType::kProgressBar,
                                SkinColourProperty::kText,
                                SkinColourState::kDisabled)
               == juce::Colour(ColourChart::kCafe600));
        expect(cream->getColour(SkinObjectType::kProgressBar,
                                SkinColourProperty::kValueBar,
                                SkinColourState::kDisabled)
               == juce::Colour(ColourChart::kCafe600));
    }
};

static ProgressBarUcsSeedTests progressBarUcsSeedTests;
