#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinJsonMapping.h"
#include "GUI/Skins/Dev/DevSkinKeyNames.h"
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

        beginTest("schemaV2_roundTrip_keepsDistinctPastillesPerColourSet");
        assertSchemaV2RoundTripDistinctSets();

        beginTest("unknownColourSet_rejectsImport_withoutChangingLayer");
        assertUnknownColourSetRejectedAtomically();

        beginTest("unsupportedSchemaVersion_rejectsImport_withoutChangingLayer");
        assertUnsupportedSchemaVersionRejectedAtomically();

        beginTest("setMapping_rejectsHigherSetForK1OnlyObject");
        assertLayerRejectsInvalidHigherSet();

        beginTest("json_rejectsHigherSetForK1OnlyObject");
        assertJsonRejectsInvalidHigherSet();

        beginTest("legacyButtonLikeObjectKeys_remapToClassicObjects");
        assertLegacyButtonLikeObjectKeysRemap();

        beginTest("legacyChromeObjectKeys_remapToChromeTextAndDialog");
        assertLegacyChromeObjectKeysRemap();
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

    void assertSchemaV2RoundTripDistinctSets()
    {
        TSS::DevSkin::Layer layer;
        layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Distinct Sets");
        auto key = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                      TSS::SkinColourProperty::kBackground,
                                      TSS::SkinColourState::kOff);
        expect(layer.setMapping(layer.activeSkinId(), key, "kEmerald400").wasOk());
        key.colourSet = TSS::ColourSet::k2;
        expect(layer.setMapping(layer.activeSkinId(), key, "kBlue").wasOk());

        const auto json = TSS::DevSkin::exportToJson(layer);
        const auto activeId = layer.activeSkinId();
        TSS::DevSkin::Layer restored;
        expect(TSS::DevSkin::importFromJson(restored, json).ok);
        const auto* skin = restored.findSkin(activeId);
        expect(skin != nullptr);
        if (skin == nullptr)
            return;

        key.colourSet = TSS::ColourSet::k1;
        expectEquals(skin->mappings.at(key), juce::String("kEmerald400"));
        key.colourSet = TSS::ColourSet::k2;
        expectEquals(skin->mappings.at(key), juce::String("kBlue"));
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

    void assertUnsupportedSchemaVersionRejectedAtomically()
    {
        TSS::DevSkin::Layer layer;
        layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Schema Gate");
        const auto before = TSS::DevSkin::exportToJson(layer);
        auto parsed = juce::JSON::parse(before);
        parsed.getDynamicObject()->setProperty("schemaVersion", 3);
        expect(! TSS::DevSkin::importFromJson(layer, juce::JSON::toString(parsed)).ok);
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
        const auto before = TSS::DevSkin::exportToJson(layer);
        auto parsed = juce::JSON::parse(before);
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
        expectEquals(TSS::DevSkin::exportToJson(layer), before);
    }

    void assertLegacyButtonLikeObjectKeysRemap()
    {
        TSS::SkinObjectType object {};
        expect(TSS::DevSkin::parseObjectType("SliderButtonLike", object));
        expect(object == TSS::SkinObjectType::kSlider);
        expect(TSS::DevSkin::parseObjectType("ComboBoxButtonLike", object));
        expect(object == TSS::SkinObjectType::kComboBox);
        expect(TSS::DevSkin::parseObjectType("PopupMenuButtonLike", object));
        expect(object == TSS::SkinObjectType::kPopupMenu);

        const auto expectRemapToK4 = [this](const char* objectName,
                                            TSS::SkinObjectType expectedObject,
                                            bool includeColourSet,
                                            const char* colourSetValue)
        {
            auto* mappingObject = new juce::DynamicObject();
            mappingObject->setProperty("object", objectName);
            mappingObject->setProperty("property", "Background");
            mappingObject->setProperty("state", "Default");
            if (includeColourSet)
                mappingObject->setProperty("colourSet", colourSetValue);
            mappingObject->setProperty("paletteName", "kGrey600");

            TSS::SkinColourKey key {};
            juce::String paletteName;
            juce::String error;
            expect(TSS::DevSkin::mappingFromVar(juce::var(mappingObject), key, paletteName, error));
            expect(key.object == expectedObject);
            expect(key.colourSet == TSS::ColourSet::k4);
            expectEquals(paletteName, juce::String("kGrey600"));
            expect(TSS::DevSkin::toString(key.object) != objectName);
        };

        expectRemapToK4("ComboBoxButtonLike", TSS::SkinObjectType::kComboBox, true, "k4");
        expectRemapToK4("ComboBoxButtonLike", TSS::SkinObjectType::kComboBox, false, "");
        expectRemapToK4("ComboBoxButtonLike", TSS::SkinObjectType::kComboBox, true, "k1");
        expectRemapToK4("SliderButtonLike", TSS::SkinObjectType::kSlider, false, "");
        expectRemapToK4("SliderButtonLike", TSS::SkinObjectType::kSlider, true, "k1");
        expectRemapToK4("PopupMenuButtonLike", TSS::SkinObjectType::kPopupMenu, true, "k1");
    }

    struct LegacyChromeMappingCase
    {
        const char* objectName = nullptr;
        TSS::SkinObjectType expectedObject {};
        const char* propertyName = nullptr;
        const char* stateName = nullptr;
        const char* expectedObjectName = nullptr;
    };

    void expectLegacyChromeMapping(const LegacyChromeMappingCase& testCase)
    {
        auto* mappingObject = new juce::DynamicObject();
        mappingObject->setProperty("object", testCase.objectName);
        mappingObject->setProperty("property", testCase.propertyName);
        mappingObject->setProperty("state", testCase.stateName);
        mappingObject->setProperty("paletteName", "kRed900");

        TSS::SkinColourKey key {};
        juce::String paletteName;
        juce::String error;
        expect(TSS::DevSkin::mappingFromVar(juce::var(mappingObject), key, paletteName, error));
        expect(key.object == testCase.expectedObject);
        expectEquals(TSS::DevSkin::toString(key.property), juce::String(testCase.propertyName));
        expectEquals(TSS::DevSkin::toString(key.state), juce::String(testCase.stateName));
        expectEquals(paletteName, juce::String("kRed900"));
        expect(TSS::DevSkin::toString(key.object) != testCase.objectName);

        const auto resaved = TSS::DevSkin::mappingToVar(key, paletteName);
        auto* resavedObject = resaved.getDynamicObject();
        expect(resavedObject != nullptr);
        const auto objectOut = resavedObject->getProperty("object").toString();
        expectEquals(objectOut, juce::String(testCase.expectedObjectName));
        expectEquals(resavedObject->getProperty("property").toString(),
                     juce::String(testCase.propertyName));
        expectEquals(resavedObject->getProperty("state").toString(),
                     juce::String(testCase.stateName));
        expect(objectOut != "DarkPanel" && objectOut != "WindowClose");
    }

    void assertLegacyChromeObjectKeysRemap()
    {
        TSS::SkinObjectType object {};
        expect(TSS::DevSkin::parseObjectType("DarkPanel", object));
        expect(object == TSS::SkinObjectType::kChromeText);
        expect(TSS::DevSkin::parseObjectType("WindowClose", object));
        expect(object == TSS::SkinObjectType::kDialog);
        expectEquals(TSS::DevSkin::toString(TSS::SkinObjectType::kChromeText), juce::String("ChromeText"));
        expectEquals(TSS::DevSkin::toString(TSS::SkinObjectType::kDialog), juce::String("Dialog"));
        expectEquals(TSS::DevSkin::toString(TSS::SkinObjectType::kWindowSurface),
                     juce::String("WindowSurface"));

        expectLegacyChromeMapping({ "DarkPanel", TSS::SkinObjectType::kChromeText, "Text", "Default", "ChromeText" });
        expectLegacyChromeMapping({ "DarkPanel", TSS::SkinObjectType::kChromeText, "Text", "Hover", "ChromeText" });
        expectLegacyChromeMapping({ "WindowClose", TSS::SkinObjectType::kDialog, "Glyph", "Default", "Dialog" });
    }
};

static ColourSetModelTests colourSetModelTests;
