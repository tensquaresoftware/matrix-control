#include "Core/MIDI/MidiPortOpenFeedback.h"

#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/Loggers/MidiLogger.h"
#include "Shared/Exceptions/WidgetFactoryExceptions.h"
#include "Shared/ProjectPaths.h"

#if JUCE_WINDOWS
namespace
{
constexpr const char* kWindowsMidiSetupDocRelativePath = "Documentation/windows-midi-multi-client.md";

juce::String windowsMidiSetupDocSuffix()
{
    juce::String suffix = " See ";
    suffix += kWindowsMidiSetupDocRelativePath;
    suffix += " for setup help.";

    const juce::File docFile = ProjectPaths::getProjectRoot()
                                   .getChildFile(kWindowsMidiSetupDocRelativePath);

    if (docFile.existsAsFile())
        suffix += " (" + docFile.getFullPathName() + ")";

    return suffix;
}
}
#endif

namespace Core::MidiPortOpenFeedback
{
juce::String failureReasonCode(MidiPortOpenFailureReason reason)
{
    switch (reason)
    {
        case MidiPortOpenFailureReason::kNotFound:
            return "not_found";
        case MidiPortOpenFailureReason::kOpenRejected:
            return "open_rejected";
        case MidiPortOpenFailureReason::kNone:
        default:
            return "none";
    }
}

juce::String formatFooterMessage(bool isInput,
                                 const juce::String& portDisplayName,
                                 MidiPortOpenFailureReason reason)
{
    const juce::String direction = isInput ? "MIDI From" : "MIDI To";
    const juce::String displayName = portDisplayName.isNotEmpty() ? portDisplayName : "Unknown port";

    if (reason == MidiPortOpenFailureReason::kNotFound)
    {
        return direction + ": \"" + displayName
               + "\" — port not found (device disconnected or unavailable).";
    }

    if (reason == MidiPortOpenFailureReason::kOpenRejected)
    {
        juce::String message = direction + ": \"" + displayName
                               + "\" — could not open (port may be in use by another application";

#if JUCE_WINDOWS
        message += ", e.g. your DAW. Try closing other apps or use a virtual MIDI port such as loopMIDI";
#else
        message += ". Try closing other applications using this port";
#endif

        message += ").";
#if JUCE_WINDOWS
        message += windowsMidiSetupDocSuffix();
#endif
        return message;
    }

    return {};
}

void logOpenFailure(bool isInput,
                    const juce::String& portDisplayName,
                    const juce::String& deviceId,
                    MidiPortOpenFailureReason reason)
{
    const juce::String direction = isInput ? "input" : "output";
    const juce::String message = "MIDI " + direction + " open failed"
                                 + " [port=" + portDisplayName + "]"
                                 + " [id=" + deviceId + "]"
                                 + " [reason=" + failureReasonCode(reason) + "]";
    MidiLogger::getInstance().logError(message);
}

void propagateOpenFailure(juce::AudioProcessorValueTreeState& apvts,
                          bool isInput,
                          const juce::String& portDisplayName,
                          MidiPortOpenFailureReason reason)
{
    if (reason == MidiPortOpenFailureReason::kNone)
        return;

    const juce::String message = formatFooterMessage(isInput, portDisplayName, reason);
    const WidgetFactoryException exception(message, WidgetFactoryErrors::Severity::Error);
    ExceptionPropagator::propagateToApvts(apvts, exception);
}
}
