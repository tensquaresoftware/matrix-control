#include <algorithm>

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "GUI/Skins/SkinValues.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"

/** Build B — sort after filter, header gestures, Object macro-filter. */
class SkinEditorMappingsSortErgoTests : public juce::UnitTest
{
public:
    SkinEditorMappingsSortErgoTests()
        : juce::UnitTest("SkinEditorMappingsSortErgo")
    {
    }

    void runTest() override
    {
        sortFilteredKeysAscendingThenDescending();
        sortFilteredKeysStableWhenNone();
        sortDisplayValueUnboundMapped();
        filterThenSortPropertyAscending();
        headerGestureLeftSortRightFilter();
        objectMacroFilterAllVersusSingle();
        objectFilterNameFromComboSelectionHelper();
    }

private:
    static TSS::SkinColourKey key(TSS::SkinObjectType object,
                                  TSS::SkinColourProperty property,
                                  TSS::SkinColourState state)
    {
        return TSS::skinColourKey(object, property, state);
    }

    static TSS::SkinColourKey borderOff()
    {
        return key(TSS::SkinObjectType::kButton,
                   TSS::SkinColourProperty::kBorder,
                   TSS::SkinColourState::kOff);
    }

    static TSS::SkinColourKey borderOn()
    {
        return key(TSS::SkinObjectType::kButton,
                   TSS::SkinColourProperty::kBorder,
                   TSS::SkinColourState::kOn);
    }

    static TSS::SkinColourKey textHover()
    {
        return key(TSS::SkinObjectType::kButton,
                   TSS::SkinColourProperty::kText,
                   TSS::SkinColourState::kHover);
    }

    static TSS::DevSkin::NamedSkin makeSkin()
    {
        TSS::DevSkin::NamedSkin skin;
        skin.id = "test-sort-ergo";
        skin.name = "Test";
        skin.mappings[borderOff()] = "kBlue";
        skin.mappings[borderOn()] = "kBlue";
        return skin;
    }

    void sortFilteredKeysAscendingThenDescending()
    {
        beginTest("sortFilteredKeys orders Property A->Z then Z->A");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        std::vector<TSS::SkinColourKey> keys { textHover(), borderOff(), borderOn() };

        SortSpec ascending { SortColumn::kProperty, true };
        sortFilteredKeys(skin, keys, ascending);
        expectEquals(TSS::DevSkin::toString(keys[0].property), juce::String("Border"));
        expectEquals(TSS::DevSkin::toString(keys[1].property), juce::String("Border"));
        expectEquals(TSS::DevSkin::toString(keys[2].property), juce::String("Text"));

        SortSpec descending { SortColumn::kProperty, false };
        sortFilteredKeys(skin, keys, descending);
        expectEquals(TSS::DevSkin::toString(keys[0].property), juce::String("Text"));
        expectEquals(TSS::DevSkin::toString(keys[1].property), juce::String("Border"));
        expectEquals(TSS::DevSkin::toString(keys[2].property), juce::String("Border"));
    }

    void sortFilteredKeysStableWhenNone()
    {
        beginTest("sortFilteredKeys is a no-op when SortColumn is kNone");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        std::vector<TSS::SkinColourKey> keys { textHover(), borderOff() };
        const auto before0 = keys[0];
        const auto before1 = keys[1];
        sortFilteredKeys(skin, keys, {});
        expect(keys[0].property == before0.property && keys[0].state == before0.state);
        expect(keys[1].property == before1.property && keys[1].state == before1.state);
    }

    void sortDisplayValueUnboundMapped()
    {
        beginTest("sortDisplayValue uses Unbound for empty mapped colour");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        expectEquals(sortDisplayValue(skin, textHover(), SortColumn::kMappedColour),
                     juce::String(kUnboundMenuValue));
        expectEquals(sortDisplayValue(skin, borderOff(), SortColumn::kMappedColour),
                     juce::String("kBlue"));
        expectEquals(sortDisplayValue(skin, borderOn(), SortColumn::kColourSet),
                     juce::String("k1"));
    }

    void filterThenSortPropertyAscending()
    {
        beginTest("State filter then Property sort keeps only matching rows ordered A->Z");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        ActiveFilters filters;
        filters.state = "On";
        std::vector<TSS::SkinColourKey> keys { borderOn(), textHover(), borderOff() };
        keys.erase(std::remove_if(keys.begin(), keys.end(),
                                  [&](const TSS::SkinColourKey& candidate)
                                  {
                                      return ! keyMatches(skin, candidate, filters);
                                  }),
                   keys.end());
        sortFilteredKeys(skin, keys, { SortColumn::kProperty, true });
        expectEquals(static_cast<int>(keys.size()), 1);
        expectEquals(TSS::DevSkin::toString(keys[0].property), juce::String("Border"));
        expect(keys[0].state == TSS::SkinColourState::kOn);
    }

    void headerGestureLeftSortRightFilter()
    {
        beginTest("Header left-click sorts; popup/right-click opens filter");

        using namespace SkinEditorMappingsColumnFilter;
        expect(headerGestureForClick(false) == HeaderGesture::kSort);
        expect(headerGestureForClick(true) == HeaderGesture::kOpenFilter);
    }

    void objectMacroFilterAllVersusSingle()
    {
        beginTest("Empty Object filter shows all groups; named filter shows one object");

        using namespace SkinEditorMappingsColumnFilter;
        expect(objectMacroFilterShowsGroup({}, TSS::SkinObjectType::kSlider));
        expect(objectMacroFilterShowsGroup({}, TSS::SkinObjectType::kButton));
        expect(objectMacroFilterShowsGroup("Slider", TSS::SkinObjectType::kSlider));
        expect(! objectMacroFilterShowsGroup("Slider", TSS::SkinObjectType::kButton));
        expect(objectMacroFilterShowsGroup(juce::String(), TSS::SkinObjectType::kComboBox));
    }

    void objectFilterNameFromComboSelectionHelper()
    {
        beginTest("objectFilterNameFromComboSelection: id<=1 clears; else keeps item text");

        using namespace SkinEditorMappingsColumnFilter;
        expect(objectFilterNameFromComboSelection(1, "- All objects -").isEmpty());
        expect(objectFilterNameFromComboSelection(0, "Slider").isEmpty());
        expectEquals(objectFilterNameFromComboSelection(2, "Slider"), juce::String("Slider"));
        expectEquals(objectFilterNameFromComboSelection(5, "ProgressBar"),
                     juce::String("ProgressBar"));
    }
};

static SkinEditorMappingsSortErgoTests skinEditorMappingsSortErgoTests;
