#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/ActiveNoteRegistry.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "MidiManagerTestSupport.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

using MidiManagerTestSupport::MinimalAudioProcessor;
using MidiManagerTestSupport::firstAvailableOutputDeviceId;
using MidiManagerTestSupport::openFirstAvailableOutputOrSkip;
using MidiManagerTestSupport::waitForQueueEmpty;

namespace
{
    class MidiChannelAudioProcessor : public juce::AudioProcessor
    {
    public:
        explicit MidiChannelAudioProcessor(int channelChoiceIndex)
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", makeLayout(channelChoiceIndex))
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

    private:
        static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(int channelChoiceIndex)
        {
            juce::AudioProcessorValueTreeState::ParameterLayout layout;

            for (const auto& d : PluginDescriptors::MasterEditSection::MidiModule::kChoiceParameters)
            {
                if (d.parameterId
                    != PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel)
                    continue;

                layout.add(std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(d.parameterId, 1),
                    d.displayName,
                    d.choices,
                    channelChoiceIndex));
            }

            return layout;
        }
    };

    void expectPanicControllersForChannel(juce::UnitTest& test,
                                          Core::MidiOutboundQueue& queue,
                                          int channel)
    {
        auto notesOff = queue.dequeue();
        test.expect(notesOff.has_value());
        test.expect(notesOff->midiMessage.isController());
        test.expectEquals(notesOff->midiMessage.getControllerNumber(), 123);
        test.expectEquals(notesOff->midiMessage.getControllerValue(), 0);
        test.expectEquals(notesOff->midiMessage.getChannel(), channel);

        auto reset = queue.dequeue();
        test.expect(reset.has_value());
        test.expect(reset->midiMessage.isController());
        test.expectEquals(reset->midiMessage.getControllerNumber(), 121);
        test.expectEquals(reset->midiMessage.getControllerValue(), 0);
        test.expectEquals(reset->midiMessage.getChannel(), channel);
    }

    void expectFallbackNoteOffSpray(juce::UnitTest& test, Core::MidiOutboundQueue& queue, int channel)
    {
        for (int note = 0; note < 128; ++note)
        {
            auto noteOff = queue.dequeue();
            test.expect(noteOff.has_value());
            test.expect(noteOff->midiMessage.isNoteOff());
            test.expectEquals(noteOff->midiMessage.getNoteNumber(), note);
            test.expectEquals(noteOff->midiMessage.getChannel(), channel);
        }
    }
}

class MidiManagerRealtimeTests : public juce::UnitTest
{
public:
    MidiManagerRealtimeTests() : juce::UnitTest("MidiManager Realtime") {}

    void runTest() override
    {
        testRealtimeRetainedWithoutOutput();
        testSysExRetainedWithoutOutput();
        testQueuedSysExGateSharingTwoMessagesDrain();
        testNoOutputPortDoesNotThrow();
        testRealtimeDispatchesAfterOutputPortOpened();
        testEmptySysExPayloadSkipped();
        testRealtimeNotStarvedDuringSysExGate();
        testSendPanicFallbackOmniUsesChannel1NoteOffsNotFullSpray();
        testSendPanicEnqueuesHeldNotesThenControllers();
        testSendPanicEnqueuesNoteOffSprayOnBasicChannel();
        testSendPanicCoalescesWhileRealtimeDepthHigh();
        testDrainRealtimeDoesNotReorderSysEx();
    }

private:
    void testRealtimeRetainedWithoutOutput()
    {
        beginTest("Enqueue realtime — queue retained when no output port");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

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
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

        manager.startThread();
        manager.enqueueRemoteParameterEdit(10, 64);

        juce::Thread::sleep(50);
        expect(!queue.isEmpty(), "SysEx message should remain queued without output port");
        manager.stopThread(2000);
        expect(!manager.isThreadRunning(), "MIDI thread should stop cleanly");
    }

    void testQueuedSysExGateSharingTwoMessagesDrain()
    {
        beginTest("Queued SysEx gate sharing — two SysEx drain without hang");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

        if (!openFirstAvailableOutputOrSkip(manager, *this))
            return;

        manager.startThread();
        manager.enqueueRemoteParameterEdit(1, 10);
        manager.enqueueRemoteParameterEdit(2, 20);

        expect(waitForQueueEmpty(queue, 5000),
               "Two queued SysEx messages should drain via gate sharing");
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
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

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
        beginTest("Empty SysEx payload — dequeued and skipped without blocking delay");

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
        proc.apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                      MatrixDeviceTypes::kMatrix1000Id,
                                      nullptr);

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

