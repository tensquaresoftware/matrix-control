#include <juce_core/juce_core.h>

#include "Core/Loggers/ApvtsLogger.h"
#include "Core/Loggers/LogSessionHeader.h"
#include "Core/Loggers/MidiLogger.h"
#include "Core/PluginProcessorInternal.h"
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
        defaultLogsDirectoryMatchesDiagnostic();
        emptyPathOpensUnderApplicationSupport();
        apvtsEmptyPathOpensUnderApplicationSupport();
        developmentConsoleRestoreDoesNotOpenFiles();
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

    static juce::Array<juce::File> listLogFiles(const juce::File& dir, const juce::String& pattern)
    {
        return dir.findChildFiles(juce::File::findFiles, false, pattern);
    }

    static juce::File pickCreatedOrNewest(const juce::Array<juce::File>& before,
                                          const juce::Array<juce::File>& after)
    {
        for (const auto& f : after)
        {
            bool wasBefore = false;
            for (const auto& b : before)
            {
                if (b == f)
                {
                    wasBefore = true;
                    break;
                }
            }
            if (! wasBefore)
                return f;
        }

        // Same-second stamp may overwrite an existing file — pick the newest.
        juce::File newest;
        for (const auto& f : after)
        {
            if (! newest.existsAsFile()
                || f.getLastModificationTime() > newest.getLastModificationTime())
            {
                newest = f;
            }
        }
        return newest;
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
        const juce::File appData = ProjectPaths::getApplicationDataDirectory();
        expect(root.getFullPathName().contains("Matrix-Control"));
        expectEquals(root.getFileName(), juce::String("Logs"));
        expectEquals(root.getParentDirectory().getFullPathName(), appData.getFullPathName());
        expectEquals(ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kMidi).getFileName(),
                     juce::String("MIDI"));
        expectEquals(ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kApvts).getFileName(),
                     juce::String("APVTS"));
    }

    void defaultLogsDirectoryMatchesDiagnostic()
    {
        beginTest("Default getLogsDirectory matches diagnostic Application Support dirs");
        expectEquals(ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kMidi).getFullPathName(),
                     ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kMidi)
                         .getFullPathName());
        expectEquals(ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kApvts).getFullPathName(),
                     ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kApvts)
                         .getFullPathName());
        expectEquals(ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kMidi)
                         .getParentDirectory()
                         .getFullPathName(),
                     ProjectPaths::getDiagnosticLogsRoot().getFullPathName());
    }

    void emptyPathOpensUnderApplicationSupport()
    {
        beginTest("Empty MidiLogger setLogToFile path opens under Application Support");
        auto& midi = MidiLogger::getInstance();
        const juce::File expectedDir = ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kMidi);
        expect(expectedDir.createDirectory().wasOk() || expectedDir.isDirectory());
        const auto before = listLogFiles(expectedDir, "midi-log-*.txt");

        midi.setLogLevel(MidiLogger::LogLevel::kInfo);
        midi.setLogToConsole(false);
        midi.setLogToFile(true);
        expect(midi.isLoggingToFile());

        const auto after = listLogFiles(expectedDir, "midi-log-*.txt");
        const juce::File created = pickCreatedOrNewest(before, after);
        expect(created.existsAsFile());
        expect(created.getFullPathName().startsWith(
            ProjectPaths::getDiagnosticLogsRoot().getFullPathName()));
        expectEquals(created.getParentDirectory().getFullPathName(),
                     expectedDir.getFullPathName());

        midi.setLogToFile(false);
        if (before.size() < after.size())
            created.deleteFile();
        restoreMidiLoggerDefaults(midi);
    }

    void apvtsEmptyPathOpensUnderApplicationSupport()
    {
        beginTest("Empty ApvtsLogger setLogToFile path opens under Application Support");
        auto& apvtsLogger = ApvtsLogger::getInstance();
        const juce::File expectedDir = ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kApvts);
        expect(expectedDir.createDirectory().wasOk() || expectedDir.isDirectory());
        const auto before = listLogFiles(expectedDir, "apvts-log-*.txt");

        apvtsLogger.setLogLevel(ApvtsLogger::LogLevel::kInfo);
        apvtsLogger.setLogToConsole(false);
        apvtsLogger.setLogToFile(true);
        expect(apvtsLogger.isLoggingToFile());

        const auto after = listLogFiles(expectedDir, "apvts-log-*.txt");
        const juce::File created = pickCreatedOrNewest(before, after);
        expect(created.existsAsFile());
        expect(created.getFullPathName().startsWith(
            ProjectPaths::getDiagnosticLogsRoot().getFullPathName()));
        expectEquals(created.getParentDirectory().getFullPathName(),
                     expectedDir.getFullPathName());

        apvtsLogger.setLogToFile(false);
        if (before.size() < after.size())
            created.deleteFile();
        restoreApvtsLoggerDefaults(apvtsLogger);
    }

    void developmentConsoleRestoreDoesNotOpenFiles()
    {
        // Calls the same PluginProcessorInternal helpers used by enableFileLoggingForSession /
        // enableApvtsLogging after SUPPORT OFF closes files.
        beginTest("Development console helpers leave file logging off");
        auto& midi = MidiLogger::getInstance();
        auto& apvtsLogger = ApvtsLogger::getInstance();
        const juce::File midiDir = ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kMidi);
        const juce::File apvtsDir = ProjectPaths::getDiagnosticLogsDirectory(ProjectPaths::LogCategory::kApvts);
        expect(midiDir.createDirectory().wasOk() || midiDir.isDirectory());
        expect(apvtsDir.createDirectory().wasOk() || apvtsDir.isDirectory());
        const juce::File midiProbe = midiDir.getChildFile("midi-log-console-restore-probe.txt");
        const juce::File apvtsProbe = apvtsDir.getChildFile("apvts-log-console-restore-probe.txt");
        midiProbe.deleteFile();
        apvtsProbe.deleteFile();

        midi.setLogToFile(true, midiProbe);
        apvtsLogger.setLogToFile(true, apvtsProbe);
        expect(midi.isLoggingToFile());
        expect(apvtsLogger.isLoggingToFile());

        midi.setLogToFile(false);
        apvtsLogger.setLogToFile(false);
        PluginProcessorInternal::enableDevelopmentMidiConsoleLogging();
        PluginProcessorInternal::enableDevelopmentApvtsConsoleLogging();

        expect(! midi.isLoggingToFile());
        expect(! apvtsLogger.isLoggingToFile());
        midiProbe.deleteFile();
        apvtsProbe.deleteFile();
        restoreMidiLoggerDefaults(midi);
        restoreApvtsLoggerDefaults(apvtsLogger);
    }

    void explicitPathOpensFile()
    {
        beginTest("Explicit diagnostic path opens a file under Application Support");
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
