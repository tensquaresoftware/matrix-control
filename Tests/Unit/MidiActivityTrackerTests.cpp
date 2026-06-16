#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"

class MidiActivityTrackerTests : public juce::UnitTest
{
public:
    MidiActivityTrackerTests() : juce::UnitTest("MidiActivityTracker") {}

    void runTest() override
    {
        testNotifySetsLevelToOne();
        testPathsAreIndependent();
        testOutboundPathIsIndependent();
        testMidiFromInboundPathIsIndependent();
        testPulseThenReturnsToZero();
        testRapidRenotifyExtendsPulse();
    }

private:
    void testNotifySetsLevelToOne()
    {
        beginTest("notifyActivity sets level to 1.0 immediately");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kInstrument);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 0.0f);
    }

    void testPathsAreIndependent()
    {
        beginTest("Instrument and editor paths are independent");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kEditor);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 0.0f);
    }

    void testOutboundPathIsIndependent()
    {
        beginTest("Outbound path is independent of instrument and editor paths");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kOutbound);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 0.0f);
    }

    void testMidiFromInboundPathIsIndependent()
    {
        beginTest("Midi-from inbound path is independent of other paths");

        Core::MidiActivityTracker tracker;
        tracker.notifyActivity(Core::MidiActivityTracker::Path::kMidiFromInbound);

        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kMidiFromInbound), 1.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kEditor), 0.0f);
        expectEquals(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound), 0.0f);
    }

    void testPulseThenReturnsToZero()
    {
        beginTest("Level returns to zero after pulse window");

        constexpr juce::int64 kBaseMs = 1'000'000;
        const juce::int64 pulseEnd = kBaseMs + 34;
        const juce::int64 afterPulse = kBaseMs + 35;

        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, kBaseMs), 1.0f);
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, pulseEnd), 1.0f);
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kBaseMs, afterPulse), 0.0f);
    }

    void testRapidRenotifyExtendsPulse()
    {
        beginTest("Re-notify within pulse window keeps level at 1.0");

        constexpr juce::int64 kFirstNotifyMs = 1'000'000;
        const juce::int64 kRenotifyMs = kFirstNotifyMs + 20;
        const juce::int64 kCheckMs = kRenotifyMs + 20;

        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kFirstNotifyMs, kFirstNotifyMs + 20), 1.0f);
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kRenotifyMs, kCheckMs), 1.0f);

        const juce::int64 kPastPulseMs = kFirstNotifyMs + 100;
        expectEquals(Core::MidiActivityTracker::computeLevelFromTimestamp(kFirstNotifyMs, kPastPulseMs), 0.0f);
    }
};

static MidiActivityTrackerTests midiActivityTrackerTests;
