#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinInventory.h"
#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinPaletteCatalog.h"

class DevSkinStarterKitTests : public juce::UnitTest
{
public:
    DevSkinStarterKitTests()
        : juce::UnitTest("DevSkinStarterKit")
    {
    }

    void runTest() override
    {
        beginTest("duplicateWithMappings_prunesUnmappedPastille");
        assertDuplicatePrunesUnmapped();

        beginTest("newDesignerSkin_paletteIsExactStarterKit");
        assertNewSkinExactStarterKit();

        beginTest("newDesignerSkin_clearsMappings_unboundEqualsInventory");
        assertNewDesignerSkinStartsFullyUnbound();

        beginTest("newDesignerSkin_mergesCustomPastillesFromCurrentDesigner");
        assertNewDesignerSkinMergesCustomPastilles();
    }

private:
    static bool paletteContains(const TSS::DevSkin::NamedSkin& skin, const juce::String& name)
    {
        for (const auto& pastille : skin.palette)
            if (pastille.name == name)
                return true;
        return false;
    }

    void assertDuplicatePrunesUnmapped()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Donor Mapped").wasOk());
        const auto donorId = layer.activeSkinId();
        expect(layer.addPastille(donorId, "kOrphanOnly", 0xffabcdef).wasOk());
        expect(paletteContains(*layer.findSkin(donorId), "kOrphanOnly"));

        expect(layer.duplicateSkin(donorId, "Copy Mapped", true).wasOk());
        const auto* copy = layer.findSkin(layer.activeSkinId());
        expect(copy != nullptr);
        expect(! paletteContains(*copy, "kOrphanOnly"));
        expect(paletteContains(*copy, "kBrandCream"));
    }

    void assertNewSkinExactStarterKit()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.createDesignerSkinWithUnboundMappings(TSS::DevSkin::kCompiledBlackId,
                                                         "Starter Only").wasOk());
        const auto* skin = layer.findSkin(layer.activeSkinId());
        expect(skin != nullptr);
        const auto kit = TSS::DevSkin::designerStarterKitPastilles();
        expectEquals(static_cast<int>(skin->palette.size()), static_cast<int>(kit.size()));
        for (size_t i = 0; i < kit.size(); ++i)
        {
            expectEquals(skin->palette[i].name, kit[i].name);
            expectEquals(static_cast<juce::int64>(skin->palette[i].argb),
                         static_cast<juce::int64>(kit[i].argb));
        }
        expect(static_cast<int>(skin->palette.size())
               < static_cast<int>(TSS::DevSkin::colourChartPastilles().size()));
    }

    void assertNewDesignerSkinStartsFullyUnbound()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.createDesignerSkinWithUnboundMappings(TSS::DevSkin::kCompiledBlackId,
                                                         "Blank Map").wasOk());
        const auto id = layer.activeSkinId();
        const auto* skin = layer.findSkin(id);
        expect(skin != nullptr);
        expect(skin->mappings.empty());
        expectEquals(static_cast<int>(skin->palette.size()),
                     static_cast<int>(TSS::DevSkin::designerStarterKitPastilles().size()));
        expectEquals(layer.unboundCount(id),
                     static_cast<int>(TSS::DevSkin::allStructuredKeys().size()));
        expect(! layer.isDesignerSkinExportAllowed(id));

        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Full Copy").wasOk());
        expectEquals(layer.unboundCount(layer.activeSkinId()), 0);
    }

    void assertNewDesignerSkinMergesCustomPastilles()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Donor").wasOk());
        const auto donorId = layer.activeSkinId();
        expect(layer.addPastille(donorId, "kCustomAccent", 0xffff00aa).wasOk());
        expect(layer.addPastille(donorId, "kCustomAccent Twin", 0xffff00aa).wasOk());
        expect(layer.addPastille(donorId, "kMyRedCopy", 0xffff0000).wasOk());

        expect(layer.createDesignerSkinWithUnboundMappings(TSS::DevSkin::kCompiledBlackId,
                                                         "Merged New",
                                                         donorId).wasOk());
        const auto* skin = layer.findSkin(layer.activeSkinId());
        expect(skin != nullptr);
        expect(skin->mappings.empty());

        bool foundCustom = false;
        bool foundTwin = false;
        bool foundProductArgbTwin = false;
        for (const auto& pastille : skin->palette)
        {
            if (pastille.name == "kCustomAccent" && pastille.argb == 0xffff00aa)
                foundCustom = true;
            if (pastille.name == "kCustomAccent Twin" && pastille.argb == 0xffff00aa)
                foundTwin = true;
            if (pastille.name == "kMyRedCopy" && pastille.argb == 0xffff0000)
                foundProductArgbTwin = true;
        }
        expect(foundCustom);
        expect(foundTwin);
        expect(foundProductArgbTwin);
    }
};

static DevSkinStarterKitTests devSkinStarterKitTests;
