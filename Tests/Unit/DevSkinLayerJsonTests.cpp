#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Skin.h"

class DevSkinLayerJsonTests : public juce::UnitTest
{
public:
    DevSkinLayerJsonTests()
        : juce::UnitTest("DevSkinLayerJson")
    {
    }

    void runTest() override
    {
        beginTest("duplicate_createsEditableDevSkin_compiledStayReadOnly");
        assertDuplicateAndReadOnly();

        beginTest("json_roundTrip_restoresDevSkinsPaletteAndMappings");
        assertJsonRoundTrip();

        beginTest("malformedJson_refusesImport_keepsPriorLayer");
        assertMalformedImportKeepsLayer();

        beginTest("bindUnbound_rejectsUnknownPastille_andClearsRowOnSuccess");
        assertBindUnbound();

        beginTest("inventory_matchesCompiledSeedMappingCount");
        assertInventoryMatchesSeed();
    }

private:
    void assertDuplicateAndReadOnly()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.isActiveReadOnly());
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Forge Cream").wasOk());
        expect(! layer.isActiveReadOnly());
        expectEquals(layer.findSkin(layer.activeSkinId())->name, juce::String("Forge Cream"));
        expect(layer.setPastilleArgb(TSS::DevSkin::kCompiledCreamId, "kGrey500", 0xff123456).failed());
    }

    void assertJsonRoundTrip()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Forge Cream").wasOk());
        auto* skin = layer.findSkinMutable(layer.activeSkinId());
        expect(skin != nullptr);

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        expect(layer.setMapping(skin->id, key, "kEmerald400").wasOk());
        expect(layer.addPastille(skin->id, "kCustomAccent", 0xffff00aa).wasOk());

        TSS::DevSkin::Layer restored;
        const auto imported = TSS::DevSkin::importFromJson(restored, TSS::DevSkin::exportToJson(layer));
        expect(imported.ok);
        expectEquals(restored.skins().size(), static_cast<size_t>(3));

        const auto* restoredDev = restored.findSkin(skin->id);
        expect(restoredDev != nullptr);
        expectEquals(restoredDev->name, juce::String("Forge Cream"));
        expect(restoredDev->mappings.count(key) == 1);
        expectEquals(restoredDev->mappings.at(key), juce::String("kEmerald400"));

        bool foundCustom = false;
        for (const auto& pastille : restoredDev->palette)
            if (pastille.name == "kCustomAccent" && pastille.argb == 0xffff00aa)
                foundCustom = true;
        expect(foundCustom);
    }

    void assertMalformedImportKeepsLayer()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep Me").wasOk());
        const auto beforeId = layer.activeSkinId();
        const auto beforeCount = layer.skins().size();
        const auto bad = TSS::DevSkin::importFromJson(layer, "{ not json");
        expect(! bad.ok);
        expectEquals(layer.activeSkinId(), beforeId);
        expectEquals(layer.skins().size(), beforeCount);
    }

    void assertBindUnbound()
    {
        TSS::DevSkin::Layer bindLayer;
        expect(bindLayer.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Bind Test").wasOk());
        auto* bindSkin = bindLayer.findSkinMutable(bindLayer.activeSkinId());
        expect(bindSkin != nullptr);

        const auto unboundKey = TSS::skinColourKey(TSS::SkinObjectType::kLed,
                                                   TSS::SkinColourProperty::kBackground,
                                                   TSS::SkinColourState::kOff);
        bindSkin->mappings.erase(unboundKey);
        expect(bindLayer.unboundKeys(bindSkin->id).size() >= 1);
        expect(bindLayer.bindUnbound(bindSkin->id, unboundKey, "no_such_pastille").failed());
        expect(bindLayer.bindUnbound(bindSkin->id, unboundKey, "kGrey500").wasOk());

        for (const auto& keyIt : bindLayer.unboundKeys(bindSkin->id))
            expect(! (keyIt == unboundKey));
    }

    void assertInventoryMatchesSeed()
    {
        TSS::DevSkin::Layer seedLayer;
        const auto* cream = seedLayer.findSkin(TSS::DevSkin::kCompiledCreamId);
        expect(cream != nullptr);

        // Independent SSOT count from compiled Skin maps (not the Dev curated key list alone).
        const auto compiledCream = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        expectEquals(cream->mappings.size(), compiledCream->getMappedColourCount());
        expectEquals(TSS::DevSkin::allStructuredKeys().size(), compiledCream->getMappedColourCount());
    }
};

static DevSkinLayerJsonTests devSkinLayerJsonTests;
