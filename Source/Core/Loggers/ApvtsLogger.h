#pragma once

// 1: allow log files under <project>/Logs/APVTS/ (ProjectPaths runtime root). 0: no file output.
#ifndef APVTS_LOGGER_ENABLED
#define APVTS_LOGGER_ENABLED 1
#endif

#ifndef MC_DEV_APVTS_LOG
#define MC_DEV_APVTS_LOG 0
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

    struct ValueTreePropertyChangeLog
    {
        juce::Identifier property;
        juce::var oldValue;
        juce::var newValue;
        juce::String threadName;
        juce::String choiceLabel;
    };

    static ApvtsLogger& getInstance();

    void setLogLevel(LogLevel level);
    void setLogToFile(bool enabled, const juce::File& logFile = juce::File());
    void setLogToConsole(bool enabled);
    bool isLoggingToFile() const noexcept { return logToFile; }

    void logMessage(LogLevel level, const juce::String& message);
    void logParameterChanged(const juce::String& parameterId,
                             float oldValue,
                             float newValue,
                             const juce::String& source = "");

    void logValueTreePropertyChanged(const ValueTreePropertyChangeLog& change);

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
    static constexpr juce::int64 kButtonClickTimestampThresholdMs = 1000000000000LL;

    LogLevel currentLogLevel = (MC_DEV_APVTS_LOG != 0) ? LogLevel::kInfo : LogLevel::kWarning;
    bool logToFile = false;
    bool logToConsole = (MC_DEV_APVTS_LOG != 0);
    juce::File logFile;
    std::unique_ptr<std::ofstream> fileStream;
    std::mutex logMutex;

    ApvtsLogger() = default;
    ~ApvtsLogger() = default;
    ApvtsLogger(const ApvtsLogger&) = delete;
    ApvtsLogger& operator=(const ApvtsLogger&) = delete;

    void writeLog(const juce::String& formattedMessage);
    void writeNonEmptyLines(const juce::String& multiLineText);

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

    static int findBreakPreferringPunctuation(const juce::String& message, int rangeStart, int rangeEnd);
    static int findBreakAtSpace(const juce::String& message, int rangeStart, int rangeEnd, bool keepSpace);
    static int skipLeadingSpaces(const juce::String& message, int position);
    juce::String wrapFirstLine(const juce::String& prefix,
                               const juce::String& message,
                               int availableWidth,
                               int& messageStart) const;
    juce::String wrapContinuationLine(const juce::String& message,
                                      int maxLineWidth,
                                      int& messageStart) const;
    juce::String buildParameterChangeContinuation(float oldValue,
                                                  float newValue,
                                                  const juce::String& source,
                                                  const juce::String& threadName) const;
    bool isButtonClickTimestamp(const juce::var& value) const;
    juce::String formatPropertyNewValue(const juce::var& newValue,
                                        const juce::String& choiceLabel,
                                        bool isButtonClick) const;
    juce::String buildValueTreePropertyMessage(const ValueTreePropertyChangeLog& change,
                                               bool isButtonClick,
                                               const juce::String& newValueStr) const;
};
