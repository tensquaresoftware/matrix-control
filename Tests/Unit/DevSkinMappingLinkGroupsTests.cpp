#include <algorithm>

#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinJsonMapping.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/SkinValues.h"

class DevSkinMappingLinkGroupsTests : public juce::UnitTest
{
public:
    DevSkinMappingLinkGroupsTests()
        : juce::UnitTest("DevSkinMappingLinkGroups")
    {
    }

    void runTest() override
    {
        beginTest("mappingLinkGroups_roundTrip_andOmittedFromPush");
        assertRoundTripAndPushOmit();
        beginTest("mappingLinkGroups_dropsOrphansAndUndersized");
        assertOrphansAndUndersizedDropped();
        beginTest("mappingLinkGroups_compiledSkinsRejectSet");
        assertCompiledRejects();
        beginTest("mappingLinkGroups_propagatePeersRemapsPeerPastille");
        assertPropagatePeers();
        beginTest("mappingLinkGroups_duplicateSkinClearsGroupsWhenDroppingMappings");
        assertDuplicateClearsLinkGroups();
        beginTest("mappingLinkGroups_sanitizeEnforcesExclusiveMembership");
        assertExclusiveMembership();
    }

private:
    static TSS::SkinColourKey sampleKey(TSS::SkinObjectType object,
                                        TSS::SkinColourProperty property)
    {
        return TSS::skinColourKey(object, property, TSS::SkinColourState::kDefault,
                                  TSS::ColourSet::k1);
    }

    void assertRoundTripAndPushOmit()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Link Skin").wasOk());
        const auto skinId = layer.activeSkinId();

        const auto a = sampleKey(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = sampleKey(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        expect(layer.setMappingLinkGroups(skinId, { { a, b } }).wasOk());
        expect(layer.setColourSetLabel(skinId, TSS::ColourSet::k1, "Patch").wasOk());

        const auto json = TSS::DevSkin::exportEditorSkinToJson(layer, skinId);
        const auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        auto* skinsArr = root->getProperty("skins").getArray();
        expect(skinsArr != nullptr && ! skinsArr->isEmpty());
        auto* skinObj = (*skinsArr)[0].getDynamicObject();
        expect(skinObj != nullptr);
        expect(skinObj->getProperty("mappingLinkGroups").isArray());
        expect(skinObj->getProperty("colourSetLabels").getDynamicObject() != nullptr);

        TSS::DevSkin::Layer restored;
        const auto imported = TSS::DevSkin::appendEditorSkinsFromJson(restored, json);
        expect(imported.ok);
        const auto* skin = restored.findSkin(imported.importedSkinIds[0]);
        expect(skin != nullptr);
        expectEquals(static_cast<int>(skin->mappingLinkGroups.size()), 1);
        expectEquals(static_cast<int>(skin->mappingLinkGroups[0].size()), 2);
        const auto& restoredGroup = skin->mappingLinkGroups[0];
        expect(std::find(restoredGroup.begin(), restoredGroup.end(), a) != restoredGroup.end());
        expect(std::find(restoredGroup.begin(), restoredGroup.end(), b) != restoredGroup.end());
        expectEquals(skin->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k1)],
                     juce::String("Patch"));

