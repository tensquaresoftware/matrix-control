// Extracted from PluginProcessor.cpp for modular maintenance.
// Clipboard paste/feedback mirrors, Patch Name inline rename, and settings defaults.

#include "PluginProcessor.h"

#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/Audio/HardwareLatency.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/ClipboardFeedbackResolver.h"
#include "Core/Services/ClipboardPasteEnabledResolver.h"
#include "Core/Services/ClipboardService.h"
#include "Core/Services/PatchNameDisplayMode.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "MIDI/MidiManager.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

void PluginProcessor::initializeClipboardPasteEnabledProperties()
{
    namespace PatchEdit = PluginIDs::PatchEditSection;
    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
    namespace MatrixMod = PluginIDs::MatrixModulationSection::StandaloneWidgets;
    namespace BankUtility = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;

    const char* pasteEnabledIds[] = {
        PatchEdit::Dco1Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Dco2Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Envelope1Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Envelope2Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Envelope3Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Lfo1Module::StandaloneWidgets::kPasteEnabled,
        PatchEdit::Lfo2Module::StandaloneWidgets::kPasteEnabled,
        InternalPatches::kPastePatchEnabled,
        MatrixMod::kMatrixModulationPasteEnabled,
        BankUtility::kPasteBankEnabled
    };

    for (const auto* propertyId : pasteEnabledIds)
    {
        if (!apvts.state.hasProperty(propertyId))
            apvts.state.setProperty(propertyId, false, nullptr);
    }
}

int PluginProcessor::getSelectedBankForClipboardTargets() const
{
    namespace BankState = PluginIDs::PatchManagerSection::BankUtilityModule::StateProperties;

    const auto limits = getResolvedDeviceMemoryLimits();
    if (! limits.hasBankConcept())
        return -1;

    const int selected = static_cast<int>(apvts.state.getProperty(
        BankState::kSelectedBank, limits.minBankNumber()));
    return juce::jlimit(limits.minBankNumber(), limits.maxBankNumber(), selected);
}

void PluginProcessor::refreshClipboardPasteEnabledProperties()
{
    if (clipboardService_ == nullptr)
        return;

    namespace PatchEdit = PluginIDs::PatchEditSection;
    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;
    namespace MatrixMod = PluginIDs::MatrixModulationSection::StandaloneWidgets;
    namespace BankUtility = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;

    const int selectedBank = getSelectedBankForClipboardTargets();
    const auto limits = getResolvedDeviceMemoryLimits();
    const bool selectedBankPasteAllowed = selectedBank >= 0
        && limits.isPasteStoreAllowed(selectedBank);

    const auto state = Core::resolvePasteEnabled(Core::ClipboardPasteEnabledResolveArgs {
        *clipboardService_, selectedBank, selectedBankPasteAllowed
    });

    apvts.state.setProperty(PatchEdit::Dco1Module::StandaloneWidgets::kPasteEnabled, state.dco1, nullptr);
    apvts.state.setProperty(PatchEdit::Dco2Module::StandaloneWidgets::kPasteEnabled, state.dco2, nullptr);
    apvts.state.setProperty(PatchEdit::Envelope1Module::StandaloneWidgets::kPasteEnabled, state.env1, nullptr);
    apvts.state.setProperty(PatchEdit::Envelope2Module::StandaloneWidgets::kPasteEnabled, state.env2, nullptr);
    apvts.state.setProperty(PatchEdit::Envelope3Module::StandaloneWidgets::kPasteEnabled, state.env3, nullptr);
    apvts.state.setProperty(PatchEdit::Lfo1Module::StandaloneWidgets::kPasteEnabled, state.lfo1, nullptr);
    apvts.state.setProperty(PatchEdit::Lfo2Module::StandaloneWidgets::kPasteEnabled, state.lfo2, nullptr);
    apvts.state.setProperty(InternalPatches::kPastePatchEnabled, state.internalPatches, nullptr);
    apvts.state.setProperty(MatrixMod::kMatrixModulationPasteEnabled, state.matrixModulation, nullptr);
    apvts.state.setProperty(BankUtility::kPasteBankEnabled, state.bankUtility, nullptr);
}

