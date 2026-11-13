#include <juce_core/juce_core.h>

#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Dev/DevSkinColourElementList.h"
#include "GUI/Skins/Dev/DevSkinSsotSeed.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinColoursCreamChroma.h"
#include "GUI/Skins/SkinColoursWidgetsControls.h"

using TSS::ColourSet;
using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;

class SkinCreamChromaTests : public juce::UnitTest
{
public:
    SkinCreamChromaTests()
        : juce::UnitTest("SkinCreamChroma")
    {
    }

    void runTest() override
    {
        beginTest("chart_softTrialRgbUnderStandardNames");
        assertSoftTrialRgb();

        beginTest("chart_relocatedBlackChromaRgbUnchanged");
        assertRelocatedBlackChromaRgb();

        beginTest("creamChroma_isCreamPlusAccentOverrides");
        assertCreamChromaIsCreamPlusAccents();

        beginTest("creamChroma_zoneAccentsMatchTrial");
        assertZoneAccentsMatchTrial();

        beginTest("creamChroma_compiledSeedUsesChartNamesOnly");
        assertCompiledSeedUsesChartNames();
    }

private:
    struct HexPair
    {
        juce::uint32 actual;
        juce::uint32 expected;
    };

    void expectAllEqual(std::initializer_list<HexPair> pairs)
    {
        for (const auto& pair : pairs)
            expectEquals(static_cast<juce::int64>(pair.actual), static_cast<juce::int64>(pair.expected));
    }

    // Trial CREAM/CHROMA 3 Soft* ARGB (Soft Orange 500 lands on Orange550).
    void assertSoftTrialRgb()
    {
        expectAllEqual({
            { ColourChart::kBlue100, 0xFFD3EBFB }, { ColourChart::kBlue200, 0xFF96C7E9 },
            { ColourChart::kBlue400, 0xFF5E99CD }, { ColourChart::kBlue500, 0xFF2F8BDC },
            { ColourChart::kBlue600, 0xFF1A60B7 }, { ColourChart::kBlue700, 0xFF0F56AC },
            { ColourChart::kBlue800, 0xFF144682 },
            { ColourChart::kOrange100, 0xFFF9E6CA }, { ColourChart::kOrange200, 0xFFE5BE86 },
            { ColourChart::kOrange400, 0xFFC08B4B }, { ColourChart::kOrange500, 0xFFD44818 },
            { ColourChart::kOrange550, 0xFFE5942F },
            { ColourChart::kOrange600, 0xFFBF5F0E }, { ColourChart::kOrange700, 0xFFAC560C },
            { ColourChart::kOrange800, 0xFF7C4215 },
            { ColourChart::kYellow100, 0xFFF8F0CD }, { ColourChart::kYellow200, 0xFFF3DF7A },
            { ColourChart::kYellow400, 0xFFC7AB2D }, { ColourChart::kYellow500, 0xFFD9B404 },
            { ColourChart::kYellow600, 0xFF9D7D04 }, { ColourChart::kYellow700, 0xFF907204 },
            { ColourChart::kYellow800, 0xFF735C03 },
        });
    }

    void assertRelocatedBlackChromaRgb()
    {
        using namespace TSS::SkinColours::Widgets;
        expectAllEqual({
            { ColourChart::kBlue450, 0xFF28A0F8 }, { ColourChart::kBlue650, 0xFF034FAD },
            { ColourChart::kBlue750, 0xFF053979 }, { ColourChart::kBlue950, 0xFF012959 },
            { ColourChart::kOrange350, 0xFFEE8B21 }, { ColourChart::kOrange650, 0xFF854408 },
            { ColourChart::kOrange750, 0xFF7A4511 }, { ColourChart::kOrange950, 0xFF422514 },
            { ColourChart::kOrange500, 0xFFD44818 },
            { ColourChart::kYellow300, 0xFFFFCC00 }, { ColourChart::kYellow650, 0xFF806600 },
            { ColourChart::kYellow850, 0xFF5C4A00 }, { ColourChart::kYellow950, 0xFF3D3208 },
            { Slider::kText.blackChromaVariant, 0xFF28A0F8 },
            { Slider::kValueBar.blackChromaVariant, 0xFF034FAD },
            { SliderK2::kText.blackChromaVariant, 0xFFEE8B21 },
            { SliderK2::kValueBar.blackChromaVariant, 0xFF854408 },
            { SliderK3::kText.blackChromaVariant, 0xFFFFCC00 },
            { SliderK3::kValueBar.blackChromaVariant, 0xFF806600 },
            { Slider::kValueBar.creamVariant, ColourChart::kGreen500 },
        });
    }

