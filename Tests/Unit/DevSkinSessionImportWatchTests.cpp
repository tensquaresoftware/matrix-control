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
        beginTest("blankLaunch_sessionHasNoWatchPathOrFingerprint");
        assertSessionCarriesNoWatchFields();

        beginTest("blankLaunch_legacyWatchFieldsInOldSessionAreIgnored");
        assertLegacyWatchFieldsIgnored();

        beginTest("hardCut_sessionFileAndDocumentUseDesignerNames");
        assertHardCutNames();
    }

private:
    void assertSessionCarriesNoWatchFields()
    {
        TSS::DevSkin::DesignerSessionState state;
        state.jsonSyncMode = TSS::DevSkin::JsonSyncMode::kOff;

        const auto json = TSS::DevSkin::exportSessionToJson(state);
        expect(! json.contains("lastImportedJsonPath"));
        expect(! json.contains("lastImportedJsonFingerprint"));

        TSS::DevSkin::DesignerSessionState restored;
        expect(TSS::DevSkin::importSessionFromJson(restored, json).ok);
        expect(restored.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kOff);
    }

    void assertLegacyWatchFieldsIgnored()
    {
        juce::TemporaryFile temp("skin-designer-import-watch.json");
        expect(temp.getFile().replaceWithText("{ \"skins\": [] }"));

        auto* root = new juce::DynamicObject();
        root->setProperty("document", TSS::DevSkin::kSessionDocumentId);
        root->setProperty("lastImportedJsonPath", temp.getFile().getFullPathName());
        root->setProperty("lastImportedJsonFingerprint", "42|123456789");
        const auto json = juce::JSON::toString(juce::var(root), true);

        // The session state has no watch fields at all: the loaded JSON is armed
        // only by Load / Save as draft, never restored from the session file.
        TSS::DevSkin::DesignerSessionState restored;
        expect(TSS::DevSkin::importSessionFromJson(restored, json).ok);
        expect(restored.designerSkinId.isEmpty());
        expect(TSS::DevSkin::exportSessionToJson(restored).contains("lastImportedJsonPath") == false);
    }

    void assertHardCutNames()
    {
        expectEquals(TSS::DevSkin::skinDesignerSessionFile().getFileName(),
                     juce::String("skin-designer-session.json"));
        expectEquals(juce::String(TSS::DevSkin::kSessionDocumentId),
                     juce::String("skinDesignerSession"));
    }
};

static DevSkinSessionImportWatchTests devSkinSessionImportWatchTests;
