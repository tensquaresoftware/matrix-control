#include <iostream>

#include "MidiLogger.h"

#include "Core/MIDI/SysEx/SysExConstants.h"

#if !defined(MATRIX_CONTROL_PROJECT_ROOT)
#error "MATRIX_CONTROL_PROJECT_ROOT must be defined by CMake (project root) so default log paths resolve to Logs/MIDI."
#endif

#include "Core/MIDI/SysEx/SysExParser.h"

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
    juce::File logDir = juce::File(juce::String(MATRIX_CONTROL_PROJECT_ROOT))
        .getChildFile("Logs")
        .getChildFile("MIDI");

    createLogDirectoryIfNeeded(logDir);
    return logDir;
}

void MidiLogger::createLogDirectoryIfNeeded(juce::File& logDir) const
{
    if (!logDir.exists())
    {
        logDir.createDirectory();
    }
}

juce::String MidiLogger::generateTimestampedFilename() const
{
    juce::Time now = juce::Time::getCurrentTime();
    
    int year = now.getYear();
    int month = now.getMonth() + 1;
    int day = now.getDayOfMonth();
    int hour = now.getHours();
    int minute = now.getMinutes();
    int second = now.getSeconds();
    
    juce::String filename = kLogFilenamePrefix;
    
    filename += "-";
    filename += juce::String(year).paddedLeft('0', 4);
    filename += "-";
    filename += juce::String(month).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(day).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(hour).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(minute).paddedLeft('0', 2);
    filename += "-";
    filename += juce::String(second).paddedLeft('0', 2);
    filename += ".txt";
    
    return filename;
}

int MidiLogger::getEffectiveLineWidth() const
{
    return (kLogLineWidth >= kMinLogLineWidth) ? kLogLineWidth : kMinLogLineWidth;
}

