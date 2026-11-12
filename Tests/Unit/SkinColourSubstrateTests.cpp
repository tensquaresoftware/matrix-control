#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursPanels.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/Logo.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/ReadOnlyValueField.h"

using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;

class SkinColourSubstrateTests : public juce::UnitTest
{
public:
    SkinColourSubstrateTests()
        : juce::UnitTest("SkinColourSubstrate")
    {
    }

    void runTest() override
    {
        beginTest("lookBuilders_resolveStructuredKeysForBothSkins");

        assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant::BlackChroma);
        assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant::Cream);

        beginTest("popupMenuAndAlertLooks_resolveDisabledAndAlertFromSkin");

        assertPopupMenuDisabledAndAlertLooks(TSS::Skin::ColourVariant::BlackChroma);
        assertPopupMenuDisabledAndAlertLooks(TSS::Skin::ColourVariant::Cream);

        beginTest("skinMaps_blackAndCreamColumnsDiverge");

        assertBodyBackgroundColumns();
        assertCreamDivergentLabelText();
        assertChromeTextColumns();

        beginTest("patchNameDisplay_secondaryTextIsSolidMappedColour");

        assertSecondarySolid(TSS::Skin::ColourVariant::BlackChroma);
        assertSecondarySolid(TSS::Skin::ColourVariant::Cream);

        beginTest("gapKeys_existForBlackAndCream");

        assertGapKeys(TSS::Skin::ColourVariant::BlackChroma);
        assertGapKeys(TSS::Skin::ColourVariant::Cream);

        beginTest("ledAndReadOnlyLooks_resolveFromSkinMaps");

        assertLedAndReadOnlyLooks(TSS::Skin::ColourVariant::BlackChroma);
        assertLedAndReadOnlyLooks(TSS::Skin::ColourVariant::Cream);

        beginTest("colourSet_missingHigherSetFallsBackToK1");

        assertMissingSetFallback();

        beginTest("colourSet_explicitHigherSetWinsOverK1");

        assertExplicitHigherSetWins();

        beginTest("ledAndPeakIndicator_useDistinctKeysWithMatchingSeedColours");

        assertLedAndPeakKeys();

        beginTest("ledAndPeak_paintLevelEncoding");

        assertLedAndPeakPaintLevelEncoding();

        beginTest("logo_hoverUsesChromeTextHover");

        assertLogoHoverUsesChromeTextHover();

        beginTest("readOnlyValueField_defaultOverrideAndSetSkinReset");

        assertReadOnlyValueFieldDefaultOverrideAndSetSkinReset();

        beginTest("footerMessage_severityKeysMatchChartForBothSkins");

        assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant::BlackChroma);
        assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant::Cream);

        beginTest("mainChrome_headerFooterBorderAndAlertKeys");

        assertMainChromeKeys(TSS::Skin::ColourVariant::BlackChroma);
        assertMainChromeKeys(TSS::Skin::ColourVariant::Cream);
    }

