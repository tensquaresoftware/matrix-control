#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/SkinValues.h"

class ColourSetModelTests : public juce::UnitTest
{
public:
    ColourSetModelTests()
        : juce::UnitTest("ColourSetModel")
    {
    }

    void runTest() override
    {
        beginTest("legacyJson_withoutColourSet_defaultsToK1");
        assertLegacyDefaultsAndExpands();

        beginTest("export_writesSchemaVersion2");
        assertExportWritesSchemaTwo();

        beginTest("unknownColourSet_rejectsImport_withoutChangingLayer");
        assertUnknownColourSetRejectedAtomically();

        beginTest("setMapping_rejectsHigherSetForK1OnlyObject");
        assertLayerRejectsInvalidHigherSet();

        beginTest("json_rejectsHigherSetForK1OnlyObject");
        assertJsonRejectsInvalidHigherSet();
    }

private:
    static juce::String makeFullLegacyJson()
    {
        TSS::DevSkin::Layer source;
        source.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Legacy Full");
        auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(source));
        auto* root = parsed.getDynamicObject();
        root->setProperty("schemaVersion", TSS::DevSkin::kLegacySchemaVersion);
        for (auto& skinValue : *root->getProperty("skins").getArray())
            for (auto& mappingValue : *skinValue.getProperty("mappings", {}).getArray())
                mappingValue.getDynamicObject()->removeProperty("colourSet");
        return juce::JSON::toString(parsed);
    }

    void assertLegacyDefaultsAndExpands()
    {
        TSS::DevSkin::Layer layer;
        expect(TSS::DevSkin::importFromJson(layer, makeFullLegacyJson()).ok);
        const auto* skin = layer.findSkin("dev:legacyfull");
        expect(skin != nullptr);
        if (skin == nullptr)
            return;

        auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                      TSS::SkinColourProperty::kBackground,
                                      TSS::SkinColourState::kOff);
        const auto primaryMapping = skin->mappings.at(key);
        for (const auto colourSet : TSS::kAllColourSets)
        {
            key.colourSet = colourSet;
            expectEquals(skin->mappings.at(key), primaryMapping);
        }
        expectEquals(layer.unboundCount(skin->id), 0);
        expect(layer.isEditorSkinExportAllowed(skin->id));
    }

    void assertExportWritesSchemaTwo()
    {
        TSS::DevSkin::Layer layer;
        const auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(layer));
        expectEquals(static_cast<int>(parsed.getProperty("schemaVersion", 0)),
                     TSS::DevSkin::kSchemaVersion);
    }

    void assertUnknownColourSetRejectedAtomically()
    {
        TSS::DevSkin::Layer layer;
        layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep");
        const auto before = TSS::DevSkin::exportToJson(layer);
        auto parsed = juce::JSON::parse(before);
        auto* skins = parsed.getProperty("skins", {}).getArray();
        auto* mappings = (*skins)[0].getProperty("mappings", {}).getArray();
        (*mappings)[0].getDynamicObject()->setProperty("colourSet", "k5");
        const auto invalid = juce::JSON::toString(parsed);

        expect(! TSS::DevSkin::importFromJson(layer, invalid).ok);
        expectEquals(TSS::DevSkin::exportToJson(layer), before);
    }

    void assertLayerRejectsInvalidHigherSet()
    {
        TSS::DevSkin::Layer layer;
        layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Invalid API");
        const auto key = TSS::skinColourKey(TSS::SkinObjectType::kHeaderPanel,
                                            TSS::SkinColourProperty::kBackground,
                                            TSS::SkinColourState::kDefault,
                                            TSS::ColourSet::k2);
        expect(layer.setMapping(layer.activeSkinId(), key, "kGrey500").failed());
    }

    void assertJsonRejectsInvalidHigherSet()
    {
        TSS::DevSkin::Layer layer;
        layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Invalid JSON");
        auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(layer));
        auto* skins = parsed.getProperty("skins", {}).getArray();
        auto* mappings = (*skins)[0].getProperty("mappings", {}).getArray();
        for (auto& mapping : *mappings)
        {
            if (mapping.getProperty("object", {}) != "HeaderPanel")
                continue;
            mapping.getDynamicObject()->setProperty("colourSet", "k2");
            break;
        }
        expect(! TSS::DevSkin::importFromJson(layer, juce::JSON::toString(parsed)).ok);
    }
};

static ColourSetModelTests colourSetModelTests;
