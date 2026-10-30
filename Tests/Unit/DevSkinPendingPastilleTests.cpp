#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"
#include "Tools/SkinEditor/SkinEditorUiHelpers.h"

class DevSkinPendingPastilleTests : public juce::UnitTest
{
public:
    DevSkinPendingPastilleTests()
        : juce::UnitTest("DevSkinPendingPastille")
    {
    }

    void runTest() override
    {
        beginTest("helpers_composeUniqueTempNameAndOpaqueArgb");
        assertHelpersComposeUniqueSeed();

        beginTest("helpers_suggestUniquePastilleNameStemAndProductClone");
        assertSuggestUniquePastilleNameStemAndProductClone();

        beginTest("addPastille_pendingTrue_persistsUntilArgbChanges");
        assertPendingClearsOnlyOnArgbChange();

        beginTest("setPastilleArgb_sameValue_leavesPendingAndRevision");
        assertSameArgbIsNoOp();

        beginTest("renamePastille_keepsPending_andRewritesMappings");
        assertRenameKeepsPendingAndMappings();

        beginTest("json_roundTrip_pending");
        assertJsonPendingRoundTrip();

        beginTest("json_legacyMissingPending_defaultsFalse");
        assertLegacyJsonPendingDefaultsFalse();

        beginTest("colourChartAndDuplicate_seedPendingFalse");
        assertProductSeedsNotPending();

        beginTest("setPastilleArgb_andAdd_allowSameArgbTwins");
        assertSameArgbAllowedAcrossPastilles();

        beginTest("duplicatePastille_sameArgb_notPending_uniqueStem");
        assertDuplicatePastilleOutcome();

        beginTest("sameArgbMappedUnderOtherName_blocksSecondBind");
        assertSameArgbMappedCollisionProbe();

        beginTest("renamePastille_rejectsEmptyCollisionAndProductName");
        assertRenameRejectsEmptyCollisionAndProduct();

        beginTest("addPastille_threeArgDefaultsPendingFalse");
        assertThreeArgAddPendingFalse();
    }

private:
    void assertHelpersComposeUniqueSeed()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);

        const auto name1 = SkinEditorUi::suggestUniqueTempPastilleName(*skin);
        expectEquals(name1, juce::String("Temp 1"));
        const auto argb1 = SkinEditorUi::suggestUniqueOpaqueArgb(*skin);
        expect((argb1 & 0xff000000u) == 0xff000000u);
        expect(layer.addPastille(id, name1, argb1, true).wasOk());

        skin = layer.findSkin(id);
        expect(skin != nullptr);
        expectEquals(SkinEditorUi::suggestUniquePastilleName(*skin, "Temp 1"),
                     juce::String("Temp 1 2"));
        const auto name2 = SkinEditorUi::suggestUniqueTempPastilleName(*skin);
        expectEquals(name2, juce::String("Temp 2"));
        const auto argb2 = SkinEditorUi::suggestUniqueOpaqueArgb(*skin);
        expect(argb2 != argb1);
        expect((argb2 & 0xff000000u) == 0xff000000u);
        expect(layer.addPastille(id, name2, argb2, true).wasOk());

        skin = layer.findSkin(id);
        expect(skin != nullptr);
        int pendingCount = 0;
        for (const auto& pastille : skin->palette)
            if (pastille.pending)
                ++pendingCount;
        expectEquals(pendingCount, 2);
    }

    void assertSuggestUniquePastilleNameStemAndProductClone()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Editor Accent", 0xffabcdef, false).wasOk());
        expect(layer.addPastille(id, "Editor Accent 2", 0xfffedcba, false).wasOk());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        expectEquals(SkinEditorUi::suggestUniquePastilleName(*skin, "Editor Accent"),
                     juce::String("Editor Accent 3"));

        juce::String productName;
        juce::uint32 productArgb = 0;
        for (const auto& pastille : skin->palette)
        {
            if (! layer.isProductPastilleName(pastille.name))
                continue;
            productName = pastille.name;
            productArgb = pastille.argb;
            break;
        }
        expect(productName.isNotEmpty());
        const auto cloneName = SkinEditorUi::suggestUniquePastilleName(*skin, productName);
        expect(cloneName.isNotEmpty());
        expect(! layer.isProductPastilleName(cloneName));
        expect(layer.addPastille(id, cloneName, productArgb, false).wasOk());

        skin = layer.findSkin(id);
        expect(skin != nullptr);
        bool foundClone = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name != cloneName)
                continue;
            expectEquals(static_cast<juce::int64>(pastille.argb),
                         static_cast<juce::int64>(productArgb));
            expect(! pastille.pending);
            foundClone = true;
        }
        expect(foundClone);
    }

    void assertPendingClearsOnlyOnArgbChange()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Temp 1", 0xff112233, true).wasOk());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        bool foundPending = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == "Temp 1")
            {
                expect(pastille.pending);
                foundPending = true;
            }
        }
        expect(foundPending);

        expect(layer.setPastilleArgb(id, "Temp 1", 0xff445566).wasOk());
        skin = layer.findSkin(id);
        expect(skin != nullptr);
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == "Temp 1")
            {
                expect(! pastille.pending);
                expectEquals(static_cast<juce::int64>(pastille.argb),
                             static_cast<juce::int64>(0xff445566));
            }
        }
    }

    void assertSameArgbIsNoOp()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Temp 1", 0xff112233, true).wasOk());
        const auto revisionAfterAdd = layer.revision();

        expect(layer.setPastilleArgb(id, "Temp 1", 0xff112233).wasOk());
        expectEquals(layer.revision(), revisionAfterAdd);

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        for (const auto& pastille : skin->palette)
            if (pastille.name == "Temp 1")
                expect(pastille.pending);
    }

    void assertRenameKeepsPendingAndMappings()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Temp 1", 0xff112233, true).wasOk());

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        expect(layer.setMapping(id, key, "Temp 1").wasOk());

        expect(layer.renamePastille(id, "Temp 1", "Accent Draft").wasOk());
        expect(layer.renamePastille(id, "kGrey500", "Nope").failed());
        expect(layer.renamePastille(TSS::DevSkin::kCompiledCreamId, "kGrey500", "Nope").failed());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        expect(skin->mappings.at(key) == "Accent Draft");

        bool found = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == "Accent Draft")
            {
                expect(pastille.pending);
                found = true;
            }
            expect(pastille.name != "Temp 1");
        }
        expect(found);
    }

    void assertJsonPendingRoundTrip()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Temp 1", 0xff112233, true).wasOk());
        expect(layer.addPastille(id, "Calibrated", 0xffabcdef, false).wasOk());

        TSS::DevSkin::Layer restored;
        const auto imported = TSS::DevSkin::importFromJson(restored, TSS::DevSkin::exportToJson(layer));
        expect(imported.ok);
        const auto* skin = restored.findSkin(id);
        expect(skin != nullptr);
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == "Temp 1")
                expect(pastille.pending);
            if (pastille.name == "Calibrated")
                expect(! pastille.pending);
        }
    }

    void assertLegacyJsonPendingDefaultsFalse()
    {
        const juce::String legacy = R"({
            "schemaVersion": 1,
            "activeSkinId": "dev:legacy",
            "skins": [{
                "id": "dev:legacy",
                "name": "Legacy",
                "kind": "dev",
                "derivedFrom": "compiled:cream",
                "palette": [
                    { "name": "kGrey500", "argb": "0xFF9E9E9E" },
                    { "name": "OldCustom", "argb": "0xFFAA00FF" }
                ],
                "mappings": []
            }]
        })";
        TSS::DevSkin::Layer legacyLayer;
        const auto legacyImport = TSS::DevSkin::importFromJson(legacyLayer, legacy);
        expect(legacyImport.ok);
        const auto* legacySkin = legacyLayer.findSkin("dev:legacy");
        expect(legacySkin != nullptr);
        for (const auto& pastille : legacySkin->palette)
            expect(! pastille.pending);
    }

    void assertProductSeedsNotPending()
    {
        TSS::DevSkin::Layer layer;
        for (const auto& skin : layer.skins())
        {
            if (skin.id != TSS::DevSkin::kCompiledBlackId
                && skin.id != TSS::DevSkin::kCompiledCreamId)
                continue;
            for (const auto& pastille : skin.palette)
                expect(! pastille.pending);
        }

        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Dup Cream").wasOk());
        const auto* dup = layer.findSkin(layer.activeSkinId());
        expect(dup != nullptr);
        for (const auto& pastille : dup->palette)
            expect(! pastille.pending);
    }

    void assertSameArgbAllowedAcrossPastilles()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Temp 1", 0xff112233, true).wasOk());
        expect(layer.addPastille(id, "Temp 2", 0xff445566, true).wasOk());

        expect(layer.setPastilleArgb(id, "Temp 1", 0xff445566).wasOk());
        expect(layer.addPastille(id, "Temp 3", 0xff445566, false).wasOk());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        int twinCount = 0;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.argb != 0xff445566)
                continue;
            ++twinCount;
            if (pastille.name == "Temp 1")
                expect(! pastille.pending);
        }
        expectEquals(twinCount, 3);
    }

    void assertDuplicatePastilleOutcome()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Dup Lab").wasOk());
        const auto id = layer.activeSkinId();
        constexpr juce::uint32 kSourceArgb = 0xff102030;
        expect(layer.addPastille(id, "Editor Accent", kSourceArgb, false).wasOk());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        const auto copyName = SkinEditorUi::suggestUniquePastilleName(*skin, "Editor Accent");
        expectEquals(copyName, juce::String("Editor Accent 2"));
        expect(layer.addPastille(id, copyName, kSourceArgb, false).wasOk());

        skin = layer.findSkin(id);
        expect(skin != nullptr);
        bool foundCopy = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name != copyName)
                continue;
            foundCopy = true;
            expectEquals(static_cast<juce::int64>(pastille.argb),
                         static_cast<juce::int64>(kSourceArgb));
            expect(! pastille.pending);
            expect(! layer.isProductPastilleName(pastille.name));
        }
        expect(foundCopy);
        // Neither twin mapped yet — first bind is still allowed.
        expect(! SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, copyName));
        expect(! SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Editor Accent"));
    }

    void assertSameArgbMappedCollisionProbe()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Collision Lab").wasOk());
        const auto id = layer.activeSkinId();
        constexpr juce::uint32 kTwin = 0xffabcdef;
        expect(layer.addPastille(id, "Accent A", kTwin, false).wasOk());
        expect(layer.addPastille(id, "Accent B", kTwin, false).wasOk());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        expect(! SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent A"));
        expect(! SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent B"));

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        expect(layer.setMapping(id, key, "Accent A").wasOk());

        skin = layer.findSkin(id);
        expect(skin != nullptr);
        expect(! SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent A"));
        expect(SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent B"));
        expect(SkinEditorMappingPolicy::canBindPastille(
            false, SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent A")));
        expect(! SkinEditorMappingPolicy::canBindPastille(
            false, SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent B")));

        expect(layer.setPastilleArgb(id, "Accent B", 0xff010203).wasOk());
        skin = layer.findSkin(id);
        expect(skin != nullptr);
        expect(! SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent B"));
        expect(SkinEditorMappingPolicy::canBindPastille(
            false, SkinEditorUi::isPastilleArgbMappedUnderOtherName(*skin, "Accent B")));
    }

    void assertRenameRejectsEmptyCollisionAndProduct()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Temp 1", 0xff112233, true).wasOk());
        expect(layer.addPastille(id, "Temp 2", 0xff445566, true).wasOk());
        const auto revisionBefore = layer.revision();

        expect(layer.renamePastille(id, "Temp 1", {}).failed());
        expect(layer.renamePastille(id, "Temp 1", "   ").failed());
        expect(layer.renamePastille(id, "Temp 1", "temp 2").failed());
        expect(layer.renamePastille(id, "Temp 1", "kGrey500").failed());
        expectEquals(layer.revision(), revisionBefore);

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        bool foundTemp1 = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == "Temp 1")
                foundTemp1 = true;
            expect(pastille.name != "temp 2");
        }
        expect(foundTemp1);
    }

    void assertThreeArgAddPendingFalse()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Pending Lab").wasOk());
        const auto id = layer.activeSkinId();
        expect(layer.addPastille(id, "Calibrated", 0xffabcdef).wasOk());

        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        for (const auto& pastille : skin->palette)
            if (pastille.name == "Calibrated")
                expect(! pastille.pending);
    }
};

static DevSkinPendingPastilleTests devSkinPendingPastilleTests;
