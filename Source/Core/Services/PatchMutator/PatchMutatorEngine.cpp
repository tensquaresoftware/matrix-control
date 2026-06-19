#include "Core/Services/PatchMutator/PatchMutatorEngine.h"

#include <cstring>

#include "Core/MIDI/MidiManager.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/PatchMutator/MutationNaming.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

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

    // Pipe-separated root/retry labels — e.g. "M00|M02|M05", "—|R00|R02".
    juce::String joinLabels(const juce::StringArray& labels)
    {
        return labels.joinIntoString("|");
    }

    juce::String historyRootSentinelLabel()
    {
        return PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryRootSentinel;
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
    applySelectionFromApvts();

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

    selectedRootIndex_ = rootIndex;
    selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    syncHistoryUiProperties(apvts_);

    MutatorActionResult result;
    result.success = true;
    return result;
}

MutatorActionResult PatchMutatorEngine::retry()
{
    applySelectionFromApvts();

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

    selectedRetryIndex_ = retryIndex;
    syncHistoryUiProperties(apvts_);

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
    if (readBoolProperty(apvts_.state, MutatorState::kCompareActive, false))
        return;

    applySelectionFromApvts();

    if (historyStore_.isEmpty() || selectedRootIndex_ < 0)
        return;

    const PatchModel auditionModel = resolveAuditionBuffer();

    if (std::memcmp(auditionModel.data(), patchModel_->data(), PatchModel::kBufferSize) == 0)
        return;

    pushResultToEditorAndSynth(auditionModel);
}

void PatchMutatorEngine::syncHistoryUiProperties(juce::AudioProcessorValueTreeState& apvts)
{
    auto& state = apvts.state;

    if (! state.hasProperty(MutatorState::kCompareActive))
        state.setProperty(MutatorState::kCompareActive, false, nullptr);

    // Story 7.4: call syncHistoryUiProperties when kSelectedM changes so kHistoryRList
    // rebuilds for the new root (panel writes properties only — AD-5).
    if (state.hasProperty(MutatorState::kSelectedM))
    {
        const int apvtsM = static_cast<int>(state.getProperty(MutatorState::kSelectedM, -1));
        if (apvtsM != selectedRootIndex_)
            applySelectionFromApvts();
    }

    const auto roots = historyStore_.getSortedRootIndices();
    if (roots.isEmpty())
    {
        state.setProperty(MutatorState::kHistoryMList, juce::String(), nullptr);
        state.setProperty(MutatorState::kHistoryRList, juce::String(), nullptr);
        state.setProperty(MutatorState::kSelectedM, -1, nullptr);
        state.setProperty(MutatorState::kSelectedR, MutationHistoryStore::kRootOnly, nullptr);
        selectedRootIndex_ = -1;
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
        return;
    }

    juce::StringArray mLabels;
    for (const int rootIndex : roots)
        mLabels.add(MutationNaming::formatRootLabel(rootIndex));
    state.setProperty(MutatorState::kHistoryMList, joinLabels(mLabels), nullptr);

    int m = selectedRootIndex_;
    if (m < 0 || ! historyStore_.hasRoot(m))
        m = roots.getLast();

    selectedRootIndex_ = m;

    const auto retries = historyStore_.getSortedRetryIndices(m);
    if (selectedRetryIndex_ != MutationHistoryStore::kRootOnly
        && ! historyStore_.hasRetry(m, selectedRetryIndex_))
    {
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    }

    juce::StringArray rLabels;
    rLabels.add(historyRootSentinelLabel());
    for (const int retryIndex : retries)
        rLabels.add(MutationNaming::formatRetryLabel(retryIndex));
    state.setProperty(MutatorState::kHistoryRList, joinLabels(rLabels), nullptr);
    state.setProperty(MutatorState::kSelectedM, m, nullptr);
    state.setProperty(MutatorState::kSelectedR, selectedRetryIndex_, nullptr);
}

void PatchMutatorEngine::applySelectionFromApvts()
{
    const auto& state = apvts_.state;
    if (! state.hasProperty(MutatorState::kSelectedM))
        return;

    const int m = static_cast<int>(state.getProperty(MutatorState::kSelectedM, -1));
    const int r = static_cast<int>(state.getProperty(MutatorState::kSelectedR,
                                                     MutationHistoryStore::kRootOnly));

    if (m < 0 || ! historyStore_.hasRoot(m))
    {
        selectedRootIndex_ = -1;
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
        return;
    }

    selectedRootIndex_ = m;

    if (r == MutationHistoryStore::kRootOnly)
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
    else if (historyStore_.hasRetry(m, r))
        selectedRetryIndex_ = r;
    else
        selectedRetryIndex_ = MutationHistoryStore::kRootOnly;
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

PatchModel PatchMutatorEngine::resolveAuditionBuffer()
{
    applySelectionFromApvts();

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
