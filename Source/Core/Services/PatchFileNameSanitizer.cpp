#include "Core/Services/PatchFileNameSanitizer.h"

namespace Core
{

    juce::String PatchFileNameSanitizer::sanitizeFileStem(juce::String input)
    {
        if (input.containsChar('/') || input.containsChar('\\'))
        {
            const int lastSep = juce::jmax(input.lastIndexOfChar('/'), input.lastIndexOfChar('\\'));
            input = input.substring(lastSep + 1);
        }

        if (input.endsWithIgnoreCase(".syx"))
            input = input.upToLastOccurrenceOf(".", false, false);

        return sanitizeToMatrixName(stripOsForbiddenChars(input.trim()));
    }

    juce::String PatchFileNameSanitizer::sanitizeToMatrixName(juce::String stem)
    {
        stem = stem.toUpperCase();
        juce::String filtered;

        for (auto character : stem)
        {
            if (isAllowedMatrixChar(character))
                filtered += character;
        }

        filtered = filtered.substring(0, kMaxNameLength);

        if (filtered.isEmpty())
            return kEmptyNameFallback;

        return filtered;
    }

    juce::String PatchFileNameSanitizer::ensureSyxExtension(const juce::String& stem)
    {
        if (stem.endsWithIgnoreCase(".syx"))
            return stem;

        return stem + ".syx";
    }

    juce::String PatchFileNameSanitizer::stripOsForbiddenChars(juce::String text)
    {
        for (int i = text.length(); --i >= 0;)
        {
            const auto character = text[i];

            if (character < 32
                || character == '/'
                || character == '\\'
                || character == ':'
                || character == '*'
                || character == '?'
                || character == '"'
                || character == '<'
                || character == '>'
                || character == '|')
            {
                text = text.substring(0, i) + text.substring(i + 1);
            }
        }

        return text;
    }

    bool PatchFileNameSanitizer::isAllowedMatrixChar(juce::juce_wchar character) noexcept
    {
        return (character >= 'A' && character <= 'Z')
            || (character >= '0' && character <= '9')
            || character == ' '
            || character == '-'
            || character == '_';
    }

} // namespace Core
