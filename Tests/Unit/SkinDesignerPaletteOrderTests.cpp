#include <algorithm>
#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

class SkinDesignerPaletteOrderTests : public juce::UnitTest
{
public:
    SkinDesignerPaletteOrderTests()
        : juce::UnitTest("SkinDesignerPaletteOrder")
    {
    }

    void runTest() override
    {
        beginTest("comparePalettePastilleNames_fixedHeadGreysBlackThenNaturalTail");
        assertCompareOrder();

        beginTest("designerStarterKitPastilles_whiteBeforeBlack");
        assertStarterKitOrder();
    }

private:
    void assertCompareOrder()
    {
        using SkinDesignerUi::comparePalettePastilleNames;

        expect(comparePalettePastilleNames("kBrandCream", "kBrandBlue") < 0);
        expect(comparePalettePastilleNames("kBrandBlue", "kBrandOrange") < 0);
        expect(comparePalettePastilleNames("kBrandOrange", "kWhite") < 0);
        expect(comparePalettePastilleNames("kWhite", "kGrey100") < 0);
        expect(comparePalettePastilleNames("kGrey100", "kGrey900") < 0);
        expect(comparePalettePastilleNames("kGrey900", "kBlack") < 0);
        expect(comparePalettePastilleNames("kBlack", "Accent") < 0);
        expect(comparePalettePastilleNames("Accent 2", "Accent 10") < 0);
        expect(comparePalettePastilleNames("BrandCream", "kBrandCream") == 0);
        expect(comparePalettePastilleNames("GreyFoo", "kBlack") > 0);

        std::vector<juce::String> names {
            "kBlack", "Accent", "kGrey900", "kBrandOrange", "kWhite",
            "kBrandCream", "kGrey100", "kBrandBlue", "Zebra",
        };
        std::sort(names.begin(), names.end(),
                  [](const juce::String& a, const juce::String& b)
                  {
                      return comparePalettePastilleNames(a, b) < 0;
                  });
        expectEquals(names[0], juce::String("kBrandCream"));
        expectEquals(names[1], juce::String("kBrandBlue"));
        expectEquals(names[2], juce::String("kBrandOrange"));
        expectEquals(names[3], juce::String("kWhite"));
        expectEquals(names[4], juce::String("kGrey100"));
        expectEquals(names[5], juce::String("kGrey900"));
        expectEquals(names[6], juce::String("kBlack"));
        expectEquals(names[7], juce::String("Accent"));
        expectEquals(names[8], juce::String("Zebra"));
    }

    void assertStarterKitOrder()
    {
        const auto kit = TSS::DevSkin::designerStarterKitPastilles();
        expectEquals(static_cast<int>(kit.size()), 5);
        expectEquals(kit[0].name, juce::String("kBrandCream"));
        expectEquals(kit[1].name, juce::String("kBrandBlue"));
        expectEquals(kit[2].name, juce::String("kBrandOrange"));
        expectEquals(kit[3].name, juce::String("kWhite"));
        expectEquals(kit[4].name, juce::String("kBlack"));
    }
};

static SkinDesignerPaletteOrderTests skinDesignerPaletteOrderTests;
