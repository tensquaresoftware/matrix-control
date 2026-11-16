#include <juce_core/juce_core.h>

#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"
#include "GUI/Skins/Dev/DevSkinStandalonePaintState.h"
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

class SkinZoneMapLayerTests : public juce::UnitTest
{
public:
    SkinZoneMapLayerTests()
        : juce::UnitTest("SkinZoneMapLayer")
    {
    }

    void runTest() override
    {
        beginTest("setZoneMap_resetEmpty_pushRestoresRegistryDefaults");
        assertResetEmptyZoneMapRestoresDefaults();

        beginTest("setZoneMap_compiledRejected_andPerSkinMapsIsolated");
        assertCompiledRejectAndPerSkinIsolation();
    }

private:
    void assertResetEmptyZoneMapRestoresDefaults()
    {
        TSS::clearSkinZoneOverlay();

        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZoneReset").wasOk());
        const auto skinId = layer.activeSkinId();

        TSS::SkinZoneMap map;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k2;
        bool changed = false;
        layer.onChanged = [&] { changed = true; };
        const auto revBeforeAssign = layer.revision();
        expect(layer.setZoneMap(skinId, map).wasOk());
        expect(changed);
        expect(layer.revision() > revBeforeAssign);
        expect(ensureChartPastille(layer, skinId, "kGreen300"));
        expect(layer.setMapping(skinId,
                                TSS::skinColourKey(TSS::SkinObjectType::kSlider,
                                                   TSS::SkinColourProperty::kTrack,
                                                   TSS::SkinColourState::kDefault,
                                                   TSS::ColourSet::k2),
                                "kGreen300")
                   .wasOk());

        TSS::DevSkin::StandalonePaintState paint;
        expect(paint.applyPush(layer.makePushPayload()).ok);
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::ColourSet::k2);

        changed = false;
        const auto revBeforeReset = layer.revision();
        expect(layer.setZoneMap(skinId, {}).wasOk());
        expect(changed);
        expect(layer.revision() > revBeforeReset);
        const auto* skin = layer.findSkin(skinId);
        expect(skin != nullptr);
        expect(skin->zoneMap.empty());
        expect(paint.applyPush(layer.makePushPayload()).ok);
        expect(TSS::resolveZoneColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch)
               == TSS::defaultColourSet(TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch));
        TSS::clearSkinZoneOverlay();
    }

    void assertCompiledRejectAndPerSkinIsolation()
    {
        TSS::DevSkin::Layer layer;
        TSS::SkinZoneMap map;
        map[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k2;

        expect(layer.setZoneMap(TSS::DevSkin::kCompiledCreamId, map).failed());

        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZoneA").wasOk());
        const auto skinA = layer.activeSkinId();
        bool changed = false;
        layer.onChanged = [&] { changed = true; };
        const auto revBeforeA = layer.revision();
        expect(layer.setZoneMap(skinA, map).wasOk());
        expect(changed);
        expect(layer.revision() > revBeforeA);

        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "ZoneB").wasOk());
        const auto skinB = layer.activeSkinId();
        TSS::SkinZoneMap mapB;
        mapB[TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider, TSS::SkinZone::kPatch }]
            = TSS::ColourSet::k3;
        changed = false;
        const auto revBeforeB = layer.revision();
        expect(layer.setZoneMap(skinB, mapB).wasOk());
        expect(changed);
        expect(layer.revision() > revBeforeB);

        const auto* a = layer.findSkin(skinA);
        const auto* b = layer.findSkin(skinB);
        expect(a != nullptr && b != nullptr);
        expect(a->zoneMap.at(TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider,
                                                   TSS::SkinZone::kPatch })
               == TSS::ColourSet::k2);
        expect(b->zoneMap.at(TSS::SkinZoneMapKey { TSS::SkinObjectType::kSlider,
                                                   TSS::SkinZone::kPatch })
               == TSS::ColourSet::k3);
    }
};

static SkinZoneMapLayerTests skinZoneMapLayerTests;
