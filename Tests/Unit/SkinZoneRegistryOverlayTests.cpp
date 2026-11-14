#include <juce_core/juce_core.h>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Dev/DevSkinApplier.h"
#include "GUI/Skins/Dev/DevSkinJson.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/Dev/DevSkinStandalonePaintState.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinZoneOverlay.h"
#include "GUI/Skins/SkinZoneRegistry.h"

namespace
{
    bool ensureChartPastille(TSS::DevSkin::Layer& layer,
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
}

class SkinZoneRegistryOverlayTests : public juce::UnitTest
{
public:
    SkinZoneRegistryOverlayTests()
        : juce::UnitTest("SkinZoneRegistryOverlay")
    {
    }

    void runTest() override
    {
        beginTest("registry_occupancy_matchesCompanionTable");
        assertOccupancy();

        beginTest("registry_defaults_matchZoneColourSet");
        assertDefaults();

        beginTest("registry_exceptionRows_matchCompanionTable");
        assertExceptionRows();

        beginTest("noOverlay_lookForZone_usesRegistryDefaults");
        assertNoOverlayDefaults();

        beginTest("overlay_sliderPatchToCs2_resolvesAndLook");
        assertOverlaySliderRemap();

        beginTest("overlay_clear_andExceptionRegistryDefaults");
        assertOverlayClearAndExceptionDefaults();

        beginTest("apply_rejectsUnusedOrInvalidZoneMap_atomically");
        assertApplyRejectsInvalidZoneMap();

        beginTest("pushJson_roundTrip_preservesZoneMap");
        assertPushJsonRoundTrip();

        beginTest("missingZoneMapOnPush_treatsAsDefaults");
        assertMissingZoneMapDefaults();

        beginTest("overlay_ledHeaderRemap_resolveZoneColourSet");
        assertOverlayLedRemap();
    }

private:
    void assertOccupancy()
    {
        using TSS::SkinObjectType;
        using TSS::SkinZone;

        for (const auto zone : TSS::kAllSkinZones)
            expect(TSS::isZoneUsed(SkinObjectType::kSlider, zone));

        expect(TSS::isZoneUsed(SkinObjectType::kComboBox, SkinZone::kPatch));
        expect(TSS::isZoneUsed(SkinObjectType::kComboBox, SkinZone::kMaster));
        expect(! TSS::isZoneUsed(SkinObjectType::kComboBox, SkinZone::kHeader));
        expect(TSS::isZoneUsed(SkinObjectType::kComboBox, SkinZone::kModales));

        expect(TSS::isZoneUsed(SkinObjectType::kPopupMenu, SkinZone::kPatch));
        expect(TSS::isZoneUsed(SkinObjectType::kPopupMenu, SkinZone::kMaster));
        expect(! TSS::isZoneUsed(SkinObjectType::kPopupMenu, SkinZone::kHeader));
        expect(TSS::isZoneUsed(SkinObjectType::kPopupMenu, SkinZone::kModales));

        expect(! TSS::isZoneUsed(SkinObjectType::kLed, SkinZone::kPatch));
        expect(! TSS::isZoneUsed(SkinObjectType::kLed, SkinZone::kMaster));
        expect(TSS::isZoneUsed(SkinObjectType::kLed, SkinZone::kHeader));
        expect(TSS::isZoneUsed(SkinObjectType::kLed, SkinZone::kModales));

        expect(! TSS::isZoneUsed(SkinObjectType::kPeakIndicator, SkinZone::kPatch));
        expect(! TSS::isZoneUsed(SkinObjectType::kPeakIndicator, SkinZone::kMaster));
        expect(TSS::isZoneUsed(SkinObjectType::kPeakIndicator, SkinZone::kHeader));
        expect(TSS::isZoneUsed(SkinObjectType::kPeakIndicator, SkinZone::kModales));

        expect(! TSS::isZoneUsed(SkinObjectType::kButton, SkinZone::kHeader));
    }

    void assertDefaults()
    {
        using TSS::ColourSet;
        using TSS::SkinObjectType;
        using TSS::SkinZone;

        for (const auto zone : TSS::kAllSkinZones)
            expect(TSS::defaultColourSet(SkinObjectType::kSlider, zone)
                   == TSS::zoneColourSet(SkinObjectType::kSlider, zone));

        expect(TSS::defaultColourSet(SkinObjectType::kSlider, SkinZone::kPatch) == ColourSet::k1);
        expect(TSS::defaultColourSet(SkinObjectType::kSlider, SkinZone::kMaster) == ColourSet::k2);
        expect(TSS::defaultColourSet(SkinObjectType::kSlider, SkinZone::kHeader) == ColourSet::k3);
        expect(TSS::defaultColourSet(SkinObjectType::kSlider, SkinZone::kModales) == ColourSet::k4);

        expect(TSS::defaultColourSet(SkinObjectType::kComboBox, SkinZone::kModales) == ColourSet::k3);
        expect(TSS::defaultColourSet(SkinObjectType::kLed, SkinZone::kHeader) == ColourSet::k1);
        expect(TSS::defaultColourSet(SkinObjectType::kLed, SkinZone::kModales) == ColourSet::k2);
    }

