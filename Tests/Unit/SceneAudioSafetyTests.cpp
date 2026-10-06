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
        testEmptyCatalogDefersNonEmptySource();
        testEmptyCatalogClearsOnKnownIdentityMismatch();
        testInitEmptySourceNeverInventsFromChannelMode();
        testPreferredSkippedWhenSourceEmpty();
        testPreferredAllowedWhenSourceNonEmpty();
        testForceEndpointNoneWhenPersistedMissing();
        testKeepEndpointWhenPersistedStillAvailable();
        testIncompleteListsSkipMissingDevicePolicy();
        testOneSidedEmptyListSkipsThatEndpoint();
        testReadPersistedEndpointsFromSetupXml();
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

    void testEmptyCatalogDefersNonEmptySource()
    {
        beginTest("Empty active-channel catalog defers non-empty saved source id");

        const juce::StringArray catalog;
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:0", "Scarlett", "Scarlett", catalog);

        expectEquals(decision.sourceIdToApply, juce::String("mono:0"));
        expect(decision.shouldDefer);
        expect(! decision.selectionKept);
        expect(! decision.shouldClearBoundIdentity);
    }

    void testEmptyCatalogClearsOnKnownIdentityMismatch()
    {
        beginTest("Empty catalog still clears when Input identity already changed");

        const juce::StringArray catalog;
        const auto decision = Core::decideAudioFromSelectionSync(
            "mono:0", "Scarlett", "MacBook Mic", catalog);

        expect(decision.sourceIdToApply.isEmpty());
        expect(! decision.shouldDefer);
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

    void testPreferredSkippedWhenSourceEmpty()
    {
        beginTest("Preferred-after-inquiry does not apply when AUDIO FROM is None");

        expect(! Core::shouldApplyPreferredAudioFrom({}));
    }

    void testPreferredAllowedWhenSourceNonEmpty()
    {
        beginTest("Preferred-after-inquiry may remap an existing non-empty selection");

        expect(Core::shouldApplyPreferredAudioFrom("mono:0"));
    }

    void testForceEndpointNoneWhenPersistedMissing()
    {
        beginTest("Missing persisted device forces endpoint to None");

        const juce::StringArray available { "Micro MacBook Pro", "Haut-parleurs MacBook Pro" };
        expect(Core::shouldForceAudioEndpointToNone("Scarlett 2i2", available));
        expect(! Core::shouldForceAudioEndpointToNone({}, available));
    }

    void testKeepEndpointWhenPersistedStillAvailable()
    {
        beginTest("Persisted device still listed keeps endpoint");

        const juce::StringArray available { "Scarlett 2i2", "Micro MacBook Pro" };
        expect(! Core::shouldForceAudioEndpointToNone("Scarlett 2i2", available));
    }

    void testIncompleteListsSkipMissingDevicePolicy()
    {
        beginTest("Empty Input and Output lists are incomplete, not missing");

        expect(Core::areAudioDeviceNameListsStillIncomplete({}, {}));
        expect(! Core::areAudioDeviceNameListsStillIncomplete({ "Out" }, {}));
        expect(! Core::areAudioDeviceNameListsStillIncomplete({}, { "In" }));
    }

    void testOneSidedEmptyListSkipsThatEndpoint()
    {
        beginTest("One empty device list does not force that endpoint to None");

        expect(! Core::shouldApplyMissingDeviceNoneForEndpoint({}));
        expect(Core::shouldApplyMissingDeviceNoneForEndpoint({ "Scarlett 2i2" }));
    }

    void testReadPersistedEndpointsFromSetupXml()
    {
        beginTest("Persisted audioSetup XML yields input and output names");

        juce::XmlElement xml("DEVICESETUP");
        xml.setAttribute("audioInputDeviceName", "Scarlett In");
        xml.setAttribute("audioOutputDeviceName", "Scarlett Out");

        const auto endpoints = Core::readPersistedAudioEndpoints(&xml);
        expectEquals(endpoints.inputDeviceName, juce::String("Scarlett In"));
        expectEquals(endpoints.outputDeviceName, juce::String("Scarlett Out"));
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
