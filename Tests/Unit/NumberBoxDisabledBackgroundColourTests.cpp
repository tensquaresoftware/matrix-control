#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"
#include "GUI/Skins/SkinColoursWidgetsDisplays.h"

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

        beginTest("disabledBackground_matchesButtonDisabled");

        expect(NumberBox::kBackgroundDisabled.blackVariant == ColourChart::kGrey950);
        expect(NumberBox::kBackgroundDisabled.creamVariant == ColourChart::kBrandCream);
        expect(NumberBox::kBackgroundDisabled.blackVariant
               == Button::kBackgroundDisabled.blackVariant);
        expect(NumberBox::kBackgroundDisabled.creamVariant
               == Button::kBackgroundDisabled.creamVariant);

        beginTest("enabledBackground_blackPlateOnBlackChroma_creamPlateOnCream");

        expect(NumberBox::kBackground.blackVariant == ColourChart::kBlack);
        expect(NumberBox::kBackground.creamVariant == ColourChart::kCafe50);

        beginTest("skinLoaded_disabledBackgroundFollowsSeed");

        assertDisabledBackgroundFromSkin(TSS::Skin::ColourVariant::Black, ColourChart::kGrey950);
        assertDisabledBackgroundFromSkin(TSS::Skin::ColourVariant::Cream, ColourChart::kBrandCream);

        beginTest("caret_seedIsWhiteAndIndependentOfEditorText");

        expect(NumberBox::kCaret.blackVariant == ColourChart::kWhite);
        expect(NumberBox::kCaret.creamVariant == ColourChart::kWhite);
        expect(NumberBox::kCaret.blackVariant == NumberBox::kEditorText.blackVariant);
        assertCaretFromSkin(TSS::Skin::ColourVariant::Black);
        assertCaretFromSkin(TSS::Skin::ColourVariant::Cream);

        beginTest("remapCaretOnly_leavesEditorTextWhite");

        assertRemapCaretLeavesEditorText();

        beginTest("borders_seedFromFormerButtonBorders");

        expect(NumberBox::kBorder.blackVariant == Button::kBorderOn.blackVariant);
        expect(NumberBox::kBorder.creamVariant == Button::kBorderOn.creamVariant);
        expect(NumberBox::kBorderDisabled.blackVariant == Button::kBorderOff.blackVariant);
        expect(NumberBox::kBorderDisabled.creamVariant == Button::kBorderOff.creamVariant);

        beginTest("remapButtonBorders_leavesNumberBoxBorders");

        assertRemapButtonBordersLeavesNumberBox();
    }

private:
    void assertDisabledBackgroundFromSkin(TSS::Skin::ColourVariant variant, juce::uint32 expectedArgb)
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
        expect(look.backgroundDisabled == juce::Colour(expectedArgb));
    }

    void assertCaretFromSkin(TSS::Skin::ColourVariant variant)
    {
        using TSS::SkinColourProperty;
        using TSS::SkinColourState;
        using TSS::SkinObjectType;

        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::numberBoxLookFromSkin(*skin);
        const auto caret = skin->getColour(SkinObjectType::kNumberBox,
                                           SkinColourProperty::kCaret,
                                           SkinColourState::kDefault);
        const auto editorText = skin->getColour(SkinObjectType::kNumberBox,
                                                SkinColourProperty::kEditorText,
                                                SkinColourState::kDefault);
        expect(look.editorCaret == caret);
        expect(look.editorText == editorText);
        expect(look.editorCaret == juce::Colour(ColourChart::kWhite));
    }

    void assertRemapCaretLeavesEditorText()
    {
        using TSS::SkinColourProperty;
        using TSS::SkinColourState;
        using TSS::SkinObjectType;

        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto remappedCaret = juce::Colour(ColourChart::kRed500);
        expect(remappedCaret != juce::Colour(ColourChart::kWhite));

        skin->setColour(TSS::skinColourKey(SkinObjectType::kNumberBox,
                                           SkinColourProperty::kCaret),
                        remappedCaret);

        const auto look = TSS::numberBoxLookFromSkin(*skin);
        expect(look.editorCaret == remappedCaret);
        expect(look.editorText == juce::Colour(ColourChart::kWhite));
        expect(skin->getColour(SkinObjectType::kNumberBox,
                               SkinColourProperty::kEditorText,
                               SkinColourState::kDefault)
               == juce::Colour(ColourChart::kWhite));
    }

    void assertRemapButtonBordersLeavesNumberBox()
    {
        using TSS::SkinColourProperty;
        using TSS::SkinColourState;
        using TSS::SkinObjectType;

        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto before = TSS::numberBoxLookFromSkin(*skin);
        const auto remapped = juce::Colour(ColourChart::kRed500);
        expect(before.border != remapped && before.borderDisabled != remapped);

        skin->setColour(TSS::skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff),
                        remapped);
        skin->setColour(TSS::skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn),
                        remapped);

        const auto after = TSS::numberBoxLookFromSkin(*skin);
        expect(after.border == before.border);
        expect(after.borderDisabled == before.borderDisabled);
        expect(TSS::buttonLookFromSkin(*skin).borderOn == remapped);

        skin->setColour(TSS::skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBorder), remapped);
        expect(TSS::numberBoxLookFromSkin(*skin).border == remapped);
    }
};

static NumberBoxDisabledBackgroundColourTests numberBoxDisabledBackgroundColourTests;
