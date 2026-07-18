#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"

namespace
{
    class MinimalAudioProcessor : public juce::AudioProcessor
    {
    public:
        MinimalAudioProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", {})
        {
        }

        juce::AudioProcessorValueTreeState apvts;

        const juce::String getName() const override { return "Test"; }
        void prepareToPlay(double, int) override {}
        void releaseResources() override {}
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
        juce::AudioProcessorEditor* createEditor() override { return nullptr; }
        bool hasEditor() const override { return false; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
        bool isMidiEffect() const override { return false; }
        double getTailLengthSeconds() const override { return 0.0; }
        int getNumPrograms() override { return 1; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int) override {}
        const juce::String getProgramName(int) override { return {}; }
        void changeProgramName(int, const juce::String&) override {}
        void getStateInformation(juce::MemoryBlock&) override {}
        void setStateInformation(const void*, int) override {}
    };

    bool waitForQueueEmpty(Core::MidiOutboundQueue& queue, int timeoutMs)
    {
        const auto deadline = juce::Time::getMillisecondCounter()
                              + static_cast<juce::uint32>(timeoutMs);

        while (juce::Time::getMillisecondCounter() < deadline)
        {
            if (queue.isEmpty())
                return true;

            juce::Thread::sleep(1);
        }

        return queue.isEmpty();
    }

    juce::String firstAvailableOutputDeviceId()
    {
        const auto devices = juce::MidiOutput::getAvailableDevices();
        return devices.isEmpty() ? juce::String() : devices.getReference(0).identifier;
    }

    bool openFirstAvailableOutputOrSkip(MidiManager& manager, juce::UnitTest& test)
    {
        const auto outputId = firstAvailableOutputDeviceId();
        if (outputId.isEmpty())
        {
            test.logMessage("Skipped — no MIDI output device available");
            return false;
        }

        if (!manager.setMidiOutputPort(outputId))
        {
            test.logMessage("Skipped — MIDI output port could not be opened");
            return false;
        }

        return true;
    }
}

class MidiManagerTests : public juce::UnitTest
{
public:
    MidiManagerTests() : juce::UnitTest("MidiManager Tests") {}

    void runTest() override
    {
        testRealtimeRetainedWithoutOutput();
        testSysExRetainedWithoutOutput();
        testQueuedSysExGateSharingTwoMessagesDrain();
        testNoOutputPortDoesNotThrow();
        testRealtimeDispatchesAfterOutputPortOpened();
        testEmptySysExPayloadSkipped();
        testRealtimeNotStarvedDuringSysExGate();
        testDeviceDumpUnavailableWithoutDevice();
        testWaitUntilOutboundQueueIdleReturnsTrueWhenEmpty();
        testRefreshInquiryClearsDetectionWithoutPorts();
        testEditorOutboundGateBlocksProgramChangeWhenUndetected();
        testEditorOutboundGateBlocksSysExWhenUndetected();
        testEditorOutboundGateAllowsSendWhenDetected();
    }

private:
    void testDeviceDumpUnavailableWithoutDevice()
    {
        beginTest("Device dump unavailable when no output/device detected");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        // No MIDI ports open at construction.
        expect(!manager.isDeviceDumpAvailable(),
               "Device dump must be unavailable without open MIDI input and output ports");
    }

    void testRefreshInquiryClearsDetectionWithoutPorts()
    {
        beginTest("refreshDeviceInquiryAfterPortSync clears stale detection without ports");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty("deviceVersion", "1.20", nullptr);
        proc.apvts.state.setProperty("deviceType", "Matrix-1000", nullptr);

        manager.refreshDeviceInquiryAfterPortSync();

        expect(! static_cast<bool>(proc.apvts.state.getProperty("deviceDetected")),
               "Detection must clear when MIDI From/To are not both available");
        expectEquals(proc.apvts.state.getProperty("deviceType").toString(), juce::String("Unknown"));
        expect(proc.apvts.state.getProperty("deviceVersion").toString().isEmpty(),
               "deviceVersion must clear with detection");
    }

    void testWaitUntilOutboundQueueIdleReturnsTrueWhenEmpty()
    {
        beginTest("waitUntilOutboundQueueIdle returns true when queue already empty");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        const auto startMs = juce::Time::getMillisecondCounter();
        const bool idle = manager.waitUntilOutboundQueueIdle(200);
        const auto elapsedMs = juce::Time::getMillisecondCounter() - startMs;

        expect(idle, "Empty queue should report idle immediately");
        expect(elapsedMs < 100, "Idle wait should return promptly on an empty queue");
    }

    void testEditorOutboundGateBlocksProgramChangeWhenUndetected()
    {
        beginTest("FR-2 — Program Change not enqueued when deviceDetected=false");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);
        expect(! manager.isEditorOutboundAllowed());

        manager.startThread();
        manager.sendProgramChange(42, 1);
        juce::Thread::sleep(50);

        expect(queue.isEmpty(), "Program Change must not enqueue while locked");
        manager.stopThread(2000);
    }

    void testEditorOutboundGateBlocksSysExWhenUndetected()
    {
        beginTest("FR-2 — editor SysEx not enqueued when deviceDetected=false");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", false, nullptr);

        manager.startThread();
        manager.enqueueRemoteParameterEdit(10, 64);
        manager.sendSetBank(1);
        juce::Thread::sleep(50);

        expect(queue.isEmpty(), "Editor SysEx must not enqueue while locked");
        manager.stopThread(2000);
    }

