#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
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

        beginTest("settingsAndComputerPatches_buttonLikeAndNumberBoxUseK4");
        assertSettingsAndComputerPatchesUseK4();

        beginTest("headerInputGain_sliderUsesK3");
        assertHeaderInputGainSliderUsesK3();

        beginTest("blackCream_identicalSeedKeepsLooksAcrossSets");
        assertBlackCreamIdenticalSeedLooksAcrossSets(TSS::Skin::ColourVariant::Black);
        assertBlackCreamIdenticalSeedLooksAcrossSets(TSS::Skin::ColourVariant::Cream);
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

        const auto latency = TSS::sliderLookButtonLikeFromSkin(*skin, TSS::ColourSet::k4);
        expect(latency.trackEnabled
               == skin->getColour(SkinObjectType::kSliderButtonLike,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(latency.trackEnabled
               != skin->getColour(SkinObjectType::kSliderButtonLike,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));

        const auto combo = TSS::comboBoxLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(combo.buttonLikeBackground
               == skin->getColour(SkinObjectType::kComboBoxButtonLike,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(combo.buttonLikeBackground
               != skin->getColour(SkinObjectType::kComboBoxButtonLike,
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

    void assertBlackCreamIdenticalSeedLooksAcrossSets(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto k1 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k1);
        const auto k2 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k2);
        const auto k3 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto k4 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(k1.trackEnabled == k2.trackEnabled
               && k1.trackEnabled == k3.trackEnabled
               && k1.trackEnabled == k4.trackEnabled);
        expect(k1.valueBarEnabled == k4.valueBarEnabled);
        expect(k1.textEnabled == k4.textEnabled);

        const auto peakK3 = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto peakK4 = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(peakK3.maxOn == peakK4.maxOn
               && peakK3.backgroundOff == peakK4.backgroundOff
               && peakK3.borderOff == peakK4.borderOff);
    }
};

static SkinColourWiringCoverageTests skinColourWiringCoverageTests;
