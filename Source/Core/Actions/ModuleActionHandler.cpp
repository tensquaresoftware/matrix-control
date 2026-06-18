#include "Core/Actions/ModuleActionHandler.h"

#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/Init/MatrixModInitService.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/ClipboardPasteEnabledResolver.h"
#include "Core/Services/ClipboardService.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    ModuleActionHandler::ModuleActionHandler(juce::AudioProcessorValueTreeState& apvts,
                                             PatchModel* patchModel,
                                             ApvtsPatchMapper* apvtsPatchMapper,
                                             ClipboardService* clipboardService,
                                             MatrixModInitService* matrixModInitService,
                                             MasterModuleInitService* masterModuleInitService,
                                             RefreshPasteMirrorsCallback refreshPasteMirrors,
                                             ActionExecutionHooks hooks)
        : apvts_(apvts)
        , patchModel_(patchModel)
        , apvtsPatchMapper_(apvtsPatchMapper)
        , clipboardService_(clipboardService)
        , matrixModInitService_(matrixModInitService)
        , masterModuleInitService_(masterModuleInitService)
        , refreshPasteMirrors_(std::move(refreshPasteMirrors))
        , hooks_(std::move(hooks))
    {
    }

    void ModuleActionHandler::handleAction(const juce::String& propertyId, const juce::var&)
    {
        handleMatrixModInit(propertyId);
        handleMasterModuleInit(propertyId);
        handleClipboardCopy(propertyId);

        if (propertyId.endsWith("Paste"))
            return; // Story 7.2

        if (propertyId.endsWith("Init"))
            return; // Patch module init — Story 7.2
    }

    void ModuleActionHandler::handleMatrixModInit(const juce::String& propertyId)
    {
        using namespace PluginIDs::MatrixModulationSection;

        if (matrixModInitService_ == nullptr)
            return;

        const int busIndex = parseMatrixModBusInitIndex(propertyId);
        const bool isSectionInit = propertyId == StandaloneWidgets::kMatrixModulationInit;

        if (!isSectionInit && busIndex < 0)
            return;

        if (hooks_.setSuppressMatrixModSysEx)
            hooks_.setSuppressMatrixModSysEx(true);

        if (isSectionInit)
            matrixModInitService_->initAllBuses();
        else
            matrixModInitService_->initBus(busIndex);

        if (hooks_.setSuppressMatrixModSysEx)
            hooks_.setSuppressMatrixModSysEx(false);
    }

    void ModuleActionHandler::handleMasterModuleInit(const juce::String& propertyId)
    {
        using namespace PluginIDs::MasterEditSection;

        if (masterModuleInitService_ == nullptr)
            return;

        std::optional<MasterModuleKind> moduleKind;

        if (propertyId == MidiModule::StandaloneWidgets::kInit)
            moduleKind = MasterModuleKind::kMidi;
        else if (propertyId == VibratoModule::StandaloneWidgets::kInit)
            moduleKind = MasterModuleKind::kVibrato;
        else if (propertyId == MiscModule::StandaloneWidgets::kInit)
            moduleKind = MasterModuleKind::kMisc;

        if (!moduleKind.has_value())
            return;

        if (hooks_.setSuppressMasterSysEx)
            hooks_.setSuppressMasterSysEx(true);

        const auto result = masterModuleInitService_->initModule(*moduleKind);

        if (hooks_.setSuppressMasterSysEx)
            hooks_.setSuppressMasterSysEx(false);

        propagateInitTemplateFooterMessage(result);
    }

    void ModuleActionHandler::handleClipboardCopy(const juce::String& propertyId)
    {
        if (clipboardService_ == nullptr || patchModel_ == nullptr || apvtsPatchMapper_ == nullptr)
            return;

        namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
        namespace MatrixMod = PluginIDs::MatrixModulationSection::StandaloneWidgets;

        if (propertyId == InternalPatches::kCopyPatch)
        {
            apvtsPatchMapper_->apvtsToBuffer();
            clipboardService_->copyFullPatch(*patchModel_);
            if (refreshPasteMirrors_)
                refreshPasteMirrors_();
            return;
        }

        if (propertyId == MatrixMod::kMatrixModulationCopy)
        {
            apvtsPatchMapper_->apvtsToBuffer();
            clipboardService_->copyMatrixModulation(*patchModel_);
            if (refreshPasteMirrors_)
                refreshPasteMirrors_();
            return;
        }

        const auto moduleKind = patchModuleKindFromWidgetId(propertyId);
        if (!moduleKind.has_value())
            return;

        if (propertyId.endsWith("Copy"))
        {
            apvtsPatchMapper_->apvtsToBuffer();
            clipboardService_->copyModule(*moduleKind, *patchModel_);
            if (refreshPasteMirrors_)
                refreshPasteMirrors_();
        }
    }

    void ModuleActionHandler::propagateInitTemplateFooterMessage(const InitTemplateLoadResult& result)
    {
        if (result.infoMessage.isEmpty())
        {
            ExceptionPropagator::clearMessage(apvts_);
            return;
        }

        apvts_.state.setProperty("uiMessageText", result.infoMessage, nullptr);

        const auto severity = (result.fallbackReason == InitTemplateFallbackReason::kFileInvalid)
            ? juce::String("warning")
            : juce::String("info");
        apvts_.state.setProperty("uiMessageSeverity", severity, nullptr);
    }

    int ModuleActionHandler::parseMatrixModBusInitIndex(const juce::String& propertyId) const
    {
        using namespace PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets;

        if (propertyId == kBus0Init) return 0;
        if (propertyId == kBus1Init) return 1;
        if (propertyId == kBus2Init) return 2;
        if (propertyId == kBus3Init) return 3;
        if (propertyId == kBus4Init) return 4;
        if (propertyId == kBus5Init) return 5;
        if (propertyId == kBus6Init) return 6;
        if (propertyId == kBus7Init) return 7;
        if (propertyId == kBus8Init) return 8;
        if (propertyId == kBus9Init) return 9;

        return -1;
    }

} // namespace Core