juce::String MidiLogger::generateSeparatorLine() const
{
    int lineWidth = getEffectiveLineWidth();
    
    juce::String separator;
    for (int i = 0; i < lineWidth; ++i)
    {
        separator += "=";
    }
    
    return separator;
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
    if (enabled)
    {
        logFile = determineLogFilePath(filePath);
        ensureLogDirectoryExists(logFile);
        
        if (!logToFile)
        {
            return;
        }
        
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
}

void MidiLogger::writeSessionEndedFooter()
{
    juce::String separator = generateSeparatorLine();
    juce::String footerText = "=== MIDI Log Session Ended ";
    int remainingChars = getEffectiveLineWidth() - footerText.length();
    
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
    {
        return filePath;
    }
    
    juce::File logDir = getDefaultLogDirectory();
    juce::String filename = generateTimestampedFilename();
    return logDir.getChildFile(filename);
}

void MidiLogger::ensureLogDirectoryExists(const juce::File& targetLogFile)
{
    juce::File parentDir = targetLogFile.getParentDirectory();
    juce::Result createResult = parentDir.createDirectory();
    
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
    juce::Time now = juce::Time::getCurrentTime();
    juce::String separator = generateSeparatorLine();
    
    juce::String headerText = "=== MIDI Log Session Started ";
    int remainingChars = getEffectiveLineWidth() - headerText.length();
    
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

void MidiLogger::setLogToConsole(bool enabled)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logToConsole = enabled;
}

juce::String MidiLogger::formatLogLevelColumn(LogLevel level) const
{
    juce::String levelName = kLogLevelNames[static_cast<int>(level)];
    juce::String levelColumn = "[" + levelName + "]";
    return levelColumn.paddedRight(' ', kLogLevelColumnWidth);
}

juce::String MidiLogger::wrapLogMessage(const juce::String& prefix, const juce::String& message) const
{
    int maxLineWidth = getEffectiveLineWidth();
    int prefixLength = prefix.length();
    int availableWidth = maxLineWidth - prefixLength;
    
    if (prefixLength + message.length() <= maxLineWidth)
    {
        return prefix + message;
    }
    
    juce::String wrappedMessage;
    int messageStart = 0;
    bool isFirstLine = true;
    
    while (messageStart < message.length())
    {
        if (isFirstLine)
        {
            int firstLineEnd = messageStart + availableWidth;
            
            if (firstLineEnd >= message.length())
            {
                wrappedMessage += prefix + message.substring(messageStart);
                break;
            }
            
            int breakPoint = firstLineEnd;
            
            for (int i = firstLineEnd; i > messageStart && i > firstLineEnd - 30; --i)
            {
                juce::juce_wchar ch = message[i];
                if (ch == '.' || ch == ':')
                {
                    breakPoint = i + 1;
                    break;
                }
            }
            
            if (breakPoint == firstLineEnd)
            {
                for (int i = firstLineEnd; i > messageStart && i > firstLineEnd - 30; --i)
                {
                    if (message[i] == ' ')
                    {
                        breakPoint = i + 1;
                        break;
                    }
                }
            }
            
            juce::String firstLineContent = message.substring(messageStart, breakPoint).trimEnd();
            wrappedMessage += prefix + firstLineContent + "\n";
            
            messageStart = breakPoint;
            while (messageStart < message.length() && message[messageStart] == ' ')
            {
                ++messageStart;
            }
            
            isFirstLine = false;
        }
        else
        {
            int lineEnd = messageStart + maxLineWidth;
            
            if (lineEnd >= message.length())
            {
                wrappedMessage += message.substring(messageStart);
                break;
            }
            
            int breakPoint = lineEnd;
            
            for (int i = lineEnd; i > messageStart && i > lineEnd - 30; --i)
            {
                if (message[i] == ' ')
                {
                    breakPoint = i;
                    break;
                }
            }
            
            juce::String lineContent = message.substring(messageStart, breakPoint);
            wrappedMessage += lineContent + "\n";
            
            messageStart = breakPoint;
            while (messageStart < message.length() && message[messageStart] == ' ')
            {
                ++messageStart;
            }
        }
    }
    
    return wrappedMessage.trimEnd();
}

void MidiLogger::logMessage(LogLevel level, const juce::String& message)
{
    if (level > currentLogLevel)
        return;
    
    juce::String levelColumn = formatLogLevelColumn(level);
    juce::String prefix = levelColumn + " " + getTimestamp() + " - ";
    juce::String wrappedMessage = wrapLogMessage(prefix, message);
    
    juce::StringArray lines;
    lines.addLines(wrappedMessage);
    
    for (int i = 0; i < lines.size(); ++i)
    {
        juce::String line = lines[i];
        if (line.isNotEmpty())
        {
            writeLog(line);
        }
    }
}

juce::String MidiLogger::buildSysExHeaderMessage(const juce::String& direction, const juce::String& description, size_t byteCount) const
{
    juce::String message = direction + " SysEx";
    if (description.isNotEmpty())
    {
        message += " (" + description + ")";
    }
    message += " (" + juce::String(byteCount) + " bytes)";
    message += ":";
    return message;
}

void MidiLogger::logSysExSent(const juce::MemoryBlock& sysEx, const juce::String& description)
{
    if (LogLevel::kDebug > currentLogLevel)
        return;
    
    juce::String message = buildSysExHeaderMessage("SENT", description, sysEx.getSize());
    logMessage(LogLevel::kDebug, message);
    writeLogRaw(formatSysExMessage(sysEx));
}

void MidiLogger::logSysExReceived(const juce::MemoryBlock& sysEx, const juce::String& description)
{
    if (LogLevel::kDebug > currentLogLevel)
        return;
    
    juce::ignoreUnused(description);
    
    logMessage(LogLevel::kDebug, "Received SysEx data:");
    writeLogRaw(formatSysExMessage(sysEx));
    
    juce::String analysis = analyzeSysExMessage(sysEx);
    if (analysis.isNotEmpty())
    {
        logMessage(LogLevel::kInfo, analysis);
    }
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

void MidiLogger::logSysExDataDecimal(const juce::uint8* data, size_t size, const juce::String& description)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;

    if (data == nullptr || size == 0)
    {
        logMessage(LogLevel::kWarning, "Cannot log decimal data: null pointer or zero size");
        return;
    }

    juce::String message = "Data";
    if (description.isNotEmpty())
    {
        message += " (" + description + ")";
    }
    message += " (" + juce::String(size) + " bytes)";
    message += ": ";

    juce::String dataString;
    for (size_t i = 0; i < size; ++i)
    {
        if (i > 0)
        {
            dataString += ", ";
        }
        dataString += juce::String(static_cast<int>(data[i]));
    }

    juce::String fullMessage = message + dataString;
    juce::String levelColumn = formatLogLevelColumn(LogLevel::kInfo);
    juce::String prefix = levelColumn + " " + getTimestamp() + " - ";
    juce::String wrappedMessage = wrapLogMessage(prefix, fullMessage);

    juce::StringArray lines;
    lines.addLines(wrappedMessage);

    for (int i = 0; i < lines.size(); ++i)
    {
        juce::String line = lines[i];
        if (line.isNotEmpty())
        {
            writeLog(line);
        }
    }
}

int MidiLogger::calculateBytesPerLine() const
{
    int lineWidth = getEffectiveLineWidth();
    return (lineWidth - 1) / 3;
}

void MidiLogger::insertNewlineIfNeeded(juce::String& hexString, size_t currentIndex, int bytesPerLine) const
{
    if (currentIndex > 0 && currentIndex % static_cast<size_t>(bytesPerLine) == 0)
    {
        hexString += "\n";
    }
}

void MidiLogger::appendHexByteWithSpace(juce::String& hexString, juce::uint8 byte, bool isLastByte) const
{
    hexString += juce::String::toHexString(byte).paddedLeft('0', 2).toUpperCase();
    if (!isLastByte)
    {
        hexString += " ";
    }
}

juce::String MidiLogger::formatHexBytesWithLineWrapping(const juce::MemoryBlock& sysEx) const
{
    juce::String hexString;
    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    int bytesPerLine = calculateBytesPerLine();
    
    for (size_t i = 0; i < sysEx.getSize(); ++i)
    {
        insertNewlineIfNeeded(hexString, i, bytesPerLine);
        bool isLastByte = (i == sysEx.getSize() - 1);
        appendHexByteWithSpace(hexString, data[i], isLastByte);
    }
    
    return hexString;
}

juce::String MidiLogger::formatSysExMessage(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() == 0)
        return "empty";
    
    return formatHexBytesWithLineWrapping(sysEx);
}

