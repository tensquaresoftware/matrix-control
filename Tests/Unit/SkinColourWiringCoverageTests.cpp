#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Widgets/ReadOnlyValueField.h"
#include "SkinColourWiringTestSupport.h"

using TSS::ColourSet;
using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;
using TSS::SkinZone;
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
        beginTest("zoneMatrix_mcsObjectsFollowFrozenZoneToColourSet");
        assertMcsZoneMatrix();

        beginTest("zoneMatrix_ucsObjectsAlwaysResolveK1");
        assertUcsObjectsResolveK1();

        beginTest("zoneMatrix_monoMenuExceptionResolvesComboAndPopupK3");
        assertMonoMenuException();

        beginTest("moduleHeaderLook_textFocusUsesNumberBoxUcs");
        assertModuleHeaderLookTextFocusUsesNumberBoxUcs();

        beginTest("readOnlyValueField_ucsConstructionAndSetSkin");
        assertReadOnlyValueFieldUcsConstructionAndSetSkin();

        beginTest("modalesAndHeaderLooks_resolveZoneSets");
        assertModalesAndHeaderLooksResolveZoneSets();

        beginTest("blackDuoSeed_zoneAccentsAndModalesMono");
        assertBlackDuoSeedZoneLooks();

        beginTest("creamSeed_sharedGreensAndCafeModales");
        assertCreamSeedZoneLooks();
    }

