// Extracted from PluginProcessor.cpp for modular maintenance.
// Construction phases: the constructor body, split into ordered steps.

#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Core/Actions/ActionDispatcher.h"
#include "Core/Actions/ActionExecutionHooks.h"
#include "Core/Actions/ModuleActionHandler.h"
#include "Core/Actions/MutatorActionHandler.h"
#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/Init/MatrixModInitService.h"
#include "Core/Init/PatchInitService.h"
#include "Core/Init/PatchModuleInitService.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MatrixModBusParameterSysExDispatcher.h"
#include "Core/MIDI/MatrixModBusReorderService.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/MIDI/PatchSelectionMidiSync.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Shared/ProjectPaths.h"
#include "Core/Services/PatchFileService.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/Util/ComboboxPatchSendDebouncer.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PluginProcessorInternal;

void PluginProcessor::createSysExDispatchers()
{
    patchParameterSysExDispatcher_ = std::make_unique<Core::PatchParameterSysExDispatcher>(
        *patchModel_,
        [this](int parameterNumber, juce::uint8 packedValue)
        {
            midiManager->enqueueRemoteParameterEdit(parameterNumber, packedValue);
        });

    masterParameterSysExDispatcher_ = std::make_unique<Core::MasterParameterSysExDispatcher>(
        *masterModel_,
        [this](const juce::uint8* packedData)
        {
            midiManager->sendMaster(0x03, packedData);
        });

    matrixModBusParameterSysExDispatcher_ = std::make_unique<Core::MatrixModBusParameterSysExDispatcher>(
        *patchModel_,
        [this](juce::uint8 bus, juce::uint8 source, juce::uint8 amount, juce::uint8 destination)
        {
            midiManager->enqueueMatrixModBusEdit(bus, source, amount, destination);
        });

    matrixModSysExCoalesceTimer_ = std::make_unique<MatrixModSysExCoalesceTimer>(
        *matrixModBusParameterSysExDispatcher_);

    masterEditSysExDebouncer_ = std::make_unique<Core::ComboboxPatchSendDebouncer>(
        Core::kMasterEditSysExDebounceMs);

    matrixModBusReorderService_ = std::make_unique<Core::MatrixModBusReorderService>(
        *patchModel_,
        *apvtsPatchMapper_,
        *matrixModBusParameterSysExDispatcher_);

    matrixModInitService_ = std::make_unique<Core::MatrixModInitService>(
        *patchModel_,
        *apvtsPatchMapper_,
        *matrixModBusParameterSysExDispatcher_);

    sysExParser_ = std::make_unique<SysExParser>();
    sysExDecoder_ = std::make_unique<SysExDecoder>(*sysExParser_);
    patchFileService_ = std::make_unique<Core::PatchFileService>(*sysExDecoder_);
}

void PluginProcessor::createInitAndFileServices()
{
    initTemplateLoader_ = std::make_unique<Core::InitTemplateLoader>(*sysExDecoder_);
    const auto fixedInitTemplatesFolder = []()
    {
        return ProjectPaths::getInitTemplatesDirectory();
    };

    masterModuleInitService_ = std::make_unique<Core::MasterModuleInitService>(
        *masterModel_,
        *apvtsMasterMapper_,
        *initTemplateLoader_,
        *masterParameterSysExDispatcher_,
        fixedInitTemplatesFolder);

    patchModuleInitService_ = std::make_unique<Core::PatchModuleInitService>(
        *patchModel_,
        *apvtsPatchMapper_,
        *initTemplateLoader_,
        *patchParameterSysExDispatcher_,
        fixedInitTemplatesFolder);

    patchInitService_ = std::make_unique<Core::PatchInitService>(
        *patchModel_,
        *initTemplateLoader_,
        fixedInitTemplatesFolder);

    patchSelectionMidiSync_ = std::make_unique<Core::PatchSelectionMidiSync>(midiManager.get());
}

