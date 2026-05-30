#pragma once

// 1: allow log files under <project>/Logs/APVTS (MATRIX_CONTROL_PROJECT_ROOT from CMake). 0: no file output.
#ifndef APVTS_LOGGER_ENABLED
#define APVTS_LOGGER_ENABLED 1
#endif

#include <fstream>
#include <memory>
#include <mutex>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

class ApvtsLogger
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

    static ApvtsLogger& getInstance();
    
    void setLogLevel(LogLevel level);
    void setLogToFile(bool enabled, const juce::File& logFile = juce::File());
    void setLogToConsole(bool enabled);
    
    void logMessage(LogLevel level, const juce::String& message);
    void logParameterChanged(const juce::String& parameterId, 
                            float oldValue, 
                            float newValue,
                            const juce::String& source = "");
    
    void logValueTreePropertyChanged(const juce::Identifier& property,
                                    const juce::var& oldValue,
                                    const juce::var& newValue,
                                    const juce::String& threadName = "",
                                    const juce::String& choiceLabel = "");
    
    void logAttachmentCreated(const juce::String& parameterId, 
                             const juce::String& widgetType);
    
    void logAttachmentDestroyed(const juce::String& parameterId);
    
    void logStateReplaced();
    void logStateLoaded(const juce::String& source);
    
    void logError(const juce::String& errorMessage);
    void logWarning(const juce::String& warningMessage);
    void logInfo(const juce::String& infoMessage);
    void logDebug(const juce::String& debugMessage);
    
private:
    static constexpr const char* kLogFilenamePrefix = "apvts-log";
    
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

    ApvtsLogger() = default;
    ~ApvtsLogger() = default;
    ApvtsLogger(const ApvtsLogger&) = delete;
    ApvtsLogger& operator=(const ApvtsLogger&) = delete;

    void writeLog(const juce::String& formattedMessage);

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
    juce::String buildTimestampString() const;
    void createLogDirectoryIfNeeded(juce::File& logDir) const;
    juce::String formatVarValue(const juce::var& value) const;
    juce::String getCurrentThreadName() const;
};

