#include <juce_core/juce_core.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "GUI/Skins/Dev/DevSkinSession.h"

class DevSkinLaunchProductTests : public juce::UnitTest
{
public:
    DevSkinLaunchProductTests()
        : juce::UnitTest("DevSkinLaunchProduct")
    {
    }

    void runTest() override
    {
        beginTest("launchProduct_forceFirst_ignoresSessionCream");
        assertLaunchProductForceIgnoresSessionCream();

        beginTest("launchProduct_forceFirst_onDefaultAndInvalidId");
        assertLaunchProductForceOnDefaultAndInvalidId();

        beginTest("midSessionProduct_creamRoundTrip_withoutLaunchForce");
        assertMidSessionProductCreamRoundTripWithoutLaunchForce();
    }

private:
    void assertLaunchProductForceIgnoresSessionCream()
    {
        TSS::DevSkin::Layer layer;
        expect(layer.duplicateSkin(TSS::DevSkin::kCompiledCreamId, "Keep").wasOk());
        TSS::DevSkin::EditorSessionState seed;
        seed.productSkinId = TSS::DevSkin::kCompiledCreamId;
        seed.editorSkinId = layer.activeSkinId();
        const auto json = TSS::DevSkin::exportSessionToJson(layer, seed);

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restored, restoredState, json).ok);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledCreamId));

        TSS::DevSkin::forceProductToFirstAvailable(restoredState);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
    }

    void assertLaunchProductForceOnDefaultAndInvalidId()
    {
        TSS::DevSkin::EditorSessionState defaults;
        expectEquals(defaults.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));

        TSS::DevSkin::Layer layer;
        TSS::DevSkin::EditorSessionState seed;
        seed.productSkinId = TSS::DevSkin::kCompiledCreamId;
        auto json = TSS::DevSkin::exportSessionToJson(layer, seed);
        auto parsed = juce::JSON::parse(json);
        auto* root = parsed.getDynamicObject();
        expect(root != nullptr);
        root->setProperty("productSkinId", "not-a-product");
        json = juce::JSON::toString(parsed, true);

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restored, restoredState, json).ok);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));

        TSS::DevSkin::forceProductToFirstAvailable(restoredState);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledBlackId));
    }

    void assertMidSessionProductCreamRoundTripWithoutLaunchForce()
    {
        TSS::DevSkin::Layer layer;
        TSS::DevSkin::EditorSessionState seed;
        seed.productSkinId = TSS::DevSkin::kCompiledCreamId;
        const auto json = TSS::DevSkin::exportSessionToJson(layer, seed);

        TSS::DevSkin::Layer restored;
        TSS::DevSkin::EditorSessionState restoredState;
        expect(TSS::DevSkin::importSessionFromJson(restored, restoredState, json).ok);
        expectEquals(restoredState.productSkinId, juce::String(TSS::DevSkin::kCompiledCreamId));
    }
};

static DevSkinLaunchProductTests devSkinLaunchProductTests;
