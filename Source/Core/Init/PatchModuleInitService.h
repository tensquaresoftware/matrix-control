#pragma once

#include <functional>
#include <optional>

#include <juce_core/juce_core.h>

#include "Core/Init/InitTemplateLoader.h"
#include "Core/Services/ClipboardService.h"

namespace Core
{
    class ApvtsPatchMapper;
    class InitTemplateLoader;
    class PatchModel;
    class PatchParameterSysExDispatcher;

    /// Resets one PATCH module slice from PatchInit.syx (or InitDefaults fallback).
    /// Message-thread only — no GUI dependencies.
    class PatchModuleInitService
    {
    public:
        using TemplatesFolderSupplier = std::function<juce::File()>;

        PatchModuleInitService(PatchModel& patchModel,
                               ApvtsPatchMapper& apvtsPatchMapper,
                               InitTemplateLoader& initTemplateLoader,
                               const PatchParameterSysExDispatcher& sysExDispatcher,
                               TemplatesFolderSupplier templatesFolder);

        InitTemplateLoadResult initModule(const juce::String& moduleGroupId);

        static juce::String moduleGroupIdFromInitPropertyId(const juce::String& propertyId) noexcept;
        static juce::String moduleGroupIdFromPatchModuleKind(PatchModuleKind kind) noexcept;

    private:
        void copyModuleFromInitTemplate(const PatchModel& initTemplate,
                                        const juce::String& moduleGroupId);

        PatchModel& patchModel_;
        ApvtsPatchMapper& apvtsPatchMapper_;
        InitTemplateLoader& initTemplateLoader_;
        const PatchParameterSysExDispatcher& sysExDispatcher_;
        TemplatesFolderSupplier templatesFolder_;
    };

} // namespace Core
