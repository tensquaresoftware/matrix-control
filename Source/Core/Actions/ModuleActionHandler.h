#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/IActionHandler.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/MasterModuleInitService.h"

namespace Core
{
    class ApvtsPatchMapper;
    class ClipboardService;
    class MatrixModInitService;
    class PatchModel;

    class ModuleActionHandler final : public IActionHandler
    {
    public:
        using RefreshPasteMirrorsCallback = std::function<void()>;

        ModuleActionHandler(juce::AudioProcessorValueTreeState& apvts,
                            PatchModel* patchModel,
                            ApvtsPatchMapper* apvtsPatchMapper,
                            ClipboardService* clipboardService,
                            MatrixModInitService* matrixModInitService,
                            MasterModuleInitService* masterModuleInitService,
                            RefreshPasteMirrorsCallback refreshPasteMirrors,
                            ActionExecutionHooks hooks);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

    private:
        void handleMatrixModInit(const juce::String& propertyId);
        void handleMasterModuleInit(const juce::String& propertyId);
        void handleClipboardCopy(const juce::String& propertyId);
        void propagateInitTemplateFooterMessage(const InitTemplateLoadResult& result);
        int parseMatrixModBusInitIndex(const juce::String& propertyId) const;

        juce::AudioProcessorValueTreeState& apvts_;
        PatchModel* patchModel_;
        ApvtsPatchMapper* apvtsPatchMapper_;
        ClipboardService* clipboardService_;
        MatrixModInitService* matrixModInitService_;
        MasterModuleInitService* masterModuleInitService_;
        RefreshPasteMirrorsCallback refreshPasteMirrors_;
        ActionExecutionHooks hooks_;
    };

} // namespace Core
