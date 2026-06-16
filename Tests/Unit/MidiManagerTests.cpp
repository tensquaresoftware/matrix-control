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
    }

private:
    void testRealtimeRetainedWithoutOutput()
    {
        beginTest("Enqueue realtime — queue retained when no output port");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

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

        const auto outputId = firstAvailableOutputDeviceId();
        if (outputId.isNotEmpty())
            manager.setMidiOutputPort(outputId);

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

        expect(manager.setMidiOutputPort(outputId), "Output port should open");
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

        const auto outputId = firstAvailableOutputDeviceId();
        if (outputId.isNotEmpty())
            manager.setMidiOutputPort(outputId);

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

        manager.setMidiOutputPort(outputId);
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
