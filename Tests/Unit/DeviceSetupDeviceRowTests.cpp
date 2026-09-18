#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Services/DeviceSetupDeviceRow.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

class DeviceSetupDeviceRowTests : public juce::UnitTest
{
public:
    DeviceSetupDeviceRowTests() : juce::UnitTest("DeviceSetupDeviceRow") {}

    void runTest() override
    {
        testShouldOpenAssistant();
        testPartialPortsStayNotConnected();
        testSearchingWhileWindowActive();
        testBothPortsWithoutActiveWindowStayNotConnected();
        testSearchingWindowStartExhaustRestart();
        testSearchingWindowUnresponsiveSetsExhausted();
        testSearchingWindowExhaustClearPortSamePairRestart();
        testInquirySuccessShowsConnected();
        testUnresponsiveSettlesNotConnected();
        testConnectedDetailFormat();
        testConfirmPersistsEpromAndFinishes();
        testSpecifyLaterFinishFlags();
        testNextEpromPreferredIdBranches();
        testFamilyChangeCoercesGligliOnMatrix6();
    }

private:
    void testShouldOpenAssistant()
    {
        beginTest("shouldOpenDeviceSetupAssistant - promptDone or visible blocks");

        expect(Core::shouldOpenDeviceSetupAssistant(false, false));
        expect(! Core::shouldOpenDeviceSetupAssistant(true, false));
        expect(! Core::shouldOpenDeviceSetupAssistant(false, true));
        expect(! Core::shouldOpenDeviceSetupAssistant(true, true));
        expect(! Core::shouldOpenDeviceSetupAssistant(false, false, true));
        expect(Core::shouldOpenDeviceSetupAssistant(false, false, false));
    }

    void testPartialPortsStayNotConnected()
    {
        beginTest("partial ports - NOT CONNECTED even if searching flag set");

        const auto fromOnly = Core::resolveDeviceSetupDeviceRow({
            .midiFromReady = true,
            .midiToReady = false,
            .searchingWindowActive = true,
        });
        expect(fromOnly.kind == Core::DeviceSetupDeviceRowKind::kNotConnected);
        expectEquals(fromOnly.detailText,
                     juce::String(PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail));

        const auto toOnly = Core::resolveDeviceSetupDeviceRow({
            .midiFromReady = false,
            .midiToReady = true,
            .searchingWindowActive = true,
        });
        expect(toOnly.kind == Core::DeviceSetupDeviceRowKind::kNotConnected);
    }

    void testSearchingWhileWindowActive()
    {
        beginTest("both ports + active searching window - SEARCHING");

        const auto view = Core::resolveDeviceSetupDeviceRow({
            .midiFromReady = true,
            .midiToReady = true,
            .deviceDetected = false,
            .searchingWindowActive = true,
        });
        expect(view.kind == Core::DeviceSetupDeviceRowKind::kSearching);
        expect(! view.identityOk);
        expectEquals(view.detailText,
                     juce::String(PluginDisplayNames::Dialogs::EpromTypePrompt::kSearching));
        expect(Core::shouldRunDeviceSetupSearchingWindow(true, true, false, false));
    }

    void testBothPortsWithoutActiveWindowStayNotConnected()
    {
        beginTest("both ports + inactive searching window - NOT CONNECTED");

        const auto view = Core::resolveDeviceSetupDeviceRow({
            .midiFromReady = true,
            .midiToReady = true,
            .deviceDetected = false,
            .searchingWindowActive = false,
        });
        expect(view.kind == Core::DeviceSetupDeviceRowKind::kNotConnected);
        expectEquals(view.detailText,
                     juce::String(PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail));
    }

