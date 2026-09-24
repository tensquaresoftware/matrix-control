#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "Core/MIDI/MidiPortStateCoherence.h"
#include "Core/MIDI/Queue/MidiRequestTiming.h"
#include "Core/MIDI/Queue/SysExDelayProfile.h"
#include "Shared/Definitions/PluginDisplayNames.h"

class MidiPortStateCoherenceTests : public juce::UnitTest
{
public:
    MidiPortStateCoherenceTests() : juce::UnitTest("MidiPortStateCoherence") {}

    void runTest() override
    {
        testCoherentPortIdAfterOpenAttempt();
        testIsMidiFromKeyboardFromConflict();
        testFooterConstants();
        testClearMidiFromKeyboardFromConflictFooterIfPresent();
        testMaybeAlignApvtsPortIdAfterOpenAttempt();
        testConflictRejectLeavesPeerRoleUnchanged();
        testSoftDeadPortUiRefreshPolicy();
    }

private:
    void testCoherentPortIdAfterOpenAttempt()
    {
        beginTest("coherentPortIdAfterOpenAttempt — success keeps requested id");
        expectEquals(Core::coherentPortIdAfterOpenAttempt(true, "port-a", "port-b"),
                     juce::String("port-a"));

        beginTest("coherentPortIdAfterOpenAttempt — failure mirrors open reality");
        expectEquals(Core::coherentPortIdAfterOpenAttempt(false, "port-a", juce::String()),
                     juce::String());
        expectEquals(Core::coherentPortIdAfterOpenAttempt(false, "port-a", "port-open"),
                     juce::String("port-open"));
    }

    void testIsMidiFromKeyboardFromConflict()
    {
        beginTest("isMidiFromKeyboardFromConflict — same non-empty ids conflict");
        expect(Core::isMidiFromKeyboardFromConflict("dev-1", "dev-1"));
        expect(! Core::isMidiFromKeyboardFromConflict("dev-1", "dev-2"));
        expect(! Core::isMidiFromKeyboardFromConflict({}, "dev-1"));
        expect(! Core::isMidiFromKeyboardFromConflict("dev-1", {}));
        expect(! Core::isMidiFromKeyboardFromConflict({}, {}));
    }

    void testFooterConstants()
    {
        beginTest("footer constants — Unknown and conflict copy are distinct");
        expect(juce::String(PluginDisplayNames::FooterPanel::kUnsupportedMatrixDeviceFooter)
               != juce::String(PluginDisplayNames::FooterPanel::kDeviceLockGuidance));
        expect(PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter != nullptr);
    }

    void testClearMidiFromKeyboardFromConflictFooterIfPresent()
    {
        beginTest("clearMidiFromKeyboardFromConflictFooterIfPresent — exact-string clear only");

        juce::ValueTree state("P");
        state.setProperty("uiMessageText",
                          PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter,
                          nullptr);
        state.setProperty("uiMessageSeverity", "warning", nullptr);
        Core::clearMidiFromKeyboardFromConflictFooterIfPresent(state);
        expect(state.getProperty("uiMessageText").toString().isEmpty());
        expect(state.getProperty("uiMessageSeverity").toString().isEmpty());

        state.setProperty("uiMessageText",
                          PluginDisplayNames::FooterPanel::kDeviceLockGuidance,
                          nullptr);
        state.setProperty("uiMessageSeverity", "error", nullptr);
        Core::clearMidiFromKeyboardFromConflictFooterIfPresent(state);
        expectEquals(state.getProperty("uiMessageText").toString(),
                     juce::String(PluginDisplayNames::FooterPanel::kDeviceLockGuidance));
    }

