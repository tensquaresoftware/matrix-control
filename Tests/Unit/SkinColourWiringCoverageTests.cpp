#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Widgets/ReadOnlyValueField.h"
#include "SkinColourWiringTestSupport.h"

using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;
using SkinColourWiringTestSupport::paintDistinctPastillesForSets;

class SkinColourWiringCoverageTests : public juce::UnitTest
{
public:
    SkinColourWiringCoverageTests()
        : juce::UnitTest("SkinColourWiringCoverage")
    {
    }

    void runTest() override
    {
        beginTest("moduleHeaderLook_textFocusUsesNumberBoxK4");
        assertModuleHeaderLookTextFocusUsesNumberBoxK4();

        beginTest("readOnlyValueField_k4ConstructionAndSetSkin");
        assertReadOnlyValueFieldK4ConstructionAndSetSkin();

        beginTest("settingsAndComputerPatches_sliderComboAndNumberBoxUseK4");
        assertSettingsAndComputerPatchesUseK4();

        beginTest("headerInputGain_sliderUsesK3");
        assertHeaderInputGainSliderUsesK3();

        beginTest("blackCream_k1ToK3Match_andSliderComboPopupK4CarryFormerButtonLike");
        assertBlackCreamSeedLooksAfterUnify(TSS::Skin::ColourVariant::Black);
        assertBlackCreamSeedLooksAfterUnify(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertModuleHeaderLookTextFocusUsesNumberBoxK4()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        paintDistinctPastillesForSets(*skin);

        const auto look = TSS::moduleHeaderLookFromSkin(*skin);
        const auto focusK4 = skin->getColour(SkinObjectType::kNumberBox,
                                             SkinColourProperty::kText,
                                             SkinColourState::kFocus,
                                             TSS::ColourSet::k4);
        const auto focusK1 = skin->getColour(SkinObjectType::kNumberBox,
                                             SkinColourProperty::kText,
                                             SkinColourState::kFocus,
                                             TSS::ColourSet::k1);
        expect(look.textFocus == focusK4);
        expect(look.textFocus != focusK1);
    }

    void assertReadOnlyValueFieldK4ConstructionAndSetSkin()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        TSS::ReadOnlyValueField field(*skinA, TSS::ColourSet::k4);
        expect(field.getColourSet() == TSS::ColourSet::k4);
        expect(field.fill_
               == skinA->getColour(SkinObjectType::kReadOnlyValueField,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kDefault,
                                   TSS::ColourSet::k4));
        expect(field.textColour_
               == skinA->getColour(SkinObjectType::kReadOnlyValueField,
                                   SkinColourProperty::kText,
                                   SkinColourState::kDefault,
                                   TSS::ColourSet::k4));
        expect(field.fill_
               != skinA->getColour(SkinObjectType::kReadOnlyValueField,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kDefault,
                                   TSS::ColourSet::k1));

        field.setSkin(*skinB);
        expect(field.getColourSet() == TSS::ColourSet::k4);
        expect(field.fill_
               == skinB->getColour(SkinObjectType::kReadOnlyValueField,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kDefault,
                                   TSS::ColourSet::k4));
        expect(field.textColour_
               == skinB->getColour(SkinObjectType::kReadOnlyValueField,
                                   SkinColourProperty::kText,
                                   SkinColourState::kDefault,
                                   TSS::ColourSet::k4));
    }

    void assertSettingsAndComputerPatchesUseK4()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        paintDistinctPastillesForSets(*skin);

        const auto latency = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(latency.trackEnabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(latency.trackEnabled
               != skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));

        const auto combo = TSS::comboBoxLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(combo.backgroundEnabled
               == skin->getColour(SkinObjectType::kComboBox,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(combo.backgroundEnabled
               != skin->getColour(SkinObjectType::kComboBox,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));

        const auto numberBox = TSS::numberBoxLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(numberBox.background
               == skin->getColour(SkinObjectType::kNumberBox,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(numberBox.background
               != skin->getColour(SkinObjectType::kNumberBox,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));
    }

    void assertHeaderInputGainSliderUsesK3()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        paintDistinctPastillesForSets(*skin);

        const auto headerGain = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k3);
        expect(headerGain.trackEnabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k3));
        expect(headerGain.trackEnabled
               != skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));
    }

    void assertBlackCreamSeedLooksAfterUnify(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto sliderK1 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k1);
        const auto sliderK2 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k2);
        const auto sliderK3 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto sliderK4 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(sliderK1.trackEnabled == sliderK2.trackEnabled
               && sliderK1.trackEnabled == sliderK3.trackEnabled);
        expect(sliderK1.trackEnabled != sliderK4.trackEnabled);
        expect(sliderK4.trackEnabled == juce::Colour(ColourChart::kBlack));
        expect(sliderK4.valueBarEnabled == juce::Colour(ColourChart::kGrey600));
        expect(sliderK4.textEnabled == juce::Colour(ColourChart::kGrey200));

        const auto comboK1 = TSS::comboBoxLookFromSkin(*skin, TSS::ColourSet::k1);
        const auto comboK4 = TSS::comboBoxLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(comboK1.backgroundEnabled != comboK4.backgroundEnabled);
        expect(comboK4.backgroundEnabled == juce::Colour(ColourChart::kBlack));
        expect(comboK4.textEnabled == juce::Colour(ColourChart::kGrey200));
        expect(comboK4.triangleEnabled == juce::Colour(ColourChart::kGrey600));
        expect(comboK1.triangleEnabled != comboK4.triangleEnabled);
        expect(comboK4.focusBorder == juce::Colour(ColourChart::kGrey800));
        expect(comboK1.focusBorder != comboK4.focusBorder);

        const auto popupK1 = TSS::popupMenuLookFromSkin(*skin, TSS::ColourSet::k1);
        const auto popupK4 = TSS::popupMenuLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(popupK1.background != popupK4.background);
        expect(popupK4.background == juce::Colour(ColourChart::kBlack));
        expect(popupK4.text == juce::Colour(ColourChart::kGrey200));
        expect(popupK4.border == juce::Colour(ColourChart::kGrey600));
        expect(popupK1.border != popupK4.border);
        expect(popupK4.separator == juce::Colour(ColourChart::kGrey600));
        expect(popupK1.separator != popupK4.separator);
        expect(popupK4.backgroundHover == juce::Colour(ColourChart::kGrey200));
        expect(popupK1.backgroundHover != popupK4.backgroundHover);
        expect(popupK4.textHover == juce::Colour(ColourChart::kBlack));
        expect(popupK1.textHover != popupK4.textHover);

        const auto peakK3 = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto peakK4 = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(peakK3.maxOn == peakK4.maxOn
               && peakK3.backgroundOff == peakK4.backgroundOff
               && peakK3.borderOff == peakK4.borderOff);
    }
};

static SkinColourWiringCoverageTests skinColourWiringCoverageTests;