    void testSearchingWindowStartExhaustRestart()
    {
        beginTest("searching window - start, exhaust after timeout, restart on port-pair change");

        Core::DeviceSetupSearchingWindowState state;
        auto started = Core::advanceDeviceSetupSearchingWindow(state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 1000,
        });
        expect(started.state.active);
        expect(! started.state.exhausted);
        expect(started.shouldKickInquiry);
        expectEquals(started.state.trackedFromId, juce::String("in-a"));
        expectEquals(started.state.trackedToId, juce::String("out-a"));

        auto sameTick = Core::advanceDeviceSetupSearchingWindow(started.state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 1500,
        });
        expect(sameTick.state.active);
        expect(! sameTick.state.exhausted);
        expect(! sameTick.shouldKickInquiry);

        auto exhausted = Core::advanceDeviceSetupSearchingWindow(started.state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 1000 + static_cast<juce::uint32>(SysExConstants::kDefaultTimeoutMs),
        });
        expect(! exhausted.state.active);
        expect(exhausted.state.exhausted);
        expect(! exhausted.shouldKickInquiry);

        auto restarted = Core::advanceDeviceSetupSearchingWindow(exhausted.state, {
            .midiFromId = "in-b",
            .midiToId = "out-a",
            .nowMs = 5000,
        });
        expect(restarted.state.active);
        expect(! restarted.state.exhausted);
        expect(restarted.shouldKickInquiry);
        expectEquals(restarted.state.trackedFromId, juce::String("in-b"));
    }

    void testSearchingWindowUnresponsiveSetsExhausted()
    {
        beginTest("searching window - unresponsive stop sets exhausted");

        Core::DeviceSetupSearchingWindowState state;
        auto started = Core::advanceDeviceSetupSearchingWindow(state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 1000,
        });
        expect(started.state.active);
        expect(! started.state.exhausted);

        auto unresponsive = Core::advanceDeviceSetupSearchingWindow(started.state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .deviceMidiUnresponsive = true,
            .nowMs = 1500,
        });
        expect(! unresponsive.state.active);
        expect(unresponsive.state.exhausted);
        expect(! unresponsive.shouldKickInquiry);
    }

    void testSearchingWindowExhaustClearPortSamePairRestart()
    {
        beginTest("searching window - exhaust, clear a port, then same-pair restart");

        Core::DeviceSetupSearchingWindowState state;
        auto started = Core::advanceDeviceSetupSearchingWindow(state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 1000,
        });

        auto exhausted = Core::advanceDeviceSetupSearchingWindow(started.state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 1000 + static_cast<juce::uint32>(SysExConstants::kDefaultTimeoutMs),
        });
        expect(! exhausted.state.active);
        expect(exhausted.state.exhausted);

        auto cleared = Core::advanceDeviceSetupSearchingWindow(exhausted.state, {
            .midiFromId = {},
            .midiToId = "out-a",
            .nowMs = 4000,
        });
        expect(! cleared.state.active);
        expect(! cleared.state.exhausted);

        auto restarted = Core::advanceDeviceSetupSearchingWindow(cleared.state, {
            .midiFromId = "in-a",
            .midiToId = "out-a",
            .nowMs = 5000,
        });
        expect(restarted.state.active);
        expect(! restarted.state.exhausted);
        expect(restarted.shouldKickInquiry);
    }

    void testInquirySuccessShowsConnected()
    {
        beginTest("detected supported device - connected detail and OK chrome");

        const auto view = Core::resolveDeviceSetupDeviceRow({
            .midiFromReady = true,
            .midiToReady = true,
            .deviceDetected = true,
            .deviceMidiUnresponsive = false,
            .searchingWindowActive = true,
            .deviceType = MatrixDeviceTypes::Type::kMatrix1000,
        }, "1.20");

        expect(view.kind == Core::DeviceSetupDeviceRowKind::kConnected);
        expect(view.identityOk);
        expectEquals(view.detailText, juce::String("MATRIX-1000 (V1.20)"));
        expect(! Core::shouldRunDeviceSetupSearchingWindow(true, true, true, false));
    }

    void testUnresponsiveSettlesNotConnected()
    {
        beginTest("unresponsive - NOT CONNECTED even when searching window active");

        expect(! Core::isDeviceSetupIdentityOk(true, true, MatrixDeviceTypes::Type::kMatrix1000));

        const auto view = Core::resolveDeviceSetupDeviceRow({
            .midiFromReady = true,
            .midiToReady = true,
            .deviceDetected = true,
            .deviceMidiUnresponsive = true,
            .searchingWindowActive = true,
            .deviceType = MatrixDeviceTypes::Type::kMatrix1000,
        });
        expect(view.kind == Core::DeviceSetupDeviceRowKind::kNotConnected);
        expectEquals(view.detailText,
                     juce::String(PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail));
        expect(! Core::shouldRunDeviceSetupSearchingWindow(true, true, false, true));
    }

    void testConnectedDetailFormat()
    {
        beginTest("connected detail - unsupported shows UNKNOWN");

        expectEquals(Core::buildDeviceSetupConnectedDetail(MatrixDeviceTypes::Type::kUnknown, "1.0"),
                     juce::String(PluginDisplayNames::FooterPanel::kDeviceUnknownDetail));
        expectEquals(Core::buildDeviceSetupConnectedDetail(MatrixDeviceTypes::Type::kMatrix6, {}),
                     juce::String("MATRIX-6"));
    }

    void testConfirmPersistsEpromAndFinishes()
    {
        beginTest("deviceSetupConfirmResult - normalizes EPROM and finishes one-shot");

        const auto withDevice = Core::deviceSetupConfirmResult(
            PluginIDs::Settings::EpromType::kTauntek);
        expectEquals(withDevice.epromTypeId, PluginIDs::Settings::EpromType::kTauntek);
        expect(withDevice.promptDone);
        expect(! withDevice.promptPending);

        const auto withoutDevice = Core::deviceSetupConfirmResult(
            PluginIDs::Settings::EpromType::kUnknown);
        expectEquals(withoutDevice.epromTypeId, PluginIDs::Settings::EpromType::kUnknown);
        expect(withoutDevice.promptDone);
        expect(! withoutDevice.promptPending);
    }

    void testSpecifyLaterFinishFlags()
    {
        beginTest("deviceSetupSpecifyLaterFlags - promptDone true, pending false");

        const auto flags = Core::deviceSetupSpecifyLaterFlags();
        expect(flags.promptDone);
        expect(! flags.promptPending);
    }

    void testNextEpromPreferredIdBranches()
    {
        beginTest("nextDeviceSetupEpromPreferredId - userTouched keeps current; else preferred");

        expectEquals(Core::nextDeviceSetupEpromPreferredId(
                         false, PluginIDs::Settings::EpromType::kFactory,
                         PluginIDs::Settings::EpromType::kTauntek),
                     PluginIDs::Settings::EpromType::kTauntek);
        expectEquals(Core::nextDeviceSetupEpromPreferredId(
                         true, PluginIDs::Settings::EpromType::kGligli,
                         PluginIDs::Settings::EpromType::kTauntek),
                     PluginIDs::Settings::EpromType::kGligli);
        expectEquals(Core::nextDeviceSetupEpromPreferredId(
                         true, 0, PluginIDs::Settings::EpromType::kTauntek),
                     PluginIDs::Settings::EpromType::kTauntek);
    }

    void testFamilyChangeCoercesGligliOnMatrix6()
    {
        beginTest("EpromTypePolicy coerce - GLIGLI becomes UNKNOWN on Matrix-6 family");

        const int coerced = Core::EpromTypePolicy::coerceForDeviceFamily(
            PluginIDs::Settings::EpromType::kGligli,
            Core::MatrixDeviceFamily::kMatrix6Or6R);
        expectEquals(coerced, PluginIDs::Settings::EpromType::kUnknown);
    }
};

static DeviceSetupDeviceRowTests deviceSetupDeviceRowTests;
