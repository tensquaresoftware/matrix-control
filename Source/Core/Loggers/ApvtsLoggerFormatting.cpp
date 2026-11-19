#include <cmath>

#include "ApvtsLogger.h"

#include <juce_core/juce_core.h>

juce::String ApvtsLogger::formatLogLevelColumn(LogLevel level) const
{
    const juce::String levelName = kLogLevelNames[static_cast<int>(level)];
    return ("[" + levelName + "]").paddedRight(' ', kLogLevelColumnWidth);
}

int ApvtsLogger::findBreakPreferringPunctuation(const juce::String& message,
                                                int rangeStart,
                                                int rangeEnd)
{
    for (int i = rangeEnd; i > rangeStart && i > rangeEnd - 30; --i)
    {
        const juce::juce_wchar ch = message[i];
        if (ch == '.' || ch == ':')
            return i + 1;
    }

    return rangeEnd;
}

int ApvtsLogger::findBreakAtSpace(const juce::String& message,
                                  int rangeStart,
                                  int rangeEnd,
                                  bool keepSpace)
{
    for (int i = rangeEnd; i > rangeStart && i > rangeEnd - 30; --i)
    {
        if (message[i] == ' ')
            return keepSpace ? i + 1 : i;
    }

    return rangeEnd;
}

int ApvtsLogger::skipLeadingSpaces(const juce::String& message, int position)
{
    while (position < message.length() && message[position] == ' ')
        ++position;

    return position;
}

juce::String ApvtsLogger::wrapFirstLine(const juce::String& prefix,
                                        const juce::String& message,
                                        int availableWidth,
                                        int& messageStart) const
{
    const int firstLineEnd = messageStart + availableWidth;

    if (firstLineEnd >= message.length())
    {
        const juce::String line = prefix + message.substring(messageStart);
        messageStart = message.length();
        return line;
    }

    int breakPoint = findBreakPreferringPunctuation(message, messageStart, firstLineEnd);
    if (breakPoint == firstLineEnd)
        breakPoint = findBreakAtSpace(message, messageStart, firstLineEnd, true);

    const juce::String line = prefix + message.substring(messageStart, breakPoint).trimEnd() + "\n";
    messageStart = skipLeadingSpaces(message, breakPoint);
    return line;
}

juce::String ApvtsLogger::wrapContinuationLine(const juce::String& message,
                                               int maxLineWidth,
                                               int& messageStart) const
{
    const int lineEnd = messageStart + maxLineWidth;

    if (lineEnd >= message.length())
    {
        const juce::String line = message.substring(messageStart);
        messageStart = message.length();
        return line;
    }

    const int breakPoint = findBreakAtSpace(message, messageStart, lineEnd, false);
    const juce::String line = message.substring(messageStart, breakPoint) + "\n";
    messageStart = skipLeadingSpaces(message, breakPoint);
    return line;
}

int ApvtsLogger::getMessageColumnIndentWidth() const
{
    // "[LEVEL] " + "HH:MM:SS.mmm" + " - "
    return kLogLevelColumnWidth + 1 + 12 + 3;
}

juce::String ApvtsLogger::wrapLogMessage(const juce::String& prefix,
                                         const juce::String& message) const
{
    const int maxLineWidth = getEffectiveLineWidth();
    const int prefixLength = prefix.length();
    const int availableWidth = maxLineWidth - prefixLength;

    if (availableWidth <= 0 || prefixLength + message.length() <= maxLineWidth)
        return prefix + message;

    const juce::String indent = juce::String::repeatedString(" ", prefixLength);
    juce::String wrappedMessage;
    int messageStart = 0;
    bool isFirstLine = true;

    while (messageStart < message.length())
    {
        if (isFirstLine)
        {
            wrappedMessage += wrapFirstLine(prefix, message, availableWidth, messageStart);
            isFirstLine = false;
        }
        else
        {
            wrappedMessage += indent + wrapContinuationLine(message, availableWidth, messageStart);
        }
    }

    return wrappedMessage.trimEnd();
}

void ApvtsLogger::logMessage(LogLevel level, const juce::String& message)
{
    if (level > currentLogLevel)
        return;

    const juce::String prefix = formatLogLevelColumn(level) + " " + getTimestamp() + " - ";
    writeNonEmptyLines(wrapLogMessage(prefix, message));
}

juce::String ApvtsLogger::buildParameterChangeContinuation(float oldValue,
                                                           float newValue,
                                                           const juce::String& source,
                                                           const juce::String& threadName) const
{
    juce::String continuationLine = "Old: " + juce::String(oldValue, 6)
                                  + " | New: " + juce::String(newValue, 6);

    if (source.isNotEmpty())
        continuationLine += " | Source: " + source;

    if (threadName.isNotEmpty())
        continuationLine += " | Thread: " + threadName;

    return continuationLine;
}

