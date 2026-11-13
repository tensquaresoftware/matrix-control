#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"
#include "GUI/Skins/Dev/DevSkinTypes.h"

class DevSkinLaunchProductTests : public juce::UnitTest
{
public:
    DevSkinLaunchProductTests()
        : juce::UnitTest("DevSkinLaunchProduct")
    {
    }

    void runTest() override
    {
        beginTest("launchProduct_sessionDoesNotPersistProduct");
        assertSessionDoesNotPersistProduct();

        beginTest("launchProduct_forceFirst_onDefaultAndInvalidId");
        assertLaunchProductForceOnDefaultAndInvalidId();

        beginTest("blankLaunch_sessionNeverRestoresDesignerSkin");
        assertBlankLaunchRestoresNoDesignerSkin();
    }

private:
    void assertSessionDoesNotPersistProduct()
    {
        TSS::DevSkin::DesignerSessionState seed;
        seed.productSkinId = TSS::DevSkin::kCompiledCreamId;
        const auto json = TSS::DevSkin::exportSessionToJson(seed);
        expect(! json.contains("productSkinId"));

        TSS::DevSkin::DesignerSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restoredState, json).ok);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
    }

    void assertLaunchProductForceOnDefaultAndInvalidId()
    {
        TSS::DevSkin::DesignerSessionState defaults;
        expectEquals(defaults.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));

        TSS::DevSkin::DesignerSessionState seed;
        auto json = TSS::DevSkin::exportSessionToJson(seed);
        auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->setProperty("productSkinId", "not-a-product");
        json = juce::JSON::toString(parsed, true);

        TSS::DevSkin::DesignerSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restoredState, json).ok);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));

        restoredState.productSkinId = TSS::DevSkin::kCompiledCreamId;
        TSS::DevSkin::forceProductToFirstAvailable(restoredState);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
    }

    void assertBlankLaunchRestoresNoDesignerSkin()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Prior Work").wasOk());

        TSS::DevSkin::DesignerSessionState seed;
        seed.designerSkinId = layer.activeSkinId();
        const auto json = TSS::DevSkin::exportSessionToJson(seed);

        TSS::DevSkin::DesignerSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restoredState, json).ok);
        expect(restoredState.designerSkinId.isEmpty());
        expect(! restoredState.compareOn);

        // A fresh layer (what the app builds at launch) holds compiled Product skins only.
        TSS::DevSkin::Layer launchLayer;
        expectEquals(launchLayer.skins().size(), static_cast<size_t>(6));
        const auto* creamChroma = launchLayer.findSkin(TSS::DevSkin::kCompiledCreamChromaId);
        expect(creamChroma != nullptr);
        if (creamChroma != nullptr)
            expectEquals(creamChroma->name, juce::String("CREAM/CHROMA"));
    }
};

static DevSkinLaunchProductTests devSkinLaunchProductTests;