    void testEditorOutboundGateAllowsSendWhenDetected()
    {
        beginTest("FR-2 — Program Change and SysEx enqueue when deviceDetected=true");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        expect(manager.isEditorOutboundAllowed());

        manager.startThread();
        manager.sendProgramChange(7, 1);
        manager.enqueueRemoteParameterEdit(5, 32);
        juce::Thread::sleep(50);

        expect(! queue.isEmpty(), "Editor outbound must enqueue once unlocked");
        manager.stopThread(2000);
    }

    void testRealtimeRetainedWithoutOutput()
    {
        beginTest("Enqueue realtime — queue retained when no output port");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);

        manager.startThread();
        manager.sendProgramChange(42, 1);

        juce::Thread::sleep(50);
        expect(!queue.isEmpty(), "Realtime message should remain queued without output port");
        manager.stopThread(2000);
        expect(!manager.isThreadRunning(), "MIDI thread should stop cleanly");
    }

    void testSysExRetainedWithoutOutput()
    {
        beginTest("Enqueue SysEx — queue retained when no output port");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);

        manager.startThread();
        manager.enqueueRemoteParameterEdit(10, 64);

        juce::Thread::sleep(50);
        expect(!queue.isEmpty(), "SysEx message should remain queued without output port");
        manager.stopThread(2000);
        expect(!manager.isThreadRunning(), "MIDI thread should stop cleanly");
    }

    void testQueuedSysExGateSharingTwoMessagesDrain()
    {
        beginTest("Queued SysEx gate sharing (sendSysExWithDelay) — two SysEx drain without hang");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);

        if (!openFirstAvailableOutputOrSkip(manager, *this))
            return;

        manager.startThread();

        manager.enqueueRemoteParameterEdit(1, 10);
        manager.enqueueRemoteParameterEdit(2, 20);

        expect(waitForQueueEmpty(queue, 5000),
               "Two queued SysEx messages should drain via sendSysExWithDelay gate sharing");

        manager.stopThread(2000);
        expect(!manager.isThreadRunning(), "MIDI thread should stop cleanly after SysEx drain");
    }

    void testNoOutputPortDoesNotThrow()
    {
        beginTest("No output port — enqueue does not throw and retains messages");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);

        manager.startThread();

        bool threw = false;
        try
        {
            manager.sendProgramChange(7, 1);
            manager.enqueueRemoteParameterEdit(5, 32);
            juce::Thread::sleep(50);
            expect(!queue.isEmpty(), "Queue should retain messages when output unavailable");
        }
        catch (...)
        {
            threw = true;
        }

        expect(!threw, "Enqueue with no output port must not throw");
        manager.stopThread(2000);
    }

    void testRealtimeDispatchesAfterOutputPortOpened()
    {
        beginTest("Realtime message dispatches after output port becomes available");

        const auto outputId = firstAvailableOutputDeviceId();
        if (outputId.isEmpty())
        {
            logMessage("Skipped — no MIDI output device available");
            return;
        }

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        manager.startThread();
        queue.enqueueRealtime(juce::MidiMessage::noteOn(1, 60, static_cast<juce::uint8>(100)));

        juce::Thread::sleep(50);
        expect(!queue.isEmpty(), "Message should wait until output port is opened");

        if (!manager.setMidiOutputPort(outputId))
        {
            logMessage("Skipped — MIDI output port could not be opened");
            manager.stopThread(2000);
            return;
        }

        expect(waitForQueueEmpty(queue, 2000), "Message should dispatch after output port opens");
        expect(tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound) > 0.0f,
               "Outbound activity should be recorded after successful send");

        manager.stopThread(2000);
    }

    void testEmptySysExPayloadSkipped()
    {
        beginTest("Empty SysEx payload — dequeued and skipped without sendSysExWithDelay");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        if (!openFirstAvailableOutputOrSkip(manager, *this))
            return;

        manager.startThread();
        queue.enqueueSysEx(juce::MemoryBlock());

        expect(waitForQueueEmpty(queue, 2000), "Empty SysEx should still be dequeued when output is available");
        manager.stopThread(2000);
    }

    void testRealtimeNotStarvedDuringSysExGate()
    {
        beginTest("Realtime MIDI drains while SysEx inter-message gate is active");

        const auto outputId = firstAvailableOutputDeviceId();
        if (outputId.isEmpty())
        {
            logMessage("Skipped — no MIDI output device available");
            return;
        }

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);

        if (!manager.setMidiOutputPort(outputId))
        {
            logMessage("Skipped — MIDI output port could not be opened");
            return;
        }

        manager.startThread();

        constexpr int kRealtimeBurstCount = 50;
        manager.enqueueRemoteParameterEdit(1, 10);

        for (int i = 0; i < kRealtimeBurstCount; ++i)
            queue.enqueueRealtime(juce::MidiMessage::noteOff(1, static_cast<int>(i % 128)));

        manager.enqueueRemoteParameterEdit(2, 20);

        const auto startMs = juce::Time::getMillisecondCounter();
        expect(waitForQueueEmpty(queue, 3000),
               "Realtime burst should not be blocked by SysEx inter-message gate");
        const auto elapsedMs = juce::Time::getMillisecondCounter() - startMs;

        expect(elapsedMs < 500,
               "Draining realtime during SysEx gate should complete well under gate*N blocking");

        manager.stopThread(2000);
    }
};

static MidiManagerTests midiManagerTests;
