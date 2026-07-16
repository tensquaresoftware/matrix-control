#pragma once

#include <juce_core/juce_core.h>

namespace Core
{

    // Explicit single source of truth for where the editor patch currently came from.
    // Updated at real load sites (Computer .syx, device dump after nav, INIT, PASTE) and
    // frozen by PatchMutatorEngine on the first MUTATE to build stable Export folder names.
    // Deliberately NOT an Observer framework (see spec: use an explicit context, not events).
    class PatchLoadContext
    {
    public:
        enum class Origin
        {
            kDeviceMemory,
            kComputerFile
        };

        Origin origin { Origin::kDeviceMemory };
        int bank { 0 };
        int patch { 0 };
        juce::String fileStem; // Computer origin only — raw stem, sanitized on demand.

        static PatchLoadContext deviceMemory(int bank, int patch);
        static PatchLoadContext computerFile(const juce::String& fileStem);

        // Export session folder basename per spec:
        //   Device / INIT / PASTE : B{bb:02d}-P{pp:02d}[-NAME]  (NAME omitted when it sanitizes empty)
        //   Computer file         : Syx-{sanitizedFileStem}
        juce::String computeExportBasename(const juce::String& patchName) const;

    private:
        juce::String computeDeviceBasename(const juce::String& patchName) const;
        juce::String computeComputerBasename() const;
    };

} // namespace Core
