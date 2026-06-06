#pragma once

#include <mutex>
#include <optional>
#include <queue>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

namespace Core
{
    // MidiOutboundQueue — MPSC outbound merge (AD-3, FR-7).
    //
    // Audit R-4 (2026-06-06): std::mutex + dual std::queue — MPSC-safe; no impl change required.
    // juce::AbstractFifo is SPSC-only; rejected in Story 2.1.
    //
    // Producers (concurrent, any thread):
    //   InstrumentMidiForwarder  — audio thread       → enqueueRealtime
    //   EditorPath               — message thread     → enqueueSysEx, enqueueRealtime (Program Change)
    //   KeyboardFromMidiInput    — MIDI input callback → enqueueRealtime
    //
    // Consumer (single thread only):
    //   MidiManager::run()       — dedicated MIDI thread → dequeue
    //
    // Priority: dequeue drains realtimeQueue_ before sysExQueue_ (FR-7).
    // Forbidden on producer threads: MidiOutput::sendMessageNow or blocking MIDI I/O.
    //
    // | Method          | Allowed threads                          |
    // |-----------------|------------------------------------------|
    // | enqueueRealtime | any producer (incl. audio, MIDI input)   |
    // | enqueueSysEx    | message / MIDI thread (EditorPath)       |
    // | dequeue         | MidiManager::run() only                  |
    // | isEmpty         | any (do not gate dequeue on prior isEmpty — TOCTOU) |
    class MidiOutboundQueue
    {
    public:
        enum class MessageCategory { kRealtime, kSysEx };

        struct Message
        {
            MessageCategory category;
            juce::MidiMessage midiMessage;
            juce::MemoryBlock sysExData;
        };

        MidiOutboundQueue() = default;

        void enqueueRealtime(juce::MidiMessage message);
        void enqueueSysEx(juce::MemoryBlock sysEx);

        std::optional<Message> dequeue();
        bool isEmpty() const noexcept;

    private:
        std::queue<juce::MidiMessage> realtimeQueue_;
        std::queue<juce::MemoryBlock> sysExQueue_;
        mutable std::mutex queueMutex_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiOutboundQueue)
    };
}
