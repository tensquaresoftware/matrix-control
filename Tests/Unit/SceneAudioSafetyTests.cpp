#include <juce_core/juce_core.h>

#include "Core/Audio/SceneAudioSafety.h"

class SceneAudioSafetyTests : public juce::UnitTest
{
public:
    SceneAudioSafetyTests() : juce::UnitTest("SceneAudioSafety") {}

    void runTest() override
    {
        testEmptySourceStaysEmpty();
        testStaleIdClearsWhenMissingFromCatalog();
        testIdentityChangeClearsEvenWhenIdStillListed();
        testSameDeviceKeepsValidSelection();
        testEmptyBoundIdentityInvalidatesLegacySelection();
        testEmptyCatalogClearsNonEmptySource();
        testInitEmptySourceNeverInventsFromChannelMode();
        testFirstRunGateTrueWhenUnset();
        testFirstRunGateFalseAfterApplied();
    }

private:
    void testEmptySourceStaysEmpty()
    {
        beginTest("Empty audioFromSourceId stays empty across sync");

        const juce::StringArray catalog { "mono:0", "stereo:0" };
        const auto decision = Core::decideAudioFromSelectionSync(
            {}, "Scarlett", "Scarlett", catalog);

        expect(decision.sourceIdToApply.isEmpty());
        expect(! decision.selectionKept);
        expect(decision.shouldClearBoundIdentity);
        expect(Core::isAudioFromSelectionValid({}, "Scarlett", "Scarlett", catalog));
    }

    void testStaleIdClearsWhenMissingFromCatalog()
    {
        beginTest("Saved mono:3 not in catalog clears to empty");

        const juce::StringArray catalog { "mono:0", "mono:1", "stereo:0" };
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:3", "Scarlett", "Scarlett", catalog);

        expect(decision.sourceIdToApply.isEmpty());
        expect(! decision.selectionKept);
        expect(decision.shouldClearBoundIdentity);
    }

    void testIdentityChangeClearsEvenWhenIdStillListed()
    {
        beginTest("Input identity change clears selection without remapping index");

        const juce::StringArray catalog { "mono:0", "stereo:0" };
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:0", "Scarlett", "MacBook Mic", catalog);

        expect(decision.sourceIdToApply.isEmpty());
        expect(! decision.selectionKept);
        expect(decision.shouldClearBoundIdentity);
        expect(! Core::isAudioFromSelectionValid(
            "mono:0", "Scarlett", "MacBook Mic", catalog));
    }

    void testSameDeviceKeepsValidSelection()
    {
        beginTest("Same device and listed id keeps selection");

        const juce::StringArray catalog { "mono:0", "mono:1", "stereo:0" };
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:0", "Scarlett", "Scarlett", catalog);

        expectEquals(decision.sourceIdToApply, juce::String("mono:0"));
        expect(decision.selectionKept);
        expect(! decision.shouldClearBoundIdentity);
    }

    void testEmptyBoundIdentityInvalidatesLegacySelection()
    {
        beginTest("Missing bound identity invalidates non-empty source id");

        const juce::StringArray catalog { "mono:0" };
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:0", {}, "Scarlett", catalog);

        expect(decision.sourceIdToApply.isEmpty());
        expect(decision.shouldClearBoundIdentity);
    }

    void testEmptyCatalogClearsNonEmptySource()
    {
        beginTest("Empty active-channel catalog clears non-empty saved source id");

        const juce::StringArray catalog;
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:0", "Scarlett", "Scarlett", catalog);

        expect(decision.sourceIdToApply.isEmpty());
        expect(! decision.selectionKept);
        expect(decision.shouldClearBoundIdentity);
    }

    void testInitEmptySourceNeverInventsFromChannelMode()
    {
        beginTest("Init resolution keeps empty source id for any channel mode");

        expect(Core::resolveAudioFromSourceIdAtInit({}).isEmpty());
        expectEquals(Core::resolveAudioFromSourceIdAtInit("mono:0"), juce::String("mono:0"));
        expectEquals(Core::resolveAudioFromSourceIdAtInit("stereo:0"), juce::String("stereo:0"));
    }

    void testFirstRunGateTrueWhenUnset()
    {
        beginTest("First-run gate applies when flag is unset");

        expect(Core::shouldApplySceneAudioSafetyDefaults(false));
    }

    void testFirstRunGateFalseAfterApplied()
    {
        beginTest("First-run gate skips when flag is already set");

        expect(! Core::shouldApplySceneAudioSafetyDefaults(true));
    }
};

static SceneAudioSafetyTests sceneAudioSafetyTests;
