#include <iostream>

#include "MidiLogger.h"

#include "LogSessionHeader.h"
#include "Shared/ProjectPaths.h"

#include <mutex>

MidiLogger& MidiLogger::getInstance()
{
    static MidiLogger instance;
    return instance;
}

void MidiLogger::setLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(logMutex);
    currentLogLevel = level;
}

juce::File MidiLogger::getDefaultLogDirectory() const
{
    juce::File logDir = ProjectPaths::getLogsDirectory(ProjectPaths::LogCategory::kMidi);
    createLogDirectoryIfNeeded(logDir);
    return logDir;
}

void MidiLogger::createLogDirectoryIfNeeded(juce::File& logDir) const
{
    if (!logDir.exists())
        logDir.createDirectory();
}

juce::String MidiLogger::generateTimestampedFilename() const
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

int MidiLogger::getEffectiveLineWidth() const
{
    return (kLogLineWidth >= kMinLogLineWidth) ? kLogLineWidth : kMinLogLineWidth;
}

juce::String MidiLogger::generateSeparatorLine() const
{
    return juce::String::repeatedString("=", getEffectiveLineWidth());
}

void MidiLogger::closeExistingLogFile()
{
    if (fileStream && fileStream->is_open())
    {
        writeSessionEndedFooter();
        fileStream->flush();
        fileStream->close();
    }
    fileStream.reset();
}

void MidiLogger::setLogToFile(bool enabled, const juce::File& filePath)
{
    std::lock_guard<std::mutex> lock(logMutex);

    closeExistingLogFile();

#if !MIDI_LOGGER_ENABLED
    juce::ignoreUnused(filePath);
    logToFile = false;
    return;
#endif

    logToFile = enabled;
    if (! enabled)
        return;

    // Empty path → Application Support via getLogsDirectory; non-empty path is caller-chosen.
    logFile = determineLogFilePath(filePath);
    ensureLogDirectoryExists(logFile);

    if (! logToFile)
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

void MidiLogger::writeSessionEndedFooter()
{
    const juce::String separator = generateSeparatorLine();
    const juce::String footerText = "=== MIDI Log Session Ended ";
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

juce::File MidiLogger::determineLogFilePath(const juce::File& filePath)
{
    if (filePath.getFullPathName().isNotEmpty())
        return filePath;

    return getDefaultLogDirectory().getChildFile(generateTimestampedFilename());
}

void MidiLogger::ensureLogDirectoryExists(const juce::File& targetLogFile)
{
    const juce::File parentDir = targetLogFile.getParentDirectory();
    const juce::Result createResult = parentDir.createDirectory();

    if (!createResult.wasOk())
    {
        if (logToConsole)
        {
            std::cout << "[ERROR] Failed to create log directory: "
                      << parentDir.getFullPathName().toRawUTF8()
                      << " - " << createResult.getErrorMessage().toRawUTF8() << std::endl;
        }
        logToFile = false;
    }
}

void MidiLogger::openNewLogFile()
{
    fileStream = std::make_unique<std::ofstream>(logFile.getFullPathName().toRawUTF8(),
                                                   std::ios::out);
}

void MidiLogger::writeSessionStartedHeader()
{
    const juce::String separator = generateSeparatorLine();
    const juce::String bannerText = "=== MIDI Log Session Started ";
    const int remainingChars = getEffectiveLineWidth() - bannerText.length();
    const juce::String banner = remainingChars > 0
        ? bannerText + separator.substring(separator.length() - remainingChars)
        : bannerText;

    // Cartouche first; blank line; then session banner before the first event lines.
    *fileStream << separator.toRawUTF8() << "\n";
    *fileStream << LogSessionHeader::buildMetadataBlock(logFile).toRawUTF8();
    *fileStream << separator.toRawUTF8() << "\n";
    *fileStream << "\n";
    *fileStream << banner.toRawUTF8() << "\n\n";
    fileStream->flush();
}

void MidiLogger::setLogToConsole(bool enabled)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logToConsole = enabled;
}

void MidiLogger::logSysExSent(const juce::MemoryBlock& sysEx, const juce::String& description)
{
    if (LogLevel::kDebug > currentLogLevel)
        return;

    // Header + hex on one logical message; wrapLogMessage breaks only at kLogLineWidth.
    logMessage(LogLevel::kDebug,
               buildSysExHeaderMessage("SENT", description, sysEx.getSize())
                   + " " + formatSysExMessage(sysEx));
}

void MidiLogger::logSysExReceived(const juce::MemoryBlock& sysEx, const juce::String& description)
{
    if (LogLevel::kDebug > currentLogLevel)
        return;

    logMessage(LogLevel::kDebug,
               buildSysExHeaderMessage("RECEIVED", description, sysEx.getSize())
                   + " " + formatSysExMessage(sysEx));

    const juce::String analysis = analyzeSysExMessage(sysEx);
    if (analysis.isNotEmpty())
        logMessage(LogLevel::kInfo, analysis);
}

void MidiLogger::logProgramChange(juce::uint8 programNumber, const juce::String& direction)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;

    juce::String message = "Program Change";
    if (direction.isNotEmpty())
        message += " " + direction;
    message += ": " + juce::String(programNumber);

    logMessage(LogLevel::kInfo, message);
}

void MidiLogger::logError(const juce::String& errorMessage)
{
    logMessage(LogLevel::kError, "ERROR: " + errorMessage);
}

void MidiLogger::logWarning(const juce::String& warningMessage)
{
    logMessage(LogLevel::kWarning, "WARNING: " + warningMessage);
}

void MidiLogger::logInfo(const juce::String& infoMessage)
{
    logMessage(LogLevel::kInfo, infoMessage);
}

void MidiLogger::writeLog(const juce::String& formattedMessage)
{
    std::lock_guard<std::mutex> lock(logMutex);

    if (logToConsole)
        std::cout << formattedMessage.toRawUTF8() << std::endl;

#if MIDI_LOGGER_ENABLED
    if (logToFile && fileStream && fileStream->is_open())
    {
        *fileStream << formattedMessage.toRawUTF8() << std::endl;
        // Avoid flush-per-message on the MIDI consumer hot path (stuck-note residual under Debug).
    }
#endif
}

void MidiLogger::writeLogRaw(const juce::String& message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    if (logToConsole)
        std::cout << message.toRawUTF8() << std::endl;

#if MIDI_LOGGER_ENABLED
    if (logToFile && fileStream && fileStream->is_open())
    {
        *fileStream << message.toRawUTF8() << std::endl;
        // Avoid flush-per-message on the MIDI consumer hot path.
    }
#endif
}

void MidiLogger::writeNonEmptyLines(const juce::String& multiLineText)
{
    juce::StringArray lines;
    lines.addLines(multiLineText);

    for (const auto& line : lines)
    {
        if (line.isNotEmpty())
            writeLog(line);
    }
}