    void testMaybeAlignApvtsPortIdAfterOpenAttempt()
    {
        beginTest("maybeAlignApvtsPortIdAfterOpenAttempt — option 2 soft keeps desired id");
        {
            juce::ValueTree state("P");
            state.setProperty("midiInputPortId", "desired-dead", nullptr);
            Core::maybeAlignApvtsPortIdAfterOpenAttempt(
                state, "midiInputPortId", false, false, "desired-dead", {});
            expectEquals(state.getProperty("midiInputPortId").toString(), juce::String("desired-dead"));
        }

        beginTest("maybeAlignApvtsPortIdAfterOpenAttempt — reporting sync clears to open reality");
        {
            juce::ValueTree state("P");
            state.setProperty("midiInputPortId", "desired-dead", nullptr);
            Core::maybeAlignApvtsPortIdAfterOpenAttempt(
                state, "midiInputPortId", true, false, "desired-dead", {});
            expect(state.getProperty("midiInputPortId").toString().isEmpty());
        }
    }

    void testConflictRejectLeavesPeerRoleUnchanged()
    {
        beginTest("conflict reject leaves peer role unchanged — setter contract");

        // Mirrors PluginProcessor::setMidiInputPort / setKeyboardFromPort: on conflict return
        // false without writing the rejected role's port id.
        juce::ValueTree state("P");
        state.setProperty("midiInputPortId", "dev-a", nullptr);
        state.setProperty("keyboardFromPortId", "dev-b", nullptr);

        const juce::String attemptedKeyboard = "dev-a";
        expect(Core::isMidiFromKeyboardFromConflict(
            state.getProperty("midiInputPortId").toString(), attemptedKeyboard));

        // Rejected role unchanged:
        expectEquals(state.getProperty("keyboardFromPortId").toString(), juce::String("dev-b"));
        expectEquals(state.getProperty("midiInputPortId").toString(), juce::String("dev-a"));

        state.setProperty("uiMessageText",
                          PluginDisplayNames::FooterPanel::kMidiFromKeyboardFromConflictFooter,
                          nullptr);
        state.setProperty("uiMessageSeverity", "warning", nullptr);

        // Successful later selection clears conflict footer:
        state.setProperty("keyboardFromPortId", "dev-c", nullptr);
        Core::clearMidiFromKeyboardFromConflictFooterIfPresent(state);
        expect(state.getProperty("uiMessageText").toString().isEmpty());
    }

    void testSoftDeadPortUiRefreshPolicy()
    {
        beginTest("shouldForceReopenForUiRefresh — only when desired and not live");
        expect(Core::shouldForceReopenForUiRefresh("port-a", false));
        expect(! Core::shouldForceReopenForUiRefresh("port-a", true));
        expect(! Core::shouldForceReopenForUiRefresh({}, false));

        beginTest("shouldClearDeadPortAfterReopen — clear only on reopen failure");
        expect(Core::shouldClearDeadPortAfterReopen("port-a", false));
        expect(! Core::shouldClearDeadPortAfterReopen("port-a", true));
        expect(! Core::shouldClearDeadPortAfterReopen({}, false));
    }
};

static MidiPortStateCoherenceTests midiPortStateCoherenceTests;

class MidiRequestTimingTests : public juce::UnitTest
{
public:
    MidiRequestTimingTests() : juce::UnitTest("MidiRequestTiming") {}

    void runTest() override
    {
        beginTest("deviceSettleMs — floors at 50 ms and scales with profile delay");
        expectEquals(Core::MidiRequestTiming::deviceSettleMs(10), 50);
        expectEquals(Core::MidiRequestTiming::deviceSettleMs(20), 100);

        beginTest("outboundIdleTimeoutMs — floors at 500 ms and scales with profile delay");
        expectEquals(Core::MidiRequestTiming::outboundIdleTimeoutMs(10), 500);
        expectEquals(Core::MidiRequestTiming::outboundIdleTimeoutMs(20), 1000);

        beginTest("profile helpers — stock M-1000 keeps historical floors");
        const auto stock = Core::SysExDelayProfile::stockDefault();
        expectEquals(Core::MidiRequestTiming::deviceSettleMs(stock),
                     Core::MidiRequestTiming::kMinDeviceSettleMs);
        expectEquals(Core::MidiRequestTiming::outboundIdleTimeoutMs(stock),
                     Core::MidiRequestTiming::kMinOutboundIdleTimeoutMs);
    }
};

static MidiRequestTimingTests midiRequestTimingTests;