private:
    static juce::Colour paintPixel(juce::Component& component, int x, int y)
    {
        juce::Image image(juce::Image::ARGB, component.getWidth(), component.getHeight(), true);
        {
            juce::Graphics g(image);
            component.paint(g);
        }
        return image.getPixelAt(x, y);
    }

    static juce::Colour seedFor(TSS::Skin::ColourVariant variant, juce::uint32 blackDuo, juce::uint32 cream)
    {
        // Duo-era pins: BlackChroma column vs Cream. Classic BLACK has its own seeds.
        return juce::Colour(variant == TSS::Skin::ColourVariant::BlackChroma ? blackDuo : cream);
    }

    static juce::Colour compositeOver(juce::Colour under, juce::Colour over)
    {
        juce::Image image(juce::Image::ARGB, 1, 1, true);
        {
            juce::Graphics g(image);
            g.setColour(under);
            g.fillAll();
            g.setColour(over);
            g.fillAll();
        }
        return image.getPixelAt(0, 0);
    }

    void assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::buttonLookFromSkin(*skin);
        const auto mapped = skin->getColour(SkinObjectType::kButton,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kOff);
        expect(look.backgroundOff == mapped);
        expect(look.backgroundOff == seedFor(variant, ColourChart::kBlack, ColourChart::kCafe50));
    }

    void assertPopupMenuDisabledAndAlertLooks(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);

        const auto popupLook = TSS::popupMenuLookFromSkin(*skin);
        expect(popupLook.textDisabled
               == skin->getColour(SkinObjectType::kPopupMenu,
                                  SkinColourProperty::kText,
                                  SkinColourState::kDisabled));
        expect(popupLook.textDisabled
               == seedFor(variant, ColourChart::kBlue600, ColourChart::kGreen400));

        const auto popupLookMono = TSS::popupMenuLookForZone(*skin, TSS::kMonoMenuExceptionZone);
        expect(popupLookMono.textDisabled
               == skin->getColour(SkinObjectType::kPopupMenu,
                                  SkinColourProperty::kText,
                                  SkinColourState::kDisabled,
                                  TSS::ColourSet::k3));
        expect(popupLookMono.textDisabled
               == seedFor(variant, ColourChart::kGrey400, ColourChart::kCafe300));

        const auto alertLook = TSS::buttonAlertLookFromSkin(*skin);
        const auto alertRed = skin->getColour(SkinObjectType::kButton,
                                              SkinColourProperty::kBackground,
                                              SkinColourState::kError);
        const auto alertBlack = skin->getColour(SkinObjectType::kButton,
                                                SkinColourProperty::kText,
                                                SkinColourState::kError);
        const auto alertWhite = skin->getColour(SkinObjectType::kButton,
                                                SkinColourProperty::kSecondaryText,
                                                SkinColourState::kHover);
        expect(alertLook.backgroundOff == alertRed);
        expect(alertLook.backgroundOn == alertRed);
        expect(alertLook.backgroundHover == alertRed);
        expect(alertLook.backgroundClicked == alertRed);
        expect(alertLook.textOff == alertBlack);
        expect(alertLook.textOn == alertBlack);
        expect(alertLook.textHover == alertWhite && alertLook.textClicked == alertWhite);
        expect(alertWhite == seedFor(variant, ColourChart::kWhite, ColourChart::kCafe750));
    }

    void assertBodyBackgroundColumns()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        expect(black->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey900));
        expect(cream->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kBrandCream));
    }

    void assertCreamDivergentLabelText()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto blackText = black->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        const auto creamText = cream->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        expect(blackText == juce::Colour(ColourChart::kGrey200));
        expect(creamText == juce::Colour(ColourChart::kCafe850));
        expect(blackText != creamText);
    }

    void assertChromeTextColumns()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto blackText = black->getColour(SkinObjectType::kChromeText, SkinColourProperty::kText);
        const auto creamText = cream->getColour(SkinObjectType::kChromeText, SkinColourProperty::kText);
        expect(blackText == juce::Colour(ColourChart::kGrey200));
        expect(creamText == juce::Colour(ColourChart::kCafe700));
        expect(creamText != cream->getColour(SkinObjectType::kLabel, SkinColourProperty::kText));
    }

    void assertSecondarySolid(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto look = TSS::patchNameDisplayLookFromSkin(*skin);
        const auto mapped = skin->getColour(SkinObjectType::kPatchNameDisplay,
                                            SkinColourProperty::kSecondaryText);
        expect(look.secondaryText == mapped);
        expect(look.secondaryText == juce::Colour(ColourChart::kRed800));
        expect(look.secondaryText.getFloatAlpha() == 1.0f);
    }

    void assertGapKeys(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        auto expectSeed = [&](SkinObjectType object,
                              SkinColourProperty property,
                              SkinColourState state,
                              juce::uint32 black,
                              juce::uint32 cream)
        {
            expect(skin->getColour(object, property, state) == seedFor(variant, black, cream));
        };

        expectSeed(SkinObjectType::kDialog, SkinColourProperty::kBorder, SkinColourState::kDefault,
                   ColourChart::kGrey550, ColourChart::kCafe400);
        expectSeed(SkinObjectType::kDialog, SkinColourProperty::kTitleBand, SkinColourState::kDefault,
                   ColourChart::kBlack, ColourChart::kCafe50);
        expectSeed(SkinObjectType::kDialog, SkinColourProperty::kGlyph, SkinColourState::kDefault,
                   ColourChart::kRed900, ColourChart::kRed900);
        expectSeed(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground, SkinColourState::kDefault,
                   ColourChart::kGrey700, ColourChart::kCafe50);
        expectSeed(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText, SkinColourState::kDefault,
                   ColourChart::kGrey200, ColourChart::kCafe850);
        expectSeed(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText, SkinColourState::kError,
                   ColourChart::kRed500, ColourChart::kCafe850);
        expectSeed(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOff,
                   ColourChart::kBlack, ColourChart::kBrandCream);
        expectSeed(SkinObjectType::kLed, SkinColourProperty::kBorder, SkinColourState::kOff,
                   ColourChart::kGrey800, ColourChart::kCafe500);
        expectSeed(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn,
                   ColourChart::kYellow400, ColourChart::kGreen500);
        expectSeed(SkinObjectType::kChromeText, SkinColourProperty::kText, SkinColourState::kHover,
                   ColourChart::kWhite, ColourChart::kCafe750);
        expectSeed(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange, SkinColourState::kDefault,
                   ColourChart::kBrandOrange, ColourChart::kBrandOrange);
        expectSeed(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret, SkinColourState::kDefault,
                   ColourChart::kBlack, ColourChart::kCafe850);
    }

    void assertLedAndReadOnlyLooks(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto ledLook = TSS::ledLookFromSkin(*skin);
        expect(ledLook.backgroundOff
               == skin->getColour(SkinObjectType::kLed,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kOff));
        expect(ledLook.borderOff
               == skin->getColour(SkinObjectType::kLed,
                                  SkinColourProperty::kBorder,
                                  SkinColourState::kOff));
        expect(ledLook.maxOn
               == skin->getColour(SkinObjectType::kLed,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kOn));
        expect(ledLook.maxOn == seedFor(variant, ColourChart::kYellow400, ColourChart::kGreen500));
        expect(ledLook.maxOn.getFloatAlpha() == 1.0f);

        const auto fieldLook = TSS::readOnlyValueFieldLookFromSkin(*skin);
        expect(fieldLook.background
               == skin->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground));
        expect(fieldLook.text
               == skin->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText));

        const auto patchLook = TSS::patchNameDisplayLookFromSkin(*skin);
        expect(patchLook.caretInk
               == skin->getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret));
        expect(patchLook.secondaryText.getFloatAlpha() == 1.0f);
    }

    void assertMissingSetFallback()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        auto key = TSS::skinColourKey(SkinObjectType::kButton,
                                      SkinColourProperty::kBackground,
                                      SkinColourState::kOff);
        const auto defaultColour = skin->getColour(key);
        skin->clearHigherColourSets();
        key.colourSet = TSS::ColourSet::k3;
        expect(skin->getColour(key) == defaultColour);
    }

    void assertExplicitHigherSetWins()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        auto key = TSS::skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack);
        const auto primaryColour = juce::Colour(ColourChart::kRed500);
        const auto setTwoColour = juce::Colour(ColourChart::kBrandBlue);
        skin->setColour(key, primaryColour);
        key.colourSet = TSS::ColourSet::k2;
        skin->setColour(key, setTwoColour);
        expect(skin->getColour(key) == setTwoColour && skin->getColour(key) != primaryColour);
    }

    void assertLedAndPeakKeys()
    {
        for (const auto variant : { TSS::Skin::ColourVariant::BlackChroma,
                                    TSS::Skin::ColourVariant::Cream })
        {
            const auto skin = TSS::Skin::create(variant);
            const auto led = TSS::ledLookFromSkin(*skin);
            const auto peak = TSS::peakIndicatorLookFromSkin(*skin);
            expect(led.backgroundOff == peak.backgroundOff
                   && led.borderOff == peak.borderOff
                   && led.maxOn == peak.maxOn);
            expect(! (TSS::skinColourKey(SkinObjectType::kLed,
                                         SkinColourProperty::kBackground,
                                         SkinColourState::kOn)
                      == TSS::skinColourKey(SkinObjectType::kPeakIndicator,
                                            SkinColourProperty::kBackground,
                                            SkinColourState::kOn)));
        }
    }

    static void setDistinctLedAndPeakColours(TSS::Skin& skin)
    {
        using O = SkinObjectType;
        using P = SkinColourProperty;
        using S = SkinColourState;
        skin.setColour(TSS::skinColourKey(O::kLed, P::kBackground, S::kOn),
                       juce::Colour(ColourChart::kRed500));
        skin.setColour(TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOn),
                       juce::Colour(ColourChart::kBrandBlue));
        skin.setColour(TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOff),
                       juce::Colour(ColourChart::kGrey700));
    }

    void assertLedAndPeakPaintLevelEncoding()
    {
        constexpr float kPartialLevel = 0.5f;
        constexpr int kSize = 20;
        constexpr int kCentre = kSize / 2;

        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        setDistinctLedAndPeakColours(*skin);
        const auto ledMaxOn = skin->getColour(SkinObjectType::kLed,
                                              SkinColourProperty::kBackground,
                                              SkinColourState::kOn);
        const auto ledBackgroundOff = skin->getColour(SkinObjectType::kLed,
                                                      SkinColourProperty::kBackground,
                                                      SkinColourState::kOff);
        const auto peakMaxOn = skin->getColour(SkinObjectType::kPeakIndicator,
                                               SkinColourProperty::kBackground,
                                               SkinColourState::kOn);
        const auto peakBackgroundOff = skin->getColour(SkinObjectType::kPeakIndicator,
                                                       SkinColourProperty::kBackground,
                                                       SkinColourState::kOff);
        expect(ledMaxOn != peakMaxOn);
        expect(ledMaxOn.getFloatAlpha() == 1.0f);

        TSS::Led led(kSize, kSize);
        led.setSkin(*skin);
        led.displayedLevel_ = kPartialLevel;
        const auto ledPixel = paintPixel(led, kCentre, kCentre);
        // Paint path: maxOn RGB with reduced alpha over the off plate (not a Colour-only check).
        expect(ledPixel == compositeOver(ledBackgroundOff, ledMaxOn.withAlpha(kPartialLevel)));

        TSS::PeakIndicator peak(kSize, kSize);
        peak.setSkin(*skin);
        peak.setLevel(kPartialLevel);
        // Height fill: lit band is the lower half — sample near bottom vs near top (inside border).
        const auto peakLit = paintPixel(peak, kCentre, kSize - 4);
        const auto peakOff = paintPixel(peak, kCentre, 4);
        expect(peakLit == peakMaxOn);
        expect(peakLit.getFloatAlpha() == 1.0f);
        expect(peakOff == peakBackgroundOff);
    }

    void assertLogoHoverUsesChromeTextHover()
    {
        struct LogoProbe : TSS::Logo
        {
            using TSS::Logo::Logo;
            juce::Colour textColour() const noexcept { return look_.text; }
        };

        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        const auto hover = skin->getColour(SkinObjectType::kChromeText,
                                           SkinColourProperty::kText,
                                           SkinColourState::kHover);
        LogoProbe logo(*skin, 80, 24);
        logo.setHighlighted(true);
        expect(logo.textColour() == hover);
        expect(logo.textColour() == juce::Colour(ColourChart::kWhite));
    }

    void assertReadOnlyValueFieldDefaultOverrideAndSetSkinReset()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackChroma);
        const auto mappedFill = skin->getColour(SkinObjectType::kReadOnlyValueField,
                                                SkinColourProperty::kBackground);
        const auto mappedText = skin->getColour(SkinObjectType::kReadOnlyValueField,
                                                SkinColourProperty::kText);
        TSS::ReadOnlyValueField field(*skin);
        field.setBounds(0, 0, 40, 20);
        // Non-empty text so paint exercises textColour_ (fill sampling stays in left pad).
        field.setText("Ab");

        expect(paintPixel(field, 5, 10) == mappedFill);
        expect(paintPixel(field, 5, 10) == juce::Colour(ColourChart::kGrey700));
        expect(field.textColour_ == mappedText);
        expect(field.textColour_ == juce::Colour(ColourChart::kGrey200));

        field.setColours(juce::Colour(ColourChart::kRed500), juce::Colour(ColourChart::kWhite));
        expect(paintPixel(field, 5, 10) == juce::Colour(ColourChart::kRed500));
        expect(field.textColour_ == juce::Colour(ColourChart::kWhite));

        field.setSkin(*skin);
        expect(paintPixel(field, 5, 10) == mappedFill);
        expect(field.textColour_ == mappedText);
        expect(field.textColour_ == juce::Colour(ColourChart::kGrey200));
    }

    void assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kInfo)
               == seedFor(variant, ColourChart::kGrey500, ColourChart::kCafe700));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kWarning)
               == juce::Colour(ColourChart::kOrange500));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kError)
               == juce::Colour(ColourChart::kRed500));
    }

    void assertMainChromeKeys(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto headerBorder = skin->getColour(SkinObjectType::kHeaderPanel, SkinColourProperty::kBorder);
        const auto footerBorder = skin->getColour(SkinObjectType::kFooterPanel, SkinColourProperty::kBorder);
        expect(headerBorder == seedFor(variant, TSS::SkinColours::Panels::kMainChromeGrey, ColourChart::kCafe700));
        expect(footerBorder == seedFor(variant, TSS::SkinColours::Panels::kMainChromeGrey, ColourChart::kCafe750));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kBackground,
                               SkinColourState::kError)
               == juce::Colour(ColourChart::kBlack));
        expect(TSS::moduleHeaderLookFromSkin(*skin).lineOrange
               == skin->getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange));
    }

};

static SkinColourSubstrateTests skinColourSubstrateTests;
