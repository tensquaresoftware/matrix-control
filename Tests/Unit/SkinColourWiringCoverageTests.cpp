#include <utility>
#include <vector>

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

        beginTest("moduleHeaderLook_textHoverUsesModuleHeaderOwner");
        assertModuleHeaderLookTextHoverUsesOwner();

        beginTest("ownerSeeds_matchFormerBorrowSourcesForEveryProductSkin");
        assertOwnerSeedParity(TSS::Skin::ColourVariant::Black);
        assertOwnerSeedParity(TSS::Skin::ColourVariant::BlackDuo);
        assertOwnerSeedParity(TSS::Skin::ColourVariant::Cream);

        beginTest("remapNumberBoxFocusAndCaret_leavesModuleHeaderAndSlider");
        assertRemapNumberBoxLeavesModuleHeaderAndSlider();

        beginTest("remapSliderK4EditorTokens_followsModalesOnly");
        assertRemapSliderK4EditorTokensFollowsModalesOnly();

        beginTest("remapButtonTextOff_leavesDialogTextTokens");
        assertRemapButtonTextOffLeavesDialogTokens();

        beginTest("remapModuleHeaderLineOrange_leavesBadgeAndAboutTokens");
        assertRemapLineOrangeLeavesBadgeAndAboutTokens();

        beginTest("alertHoverAndButtonTextHover_remapIndependently");
        assertAlertHoverIndependentOfButtonTextHover();

        beginTest("sharedChrome_chromeTextRemapStillReachesChromeLook");
        assertSharedChromeTextFollows();

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

    static TSS::SkinColourKey key(SkinObjectType object,
                                  SkinColourProperty property,
                                  SkinColourState state = SkinColourState::kDefault,
                                  ColourSet colourSet = ColourSet::k1)
    {
        return TSS::skinColourKey(object, property, state, colourSet);
    }

    static juce::Colour remapColour() { return juce::Colour(ColourChart::kYellow400); }

    void assertModuleHeaderLookTextHoverUsesOwner()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto look = TSS::moduleHeaderLookFromSkin(*skin);
        expect(look.textHover == skin->getColour(key(SkinObjectType::kModuleHeader,
                                                     SkinColourProperty::kText,
                                                     SkinColourState::kHover)));
        expect(look.textHover == juce::Colour(ColourChart::kRed500));
    }

    void assertOwnerSeedParity(TSS::Skin::ColourVariant variant)
    {
        using O = SkinObjectType;
        using P = SkinColourProperty;
        using S = SkinColourState;
        const std::vector<std::pair<TSS::SkinColourKey, TSS::SkinColourKey>> ownerToFormerSource {
            { key(O::kModuleHeader, P::kText, S::kHover), key(O::kNumberBox, P::kText, S::kFocus) },
            { key(O::kModuleHeader, P::kBackground, S::kEmphasis), key(O::kModuleHeader, P::kLineOrange) },
            { key(O::kModuleHeader, P::kText, S::kEmphasis), key(O::kBodyPanel, P::kBackground) },
            { key(O::kNumberBox, P::kBorder), key(O::kButton, P::kBorder, S::kOn) },
            { key(O::kNumberBox, P::kBorder, S::kDisabled), key(O::kButton, P::kBorder, S::kOff) },
            { key(O::kButton, P::kBackground, S::kError), key(O::kFooterMessage, P::kText, S::kError) },
            { key(O::kButton, P::kText, S::kError), key(O::kFooterMessage, P::kBackground, S::kError) },
            { key(O::kButton, P::kSecondaryText, S::kHover), key(O::kChromeText, P::kText, S::kHover) },
            { key(O::kDialog, P::kText), key(O::kButton, P::kText, S::kOff) },
            { key(O::kDialog, P::kText, S::kOn), key(O::kButton, P::kText, S::kOff) },
            { key(O::kChromeText, P::kText, S::kEmphasis), key(O::kModuleHeader, P::kLineOrange) },
            { key(O::kFooterMessage, P::kGlyph, S::kHover), key(O::kButton, P::kText, S::kHover) },
            { key(O::kLabel, P::kBackground, S::kClicked), key(O::kSectionHeader, P::kLineBlue) },
            { key(O::kLabel, P::kBackground, S::kHover), key(O::kFooterMessage, P::kText, S::kError) },
            { key(O::kProgressBar, P::kSecondaryText), key(O::kChromeText, P::kText) },
            { key(O::kProgressBar, P::kSecondaryText, S::kDisabled), key(O::kProgressBar, P::kText, S::kDisabled) },
        };

        const auto skin = TSS::Skin::create(variant);
        for (const auto& [owner, formerSource] : ownerToFormerSource)
            expect(skin->getColour(owner) == skin->getColour(formerSource));

        for (const auto colourSet : TSS::kAllColourSets)
        {
            expect(skin->getColour(key(O::kSlider, P::kEditorText, S::kDefault, colourSet))
                   == skin->getColour(key(O::kNumberBox, P::kEditorText)));
            expect(skin->getColour(key(O::kSlider, P::kCaret, S::kDefault, colourSet))
                   == skin->getColour(key(O::kNumberBox, P::kCaret)));
        }
    }

    void assertRemapNumberBoxLeavesModuleHeaderAndSlider()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto headerBefore = TSS::moduleHeaderLookFromSkin(*skin);
        const auto sliderBefore = TSS::sliderLookForZone(*skin, SkinZone::kPatch);

        skin->setColour(key(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus),
                        remapColour());
        skin->setColour(key(SkinObjectType::kNumberBox, SkinColourProperty::kCaret), remapColour());
        skin->setColour(key(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText), remapColour());

        expect(TSS::numberBoxLookFromSkin(*skin).textFocus == remapColour());
        expect(TSS::numberBoxLookFromSkin(*skin).editorCaret == remapColour());
        expect(TSS::numberBoxLookFromSkin(*skin).editorText == remapColour());
        expect(TSS::moduleHeaderLookFromSkin(*skin).textHover == headerBefore.textHover);
        for (const auto zone : { SkinZone::kPatch, SkinZone::kMaster, SkinZone::kHeader, SkinZone::kModales })
        {
            const auto slider = TSS::sliderLookForZone(*skin, zone);
            expect(slider.editorCaret == sliderBefore.editorCaret);
            expect(slider.editorText == sliderBefore.editorText);
        }
    }

    void assertRemapSliderK4EditorTokensFollowsModalesOnly()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto patchBefore = TSS::sliderLookForZone(*skin, SkinZone::kPatch);
        const auto caretK4 = juce::Colour(ColourChart::kOrange400);

        skin->setColour(key(SkinObjectType::kSlider, SkinColourProperty::kCaret, SkinColourState::kDefault,
                            ColourSet::k4),
                        caretK4);
        skin->setColour(key(SkinObjectType::kSlider, SkinColourProperty::kEditorText, SkinColourState::kDefault,
                            ColourSet::k4),
                        remapColour());

        const auto modales = TSS::sliderLookForZone(*skin, SkinZone::kModales);
        const auto patch = TSS::sliderLookForZone(*skin, SkinZone::kPatch);
        expect(modales.editorCaret == caretK4);
        expect(modales.editorText == remapColour());
        expect(patch.editorCaret == patchBefore.editorCaret);
        expect(patch.editorText == patchBefore.editorText);
        expect(TSS::numberBoxLookFromSkin(*skin).editorCaret != caretK4);
    }

    void assertRemapButtonTextOffLeavesDialogTokens()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto dialogText = skin->getColour(key(SkinObjectType::kDialog, SkinColourProperty::kText));
        const auto selectedTab = skin->getColour(key(SkinObjectType::kDialog,
                                                     SkinColourProperty::kText,
                                                     SkinColourState::kOn));

        skin->setColour(key(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOff),
                        remapColour());

        expect(TSS::buttonLookFromSkin(*skin).textOff == remapColour());
        expect(skin->getColour(key(SkinObjectType::kDialog, SkinColourProperty::kText)) == dialogText);
        expect(skin->getColour(key(SkinObjectType::kDialog, SkinColourProperty::kText, SkinColourState::kOn))
               == selectedTab);
    }

    void assertRemapLineOrangeLeavesBadgeAndAboutTokens()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto aboutHoverKey = key(SkinObjectType::kChromeText, SkinColourProperty::kText,
                                       SkinColourState::kEmphasis);

        skin->setColour(key(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange), remapColour());

        const auto look = TSS::moduleHeaderLookFromSkin(*skin);
        expect(look.lineOrange == remapColour());
        expect(look.backgroundEmphasis == juce::Colour(ColourChart::kOrange));
        expect(look.textEmphasis == juce::Colour(ColourChart::kCream));
        expect(skin->getColour(aboutHoverKey) == juce::Colour(ColourChart::kOrange));
    }

    void assertAlertHoverIndependentOfButtonTextHover()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        expect(TSS::buttonAlertLookFromSkin(*skin).textHover == juce::Colour(ColourChart::kCafe900));
        expect(TSS::buttonLookFromSkin(*skin).textHover == juce::Colour(ColourChart::kBlack));

        skin->setColour(key(SkinObjectType::kButton, SkinColourProperty::kSecondaryText, SkinColourState::kHover),
                        remapColour());
        expect(TSS::buttonAlertLookFromSkin(*skin).textHover == remapColour());
        expect(TSS::buttonAlertLookFromSkin(*skin).textClicked == remapColour());
        expect(TSS::buttonLookFromSkin(*skin).textHover == juce::Colour(ColourChart::kBlack));

        const auto buttonHover = juce::Colour(ColourChart::kOrange400);
        skin->setColour(key(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kHover),
                        buttonHover);
        expect(TSS::buttonLookFromSkin(*skin).textHover == buttonHover);
        expect(TSS::buttonAlertLookFromSkin(*skin).textHover == remapColour());
    }

    void assertSharedChromeTextFollows()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        skin->setColour(key(SkinObjectType::kChromeText, SkinColourProperty::kText), remapColour());
        expect(TSS::chromeTextLabelLookFromSkin(*skin).text == remapColour());
        expect(skin->getColour(key(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText))
               != remapColour());
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
