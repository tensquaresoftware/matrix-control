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
        sanitizeOsPathSegment_keepsSpacesStripsStar();
        sanitizeOsFileStem_fallbackWhenEmpty();
        normalizeMatrixSaveStem_acceptsCaseOnlyFold();
        normalizeMatrixSaveStem_refusesAccentsExoticAndOverlength();
        bankExportFileStem_padsSlotAndAppendsSanitizedName();
        bankExportFileStem_omitsNameWhenSanitizedEmpty();
        bankExportFileStem_clampsOutOfRangeSlots();
        formatBankPatchLabel_usesHyphenatedBxPyy();
        formatOberheimBankPlaceholderName_matchesBnkPattern();
        resolvePatchNameOrBankPatchFallback_keepsUsableNames();
        nameFromBankExportStem_extractsNameAfterSlot();
        isOberheimBankPlaceholderName_detectsBnkPattern();
        isInitPatchNameSentinel_matchesRuntimeLiteral();
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

    void sanitizeOsPathSegment_keepsSpacesStripsStar()
    {
        beginTest("sanitizeOsPathSegment_keepsSpacesStripsStar");

        expectEquals(Core::PatchFileNameSanitizer::sanitizeOsPathSegmentOrEmpty("FLUTE."),
                     juce::String("FLUTE"));
        expectEquals(Core::PatchFileNameSanitizer::sanitizeOsPathSegmentOrEmpty("BS ETAK*"),
                     juce::String("BS ETAK"));
        expectEquals(Core::PatchFileNameSanitizer::sanitizeOsPathSegmentOrEmpty("*'CANOPY"),
                     juce::String("'CANOPY"));
    }

    void sanitizeOsFileStem_fallbackWhenEmpty()
    {
        beginTest("sanitizeOsFileStem_fallbackWhenEmpty");

        expectEquals(Core::PatchFileNameSanitizer::sanitizeOsFileStem("***"),
                     juce::String(Core::PatchFileNameSanitizer::kEmptyNameFallback));
        expectEquals(Core::PatchFileNameSanitizer::sanitizeOsFileStem("WARM PAD.syx"),
                     juce::String("WARM PAD"));
    }

    void normalizeMatrixSaveStem_acceptsCaseOnlyFold()
    {
        beginTest("normalizeMatrixSaveStem_acceptsCaseOnlyFold");

        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("test"),
                     juce::String("TEST"));
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("Warm-Pad.syx"),
                     juce::String("WARM-PAD"));
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("ABCDEFGH"),
                     juce::String("ABCDEFGH"));
        expect(Core::PatchFileNameSanitizer::isExactMatrixFileStem("ab_12"));
        expect(Core::PatchFileNameSanitizer::isExactMatrixFileStem("A B"));
    }

    void normalizeMatrixSaveStem_refusesAccentsExoticAndOverlength()
    {
        beginTest("normalizeMatrixSaveStem_refusesAccentsExoticAndOverlength");

        // "réso" as explicit UTF-8 (avoid source-charset / juce_String literal asserts).
        const auto accented = juce::String::fromUTF8("r\xc3\xa9so");
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty(accented),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("*'CANOPY"),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("TOOLONGNAME"),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("ABCDEFGHI"),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty("   "),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::normalizeMatrixSaveStemOrEmpty(""),
                     juce::String());
        expect(! Core::PatchFileNameSanitizer::isExactMatrixFileStem(accented));
        expect(! Core::PatchFileNameSanitizer::isExactMatrixFileStem("*'CANOPY"));
    }

    void bankExportFileStem_padsSlotAndAppendsSanitizedName()
    {
        beginTest("bankExportFileStem_padsSlotAndAppendsSanitizedName");

        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(3, "WARM PAD"),
                     juce::String("P03. WARM PAD"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(10, "NYLON 12"),
                     juce::String("P10. NYLON 12"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(99, "lead"),
                     juce::String("P99. LEAD"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(0, "INIT"),
                     juce::String("P00. INIT"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(1, "INIT"),
                     juce::String("P01. INIT"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(5, "P99 - DJ"),
                     juce::String("P05. P99 - DJ"));
    }

    void bankExportFileStem_omitsNameWhenSanitizedEmpty()
    {
        beginTest("bankExportFileStem_omitsNameWhenSanitizedEmpty");

        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(0, ""),
                     juce::String("P00"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(7, "   "),
                     juce::String("P07"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(76, ""),
                     juce::String("P76"));
    }

    void bankExportFileStem_clampsOutOfRangeSlots()
    {
        beginTest("bankExportFileStem_clampsOutOfRangeSlots");

        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(-1, ""), juce::String("P00"));
        expectEquals(Core::PatchFileNameSanitizer::bankExportFileStem(150, ""), juce::String("P99"));
    }

    void formatBankPatchLabel_usesHyphenatedBxPyy()
    {
        beginTest("formatBankPatchLabel_usesHyphenatedBxPyy");

        expectEquals(Core::PatchFileNameSanitizer::formatBankPatchLabel(1, 23),
                     juce::String("B1-P23"));
        expectEquals(Core::PatchFileNameSanitizer::formatBankPatchLabel(0, 0),
                     juce::String("B0-P00"));
        expectEquals(Core::PatchFileNameSanitizer::formatBankPatchLabel(9, 99),
                     juce::String("B9-P99"));
        expectEquals(Core::PatchFileNameSanitizer::formatBankPatchLabel(-3, 150),
                     juce::String("B0-P99"));
    }

    void formatOberheimBankPlaceholderName_matchesBnkPattern()
    {
        beginTest("formatOberheimBankPlaceholderName_matchesBnkPattern");

        expectEquals(Core::PatchFileNameSanitizer::formatOberheimBankPlaceholderName(0, 12),
                     juce::String("BNK0: 12"));
        expectEquals(Core::PatchFileNameSanitizer::formatOberheimBankPlaceholderName(4, 5),
                     juce::String("BNK4: 05"));
        expect(Core::PatchFileNameSanitizer::isOberheimBankPlaceholderName(
            Core::PatchFileNameSanitizer::formatOberheimBankPlaceholderName(2, 0)));
    }

    void resolvePatchNameOrBankPatchFallback_keepsUsableNames()
    {
        beginTest("resolvePatchNameOrBankPatchFallback_keepsUsableNames");

        expect(Core::PatchFileNameSanitizer::isUsablePatchName("BNK0:00"));
        expect(Core::PatchFileNameSanitizer::isUsablePatchName("NYLON 12"));
        expect(! Core::PatchFileNameSanitizer::isUsablePatchName(""));
        expect(! Core::PatchFileNameSanitizer::isUsablePatchName("   "));

        expectEquals(Core::PatchFileNameSanitizer::resolvePatchNameOrBankPatchFallback("WARM", 1, 5),
                     juce::String("WARM"));
        expectEquals(Core::PatchFileNameSanitizer::resolvePatchNameOrBankPatchFallback("BNK4: 05", 4, 5),
                     juce::String("BNK4: 05"));
        expectEquals(Core::PatchFileNameSanitizer::resolvePatchNameOrBankPatchFallback("  ", 1, 23),
                     juce::String("B1-P23"));
    }

    void nameFromBankExportStem_extractsNameAfterSlot()
    {
        beginTest("nameFromBankExportStem_extractsNameAfterSlot");

        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P10. NYLON 12"),
                     juce::String("NYLON 12"));
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P00. GOODTIME.syx"),
                     juce::String("GOODTIME"));
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P05. P99 - DJ"),
                     juce::String("P99 - DJ"));
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P76. AA.syx"),
                     juce::String("AA"));
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P07"),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P76."),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P76. "),
                     juce::String());
        // Hyphen legacy / artistic bare stems are not bank-export prefixes.
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P10 - NYLON 12"),
                     juce::String());
        expectEquals(Core::PatchFileNameSanitizer::nameFromBankExportStem("P99 - DJ.syx"),
                     juce::String());
    }

    void isOberheimBankPlaceholderName_detectsBnkPattern()
    {
        beginTest("isOberheimBankPlaceholderName_detectsBnkPattern");

        expect(Core::PatchFileNameSanitizer::isOberheimBankPlaceholderName("BNK4: 05"));
        expect(Core::PatchFileNameSanitizer::isOberheimBankPlaceholderName("bnk0:00"));
        expect(! Core::PatchFileNameSanitizer::isOberheimBankPlaceholderName("GOODTIME"));
        expect(! Core::PatchFileNameSanitizer::isOberheimBankPlaceholderName(""));
    }

    void isInitPatchNameSentinel_matchesRuntimeLiteral()
    {
        beginTest("isInitPatchNameSentinel_matchesRuntimeLiteral");

        expect(Core::PatchFileNameSanitizer::isInitPatchNameSentinel("* INIT *"));
        expect(! Core::PatchFileNameSanitizer::isInitPatchNameSentinel("INIT"));
        expect(! Core::PatchFileNameSanitizer::isInitPatchNameSentinel("--------"));
        expect(! Core::PatchFileNameSanitizer::isInitPatchNameSentinel("*INIT*"));
        expect(! Core::PatchFileNameSanitizer::isExactMatrixFileStem("* INIT *"));
    }
};

static PatchFileNameSanitizerTests patchFileNameSanitizerTests;
