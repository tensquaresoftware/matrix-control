#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"

class DevSkinSessionJsonSyncTests : public juce::UnitTest
{
public:
    DevSkinSessionJsonSyncTests()
        : juce::UnitTest("DevSkinSessionJsonSync")
    {
    }

    void runTest() override
    {
        beginTest("JSON Sync mode persists; missing key defaults to Ask");

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Sync Pref").wasOk());

        TSS::DevSkin::EditorSessionState state;
        state.editorSkinId = layer.activeSkinId();
        state.jsonSyncMode = TSS::DevSkin::JsonSyncMode::kAuto;

        auto json = TSS::DevSkin::exportSessionToJson(layer, state);
        expect(json.containsIgnoreCase("auto"));

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restored, restoredState, json).ok);
        expect(restoredState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAuto);

        auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->removeProperty("jsonSyncMode");
        json = juce::JSON::toString(parsed, true);

        TSS::DevSkin::Layer legacy;
        TSS::DevSkin::EditorSessionState legacyState;
        expect(TSS::DevSkin::importSessionFromJson(legacy, legacyState, json).ok);
        expect(legacyState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAsk);

        legacyState.jsonSyncMode = TSS::DevSkin::JsonSyncMode::kOff;
        json = TSS::DevSkin::exportSessionToJson(legacy, legacyState);
        TSS::DevSkin::Layer offRestored;
        TSS::DevSkin::EditorSessionState offState;
        expect(TSS::DevSkin::importSessionFromJson(offRestored, offState, json).ok);
        expect(offState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kOff);
    }
};

static DevSkinSessionJsonSyncTests devSkinSessionJsonSyncTests;