    void assertExceptionRows()
    {
        expectEquals(static_cast<int>(TSS::kZoneExceptionRows.size()), 4);

        const auto& combo = TSS::kZoneExceptionRows[0];
        expect(combo.object == TSS::SkinObjectType::kComboBox);
        expect(combo.usesKind == TSS::ZoneExceptionUsesKind::kColourSet);
        expect(combo.colourSet == TSS::ColourSet::k3);
        expect(juce::String(combo.where).contains("Computer Patches"));

        const auto& popupBrowser = TSS::kZoneExceptionRows[1];
        expect(popupBrowser.object == TSS::SkinObjectType::kPopupMenu);
        expect(popupBrowser.colourSet == TSS::ColourSet::k3);

        const auto& logo = TSS::kZoneExceptionRows[2];
        expect(logo.object == TSS::SkinObjectType::kPopupMenu);
        expect(juce::String(logo.where).contains("logo"));
        expect(logo.colourSet == TSS::ColourSet::k3);

        const auto& buttons = TSS::kZoneExceptionRows[3];
        expect(buttons.object == TSS::SkinObjectType::kButton);
        expect(buttons.usesKind == TSS::ZoneExceptionUsesKind::kUcs);
        expect(buttons.colourSet == TSS::ColourSet::k1);
    }