    static const TSS::SkinColours::CreamChroma::AccentOverride* findAccent(TSS::SkinColourKey key)
    {
        for (const auto& accent : TSS::SkinColours::CreamChroma::kAccentOverrides)
            if (accent.key == key)
                return &accent;
        return nullptr;
    }

    void assertCreamChromaIsCreamPlusAccents()
    {
        const auto cream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto chroma = TSS::Skin::create(TSS::Skin::ColourVariant::CreamChroma);
        expectEquals(chroma->getMappedColourCount(), cream->getMappedColourCount());

        int overridden = 0;
        TSS::DevSkin::forEachCompiledColourElement([&](const TSS::SkinColours::ColourElement& element)
        {
            const auto* accent = findAccent(element.key);
            const auto expected = accent != nullptr ? accent->argb : element.creamVariant;
            overridden += accent != nullptr ? 1 : 0;
            expect(chroma->getColour(element.key) == juce::Colour(expected));
        });
        expectEquals(overridden, static_cast<int>(std::size(TSS::SkinColours::CreamChroma::kAccentOverrides)));
    }

    static TSS::SkinColourKey keyAt(SkinObjectType object, SkinColourProperty property, ColourSet colourSet)
    {
        return TSS::skinColourKey(object, property, SkinColourState::kDefault, colourSet);
    }

    void assertZoneAccentsMatchTrial()
    {
        const auto chroma = TSS::Skin::create(TSS::Skin::ColourVariant::CreamChroma);
        const auto argb = [&chroma](TSS::SkinColourKey key) { return chroma->getColour(key).getARGB(); };

        expectAllEqual({
            { argb(keyAt(SkinObjectType::kSlider, SkinColourProperty::kValueBar, ColourSet::k1)), 0xFF2F8BDC },
            { argb(keyAt(SkinObjectType::kSlider, SkinColourProperty::kValueBar, ColourSet::k2)), 0xFFE5942F },
            { argb(keyAt(SkinObjectType::kSlider, SkinColourProperty::kValueBar, ColourSet::k3)), 0xFFD9B404 },
            { argb(keyAt(SkinObjectType::kComboBox, SkinColourProperty::kBackground, ColourSet::k1)), 0xFF1A60B7 },
            { argb(keyAt(SkinObjectType::kComboBox, SkinColourProperty::kBackground, ColourSet::k2)), 0xFFBF5F0E },
            { argb(keyAt(SkinObjectType::kPopupMenu, SkinColourProperty::kText, ColourSet::k1)), 0xFF144682 },
            { argb(keyAt(SkinObjectType::kPopupMenu, SkinColourProperty::kText, ColourSet::k2)), 0xFF7C4215 },
            { argb(TSS::skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground,
                                      SkinColourState::kOn)), 0xFFD9B404 },
            { argb(keyAt(SkinObjectType::kFooterMessage, SkinColourProperty::kText, ColourSet::k1)),
              TSS::Skin::create(TSS::Skin::ColourVariant::Cream)->getColour(
                  keyAt(SkinObjectType::kFooterMessage, SkinColourProperty::kText, ColourSet::k1)).getARGB() },
        });
    }

    void assertCompiledSeedUsesChartNames()
    {
        const auto skin = TSS::DevSkin::makeCompiledSkin(TSS::DevSkin::SkinKind::kCompiledCreamChroma);
        expectEquals(skin.id, juce::String(TSS::DevSkin::kCompiledCreamChromaId));
        expectEquals(skin.name, juce::String("CREAM/CHROMA"));

        for (const auto& [key, paletteName] : skin.mappings)
        {
            expect(! paletteName.containsIgnoreCase("Soft"));
            expect(! paletteName.startsWith("custom_"));
        }

        expectEquals(skin.mappings.at(keyAt(SkinObjectType::kSlider, SkinColourProperty::kTrack, ColourSet::k1)),
                     juce::String("kBlue600"));
        expectEquals(skin.mappings.at(keyAt(SkinObjectType::kSlider, SkinColourProperty::kValueBar, ColourSet::k2)),
                     juce::String("kOrange550"));
        expectEquals(skin.mappings.at(keyAt(SkinObjectType::kSlider, SkinColourProperty::kText, ColourSet::k3)),
                     juce::String("kYellow100"));
    }
};

static SkinCreamChromaTests skinCreamChromaTests;
