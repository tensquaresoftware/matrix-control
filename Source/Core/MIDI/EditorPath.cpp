#include "Core/MIDI/EditorPath.h"

namespace Core
{
    EditorPath::EditorPath(MidiOutboundQueue& queue, MidiActivityTracker& tracker) noexcept
        : queue_(queue)
        , tracker_(tracker)
    {
    }

    void EditorPath::enqueueSysEx(juce::MemoryBlock sysEx)
    {
        queue_.enqueueSysEx(std::move(sysEx));
        tracker_.notifyActivity(MidiActivityTracker::Path::kEditor);
    }

    void EditorPath::enqueueProgramChange(int programNumber, int channel)
    {
        const int clampedChannel = juce::jlimit(1, 16, channel);
        const int clampedProgram = juce::jlimit(0, 127, programNumber);
        queue_.enqueueRealtime(juce::MidiMessage::programChange(clampedChannel, clampedProgram));
        tracker_.notifyActivity(MidiActivityTracker::Path::kEditor);
    }
}
