#include <juce_core/juce_core.h>

#include "Core/Loggers/ApvtsLogger.h"
#include "Core/Loggers/LogSessionHeader.h"
#include "Core/Loggers/MidiLogger.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/ProjectPaths.h"

class DiagnosticLoggingSettingsTests : public juce::UnitTest
{
public:
    DiagnosticLoggingSettingsTests()
        : juce::UnitTest("DiagnosticLoggingSettings")
    {
    }

    void runTest() override
    {
        preferenceDefaultsOff();
        diagnosticPathsUnderApplicationSupport();
        explicitPathOpensFile();
        apvtsExplicitPathOpensFile();
        releaseStyleInfoIsQuietInFile();
        sessionHeaderContainsRequiredKeys();
    }

private:
    static void restoreMidiLoggerDefaults(MidiLogger& midi)
    {
        midi.setLogToFile(false);
        midi.setLogLevel((MC_DEV_MIDI_LOG != 0) ? MidiLogger::LogLevel::kInfo
                                                : MidiLogger::LogLevel::kWarning);
        midi.setLogToConsole(MC_DEV_MIDI_LOG != 0);
    }

    static void restoreApvtsLoggerDefaults(ApvtsLogger& apvts)
    {
        apvts.setLogToFile(false);
        apvts.setLogLevel((MC_DEV_APVTS_LOG != 0) ? ApvtsLogger::LogLevel::kInfo
                                                  : ApvtsLogger::LogLevel::kWarning);
        apvts.setLogToConsole(MC_DEV_APVTS_LOG != 0);
    }

    void preferenceDefaultsOff()
    {
        beginTest("Diagnostic logging preference defaults to OFF");
        expectEquals(PluginIDs::Settings::DiagnosticLogging::kDefault,
                     PluginIDs::Settings::DiagnosticLogging::kOff);
        expectEquals(juce::String(PluginDisplayNames::Settings::kSupportTab),
                     juce::String("SUPPORT"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kDiagnosticLoggingLabel),
                     juce::String("DIAGNOSTIC LOGGING"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kOpenLogFolderButton),
                     juce::String("OPEN LOG FOLDER"));
    }

    void diagnosticPathsUnderApplicationSupport()
    {
        beginTest("Diagnostic logs root is under Application Support");
        const juce::File root = ProjectPaths::getDiagnosticLogsRoot();
        expect(root.getFullPathName().contains("Matrix-Control"));
        expectEquals(root.getFileName(), juce::String("Logs"));
        expectEquals(ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kMidi).getFileName(),
                     juce::String("MIDI"));
        expectEquals(ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kApvts).getFileName(),
                     juce::String("APVTS"));
    }

    void explicitPathOpensFile()
    {
        beginTest("Explicit diagnostic path opens a file without relying on repo Logs");
        auto& midi = MidiLogger::getInstance();
        const juce::File midiDir = ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kMidi);
        expect(midiDir.createDirectory().wasOk() || midiDir.isDirectory());
        const juce::File probe = midiDir.getChildFile("midi-log-unit-probe.txt");
        probe.deleteFile();
        midi.setLogLevel(MidiLogger::LogLevel::kInfo);
        midi.setLogToConsole(false);
        midi.setLogToFile(true, probe);
        expect(midi.isLoggingToFile());
        expect(probe.existsAsFile());
        midi.logInfo("unit probe");
        midi.setLogToFile(false);
        expect(! midi.isLoggingToFile());
        expect(probe.loadFileAsString().contains("unit probe"));
        probe.deleteFile();
        restoreMidiLoggerDefaults(midi);
    }

    void apvtsExplicitPathOpensFile()
    {
        beginTest("ApvtsLogger explicit diagnostic path opens, writes, and closes");
        auto& apvtsLogger = ApvtsLogger::getInstance();
        const juce::File apvtsDir = ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kApvts);
        expect(apvtsDir.createDirectory().wasOk() || apvtsDir.isDirectory());
        const juce::File probe = apvtsDir.getChildFile("apvts-log-unit-probe.txt");
        probe.deleteFile();
        apvtsLogger.setLogLevel(ApvtsLogger::LogLevel::kInfo);
        apvtsLogger.setLogToConsole(false);
        apvtsLogger.setLogToFile(true, probe);
        expect(apvtsLogger.isLoggingToFile());
        expect(probe.existsAsFile());
        apvtsLogger.logInfo("apvts unit probe");
        apvtsLogger.setLogToFile(false);
        expect(! apvtsLogger.isLoggingToFile());
        expect(probe.loadFileAsString().contains("apvts unit probe"));
        probe.deleteFile();
        restoreApvtsLoggerDefaults(apvtsLogger);
    }

    void releaseStyleInfoIsQuietInFile()
    {
        beginTest("Release-style MidiLogger drops Info from the log file");
        auto& midi = MidiLogger::getInstance();
        const juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
        const juce::File probe = tempDir.getChildFile("midi-log-release-quiet-probe.txt");
        probe.deleteFile();

        midi.setLogLevel(MidiLogger::LogLevel::kWarning);
        midi.setLogToConsole(false);
        midi.setLogToFile(true, probe);
        expect(midi.isLoggingToFile());
        midi.logInfo("must stay silent");
        midi.setLogToFile(false);

        const juce::String contents = probe.loadFileAsString();
        expect(! contents.contains("must stay silent"));
        probe.deleteFile();
        restoreMidiLoggerDefaults(midi);
    }

    void sessionHeaderContainsRequiredKeys()
    {
        beginTest("Diagnostic log session header includes required support keys");
        auto& midi = MidiLogger::getInstance();
        const juce::File tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
        const juce::File probe = tempDir.getChildFile("midi-log-header-probe.txt");
        probe.deleteFile();

        LogSessionHeader::setReportingInstanceId(LogSessionHeader::allocateInstanceId());
        LogSessionHeader::setAudioSnapshot(48000.0, 256);
        LogSessionHeader::setNextHeaderFormatAndHost("Standalone", "n/a");

        midi.setLogLevel(MidiLogger::LogLevel::kInfo);
        midi.setLogToConsole(false);
        midi.setLogToFile(true, probe);
        expect(midi.isLoggingToFile());
        midi.setLogToFile(false);

        const juce::String contents = probe.loadFileAsString();
        // Keys are right-padded before ':' (e.g. "Product     : …") — match the key text only.
        expect(contents.contains("Organization"));
        expect(contents.contains("Product"));
        expect(contents.contains("Version"));
        expect(contents.contains("Host"));
        expect(contents.contains("Sample rate"));
        expect(contents.contains("Buffer size"));
        probe.deleteFile();
        LogSessionHeader::clearAudioSnapshot();
        restoreMidiLoggerDefaults(midi);
    }
};

static DiagnosticLoggingSettingsTests diagnosticLoggingSettingsTests;
