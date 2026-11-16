#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/SkinZoneRegistry.h"

class DevSkinZoneMapPackJsonTests : public juce::UnitTest
{
public:
    DevSkinZoneMapPackJsonTests()
        : juce::UnitTest("DevSkinZoneMapPackJson")
    {
    }

    void runTest() override
    {
        beginTest("pack_roundTrip_restoresUsedCellZoneMap");
        assertPackRoundTrip();
        beginTest("pack_missingZoneMap_loadsEmptyMap");
        assertMissingZoneMapEmpty();
        beginTest("pack_softLoad_dropsJunkKeepsValid");
        assertSoftLoadDropsJunk();
        beginTest("pack_nonArrayZoneMap_failsImport");
        assertNonArrayFails();
    }

private:
    static TSS::SkinZoneMapKey sliderPatch()
    {
        return { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch };
    }

    static TSS::SkinZoneMapKey sliderMaster()
    {
        return { TSS::SkinObjectType::kSlider, TSS::SkinZone::kMaster };
    }

    void assertPackRoundTrip()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZonePack").wasOk());
        TSS::SkinZoneMap map;
        map[sliderPatch()] = TSS::ColourSet::k2;
        map[sliderMaster()] = TSS::ColourSet::k3;
        expect(layer.setZoneMap(layer.activeSkinId(), map).wasOk());

        TSS::DevSkin::Layer restored;
        const auto imported = TSS::DevSkin::importFromJson(restored, TSS::DevSkin::exportToJson(layer));
        expect(imported.ok);

        const auto* skin = restored.findSkin(layer.activeSkinId());
        expect(skin != nullptr);
        expectEquals(static_cast<int>(skin->zoneMap.size()), 2);
        expect(skin->zoneMap.at(sliderPatch()) == TSS::ColourSet::k2);
        expect(skin->zoneMap.at(sliderMaster()) == TSS::ColourSet::k3);
    }

    void assertMissingZoneMapEmpty()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "LegacyPack").wasOk());
        auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(layer));
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        auto* skins = root->getProperty("skins").getArray();
        expect(skins != nullptr && ! skins->isEmpty());
        auto* skinObj = skins->getReference(0).getDynamicObject();
        expect(skinObj != nullptr);
        expect(! skinObj->hasProperty("zoneMap"));

        TSS::DevSkin::Layer restored;
        expect(TSS::DevSkin::importFromJson(restored, juce::JSON::toString(parsed)).ok);
        const auto* skin = restored.findSkin(layer.activeSkinId());
        expect(skin != nullptr);
        expect(skin->zoneMap.empty());
    }

    static void appendZoneEntry(juce::Array<juce::var>& zoneArr,
                                const char* object,
                                const char* zone,
                                const char* colourSet)
    {
        auto* entry = new juce::DynamicObject();
        entry->setProperty("object", object);
        entry->setProperty("zone", zone);
        entry->setProperty("colourSet", colourSet);
        zoneArr.add(juce::var(entry));
    }

    static void appendSoftLoadJunkEntries(juce::Array<juce::var>& zoneArr)
    {
        zoneArr.add(juce::var("not-an-object-entry")); // soft-skip; keep siblings
        appendZoneEntry(zoneArr, "NotAnObject", "Patch", "cs2");
        appendZoneEntry(zoneArr, "Slider", "NotAZone", "cs2");
        appendZoneEntry(zoneArr, "Slider", "Master", "cs99");
        // ComboBox x Header is unused in the registry — soft-drop.
        appendZoneEntry(zoneArr, "ComboBox", "Header", "cs1");
        // Led only supports cs1..cs2 — cs3 is out of range.
        appendZoneEntry(zoneArr, "Led", "Header", "cs3");
        // Valid sibling kept alongside junk.
        appendZoneEntry(zoneArr, "Slider", "Master", "cs1");
    }

    void assertSoftLoadDropsJunk()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "SoftPack").wasOk());
        TSS::SkinZoneMap map;
        map[sliderPatch()] = TSS::ColourSet::k2;
        expect(layer.setZoneMap(layer.activeSkinId(), map).wasOk());

        auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(layer));
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        auto* skins = root->getProperty("skins").getArray();
        expect(skins != nullptr && ! skins->isEmpty());
        auto* skinObj = skins->getReference(0).getDynamicObject();
        expect(skinObj != nullptr);
        auto* zoneArr = skinObj->getProperty("zoneMap").getArray();
        expect(zoneArr != nullptr);
        appendSoftLoadJunkEntries(*zoneArr);

        TSS::DevSkin::Layer restored;
        expect(TSS::DevSkin::importFromJson(restored, juce::JSON::toString(parsed)).ok);
        const auto* skin = restored.findSkin(layer.activeSkinId());
        expect(skin != nullptr);
        expectEquals(static_cast<int>(skin->zoneMap.size()), 2);
        expect(skin->zoneMap.at(sliderPatch()) == TSS::ColourSet::k2);
        expect(skin->zoneMap.at(sliderMaster()) == TSS::ColourSet::k1);
        expect(skin->zoneMap.count({ TSS::SkinObjectType::kComboBox, TSS::SkinZone::kHeader }) == 0);
        expect(skin->zoneMap.count({ TSS::SkinObjectType::kLed, TSS::SkinZone::kHeader }) == 0);
    }

    void assertNonArrayFails()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "BadShape").wasOk());
        const auto beforeId = layer.activeSkinId();
        const auto beforeCount = layer.skins().size();
        const auto beforeJson = TSS::DevSkin::exportToJson(layer);

        auto parsed = juce::JSON::parse(beforeJson);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        auto* skins = root->getProperty("skins").getArray();
        expect(skins != nullptr && ! skins->isEmpty());
        auto* skinObj = skins->getReference(0).getDynamicObject();
        expect(skinObj != nullptr);
        skinObj->setProperty("zoneMap", juce::var("not-an-array"));

        const auto bad = TSS::DevSkin::importFromJson(layer, juce::JSON::toString(parsed));
        expect(! bad.ok);
        expectEquals(layer.activeSkinId(), beforeId);
        expectEquals(layer.skins().size(), beforeCount);
        expectEquals(TSS::DevSkin::exportToJson(layer), beforeJson);
    }
};

static DevSkinZoneMapPackJsonTests devSkinZoneMapPackJsonTests;
