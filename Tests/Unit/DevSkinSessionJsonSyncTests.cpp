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

        TSS::DevSkin::DesignerSessionState state;
        state.jsonSyncMode = TSS::DevSkin::JsonSyncMode::kAuto;

        auto json = TSS::DevSkin::exportSessionToJson(state);
        expect(json.containsIgnoreCase("auto"));

        TSS::DevSkin::DesignerSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restoredState, json).ok);
        expect(restoredState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAuto);

        auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->removeProperty("jsonSyncMode");
        json = juce::JSON::toString(parsed, true);

        TSS::DevSkin::DesignerSessionState legacyState;
        expect(TSS::DevSkin::importSessionFromJson(legacyState, json).ok);
        expect(legacyState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAsk);

        legacyState.jsonSyncMode = TSS::DevSkin::JsonSyncMode::kOff;
        json = TSS::DevSkin::exportSessionToJson(legacyState);
        TSS::DevSkin::DesignerSessionState offState;
        expect(TSS::DevSkin::importSessionFromJson(offState, json).ok);
        expect(offState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kOff);
    }
};

static DevSkinSessionJsonSyncTests devSkinSessionJsonSyncTests;
