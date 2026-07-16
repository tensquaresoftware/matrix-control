#include "Core/Services/PatchMutator/PatchLoadContext.h"

#include "Core/Services/PatchFileNameSanitizer.h"

namespace Core
{
    namespace
    {
        constexpr const char* kComputerBasenamePrefix = "Syx-";

        juce::String formatBankPatch(int bank, int patch)
        {
            return juce::String::formatted("B%02d-P%02d", bank, patch);
        }
    } // namespace

    PatchLoadContext PatchLoadContext::deviceMemory(int bank, int patch)
    {
        PatchLoadContext context;
        context.origin = Origin::kDeviceMemory;
        context.bank = bank;
        context.patch = patch;
        return context;
    }

    PatchLoadContext PatchLoadContext::computerFile(const juce::String& fileStem)
    {
        PatchLoadContext context;
        context.origin = Origin::kComputerFile;
        context.fileStem = fileStem;
        return context;
    }

    juce::String PatchLoadContext::computeExportBasename(const juce::String& patchName) const
    {
        if (origin == Origin::kComputerFile)
            return computeComputerBasename();

        return computeDeviceBasename(patchName);
    }

    juce::String PatchLoadContext::computeDeviceBasename(const juce::String& patchName) const
    {
        const auto base = formatBankPatch(bank, patch);
        const auto name = PatchFileNameSanitizer::sanitizeToMatrixNameOrEmpty(patchName.trim());

        if (name.isEmpty())
            return base;

        return base + "-" + name;
    }

    juce::String PatchLoadContext::computeComputerBasename() const
    {
        const auto stem = PatchFileNameSanitizer::sanitizeToMatrixNameOrEmpty(
            PatchFileNameSanitizer::sanitizeFileStem(fileStem));

        if (stem.isEmpty())
            return juce::String(kComputerBasenamePrefix) + PatchFileNameSanitizer::kEmptyNameFallback;

        return kComputerBasenamePrefix + stem;
    }

} // namespace Core