void PluginProcessor::createActionSubsystem()
{
    Core::ActionExecutionHooks actionHooks{
        .setSuppressMatrixModSysEx = [this](bool suppress) { suppressMatrixModParameterSysEx_ = suppress; },
        .setSuppressMasterSysEx =
            [this](bool suppress)
            {
                if (suppress)
                    cancelMasterEditSysExDebounce();
                suppressMasterParameterSysEx_ = suppress;
            },
        .setSuppressPatchSysEx = [this](bool suppress) { suppressPatchParameterSysEx_ = suppress; },
        .setSuppressPatchSelectionMidiSync = [this](bool suppress) { suppressPatchSelectionMidiSync_ = suppress; },
        .setSuppressMutatorHistorySelectionDebounce =
            [this](bool suppress) { suppressMutatorHistorySelectionDebounce_ = suppress; },
        .setPatchLoadContext = [this](const Core::PatchLoadContext& context) { patchLoadContext_ = context; },
        .onEditorialCheckpoint = [this]() { establishEditorialCheckpoint(); },
        .beginEditorialTransaction = [this](const juce::String& name) { beginEditorialTransaction(name); }};

    createPatchMutatorEngine(actionHooks);
    createModuleActionHandler(actionHooks);
    createPatchManagerActionHandler(actionHooks);
    createMutatorActionHandler();

    actionDispatcher_ = std::make_unique<Core::ActionDispatcher>(
        *moduleActionHandler_,
        *patchManagerActionHandler_,
        *mutatorActionHandler_);
}

void PluginProcessor::createPatchMutatorEngine(Core::ActionExecutionHooks& hooks)
{
    patchMutatorEngine_ = std::make_unique<Core::PatchMutatorEngine>(
        Core::PatchMutatorEngine::Dependencies {
            patchModel_.get(),
            apvtsPatchMapper_.get(),
            patchNameSyncer_.get(),
            midiManager.get(),
            apvts,
            [this]() { return getCurrentPatchNumberForMutator(); },
            [this]() { return getResolvedDeviceMemoryLimits(); },
            patchFileService_.get(),
            &midiManager->getSysExEncoder() },
        hooks);

    patchMutatorEngine_->setPatchLoadContextProvider([this]() { return patchLoadContext_; });

    hooks.onPatchLoaded = [this]()
    {
        if (patchMutatorEngine_ != nullptr)
            patchMutatorEngine_->resetSessionForPatchLoad();
        notifyClipboardCrossPatchReadyFromPatchLoad();
    };

    hooks.disarmClipboardFeedback = [this]() { disarmClipboardFeedbackSession(); };
    hooks.armBankCopyFeedbackPending = [this]() { armBankCopyFeedbackPending(); };
    hooks.clearBankCopyFeedbackPending = [this]() { clearBankCopyFeedbackPending(); };
    hooks.armClipboardFeedback = [this]() { armClipboardFeedbackSession(); };
    hooks.refreshClipboardMirrors = [this]()
    {
        refreshClipboardPasteEnabledProperties();
        refreshClipboardFeedbackProperties();
    };

    hooks.confirmPatchContextChange = [this](bool includeUnsavedEditWarning) {
        return confirmPatchContextChangeGate(includeUnsavedEditWarning);
    };

    hooks.requestNameRequiredBeforeStore = [this]()
    {
        if (nameRequiredBeforeStoreRequest_)
            nameRequiredBeforeStoreRequest_();
    };
}

void PluginProcessor::createModuleActionHandler(const Core::ActionExecutionHooks& hooks)
{
    moduleActionHandler_ = std::make_unique<Core::ModuleActionHandler>(
        Core::ModuleActionHandler::Dependencies {
            apvts,
            patchModel_.get(),
            apvtsPatchMapper_.get(),
            clipboardService_.get(),
            matrixModInitService_.get(),
            masterModuleInitService_.get(),
            patchModuleInitService_.get(),
            patchParameterSysExDispatcher_.get(),
            matrixModBusParameterSysExDispatcher_.get(),
            [this]()
            {
                refreshClipboardPasteEnabledProperties();
                armClipboardFeedbackSession();
            }
        },
        hooks);
}

