// Extracted from PluginProcessor.cpp for modular maintenance.
// Host session state (save / restore), GUI scale + skin, and Mutator ephemeral state.

#include "PluginProcessor.h"
#include "PluginProcessorInternal.h"

#include "Core/Actions/PatchManagerActionHandler.h"
#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/PatchMutator/MutatorSessionPersistence.h"
#include "Core/Services/PatchMutator/PatchMutatorEngine.h"
#include "Core/Services/SessionPersistencePolicy.h"
#include "Loggers/ApvtsLogger.h"
#include "Shared/Definitions/PluginIDs.h"

using namespace PluginProcessorInternal;

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    stripEphemeralMutatorStateForPersistence(state);

    namespace Feedback = PluginIDs::ClipboardFeedback;
    const char* feedbackIds[] = {
        Feedback::kActive,
        Feedback::kCopyLit,
        Feedback::kCopyEnvelopeShapeOnly,
        Feedback::kDco1Copy,
        Feedback::kDco2Copy,
        Feedback::kEnv1Copy,
        Feedback::kEnv2Copy,
        Feedback::kEnv3Copy,
        Feedback::kLfo1Copy,
        Feedback::kLfo2Copy,
        Feedback::kMatrixModulationCopy,
        Feedback::kInternalPatchesCopy,
        Feedback::kDco1Paste,
        Feedback::kDco2Paste,
        Feedback::kEnv1Paste,
        Feedback::kEnv2Paste,
        Feedback::kEnv3Paste,
        Feedback::kLfo1Paste,
        Feedback::kLfo2Paste,
        Feedback::kMatrixModulationPaste,
        Feedback::kInternalPatchesPaste
    };
    for (const auto* id : feedbackIds)
        state.removeProperty(id, nullptr);

    // Live MIDI health — never restore a wedged session lock from project state.
    state.removeProperty(Core::kDeviceMidiUnresponsiveProperty, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState == nullptr)
        return;

    if (! xmlState->hasTagName(apvts.state.getType()))
        return;

    applyRestoredPluginState(juce::ValueTree::fromXml(*xmlState));
}

void PluginProcessor::applyRestoredPluginState(juce::ValueTree restoredState)
{
    if (Core::SessionPersistencePolicy::shouldStripPatchAndMasterParameters(restoredState))
        Core::SessionPersistencePolicy::stripPatchAndMasterParameters(restoredState);

    // Clear ephemeral mutator state + patch name before replaceState so
    // valueTreeRedirected / PatchNameSyncer never briefly rehydrate a stale name.
    Core::MutatorSessionPersistence::resetEphemeralStateAfterSessionLoad(restoredState);
    restoredState.setProperty(Core::kDeviceMidiUnresponsiveProperty, false, nullptr);

    // Drop in-memory Computer Patches scan before replaceState so redirected panel
    // refresh cannot briefly show the previous session's file list.
    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->discardComputerPatchesScanCacheQuietly();

    apvts.replaceState(restoredState);
    initializeMutatorRecipeState();
    resetEphemeralMutatorStateAfterSessionLoad();
    initializeMutatorActionEnabledMirrorsForEmptyHistory();
    syncAudioRuntimeFromState();
    syncHardwareLatencyFromState();
    // Option 2 (V1.2 review): standalone has no deferred retry series — align to open
    // reality immediately. Plugin host keeps a soft first sync so the desired id survives
    // intermediate retries; the last deferred attempt reports failures and aligns.
    syncMidiPortsFromState(isStandaloneWrapper());
    scheduleDeferredMidiPortSyncForPluginHost();

    if (patchManagerActionHandler_ != nullptr)
        patchManagerActionHandler_->applyComputerPatchesBrowserAfterSessionLoad();

    resetInternalPatchCoordinatesToDefaults();

    // Re-baseline after host restore so subsequent edits are measured against the
    // restored (or stripped-then-default) PATCH buffer, not a pre-session snapshot.
    if (dirtyPatchTracker_ != nullptr && apvtsPatchMapper_ != nullptr && patchNameSyncer_ != nullptr
        && patchModel_ != nullptr)
    {
        apvtsPatchMapper_->apvtsToBuffer();
        patchNameSyncer_->apvtsToBuffer();
        dirtyPatchTracker_->captureSnapshot(*patchModel_);
    }

    if (shouldUseDevelopmentLogging())
        ApvtsLogger::getInstance().logStateLoaded("DAW state");
}

int PluginProcessor::getGuiScaleId() const
{
    return static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kGuiScale,
        PluginIDs::Settings::ScaleLevels::kDefault));
}

void PluginProcessor::setGuiScaleId(int scaleId)
{
    apvts.state.setProperty(PluginIDs::Settings::kGuiScale, scaleId, nullptr);
    notifyNonParameterStateChanged();
}

int PluginProcessor::getSkinVariantId() const
{
    return static_cast<int>(apvts.state.getProperty(
        PluginIDs::Settings::kSkinVariant,
        PluginIDs::Settings::SkinVariants::kDefault));
}

void PluginProcessor::setSkinVariantId(int skinVariantId)
{
    apvts.state.setProperty(PluginIDs::Settings::kSkinVariant, skinVariantId, nullptr);
    notifyNonParameterStateChanged();
}

void PluginProcessor::notifyNonParameterStateChanged()
{
    updateHostDisplay(juce::AudioProcessorListener::ChangeDetails().withNonParameterStateChanged(true));
}

void PluginProcessor::initializeMutatorRecipeState()
{
    Core::MutatorSessionPersistence::initializeRecipeState(apvts.state);
}

void PluginProcessor::stripEphemeralMutatorStateForPersistence(juce::ValueTree& state)
{
    Core::MutatorSessionPersistence::stripEphemeralStateForPersistence(state);
}

void PluginProcessor::resetEphemeralMutatorStateAfterSessionLoad()
{
    Core::MutatorSessionPersistence::resetEphemeralStateAfterSessionLoad(apvts.state);

    if (patchNameSyncer_ != nullptr)
        patchNameSyncer_->apvtsToBuffer();

    if (patchMutatorEngine_ != nullptr)
        patchMutatorEngine_->resetSessionForPatchLoad();
}

void PluginProcessor::initializeMutatorActionEnabledMirrorsForEmptyHistory()
{
    if (patchMutatorEngine_ != nullptr)
        patchMutatorEngine_->refreshActionEnabledMirrors(apvts);
    else
        Core::MutatorSessionPersistence::setActionEnabledMirrorsForEmptyHistory(apvts.state);
}
