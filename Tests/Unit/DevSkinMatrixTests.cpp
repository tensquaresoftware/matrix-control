#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Dev/DevSkinApplier.h"
#include "GUI/Skins/Dev/DevSkinBridgeProtocol.h"
#include "GUI/Skins/Dev/DevSkinBridgeSession.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/Dev/DevSkinStandalonePaintState.h"
#include "GUI/Skins/Skin.h"

class DevSkinMatrixTests : public juce::UnitTest
{
public:
    DevSkinMatrixTests()
        : juce::UnitTest("DevSkinMatrix")
    {
    }

    void runTest() override
    {
        beginTest("happyInvent_duplicateEditPush_appliesToStandalonePaintPath");
        assertHappyInventApply();

        beginTest("offlineEdit_queuesPush_disconnectedStatus_noFakePreview");
        assertOfflineEditQueuesPush();

        beginTest("pushOnConnect_sendsPendingActiveSkin");
        assertPushOnConnect();

        beginTest("unboundBind_rejectsInvalid_thenClearsRow");
        assertUnboundBind();

        beginTest("bcMutation_blockedUntilDuplicate");
        assertBcMutationBlocked();

        beginTest("jsonRoundTrip_andMalformedKeepsLayer");
        assertJsonRoundTripAndMalformed();

        beginTest("devApply_missingHigherSetFallsBackToEditedK1");
        assertDevApplyMissingSetFallback();

        beginTest("disconnect_clearsOverlay_fallsBackToCompiledBc");
        assertDisconnectFallback();

        beginTest("applyError_keepsDesignerLayerUnchanged");
        assertApplyErrorKeepsLayer();

        beginTest("applyPushToSkin_unknownPastille_leavesHigherSetsIntact");
        assertApplyPushUnknownPastilleLeavesHigherSets();

        beginTest("bridgeWire_roundTripEncodeDecodeApply");
        assertBridgeWireRoundTrip();

        beginTest("devFromDev_blackLineage_usesBlackBase");
        assertDevFromDevBlackLineage();

        beginTest("takePushOnConnect_clearsPending");
        assertTakePushOnConnectClearsPending();

        beginTest("jsonImport_rejectsDuplicateIds_andInvalidArgb");
        assertJsonRejectsDuplicatesAndInvalidArgb();
    }

private:
    static TSS::SkinColourKey buttonOffKey()
    {
        return TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                  TSS::SkinColourProperty::kBackground,
                                  TSS::SkinColourState::kOff);
    }

    bool ensureChartPastilleOnSkin(TSS::DevSkin::Layer& layer,
                                   const juce::String& skinId,
                                   const juce::String& name)
    {
        const auto* skin = layer.findSkin(skinId);
        if (skin == nullptr)
            return false;
        for (const auto& pastille : skin->palette)
            if (pastille.name == name)
                return true;
        for (const auto& pastille : TSS::DevSkin::colourChartPastilles())
        {
            if (pastille.name != name)
                continue;
            return layer.addPastille(skinId, name, pastille.argb).wasOk();
        }
        return false;
    }

    void assertHappyInventApply()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Invent A").wasOk());
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kGreen300"));
        expect(layer.setMapping(layer.activeSkinId(), buttonOffKey(), "kGreen300").wasOk());

        const auto payload = layer.makePushPayload();
        expect(payload.kind == TSS::DevSkin::SkinKind::kDev);

        TSS::DevSkin::StandalonePaintState paint;
        const auto before = paint.activeSkin().getColour(buttonOffKey());
        const auto result = paint.applyPush(payload);
        expect(result.ok);
        expect(paint.hasDevOverlay());

        const auto after = paint.activeSkin().getColour(buttonOffKey());
        expect(after == juce::Colour(ColourChart::kGreen300));
        expect(after != before);
    }

    void assertOfflineEditQueuesPush()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Offline").wasOk());
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kRed500"));
        expect(layer.setMapping(layer.activeSkinId(), buttonOffKey(), "kRed500").wasOk());

        TSS::DevSkin::BridgeSession session;
        expect(! session.isConnected());
        expect(session.connectionStatusText().containsIgnoreCase("disconnected"));
        expect(! session.connectionStatusText().containsIgnoreCase("offline"));

        const auto payload = layer.makePushPayload();
        const auto outbound = session.queuePush(payload);
        expect(! outbound.has_value());
        expect(session.hasPendingPush());
        expect(session.pendingPush() != nullptr);
        expectEquals(session.pendingPush()->activeSkinId, payload.activeSkinId);

        // Designer layer remains editable offline.
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kGrey500"));
        expect(layer.setPastilleArgb(layer.activeSkinId(), "kGrey500", 0xff112233).wasOk());
    }

    void assertPushOnConnect()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "OnConnect").wasOk());
        const auto payload = layer.makePushPayload();

        TSS::DevSkin::BridgeSession session;
        expect(! session.queuePush(payload).has_value());
        session.markConnected();
        const auto onConnect = session.takePushOnConnect();
        expect(onConnect.has_value());
        expectEquals(onConnect->activeSkinId, payload.activeSkinId);
        expect(onConnect->kind == TSS::DevSkin::SkinKind::kDev);
        expect(! session.hasPendingPush());

        // Connected push sends immediately.
        auto next = payload;
        next.revision = payload.revision + 1;
        const auto immediate = session.queuePush(next);
        expect(immediate.has_value());
        expectEquals(immediate->revision, next.revision);
    }

    void assertUnboundBind()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Unbound").wasOk());
        auto* skin = layer.findSkinMutable(layer.activeSkinId());
        expect(skin != nullptr);

        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kLed,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        skin->mappings.erase(key);
        expect(layer.unboundKeys(skin->id).size() >= 1);
        expect(layer.bindUnbound(skin->id, key, "missing_pastille").failed());
        expect(ensureChartPastilleOnSkin(layer, skin->id, "kGrey500"));
        expect(layer.bindUnbound(skin->id, key, "kGrey500").wasOk());

        for (const auto& unbound : layer.unboundKeys(skin->id))
            expect(! (unbound == key));

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(layer.makePushPayload()).ok);
        expect(paint.activeSkin().getColour(key) == juce::Colour(ColourChart::kGrey500));
    }

    void assertBcMutationBlocked()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.selectSkin(TSS::DevSkin::kCompiledBlackId));
        expect(layer.isActiveReadOnly());
        expect(layer.setMapping(TSS::DevSkin::kCompiledBlackId, buttonOffKey(), "kRed500").failed());
        expect(layer.setPastilleArgb(TSS::DevSkin::kCompiledCreamId, "kGrey500", 0xff000001).failed());
        expect(layer.addPastille(TSS::DevSkin::kCompiledCreamId, "kX", 0xffffffff).failed());
        expect(layer.deleteDevSkin(TSS::DevSkin::kCompiledBlackId).failed());
    }

    void assertJsonRoundTripAndMalformed()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "RoundTrip").wasOk());
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kBrandBlue"));
        expect(layer.setMapping(layer.activeSkinId(), buttonOffKey(), "kBrandBlue").wasOk());
        const auto json = TSS::DevSkin::exportToJson(layer);
        const auto activeId = layer.activeSkinId();

        TSS::DevSkin::Layer restored;
        expect(TSS::DevSkin::importFromJson(restored, json).ok);
        expect(restored.findSkin(activeId) != nullptr);
        expectEquals(restored.findSkin(activeId)->mappings.at(buttonOffKey()), juce::String("kBrandBlue"));

        const auto beforeCount = restored.skins().size();
        expect(! TSS::DevSkin::importFromJson(restored, "not-json{").ok);
        expectEquals(restored.skins().size(), beforeCount);
    }

    void assertDevApplyMissingSetFallback()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Legacy Sets").wasOk());
        auto* skin = layer.findSkinMutable(layer.activeSkinId());
        expect(skin != nullptr);
        if (skin == nullptr)
            return;

        const auto primary = buttonOffKey();
        expect(ensureChartPastilleOnSkin(layer, skin->id, "kGreen300"));
        expect(layer.setMapping(skin->id, primary, "kGreen300").wasOk());
        for (const auto colourSet : { TSS::ColourSet::k2,
                                      TSS::ColourSet::k3,
                                      TSS::ColourSet::k4 })
        {
            auto key = primary;
            key.colourSet = colourSet;
            skin->mappings.erase(key);
        }

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(layer.makePushPayload()).ok);
        auto requested = primary;
        requested.colourSet = TSS::ColourSet::k3;
        expect(paint.activeSkin().getColour(requested) == juce::Colour(ColourChart::kGreen300));
    }

    void assertDisconnectFallback()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Live").wasOk());
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kGreen300"));
        expect(layer.setMapping(layer.activeSkinId(), buttonOffKey(), "kGreen300").wasOk());

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(layer.makePushPayload()).ok);
        expect(paint.hasDevOverlay());
        expect(paint.activeSkin().getColour(buttonOffKey()) == juce::Colour(ColourChart::kGreen300));

        const auto creamReferenceSkin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto creamReference = creamReferenceSkin->getColour(buttonOffKey());

        paint.clearOverlayToCompiled();
        expect(! paint.hasDevOverlay());
        expect(paint.compiledSelection() == TSS::Skin::ColourVariant::Cream);
        expect(paint.activeSkin().getColour(buttonOffKey()) == creamReference);

        // Designer keeps its Dev layer after Standalone disconnect.
        expect(layer.findSkin(layer.activeSkinId()) != nullptr);
        expectEquals(layer.findSkin(layer.activeSkinId())->mappings.at(buttonOffKey()),
                     juce::String("kGreen300"));

        TSS::DevSkin::BridgeSession session;
        session.markConnected();
        session.markDisconnected();
        expect(! session.isConnected());
        expect(session.connectionStatusText().containsIgnoreCase("disconnected"));
    }

    void assertApplyErrorKeepsLayer()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep").wasOk());
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kGreen300"));
        expect(layer.setMapping(layer.activeSkinId(), buttonOffKey(), "kGreen300").wasOk());
        const auto jsonBefore = TSS::DevSkin::exportToJson(layer);

        auto payload = layer.makePushPayload();
        payload.mappings[buttonOffKey()] = "totally_unknown_pastille";

        TSS::DevSkin::StandalonePaintState paint;
        expect(! paint.hasDevOverlay());
        const auto result = paint.applyPush(payload);
        expect(! result.ok);
        expectEquals(result.code, juce::String("unknown_pastille"));
        expect(! paint.hasDevOverlay());

        expectEquals(TSS::DevSkin::exportToJson(layer), jsonBefore);
    }

    void assertApplyPushUnknownPastilleLeavesHigherSets()
    {
        auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        auto higherKey = buttonOffKey();
        higherKey.colourSet = TSS::ColourSet::k2;
        const auto marker = juce::Colour(ColourChart::kRed500);
        skin->setColour(higherKey, marker);

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Atomic").wasOk());
        auto payload = layer.makePushPayload();
        payload.mappings[buttonOffKey()] = "totally_unknown_pastille";

        const auto result = TSS::DevSkin::applyPushToSkin(payload, *skin);
        expect(! result.ok);
        expectEquals(result.code, juce::String("unknown_pastille"));
        expect(skin->getColour(higherKey) == marker);
    }

    void assertBridgeWireRoundTrip()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Wire").wasOk());
        expect(ensureChartPastilleOnSkin(layer, layer.activeSkinId(), "kGreen300"));
        expect(layer.setMapping(layer.activeSkinId(), buttonOffKey(), "kGreen300").wasOk());

        const auto original = layer.makePushPayload();
        const auto encoded = TSS::DevSkin::encodeMessage(
            juce::JSON::toString(TSS::DevSkin::pushPayloadToVar(original)));

        juce::String decodedJson;
        expect(TSS::DevSkin::decodeMessage(encoded, decodedJson));

        TSS::DevSkin::PushPayload restored;
        juce::String error;
        expect(TSS::DevSkin::pushPayloadFromVar(juce::JSON::parse(decodedJson), restored, error));
        expectEquals(restored.activeSkinId, original.activeSkinId);
        expectEquals(restored.compiledAncestorId, juce::String(TSS::DevSkin::kCompiledCreamId));

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(restored).ok);
        expect(paint.activeSkin().getColour(buttonOffKey()) == juce::Colour(ColourChart::kGreen300));

        juce::MemoryBlock oversized;
        oversized.append(TSS::DevSkin::kBridgeMagic, 4);
        juce::uint32 huge = juce::ByteOrder::swapIfBigEndian(TSS::DevSkin::kBridgeMaxPayloadBytes + 1);
        oversized.append(&huge, sizeof(huge));
        juce::String ignored;
        expect(! TSS::DevSkin::decodeMessage(oversized, ignored));
    }

    void assertDevFromDevBlackLineage()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Child").wasOk());
        const auto childId = layer.activeSkinId();
        expect(layer.duplicateSkin(childId, "Grandchild").wasOk());

        const auto payload = layer.makePushPayload();
        expectEquals(payload.compiledAncestorId, juce::String(TSS::DevSkin::kCompiledBlackId));
        expect(TSS::DevSkin::compiledBaseVariant(payload) == TSS::Skin::ColourVariant::Black);
    }

    void assertTakePushOnConnectClearsPending()
    {
        TSS::DevSkin::BridgeSession session;
        TSS::DevSkin::PushPayload payload;
        payload.activeSkinId = "dev:x";
        payload.revision = 3;
        expect(! session.queuePush(payload).has_value());
        expect(session.hasPendingPush());
        session.markConnected();
        expect(session.takePushOnConnect().has_value());
        expect(! session.hasPendingPush());
        expect(! session.takePushOnConnect().has_value());
    }

    void assertJsonRejectsDuplicatesAndInvalidArgb()
    {
        const juce::String duplicateJson = R"({
          "schemaVersion": 1,
          "activeSkinId": "dev:a",
          "skins": [
            { "id": "dev:a", "name": "A", "kind": "dev", "palette": [], "mappings": [] },
            { "id": "dev:a", "name": "A2", "kind": "dev", "palette": [], "mappings": [] }
          ]
        })";
        TSS::DevSkin::Layer layer;
        expect(! TSS::DevSkin::importFromJson(layer, duplicateJson).ok);

        const juce::String badArgb = R"({
          "schemaVersion": 1,
          "activeSkinId": "dev:b",
          "skins": [
            {
              "id": "dev:b",
              "name": "B",
              "kind": "dev",
              "palette": [ { "name": "kBad", "argb": "GGHHII" } ],
              "mappings": []
            }
          ]
        })";
        expect(! TSS::DevSkin::importFromJson(layer, badArgb).ok);
    }
};

static DevSkinMatrixTests devSkinMatrixTests;
