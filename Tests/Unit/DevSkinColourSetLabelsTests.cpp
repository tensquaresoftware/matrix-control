#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/SkinValues.h"

class DevSkinColourSetLabelsTests : public juce::UnitTest
{
public:
    DevSkinColourSetLabelsTests()
        : juce::UnitTest("DevSkinColourSetLabels")
    {
    }

    void runTest() override
    {
        beginTest("colourSetLabels_roundTrip_andOmittedFromPush");
        assertColourSetLabelsRoundTripAndPushOmit();
        beginTest("colourSetLabels_hardCap16_andMappingUntouched");
        assertColourSetLabelHardCapAndMappingUntouched();
        beginTest("colourSetLabels_stripsNonAsciiOnSet");
        assertColourSetLabelStripsNonAscii();
        beginTest("colourSetLabels_importCapsValueLongerThanMax");
        assertColourSetLabelImportCapsLongValue();
        beginTest("colourSetLabels_compiledSkinsRejectSet");
        assertColourSetLabelCompiledRejects();
    }

private:
    void assertColourSetLabelHardCapAndMappingUntouched()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Cap Skin").wasOk());
        const auto skinId = layer.activeSkinId();
        const auto* before = layer.findSkin(skinId);
        expect(before != nullptr);
        const auto mappingCountBefore = before->mappings.size();
        juce::String seventeen;
        for (int i = 0; i < 17; ++i)
            seventeen += 'a';
        expect(layer.setColourSetLabel(skinId, TSS::ColourSet::k3, seventeen).wasOk());
        const auto* after = layer.findSkin(skinId);
        expect(after != nullptr);
        expectEquals(after->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k3)].length(),
                     TSS::DevSkin::kColourSetLabelMaxChars);
        juce::String sixteen;
        for (int i = 0; i < TSS::DevSkin::kColourSetLabelMaxChars; ++i)
            sixteen += 'a';
        expectEquals(after->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k3)], sixteen);
        expectEquals(static_cast<int>(after->mappings.size()),
                     static_cast<int>(mappingCountBefore));
    }

    void assertColourSetLabelsRoundTripAndPushOmit()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Label Skin").wasOk());
        const auto skinId = layer.activeSkinId();
        expect(layer.setColourSetLabel(skinId, TSS::ColourSet::k2, "Master").wasOk());
        expect(layer.setColourSetLabel(skinId, TSS::ColourSet::k4, "Modales").wasOk());
        expect(layer.setColourSetLabel(skinId, TSS::ColourSet::k1, "   ").wasOk());

        const auto json = TSS::DevSkin::exportEditorSkinToJson(layer, skinId);
        const auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        auto* skinsArr = root->getProperty("skins").getArray();
        expect(skinsArr != nullptr && ! skinsArr->isEmpty());
        auto* skinObj = (*skinsArr)[0].getDynamicObject();
        expect(skinObj != nullptr);
        auto* labelsObj = skinObj->getProperty("colourSetLabels").getDynamicObject();
        expect(labelsObj != nullptr);
        expectEquals(labelsObj->getProperty("k2").toString(), juce::String("Master"));
        expectEquals(labelsObj->getProperty("k4").toString(), juce::String("Modales"));
        expect(! labelsObj->hasProperty("k1"));

        TSS::DevSkin::Layer restored;
        const auto imported = TSS::DevSkin::appendEditorSkinsFromJson(restored, json);
        expect(imported.ok);
        expectEquals(imported.importedSkinIds.size(), 1);
        const auto* skin = restored.findSkin(imported.importedSkinIds[0]);
        expect(skin != nullptr);
        expectEquals(skin->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k2)],
                     juce::String("Master"));
        expectEquals(skin->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k4)],
                     juce::String("Modales"));
        expect(skin->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k1)].isEmpty());

        const auto pushVar = TSS::DevSkin::pushPayloadToVar(layer.makePushPayloadForSkin(skinId));
        auto* pushObj = pushVar.getDynamicObject();
        expect(pushObj != nullptr);
        expect(pushObj->getProperty("colourSetLabels").isVoid());
    }

    void assertColourSetLabelStripsNonAscii()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Ascii Skin").wasOk());
        const auto skinId = layer.activeSkinId();
        // "Patch" + non-ASCII + "Zone" — only printable ASCII should remain.
        const juce::String mixed = juce::String("Patch")
                                   + juce::String::charToString(static_cast<juce::juce_wchar>(0x00E9))
                                   + "Zone";
        expect(layer.setColourSetLabel(skinId, TSS::ColourSet::k1, mixed).wasOk());
        const auto* skin = layer.findSkin(skinId);
        expect(skin != nullptr);
        expectEquals(skin->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k1)],
                     juce::String("PatchZone"));
    }

    void assertColourSetLabelImportCapsLongValue()
    {
        juce::String seventeen;
        for (int i = 0; i < 17; ++i)
            seventeen += 'x';

        auto* labels = new juce::DynamicObject();
        labels->setProperty("k3", seventeen);
        auto* skinObj = new juce::DynamicObject();
        skinObj->setProperty("id", "import-cap-skin");
        skinObj->setProperty("name", "Import Cap");
        skinObj->setProperty("kind", "dev");
        skinObj->setProperty("palette", juce::Array<juce::var>());
        skinObj->setProperty("mappings", juce::Array<juce::var>());
        skinObj->setProperty("colourSetLabels", juce::var(labels));

        auto* root = new juce::DynamicObject();
        root->setProperty("schemaVersion", TSS::DevSkin::kSchemaVersion);
        root->setProperty("activeSkinId", "import-cap-skin");
        juce::Array<juce::var> skins;
        skins.add(juce::var(skinObj));
        root->setProperty("skins", skins);
        const auto json = juce::JSON::toString(juce::var(root), false);

        TSS::DevSkin::Layer layer;
        const auto imported = TSS::DevSkin::appendEditorSkinsFromJson(layer, json);
        expect(imported.ok);
        expectEquals(imported.importedSkinIds.size(), 1);
        const auto* skin = layer.findSkin(imported.importedSkinIds[0]);
        expect(skin != nullptr);
        expectEquals(skin->colourSetLabels[static_cast<size_t>(TSS::ColourSet::k3)].length(),
                     TSS::DevSkin::kColourSetLabelMaxChars);
    }

    void assertColourSetLabelCompiledRejects()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.setColourSetLabel(TSS::DevSkin::kCompiledBlackId,
                                       TSS::ColourSet::k1,
                                       "Patch").failed());
        expect(layer.setColourSetLabel(TSS::DevSkin::kCompiledCreamId,
                                       TSS::ColourSet::k2,
                                       "Master").failed());
    }
};

static DevSkinColourSetLabelsTests devSkinColourSetLabelsTests;