void MidiLogger::writeLog(const juce::String& formattedMessage)
{
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (logToConsole)
    {
        std::cout << formattedMessage.toRawUTF8() << std::endl;
    }
    
#if MIDI_LOGGER_ENABLED
    if (logToFile && fileStream && fileStream->is_open())
    {
        *fileStream << formattedMessage.toRawUTF8() << std::endl;
        fileStream->flush();
    }
#endif
}

void MidiLogger::writeLogRaw(const juce::String& message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (logToConsole)
    {
        std::cout << message.toRawUTF8() << std::endl;
    }
    
#if MIDI_LOGGER_ENABLED
    if (logToFile && fileStream && fileStream->is_open())
    {
        *fileStream << message.toRawUTF8() << std::endl;
        fileStream->flush();
    }
#endif
}

juce::String MidiLogger::buildTimestampString() const
{
    juce::Time now = juce::Time::getCurrentTime();
    
    int hour = now.getHours();
    int minute = now.getMinutes();
    int second = now.getSeconds();
    int millisecond = now.getMilliseconds();
    
    juce::String timestamp;
    timestamp += juce::String(hour).paddedLeft('0', 2);
    timestamp += ":";
    timestamp += juce::String(minute).paddedLeft('0', 2);
    timestamp += ":";
    timestamp += juce::String(second).paddedLeft('0', 2);
    timestamp += ".";
    timestamp += juce::String(millisecond).paddedLeft('0', 3);
    
    return timestamp;
}

juce::String MidiLogger::getTimestamp() const
{
    return buildTimestampString();
}

juce::String MidiLogger::analyzeSysExMessage(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < 2)
    {
        return {};
    }
    
    juce::MemoryBlock completeSysEx = addSysExDelimiters(sysEx);
    
    SysExParser parser;
    auto validation = parser.validateSysEx(completeSysEx);
    
    if (!validation.isValid)
    {
        return {};
    }
    
    juce::String analysis;
    
    switch (validation.messageType)
    {
        case SysExParser::MessageType::kPatch:
            analysis = "Valid Patch received";
            break;
        case SysExParser::MessageType::kMaster:
            analysis = "Valid Master received";
            break;
        case SysExParser::MessageType::kDeviceId:
        case SysExParser::MessageType::kSplitPatch:
        case SysExParser::MessageType::kUnknown:
        default:
            return {};
    }
    
    juce::uint8 checksum = extractChecksumFromSysEx(completeSysEx);
    analysis += " | Checksum: 0x" + juce::String::toHexString(checksum).paddedLeft('0', 2).toUpperCase();
    
    return analysis;
}

juce::uint8 MidiLogger::extractChecksumFromSysEx(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < 2)
    {
        return 0;
    }
    
    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    
    if (data[0] == SysExConstants::kSysExStart && 
        data[1] == SysExConstants::DeviceInquiry::kUniversalNonRealtimeId)
    {
        return 0;
    }
    
    if (sysEx.getSize() >= 2 && data[sysEx.getSize() - 1] == SysExConstants::kSysExEnd)
    {
        size_t checksumIndex = sysEx.getSize() - 2;
        return data[checksumIndex];
    }
    
    return 0;
}

juce::MemoryBlock MidiLogger::addSysExDelimiters(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() == 0)
    {
        return sysEx;
    }
    
    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    
    if (data[0] == SysExConstants::kSysExStart && 
        data[sysEx.getSize() - 1] == SysExConstants::kSysExEnd)
    {
        return sysEx;
    }
    
    juce::MemoryBlock completeSysEx;
    juce::uint8 startByte = SysExConstants::kSysExStart;
    juce::uint8 endByte = SysExConstants::kSysExEnd;
    
    completeSysEx.append(&startByte, 1);
    completeSysEx.append(sysEx.getData(), sysEx.getSize());
    completeSysEx.append(&endByte, 1);
    
    return completeSysEx;
}