void PluginProcessor::initializeClipboardFeedbackProperties()
{
    namespace Feedback = PluginIDs::ClipboardFeedback;

    const char* feedbackIds[] = {
        Feedback::kActive,
        Feedback::kCopyLit,
        Feedback::kDco1Copy,
        Feedback::kDco2Copy,
        Feedback::kEnv1Copy,
        Feedback::kEnv2Copy,
        Feedback::kEnv3Copy,
        Feedback::kLfo1Copy,
        Feedback::kLfo2Copy,
        Feedback::kMatrixModulationCopy,
        Feedback::kInternalPatchesCopy,
        Feedback::kBankUtilityCopy,
        Feedback::kDco1Paste,
        Feedback::kDco2Paste,
        Feedback::kEnv1Paste,
        Feedback::kEnv2Paste,
        Feedback::kEnv3Paste,
        Feedback::kLfo1Paste,
        Feedback::kLfo2Paste,
        Feedback::kMatrixModulationPaste,
        Feedback::kInternalPatchesPaste,
        Feedback::kBankUtilityPaste
    };

    for (const auto* propertyId : feedbackIds)
    {
        if (! apvts.state.hasProperty(propertyId))
        {
            const bool defaultValue = juce::String(propertyId) == Feedback::kCopyLit;
            apvts.state.setProperty(propertyId, defaultValue, nullptr);
        }
    }
}

void PluginProcessor::refreshClipboardFeedbackProperties()
{
    if (clipboardService_ == nullptr)
        return;

    namespace Feedback = PluginIDs::ClipboardFeedback;

    const int selectedBank = getSelectedBankForClipboardTargets();
    const auto limits = getResolvedDeviceMemoryLimits();
    const bool selectedBankPasteAllowed = selectedBank >= 0
        && limits.isPasteStoreAllowed(selectedBank);

    const auto state = Core::resolveClipboardFeedback(Core::ClipboardFeedbackResolveArgs {
        *clipboardService_,
        clipboardFeedbackActive_,
        clipboardFeedbackCrossPatchReady_,
        clipboardFeedbackBankCopyPending_,
        selectedBank,
        selectedBankPasteAllowed
    });

    apvts.state.setProperty(Feedback::kActive, state.active, nullptr);
    apvts.state.setProperty(Feedback::kDco1Copy, state.dco1Copy, nullptr);
    apvts.state.setProperty(Feedback::kDco2Copy, state.dco2Copy, nullptr);
    apvts.state.setProperty(Feedback::kEnv1Copy, state.env1Copy, nullptr);
    apvts.state.setProperty(Feedback::kEnv2Copy, state.env2Copy, nullptr);
    apvts.state.setProperty(Feedback::kEnv3Copy, state.env3Copy, nullptr);
    apvts.state.setProperty(Feedback::kLfo1Copy, state.lfo1Copy, nullptr);
    apvts.state.setProperty(Feedback::kLfo2Copy, state.lfo2Copy, nullptr);
    apvts.state.setProperty(Feedback::kMatrixModulationCopy, state.matrixModulationCopy, nullptr);
    apvts.state.setProperty(Feedback::kInternalPatchesCopy, state.internalPatchesCopy, nullptr);
    apvts.state.setProperty(Feedback::kBankUtilityCopy, state.bankUtilityCopy, nullptr);
    apvts.state.setProperty(Feedback::kDco1Paste, state.dco1Paste, nullptr);
    apvts.state.setProperty(Feedback::kDco2Paste, state.dco2Paste, nullptr);
    apvts.state.setProperty(Feedback::kEnv1Paste, state.env1Paste, nullptr);
    apvts.state.setProperty(Feedback::kEnv2Paste, state.env2Paste, nullptr);
    apvts.state.setProperty(Feedback::kEnv3Paste, state.env3Paste, nullptr);
    apvts.state.setProperty(Feedback::kLfo1Paste, state.lfo1Paste, nullptr);
    apvts.state.setProperty(Feedback::kLfo2Paste, state.lfo2Paste, nullptr);
    apvts.state.setProperty(Feedback::kMatrixModulationPaste, state.matrixModulationPaste, nullptr);
    apvts.state.setProperty(Feedback::kInternalPatchesPaste, state.internalPatchesPaste, nullptr);
    apvts.state.setProperty(Feedback::kBankUtilityPaste, state.bankUtilityPaste, nullptr);
}

void PluginProcessor::armClipboardFeedbackSession()
{
    clipboardFeedbackActive_ = true;
    clipboardFeedbackCrossPatchReady_ = false;
    clipboardFeedbackBankCopyPending_ = false;
    clipboardFeedbackOriginContext_ = patchLoadContext_;
    apvts.state.setProperty(PluginIDs::ClipboardFeedback::kCopyLit, true, nullptr);
    refreshClipboardFeedbackProperties();
}

