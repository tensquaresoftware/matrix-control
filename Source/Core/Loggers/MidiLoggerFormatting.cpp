#include "MidiLogger.h"

#include "Core/MIDI/Matrix6MasterResponseFilter.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExParser.h"

juce::String MidiLogger::formatLogLevelColumn(LogLevel level) const
{
    const juce::String levelName = kLogLevelNames[static_cast<int>(level)];
    return ("[" + levelName + "]").paddedRight(' ', kLogLevelColumnWidth);
}

int MidiLogger::findBreakPreferringPunctuation(const juce::String& message,
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

int MidiLogger::findBreakAtSpace(const juce::String& message,
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

int MidiLogger::skipLeadingSpaces(const juce::String& message, int position)
{
    while (position < message.length() && message[position] == ' ')
        ++position;

    return position;
}

juce::String MidiLogger::wrapFirstLine(const juce::String& prefix,
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

juce::String MidiLogger::wrapContinuationLine(const juce::String& message,
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

juce::String MidiLogger::wrapLogMessage(const juce::String& prefix,
                                        const juce::String& message) const
{
    const int maxLineWidth = getEffectiveLineWidth();
    const int prefixLength = prefix.length();
    const int availableWidth = maxLineWidth - prefixLength;

    if (prefixLength + message.length() <= maxLineWidth)
        return prefix + message;

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
            wrappedMessage += wrapContinuationLine(message, maxLineWidth, messageStart);
        }
    }

    return wrappedMessage.trimEnd();
}

void MidiLogger::logMessage(LogLevel level, const juce::String& message)
{
    if (level > currentLogLevel)
        return;

    const juce::String prefix = formatLogLevelColumn(level) + " " + getTimestamp() + " - ";
    writeNonEmptyLines(wrapLogMessage(prefix, message));
}

juce::String MidiLogger::buildSysExHeaderMessage(const juce::String& direction,
                                                 const juce::String& description,
                                                 size_t byteCount) const
{
    juce::String message = direction + " SysEx";
    if (description.isNotEmpty())
        message += " (" + description + ")";

    message += " (" + juce::String(byteCount) + " bytes):";
    return message;
}

void MidiLogger::logSysExDataDecimal(const juce::uint8* data,
                                     size_t size,
                                     const juce::String& description)
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
        message += " (" + description + ")";

    message += " (" + juce::String(size) + " bytes): ";

    juce::String dataString;
    for (size_t i = 0; i < size; ++i)
    {
        if (i > 0)
            dataString += ", ";

        dataString += juce::String(static_cast<int>(data[i]));
    }

    const juce::String prefix = formatLogLevelColumn(LogLevel::kInfo) + " "
                              + getTimestamp() + " - ";
    writeNonEmptyLines(wrapLogMessage(prefix, message + dataString));
}

int MidiLogger::calculateBytesPerLine() const
{
    return (getEffectiveLineWidth() - 1) / 3;
}

void MidiLogger::insertNewlineIfNeeded(juce::String& hexString,
                                       size_t currentIndex,
                                       int bytesPerLine) const
{
    if (currentIndex > 0 && currentIndex % static_cast<size_t>(bytesPerLine) == 0)
        hexString += "\n";
}

void MidiLogger::appendHexByteWithSpace(juce::String& hexString,
                                        juce::uint8 byte,
                                        bool isLastByte) const
{
    hexString += juce::String::toHexString(byte).paddedLeft('0', 2).toUpperCase();
    if (!isLastByte)
        hexString += " ";
}

juce::String MidiLogger::formatHexBytesWithLineWrapping(const juce::MemoryBlock& sysEx) const
{
    juce::String hexString;
    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());
    const int bytesPerLine = calculateBytesPerLine();

    for (size_t i = 0; i < sysEx.getSize(); ++i)
    {
        insertNewlineIfNeeded(hexString, i, bytesPerLine);
        appendHexByteWithSpace(hexString, data[i], i == sysEx.getSize() - 1);
    }

    return hexString;
}

juce::String MidiLogger::formatSysExMessage(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() == 0)
        return "empty";

    return formatHexBytesWithLineWrapping(sysEx);
}

juce::String MidiLogger::buildTimestampString() const
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

juce::String MidiLogger::getTimestamp() const
{
    return buildTimestampString();
}

juce::String MidiLogger::analyzeSysExMessage(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < 2)
        return {};

    const juce::MemoryBlock completeSysEx = addSysExDelimiters(sysEx);

    // Matrix-6/6R Master dumps are legitimate at 478/479 bytes; Matrix-1000
    // validateSysEx expects 351 and would log a false ERROR on the receive path.
    if (const auto matrix6Analysis = Core::matrix6MasterDumpReceiveLogAnalysis(completeSysEx);
        matrix6Analysis.isNotEmpty())
    {
        return matrix6Analysis;
    }

    SysExParser parser;
    const auto validation = parser.validateSysEx(completeSysEx);

    if (!validation.isValid)
        return {};

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

    const juce::uint8 checksum = extractChecksumFromSysEx(completeSysEx);
    analysis += " | Checksum: 0x"
              + juce::String::toHexString(checksum).paddedLeft('0', 2).toUpperCase();

    return analysis;
}

juce::uint8 MidiLogger::extractChecksumFromSysEx(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() < 2)
        return 0;

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());

    if (data[0] == SysExConstants::kSysExStart
        && data[1] == SysExConstants::DeviceInquiry::kUniversalNonRealtimeId)
    {
        return 0;
    }

    if (data[sysEx.getSize() - 1] == SysExConstants::kSysExEnd)
        return data[sysEx.getSize() - 2];

    return 0;
}

juce::MemoryBlock MidiLogger::addSysExDelimiters(const juce::MemoryBlock& sysEx) const
{
    if (sysEx.getSize() == 0)
        return sysEx;

    const auto* data = static_cast<const juce::uint8*>(sysEx.getData());

    if (data[0] == SysExConstants::kSysExStart
        && data[sysEx.getSize() - 1] == SysExConstants::kSysExEnd)
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