private:
    void assertMcsZoneMatrix()
    {
        expect(TSS::zoneColourSet(SkinObjectType::kSlider, SkinZone::kPatch) == ColourSet::k1);
        expect(TSS::zoneColourSet(SkinObjectType::kSlider, SkinZone::kMaster) == ColourSet::k2);
        expect(TSS::zoneColourSet(SkinObjectType::kSlider, SkinZone::kHeader) == ColourSet::k3);
        expect(TSS::zoneColourSet(SkinObjectType::kSlider, SkinZone::kModales) == ColourSet::k4);

        for (const auto object : { SkinObjectType::kComboBox, SkinObjectType::kPopupMenu })
        {
            expect(TSS::zoneColourSet(object, SkinZone::kPatch) == ColourSet::k1);
            expect(TSS::zoneColourSet(object, SkinZone::kMaster) == ColourSet::k2);
            expect(TSS::zoneColourSet(object, SkinZone::kModales) == ColourSet::k3);
        }

        for (const auto object : { SkinObjectType::kLed, SkinObjectType::kPeakIndicator })
        {
            expect(TSS::zoneColourSet(object, SkinZone::kHeader) == ColourSet::k1);
            expect(TSS::zoneColourSet(object, SkinZone::kModales) == ColourSet::k2);
        }
    }

    void assertUcsObjectsResolveK1()
    {
        for (const auto object : { SkinObjectType::kButton,
                                   SkinObjectType::kToggle,
                                   SkinObjectType::kNumberBox,
                                   SkinObjectType::kReadOnlyValueField,
                                   SkinObjectType::kProgressBar })
        {
            for (const auto zone : { SkinZone::kPatch, SkinZone::kMaster, SkinZone::kHeader, SkinZone::kModales })
                expect(TSS::zoneColourSet(object, zone) == ColourSet::k1);
        }
    }

    void assertMonoMenuException()
    {
        expect(TSS::zoneColourSet(SkinObjectType::kComboBox, TSS::kMonoMenuExceptionZone) == ColourSet::k3);
        expect(TSS::zoneColourSet(SkinObjectType::kPopupMenu, TSS::kMonoMenuExceptionZone) == ColourSet::k3);
    }

    void assertModuleHeaderLookTextFocusUsesNumberBoxUcs()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto look = TSS::moduleHeaderLookFromSkin(*skin);
        expect(look.textFocus
               == skin->getColour(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus));
    }

    void assertReadOnlyValueFieldUcsConstructionAndSetSkin()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);

        TSS::ReadOnlyValueField field(*skinA);
        expect(field.getColourSet() == ColourSet::k1);
        expect(field.fill_
               == skinA->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground));
        expect(field.textColour_
               == skinA->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText));

        field.setSkin(*skinB);
        expect(field.fill_
               == skinB->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground));
        expect(field.textColour_
               == skinB->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText));
        expect(field.fill_ == juce::Colour(ColourChart::kCream400));
    }

    void expectLookUsesSet(juce::Colour lookColour, const TSS::Skin& skin, TSS::SkinColourKey key)
    {
        expect(lookColour == skin.getColour(key));
        key.colourSet = ColourSet::k1;
        expect(lookColour != skin.getColour(key));
    }

    void assertModalesAndHeaderLooksResolveZoneSets()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        paintDistinctPastillesForSets(*skin);

        expectLookUsesSet(TSS::sliderLookForZone(*skin, SkinZone::kModales).trackEnabled,
                          *skin,
                          TSS::skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                                             SkinColourState::kDefault, ColourSet::k4));
        expectLookUsesSet(TSS::sliderLookForZone(*skin, SkinZone::kHeader).trackEnabled,
                          *skin,
                          TSS::skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack,
                                             SkinColourState::kDefault, ColourSet::k3));
        expectLookUsesSet(TSS::comboBoxLookForZone(*skin, SkinZone::kModales).backgroundEnabled,
                          *skin,
                          TSS::skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground,
                                             SkinColourState::kDefault, ColourSet::k3));
        expectLookUsesSet(TSS::popupMenuLookForZone(*skin, TSS::kMonoMenuExceptionZone).border,
                          *skin,
                          TSS::skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder,
                                             SkinColourState::kDefault, ColourSet::k3));
    }

    void assertBlackDuoMonoMenus(const TSS::Skin& skin)
    {
        const auto comboPatch = TSS::comboBoxLookForZone(skin, SkinZone::kPatch);
        const auto comboMono = TSS::comboBoxLookForZone(skin, SkinZone::kModales);
        expect(comboPatch.backgroundEnabled != comboMono.backgroundEnabled);
        expect(comboMono.backgroundEnabled == juce::Colour(ColourChart::kBlack));
        expect(comboMono.textEnabled == juce::Colour(ColourChart::kGrey200));
        expect(comboMono.triangleEnabled == juce::Colour(ColourChart::kGrey600));
        expect(comboMono.focusBorder == juce::Colour(ColourChart::kGrey800));

        const auto popupMono = TSS::popupMenuLookForZone(skin, SkinZone::kModales);
        expect(popupMono.background == juce::Colour(ColourChart::kBlack));
        expect(popupMono.text == juce::Colour(ColourChart::kGrey200));
        expect(popupMono.border == juce::Colour(ColourChart::kGrey600));
        expect(popupMono.separator == juce::Colour(ColourChart::kGrey600));
        expect(popupMono.backgroundHover == juce::Colour(ColourChart::kGrey200));
        expect(popupMono.textHover == juce::Colour(ColourChart::kBlack));
    }

    void assertBlackDuoSeedZoneLooks()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto patch = TSS::sliderLookForZone(*skin, SkinZone::kPatch);
        const auto master = TSS::sliderLookForZone(*skin, SkinZone::kMaster);
        const auto header = TSS::sliderLookForZone(*skin, SkinZone::kHeader);
        const auto modales = TSS::sliderLookForZone(*skin, SkinZone::kModales);
        expect(patch.textEnabled == juce::Colour(ColourChart::kBlue400));
        expect(master.textEnabled == juce::Colour(ColourChart::kOrange400));
        expect(header.textEnabled == juce::Colour(ColourChart::kYellow400));
        expect(modales.trackEnabled == juce::Colour(ColourChart::kBlack));
        expect(modales.valueBarEnabled == juce::Colour(ColourChart::kGrey600));
        expect(modales.textEnabled == juce::Colour(ColourChart::kGrey200));

        assertBlackDuoMonoMenus(*skin);
    }

    void assertCreamSeedZoneLooks()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto patch = TSS::sliderLookForZone(*skin, SkinZone::kPatch);
        const auto master = TSS::sliderLookForZone(*skin, SkinZone::kMaster);
        const auto header = TSS::sliderLookForZone(*skin, SkinZone::kHeader);
        const auto modales = TSS::sliderLookForZone(*skin, SkinZone::kModales);
        expect(patch.trackEnabled == juce::Colour(ColourChart::kGreen600));
        expect(master.trackEnabled == patch.trackEnabled && header.trackEnabled == patch.trackEnabled);
        expect(modales.trackEnabled == juce::Colour(ColourChart::kCream));
        expect(modales.valueBarEnabled == juce::Colour(ColourChart::kCafe800));

        const auto comboMono = TSS::comboBoxLookForZone(*skin, SkinZone::kModales);
        expect(comboMono.backgroundEnabled == juce::Colour(ColourChart::kCream));
        expect(comboMono.textEnabled == juce::Colour(ColourChart::kCafe800));
    }
};

static SkinColourWiringCoverageTests skinColourWiringCoverageTests;
