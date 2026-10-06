#pragma once

#include <bitset>
#include <mutex>
#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>

namespace Core
{
    // Tracks notes currently held on the instrument path (keyboard / host MIDI).
    // Used by PANIC to emit Note Offs without flooding all 16×128 pitches.
    class ActiveNoteRegistry
    {
    public:
        void apply(const juce::MidiMessage& message)
        {
            if (! message.isNoteOnOrOff())
                return;

            const int channel = message.getChannel();
            const int note = message.getNoteNumber();
            if (channel < 1 || channel > 16 || note < 0 || note > 127)
                return;

            const std::size_t index = static_cast<std::size_t>((channel - 1) * 128 + note);
            const std::lock_guard<std::mutex> lock(mutex_);

            if (message.isNoteOn() && message.getVelocity() > 0)
                notes_.set(index);
            else
                notes_.reset(index);
        }

        // Snapshot held notes as Note Offs (channel-major), then clear the registry.
        std::vector<juce::MidiMessage> takeNoteOffsAndClear()
        {
            std::vector<juce::MidiMessage> noteOffs;
            const std::lock_guard<std::mutex> lock(mutex_);
            noteOffs.reserve(notes_.count());

            for (int channel = 1; channel <= 16; ++channel)
            {
                for (int note = 0; note < 128; ++note)
                {
                    const std::size_t index = static_cast<std::size_t>((channel - 1) * 128 + note);
                    if (! notes_.test(index))
                        continue;

                    noteOffs.push_back(juce::MidiMessage::noteOff(channel, note));
                }
            }

            notes_.reset();
            return noteOffs;
        }

        std::size_t count() const
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            return notes_.count();
        }

    private:
        mutable std::mutex mutex_;
        std::bitset<16 * 128> notes_;
    };
}
