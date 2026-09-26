#pragma once

#include <juce_core/juce_core.h>

class SysExDecoder;

namespace Core
{
    // Drag-drop / drop-gate peek for Master user files. Does not touch a live MasterModel.
    // Distinct from PatchFileService patch assess (275 / .m1kp) — Master is 351 .syx / 344 .m1km.
    namespace MasterFileAssess
    {
        enum class Format
        {
            kNone,
            kSyx,
            kM1km
        };

        struct Assessment
        {
            bool isValidMaster = false;
            Format format = Format::kNone;
        };

        bool hasSyxExtension(const juce::File& file) noexcept;
        Assessment assess(const juce::File& file, SysExDecoder& decoder);
    }
} // namespace Core
