#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
#include "GUI/Skins/SkinValues.h"
#include "Tools/SkinEditor/SkinEditorMappingsColumnFilter.h"

class SkinEditorMappingsColumnFilterTests : public juce::UnitTest
{
public:
    SkinEditorMappingsColumnFilterTests()
        : juce::UnitTest("SkinEditorMappingsColumnFilter")
    {
    }

    void runTest() override
    {
        singlePropertyFilter();
        multiColumnAnd();
        colourSetFilter();
        clearOneColumnKeepsOther();
        changeMappedValue();
        noMatchesKeepFiltersMeaningful();
        unboundMappedFilter();
        presentValuesIncludeUnboundAndPastilles();
        presentValuesNoDuplicateUnbound();
        filteredObjectGroupsOmitEmpty();
        emptyFiltersMatchAll();
        filteredObjectGroupsFollowDisplayOrder();
        displayOrderCoversInventoryObjects();
        peakIndicatorFollowsLed();
        inventoryExpandsOnlyMultiSetObjects();
    }

private:
    static TSS::SkinColourKey key(TSS::SkinObjectType object,
                                  TSS::SkinColourProperty property,
                                  TSS::SkinColourState state)
    {
        return TSS::skinColourKey(object, property, state);
    }

    // Inventory keys from SkinColoursWidgetsControls (Button Border/Text).
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
        skin.id = "test-editor";
        skin.name = "Test";
        skin.mappings[borderOff()] = "kBlue";
        skin.mappings[borderOn()] = "kBlue";
        // textHover intentionally unbound
        return skin;
    }

    void singlePropertyFilter()
    {
        beginTest("Single Property=Border keeps only Border keys");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters borderOnly { "Border", {}, {}, {} };

        expect(keyMatches(skin, borderOff(), borderOnly));
        expect(! keyMatches(skin, textHover(), borderOnly));
    }

    void multiColumnAnd()
    {
        beginTest("Property=Border and State=On require both");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters both { "Border", "On", {}, {} };

        expect(keyMatches(skin, borderOn(), both));
        expect(! keyMatches(skin, borderOff(), both));
    }

    void clearOneColumnKeepsOther()
    {
        beginTest("Clearing State filter leaves Property filter active");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        // Arrange: Property+State reject a State-mismatched Border key.
        const ActiveFilters propertyAndState { "Border", "On", {}, {} };
        expect(! keyMatches(skin, borderOff(), propertyAndState));
        // Assert: Property-only (State cleared) still matches that key.
        const ActiveFilters propertyOnly { "Border", {}, {}, {} };
        expect(keyMatches(skin, borderOff(), propertyOnly));
    }

    void colourSetFilter()
    {
        beginTest("ColourSet filter combines with Property and State");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        auto setTwo = borderOn();
        setTwo.colourSet = TSS::ColourSet::k2;
        const ActiveFilters setTwoOnly { "Border", "On", "k2", {} };

        expect(keyMatches(skin, setTwo, setTwoOnly));
        expect(! keyMatches(skin, borderOn(), setTwoOnly));

        const auto values = presentValues(skin, Column::kColourSet);
        expect(values.contains("k1"));
        expect(values.contains("k4"));
    }

    void changeMappedValue()
    {
        beginTest("Mapped colour filter updates from kBlue to Unbound");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters blue { {}, {}, {}, "kBlue" };
        const ActiveFilters unbound { {}, {}, {}, kUnboundMenuValue };

        expect(keyMatches(skin, borderOff(), blue));
        expect(! keyMatches(skin, textHover(), blue));
        expect(keyMatches(skin, textHover(), unbound));
        expect(! keyMatches(skin, borderOff(), unbound));
    }

    void noMatchesKeepFiltersMeaningful()
    {
        beginTest("Impossible AND yields no matches without clearing filters");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters impossible { "Border", "On", {}, "missing-pastille" };
        int hits = 0;
        for (const auto& k : TSS::DevSkin::allStructuredKeys())
        {
            if (keyMatches(skin, k, impossible))
                ++hits;
        }
        expectEquals(hits, 0);
    }

    void unboundMappedFilter()
    {
        beginTest("Mapped colour Unbound isolates unbound rows");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters unbound { {}, {}, {}, kUnboundMenuValue };

        expect(keyMatches(skin, textHover(), unbound));
        expect(! keyMatches(skin, borderOff(), unbound));
    }

    void presentValuesIncludeUnboundAndPastilles()
    {
        beginTest("Present mapped values include Unbound and kBlue");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const auto values = presentValues(skin, Column::kMappedColour);
        expect(values.contains(kUnboundMenuValue));
        expect(values.contains("kBlue"));
        expectEquals(values.indexOf(kUnboundMenuValue), 0);

        const auto props = presentValues(skin, Column::kProperty);
        expect(props.contains("Border"));
        expect(props.contains("Text"));
    }

    void presentValuesNoDuplicateUnbound()
    {
        beginTest("Present values keep a single Unbound when a pastille is named Unbound");

        using namespace SkinEditorMappingsColumnFilter;
        auto skin = makeSkin();
        skin.mappings[borderOff()] = kUnboundMenuValue;
        const auto values = presentValues(skin, Column::kMappedColour);
        int unboundCount = 0;
        for (const auto& value : values)
            if (value == kUnboundMenuValue)
                ++unboundCount;
        expectEquals(unboundCount, 1);
    }

    void filteredObjectGroupsOmitEmpty()
    {
        beginTest("filteredObjectGroups omits empty objects and keeps only matches");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters borderOnly { "Border", {}, {}, {} };
        const auto groups = filteredObjectGroups(skin, borderOnly);

        expect(! groups.empty());
        for (const auto& group : groups)
        {
            expect(! group.keys.empty());
            for (const auto& k : group.keys)
            {
                expect(keyMatches(skin, k, borderOnly));
                expectEquals(TSS::DevSkin::toString(k.property), juce::String("Border"));
            }
        }

        bool sawButton = false;
        for (const auto& group : groups)
            if (group.object == TSS::SkinObjectType::kButton)
                sawButton = true;
        expect(sawButton);

        // Objects with no Border keys must be absent.
        for (const auto& group : groups)
        {
            bool anyMatch = false;
            for (const auto& k : TSS::DevSkin::allStructuredKeys())
            {
                if (k.object != group.object)
                    continue;
                if (keyMatches(skin, k, borderOnly))
                {
                    anyMatch = true;
                    break;
                }
            }
            expect(anyMatch);
        }
    }

    void emptyFiltersMatchAll()
    {
        beginTest("Empty filters match every inventory key");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters none {};
        for (const auto& k : TSS::DevSkin::allStructuredKeys())
            expect(keyMatches(skin, k, none));
    }

    void filteredObjectGroupsFollowDisplayOrder()
    {
        beginTest("filteredObjectGroups follows editor descending display order");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters none {};
        const auto groups = filteredObjectGroups(skin, none);
        const auto& order = mappingsObjectDisplayOrder();

        // Full inventory coverage ⇒ exact display list (append path unused today).
        expectEquals(static_cast<int>(groups.size()), static_cast<int>(order.size()));
        for (size_t i = 0; i < groups.size(); ++i)
            expect(groups[i].object == order[i]);
    }

    void displayOrderCoversInventoryObjects()
    {
        beginTest("mappingsObjectDisplayOrder lists every inventory object once");

        using namespace SkinEditorMappingsColumnFilter;
        const auto& order = mappingsObjectDisplayOrder();
        for (size_t i = 0; i < order.size(); ++i)
            for (size_t j = i + 1; j < order.size(); ++j)
                expect(order[i] != order[j]);

        for (const auto& key : TSS::DevSkin::allStructuredKeys())
        {
            bool listed = false;
            for (const auto object : order)
            {
                if (object == key.object)
                {
                    listed = true;
                    break;
                }
            }
            expect(listed);
        }
    }

    void peakIndicatorFollowsLed()
    {
        beginTest("PeakIndicator table follows Led");

        using namespace SkinEditorMappingsColumnFilter;
        const auto& order = mappingsObjectDisplayOrder();
        int ledIndex = -1;
        int peakIndex = -1;
        for (int i = 0; i < static_cast<int>(order.size()); ++i)
        {
            if (order[static_cast<size_t>(i)] == TSS::SkinObjectType::kLed)
                ledIndex = i;
            if (order[static_cast<size_t>(i)] == TSS::SkinObjectType::kPeakIndicator)
                peakIndex = i;
        }
        expectEquals(peakIndex, ledIndex + 1);
    }

    void inventoryExpandsOnlyMultiSetObjects()
    {
        beginTest("All twelve multi-set objects have k1-k4; remaining objects stay k1");

        const auto inventory = TSS::DevSkin::allStructuredKeys();
        for (const auto object : TSS::kMultiSetSkinObjects)
        {
            bool seenSets[TSS::kAllColourSets.size()] {};
            for (const auto& inventoryKey : inventory)
                if (inventoryKey.object == object)
                    seenSets[static_cast<size_t>(inventoryKey.colourSet)] = true;
            for (const bool seen : seenSets)
                expect(seen);
        }

        for (const auto& inventoryKey : inventory)
            if (! TSS::supportsMultipleColourSets(inventoryKey.object))
                expect(inventoryKey.colourSet == TSS::ColourSet::k1);
    }
};

static SkinEditorMappingsColumnFilterTests skinEditorMappingsColumnFilterTests;
