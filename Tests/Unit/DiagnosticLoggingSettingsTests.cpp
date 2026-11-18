#include <juce_core/juce_core.h>

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
#if ! (defined(MC_DEV_MIDI_LOG) && MC_DEV_MIDI_LOG)
        releaseStyleInfoIsQuiet();
#endif
    }

private:
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
    }

#if ! (defined(MC_DEV_MIDI_LOG) && MC_DEV_MIDI_LOG)
    void releaseStyleInfoIsQuiet()
    {
        beginTest("Release-style MidiLogger defaults drop Info when console is off");
        auto& midi = MidiLogger::getInstance();
        midi.setLogLevel(MidiLogger::LogLevel::kWarning);
        midi.setLogToConsole(false);
        midi.setLogToFile(false);
        midi.logInfo("must stay silent");
        expect(! midi.isLoggingToFile());
    }
#endif
};

static DiagnosticLoggingSettingsTests diagnosticLoggingSettingsTests;
