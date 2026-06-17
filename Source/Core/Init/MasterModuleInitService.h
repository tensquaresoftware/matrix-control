#pragma once

#include <functional>

#include <juce_core/juce_core.h>

#include "Core/Init/InitTemplateLoader.h"

namespace Core
{
    class ApvtsMasterMapper;
    class InitTemplateLoader;
    class MasterModel;
    class MasterParameterSysExDispatcher;

    enum class MasterModuleKind { kMidi, kVibrato, kMisc };

    /// Resets one MASTER module slice from MasterInit.syx (or InitDefaults fallback).
    /// Message-thread only — no GUI dependencies.
    class MasterModuleInitService
    {
    public:
        using TemplatesFolderSupplier = std::function<juce::File()>;

        MasterModuleInitService(MasterModel& masterModel,
                                ApvtsMasterMapper& apvtsMasterMapper,
                                InitTemplateLoader& initTemplateLoader,
                                const MasterParameterSysExDispatcher& sysExDispatcher,
                                TemplatesFolderSupplier templatesFolder);

        InitTemplateLoadResult initModule(MasterModuleKind module);

    private:
        static juce::String moduleGroupIdFor(MasterModuleKind module) noexcept;
        void copyModuleFromInitTemplate(const MasterModel& initTemplate,
                                        const juce::String& moduleGroupId);

        MasterModel& masterModel_;
        ApvtsMasterMapper& apvtsMasterMapper_;
        InitTemplateLoader& initTemplateLoader_;
        const MasterParameterSysExDispatcher& sysExDispatcher_;
        TemplatesFolderSupplier templatesFolder_;
    };

} // namespace Core
