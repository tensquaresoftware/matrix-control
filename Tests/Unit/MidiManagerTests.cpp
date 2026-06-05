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
        testRealtimeDrainWithoutOutput();
        testSysExDrainWithoutOutput();
        testQueuedSysExGateSharingTwoMessagesDrain();
        testNoOutputPortDoesNotThrow();
        testEmptySysExPayloadSkipped();
    }

private:
    void testRealtimeDrainWithoutOutput()
    {
        beginTest("Enqueue realtime — consumer drains queue (no output port)");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        manager.startThread();
        manager.sendProgramChange(42, 1);

        expect(waitForQueueEmpty(queue, 2000), "Realtime message should be dequeued");
        manager.stopThread(2000);
        expect(!manager.isThreadRunning(), "MIDI thread should stop cleanly");
    }

    void testSysExDrainWithoutOutput()
    {
        beginTest("Enqueue SysEx — consumer drains queue (no output port)");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        manager.startThread();
        manager.enqueueRemoteParameterEdit(10, 64);

        expect(waitForQueueEmpty(queue, 2000), "SysEx message should be dequeued");
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
        beginTest("No output port — enqueue + drain does not throw");

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
            expect(waitForQueueEmpty(queue, 2000), "Queue should empty when output unavailable");
        }
        catch (...)
        {
            threw = true;
        }

        expect(!threw, "Drain with no output port must not throw");
        manager.stopThread(2000);
    }

    void testEmptySysExPayloadSkipped()
    {
        beginTest("Empty SysEx payload — dequeued and skipped without sendSysExWithDelay");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        manager.startThread();
        queue.enqueueSysEx(juce::MemoryBlock());

        expect(waitForQueueEmpty(queue, 2000), "Empty SysEx should still be dequeued");
        manager.stopThread(2000);
    }
};

static MidiManagerTests midiManagerTests;
