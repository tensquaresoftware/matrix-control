#include <juce_core/juce_core.h>

#include "GUI/Skins/SkinValues.h"
#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"
#include "Tools/SkinDesigner/SkinDesignerMappingLinkGroups.h"
#include "Tools/SkinDesigner/SkinDesignerMappingModePolicy.h"
#include "Tools/SkinDesigner/SkinDesignerMappingsColumns.h"

class SkinDesignerMappingLinkGroupsTests : public juce::UnitTest
{
public:
    SkinDesignerMappingLinkGroupsTests()
        : juce::UnitTest("SkinDesignerMappingLinkGroups")
    {
    }

    void runTest() override
    {
        createMergeRemoveDissolve();
        winnerPastilleRules();
        headerStateMachine();
        objectFilterBlocksCrossObjectExtend();
        linkColumnHitTest();
        calmRestAndUnlinkAll();
        hoverChainAndRevealWithSelection();
    }

private:
    static TSS::SkinColourKey key(TSS::SkinObjectType object,
                                  TSS::SkinColourProperty property,
                                  TSS::SkinColourState state = TSS::SkinColourState::kDefault,
                                  TSS::ColourSet set = TSS::ColourSet::k1)
    {
        return TSS::skinColourKey(object, property, state, set);
    }

    void createMergeRemoveDissolve()
    {
        beginTest("create / merge / remove / dissolve");

        using namespace SkinDesignerMappingLink;
        const auto a = key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = key(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        const auto c = key(TSS::SkinObjectType::kToggle, TSS::SkinColourProperty::kText);
        const auto d = key(TSS::SkinObjectType::kButton, TSS::SkinColourProperty::kBackground);

        Groups groups;
        groups = addToContext(std::move(groups), a, b, std::nullopt);
        expectEquals(static_cast<int>(groups.size()), 1);
        expectEquals(static_cast<int>(groups[0].size()), 2);

        groups = addToContext(std::move(groups), a, c, std::nullopt);
        expectEquals(static_cast<int>(groups[0].size()), 3);

        groups = linkOrMerge(std::move(groups), { d, a }, std::nullopt);
        expectEquals(static_cast<int>(groups.size()), 1);
        expect(findGroupIndex(groups, b) >= 0);
        expect(findGroupIndex(groups, d) >= 0);
        expectEquals(static_cast<int>(groups[0].size()), 4);

        groups = removeMember(std::move(groups), b);
        expect(findGroupIndex(groups, b) < 0);
        expectEquals(static_cast<int>(groups[0].size()), 3);

        groups = removeMember(std::move(groups), c);
        groups = removeMember(std::move(groups), d);
        expect(groups.empty()); // last pair dissolves when one remains then removed
        // Rebuild pair then dissolve by removing one of two.
        groups = addToContext({}, a, b, std::nullopt);
        groups = removeMember(std::move(groups), a);
        expect(groups.empty());

        // Joining A leaves B: candidate moves; other B members stay (or dissolve if alone).
        groups = addToContext({}, a, b, std::nullopt);
        groups = addToContext(std::move(groups), c, d, std::nullopt);
        expectEquals(static_cast<int>(groups.size()), 2);
        groups = addToContext(std::move(groups), a, c, std::nullopt);
        expectEquals(static_cast<int>(groups.size()), 1);
        expectEquals(static_cast<int>(groups[0].size()), 3);
        expect(findGroupIndex(groups, a) >= 0);
        expect(findGroupIndex(groups, b) >= 0);
        expect(findGroupIndex(groups, c) >= 0);
        expect(findGroupIndex(groups, d) < 0);
    }

    void winnerPastilleRules()
    {
        beginTest("winner pastille: mapped over Unbound; last mapped wins conflicts");

        using namespace SkinDesignerMappingLink;
        const auto a = key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = key(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        const auto c = key(TSS::SkinObjectType::kToggle, TSS::SkinColourProperty::kText);

        std::map<TSS::SkinColourKey, juce::String> mappings;
        mappings[b] = "Blue";
        expectEquals(winnerPastille(mappings, { a, b }), juce::String("Blue"));

        mappings[a] = "Red";
        mappings[c] = "Green";
        expectEquals(winnerPastille(mappings, { a, b, c }), juce::String("Green"));
        expectEquals(winnerPastille(mappings, { c, b, a }), juce::String("Red"));

        // Unbound selection still aligns onto a mapped peer already in the group.
        const Group members { a, b, c };
        expectEquals(winnerPastilleForGroup(mappings, members, { a }), juce::String("Red"));
        mappings.erase(a);
        expectEquals(winnerPastilleForGroup(mappings, members, { a }), juce::String("Green"));
    }

    void headerStateMachine()
    {
        beginTest("header chrome / action from selection");

        using namespace SkinDesignerMappingLink;
        const auto a = key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = key(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        const auto c = key(TSS::SkinObjectType::kToggle, TSS::SkinColourProperty::kText);

        Groups groups;
        auto empty = headerState(groups, {});
        expect(empty.chrome == HeaderChrome::kInactiveGrey);
        expect(empty.action == HeaderAction::kNone);
        expectEquals(empty.badgeCount, 0);

        auto oneFree = headerState(groups, { a });
        expect(oneFree.chrome == HeaderChrome::kInactiveGrey);
        expect(oneFree.action == HeaderAction::kNone);
        expectEquals(oneFree.badgeCount, 1);

        auto multiFree = headerState(groups, { a, b });
        expect(multiFree.chrome == HeaderChrome::kActiveGrey);
        expect(multiFree.action == HeaderAction::kCreateOrMerge);
        expectEquals(multiFree.badgeCount, 2);
        // Paint uses ActiveGrey → muted count badge (create/merge), Yellow → attention.

        groups = addToContext({}, a, b, std::nullopt);
        auto oneGrouped = headerState(groups, { a });
        expect(oneGrouped.chrome == HeaderChrome::kYellow);
        expect(oneGrouped.action == HeaderAction::kUnlinkAll);
        expectEquals(oneGrouped.badgeCount, 2);

        groups = addToContext(std::move(groups), a, c, std::nullopt);
        auto mixed = headerState(groups, { a, key(TSS::SkinObjectType::kButton,
                                                  TSS::SkinColourProperty::kBackground) });
        expect(mixed.action == HeaderAction::kCreateOrMerge);
    }

    void objectFilterBlocksCrossObjectExtend()
    {
        beginTest("Object filter blocks cross-object create/extend");

        using namespace SkinDesignerMappingLink;
        const auto slider = key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto combo = key(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        const auto filter = std::optional<TSS::SkinObjectType>(TSS::SkinObjectType::kSlider);

        Groups groups;
        groups = addToContext(std::move(groups), slider, combo, filter);
        expect(groups.empty());

        groups = addToContext({}, slider,
                              key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kBackground),
                              filter);
        expectEquals(static_cast<int>(groups.size()), 1);
    }

    void linkColumnHitTest()
    {
        beginTest("isLinkColumn hit geometry");

        using namespace SkinDesignerMappingPolicy;
        expect(isLinkColumn(200, 200, 70));
        expect(! isLinkColumn(199, 200, 70));
        expect(! isLinkColumn(270, 200, 70));

        const auto cols = SkinDesignerMappingsColumns::forWidth(
            SkinDesignerChrome::kMappingsTablePaintMinWidth);
        expectEquals(cols.linkW, SkinDesignerChrome::kMappingsLinkColumnWidth);
        const int expectedLinkLeft = SkinDesignerChrome::kMappingsPropertyColumnWidth
                                     + SkinDesignerChrome::kMappingsStateColumnWidth
                                     + SkinDesignerChrome::kMappingsColourSetColumnWidth;
        expectEquals(cols.linkLeft(), expectedLinkLeft);
        expect(SkinDesignerMappingsColumns::isLinkColumnX(expectedLinkLeft, cols));
        expect(! SkinDesignerMappingsColumns::isLinkColumnX(expectedLinkLeft - 1, cols));
        expect(! SkinDesignerMappingsColumns::isLinkColumnX(
            expectedLinkLeft + SkinDesignerChrome::kMappingsLinkColumnWidth, cols));
    }

    void calmRestAndUnlinkAll()
    {
        beginTest("calm rest: no hover chains; unlink-all dissolves group");

        using namespace SkinDesignerMappingLink;
        const auto a = key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = key(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);

        Groups groups = addToContext({}, a, b, std::nullopt);
        expect(! showsHoverChain(groups, {}, b, std::nullopt));
        expect(revealedKeys(groups, {}).empty());

        const auto header = headerState(groups, { a });
        expect(header.action == HeaderAction::kUnlinkAll);
        groups = unlinkKeys(std::move(groups), { a, b });
        expect(groups.empty());
    }

    void hoverChainAndRevealWithSelection()
    {
        beginTest("hover chain + reveal expand when selection is non-empty");

        using namespace SkinDesignerMappingLink;
        const auto a = key(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = key(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        const auto free = key(TSS::SkinObjectType::kToggle, TSS::SkinColourProperty::kText);

        Groups groups = addToContext({}, a, b, std::nullopt);
        const auto revealed = revealedKeys(groups, { a });
        expect(revealed.find(a) != revealed.end());
        expect(revealed.find(b) != revealed.end());
        expect(revealed.find(free) == revealed.end());

        expect(showsHoverChain(groups, { a }, free, std::nullopt));
        expect(! showsHoverChain(groups, { a }, a, std::nullopt));
        expect(! showsHoverChain(groups, { a }, b, std::nullopt));
    }
};

static SkinDesignerMappingLinkGroupsTests skinDesignerMappingLinkGroupsTests;
