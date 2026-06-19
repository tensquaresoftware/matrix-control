#include "Core/Services/PatchMutator/PatchMutatorEngine.h"

#include <cstring>

#include "Core/MIDI/MidiManager.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

    constexpr const char* kNoOpRecipeFooterMessage = "Set Amount and Random above 0 to mutate.";
    constexpr const char* kNoMutationChangeFooterMessage =
        "Mutation made no changes. Try higher Amount or Random.";
    constexpr const char* kHistoryLimitFooterMessage = "Mutation history is full. Defrag to continue.";
    constexpr const char* kEmptyHistoryFooterMessage = "Mutation history is empty.";
    constexpr const char* kNoSelectionFooterMessage = "No valid mutation history entry selected.";
    constexpr const char* kFooterSeverityWarning = "warning";

    void setPatchLoadSuppressHooks(Core::ActionExecutionHooks& hooks, bool suppress)
    {
        if (hooks.setSuppressPatchSysEx)
            hooks.setSuppressPatchSysEx(suppress);

        if (hooks.setSuppressMatrixModSysEx)
            hooks.setSuppressMatrixModSysEx(suppress);
    }

    void flushDeferredApvtsParameterSync(juce::AudioProcessorValueTreeState& apvts)
    {
        (void) apvts.copyState();
    }

    int readPatchMutatorPercent(juce::AudioProcessorValueTreeState& apvts,
                                const juce::Identifier& propertyId,
                                int defaultValue)
    {
        auto* rawValue = apvts.getRawParameterValue(propertyId);
        if (rawValue != nullptr)
            return juce::jlimit(0, 100, juce::roundToInt(rawValue->load()));

        return juce::jlimit(0, 100, static_cast<int>(apvts.state.getProperty(propertyId, defaultValue)));
    }

    void pushPatchModelToApvtsWithSuppress(juce::AudioProcessorValueTreeState& apvts,
                                           Core::ActionExecutionHooks& hooks,
                                           Core::ApvtsPatchMapper& mapper,
                                           Core::PatchNameSyncer* patchNameSyncer)
    {
        setPatchLoadSuppressHooks(hooks, true);
        mapper.bufferToApvts();

        if (patchNameSyncer != nullptr)
            patchNameSyncer->bufferToApvts();

        flushDeferredApvtsParameterSync(apvts);
        setPatchLoadSuppressHooks(hooks, false);
    }
} // namespace

