#pragma once

// 1: allow log files under <project>/Logs/MIDI/ (ProjectPaths runtime root). 0: no file output.
#ifndef MIDI_LOGGER_ENABLED
#define MIDI_LOGGER_ENABLED 1
#endif

#include <fstream>
#include <memory>
#include <mutex>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExParser.h"

class MidiLogger
{
public:
    enum class LogLevel
    {
        kNone    = 0,
        kError   = 1,
        kWarning = 2,
        kInfo    = 3,
        kDebug   = 4,
        kVerbose = 5
    };

    static MidiLogger& getInstance();
    
    void setLogLevel(LogLevel level);
    void setLogToFile(bool enabled, const juce::File& logFile = juce::File());
    void setLogToConsole(bool enabled);
    
    void logMessage(LogLevel level, const juce::String& message);
    void logSysExSent(const juce::MemoryBlock& sysEx, const juce::String& description = "");
    void logSysExReceived(const juce::MemoryBlock& sysEx, const juce::String& description = "");
    void logSysExDataDecimal(const juce::uint8* data, size_t size, const juce::String& description = "");
    void logProgramChange(juce::uint8 programNumber, const juce::String& direction = "");
    void logError(const juce::String& errorMessage);
    void logWarning(const juce::String& warningMessage);
    void logInfo(const juce::String& infoMessage);
    
    juce::String formatSysExMessage(const juce::MemoryBlock& sysEx) const;
    
private:
    static constexpr const char* kLogFilenamePrefix = "midi-log";
    
    static constexpr int kMinLogLineWidth = 60;
    static constexpr int kLogLineWidth = 80;
    
    static constexpr const char* kLogLevelNames[] = {
        "NONE", "ERROR", "WARNING", "INFO", "DEBUG", "VERBOSE"
    };
    
    static constexpr int kLogLevelColumnWidth = 9;

    LogLevel currentLogLevel = LogLevel::kInfo;
    bool logToFile = false;
    bool logToConsole = true;
    juce::File logFile;
    std::unique_ptr<std::ofstream> fileStream;
    std::mutex logMutex;

    MidiLogger() = default;
    ~MidiLogger() = default;
    MidiLogger(const MidiLogger&) = delete;
    MidiLogger& operator=(const MidiLogger&) = delete;

    void writeLog(const juce::String& formattedMessage);
    void writeLogRaw(const juce::String& message);

    juce::String getTimestamp() const;
    juce::File getDefaultLogDirectory() const;
    
    juce::String generateTimestampedFilename() const;
    juce::String generateSeparatorLine() const;
    
    int getEffectiveLineWidth() const;
    void closeExistingLogFile();
    void writeSessionEndedFooter();
    juce::File determineLogFilePath(const juce::File& filePath);
    void ensureLogDirectoryExists(const juce::File& targetLogFile);
    void openNewLogFile();
    void writeSessionStartedHeader();
    juce::String formatLogLevelColumn(LogLevel level) const;
    juce::String wrapLogMessage(const juce::String& prefix, const juce::String& message) const;
    juce::String buildSysExHeaderMessage(const juce::String& direction, const juce::String& description, size_t byteCount) const;
    int calculateBytesPerLine() const;
    juce::String formatHexBytesWithLineWrapping(const juce::MemoryBlock& sysEx) const;
    void insertNewlineIfNeeded(juce::String& hexString, size_t currentIndex, int bytesPerLine) const;
    void appendHexByteWithSpace(juce::String& hexString, juce::uint8 byte, bool isLastByte) const;
    juce::String buildTimestampString() const;
    void createLogDirectoryIfNeeded(juce::File& logDir) const;
    juce::String analyzeSysExMessage(const juce::MemoryBlock& sysEx) const;
    juce::uint8 extractChecksumFromSysEx(const juce::MemoryBlock& sysEx) const;
    juce::MemoryBlock addSysExDelimiters(const juce::MemoryBlock& sysEx) const;
};

