#pragma once

#include <functional>

#include <juce_core/juce_core.h>

#include "Core/Init/InitTemplateLoader.h"

namespace Core
{

    class InitTemplateLoader;
    class PatchModel;

    /// Loads a full patch buffer from PatchInit.syx (or InitDefaults fallback).
    /// Caller owns APVTS suppress flags and bufferToApvts push.
    class PatchInitService
    {
    public:
        using TemplatesFolderSupplier = std::function<juce::File()>;

        PatchInitService(PatchModel& patchModel,
                         InitTemplateLoader& initTemplateLoader,
                         TemplatesFolderSupplier templatesFolder);

        InitTemplateLoadResult initFullPatch();

    private:
        PatchModel& patchModel_;
        InitTemplateLoader& initTemplateLoader_;
        TemplatesFolderSupplier templatesFolder_;
    };

} // namespace Core
