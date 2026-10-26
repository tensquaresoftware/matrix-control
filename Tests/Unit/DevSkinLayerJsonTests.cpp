#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"
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

        beginTest("exportEditorSkin_writesSingleDevSkinOnly");
        assertSingleEditorSkinExport();

        beginTest("appendImport_addsEditorSkin_withoutWipingSession");
        assertAppendImportKeepsExisting();

        beginTest("deletePastille_rejectsProduct_andClearsDevWithRefs");
        assertDeletePastille();

        beginTest("deletePastille_missingName_leavesMappingsUnchanged");
        assertDeletePastilleMissingLeavesMappings();

        beginTest("exportBlocked_whenEditorSkinHasUnbound");
        assertExportBlockedWhenUnbound();

        beginTest("newEditorSkin_clearsMappings_unboundEqualsInventory");
        assertNewEditorSkinStartsFullyUnbound();

        beginTest("newEditorSkin_mergesCustomPastillesFromCurrentEditor");
        assertNewEditorSkinMergesCustomPastilles();

        beginTest("addPastille_rejectsDuplicateNameOrArgb");
        assertAddPastilleRejectsDuplicateNameOrArgb();

        beginTest("appendImport_remintsDuplicateExport_withDistinctIds");
        assertAppendRemintDistinctIds();

        beginTest("appendImport_multiSkin_addsTwoEditorSkins");
        assertAppendMultiSkinAddsTwo();

        runSessionPersistenceTests();
    }

