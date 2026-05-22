#include <iostream>
#include <cmath>

#include "ApvtsLogger.h"

#include <juce_core/juce_core.h>

#if !defined(MATRIX_CONTROL_PROJECT_ROOT)
#error "MATRIX_CONTROL_PROJECT_ROOT must be defined by CMake (project root) so default log paths resolve to Logs/APVTS."
#endif

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
    juce::File logDir = juce::File(juce::String(MATRIX_CONTROL_PROJECT_ROOT))
        .getChildFile("Logs")
        .getChildFile("APVTS");

    createLogDirectoryIfNeeded(logDir);
    return logDir;
}

void ApvtsLogger::createLogDirectoryIfNeeded(juce::File& logDir) const
{
    if (!logDir.exists())
    {
        logDir.createDirectory();
    }
}

juce::String ApvtsLogger::generateTimestampedFilename() const
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

int ApvtsLogger::getEffectiveLineWidth() const
{
    return (kLogLineWidth >= kMinLogLineWidth) ? kLogLineWidth : kMinLogLineWidth;
}

juce::String ApvtsLogger::generateSeparatorLine() const
{
    int lineWidth = getEffectiveLineWidth();
    
    juce::String separator;
    for (int i = 0; i < lineWidth; ++i)
    {
        separator += "=";
    }
    
    return separator;
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
    std::lock_guard<std::mutex> lock(logMutex);
    
    closeExistingLogFile();

#if !APVTS_LOGGER_ENABLED
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

void ApvtsLogger::writeSessionEndedFooter()
{
    juce::String separator = generateSeparatorLine();
    juce::String footerText = "=== APVTS Log Session Ended ";
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

juce::File ApvtsLogger::determineLogFilePath(const juce::File& filePath)
{
    if (filePath.getFullPathName().isNotEmpty())
    {
        return filePath;
    }
    
    juce::File logDir = getDefaultLogDirectory();
    juce::String filename = generateTimestampedFilename();
    return logDir.getChildFile(filename);
}

void ApvtsLogger::ensureLogDirectoryExists(const juce::File& targetLogFile)
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

void ApvtsLogger::openNewLogFile()
{
    fileStream = std::make_unique<std::ofstream>(logFile.getFullPathName().toRawUTF8(), 
                                                   std::ios::out);
    
    if (logToConsole && (!fileStream || !fileStream->is_open()))
    {
        std::cout << "[APVTS Logger] WARNING: Failed to open log file: " 
                  << logFile.getFullPathName().toRawUTF8() << std::endl;
    }
    else if (logToConsole)
    {
        std::cout << "[APVTS Logger] Log file opened: " 
                  << logFile.getFullPathName().toRawUTF8() << std::endl;
    }
}

void ApvtsLogger::writeSessionStartedHeader()
{
    juce::Time now = juce::Time::getCurrentTime();
    juce::String separator = generateSeparatorLine();
    
    juce::String headerText = "=== APVTS Log Session Started ";
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

void ApvtsLogger::setLogToConsole(bool enabled)
{
    std::lock_guard<std::mutex> lock(logMutex);
    logToConsole = enabled;
}

juce::String ApvtsLogger::formatLogLevelColumn(LogLevel level) const
{
    juce::String levelName = kLogLevelNames[static_cast<int>(level)];
    juce::String levelColumn = "[" + levelName + "]";
    return levelColumn.paddedRight(' ', kLogLevelColumnWidth);
}

juce::String ApvtsLogger::wrapLogMessage(const juce::String& prefix, const juce::String& message) const
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

void ApvtsLogger::logMessage(LogLevel level, const juce::String& message)
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

void ApvtsLogger::logParameterChanged(const juce::String& parameterId, 
                                      float oldValue, 
                                      float newValue,
                                      const juce::String& source)
{
    if (LogLevel::kDebug > currentLogLevel)
        return;
    
    juce::String levelColumn = formatLogLevelColumn(LogLevel::kDebug);
    juce::String timestamp = getTimestamp();
    juce::String prefix = levelColumn + " " + timestamp + " - ";
    
    juce::String firstLine = "Parameter changed: " + parameterId;
    
    int maxLineWidth = getEffectiveLineWidth();
    int prefixLength = prefix.length();
    
    juce::String continuationPrefix;
    for (int i = 0; i < prefixLength; ++i)
    {
        continuationPrefix += " ";
    }
    
    juce::String oldValueStr = juce::String(oldValue, 6);
    juce::String newValueStr = juce::String(newValue, 6);
    
    juce::String threadName = getCurrentThreadName();
    
    juce::String continuationLine = "Old: " + oldValueStr + " | New: " + newValueStr;
    
    if (source.isNotEmpty())
    {
        continuationLine += " | Source: " + source;
    }
    
    if (threadName.isNotEmpty())
    {
        continuationLine += " | Thread: " + threadName;
    }
    
    if (prefixLength + firstLine.length() + continuationLine.length() + 3 <= maxLineWidth)
    {
        juce::String message = firstLine + " | " + continuationLine.substring(4);
        logMessage(LogLevel::kDebug, message);
    }
    else
    {
        juce::String fullMessage = prefix + firstLine + "\n" + continuationPrefix + continuationLine;
        
        juce::StringArray lines;
        lines.addLines(fullMessage);
        
        for (int i = 0; i < lines.size(); ++i)
        {
            juce::String line = lines[i];
            if (line.isNotEmpty())
            {
                writeLog(line);
            }
        }
    }
}

void ApvtsLogger::logValueTreePropertyChanged(const juce::Identifier& property,
                                              const juce::var& oldValue,
                                              const juce::var& newValue,
                                              const juce::String& threadName,
                                              const juce::String& choiceLabel)
{
    juce::ignoreUnused(oldValue);
    
    if (LogLevel::kDebug > currentLogLevel)
        return;
    
    juce::String newValueStr;
    
    // Détecter si c'est un timestamp de bouton (très grand nombre)
    // Les timestamps Unix en millisecondes sont > 1000000000000 (année 2001+)
    bool isButtonClick = false;
    if (newValue.isInt64() || newValue.isDouble())
    {
        int64_t value = static_cast<int64_t>(newValue);
        if (value > 1000000000000LL)
        {
            isButtonClick = true;
            newValueStr = "clicked";
        }
    }
    
    if (!isButtonClick)
    {
        newValueStr = formatVarValue(newValue);
        
        // Ajouter le label du choix si fourni
        if (choiceLabel.isNotEmpty())
        {
            newValueStr += " (" + choiceLabel + ")";
        }
    }
    
    juce::String effectiveThreadName = threadName;
    if (effectiveThreadName.isEmpty())
    {
        effectiveThreadName = getCurrentThreadName();
    }
    
    // Format : [DEBUG] timestamp - Thread: xxx | parameter changed: value (label)
    //       ou [DEBUG] timestamp - Thread: xxx | button clicked
    juce::String levelColumn = formatLogLevelColumn(LogLevel::kDebug);
    juce::String timestamp = getTimestamp();
    
    juce::String message = levelColumn + " " + timestamp + " - ";
    
    if (effectiveThreadName.isNotEmpty())
    {
        message += "Thread: " + effectiveThreadName + " | ";
    }
    
    if (isButtonClick)
    {
        message += property.toString() + " > Clicked";
    }
    else
    {
        message += property.toString() + " > New value: " + newValueStr;
    }
    
    writeLog(message);
}

void ApvtsLogger::logAttachmentCreated(const juce::String& parameterId, 
                                       const juce::String& widgetType)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;
    
    juce::String message = "Attachment created: " + parameterId;
    message += " | Widget type: " + widgetType;
    
    logMessage(LogLevel::kInfo, message);
}

void ApvtsLogger::logAttachmentDestroyed(const juce::String& parameterId)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;
    
    juce::String message = "Attachment destroyed: " + parameterId;
    
    logMessage(LogLevel::kInfo, message);
}

void ApvtsLogger::logStateReplaced()
{
    if (LogLevel::kInfo > currentLogLevel)
        return;
    
    juce::String message = "APVTS state replaced";
    
    logMessage(LogLevel::kInfo, message);
}

void ApvtsLogger::logStateLoaded(const juce::String& source)
{
    if (LogLevel::kInfo > currentLogLevel)
        return;
    
    juce::String message = "APVTS state loaded";
    if (source.isNotEmpty())
    {
        message += " from: " + source;
    }
    
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
    {
        std::cout << formattedMessage.toRawUTF8() << std::endl;
    }
    
#if APVTS_LOGGER_ENABLED
    if (logToFile && fileStream && fileStream->is_open())
    {
        *fileStream << formattedMessage.toRawUTF8() << std::endl;
        fileStream->flush();
    }
#endif
}

juce::String ApvtsLogger::buildTimestampString() const
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

juce::String ApvtsLogger::getTimestamp() const
{
    return buildTimestampString();
}

juce::String ApvtsLogger::formatVarValue(const juce::var& value) const
{
    if (value.isVoid())
    {
        return "void";
    }
    
    if (value.isBool())
    {
        return value.toString();
    }
    
    if (value.isInt())
    {
        return value.toString();
    }
    
    if (value.isInt64())
    {
        return value.toString();
    }
    
    if (value.isDouble())
    {
        double dVal = value.operator double();
        // Si c'est un entier déguisé en double (ex: 25.000000)
        if (std::abs(dVal - std::round(dVal)) < 0.0001)
        {
            return juce::String(static_cast<int>(std::round(dVal)));
        }
        return juce::String(dVal, 2);
    }
    
    if (value.isString())
    {
        return "\"" + value.toString() + "\"";
    }
    
    return value.toString();
}

juce::String ApvtsLogger::getCurrentThreadName() const
{
    juce::String threadName;
    
    if (juce::Thread::getCurrentThread() != nullptr)
    {
        threadName = juce::Thread::getCurrentThread()->getThreadName();
    }
    else if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        threadName = "MessageThread";
    }
    else
    {
        threadName = "Unknown";
    }
    
    // Simplifier les noms de threads
    if (threadName == "MessageThread")
        return "Message";
    if (threadName.startsWith("Audio"))
        return "Audio";
    if (threadName.startsWith("MIDI") || threadName.startsWith("Midi"))
        return "MIDI";
    
    return threadName;
}