    void testSendPanicFallbackOmniUsesChannel1NoteOffsNotFullSpray()
    {
        beginTest("sendPanic Omni fallback sprays Note Off 0-127 on ch1 only, then CC on all 16");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        manager.sendPanic();

        expectEquals(static_cast<int>(queue.realtimeDepth()), 128 + (16 * 2));
        expectFallbackNoteOffSpray(*this, queue, 1);
        for (int channel = 1; channel <= 16; ++channel)
            expectPanicControllersForChannel(*this, queue, channel);
        expect(queue.isEmpty());
    }

    void testSendPanicEnqueuesHeldNotesThenControllers()
    {
        beginTest("sendPanic with registry emits Note Offs for held notes only, then CCs");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        Core::ActiveNoteRegistry notes;
        MidiChannelAudioProcessor proc(1);
        MidiManager manager(proc.apvts, queue, tracker);
        manager.setActiveNoteRegistry(&notes);

        notes.apply(juce::MidiMessage::noteOn(1, 60, 0.8f));
        notes.apply(juce::MidiMessage::noteOn(1, 64, 0.8f));

        manager.sendPanic();

        expectEquals(static_cast<int>(queue.realtimeDepth()), 2 + 2);
        expectEquals(static_cast<int>(notes.count()), 0);

        auto first = queue.dequeue();
        expect(first.has_value());
        expect(first->midiMessage.isNoteOff());
        expectEquals(first->midiMessage.getNoteNumber(), 60);

        auto second = queue.dequeue();
        expect(second.has_value());
        expect(second->midiMessage.isNoteOff());
        expectEquals(second->midiMessage.getNoteNumber(), 64);

        expectPanicControllersForChannel(*this, queue, 1);
        expect(queue.isEmpty());
    }

    void testSendPanicEnqueuesNoteOffSprayOnBasicChannel()
    {
        beginTest("sendPanic on basic channel 3 without held notes sprays Note Off 0-127 then CCs");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MidiChannelAudioProcessor proc(3); // choice index 3 → MIDI channel 3
        MidiManager manager(proc.apvts, queue, tracker);

        manager.sendPanic();

        expectEquals(static_cast<int>(queue.realtimeDepth()), 128 + 2);
        expectFallbackNoteOffSpray(*this, queue, 3);
        expectPanicControllersForChannel(*this, queue, 3);
        expect(queue.isEmpty());
    }

    void testSendPanicCoalescesWhileRealtimeDepthHigh()
    {
        beginTest("sendPanic no-ops while realtime depth is already high");

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        MinimalAudioProcessor proc;
        MidiManager manager(proc.apvts, queue, tracker);

        for (int i = 0; i < 32; ++i)
            queue.enqueueRealtime(juce::MidiMessage::noteOn(1, i, 0.5f));

        const auto depthBefore = queue.realtimeDepth();
        manager.sendPanic();
        expectEquals(static_cast<int>(queue.realtimeDepth()), static_cast<int>(depthBefore));
    }

    void testDrainRealtimeDoesNotReorderSysEx()
    {
        beginTest("tryDequeueRealtime drains realtime without touching SysEx FIFO");

        Core::MidiOutboundQueue queue;
        queue.enqueueSysEx(juce::MemoryBlock { "\xf0\x01\xf7", 3 });
        queue.enqueueSysEx(juce::MemoryBlock { "\xf0\x02\xf7", 3 });
        queue.enqueueRealtime(juce::MidiMessage::noteOff(1, 60));
        queue.enqueueRealtime(juce::MidiMessage::noteOff(1, 61));

        auto firstRt = queue.tryDequeueRealtime();
        expect(firstRt.has_value());
        expect(firstRt->isNoteOff());
        expectEquals(firstRt->getNoteNumber(), 60);

        auto secondRt = queue.tryDequeueRealtime();
        expect(secondRt.has_value());
        expectEquals(secondRt->getNoteNumber(), 61);

        expect(! queue.tryDequeueRealtime().has_value());
        expectEquals(static_cast<int>(queue.sysExDepth()), 2);

        auto firstSx = queue.dequeue();
        expect(firstSx.has_value());
        expectEquals(static_cast<int>(static_cast<juce::uint8>(firstSx->sysExData[1])), 0x01);

        auto secondSx = queue.dequeue();
        expect(secondSx.has_value());
        expectEquals(static_cast<int>(static_cast<juce::uint8>(secondSx->sysExData[1])), 0x02);
    }
};

static MidiManagerRealtimeTests midiManagerRealtimeTests;