    void assertNoOverlayDefaults()
    {
        TSS::clearSkinZoneOverlay();
        expect(! TSS::hasSkinZoneOverlay());

        for (const auto zone : TSS::kAllSkinZones)
        {
            expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, zone)
                   == TSS::zoneColourSet(TSS::SkinObjectType::kSlider, zone));
            expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kComboBox, zone)
                   == TSS::zoneColourSet(TSS::SkinObjectType::kComboBox, zone));
        }
    }

    void assertOverlaySliderRemap()
    {
        TSS::clearSkinZoneOverlay();

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZoneRemap").wasOk());
        const auto skinId = layer.activeSkinId();

        TSS::SkinZoneMap map;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k2;
        expect(layer.setZoneMap(skinId, map).wasOk());
        expect(ensureChartPastille(layer, skinId, "kGreen300"));

        const auto sliderTrackKey = TSS::skinColourKey(TSS::SkinObjectType::kSlider,
                                                       TSS::SkinColourProperty::kTrack,
                                                       TSS::SkinColourState::kDefault,
                                                       TSS::ColourSet::k2);
        expect(layer.setMapping(skinId, sliderTrackKey, "kGreen300").wasOk());

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(layer.makePushPayload()).ok);
        expect(TSS::hasSkinZoneOverlay());
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k2);
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kMaster)
               == TSS::ColourSet::k2);
        expect(paint.activeSkin().getColour(sliderTrackKey) == juce::Colour(ColourChart::kGreen300));
        expect(TSS::sliderLookForZone(paint.activeSkin(), TSS::SkinZone::kPatch).trackEnabled
               == juce::Colour(ColourChart::kGreen300));
    }

    static TSS::SkinColourKey bgKey(TSS::SkinObjectType object, TSS::ColourSet colourSet)
    {
        return TSS::skinColourKey(object,
                                  TSS::SkinColourProperty::kBackground,
                                  TSS::SkinColourState::kDefault,
                                  colourSet);
    }

    void seedExceptionLookPastilles(TSS::DevSkin::Layer& layer, const juce::String& skinId)
    {
        expect(ensureChartPastille(layer, skinId, "kGreen300"));
        expect(ensureChartPastille(layer, skinId, "kRed500"));
        expect(layer.setMapping(skinId, bgKey(TSS::SkinObjectType::kComboBox, TSS::ColourSet::k1),
                                "kGreen300").wasOk());
        expect(layer.setMapping(skinId, bgKey(TSS::SkinObjectType::kComboBox, TSS::ColourSet::k3),
                                "kRed500").wasOk());
        expect(layer.setMapping(skinId, bgKey(TSS::SkinObjectType::kPopupMenu, TSS::ColourSet::k1),
                                "kGreen300").wasOk());
        expect(layer.setMapping(skinId, bgKey(TSS::SkinObjectType::kPopupMenu, TSS::ColourSet::k3),
                                "kRed500").wasOk());
    }

    void assertExceptionLooksIgnoreOverlay(const TSS::Skin& skin)
    {
        const auto green = juce::Colour(ColourChart::kGreen300);
        const auto red = juce::Colour(ColourChart::kRed500);
        expect(TSS::comboBoxLookFromSkin(
                   skin, TSS::zoneColourSet(TSS::SkinObjectType::kComboBox, TSS::kMonoMenuExceptionZone))
                   .backgroundEnabled
               == red);
        expect(TSS::comboBoxLookForZone(skin, TSS::SkinZone::kModales).backgroundEnabled == green);
        expect(TSS::popupMenuLookFromSkin(
                   skin, TSS::zoneColourSet(TSS::SkinObjectType::kPopupMenu, TSS::kMonoMenuExceptionZone))
                   .background
               == red);
        expect(TSS::popupMenuLookForZone(skin, TSS::SkinZone::kModales).background == green);
    }

    void assertOverlayClearAndExceptionDefaults()
    {
        TSS::clearSkinZoneOverlay();

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZoneClear").wasOk());
        const auto skinId = layer.activeSkinId();
        seedExceptionLookPastilles(layer, skinId);

        TSS::SkinZoneMap map;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k2;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kComboBox, TSS::SkinZone::kModales }]
            = TSS::ColourSet::k1;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kPopupMenu, TSS::SkinZone::kModales }]
            = TSS::ColourSet::k1;
        expect(layer.setZoneMap(skinId, map).wasOk());

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(layer.makePushPayload()).ok);
        assertExceptionLooksIgnoreOverlay(paint.activeSkin());

        paint.clearOverlayToCompiled();
        expect(! TSS::hasSkinZoneOverlay());
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k1);
    }

    void assertApplyRejectsInvalidZoneMap()
    {
        TSS::clearSkinZoneOverlay();

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "BadZone").wasOk());
        auto payload = layer.makePushPayload();

        // Seed a prior valid overlay to prove rejection leaves it unchanged.
        TSS::SkinZoneMap prior;
        prior[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k2;
        TSS::setSkinZoneOverlay(prior);
        expect(TSS::hasSkinZoneOverlay());

        payload.zoneMap.clear();
        payload.zoneMap[TSS::SkinZoneMapKey { TSS::SkinObjectType::kComboBox, TSS::SkinZone::kHeader }]
            = TSS::ColourSet::k1;

        auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        const auto unusedResult = TSS::DevSkin::applyPushToSkin(payload, *skin);
        expect(! unusedResult.ok);
        expectEquals(unusedResult.code, juce::String("invalid_zone_map"));
        expect(TSS::hasSkinZoneOverlay());
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k2);

        payload.zoneMap.clear();
        payload.zoneMap[TSS::SkinZoneMapKey { TSS::SkinObjectType::kLed, TSS::SkinZone::kHeader }]
            = TSS::ColourSet::k3; // Led only has cs1/cs2
        const auto rangeResult = TSS::DevSkin::applyPushToSkin(payload, *skin);
        expect(! rangeResult.ok);
        expectEquals(rangeResult.code, juce::String("invalid_zone_map"));
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k2);

        TSS::clearSkinZoneOverlay();
    }

    void assertPushJsonRoundTrip()
    {
        TSS::clearSkinZoneOverlay();

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZoneWire").wasOk());
        TSS::SkinZoneMap map;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k2;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kMaster }]
            = TSS::ColourSet::k1;
        expect(layer.setZoneMap(layer.activeSkinId(), map).wasOk());

        const auto original = layer.makePushPayload();
        const auto encoded = juce::JSON::toString(TSS::DevSkin::pushPayloadToVar(original));

        TSS::DevSkin::PushPayload restored;
        juce::String error;
        expect(TSS::DevSkin::pushPayloadFromVar(juce::JSON::parse(encoded), restored, error));
        expectEquals(static_cast<int>(restored.zoneMap.size()), 2);
        expect(restored.zoneMap.at(TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider,
                                                         TSS::SkinZone::kPatch })
               == TSS::ColourSet::k2);
        expect(restored.zoneMap.at(TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider,
                                                         TSS::SkinZone::kMaster })
               == TSS::ColourSet::k1);

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(restored).ok);
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k2);
        TSS::clearSkinZoneOverlay();
    }

    void assertMissingZoneMapDefaults()
    {
        TSS::clearSkinZoneOverlay();

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "LegacyPush").wasOk());
        auto payloadVar = TSS::DevSkin::pushPayloadToVar(layer.makePushPayload());
        auto* obj = payloadVar.getDynamicObject();
        expect(obj != nullptr);
        obj->removeProperty("zoneMap");

        TSS::DevSkin::PushPayload restored;
        juce::String error;
        expect(TSS::DevSkin::pushPayloadFromVar(payloadVar, restored, error));
        expect(restored.zoneMap.empty());

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(restored).ok);
        expect(TSS::hasSkinZoneOverlay());
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k1);
        TSS::clearSkinZoneOverlay();
    }

    void assertOverlayLedRemap()
    {
        TSS::clearSkinZoneOverlay();

        TSS::SkinZoneMap map;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kLed, TSS::SkinZone::kHeader }]
            = TSS::ColourSet::k2;
        TSS::setSkinZoneOverlay(map);

        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kLed, TSS::SkinZone::kHeader)
               == TSS::ColourSet::k2);
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kLed, TSS::SkinZone::kModales)
               == TSS::ColourSet::k2); // default Modales Led cs2 when not in map

        TSS::clearSkinZoneOverlay();
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kLed, TSS::SkinZone::kHeader)
               == TSS::ColourSet::k1);
    }
};

static SkinZoneRegistryOverlayTests skinZoneRegistryOverlayTests;
