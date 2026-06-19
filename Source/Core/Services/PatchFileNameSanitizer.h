#pragma once

#include <juce_core/juce_core.h>

namespace Core
{

    struct PatchFileNameSanitizer
    {
        static juce::String sanitizeFileStem(juce::String input);
        static juce::String sanitizeToMatrixName(juce::String stem);
        static juce::String ensureSyxExtension(const juce::String& stem);

        static constexpr const char* kEmptyNameFallback = "PATCH";
        static constexpr int kMaxNameLength = 8;

    private:
        static juce::String stripOsForbiddenChars(juce::String text);
        static bool isAllowedMatrixChar(juce::juce_wchar character) noexcept;
    };

} // namespace Core
