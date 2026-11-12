#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"
#include "GUI/Skins/Skin.h"
#include "Tools/SkinDesigner/SkinDesignerUiHelpers.h"

/** Skin Designer blank start: Load replaces skins, Save as draft packs them, session = prefs only. */
class DevSkinWorkspaceFilesTests : public juce::UnitTest
{
public:
    DevSkinWorkspaceFilesTests()
        : juce::UnitTest("DevSkinWorkspaceFiles")
    {
    }

    void runTest() override
    {
        beginTest("load_draftPack_replacesDesignerSkinsAndRestoresActiveSkin");
        assertLoadDraftPackReplacesDesignerSkins();

        beginTest("load_singleSkinExport_replacesDesignerSkins");
        assertLoadSingleSkinExportReplaces();

        beginTest("load_refusedFile_keepsLayerUntouched");
        assertLoadRefusedKeepsLayer();

        beginTest("draftPack_withUnboundSkins_roundTripsWhileExportStaysBlocked");
        assertDraftPackKeepsUnboundSkins();

        beginTest("sessionRoundTrip_restoresPreferencesOnly");
        assertSessionRoundTrip();

        beginTest("oldOrForeignSessionDocument_isRefused");
        assertOldSessionDocumentRefused();

        beginTest("sessionWithSkinsAndWatchFields_ignoresEverythingButPreferences");
        assertSessionIgnoresSkinsAndWatchFields();

        beginTest("quitDirty_promptsOnlyWhenDirtyAndHasDesignerSkin");
        assertQuitDirtyPromptPolicy();

        beginTest("saveTargets_getJsonExtension_andStatusPrefixesAreRecognised");
        assertSaveTargetsAndStatusPrefixes();
    }

private:
    void assertLoadDraftPackReplacesDesignerSkins()
    {
        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Trial A").wasOk());
        const auto trialA = donor.activeSkinId();
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledBlackId, "Trial B").wasOk());
        const auto trialB = donor.activeSkinId();
        expect(donor.selectSkin(trialA));
        const auto pack = TSS::DevSkin::exportToJson(donor);

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Old Work").wasOk());
        const auto oldId = layer.activeSkinId();

        const auto loaded = TSS::DevSkin::replaceDesignerSkinsFromJson(layer, pack);
        expect(loaded.ok);
        expectEquals(loaded.importedSkinIds.size(), 2);
        expect(layer.findSkin(oldId) == nullptr);
        expect(layer.findSkin(trialA) != nullptr);
        expect(layer.findSkin(trialB) != nullptr);
        expectEquals(layer.skins().size(), static_cast<size_t>(7));
        expectEquals(layer.activeSkinId(), trialA);
    }

    void assertLoadSingleSkinExportReplaces()
    {
        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Ready").wasOk());
        const auto readyId = donor.activeSkinId();
        const auto exported = TSS::DevSkin::exportDesignerSkinToJson(donor, readyId);

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Old One").wasOk());
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Old Two").wasOk());

        const auto loaded = TSS::DevSkin::replaceDesignerSkinsFromJson(layer, exported);
        expect(loaded.ok);
        expectEquals(layer.skins().size(), static_cast<size_t>(6));
        expect(layer.findSkin(readyId) != nullptr);
        expectEquals(layer.activeSkinId(), readyId);
    }

    void assertLoadRefusedKeepsLayer()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep Me").wasOk());
        const auto activeId = layer.activeSkinId();
        const auto count = layer.skins().size();

        expect(! TSS::DevSkin::replaceDesignerSkinsFromJson(layer, "{ not json").ok);

        TSS::DevSkin::Layer blank;
        const auto noDesignerSkins = TSS::DevSkin::replaceDesignerSkinsFromJson(
            layer, TSS::DevSkin::exportToJson(blank));
        expect(! noDesignerSkins.ok);
        expect(noDesignerSkins.message.containsIgnoreCase("No Designer skins"));

        expectEquals(layer.activeSkinId(), activeId);
        expectEquals(layer.skins().size(), count);
    }

    void assertDraftPackKeepsUnboundSkins()
    {
        TSS::DevSkin::Layer donor;
        expect(donor.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Half Done").wasOk());
        const auto halfId = donor.activeSkinId();
        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kLed,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kOff);
        donor.findSkinMutable(halfId)->mappings.erase(key);
        expect(donor.unboundCount(halfId) >= 1);
        expect(! donor.isDesignerSkinExportAllowed(halfId));

        TSS::DevSkin::Layer layer;
        expect(TSS::DevSkin::replaceDesignerSkinsFromJson(layer, TSS::DevSkin::exportToJson(donor)).ok);
        expectEquals(layer.unboundCount(halfId), donor.unboundCount(halfId));
        expect(! layer.isDesignerSkinExportAllowed(halfId));
    }

    void assertSessionRoundTrip()
    {
        TSS::DevSkin::DesignerSessionState state;
        state.productSkinId = TSS::DevSkin::kCompiledCreamId;
        state.designerSkinId = "not-persisted";
        state.compareOn = true;
        state.jsonSyncMode = TSS::DevSkin::JsonSyncMode::kAuto;
        // Import keeps lastSkinJsonDirectory only when the path is an existing directory
        // (Linux CI often has no ~/Documents). Seed a unique real temp folder for the round-trip.
        const auto skinJsonDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                     .getNonexistentChildFile("DevSkinLayerJsonTests-session-dir", {});
        const bool skinJsonDirReady = skinJsonDir.createDirectory() || skinJsonDir.isDirectory();
        expect(skinJsonDirReady);
        if (! skinJsonDirReady)
            return;
        state.lastSkinJsonDirectory = skinJsonDir.getFullPathName();

        const auto json = TSS::DevSkin::exportSessionToJson(state);
        expect(json.contains("skinDesignerSession"));
        expect(json.contains("lastSkinJsonDirectory"));
        expect(json.contains("jsonSyncMode"));
        expect(! json.contains("skins"));
        expect(! json.contains("designerSkinId"));
        expect(! json.contains("productSkinId"));
        expect(! json.contains("compareOn"));
        expect(! json.contains("lastImportedJsonPath"));

        TSS::DevSkin::DesignerSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restoredState, json).ok);
        expectEquals(restoredState.lastSkinJsonDirectory, state.lastSkinJsonDirectory);
        expect(restoredState.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAuto);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
        expect(restoredState.designerSkinId.isEmpty());
        expect(! restoredState.compareOn);
        skinJsonDir.deleteRecursively();
    }

    void assertOldSessionDocumentRefused()
    {
        for (const char* document : { "skinEditorSession", "somethingElse" })
        {
            auto* root = new juce::DynamicObject();
            root->setProperty("document", document);
            root->setProperty("jsonSyncMode", "off");
            const auto json = juce::JSON::toString(juce::var(root), true);

            TSS::DevSkin::DesignerSessionState state;
            expect(! TSS::DevSkin::importSessionFromJson(state, json).ok);
            expect(state.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kAsk);
        }

        TSS::DevSkin::DesignerSessionState corrupt;
        expect(! TSS::DevSkin::importSessionFromJson(corrupt, "{ not-json").ok);
    }

    void assertSessionIgnoresSkinsAndWatchFields()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Never Restored").wasOk());
        auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(layer));
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->setProperty("document", TSS::DevSkin::kSessionDocumentId);
        root->setProperty("productSkinId", TSS::DevSkin::kCompiledCreamId);
        root->setProperty("designerSkinId", layer.activeSkinId());
        root->setProperty("compareOn", true);
        root->setProperty("lastImportedJsonPath", "/tmp/matrix-control-missing-watch.json");
        root->setProperty("jsonSyncMode", "off");

        TSS::DevSkin::DesignerSessionState state;
        expect(TSS::DevSkin::importSessionFromJson(state, juce::JSON::toString(parsed, true)).ok);
        expect(state.jsonSyncMode == TSS::DevSkin::JsonSyncMode::kOff);
        expectEquals(state.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
        expect(state.designerSkinId.isEmpty());
        expect(! state.compareOn);
    }

    void assertSaveTargetsAndStatusPrefixes()
    {
        const auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory);
        expectEquals(SkinDesignerUi::withJsonExtension(dir.getChildFile("Trials")).getFileName(),
                     juce::String("Trials.json"));
        expectEquals(SkinDesignerUi::withJsonExtension(dir.getChildFile("Trials.JSON")).getFileName(),
                     juce::String("Trials.JSON"));
        expectEquals(SkinDesignerUi::withJsonExtension(dir.getChildFile("v1.2")).getFileName(),
                     juce::String("v1.2.json"));

        expectEquals(SkinDesignerUi::defaultPackFile(SkinDesignerUi::kDefaultWorkspaceName,
                                                     dir.getFullPathName())
                         .getFileName(),
                     juce::String("Matrix-Control Skin Designer - Trials.json"));
        expect(! SkinDesignerUi::defaultPackFile(SkinDesignerUi::kDefaultWorkspaceName,
                                                 dir.getFullPathName())
                     .getFileName()
                     .contains("Skin Editor"));

        expect(SkinDesignerUi::isFilePathStatus("Loaded: /tmp/a.json"));
        expect(SkinDesignerUi::isFilePathStatus("Saved: /tmp/a.json"));
        expect(SkinDesignerUi::isFilePathStatus("Saved as: /tmp/a.json"));
        expect(! SkinDesignerUi::isFilePathStatus("Reloaded: /tmp/a.json"));
    }

    void assertQuitDirtyPromptPolicy()
    {
        expect(! SkinDesignerUi::shouldPromptQuitForUnsavedChanges(false, true));
        expect(! SkinDesignerUi::shouldPromptQuitForUnsavedChanges(true, false));
        expect(SkinDesignerUi::shouldPromptQuitForUnsavedChanges(true, true));
        expectEquals(juce::String(SkinDesignerUi::kQuitSaveButton), juce::String("Save"));
        expectEquals(juce::String(SkinDesignerUi::kQuitDiscardButton), juce::String("Discard"));
        expectEquals(juce::String(SkinDesignerUi::kQuitCancelButton), juce::String("Cancel"));
    }
};

static DevSkinWorkspaceFilesTests devSkinWorkspaceFilesTests;
