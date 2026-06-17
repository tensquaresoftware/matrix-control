#pragma once

#include <cstddef>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"

namespace Core
{

    // Hardcoded fallback packed buffers for PATCH and MASTER init templates when user
    // PatchInit.syx / MasterInit.syx files are absent (D-034). Story 3.2 InitTemplateLoader
    // falls back to these pointers on file miss. No file I/O, APVTS, or PluginProcessor wiring.
    class InitDefaults
    {
    public:
        static constexpr size_t kPatchSize = SysExConstants::kPatchPackedDataSize;
        static constexpr size_t kMasterSize = SysExConstants::kMasterPackedDataSize;

        // True when patch defaults are an interim placeholder pending PatchInit.syx grounding.
        static constexpr bool kPatchDefaultsArePlaceholder = false;

        static const juce::uint8* patchData() noexcept;
        static const juce::uint8* masterData() noexcept;

        InitDefaults() = delete;
    };

} // namespace Core