        const auto pushVar = TSS::DevSkin::pushPayloadToVar(layer.makePushPayloadForSkin(skinId));
        auto* pushObj = pushVar.getDynamicObject();
        expect(pushObj != nullptr);
        expect(pushObj->getProperty("mappingLinkGroups").isVoid());
        expect(pushObj->getProperty("colourSetLabels").isVoid());
    }

    void assertOrphansAndUndersizedDropped()
    {
        const auto inventory = TSS::DevSkin::allStructuredKeys();
        expect(inventory.size() >= 2);
        const std::set<TSS::SkinColourKey> validKeys(inventory.begin(), inventory.end());

        // Slider+TitleBand is not a structured inventory key (title bands belong to dialogs).
        const auto orphan = TSS::skinColourKey(TSS::SkinObjectType::kSlider,
                                               TSS::SkinColourProperty::kTitleBand);
        expect(validKeys.find(orphan) == validKeys.end());

        const auto cleaned = TSS::DevSkin::sanitizeMappingLinkGroups(
            { { inventory[0], orphan }, { inventory[0] } }, validKeys);
        expect(cleaned.empty());

        const auto kept = TSS::DevSkin::sanitizeMappingLinkGroups(
            { { inventory[0], inventory[1] }, { inventory[0] } }, validKeys);
        expectEquals(static_cast<int>(kept.size()), 1);
        expectEquals(static_cast<int>(kept[0].size()), 2);
    }

    void assertCompiledRejects()
    {
        TSS::DevSkin::Layer layer;
        const auto a = sampleKey(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = sampleKey(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        expect(layer.setMappingLinkGroups(TSS::DevSkin::kCompiledBlackId, { { a, b } }).failed());
        expect(layer.setMappingLinkGroups(TSS::DevSkin::kCompiledCreamId, { { a, b } }).failed());
    }

    void assertPropagatePeers()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Propagate Link").wasOk());
        const auto skinId = layer.activeSkinId();

        const auto a = sampleKey(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = sampleKey(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        expect(layer.setMappingLinkGroups(skinId, { { a, b } }).wasOk());

        const auto* skin = layer.findSkin(skinId);
        expect(skin != nullptr);
        expect(skin->palette.size() >= 2);
        const auto winner = skin->palette[0].name;
        const auto other = skin->palette[1].name;
        expect(winner != other);

        expect(layer.bindUnbound(skinId, b, other).wasOk());
        expect(layer.bindUnbound(skinId, a, winner).wasOk());
        expect(layer.propagateMappingToLinkPeers(skinId, a, winner).wasOk());

        skin = layer.findSkin(skinId);
        expect(skin != nullptr);
        const auto peer = skin->mappings.find(b);
        expect(peer != skin->mappings.end());
        expectEquals(peer->second, winner);
    }

    void assertDuplicateClearsLinkGroups()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Link Source").wasOk());
        const auto sourceId = layer.activeSkinId();

        const auto a = sampleKey(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kText);
        const auto b = sampleKey(TSS::SkinObjectType::kComboBox, TSS::SkinColourProperty::kText);
        expect(layer.setMappingLinkGroups(sourceId, { { a, b } }).wasOk());

        expect(layer.duplicateSkin(sourceId, "Link Copy Keep", true).wasOk());
        const auto* kept = layer.findSkin(layer.activeSkinId());
        expect(kept != nullptr);
        expectEquals(static_cast<int>(kept->mappingLinkGroups.size()), 1);

        expect(layer.duplicateSkin(sourceId, "Link Copy Drop", false).wasOk());
        const auto* dropped = layer.findSkin(layer.activeSkinId());
        expect(dropped != nullptr);
        expect(dropped->mappings.empty());
        expect(dropped->mappingLinkGroups.empty());
    }

    void assertExclusiveMembership()
    {
        const auto inventory = TSS::DevSkin::allStructuredKeys();
        expect(inventory.size() >= 3);
        const std::set<TSS::SkinColourKey> validKeys(inventory.begin(), inventory.end());

        const auto cleaned = TSS::DevSkin::sanitizeMappingLinkGroups(
            { { inventory[0], inventory[1] }, { inventory[0], inventory[2] } }, validKeys);
        expectEquals(static_cast<int>(cleaned.size()), 1);
        expectEquals(static_cast<int>(cleaned[0].size()), 2);
        expect(std::find(cleaned[0].begin(), cleaned[0].end(), inventory[0]) != cleaned[0].end());
        expect(std::find(cleaned[0].begin(), cleaned[0].end(), inventory[1]) != cleaned[0].end());
        expect(std::find(cleaned[0].begin(), cleaned[0].end(), inventory[2]) == cleaned[0].end());
    }
};

static DevSkinMappingLinkGroupsTests devSkinMappingLinkGroupsTests;
