// Diagnostic logging preference (Settings SUPPORT) — Application Support file logs.
#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Loggers/ApvtsLogger.h"
#include "Loggers/LogSessionHeader.h"
#include "Loggers/MidiLogger.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/ProjectPaths.h"

using namespace PluginProcessorInternal;

namespace
{
    juce::String makeDiagnosticLogStamp()
    {
        const juce::Time now = juce::Time::getCurrentTime();
        return juce::String(now.getYear()).paddedLeft('0', 4)
            + "-" + juce::String(now.getMonth() + 1).paddedLeft('0', 2)
            + "-" + juce::String(now.getDayOfMonth()).paddedLeft('0', 2)
            + "-" + juce::String(now.getHours()).paddedLeft('0', 2)
            + "-" + juce::String(now.getMinutes()).paddedLeft('0', 2)
            + "-" + juce::String(now.getSeconds()).paddedLeft('0', 2);
    }

    void enableDiagnosticFileLoggers(std::uint32_t instanceId)
    {
        LogSessionHeader::setReportingInstanceId(instanceId);

        const juce::File midiDir = ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kMidi);
        const juce::File apvtsDir = ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kApvts);
        midiDir.createDirectory();
        apvtsDir.createDirectory();

        const juce::String stamp = makeDiagnosticLogStamp();
        auto& midi = MidiLogger::getInstance();
        auto& apvtsLogger = ApvtsLogger::getInstance();

        midi.setLogLevel(MidiLogger::LogLevel::kDebug);
        midi.setLogToConsole(false);
        midi.setLogToFile(true, midiDir.getChildFile("midi-log-" + stamp + ".txt"));

        apvtsLogger.setLogLevel(ApvtsLogger::LogLevel::kDebug);
        apvtsLogger.setLogToConsole(false);
        apvtsLogger.setLogToFile(true, apvtsDir.getChildFile("apvts-log-" + stamp + ".txt"));
        apvtsLogger.logInfo("Diagnostic logging enabled");
    }
}

void PluginProcessor::initializeDiagnosticLoggingProperty()
{
    if (! apvts.state.hasProperty(PluginIDs::Settings::kDiagnosticLogging))
    {
        apvts.state.setProperty(
            PluginIDs::Settings::kDiagnosticLogging,
            PluginIDs::Settings::DiagnosticLogging::kDefault,
            nullptr);
    }
}

bool PluginProcessor::isDiagnosticLoggingEnabled() const noexcept
{
    return diagnosticLoggingEnabled_;
}

void PluginProcessor::applyDiagnosticLoggingFromState()
{
    using namespace PluginIDs::Settings::DiagnosticLogging;
    const int raw = static_cast<int>(
        apvts.state.getProperty(PluginIDs::Settings::kDiagnosticLogging, kDefault));
    setDiagnosticLoggingEnabled(raw == kOn);
}

void PluginProcessor::setDiagnosticLoggingEnabled(bool enabled)
{
    if (diagnosticLoggingEnabled_ == enabled)
        return;

    if (enabled)
    {
        enableDiagnosticFileLoggers(loggingInstanceId_);
        auto& midi = MidiLogger::getInstance();
        auto& apvtsLogger = ApvtsLogger::getInstance();
        diagnosticLoggingEnabled_ = midi.isLoggingToFile() && apvtsLogger.isLoggingToFile();
        if (! diagnosticLoggingEnabled_)
        {
            midi.setLogToFile(false);
            apvtsLogger.setLogToFile(false);
            apvts.state.setProperty(
                PluginIDs::Settings::kDiagnosticLogging,
                PluginIDs::Settings::DiagnosticLogging::kOff,
                nullptr);
        }
        return;
    }

    diagnosticLoggingEnabled_ = false;

    MidiLogger::getInstance().setLogToFile(false);
    ApvtsLogger::getInstance().setLogToFile(false);

    if (shouldUseDevelopmentMidiLogging())
        enableFileLoggingForSession();
    else
    {
        MidiLogger::getInstance().setLogLevel(MidiLogger::LogLevel::kWarning);
        MidiLogger::getInstance().setLogToConsole(false);
    }

    if (shouldUseDevelopmentApvtsLogging())
        enableApvtsLogging();
    else
    {
        ApvtsLogger::getInstance().setLogLevel(ApvtsLogger::LogLevel::kWarning);
        ApvtsLogger::getInstance().setLogToConsole(false);
    }
}

void PluginProcessor::revealDiagnosticLogsFolder()
{
    juce::File root = ProjectPaths::getDiagnosticLogsRoot();
    root.createDirectory();
    if (root.isDirectory())
        root.revealToUser();
}
