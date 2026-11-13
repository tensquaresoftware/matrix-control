#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"
#include "GUI/Skins/SkinColoursWidgetsDisplays.h"
#include "GUI/Skins/SkinColoursWidgetsSelection.h"

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
        beginTest("blackDuo_zoneAccentsFollowBlueOrangeYellow");
        assertBlackChromaZoneAccents();

        beginTest("black_masterHeaderSharePatchEmerald");
        assertBlackMasterHeaderSharePatchEmerald();

        beginTest("black_modalesMonoGreys");
        assertBlackModalesMonoSeeds();

        beginTest("modales_monoGreysOnBlackChromaAndCafeOnCream");
        assertModalesMonoSeeds();

        beginTest("cream_patchMasterHeaderShareGreens");
        assertCreamSharedGreens();

        beginTest("skinLoaded_patchSliderMatchesBlueStops");
        assertPatchSliderFromSkin();

        beginTest("sliderLookForZone_modalesMapsZoneSeeds");
        assertModalesLook(TSS::Skin::ColourVariant::Black);
        assertModalesLook(TSS::Skin::ColourVariant::BlackChroma);
        assertModalesLook(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertBlackChromaZoneAccents()
    {
        using namespace TSS::SkinColours::Widgets;

        expect(Slider::kTrack.blackChromaVariant == ColourChart::kBlue950);
        expect(Slider::kValueBar.blackChromaVariant == ColourChart::kBlue650);
        expect(Slider::kText.blackChromaVariant == ColourChart::kBlue450);
        expect(SliderK2::kTrack.blackChromaVariant == ColourChart::kOrange950);
        expect(SliderK2::kValueBar.blackChromaVariant == ColourChart::kOrange650);
        expect(SliderK2::kText.blackChromaVariant == ColourChart::kOrange350);
        expect(SliderK3::kTrack.blackChromaVariant == ColourChart::kYellow950);
        expect(SliderK3::kValueBar.blackChromaVariant == ColourChart::kYellow650);
        expect(SliderK3::kText.blackChromaVariant == ColourChart::kYellow300);
    }

    void assertBlackMasterHeaderSharePatchEmerald()
    {
        using namespace TSS::SkinColours::Widgets;

        expect(SliderK2::kTrack.blackVariant == Slider::kTrack.blackVariant);
        expect(SliderK2::kValueBar.blackVariant == Slider::kValueBar.blackVariant);
        expect(SliderK2::kText.blackVariant == Slider::kText.blackVariant);
        expect(SliderK2::kFocusBorder.blackVariant == Slider::kFocusBorder.blackVariant);
        expect(SliderK3::kTrack.blackVariant == Slider::kTrack.blackVariant);
        expect(SliderK3::kValueBar.blackVariant == Slider::kValueBar.blackVariant);
        expect(SliderK3::kText.blackVariant == Slider::kText.blackVariant);
        expect(SliderK3::kFocusBorder.blackVariant == Slider::kFocusBorder.blackVariant);

        expect(ComboBoxK2::kBackground.blackVariant == ComboBox::kBackground.blackVariant);
        expect(ComboBoxK2::kTriangle.blackVariant == ComboBox::kTriangle.blackVariant);
        expect(ComboBoxK2::kText.blackVariant == ComboBox::kText.blackVariant);
        expect(ComboBoxK2::kFocusBorder.blackVariant == ComboBox::kFocusBorder.blackVariant);
        expect(PopupMenuK2::kBackground.blackVariant == PopupMenu::kBackground.blackVariant);
        expect(PopupMenuK2::kText.blackVariant == PopupMenu::kText.blackVariant);
        expect(PopupMenuK2::kBorder.blackVariant == PopupMenu::kBorder.blackVariant);
        expect(PopupMenuK2::kBackgroundHover.blackVariant == PopupMenu::kBackgroundHover.blackVariant);
        expect(PopupMenuK2::kTextHover.blackVariant == PopupMenu::kTextHover.blackVariant);
        expect(PopupMenuK2::kTextDisabled.blackVariant == PopupMenu::kTextDisabled.blackVariant);
    }

    void assertBlackModalesMonoSeeds()
    {
        using namespace TSS::SkinColours::Widgets;

        expect(SliderK4::kTrack.blackVariant == ColourChart::kBlack);
        expect(SliderK4::kFocusBorder.blackVariant == ColourChart::kGrey800);
        expect(SliderK4::kValueBar.blackVariant == ColourChart::kGrey600);
        expect(SliderK4::kText.blackVariant == ColourChart::kGrey200);
        expect(LedK2::kMaxOn.blackVariant == ColourChart::kGrey200);
        expect(PeakIndicatorK2::kMaxOn.blackVariant == ColourChart::kGrey200);
    }

    void assertModalesMonoSeeds()
    {
        using namespace TSS::SkinColours::Widgets;

        expect(SliderK4::kTrack.blackChromaVariant == ColourChart::kBlack);
        expect(SliderK4::kFocusBorder.blackChromaVariant == ColourChart::kGrey800);
        expect(SliderK4::kValueBar.blackChromaVariant == ColourChart::kGrey600);
        expect(SliderK4::kText.blackChromaVariant == ColourChart::kGrey200);
        expect(SliderK4::kTrack.creamVariant == ColourChart::kBrandCream);
        expect(SliderK4::kValueBar.creamVariant == ColourChart::kCafe700);
        expect(SliderK4::kText.creamVariant == ColourChart::kCafe700);
    }

    void assertCreamSharedGreens()
    {
        using namespace TSS::SkinColours::Widgets;

        expect(Slider::kTrack.creamVariant == ColourChart::kGreen600);
        expect(SliderK2::kTrack.creamVariant == Slider::kTrack.creamVariant);
        expect(SliderK3::kTrack.creamVariant == Slider::kTrack.creamVariant);
        // Value bar must stay brighter than track (Green500 vs Green600) or it vanishes on CREAM/GREEN.
        expect(Slider::kValueBar.creamVariant == ColourChart::kGreen500);
        expect(SliderK2::kValueBar.creamVariant == Slider::kValueBar.creamVariant);
        expect(SliderK3::kValueBar.creamVariant == Slider::kValueBar.creamVariant);
        expect(Slider::kText.creamVariant == ColourChart::kGreen100);
    }

    void assertPatchSliderFromSkin()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        expect(skin->getSliderTrackColour(true) == juce::Colour(ColourChart::kBlue950));
        expect(skin->getSliderValueBarColour(true) == juce::Colour(ColourChart::kBlue650));
        expect(skin->getSliderTextColour(true) == juce::Colour(ColourChart::kBlue450));
    }

    void assertModalesLook(TSS::Skin::ColourVariant variant)
    {
        using namespace TSS::SkinColours::Widgets;

        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::sliderLookForZone(*skin, SkinZone::kModales);
        auto seed = [variant](const TSS::SkinColours::ColourElement& element)
        {
            if (variant == TSS::Skin::ColourVariant::BlackChroma)
                return juce::Colour(element.blackChromaVariant);
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
        expect(look.editorText == seed(SliderK4::kEditorText));
        expect(look.editorCaret == seed(SliderK4::kCaret));
        expect(look.editorText == seed(TSS::SkinColours::Widgets::NumberBox::kEditorText));
        expect(look.editorCaret == seed(TSS::SkinColours::Widgets::NumberBox::kCaret));
    }
};

static SliderZoneSeedTests sliderZoneSeedTests;
