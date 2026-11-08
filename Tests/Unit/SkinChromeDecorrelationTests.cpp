#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"

class SkinChromeDecorrelationTests : public juce::UnitTest
{
public:
    SkinChromeDecorrelationTests()
        : juce::UnitTest("SkinChromeDecorrelation")
    {
    }

    void runTest() override
    {
        beginTest("windowSurface_seedsFollowTrialPort_keysStayIndependent");
        assertWindowSurfaceDistinctFromPanels();

        beginTest("dialog_ownsCloseGlyph");
        assertDialogOwnsCloseGlyph();

        beginTest("chromeTextAndBrandLooks_resolveFromSkin");
        assertChromeTextAndBrandLooks(TSS::Skin::ColourVariant::BlackDuo);
        assertChromeTextAndBrandLooks(TSS::Skin::ColourVariant::Cream);
    }

private:
    void assertWindowSurfaceDistinctFromPanels()
    {
        using TSS::SkinObjectType;
        using TSS::SkinColourProperty;

        const auto black = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);

        expect(black->getColour(SkinObjectType::kWindowSurface, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey950));
        expect(cream->getColour(SkinObjectType::kWindowSurface, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kCream400));

        expect(black->getColour(SkinObjectType::kHeaderPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey950));
        expect(cream->getColour(SkinObjectType::kHeaderPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kCream400));
        expect(black->getColour(SkinObjectType::kFooterPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey950));
        expect(cream->getColour(SkinObjectType::kFooterPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kCream400));
        expect(black->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey900));
        expect(cream->getColour(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kCream));

        // Seeds may coincide; the keys stay independent.
        const auto windowKey = TSS::skinColourKey(SkinObjectType::kWindowSurface, SkinColourProperty::kBackground);
        black->setColour(windowKey, juce::Colour(ColourChart::kRed500));
        expect(black->getColour(windowKey) == juce::Colour(ColourChart::kRed500));
        expect(black->getColour(SkinObjectType::kHeaderPanel, SkinColourProperty::kBackground)
               == juce::Colour(ColourChart::kGrey950));
    }

    void assertDialogOwnsCloseGlyph()
    {
        using TSS::SkinObjectType;
        using TSS::SkinColourProperty;

        const auto assertGlyphAndTitleBand = [this](TSS::Skin::ColourVariant variant, juce::uint32 titleBand)
        {
            const auto skin = TSS::Skin::create(variant);
            expect(skin->getColour(SkinObjectType::kDialog, SkinColourProperty::kGlyph)
                   == juce::Colour(ColourChart::kRed900));
            expect(skin->getColour(SkinObjectType::kDialog, SkinColourProperty::kTitleBand)
                   == juce::Colour(titleBand));
        };

        assertGlyphAndTitleBand(TSS::Skin::ColourVariant::BlackDuo, ColourChart::kBlack);
        assertGlyphAndTitleBand(TSS::Skin::ColourVariant::Cream, ColourChart::kCream400);
    }

    void assertChromeTextAndBrandLooks(TSS::Skin::ColourVariant variant)
    {
        using TSS::SkinObjectType;
        using TSS::SkinColourProperty;

        const auto skin = TSS::Skin::create(variant);
        const auto mapped = skin->getColour(SkinObjectType::kChromeText, SkinColourProperty::kText);
        const auto chromeLook = TSS::chromeTextLabelLookFromSkin(*skin);
        const auto brandLook = TSS::brandLabelLookFromSkin(*skin);
        const auto creamLabel = TSS::Skin::create(TSS::Skin::ColourVariant::Cream)
                                    ->getColour(SkinObjectType::kLabel, SkinColourProperty::kText);

        expect(chromeLook.text == mapped);
        expect(brandLook.text == mapped);
        expect(mapped != creamLabel);
        expect(chromeLook.text != creamLabel);
        expect(brandLook.text != creamLabel);
    }
};

static SkinChromeDecorrelationTests skinChromeDecorrelationTests;