void PluginProcessor::armBankCopyFeedbackPending()
{
    clipboardFeedbackActive_ = true;
    clipboardFeedbackCrossPatchReady_ = false;
    clipboardFeedbackBankCopyPending_ = true;
    clipboardFeedbackOriginContext_ = patchLoadContext_;
    apvts.state.setProperty(PluginIDs::ClipboardFeedback::kCopyLit, true, nullptr);
    refreshClipboardFeedbackProperties();
}

void PluginProcessor::clearBankCopyFeedbackPending()
{
    clipboardFeedbackBankCopyPending_ = false;
}

void PluginProcessor::disarmClipboardFeedbackSession()
{
    clipboardFeedbackActive_ = false;
    clipboardFeedbackCrossPatchReady_ = false;
    clipboardFeedbackBankCopyPending_ = false;
    apvts.state.setProperty(PluginIDs::ClipboardFeedback::kCopyLit, true, nullptr);
    refreshClipboardFeedbackProperties();
}

void PluginProcessor::notifyClipboardCrossPatchReadyFromPatchLoad()
{
    if (! clipboardFeedbackActive_ || clipboardService_ == nullptr)
        return;

    const auto mode = clipboardService_->getMode();
    if (mode != Core::ClipboardMode::FullPatch
        && mode != Core::ClipboardMode::MatrixModulation)
    {
        return;
    }

    // Spec: unlock Paste blink only after a *different* patch is loaded (not Init on same slot).
    const auto& current = patchLoadContext_;
    const auto& origin = clipboardFeedbackOriginContext_;
    const bool sameOrigin = (current.origin == origin.origin)
        && (current.origin == Core::PatchLoadContext::Origin::kComputerFile
                ? current.fileStem == origin.fileStem
                : (current.bank == origin.bank && current.patch == origin.patch));

    if (sameOrigin)
        return;

    clipboardFeedbackCrossPatchReady_ = true;
    refreshClipboardFeedbackProperties();
}

bool PluginProcessor::clearClipboardFeedbackFromEscape()
{
    const bool hadFeedback = clipboardFeedbackActive_;
    const bool hadContent = clipboardService_ != nullptr && clipboardService_->hasContent();
    if (! hadFeedback && ! hadContent)
        return false;

    if (clipboardService_ != nullptr)
        clipboardService_->clear();

    disarmClipboardFeedbackSession();
    refreshClipboardPasteEnabledProperties();

    apvts.state.setProperty(PluginIDs::ClipboardFeedback::kCopyEnvelopeShapeOnly, false, nullptr);
    apvts.state.setProperty("uiMessageText", juce::String(), nullptr);
    apvts.state.setProperty("uiMessageSeverity", juce::String(), nullptr);
    return true;
}

int PluginProcessor::getCurrentPatchNumberForMutator() const
{
    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

    return static_cast<int>(apvts.state.getProperty(InternalPatches::kCurrentPatchNumber, 0));
}

bool PluginProcessor::canEditPatchName() const
{
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    if (static_cast<bool>(apvts.state.getProperty(MutatorState::kCompareActive, false)))
        return false;

    if (patchLoadContext_.origin == Core::PatchLoadContext::Origin::kComputerFile)
        return true;

    namespace InternalPatches = PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets;

    const auto limits = getResolvedDeviceMemoryLimits();
    const int bank = static_cast<int>(
        apvts.state.getProperty(InternalPatches::kCurrentBankNumber, limits.minBankNumber()));

    // Spec gate is ROM vs writable — use isRomBank directly (not PASTE/STORE helpers).
    if (limits.isRomBank(bank))
        return false;

    // Hardware Names + device memory on banked Matrix-1000: names are not durable via SysEx.
    const int displayMode = Core::PatchNameDisplay::normalize(static_cast<int>(
        apvts.state.getProperty(PluginIDs::Settings::kMatrix1000PatchesNamesMode,
                               PluginIDs::Settings::Matrix1000PatchesNamesMode::kDefault)));
    if (Core::PatchNameDisplay::isHardwareNames(displayMode) && limits.hasBankConcept())
        return false;

    return true;
}

void PluginProcessor::refreshPatchNameDisplayForSettingsMode()
{
    notifyNonParameterStateChanged();

    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    if (static_cast<bool>(apvts.state.getProperty(MutatorState::kCompareActive, false)))
        return;

    if (patchManagerActionHandler_ == nullptr)
        return;

    patchManagerActionHandler_->reapplyDisplayedPatchName();
}

void PluginProcessor::refreshComputerPatchNameDisplayForSettingsPolicy()
{
    notifyNonParameterStateChanged();

    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    if (static_cast<bool>(apvts.state.getProperty(MutatorState::kCompareActive, false)))
        return;

    if (patchManagerActionHandler_ == nullptr)
        return;

    if (patchLoadContext_.origin != Core::PatchLoadContext::Origin::kComputerFile)
        return;

    patchManagerActionHandler_->reapplyComputerPatchDisplayedName();
}

