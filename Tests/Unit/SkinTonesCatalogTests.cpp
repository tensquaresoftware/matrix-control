#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/EditorChromeShortcuts.h"
#include "GUI/Skins/ColourChartTones.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinCatalog.h"
#include "GUI/Skins/SkinColoursTones.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

class SkinTonesCatalogTests : public juce::UnitTest
{
public:
    SkinTonesCatalogTests()
        : juce::UnitTest("SkinTonesCatalog")
    {
    }

    void runTest() override
    {
        prefsIdsCoverSevenThroughTwentyOne();
        normalizeKeepsTonesAndFallsBackInvalid();
        displayNamesAndFamiliesAreAscii();
        familyMemberListsMatchCatalogFamilies();
        populateFlatSkinComboListsAllSkins();
        tonesSkinMatchesRampStops();
        petrolOverridesDifferFromSharedSteps();
        familyAndGlobalCycleOrder();
        arrowClassifierFamilyAndGlobal();
        bareArrowsDoNotClassifyAsSkinCycle();
        scaleZeroUnchanged();
        comboItemIdsMatchPluginIds();
    }

private:
    void prefsIdsCoverSevenThroughTwentyOne()
    {
        beginTest("prefsIdsCoverSevenThroughTwentyOne");

        using namespace PluginIDs::Settings::SkinVariants;
        expectEquals(kTonesCherry, 7);
        expectEquals(kTonesEmerald, 14);
        expectEquals(kTonesFuchsia, 21);
        expect(isValid(kTonesCherry));
        expect(isValid(kTonesFuchsia));
        expect(! isValid(0));
        expect(! isValid(22));
        expectEquals(TSS::SkinCatalog::kGlobalSkinCount, 21);
    }

    void normalizeKeepsTonesAndFallsBackInvalid()
    {
        beginTest("normalizeKeepsTonesAndFallsBackInvalid");

        using namespace PluginIDs::Settings::SkinVariants;
        expectEquals(normalize(kTonesEmerald), kTonesEmerald);
        expectEquals(normalize(99), kDefault);
        expectEquals(normalize(0), kDefault);
    }

    void displayNamesAndFamiliesAreAscii()
    {
        beginTest("displayNamesAndFamiliesAreAscii");

        using namespace PluginDisplayNames::ChoiceLists;
        expectEquals(juce::String(SkinFamilies::kBlack), juce::String("BLACK"));
        expectEquals(juce::String(SkinFamilies::kCream), juce::String("CREAM"));
        expectEquals(juce::String(SkinFamilies::kTones), juce::String("TONES"));
        expectEquals(juce::String(SkinVariants::kTonesEmerald), juce::String("TONES/EMERALD"));
        expect(! juce::String(SkinVariants::kTonesEmerald).containsChar('>'));
        expect(! juce::String(SkinFamilies::kTones).containsChar(static_cast<juce::juce_wchar>(0x25B6)));
    }

    void familyMemberListsMatchCatalogFamilies()
    {
        beginTest("familyMemberListsMatchCatalogFamilies");

        using namespace PluginIDs::Settings::SkinVariants;
        expectEquals(static_cast<int>(sizeof(TSS::SkinCatalog::kBlackFamilyIds)
                                      / sizeof(TSS::SkinCatalog::kBlackFamilyIds[0])),
                     3);
        expectEquals(static_cast<int>(sizeof(TSS::SkinCatalog::kCreamFamilyIds)
                                      / sizeof(TSS::SkinCatalog::kCreamFamilyIds[0])),
                     3);
        expectEquals(static_cast<int>(sizeof(TSS::SkinCatalog::kTonesFamilyIds)
                                      / sizeof(TSS::SkinCatalog::kTonesFamilyIds[0])),
                     15);
        expectEquals(TSS::SkinCatalog::kTonesFamilyIds[7], kTonesEmerald);
        expect(TSS::SkinCatalog::familyForItemId(kTonesEmerald)
               == TSS::Skin::SkinFamily::Tones);
        expect(TSS::SkinCatalog::familyForItemId(kBlackChroma)
               == TSS::Skin::SkinFamily::Black);
        expect(TSS::SkinCatalog::familyForItemId(kCreamMinimal)
               == TSS::Skin::SkinFamily::Cream);
    }

    void populateFlatSkinComboListsAllSkins()
    {
        beginTest("populateFlatSkinComboListsAllSkins");

        using namespace PluginIDs::Settings::SkinVariants;
        juce::ComboBox combo;
        TSS::SkinCatalog::populateFlatSkinCombo(combo);
        expectEquals(combo.getNumItems(), TSS::SkinCatalog::kGlobalSkinCount);
        expect(combo.getItemId(combo.getNumItems() - 1) == kTonesFuchsia
               || combo.getText().isNotEmpty());
        bool sawEmerald = false;
        bool sawFuchsia = false;
        for (int i = 0; i < combo.getNumItems(); ++i)
        {
            const int id = combo.getItemId(i);
            sawEmerald = sawEmerald || id == kTonesEmerald;
            sawFuchsia = sawFuchsia || id == kTonesFuchsia;
        }
        expect(sawEmerald);
        expect(sawFuchsia);
    }

