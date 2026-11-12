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

        beginTest("json_dropsSetsBeyondObjectInventory");
        assertJsonDropsSetsBeyondInventory();

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

        auto key = TSS::skinColourKey(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kTrack);
        const auto primaryMapping = skin->mappings.at(key);
        for (const auto colourSet : TSS::kAllColourSets)
        {
            key.colourSet = colourSet;
            expectEquals(skin->mappings.at(key), primaryMapping);
        }

        auto ucsKey = TSS::skinColourKey(TSS::SkinObjectType::kButton,
                                         TSS::SkinColourProperty::kBackground,
                                         TSS::SkinColourState::kOff,
                                         TSS::ColourSet::k2);
        expect(skin->mappings.count(ucsKey) == 0);
        expectEquals(layer.unboundCount(skin->id), 0);
        expect(layer.isDesignerSkinExportAllowed(skin->id));
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
        auto key = TSS::skinColourKey(TSS::SkinObjectType::kSlider, TSS::SkinColourProperty::kTrack);
        expect(layer.setMapping(layer.activeSkinId(), key, "kGreen300").wasOk());
        key.colourSet = TSS::ColourSet::k2;
        expect(layer.setMapping(layer.activeSkinId(), key, "kBrandBlue").wasOk());

        const auto json = TSS::DevSkin::exportToJson(layer);
        const auto activeId = layer.activeSkinId();
        TSS::DevSkin::Layer restored;
        expect(TSS::DevSkin::importFromJson(restored, json).ok);
        const auto* skin = restored.findSkin(activeId);
        expect(skin != nullptr);
        if (skin == nullptr)
            return;

        key.colourSet = TSS::ColourSet::k1;
        expectEquals(skin->mappings.at(key), juce::String("kGreen300"));
        key.colourSet = TSS::ColourSet::k2;
        expectEquals(skin->mappings.at(key), juce::String("kBrandBlue"));
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

    static void appendExcessColourSetRows(juce::Array<juce::var>* mappings)
    {
        auto addRow = [mappings](const char* object, const char* property, const char* state, const char* colourSet)
        {
            auto* row = new juce::DynamicObject();
            row->setProperty("object", object);
            row->setProperty("property", property);
            row->setProperty("state", state);
            row->setProperty("colourSet", colourSet);
            row->setProperty("paletteName", "kGrey500");
            mappings->add(juce::var(row));
        };
        addRow("HeaderPanel", "Background", "Default", "cs2");
        addRow("ComboBox", "Background", "Default", "cs4");
        addRow("Led", "Background", "On", "cs3");
    }

    void expectExcessColourSetKeysAbsent(const std::map<TSS::SkinColourKey, juce::String>& mappings)
    {
        const auto excessHeader = TSS::skinColourKey(TSS::SkinObjectType::kHeaderPanel,
                                                     TSS::SkinColourProperty::kBackground,
                                                     TSS::SkinColourState::kDefault,
                                                     TSS::ColourSet::k2);
        const auto excessCombo = TSS::skinColourKey(TSS::SkinObjectType::kComboBox,
                                                    TSS::SkinColourProperty::kBackground,
                                                    TSS::SkinColourState::kDefault,
                                                    TSS::ColourSet::k4);
        const auto excessLed = TSS::skinColourKey(TSS::SkinObjectType::kLed,
                                                  TSS::SkinColourProperty::kBackground,
                                                  TSS::SkinColourState::kOn,
                                                  TSS::ColourSet::k3);
        expect(mappings.count(excessHeader) == 0);
        expect(mappings.count(excessCombo) == 0);
        expect(mappings.count(excessLed) == 0);
    }

    void assertJsonDropsSetsBeyondInventory()
    {
        TSS::DevSkin::Layer layer;
        layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Old Inventory");
        const auto skinId = layer.activeSkinId();
        const auto* before = layer.findSkin(skinId);
        expect(before != nullptr);
        if (before == nullptr)
            return;

        const auto headerBackgroundKey = TSS::skinColourKey(TSS::SkinObjectType::kHeaderPanel,
                                                            TSS::SkinColourProperty::kBackground);
        const auto originalHeaderBackground = before->mappings.at(headerBackgroundKey);

        auto parsed = juce::JSON::parse(TSS::DevSkin::exportToJson(layer));
        auto* skins = parsed.getProperty("skins", {}).getArray();
        appendExcessColourSetRows((*skins)[0].getProperty("mappings", {}).getArray());

        TSS::DevSkin::Layer restored;
        expect(TSS::DevSkin::importFromJson(restored, juce::JSON::toString(parsed)).ok);
        const auto* skin = restored.findSkin(skinId);
        expect(skin != nullptr);
        if (skin == nullptr)
            return;

        for (const auto& [key, paletteName] : skin->mappings)
        {
            juce::ignoreUnused(paletteName);
            expect(TSS::supportsColourSet(key.object, key.colourSet));
        }

        expectEquals(skin->mappings.at(headerBackgroundKey), originalHeaderBackground);
        expect(originalHeaderBackground != juce::String("kGrey500"));
        expectExcessColourSetKeysAbsent(skin->mappings);
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

        const auto expectRemapToMono = [this](const char* objectName,
                                              TSS::SkinObjectType expectedObject,
                                              bool includeColourSet,
                                              const char* colourSetValue,
                                              TSS::ColourSet expectedSet)
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
            expect(key.colourSet == expectedSet);
            expectEquals(paletteName, juce::String("kGrey600"));
            expect(TSS::DevSkin::toString(key.object) != objectName);
        };

        using TSS::ColourSet;
        expectRemapToMono("ComboBoxButtonLike", TSS::SkinObjectType::kComboBox, true, "cs4", ColourSet::k3);
        expectRemapToMono("ComboBoxButtonLike", TSS::SkinObjectType::kComboBox, false, "", ColourSet::k3);
        expectRemapToMono("ComboBoxButtonLike", TSS::SkinObjectType::kComboBox, true, "cs1", ColourSet::k3);
        expectRemapToMono("SliderButtonLike", TSS::SkinObjectType::kSlider, false, "", ColourSet::k4);
        expectRemapToMono("SliderButtonLike", TSS::SkinObjectType::kSlider, true, "cs1", ColourSet::k4);
        expectRemapToMono("PopupMenuButtonLike", TSS::SkinObjectType::kPopupMenu, true, "cs1", ColourSet::k3);
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