void PluginProcessor::commitPatchNameRename(const juce::String& newName, bool suppressAuditionSysEx)
{
    if (patchModel_ == nullptr || patchNameSyncer_ == nullptr)
        return;

    if ((! canEditPatchName() && ! suppressAuditionSysEx) || newName.trim().isEmpty())
        return;

    patchModel_->setName(newName);
    patchNameSyncer_->bufferToApvts();
    rememberOverlayNameAfterRenameIfNeeded();

    if (patchMutatorEngine_ != nullptr)
        patchMutatorEngine_->refreshFrozenExportBasename(patchModel_->getName());

    if (! suppressAuditionSysEx)
        sendPatchNameRenameAudition();
}

void PluginProcessor::rememberOverlayNameAfterRenameIfNeeded()
{
    if (patchLoadContext_.origin != Core::PatchLoadContext::Origin::kDeviceMemory)
        return;

    if (patchManagerActionHandler_ == nullptr || ! getResolvedDeviceMemoryLimits().hasBankConcept())
        return;

    patchManagerActionHandler_->rememberCurrentOverlayFromModel();
}

void PluginProcessor::sendPatchNameRenameAudition()
{
    if (midiManager == nullptr || patchModel_ == nullptr)
        return;

    const auto limits = getResolvedDeviceMemoryLimits();
    const juce::uint8 patchNumber = static_cast<juce::uint8>(
        juce::jlimit(0, 255, getCurrentPatchNumberForMutator()));

    midiManager->sendFullPatchForAudition(patchModel_->data(), patchNumber, limits.hasBankConcept());
}

void PluginProcessor::executeInternalPatchStore()
{
    if (patchManagerActionHandler_ == nullptr)
        return;

    patchManagerActionHandler_->handleAction(
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
        juce::var());
}

void PluginProcessor::initializePatchNameProperty()
{
    using namespace PluginIDs::PatchEditSection::PatchNameModule;
    using namespace PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets;

    if (!apvts.state.hasProperty(kPatchName))
        apvts.state.setProperty(kPatchName, juce::String(kDefaultPatchName), nullptr);
}

void PluginProcessor::initializeHardwareLatencyProperty()
{
    if (!apvts.state.hasProperty(PluginIDs::Settings::kHardwareLatencyMs))
        apvts.state.setProperty(PluginIDs::Settings::kHardwareLatencyMs, Core::HardwareLatency::kMinMs, nullptr);

    syncHardwareLatencyFromState();
}

void PluginProcessor::initializeComputerPatchesFolderProperty()
{
    if (!apvts.state.hasProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath))
    {
        apvts.state.setProperty(
            PluginIDs::PatchManagerSection::ComputerPatchesModule::StateProperties::kFolderPath,
            juce::String(),
            nullptr);
    }
}

void PluginProcessor::initializeComputerPatchesNamesPolicyProperty()
{
    if (! apvts.state.hasProperty(PluginIDs::Settings::kComputerPatchesNamesPolicy))
    {
        apvts.state.setProperty(
            PluginIDs::Settings::kComputerPatchesNamesPolicy,
            PluginIDs::Settings::ComputerPatchesNamesPolicy::kDefault,
            nullptr);
    }
}

void PluginProcessor::initializeUnsavedStatePolicyProperty()
{
    if (! apvts.state.hasProperty(PluginIDs::Settings::kUnsavedStatePolicy))
    {
        apvts.state.setProperty(
            PluginIDs::Settings::kUnsavedStatePolicy,
            PluginIDs::Settings::UnsavedStatePolicy::kDefault,
            nullptr);
    }
}

void PluginProcessor::initializeDeleteWarningPolicyProperty()
{
    if (! apvts.state.hasProperty(PluginIDs::Settings::kDeleteWarningPolicy))
    {
        apvts.state.setProperty(
            PluginIDs::Settings::kDeleteWarningPolicy,
            PluginIDs::Settings::DeleteWarningPolicy::kDefault,
            nullptr);
    }
}

void PluginProcessor::initializeMatrix1000PatchesNamesModeProperty()
{
    if (! apvts.state.hasProperty(PluginIDs::Settings::kMatrix1000PatchesNamesMode))
    {
        apvts.state.setProperty(
            PluginIDs::Settings::kMatrix1000PatchesNamesMode,
            PluginIDs::Settings::Matrix1000PatchesNamesMode::kDefault,
            nullptr);
    }
}
