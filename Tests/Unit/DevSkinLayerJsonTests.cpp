#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
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
        runLayerJsonSuite();
        runComparePushSuite();
    }

private:
    void runLayerJsonSuite()
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
        beginTest("exportDesignerSkin_writesSingleDevSkinOnly");
        assertSingleDesignerSkinExport();
        beginTest("appendImport_addsDesignerSkin_withoutWipingSession");
        assertAppendImportKeepsExisting();
        beginTest("deletePastille_allowsFormerChartName_andClearsDevWithRefs");
        assertDeletePastille();
        beginTest("compiledSkin_paletteIsSlimNotFullChart");
        assertCompiledPaletteSlim();
        beginTest("twoDevSkins_samePastilleName_independentArgb");
        assertSamePastilleNameIndependentArgb();
        beginTest("deletePastille_missingName_leavesMappingsUnchanged");
        assertDeletePastilleMissingLeavesMappings();
        beginTest("exportBlocked_whenDesignerSkinHasUnbound");
        assertExportBlockedWhenUnbound();
        beginTest("addPastille_rejectsDuplicateName_allowsSameArgb");
        assertAddPastilleRejectsDuplicateNameAllowsSameArgb();
        beginTest("appendImport_remintsDuplicateExport_withDistinctIds");
        assertAppendRemintDistinctIds();
        beginTest("appendImport_multiSkin_addsTwoDesignerSkins");
        assertAppendMultiSkinAddsTwo();
    }

    void runComparePushSuite()
    {
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

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kSlider,
                                            TSS::SkinColourProperty::kTrack,
                                            TSS::SkinColourState::kDefault,
                                            TSS::ColourSet::k3);
        expect(ensureChartPastilleOnSkin(layer, skin->id, "kGreen300"));
        expect(layer.setMapping(skin->id, key, "kGreen300").wasOk());
        expect(layer.addPastille(skin->id, "kCustomAccent", 0xffff00aa).wasOk());

        TSS::DevSkin::Layer restored;
        const auto imported = TSS::DevSkin::importFromJson(restored, TSS::DevSkin::exportToJson(layer));
        expect(imported.ok);
        expectEquals(restored.skins().size(), static_cast<size_t>(7));

        const auto* restoredDev = restored.findSkin(skin->id);
        expect(restoredDev != nullptr);
        expectEquals(restoredDev->name, juce::String("Forge Cream"));
        expect(restoredDev->mappings.count(key) == 1);
        expectEquals(restoredDev->mappings.at(key), juce::String("kGreen300"));

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
        expect(ensureChartPastilleOnSkin(bindLayer, bindSkin->id, "kGrey500"));
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

    void assertSingleDesignerSkinExport()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Export One").wasOk());
        const auto firstId = layer.activeSkinId();
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Export Two").wasOk());

        const auto json = TSS::DevSkin::exportDesignerSkinToJson(layer, firstId);
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
        const auto json = TSS::DevSkin::exportDesignerSkinToJson(donor, donor.activeSkinId());

        const auto appended = TSS::DevSkin::appendDesignerSkinsFromJson(layer, json);
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

    bool ensureChartPastilleOnSkin(TSS::DevSkin::Layer& layer,
                                   const juce::String& skinId,
                                   const juce::String& name) const
    {
        const auto* skin = layer.findSkin(skinId);
        if (skin == nullptr)
            return false;
        if (paletteContains(*skin, name))
            return true;
        for (const auto& pastille : TSS::DevSkin::colourChartPastilles())
        {
            if (pastille.name != name)
                continue;
            return layer.addPastille(skinId, name, pastille.argb).wasOk();
        }
        return false;
    }

    void assertCompiledPaletteSlim()
    {
        TSS::DevSkin::Layer layer;
        const auto* cream = layer.findSkin(TSS::DevSkin::kCompiledCreamId);
        expect(cream != nullptr);
        const auto chartSize = static_cast<int>(TSS::DevSkin::colourChartPastilles().size());
        expect(static_cast<int>(cream->palette.size()) < chartSize);
        expect(cream->palette.size() > 0);
        expect(cream->palette.size() <= cream->mappings.size());
    }

    void assertSamePastilleNameIndependentArgb()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Skin A").wasOk());
        const auto idA = layer.activeSkinId();
        if (! paletteContains(*layer.findSkin(idA), "kGreen400"))
            expect(layer.addPastille(idA, "kGreen400", 0xff111111).wasOk());
        else
            expect(layer.setPastilleArgb(idA, "kGreen400", 0xff111111).wasOk());

        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Skin B").wasOk());
        const auto idB = layer.activeSkinId();
        if (! paletteContains(*layer.findSkin(idB), "kGreen400"))
            expect(layer.addPastille(idB, "kGreen400", 0xff222222).wasOk());
        else
            expect(layer.setPastilleArgb(idB, "kGreen400", 0xff222222).wasOk());

        expectEquals(static_cast<juce::int64>(*TSS::DevSkin::argbForPastilleName(
                         layer.findSkin(idA)->palette, "kGreen400")),
                     static_cast<juce::int64>(0xff111111));
        expectEquals(static_cast<juce::int64>(*TSS::DevSkin::argbForPastilleName(
                         layer.findSkin(idB)->palette, "kGreen400")),
                     static_cast<juce::int64>(0xff222222));
    }

    void assertDeletePastille()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pastille Edit").wasOk());
        const auto id = layer.activeSkinId();
        // Former ColourChart name, unmapped: deletable (no product lock).
        expect(ensureChartPastilleOnSkin(layer, id, "kGreen100"));
        // Ensure it is not mapped.
        expect(layer.countMappingsReferencing(id, "kGreen100") == 0
               || layer.deletePastille(id, "kGreen100", true).wasOk());
        if (! paletteContains(*layer.findSkin(id), "kGreen100"))
            expect(ensureChartPastilleOnSkin(layer, id, "kGreen100"));
        expect(layer.countMappingsReferencing(id, "kGreen100") == 0);
        expect(layer.deletePastille(id, "kGreen100", false).wasOk());
        expect(! paletteContains(*layer.findSkin(id), "kGreen100"));

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
        expect(layer.addPastille(id, "kProbeGrey", 0xff606060).wasOk());
        expect(layer.setMapping(id, key, "kProbeGrey").wasOk());
        const auto beforeMappings = layer.findSkin(id)->mappings.size();

        expect(layer.deletePastille(id, "kDoesNotExist", true).failed());
        expectEquals(layer.findSkin(id)->mappings.size(), beforeMappings);
        expectEquals(layer.findSkin(id)->mappings.at(key), juce::String("kProbeGrey"));
    }

    void assertAppendRemintDistinctIds()
    {
        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Remint Me").wasOk());
        const auto json = TSS::DevSkin::exportDesignerSkinToJson(donor, donor.activeSkinId());

        TSS::DevSkin::Layer layer;
        const auto first = TSS::DevSkin::appendDesignerSkinsFromJson(layer, json);
        expect(first.ok);
        expectEquals(first.importedSkinIds.size(), 1);
        const auto second = TSS::DevSkin::appendDesignerSkinsFromJson(layer, json);
        expect(second.ok);
        expectEquals(second.importedSkinIds.size(), 1);
        expect(first.importedSkinIds[0] != second.importedSkinIds[0]);
        expectEquals(layer.skins().size(), static_cast<size_t>(8));
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
        const auto appended = TSS::DevSkin::appendDesignerSkinsFromJson(layer, json);
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
        expect(! layer.isDesignerSkinExportAllowed(skin->id));

        expect(ensureChartPastilleOnSkin(layer, skin->id, "kGrey500"));
        expect(layer.bindUnbound(skin->id, key, "kGrey500").wasOk());
        expectEquals(layer.unboundCount(skin->id), 0);
        expect(layer.isDesignerSkinExportAllowed(skin->id));
    }

    void assertAddPastilleRejectsDuplicateNameAllowsSameArgb()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Dup Colour").wasOk());
        const auto id = layer.activeSkinId();
        constexpr juce::uint32 kUnique = 0xffaa00ff;
        expect(layer.addPastille(id, "kCustomAccent", kUnique).wasOk());
        expect(layer.addPastille(id, "kcustomaccent", 0xff12ab34).failed());
        expect(layer.addPastille(id, "KCUSTOMACCENT", 0xff56cd78).failed());
        // Same ARGB, different name — palette twins are allowed.
        expect(layer.addPastille(id, "kOtherMagenta", kUnique).wasOk());
        // Product chart colour ARGB under a new Designer name is allowed.
        expect(layer.addPastille(id, "kMyRed", 0xffff0000).wasOk());
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
