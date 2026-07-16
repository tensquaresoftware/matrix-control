#include <juce_core/juce_core.h>

#include "Core/Services/PatchMutator/PatchLoadContext.h"

class PatchLoadContextTests : public juce::UnitTest
{
public:
    PatchLoadContextTests() : juce::UnitTest("PatchLoadContext") {}

    void runTest() override
    {
        deviceNamed_buildsBankPatchName();
        deviceBlankName_omitsNameSegment();
        deviceUnsanitizableName_omitsNameSegment();
        computerFile_prefixesSyx();
    }

private:
    void deviceNamed_buildsBankPatchName()
    {
        beginTest("deviceNamed_buildsBankPatchName");

        const auto context = Core::PatchLoadContext::deviceMemory(8, 25);
        expectEquals(context.computeExportBasename("OB-VOX"), juce::String("B08-P25-OB-VOX"));
    }

    void deviceBlankName_omitsNameSegment()
    {
        beginTest("deviceBlankName_omitsNameSegment");

        const auto context = Core::PatchLoadContext::deviceMemory(0, 0);
        expectEquals(context.computeExportBasename("   "), juce::String("B00-P00"));
        expectEquals(context.computeExportBasename(""), juce::String("B00-P00"));
    }

    void deviceUnsanitizableName_omitsNameSegment()
    {
        beginTest("deviceUnsanitizableName_omitsNameSegment");

        const auto context = Core::PatchLoadContext::deviceMemory(1, 77);
        // '!' and '@' are stripped by the Matrix name filter, leaving nothing.
        expectEquals(context.computeExportBasename("!@"), juce::String("B01-P77"));
    }

    void computerFile_prefixesSyx()
    {
        beginTest("computerFile_prefixesSyx");

        const auto context = Core::PatchLoadContext::computerFile("WARM-PAD");
        expectEquals(context.computeExportBasename("IGNORED"), juce::String("Syx-WARM-PAD"));

        const auto fromExtension = Core::PatchLoadContext::computerFile("WARM-PAD.syx");
        expectEquals(fromExtension.computeExportBasename("IGNORED"), juce::String("Syx-WARM-PAD"));

        const auto blank = Core::PatchLoadContext::computerFile("***");
        expectEquals(blank.computeExportBasename("IGNORED"), juce::String("Syx-PATCH"));
    }
};

static PatchLoadContextTests patchLoadContextTests;