namespace Core
{

PatchMutatorEngine::PatchMutatorEngine(PatchModel* patchModel,
                                       ApvtsPatchMapper* apvtsPatchMapper,
                                       PatchNameSyncer* patchNameSyncer,
                                       MidiManager* midiManager,
                                       juce::AudioProcessorValueTreeState& apvts,
                                       ActionExecutionHooks hooks,
                                       std::function<int()> getCurrentPatchNumber)
    : patchModel_(patchModel)
    , apvtsPatchMapper_(apvtsPatchMapper)
    , patchNameSyncer_(patchNameSyncer)
    , midiManager_(midiManager)
    , apvts_(apvts)
    , hooks_(std::move(hooks))
    , getCurrentPatchNumber_(std::move(getCurrentPatchNumber))
{
    jassert(patchModel_ != nullptr);
    jassert(apvtsPatchMapper_ != nullptr);
}

MutatorActionResult PatchMutatorEngine::mutate()
{
    if (historyStore_.isRootSlotsFull() || historyStore_.isRootIndexExhausted())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const auto recipe = buildRecipeFromApvts();
    if (recipe.amountPercent == 0 || recipe.randomPercent == 0)
    {
        MutatorActionResult result;
        result.footerMessage = kNoOpRecipeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const PatchModel auditionBuffer = resolveAuditionBuffer();
    PatchModel parentSnapshot;
    parentSnapshot.loadFrom(auditionBuffer.data());

    PatchModel working;
    working.loadFrom(auditionBuffer.data());

    rng_.setSeedRandomly();
    JuceRandomSource rngSource(rng_);
    const bool mutated = algorithm_.apply(working, recipe, rngSource);
    if (! mutated)
    {
        MutatorActionResult result;
        result.footerMessage = kNoMutationChangeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto rootIndexOpt = historyStore_.peekNextRootIndex();
    if (! rootIndexOpt.has_value())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const int rootIndex = *rootIndexOpt;
    MutationNaming::applyPatchName(working, rootIndex);

    if (! historyStore_.insertRoot(rootIndex, working, parentSnapshot))
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    pushResultToEditorAndSynth(working);

    MutatorActionResult result;
    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::retry()
{
    if (historyStore_.isEmpty())
    {
        MutatorActionResult result;
        result.footerMessage = kEmptyHistoryFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto rootIndexOpt = resolveSelectedRootIndex();
    if (! rootIndexOpt.has_value() || ! historyStore_.hasRoot(*rootIndexOpt))
    {
        MutatorActionResult result;
        result.footerMessage = kNoSelectionFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const int rootIndex = *rootIndexOpt;

    if (historyStore_.isRetrySlotsFull(rootIndex) || historyStore_.isRetryIndexExhausted(rootIndex))
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const auto selectedEntry = resolveSelectedEntryForRetry(rootIndex);
    if (! selectedEntry.has_value())
    {
        MutatorActionResult result;
        result.footerMessage = kNoSelectionFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto recipe = buildRecipeFromApvts();
    if (recipe.amountPercent == 0 || recipe.randomPercent == 0)
    {
        MutatorActionResult result;
        result.footerMessage = kNoOpRecipeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    PatchModel parentSnapshot;
    parentSnapshot.loadFrom(selectedEntry->parentSnapshot.data());

    PatchModel working;
    working.loadFrom(selectedEntry->parentSnapshot.data());

    rng_.setSeedRandomly();
    JuceRandomSource rngSource(rng_);
    const bool mutated = algorithm_.apply(working, recipe, rngSource);
    if (! mutated)
    {
        MutatorActionResult result;
        result.footerMessage = kNoMutationChangeFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        return result;
    }

    const auto retryIndexOpt = historyStore_.peekNextRetryIndex(rootIndex);
    if (! retryIndexOpt.has_value())
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    const int retryIndex = *retryIndexOpt;
    MutationNaming::applyPatchName(working, rootIndex, retryIndex);

    if (! historyStore_.insertRetry(rootIndex, retryIndex, working, parentSnapshot))
    {
        MutatorActionResult result;
        result.footerMessage = kHistoryLimitFooterMessage;
        result.footerSeverity = kFooterSeverityWarning;
        result.defragModalRequested = true;
        return result;
    }

    pushResultToEditorAndSynth(working);

    MutatorActionResult result;
    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::toggleCompare()
{
    MutatorActionResult result;
    return result;
}

MutatorActionResult PatchMutatorEngine::deleteSelected()
{
    MutatorActionResult result;
    return result;
}

MutatorActionResult PatchMutatorEngine::clearHistory()
{
    MutatorActionResult result;
    return result;
}

MutatorActionResult PatchMutatorEngine::exportHistory(const juce::File&)
{
    MutatorActionResult result;
    return result;
}

MutatorActionResult PatchMutatorEngine::defragHistory()
{
    MutatorActionResult result;
    return result;
}

void PatchMutatorEngine::auditionSelectedHistoryEntry()
{
}

void PatchMutatorEngine::syncHistoryUiProperties(juce::AudioProcessorValueTreeState&)
{
}

void PatchMutatorEngine::refreshActionEnabledMirrors(juce::AudioProcessorValueTreeState&)
{
}

void PatchMutatorEngine::setAuditionSelection(int rootIndex, int retryIndex)
{
    selectedRootIndex_ = rootIndex;
    selectedRetryIndex_ = retryIndex;
}

int PatchMutatorEngine::rootCount() const noexcept
{
    return historyStore_.rootCount();
}

std::optional<MutationEntry> PatchMutatorEngine::getEntry(int rootIndex, int retryIndex) const
{
    return historyStore_.getEntry(rootIndex, retryIndex);
}

MutationRecipe PatchMutatorEngine::buildRecipeFromApvts() const
{
    const auto& state = apvts_.state;

    MutationRecipe recipe;
    recipe.amountPercent = readPatchMutatorPercent(apvts_, PatchMutator::kAmount, 0);
    recipe.randomPercent = readPatchMutatorPercent(apvts_, PatchMutator::kRandom, 0);
    recipe.enableDco1 = readBoolProperty(state, PatchMutator::kEnableDco1, false);
    recipe.enableDco2 = readBoolProperty(state, PatchMutator::kEnableDco2, false);
    recipe.enableVcfVca = readBoolProperty(state, PatchMutator::kEnableVcfVca, false);
    recipe.enableFmTrack = readBoolProperty(state, PatchMutator::kEnableFmTrack, false);
    recipe.enableRampPortamento = readBoolProperty(state, PatchMutator::kEnableRampPortamento, false);
    recipe.enableEnvelope1 = readBoolProperty(state, PatchMutator::kEnableEnvelope1, false);
    recipe.enableEnvelope2 = readBoolProperty(state, PatchMutator::kEnableEnvelope2, false);
    recipe.enableEnvelope3 = readBoolProperty(state, PatchMutator::kEnableEnvelope3, false);
    recipe.enableLfo1 = readBoolProperty(state, PatchMutator::kEnableLfo1, false);
    recipe.enableLfo2 = readBoolProperty(state, PatchMutator::kEnableLfo2, false);
    return recipe;
}

PatchModel PatchMutatorEngine::resolveAuditionBuffer() const
{
    if (historyStore_.isEmpty())
        return *patchModel_;

    int rootIndex = selectedRootIndex_;
    int retryIndex = selectedRetryIndex_;

    if (rootIndex < 0)
    {
        const auto sortedRoots = historyStore_.getSortedRootIndices();
        if (sortedRoots.isEmpty())
            return *patchModel_;

        rootIndex = sortedRoots.getLast();
        retryIndex = MutationHistoryStore::kRootOnly;
    }

    if (const auto entry = historyStore_.getEntry(rootIndex, retryIndex))
    {
        PatchModel model;
        model.loadFrom(entry->result.data());
        return model;
    }

    if (const auto rootEntry = historyStore_.getEntry(rootIndex, MutationHistoryStore::kRootOnly))
    {
        PatchModel model;
        model.loadFrom(rootEntry->result.data());
        return model;
    }

    return *patchModel_;
}

std::optional<int> PatchMutatorEngine::resolveSelectedRootIndex() const
{
    if (historyStore_.isEmpty())
        return std::nullopt;

    if (selectedRootIndex_ >= 0)
        return selectedRootIndex_;

    const auto sortedRoots = historyStore_.getSortedRootIndices();
    if (sortedRoots.isEmpty())
        return std::nullopt;

    return sortedRoots.getLast();
}

std::optional<MutationEntry> PatchMutatorEngine::resolveSelectedEntryForRetry(int rootIndex) const
{
    if (selectedRetryIndex_ == MutationHistoryStore::kRootOnly)
        return historyStore_.getEntry(rootIndex, MutationHistoryStore::kRootOnly);

    if (const auto retryEntry = historyStore_.getEntry(rootIndex, selectedRetryIndex_))
        return retryEntry;

    return historyStore_.getEntry(rootIndex, MutationHistoryStore::kRootOnly);
}

void PatchMutatorEngine::pushResultToEditorAndSynth(const PatchModel& mutatedModel)
{
    std::memcpy(patchModel_->data(), mutatedModel.data(), PatchModel::kBufferSize);
    pushPatchModelToApvtsWithSuppress(apvts_, hooks_, *apvtsPatchMapper_, patchNameSyncer_);

    if (midiManager_ != nullptr && getCurrentPatchNumber_)
    {
        const int patchNumber = getCurrentPatchNumber_();
        midiManager_->sendPatch(static_cast<juce::uint8>(juce::jlimit(0, 255, patchNumber)),
                               patchModel_->data());
    }
}

bool PatchMutatorEngine::readBoolProperty(const juce::ValueTree& state,
                                          const juce::Identifier& propertyId,
                                          bool defaultValue)
{
    if (! state.hasProperty(propertyId))
        return defaultValue;

    return static_cast<bool>(state.getProperty(propertyId, defaultValue));
}

} // namespace Core
