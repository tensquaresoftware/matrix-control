#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"
#include "GUI/Skins/SkinColoursWidgetsDisplays.h"
#include "GUI/Skins/SkinColoursWidgetsSelection.h"

class NumberBoxDisabledBackgroundColourTests : public juce::UnitTest
{
public:
    NumberBoxDisabledBackgroundColourTests()
        : juce::UnitTest("NumberBoxDisabledBackgroundColour")
    {
    }

    void runTest() override
    {
        using namespace TSS::SkinColours::Widgets;

        beginTest("disabledBackground_matchesButtonAndComboBoxK4Grey950");

        expect(NumberBox::kBackgroundDisabled.blackVariant == ColourChart::kGrey950);
        expect(NumberBox::kBackgroundDisabled.creamVariant == ColourChart::kGrey950);
        expect(NumberBox::kBackgroundDisabled.blackVariant
               == Button::kBackgroundDisabled.blackVariant);
        expect(NumberBox::kBackgroundDisabled.creamVariant
               == Button::kBackgroundDisabled.creamVariant);
        expect(NumberBox::kBackgroundDisabled.blackVariant
               == ComboBoxK4::kBackgroundDisabled.blackVariant);
        expect(NumberBox::kBackgroundDisabled.creamVariant
               == ComboBoxK4::kBackgroundDisabled.creamVariant);

        beginTest("enabledBackground_isBlack");

        expect(NumberBox::kBackground.blackVariant == ColourChart::kBlack);
        expect(NumberBox::kBackground.creamVariant == ColourChart::kBlack);

        beginTest("skinLoaded_disabledBackgroundIsGrey950");

        assertDisabledBackgroundFromSkin(TSS::Skin::ColourVariant::Black);
        assertDisabledBackgroundFromSkin(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertDisabledBackgroundFromSkin(TSS::Skin::ColourVariant variant)
    {
        using TSS::SkinColourProperty;
        using TSS::SkinColourState;
        using TSS::SkinObjectType;

        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::numberBoxLookFromSkin(*skin);
        const auto mapped = skin->getColour(SkinObjectType::kNumberBox,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kDisabled);
        expect(look.backgroundDisabled == mapped);
        expect(look.backgroundDisabled == juce::Colour(ColourChart::kGrey950));
    }
};

static NumberBoxDisabledBackgroundColourTests numberBoxDisabledBackgroundColourTests;