void PluginProcessor::createPatchManagerActionHandler(const Core::ActionExecutionHooks& hooks)
{
    patchManagerActionHandler_ = std::make_unique<Core::PatchManagerActionHandler>(
        Core::PatchManagerActionHandler::Dependencies {
            apvts,
            [this]() { return getResolvedDeviceMemoryLimits(); },
            patchModel_.get(),
            apvtsPatchMapper_.get(),
            clipboardService_.get(),
            patchInitService_.get(),
            patchSelectionMidiSync_.get(),
            midiManager.get(),
            patchFileService_.get(),
            patchNameSyncer_.get(),
            dirtyPatchTracker_.get(),
            &midiManager->getSysExEncoder(),
            [this]() -> juce::File
            {
                if (patchFolderPicker_)
                    return patchFolderPicker_();
                return {};
            },
            [this](juce::File suggestedFolder, juce::String suggestedStem) -> juce::File
            {
                if (patchSaveFilePicker_)
                    return patchSaveFilePicker_(suggestedFolder, suggestedStem);
                return {};
            },
            [this](juce::String internalSanitized, juce::String fileSanitized)
                -> std::optional<Core::NameReconciliationChoice>
            {
                if (patchNameReconciliationPicker_)
                    return patchNameReconciliationPicker_(internalSanitized, fileSanitized);
                return std::nullopt;
            } },
        hooks);
}

void PluginProcessor::createMutatorActionHandler()
{
    Core::MutatorActionHandler::Dependencies dependencies {
        apvts,
        patchMutatorEngine_.get(),
        [this]() -> juce::File
        {
            if (mutatorExportFolderPicker_)
                return mutatorExportFolderPicker_();
            return {};
        },
        [this](std::function<void()> onConfirmed)
        {
            if (mutatorDefragLimitModalGate_)
                mutatorDefragLimitModalGate_(std::move(onConfirmed));
        },
        [this](std::function<void(Core::ExportCollisionResolution)> onResolved)
        {
            if (mutatorExportCollisionModalGate_)
                mutatorExportCollisionModalGate_(std::move(onResolved));
        },
        [this]() -> bool
        {
            if (! mutatorFlushConfirmModalGate_)
                return true;
            return mutatorFlushConfirmModalGate_();
        },
        [this]() -> bool
        {
            if (! mutatorDeleteConfirmModalGate_)
                return true;
            return mutatorDeleteConfirmModalGate_();
        }
    };

    mutatorActionHandler_ = std::make_unique<Core::MutatorActionHandler>(std::move(dependencies));
}

void PluginProcessor::finishConstructionSetup()
{
    validatePluginDescriptorsAtStartup();
    buildChoiceParameterMap();
    buildPatchParameterIdSet();
    buildMasterParameterIdSet();
    buildMatrixModParameterIdSet();
    initializeMidiPortProperties();
    initializeAudioProperties();
    initializeHardwareLatencyProperty();
    initializeComputerPatchesFolderProperty();
    initializeComputerPatchesNamesPolicyProperty();
    initializeUnsavedStatePolicyProperty();
    initializeDeleteWarningPolicyProperty();
    initializeMatrix1000PatchesNamesModeProperty();

    initializePatchNameProperty();
    initializeClipboardPasteEnabledProperties();
    initializeClipboardFeedbackProperties();
    initializeMutatorRecipeState();
    resetEphemeralMutatorStateAfterSessionLoad();
    initializeMutatorActionEnabledMirrorsForEmptyHistory();
    apvts.state.addListener(this);
    deferredMidiPortSyncTimer_ = std::make_unique<DeferredMidiPortSyncTimer>(*this);
    installMidiDeviceListConnection();
    startMidiThread();
    refreshClipboardPasteEnabledProperties();
    refreshClipboardFeedbackProperties();
    resetInternalPatchCoordinatesToDefaults();

    // FR-51: establish a clean baseline from factory APVTS defaults so edits before the
    // first load/SAVE/STORE/INIT are still detected as dirty.
    apvtsPatchMapper_->apvtsToBuffer();
    patchNameSyncer_->apvtsToBuffer();
    dirtyPatchTracker_->captureSnapshot(*patchModel_);

    // Standalone may never call prepareToPlay when no audio device is selected in
    // Matrix-Control.settings — still start Debug file logging at construction.
    if (isStandaloneWrapper())
        ensureDevelopmentLoggingStarted();
}
