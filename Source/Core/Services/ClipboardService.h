#pragma once

#include <array>
#include <optional>
#include <unordered_map>

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Models/PatchModel.h"

namespace Core
{

    enum class ClipboardMode
    {
        Empty,
        Module,
        FullPatch,
        MatrixModulation
    };

    enum class PatchModuleKind
    {
        Dco1,
        Dco2,
        Env1,
        Env2,
        Env3,
        Lfo1,
        Lfo2
    };

    class ClipboardService
    {
    public:
        static constexpr size_t kMatrixModSnapshotOffset = 104;
        static constexpr size_t kMatrixModSnapshotSize = 30;

        bool hasContent() const noexcept;
        ClipboardMode getMode() const noexcept;
        std::optional<PatchModuleKind> getSourceModuleKind() const noexcept;

        void copyModule(PatchModuleKind source, const PatchModel& model);
        void copyFullPatch(const PatchModel& model);
        void copyMatrixModulation(const PatchModel& model);

        bool canPasteModule(PatchModuleKind target) const noexcept;
        bool canPasteFullPatch() const noexcept;
        bool canPasteMatrixModulation() const noexcept;

        bool pasteModule(PatchModuleKind target, PatchModel& model);
        bool pasteFullPatch(PatchModel& model);
        bool pasteMatrixModulation(PatchModel& model);

    private:
        struct ModuleSnapshot
        {
            std::unordered_map<juce::String, int> intValues;
            std::unordered_map<juce::String, int> choiceIndices;
        };

        static bool isEnvelopeModule(PatchModuleKind kind) noexcept;
        static bool isDcoModule(PatchModuleKind kind) noexcept;
        static bool isLfoModule(PatchModuleKind kind) noexcept;
        static bool areModulesCompatible(PatchModuleKind source, PatchModuleKind target) noexcept;

        void pasteModuleDirect(PatchModuleKind target, PatchModel& model);
        void pasteEnvelopeModule(PatchModuleKind source, PatchModuleKind target, PatchModel& model);
        void pasteDcoModule(PatchModuleKind source, PatchModuleKind target, PatchModel& model);
        void pasteLfoModule(PatchModuleKind source, PatchModuleKind target, PatchModel& model);

        ClipboardMode mode_ { ClipboardMode::Empty };
        PatchModuleKind sourceModuleKind_ { PatchModuleKind::Dco1 };
        ModuleSnapshot moduleSnapshot_;
        std::array<juce::uint8, SysExConstants::kPatchPackedDataSize> fullPatchSnapshot_ {};
        std::array<juce::uint8, kMatrixModSnapshotSize> matrixModSnapshot_ {};
    };

} // namespace Core