private:
    void runSessionPersistenceTests()
    {
        beginTest("sessionRoundTrip_restoresEditorSkins_compareAlwaysOff");
        assertSessionRoundTrip();

        beginTest("legacySession_compareOnTrue_loadsWithCompareOff");
        assertLegacyCompareOnIgnored();

        beginTest("corruptSessionJson_restoresCompiledBaseline");
        assertCorruptSessionRestoresBaseline();

        beginTest("comparePush_productSkinPayloadUsesCompiledKind");
        assertComparePushProductPayload();
    }
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

    void assertSingleEditorSkinExport()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Export One").wasOk());
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Export Two").wasOk());
        const auto firstId = layer.skins()[2].id;

        const auto json = TSS::DevSkin::exportEditorSkinToJson(layer, firstId);
        expect(json.isNotEmpty());

        const auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        auto* arr = root->getProperty("skins").getArray();
        expect(arr != nullptr);
        expectEquals(arr->size(), 1);
        expectEquals((*arr)[0].getProperty("name", {}).toString(), juce::String("Export One"));
    }

    void assertAppendImportKeepsExisting()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep Existing").wasOk());
        const auto existingId = layer.activeSkinId();
        const auto beforeCount = layer.skins().size();

        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Imported Skin").wasOk());
        const auto json = TSS::DevSkin::exportEditorSkinToJson(donor, donor.activeSkinId());

        const auto appended = TSS::DevSkin::appendEditorSkinsFromJson(layer, json);
        expect(appended.ok);
        expectEquals(layer.skins().size(), beforeCount + 1);
        expect(layer.findSkin(existingId) != nullptr);
        expect(appended.importedSkinIds.size() == 1);
    }

    bool paletteContains(const TSS::DevSkin::NamedSkin& skin, const juce::String& name) const
    {
        for (const auto& pastille : skin.palette)
            if (pastille.name == name)
                return true;
        return false;
    }

    void assertDeletePastille()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pastille Edit").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.deletePastille(id, "kGrey500", false).failed());
        expect(paletteContains(*layer.findSkin(id), "kGrey500"));

        expect(layer.addPastille(id, "kCustomAccent", 0xffff00aa).wasOk());
        expect(paletteContains(*layer.findSkin(id), "kCustomAccent"));

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        expect(layer.setMapping(id, key, "kCustomAccent").wasOk());
        expect(layer.deletePastille(id, "kCustomAccent", false).failed());
        expect(paletteContains(*layer.findSkin(id), "kCustomAccent"));

        expect(layer.deletePastille(id, "kCustomAccent", true).wasOk());
        expect(! paletteContains(*layer.findSkin(id), "kCustomAccent"));
        expect(layer.countMappingsReferencing(id, "kCustomAccent") == 0);
    }

    void assertDeletePastilleMissingLeavesMappings()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Missing Pastille").wasOk());
        const auto id = layer.activeSkinId();
        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        expect(layer.setMapping(id, key, "kGrey500").wasOk());
        const auto beforeMappings = layer.findSkin(id)->mappings.size();

        expect(layer.deletePastille(id, "kDoesNotExist", true).failed());
        expectEquals(layer.findSkin(id)->mappings.size(), beforeMappings);
        expectEquals(layer.findSkin(id)->mappings.at(key), juce::String("kGrey500"));
    }

    void assertAppendRemintDistinctIds()
    {
        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Remint Me").wasOk());
        const auto json = TSS::DevSkin::exportEditorSkinToJson(donor, donor.activeSkinId());

        TSS::DevSkin::Layer layer;
        const auto first = TSS::DevSkin::appendEditorSkinsFromJson(layer, json);
        expect(first.ok);
        expectEquals(first.importedSkinIds.size(), 1);
        const auto second = TSS::DevSkin::appendEditorSkinsFromJson(layer, json);
        expect(second.ok);
        expectEquals(second.importedSkinIds.size(), 1);
        expect(first.importedSkinIds[0] != second.importedSkinIds[0]);
        expectEquals(layer.skins().size(), static_cast<size_t>(4));
    }

    void assertAppendMultiSkinAddsTwo()
    {
        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Multi A").wasOk());
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Multi B").wasOk());
        const auto json = TSS::DevSkin::exportToJson(donor);

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep").wasOk());
        const auto before = layer.skins().size();
        const auto appended = TSS::DevSkin::appendEditorSkinsFromJson(layer, json);
        expect(appended.ok);
        expectEquals(appended.importedSkinIds.size(), 2);
        expectEquals(layer.skins().size(), before + 2);
    }

    void assertExportBlockedWhenUnbound()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Unbound Gate").wasOk());
        auto* skin = layer.findSkinMutable(layer.activeSkinId());
        expect(skin != nullptr);

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kLed,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        skin->mappings.erase(key);
        expect(layer.unboundCount(skin->id) >= 1);
        expect(! layer.isEditorSkinExportAllowed(skin->id));

        expect(layer.bindUnbound(skin->id, key, "kGrey500").wasOk());
        expectEquals(layer.unboundCount(skin->id), 0);
        expect(layer.isEditorSkinExportAllowed(skin->id));
    }

    void assertNewEditorSkinStartsFullyUnbound()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.createEditorSkinWithUnboundMappings(TSS::DevSkin::kCompiledBlackId,
                                                         "Blank Map").wasOk());
        const auto id = layer.activeSkinId();
        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        expect(skin->mappings.empty());
        expect(skin->palette.size() > 0);
        expectEquals(layer.unboundCount(id),
                     static_cast<int>(TSS::DevSkin::allStructuredKeys().size()));
        expect(! layer.isEditorSkinExportAllowed(id));

        // Duplicate still copies full Product mappings (contrast with New).
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Full Copy").wasOk());
        expectEquals(layer.unboundCount(layer.activeSkinId()), 0);
    }

    void assertNewEditorSkinMergesCustomPastilles()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Donor").wasOk());
        const auto donorId = layer.activeSkinId();
        expect(layer.addPastille(donorId, "kCustomAccent", 0xffff00aa).wasOk());

        expect(layer.createEditorSkinWithUnboundMappings(TSS::DevSkin::kCompiledBlackId,
                                                         "Merged New",
                                                         donorId).wasOk());
        const auto* skin = layer.findSkin(layer.activeSkinId());
        expect(skin != nullptr);
        expect(skin->mappings.empty());

        bool foundCustom = false;
        for (const auto& pastille : skin->palette)
            if (pastille.name == "kCustomAccent" && pastille.argb == 0xffff00aa)
                foundCustom = true;
        expect(foundCustom);
    }

    void assertAddPastilleRejectsDuplicateNameOrArgb()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Dup Colour").wasOk());
        const auto id = layer.activeSkinId();
        constexpr juce::uint32 kUnique = 0xffaa00ff;
        expect(layer.addPastille(id, "kCustomAccent", kUnique).wasOk());
        expect(layer.addPastille(id, "kcustomaccent", 0xff12ab34).failed());
        expect(layer.addPastille(id, "KCUSTOMACCENT", 0xff56cd78).failed());
        // Same ARGB, different name — still a duplicate colour.
        expect(layer.addPastille(id, "kOtherMagenta", kUnique).failed());
        // Product chart colour (kRed500) cannot be re-added under a new name.
        expect(layer.addPastille(id, "kMyRed", 0xffff0000).failed());
    }

    void assertSessionRoundTrip()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Night Work").wasOk());
        const auto editorId = layer.activeSkinId();

        TSS::DevSkin::EditorSessionState state;
        state.productSkinId = TSS::DevSkin::kCompiledBlackId;
        state.editorSkinId = editorId;
        state.compareOn = true;
        state.lastSkinJsonDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                                          .getFullPathName();

        const auto json = TSS::DevSkin::exportSessionToJson(layer, state);
        expect(json.contains("skinEditorSession"));
        expect(json.contains("lastSkinJsonDirectory"));

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        const auto imported = TSS::DevSkin::importSessionFromJson(restored, restoredState, json);
        expect(imported.ok);
        expect(restored.findSkin(editorId) != nullptr);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
        expectEquals(restoredState.editorSkinId, editorId);
        expect(! restoredState.compareOn); // Compare is runtime-only; never restored On
        expect(! json.contains("compareOn"));
        expectEquals(restoredState.lastSkinJsonDirectory, state.lastSkinJsonDirectory);
    }

    void assertLegacyCompareOnIgnored()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Legacy").wasOk());
        const auto editorId = layer.activeSkinId();

        TSS::DevSkin::EditorSessionState seed;
        seed.productSkinId = TSS::DevSkin::kCompiledCreamId;
        seed.editorSkinId = editorId;
        auto json = TSS::DevSkin::exportSessionToJson(layer, seed);
        auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->setProperty("compareOn", true);
        json = juce::JSON::toString(parsed, true);

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        const auto imported = TSS::DevSkin::importSessionFromJson(restored, restoredState, json);
        expect(imported.ok);
        expectEquals(restoredState.editorSkinId, editorId);
        expect(! restoredState.compareOn);
    }

    void assertCorruptSessionRestoresBaseline()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Will Vanish").wasOk());
        TSS::DevSkin::EditorSessionState state;
        state.editorSkinId = layer.activeSkinId();
        state.compareOn = true;

        const auto bad = TSS::DevSkin::importSessionFromJsonOrReset(layer, state, "{ not-json");
        expect(! bad.ok);
        expect(bad.message.containsIgnoreCase("Corrupt session"));
        expectEquals(layer.skins().size(), static_cast<size_t>(2));
        expect(layer.findSkin(TSS::DevSkin::kCompiledBlackId) != nullptr);
        expect(layer.findSkin(TSS::DevSkin::kCompiledCreamId) != nullptr);
        expect(state.editorSkinId.isEmpty());
        expect(! state.compareOn);
    }

    void assertComparePushProductPayload()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Editor Side").wasOk());
        const auto productPayload =
            layer.makePushPayloadForSkin(TSS::DevSkin::kCompiledCreamId);
        expect(productPayload.kind == TSS::DevSkin::SkinKind::kCompiledCream
               || productPayload.kind == TSS::DevSkin::SkinKind::kCompiledBlack);
        expectEquals(productPayload.activeSkinId, juce::String(TSS::DevSkin::kCompiledCreamId));
        expectEquals(productPayload.compiledAncestorId,
                     juce::String(TSS::DevSkin::kCompiledCreamId));

        const auto editorPayload = layer.makePushPayloadForSkin(layer.activeSkinId());
        expect(editorPayload.kind == TSS::DevSkin::SkinKind::kDev);
    }

};

static DevSkinLayerJsonTests devSkinLayerJsonTests;