void ApvtsLogger::logParameterChanged(const juce::String& parameterId,
                                      float oldValue,
                                      float newValue,
                                      const juce::String& source)
{
    if (LogLevel::kDebug > currentLogLevel)
        return;

    const juce::String prefix = formatLogLevelColumn(LogLevel::kDebug) + " "
                              + getTimestamp() + " - ";
    const juce::String firstLine = "Parameter changed: " + parameterId;
    const juce::String continuationLine = buildParameterChangeContinuation(
        oldValue, newValue, source, getCurrentThreadName());
    const int prefixLength = prefix.length();
    const int maxLineWidth = getEffectiveLineWidth();

    if (prefixLength + firstLine.length() + continuationLine.length() + 3 <= maxLineWidth)
    {
        logMessage(LogLevel::kDebug, firstLine + " | " + continuationLine.substring(4));
        return;
    }

    // Both lines go through wrapLogMessage so long parameter IDs still stay in column 3.
    writeNonEmptyLines(wrapLogMessage(prefix, firstLine));
    writeNonEmptyLines(wrapLogMessage(juce::String::repeatedString(" ", prefixLength),
                                      continuationLine));
}

bool ApvtsLogger::isButtonClickTimestamp(const juce::var& value) const
{
    if (!value.isInt64() && !value.isDouble())
        return false;

    // Unix millisecond timestamps after year 2001 exceed this threshold.
    return static_cast<juce::int64>(value) > kButtonClickTimestampThresholdMs;
}

juce::String ApvtsLogger::formatPropertyNewValue(const juce::var& newValue,
                                                 const juce::String& choiceLabel,
                                                 bool isButtonClick) const
{
    if (isButtonClick)
        return "clicked";

    juce::String newValueStr = formatVarValue(newValue);
    if (choiceLabel.isNotEmpty())
        newValueStr += " (" + choiceLabel + ")";

    return newValueStr;
}

juce::String ApvtsLogger::buildValueTreePropertyMessage(const ValueTreePropertyChangeLog& change,
                                                        bool isButtonClick,
                                                        const juce::String& newValueStr) const
{
    // Body only — logMessage() adds [LEVEL]/timestamp and wraps under the message column.
    juce::String effectiveThreadName = change.threadName;
    if (effectiveThreadName.isEmpty())
        effectiveThreadName = getCurrentThreadName();

    juce::String message;
    if (effectiveThreadName.isNotEmpty())
        message += "Thread: " + effectiveThreadName + " | ";

    if (isButtonClick)
        message += change.property.toString() + " > Clicked";
    else
        message += change.property.toString() + " > New value: " + newValueStr;

    return message;
}

void ApvtsLogger::logValueTreePropertyChanged(const ValueTreePropertyChangeLog& change)
{
    juce::ignoreUnused(change.oldValue);

    if (LogLevel::kDebug > currentLogLevel)
        return;

    const bool isButtonClick = isButtonClickTimestamp(change.newValue);
    const juce::String newValueStr = formatPropertyNewValue(
        change.newValue, change.choiceLabel, isButtonClick);

    logMessage(LogLevel::kDebug, buildValueTreePropertyMessage(change, isButtonClick, newValueStr));
}

juce::String ApvtsLogger::buildTimestampString() const
{
    const juce::Time now = juce::Time::getCurrentTime();

    juce::String timestamp;
    timestamp += juce::String(now.getHours()).paddedLeft('0', 2);
    timestamp += ":";
    timestamp += juce::String(now.getMinutes()).paddedLeft('0', 2);
    timestamp += ":";
    timestamp += juce::String(now.getSeconds()).paddedLeft('0', 2);
    timestamp += ".";
    timestamp += juce::String(now.getMilliseconds()).paddedLeft('0', 3);

    return timestamp;
}

juce::String ApvtsLogger::getTimestamp() const
{
    return buildTimestampString();
}

juce::String ApvtsLogger::formatVarValue(const juce::var& value) const
{
    if (value.isVoid())
        return "void";

    if (value.isBool() || value.isInt() || value.isInt64())
        return value.toString();

    if (value.isDouble())
    {
        const double dVal = value.operator double();
        // Whole numbers stored as double (e.g. 25.000000) print as integers.
        if (std::abs(dVal - std::round(dVal)) < 0.0001)
            return juce::String(static_cast<int>(std::round(dVal)));

        return juce::String(dVal, 2);
    }

    if (value.isString())
        return "\"" + value.toString() + "\"";

    return value.toString();
}

juce::String ApvtsLogger::getCurrentThreadName() const
{
    juce::String threadName;

    if (juce::Thread::getCurrentThread() != nullptr)
        threadName = juce::Thread::getCurrentThread()->getThreadName();
    else if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        threadName = "MessageThread";
    else
        threadName = "Unknown";

    if (threadName == "MessageThread")
        return "Message";
    if (threadName.startsWith("Audio"))
        return "Audio";
    if (threadName.startsWith("MIDI") || threadName.startsWith("Midi"))
        return "MIDI";

    return threadName;
}
