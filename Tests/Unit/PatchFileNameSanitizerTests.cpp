#include <juce_core/juce_core.h>

#include "Core/Services/PatchFileNameSanitizer.h"

class PatchFileNameSanitizerTests : public juce::UnitTest
{
public:
    PatchFileNameSanitizerTests() : juce::UnitTest("PatchFileNameSanitizer") {}

    void runTest() override
    {
        sanitize_truncatesToEight();
        sanitize_stripsForbiddenOsChars();
        sanitize_lowercaseFoldsToUpper();
        sanitize_emptyFallback();
        sanitizeFileStem_stripsPathAndExtension();
    }

private:
    void sanitize_truncatesToEight()
    {
        beginTest("sanitize_truncatesToEight");

        const auto result = Core::PatchFileNameSanitizer::sanitizeToMatrixName("LONGNAMEHERE");
        expect(result.length() <= Core::PatchFileNameSanitizer::kMaxNameLength);
        expect(result == "LONGNAME");
    }

    void sanitize_stripsForbiddenOsChars()
    {
        beginTest("sanitize_stripsForbiddenOsChars");

        const auto result = Core::PatchFileNameSanitizer::sanitizeFileStem("NAME*?:.syx");
        expect(! result.contains("*"));
        expect(! result.contains(":"));
        expect(! result.contains("?"));
        expect(result == "NAME");
    }

    void sanitize_lowercaseFoldsToUpper()
    {
        beginTest("sanitize_lowercaseFoldsToUpper");

        const auto result = Core::PatchFileNameSanitizer::sanitizeToMatrixName("abc123");
        expect(result == "ABC123");
    }

    void sanitize_emptyFallback()
    {
        beginTest("sanitize_emptyFallback");

        const auto result = Core::PatchFileNameSanitizer::sanitizeToMatrixName("");
        expect(result == Core::PatchFileNameSanitizer::kEmptyNameFallback);
    }

    void sanitizeFileStem_stripsPathAndExtension()
    {
        beginTest("sanitizeFileStem_stripsPathAndExtension");

        const auto result = Core::PatchFileNameSanitizer::sanitizeFileStem("/tmp/folder/MY-PATCH.syx");
        expect(result == "MY-PATCH");
    }
};

static PatchFileNameSanitizerTests patchFileNameSanitizerTests;
