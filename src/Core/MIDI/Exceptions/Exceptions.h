#pragma once

#include <stdexcept>

#include <juce_core/juce_core.h>

class SysExException : public std::runtime_error
{
public:
    explicit SysExException(const juce::String& errorMessage)
        : std::runtime_error(errorMessage.toStdString()), message(errorMessage)
    {
    }

    const juce::String& getMessage() const noexcept { return message; }

private:
    juce::String message;
};

class MidiTimeoutException : public SysExException
{
public:
    explicit MidiTimeoutException(const juce::String& operation = "MIDI operation")
        : SysExException(operation + " timed out")
    {
    }
};

class MidiConnectionException : public SysExException
{
public:
    explicit MidiConnectionException(const juce::String& errorMessage)
        : SysExException("MIDI connection error: " + errorMessage)
    {
    }
};

