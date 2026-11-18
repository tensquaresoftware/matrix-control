#include <iostream>

#include "ApvtsLogger.h"

#include "Shared/ProjectPaths.h"

#include <juce_core/juce_core.h>
#include <mutex>

ApvtsLogger& ApvtsLogger::getInstance()
{
    static ApvtsLogger instance;
    return instance;
}

void ApvtsLogger::setLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(logMutex);
    currentLogLevel = level;
}

juce::File ApvtsLogger::getDefaultLogDirectory() const
{
    juce::File logDir = ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kApvts);
    createLogDirectoryIfNeeded(logDir);
    return logDir;
}

void ApvtsLogger::createLogDirectoryIfNeeded(juce::File& logDir) const
{
    if (!logDir.exists())
        logDir.createDirectory();
}

juce::String ApvtsLogger::generateTimestampedFilename() const
{
    const juce::Time now = juce::Time::getCurrentTime();

    juce::String filename = kLogFilenamePrefix;
    filename += "-";
    filename += juce::String(now.getYear()).paddedLeft('0', 4);
    filename += "-";
    filename += juce::String(now.getMonth() + 1).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(now.getDayOfMonth()).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(now.getHours()).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(now.getMinutes()).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(now.getSeconds()).paddedLeft('0', 2);
    filename += ".txt";

    return filename;
}

int ApvtsLogger::getEffectiveLineWidth() const
{
    return (kLogLineWidth >= kMinLogLineWidth) ? kLogLineWidth : kMinLogLineWidth;
}

juce::String ApvtsLogger::generateSeparatorLine() const
{
    return juce::String::repeatedString("=", getEffectiveLineWidth());
}

void ApvtsLogger::closeExistingLogFile()
{
    if (fileStream && fileStream->is_open())
    {
        writeSessionEndedFooter();
        fileStream->flush();
        fileStream->close();
    }
    fileStream.reset();
}

void ApvtsLogger::setLogToFile(bool enabled, const juce::File& filePath)
{
    bool shouldEmitFallbackWarning = false;

    {
        std::lock_guard<std::mutex> lock(logMutex);

        closeExistingLogFile();

#if !APVTS_LOGGER_ENABLED
        juce::ignoreUnused(filePath);
        logToFile = false;
        return;
#endif

        logToFile = enabled;
        if (!enabled)
            return;

        // Explicit path = diagnostic/support location; do not walk project root.
        if (filePath.getFullPathName().isEmpty())
        {
            static std::once_flag fallbackWarningFlag;
            std::call_once(fallbackWarningFlag, [&shouldEmitFallbackWarning]()
            {
                shouldEmitFallbackWarning = ProjectPaths::isUsingFallbackRoot();
            });
        }

        logFile = determineLogFilePath(filePath);
        ensureLogDirectoryExists(logFile);

        if (!logToFile)
            return;

        openNewLogFile();

        if (fileStream && fileStream->is_open())
        {
            writeSessionStartedHeader();
        }
        else
        {
            logToFile = false;
            fileStream.reset();
        }
    }

    if (shouldEmitFallbackWarning)
        logWarning(ProjectPaths::getFallbackRootWarning());
}

void ApvtsLogger::writeSessionEndedFooter()
{
    const juce::String separator = generateSeparatorLine();
    const juce::String footerText = "=== APVTS Log Session Ended ";
    const int remainingChars = getEffectiveLineWidth() - footerText.length();

    if (remainingChars > 0)
    {
        *fileStream << "\n" << footerText.toRawUTF8()
                    << separator.substring(separator.length() - remainingChars).toRawUTF8() << "\n";
    }
    else
    {
        *fileStream << "\n" << footerText.toRawUTF8() << "\n";
    }
}

juce::File ApvtsLogger::determineLogFilePath(const juce::File& filePath)
{
    if (filePath.getFullPathName().isNotEmpty())
        return filePath;

    return getDefaultLogDirectory().getChildFile(generateTimestampedFilename());
}

