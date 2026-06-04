#include "Core/MIDI/EditorPath.h"

namespace Core
{
    EditorPath::EditorPath(MidiOutboundQueue& queue) noexcept
        : queue_(queue)
    {
    }

    void EditorPath::enqueueSysEx(juce::MemoryBlock sysEx)
    {
        queue_.enqueueSysEx(std::move(sysEx));
    }

    void EditorPath::enqueueProgramChange(int programNumber, int channel)
    {
        const int clampedChannel = juce::jlimit(1, 16, channel);
        const int clampedProgram = juce::jlimit(0, 127, programNumber);
        queue_.enqueueRealtime(juce::MidiMessage::programChange(clampedChannel, clampedProgram));
    }
}
