#pragma once

#include <functional>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Loggers/ApvtsLogger.h"
#include "Core/Loggers/LogSessionHeader.h"
#include "Core/Loggers/MidiLogger.h"
#include "Shared/ProjectPaths.h"

// Free helpers shared by PluginProcessor.cpp and its PluginProcessor*.cpp companions.
// Extracted from the PluginProcessor.cpp anonymous namespace for modular maintenance.
namespace PluginProcessorInternal
{
    inline bool isStandaloneWrapper()
    {
        return juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_Standalone;
    }

    inline bool isVst3Wrapper()
    {
        return juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_VST3;
    }

    inline void runSyncOnMessageThread(std::function<void()> task)
    {
        if (juce::MessageManager::existsAndIsCurrentThread())
        {
            task();
            return;
        }

        juce::MessageManager::callAsync(std::move(task));
    }

    inline bool isMidiInputDeviceAvailable(const juce::String& deviceId)
    {
        if (deviceId.isEmpty())
            return false;

        for (const auto& device : juce::MidiInput::getAvailableDevices())
        {
            if (device.identifier == deviceId)
                return true;
        }

        return false;
    }

    inline bool isMidiOutputDeviceAvailable(const juce::String& deviceId)
    {
        if (deviceId.isEmpty())
            return false;

        for (const auto& device : juce::MidiOutput::getAvailableDevices())
        {
            if (device.identifier == deviceId)
                return true;
        }

        return false;
    }

    inline juce::String sanitizePersistedMidiInputPortId(const juce::String& deviceId)
    {
        return isMidiInputDeviceAvailable(deviceId) ? deviceId : juce::String();
    }

    inline juce::String sanitizePersistedMidiOutputPortId(const juce::String& deviceId)
    {
        return isMidiOutputDeviceAvailable(deviceId) ? deviceId : juce::String();
    }

    inline bool isDevMidiLogCompileEnabled() noexcept
    {
#if defined(MC_DEV_MIDI_LOG) && MC_DEV_MIDI_LOG
        return true;
#else
        return false;
#endif
    }

    inline bool isDevApvtsLogCompileEnabled() noexcept
    {
#if defined(MC_DEV_APVTS_LOG) && MC_DEV_APVTS_LOG
        return true;
#else
        return false;
#endif
    }

    inline bool shouldUseDevelopmentMidiLogging()
    {
#if JUCE_DEBUG
        if (! isDevMidiLogCompileEnabled())
            return false;

        if (isStandaloneWrapper())
            return true;

        return ! ProjectPaths::isUsingFallbackRoot();
#else
        return false;
#endif
    }

    inline bool shouldUseDevelopmentApvtsLogging()
    {
#if JUCE_DEBUG
        if (! isDevApvtsLogCompileEnabled())
            return false;

        if (isStandaloneWrapper())
            return true;

        return ! ProjectPaths::isUsingFallbackRoot();
#else
        return false;
#endif
    }

    inline bool shouldUseDevelopmentLogging()
    {
        return shouldUseDevelopmentMidiLogging() || shouldUseDevelopmentApvtsLogging();
    }

    // Debug lab console verbosity only — never opens log files (SUPPORT owns file journals).
    inline void enableDevelopmentMidiConsoleLogging()
    {
        MidiLogger::getInstance().setLogLevel(MidiLogger::LogLevel::kDebug);
        MidiLogger::getInstance().setLogToConsole(true);
    }

    inline void enableDevelopmentApvtsConsoleLogging()
    {
        ApvtsLogger::getInstance().setLogLevel(ApvtsLogger::LogLevel::kDebug);
        ApvtsLogger::getInstance().setLogToConsole(true);
    }

    // Bodies shared by PluginProcessor::enableFileLoggingForSession / enableApvtsLogging
    // (when diagnostic logging is OFF). Tests call these so a file reopen cannot sneak in.
    inline void enableDevelopmentMidiConsoleLoggingForLabSession(std::uint32_t instanceId)
    {
        LogSessionHeader::setReportingInstanceId(instanceId);
        enableDevelopmentMidiConsoleLogging();
    }

    inline void enableDevelopmentApvtsConsoleLoggingForLabSession(std::uint32_t instanceId)
    {
        LogSessionHeader::setReportingInstanceId(instanceId);
        enableDevelopmentApvtsConsoleLogging();
        ApvtsLogger::getInstance().logInfo("APVTS logging enabled");
    }

    inline constexpr int kEditorialUndoMaxStoredUnits = 100; // transaction depth (see undo-policy.md)

    // Covers APVTS deferred flush (20–500 ms), Matrix Mod coalesce (40 ms), and SysEx queue delay.
    inline constexpr int kEditorialUndoRedoGranularMidiQuietMs = 500;

    inline void flushDeferredApvtsParameterSync(juce::AudioProcessorValueTreeState& apvts)
    {
        // setValueNotifyingHost can queue ValueTree sync on APVTS's internal timer; copyState() forces flush.
        (void) apvts.copyState();
    }
}
