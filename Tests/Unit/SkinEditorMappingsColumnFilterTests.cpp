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
        clearOneColumnKeepsOther();
        changeMappedValue();
        noMatchesKeepFiltersMeaningful();
        unboundMappedFilter();
        presentValuesIncludeUnboundAndPastilles();
        presentValuesNoDuplicateUnbound();
        filteredObjectGroupsOmitEmpty();
        emptyFiltersMatchAll();
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
        const ActiveFilters borderOnly { "Border", {}, {} };

        expect(keyMatches(skin, borderOff(), borderOnly));
        expect(! keyMatches(skin, textHover(), borderOnly));
    }

    void multiColumnAnd()
    {
        beginTest("Property=Border and State=On require both");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters both { "Border", "On", {} };

        expect(keyMatches(skin, borderOn(), both));
        expect(! keyMatches(skin, borderOff(), both));
    }

    void clearOneColumnKeepsOther()
    {
        beginTest("Clearing State filter leaves Property filter active");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        // Arrange: Property+State reject a State-mismatched Border key.
        const ActiveFilters propertyAndState { "Border", "On", {} };
        expect(! keyMatches(skin, borderOff(), propertyAndState));
        // Assert: Property-only (State cleared) still matches that key.
        const ActiveFilters propertyOnly { "Border", {}, {} };
        expect(keyMatches(skin, borderOff(), propertyOnly));
    }

    void changeMappedValue()
    {
        beginTest("Mapped colour filter updates from kBlue to Unbound");

        using namespace SkinEditorMappingsColumnFilter;
        const auto skin = makeSkin();
        const ActiveFilters blue { {}, {}, "kBlue" };
        const ActiveFilters unbound { {}, {}, kUnboundMenuValue };

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
        const ActiveFilters impossible { "Border", "On", "missing-pastille" };
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
        const ActiveFilters unbound { {}, {}, kUnboundMenuValue };

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
        const ActiveFilters borderOnly { "Border", {}, {} };
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
};

static SkinEditorMappingsColumnFilterTests skinEditorMappingsColumnFilterTests;