    void tonesSkinMatchesRampStops()
    {
        beginTest("tonesSkinMatchesRampStops");

        const auto emerald = TSS::Skin::create(TSS::Skin::ColourVariant::TonesEmerald);
        expect(emerald->getSkinFamily() == TSS::Skin::SkinFamily::Tones);
        expectEquals(static_cast<int>(emerald->getMappedColourCount()),
                     TSS::SkinColours::Tones::kZoneCount);

        const auto windowBg = emerald->getColour(TSS::SkinObjectType::kWindowSurface,
                                                 TSS::SkinColourProperty::kBackground);
        const int step = TSS::SkinColours::Tones::kSharedStepIndices[0];
        expectEquals(static_cast<juce::int64>(windowBg.getARGB()),
                     static_cast<juce::int64>(TSS::ColourChartTones::stop(
                         TSS::ColourChartTones::ToneId::kEmerald, step)));
    }

    void petrolOverridesDifferFromSharedSteps()
    {
        beginTest("petrolOverridesDifferFromSharedSteps");

        expectEquals(static_cast<int>(sizeof(TSS::SkinColours::Tones::kPetrolStepOverrides)
                                      / sizeof(TSS::SkinColours::Tones::kPetrolStepOverrides[0])),
                     5);

        const auto cherry = TSS::Skin::create(TSS::Skin::ColourVariant::TonesCherry);
        const auto petrol = TSS::Skin::create(TSS::Skin::ColourVariant::TonesPetrol);
        const auto key = TSS::SkinColours::Tones::kZoneKeys[11];
        expect(cherry->getColour(key) != petrol->getColour(key));
    }

    void familyAndGlobalCycleOrder()
    {
        beginTest("familyAndGlobalCycleOrder");

        using namespace PluginIDs::Settings::SkinVariants;
        expectEquals(TSS::SkinCatalog::cycleFamily(kTonesGold, 1), kTonesOlive);
        expectEquals(TSS::SkinCatalog::cycleFamily(kTonesFuchsia, 1), kTonesCherry);
        expectEquals(TSS::SkinCatalog::cycleFamily(kTonesCherry, -1), kTonesFuchsia);

        expectEquals(TSS::SkinCatalog::cycleGlobal(kCreamMinimal, 1), kTonesCherry);
        expectEquals(TSS::SkinCatalog::cycleGlobal(kBlackMinimal, 1), kCream);
        expectEquals(TSS::SkinCatalog::cycleGlobal(kTonesFuchsia, 1), kBlack);
        expectEquals(TSS::SkinCatalog::cycleGlobal(kBlack, -1), kTonesFuchsia);
    }

    void arrowClassifierFamilyAndGlobal()
    {
        beginTest("arrowClassifierFamilyAndGlobal");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        const auto commandShift = command | ModifierKeys::shiftModifier;

        const auto familyDown = TSS::classifySkinArrowCycleShortcut(
            juce::KeyPress(juce::KeyPress::downKey, command, 0));
        expect(familyDown.isValid());
        expectEquals(familyDown.direction, 1);
        expect(! familyDown.global);

        const auto globalDown = TSS::classifySkinArrowCycleShortcut(
            juce::KeyPress(juce::KeyPress::downKey, commandShift, 0));
        expect(globalDown.isValid());
        expect(globalDown.global);
        expectEquals(globalDown.direction, 1);

        const auto familyUp = TSS::classifySkinArrowCycleShortcut(
            juce::KeyPress(juce::KeyPress::upKey, command, 0));
        expectEquals(familyUp.direction, -1);

        const auto familyRight = TSS::classifySkinArrowCycleShortcut(
            juce::KeyPress(juce::KeyPress::rightKey, command, 0));
        expectEquals(familyRight.direction, 1);

        const auto familyLeft = TSS::classifySkinArrowCycleShortcut(
            juce::KeyPress(juce::KeyPress::leftKey, command, 0));
        expectEquals(familyLeft.direction, -1);

        expect(TSS::isEditorChromeShortcut(
            juce::KeyPress(juce::KeyPress::leftKey, command, 0)));
    }

    void bareArrowsDoNotClassifyAsSkinCycle()
    {
        beginTest("bareArrowsDoNotClassifyAsSkinCycle");

        expect(! TSS::classifySkinArrowCycleShortcut(
                     juce::KeyPress(juce::KeyPress::downKey, 0, 0))
                     .isValid());
        expect(! TSS::classifySkinArrowCycleShortcut(
                     juce::KeyPress(juce::KeyPress::downKey, juce::ModifierKeys::shiftModifier, 0))
                     .isValid());
        expect(! TSS::isEditorChromeShortcut(juce::KeyPress(juce::KeyPress::upKey, 0, 0)));
    }

    void scaleZeroUnchanged()
    {
        beginTest("scaleZeroUnchanged");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('0', command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(TSS::classifySkinVariantShortcut(juce::KeyPress('0', command, 0)), 0);
    }

    void comboItemIdsMatchPluginIds()
    {
        beginTest("comboItemIdsMatchPluginIds");

        using namespace PluginIDs::Settings::SkinVariants;
        expectEquals(static_cast<int>(TSS::Skin::SkinComboBoxItemId::kTonesCherry), kTonesCherry);
        expectEquals(static_cast<int>(TSS::Skin::SkinComboBoxItemId::kTonesEmerald), kTonesEmerald);
        expectEquals(static_cast<int>(TSS::Skin::SkinComboBoxItemId::kTonesFuchsia), kTonesFuchsia);
        expectEquals(TSS::Skin::itemIdFromColourVariant(TSS::Skin::ColourVariant::TonesEmerald),
                     kTonesEmerald);
        expect(TSS::Skin::colourVariantFromItemId(kTonesPetrol)
               == TSS::Skin::ColourVariant::TonesPetrol);
    }
};

static SkinTonesCatalogTests skinTonesCatalogTests;
