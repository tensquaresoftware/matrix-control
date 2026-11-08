#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"

class DevSkinSessionImportWatchTests : public juce::UnitTest
{
public:
    DevSkinSessionImportWatchTests()
        : juce::UnitTest("DevSkinSessionImportWatch")
    {
    }

    void runTest() override
    {
        beginTest("sessionRoundTrip_restoresLastImportedJsonWatchFields");
        assertSessionRoundTripRestoresImportWatch();

        beginTest("sessionImport_missingLastImportedPath_clearsWatchFields");
        assertSessionImportClearsMissingImportPath();
    }

private:
    void assertSessionRoundTripRestoresImportWatch()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Watch Me").wasOk());
        const auto editorId = layer.activeSkinId();

        juce::TemporaryFile temp("skin-editor-import-watch.json");
        expect(temp.getFile().replaceWithText("{ \"skins\": [] }"));

        TSS::DevSkin::EditorSessionState state;
        state.productSkinId = TSS::DevSkin::kCompiledBlackId;
        state.editorSkinId = editorId;
        state.lastImportedJsonPath = temp.getFile().getFullPathName();
        state.lastImportedJsonFingerprint = "42|123456789";

        const auto json = TSS::DevSkin::exportSessionToJson(layer, state);
        expect(json.contains("lastImportedJsonPath"));
        expect(json.contains("lastImportedJsonFingerprint"));

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        const auto imported = TSS::DevSkin::importSessionFromJson(restored, restoredState, json);
        expect(imported.ok);
        expectEquals(restoredState.lastImportedJsonPath, state.lastImportedJsonPath);
        expectEquals(restoredState.lastImportedJsonFingerprint, state.lastImportedJsonFingerprint);
    }

    void assertSessionImportClearsMissingImportPath()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Gone Path").wasOk());
        const auto editorId = layer.activeSkinId();

        TSS::DevSkin::EditorSessionState seed;
        seed.productSkinId = TSS::DevSkin::kCompiledBlackId;
        seed.editorSkinId = editorId;
        seed.lastImportedJsonPath = "/tmp/matrix-control-missing-import-watch.json";
        seed.lastImportedJsonFingerprint = "1|999";

        auto json = TSS::DevSkin::exportSessionToJson(layer, seed);
        // Force a path that does not exist on disk into the session document.
        auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->setProperty("lastImportedJsonPath", seed.lastImportedJsonPath);
        root->setProperty("lastImportedJsonFingerprint", seed.lastImportedJsonFingerprint);
        json = juce::JSON::toString(parsed, true);

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        const auto imported = TSS::DevSkin::importSessionFromJson(restored, restoredState, json);
        expect(imported.ok);
        expect(restoredState.lastImportedJsonPath.isEmpty());
        expect(restoredState.lastImportedJsonFingerprint.isEmpty());
    }
};

static DevSkinSessionImportWatchTests devSkinSessionImportWatchTests;