void ApvtsLogger::ensureLogDirectoryExists(const juce::File& targetLogFile)
{
    const juce::File parentDir = targetLogFile.getParentDirectory();
    const juce::Result createResult = parentDir.createDirectory();

    if (createResult.wasOk())
        return;

    if (logToConsole)
    {
        std::cout << "[ERROR] Failed to create log directory: "
                  << parentDir.getFullPathName().toRawUTF8()
                  << " - " << createResult.getErrorMessage().toRawUTF8() << std::endl;
    }
    logToFile = false;
}

void ApvtsLogger::openNewLogFile()
{
    fileStream = std::make_unique<std::ofstream>(logFile.getFullPathName().toRawUTF8(),
                                                   std::ios::out);

    if (!logToConsole)
        return;

    if (!fileStream || !fileStream->is_open())
    {
        std::cout << "[APVTS Logger] WARNING: Failed to open log file: "
                  << logFile.getFullPathName().toRawUTF8() << std::endl;
    }
    else
    {
        std::cout << "[APVTS Logger] Log file opened: "
                  << logFile.getFullPathName().toRawUTF8() << std::endl;
    }
}

void ApvtsLogger::writeSessionStartedHeader()
{
    const juce::Time now = juce::Time::getCurrentTime();
    const juce::String separator = generateSeparatorLine();
    const juce::String headerText = "=== APVTS Log Session Started ";
    const int remainingChars = getEffectiveLineWidth() - headerText.length();

    if (remainingChars > 0)
    {
        *fileStream << headerText.toRawUTF8()
                    << separator.substring(separator.length() - remainingChars).toRawUTF8() << "\n";
    }
    else
    {
        *fileStream << headerText.toRawUTF8() << "\n";
    }

    *fileStream << "Date: " << now.toString(true, true, false, true).toRawUTF8() << "\n";
    *fileStream << "Log file: " << logFile.getFileName().toRawUTF8() << "\n";
    *fileStream << separator.toRawUTF8() << "\n\n";
    fileStream->flush();
}

void ApvtsLogger::setLogToConsole(bool enabled)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logToConsole = enabled;
}

void ApvtsLogger::logAttachmentCreated(const juce::String& parameterId,
                                       const juce::String& widgetType)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;

    logMessage(LogLevel::kInfo,
               "Attachment created: " + parameterId + " | Widget type: " + widgetType);
}

void ApvtsLogger::logAttachmentDestroyed(const juce::String& parameterId)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;

    logMessage(LogLevel::kInfo, "Attachment destroyed: " + parameterId);
}

void ApvtsLogger::logStateReplaced()
{
    if (LogLevel::kInfo > currentLogLevel)
        return;

    logMessage(LogLevel::kInfo, "APVTS state replaced");
}

void ApvtsLogger::logStateLoaded(const juce::String& source)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;

    juce::String message = "APVTS state loaded";
    if (source.isNotEmpty())
        message += " from: " + source;

    logMessage(LogLevel::kInfo, message);
}

void ApvtsLogger::logError(const juce::String& errorMessage)
{
    logMessage(LogLevel::kError, "ERROR: " + errorMessage);
}

void ApvtsLogger::logWarning(const juce::String& warningMessage)
{
    logMessage(LogLevel::kWarning, "WARNING: " + warningMessage);
}

void ApvtsLogger::logInfo(const juce::String& infoMessage)
{
    logMessage(LogLevel::kInfo, infoMessage);
}

void ApvtsLogger::logDebug(const juce::String& debugMessage)
{
    logMessage(LogLevel::kDebug, debugMessage);
}

void ApvtsLogger::writeLog(const juce::String& formattedMessage)
{
    std::lock_guard<std::mutex> lock(logMutex);

    if (logToConsole)
        std::cout << formattedMessage.toRawUTF8() << std::endl;

#if APVTS_LOGGER_ENABLED
    if (logToFile && fileStream && fileStream->is_open())
    {
        *fileStream << formattedMessage.toRawUTF8() << std::endl;
        fileStream->flush();
    }
#endif
}

void ApvtsLogger::writeNonEmptyLines(const juce::String& multiLineText)
{
    juce::StringArray lines;
    lines.addLines(multiLineText);

    for (const auto& line : lines)
    {
        if (line.isNotEmpty())
            writeLog(line);
    }
}
