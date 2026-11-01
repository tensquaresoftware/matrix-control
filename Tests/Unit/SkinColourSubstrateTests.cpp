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

        assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant::Black);
        assertButtonBackgroundOffFromSkin(TSS::Skin::ColourVariant::Cream);

        beginTest("popupMenuAndAlertLooks_resolveDisabledAndAlertFromSkin");

        assertPopupMenuDisabledAndAlertLooks(TSS::Skin::ColourVariant::Black);
        assertPopupMenuDisabledAndAlertLooks(TSS::Skin::ColourVariant::Cream);

        beginTest("skinMaps_blackAndCreamColumnsDiverge");

        assertBodyBackgroundColumns();
        assertCreamDivergentLabelText();
        assertDarkPanelTextInvariant();

        beginTest("patchNameDisplay_secondaryTextIsSolidMappedColour");

        assertSecondarySolid(TSS::Skin::ColourVariant::Black);
        assertSecondarySolid(TSS::Skin::ColourVariant::Cream);

        beginTest("gapKeys_existForBlackAndCream");

        assertGapKeys(TSS::Skin::ColourVariant::Black);
        assertGapKeys(TSS::Skin::ColourVariant::Cream);

        beginTest("ledAndReadOnlyLooks_resolveFromSkinMaps");

        assertLedAndReadOnlyLooks(TSS::Skin::ColourVariant::Black);
        assertLedAndReadOnlyLooks(TSS::Skin::ColourVariant::Cream);

        beginTest("colourSet_missingHigherSetFallsBackToK1");

        assertMissingSetFallback();

        beginTest("colourSet_explicitHigherSetWinsOverK1");

        assertExplicitHigherSetWins();

        beginTest("ledAndPeakIndicator_useDistinctKeysWithMatchingSeedColours");

        assertLedAndPeakKeys();

        beginTest("ledAndPeak_paintLevelEncoding");

        assertLedAndPeakPaintLevelEncoding();

        beginTest("logo_hoverUsesDarkPanelTextHover");

        assertLogoHoverUsesDarkPanelTextHover();

        beginTest("readOnlyValueField_defaultOverrideAndSetSkinReset");

        assertReadOnlyValueFieldDefaultOverrideAndSetSkinReset();

        beginTest("footerMessage_severityKeysMatchChartForBothSkins");

        assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant::Black);
        assertFooterMessageSeverityKeys(TSS::Skin::ColourVariant::Cream);

        beginTest("mainChrome_headerFooterBorderAndAlertKeys");

        assertMainChromeKeys(TSS::Skin::ColourVariant::Black);
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
        expect(look.backgroundOff == juce::Colour(ColourChart::kBlack));
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
               == juce::Colour(ColourChart::kEmerald700));
        expect(popupLook.textDisabledButtonLike
               == skin->getColour(SkinObjectType::kPopupMenuButtonLike,
                                  SkinColourProperty::kText,
                                  SkinColourState::kDisabled));
        // Common::kContentDisabled == ColourChart::kGrey600 (solid muted grey).
        expect(popupLook.textDisabledButtonLike
               == juce::Colour(ColourChart::kGrey600));

        const auto alertLook = TSS::buttonAlertLookFromSkin(*skin);
        const auto alertRed = skin->getColour(SkinObjectType::kFooterMessage,
                                              SkinColourProperty::kText,
                                              SkinColourState::kError);
        const auto alertBlack = skin->getColour(SkinObjectType::kFooterMessage,
                                                SkinColourProperty::kBackground,
                                                SkinColourState::kError);
        const auto alertWhite = skin->getColour(SkinObjectType::kDarkPanel,
                                                SkinColourProperty::kText,
                                                SkinColourState::kHover);
        expect(alertLook.backgroundOff == alertRed);
        expect(alertLook.backgroundOn == alertRed);
        expect(alertLook.backgroundHover == alertRed);
        expect(alertLook.backgroundClicked == alertRed);
        expect(alertLook.textOff == alertBlack);
        expect(alertLook.textOn == alertBlack);
        expect(alertLook.textHover == alertWhite);
        expect(alertLook.textClicked == alertWhite);
    }

    void assertBodyBackgroundColumns()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        expect(black->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey900));
        expect(cream->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kCream));
    }

    void assertCreamDivergentLabelText()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto blackText = black->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        const auto creamText = cream->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);
        expect(blackText == juce::Colour(ColourChart::kGrey200));
        expect(creamText == juce::Colour(ColourChart::kGrey950));
        expect(blackText != creamText);
    }

    void assertDarkPanelTextInvariant()
    {
        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto blackText = black->getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        const auto creamText = cream->getColour(SkinObjectType::kDarkPanel, SkinColourProperty::kText);
        expect(blackText == juce::Colour(ColourChart::kGrey200));
        expect(creamText == juce::Colour(ColourChart::kGrey200));
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

        expect(skin->getColour(SkinObjectType::kDialog, SkinColourProperty::kBorder)
               == juce::Colour(ColourChart::kGrey525));
        expect(skin->getColour(SkinObjectType::kDialog, SkinColourProperty::kTitleBand)
               == juce::Colour(ColourChart::kBlack));
        expect(skin->getColour(SkinObjectType::kWindowClose, SkinColourProperty::kGlyph)
               == juce::Colour(ColourChart::kRed900));
        expect(skin->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey700));
        expect(skin->getColour(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText)
               == juce::Colour(ColourChart::kGrey200));
        expect(skin->getColour(SkinObjectType::kReadOnlyValueField,
                               SkinColourProperty::kText,
                               SkinColourState::kError)
               == juce::Colour(ColourChart::kRed500));
        expect(skin->getColour(SkinObjectType::kLed,
                               SkinColourProperty::kBackground,
                               SkinColourState::kOff)
               == juce::Colour(ColourChart::kBlack));
        expect(skin->getColour(SkinObjectType::kLed,
                               SkinColourProperty::kBorder,
                               SkinColourState::kOff)
               == juce::Colour(ColourChart::kGrey800));
        expect(skin->getColour(SkinObjectType::kLed,
                               SkinColourProperty::kBackground,
                               SkinColourState::kOn)
               == juce::Colour(ColourChart::kEmerald400));
        // About-reused White / Orange pastilles (DarkPanel text Hover; ModuleHeader Orange line).
        expect(skin->getColour(SkinObjectType::kDarkPanel,
                               SkinColourProperty::kText,
                               SkinColourState::kHover)
               == juce::Colour(ColourChart::kWhite));
        expect(skin->getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange)
               == juce::Colour(ColourChart::kOrange));
        expect(skin->getColour(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret)
               == juce::Colour(ColourChart::kBlack));
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
        expect(ledLook.maxOn == juce::Colour(ColourChart::kEmerald400));
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
        const auto defaultColour = skin->getColour(SkinObjectType::kLabel,
                                                   SkinColourProperty::kText);
        const auto fallbackColour = skin->getColour(SkinObjectType::kLabel,
                                                    SkinColourProperty::kText,
                                                    SkinColourState::kDefault,
                                                    TSS::ColourSet::k3);
        expect(fallbackColour == defaultColour);
    }

    void assertExplicitHigherSetWins()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto primaryKey = TSS::skinColourKey(SkinObjectType::kButton,
                                                   SkinColourProperty::kBackground,
                                                   SkinColourState::kOff);
        auto setTwoKey = primaryKey;
        setTwoKey.colourSet = TSS::ColourSet::k2;
        const auto primaryColour = juce::Colour(ColourChart::kRed500);
        const auto setTwoColour = juce::Colour(ColourChart::kBlue);
        skin->setColour(primaryKey, primaryColour);
        skin->setColour(setTwoKey, setTwoColour);

        expect(skin->getColour(setTwoKey) == setTwoColour);
        expect(skin->getColour(setTwoKey) != primaryColour);
    }

    void assertLedAndPeakKeys()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto ledKey = TSS::skinColourKey(SkinObjectType::kLed,
                                               SkinColourProperty::kBackground,
                                               SkinColourState::kOn);
        const auto peakKey = TSS::skinColourKey(SkinObjectType::kPeakIndicator,
                                                SkinColourProperty::kBackground,
                                                SkinColourState::kOn);
        expect(! (ledKey == peakKey));
        expect(skin->getColour(ledKey) == skin->getColour(peakKey));
        expect(TSS::peakIndicatorLookFromSkin(*skin).maxOn == skin->getColour(peakKey));
    }

    static void setDistinctLedAndPeakColours(TSS::Skin& skin)
    {
        skin.setColour(TSS::skinColourKey(SkinObjectType::kLed,
                                          SkinColourProperty::kBackground,
                                          SkinColourState::kOn),
                       juce::Colour(ColourChart::kRed500));
        skin.setColour(TSS::skinColourKey(SkinObjectType::kPeakIndicator,
                                          SkinColourProperty::kBackground,
                                          SkinColourState::kOn),
                       juce::Colour(ColourChart::kBlue));
        skin.setColour(TSS::skinColourKey(SkinObjectType::kPeakIndicator,
                                          SkinColourProperty::kBackground,
                                          SkinColourState::kOff),
                       juce::Colour(ColourChart::kGrey700));
    }

    void assertLedAndPeakPaintLevelEncoding()
    {
        constexpr float kPartialLevel = 0.5f;
        constexpr int kSize = 20;
        constexpr int kCentre = kSize / 2;

        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
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

    void assertLogoHoverUsesDarkPanelTextHover()
    {
        struct LogoProbe : TSS::Logo
        {
            using TSS::Logo::Logo;
            juce::Colour textColour() const noexcept { return look_.text; }
        };

        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto hover = skin->getColour(SkinObjectType::kDarkPanel,
                                           SkinColourProperty::kText,
                                           SkinColourState::kHover);
        LogoProbe logo(*skin, 80, 24);
        logo.setHighlighted(true);
        expect(logo.textColour() == hover);
        expect(logo.textColour() == juce::Colour(ColourChart::kWhite));
    }

    void assertReadOnlyValueFieldDefaultOverrideAndSetSkinReset()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
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
               == juce::Colour(ColourChart::kGrey500));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kWarning)
               == juce::Colour(ColourChart::kOrange));
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kText,
                               SkinColourState::kError)
               == juce::Colour(ColourChart::kRed500));
    }

    void assertMainChromeKeys(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto chromeGrey = juce::Colour(TSS::SkinColours::Panels::kMainChromeGrey);
        const auto headerBorder = skin->getColour(SkinObjectType::kHeaderPanel, SkinColourProperty::kBorder);
        const auto footerBorder = skin->getColour(SkinObjectType::kFooterPanel, SkinColourProperty::kBorder);
        expect(headerBorder == chromeGrey);
        expect(footerBorder == chromeGrey);
        expect(headerBorder == footerBorder);
        expect(skin->getColour(SkinObjectType::kFooterMessage,
                               SkinColourProperty::kBackground,
                               SkinColourState::kError)
               == juce::Colour(ColourChart::kBlack));
        expect(TSS::moduleHeaderLookFromSkin(*skin).lineOrange
               == skin->getColour(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange));
    }
};

static SkinColourSubstrateTests skinColourSubstrateTests;
