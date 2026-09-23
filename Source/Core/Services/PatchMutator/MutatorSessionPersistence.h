#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "Core/Services/PatchMutator/MutationPolicy.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core::MutatorSessionPersistence
{
    namespace detail
    {
        inline constexpr const char* kRecipeModuleToggleIds[] = {
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2,
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableMatrixMod
        };
    } // namespace detail

    inline bool isRecipeModuleEnableProperty(const juce::String& propertyId)
    {
        for (const auto* toggleId : detail::kRecipeModuleToggleIds)
        {
            if (propertyId == toggleId)
                return true;
        }

        return false;
    }

    inline bool anyRecipeModuleToggleEnabled(const juce::ValueTree& state)
    {
        for (const auto* toggleId : detail::kRecipeModuleToggleIds)
        {
            if (static_cast<bool>(state.getProperty(toggleId, false)))
                return true;
        }

        return false;
    }

    // MODE and PITCH are the recipe surface the user actually sees, so they are always
    // present and always inside the range the combo boxes offer.
    inline void initializeModeAndPitchState(juce::ValueTree& state)
    {
        namespace Mutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

        const auto clampStoredIndex = [&state](const char* propertyId, int lowest, int highest, int defaultValue)
        {
            const int stored = state.hasProperty(propertyId)
                                   ? static_cast<int>(state.getProperty(propertyId))
                                   : defaultValue;
            state.setProperty(propertyId, juce::jlimit(lowest, highest, stored), nullptr);
        };

        clampStoredIndex(Mutator::kMode,
                         0,
                         Core::kMutationModeCount - 1,
                         static_cast<int>(Core::MutationMode::kDrift));
        clampStoredIndex(Mutator::kPitch,
                         0,
                         Core::kMutationPitchModeCount - 1,
                         static_cast<int>(Core::MutationPitchMode::kPreserve));
        clampStoredIndex(Mutator::kPitchOctaves,
                         Core::MutationCalibration::kMinPitchOctaves,
                         Core::MutationCalibration::kMaxPitchOctaves,
                         Core::MutationCalibration::kDefaultPitchOctaves);
    }

    inline void initializeRecipeState(juce::ValueTree& state)
    {
        namespace Mutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

        constexpr int kRecipePercentMin = 1;
        constexpr int kRecipePercentMax = 100;
        constexpr int kDefaultAmountPercent = 50;
        constexpr int kDefaultRandomPercent = 25;

        const auto clampRecipePercent = [](int value) noexcept
        {
            return juce::jlimit(kRecipePercentMin, kRecipePercentMax, value);
        };

        if (! state.hasProperty(Mutator::kAmount))
            state.setProperty(Mutator::kAmount, kDefaultAmountPercent, nullptr);
        else
            state.setProperty(Mutator::kAmount,
                              clampRecipePercent(static_cast<int>(state.getProperty(Mutator::kAmount))),
                              nullptr);

        if (! state.hasProperty(Mutator::kRandom))
            state.setProperty(Mutator::kRandom, kDefaultRandomPercent, nullptr);
        else
            state.setProperty(Mutator::kRandom,
                              clampRecipePercent(static_cast<int>(state.getProperty(Mutator::kRandom))),
                              nullptr);

        initializeModeAndPitchState(state);

        for (const auto* toggleId : detail::kRecipeModuleToggleIds)
        {
            if (! state.hasProperty(toggleId))
                state.setProperty(toggleId, false, nullptr);
        }
    }

    inline void stripEphemeralStateForPersistence(juce::ValueTree& state)
    {
        namespace S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

        state.removeProperty(S::kHistoryMutateList, nullptr);
        state.removeProperty(S::kHistoryRetryList, nullptr);
        state.removeProperty(S::kHistoryRetryListsByRoot, nullptr);
        state.removeProperty(S::kSelectedMutateRootIndex, nullptr);
        state.removeProperty(S::kSelectedRetryIndex, nullptr);
        state.removeProperty(S::kCompareActive, nullptr);
        state.removeProperty(S::kInitialSelected, nullptr);
        state.removeProperty(S::kInitialSnapshotAvailable, nullptr);
        state.removeProperty(S::kMutateEnabled, nullptr);
        state.removeProperty(S::kRetryEnabled, nullptr);
        state.removeProperty(S::kExportEnabled, nullptr);
        state.removeProperty(S::kDeleteEnabled, nullptr);
        state.removeProperty(S::kClearEnabled, nullptr);
        state.removeProperty(S::kMutateAllocationBlocked, nullptr);
        state.removeProperty(S::kRetryAllocationBlocked, nullptr);
        state.removeProperty(S::kMutateDefragRecovery, nullptr);
        state.removeProperty(S::kRetryDefragRecovery, nullptr);
        // Patch name is session-ephemeral (D-010: no claimed loaded patch across sessions).
        state.removeProperty(PluginIDs::PatchEditSection::PatchNameModule::kPatchName, nullptr);
    }

    inline void resetEphemeralStateAfterSessionLoad(juce::ValueTree& state)
    {
        namespace S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName;

        state.setProperty(S::kHistoryMutateList, juce::String(), nullptr);
        state.setProperty(S::kHistoryRetryList, juce::String(), nullptr);
        state.setProperty(S::kHistoryRetryListsByRoot, juce::String(), nullptr);
        state.setProperty(S::kSelectedMutateRootIndex, -1, nullptr);
        state.setProperty(S::kSelectedRetryIndex, S::kSelectedRetryRootOnly, nullptr);
        state.setProperty(S::kCompareActive, false, nullptr);
        state.setProperty(S::kInitialSelected, false, nullptr);
        state.setProperty(S::kInitialSnapshotAvailable, false, nullptr);
        // Force idle name even if stale XML still embeds a previous session name (crash / legacy).
        state.setProperty(PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
                          juce::String(kDefaultPatchName),
                          nullptr);
    }

    inline void setActionEnabledMirrorsForEmptyHistory(juce::ValueTree& state)
    {
        namespace S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

        state.setProperty(S::kMutateEnabled, anyRecipeModuleToggleEnabled(state), nullptr);
        state.setProperty(S::kRetryEnabled, false, nullptr);
        state.setProperty(S::kExportEnabled, false, nullptr);
        state.setProperty(S::kDeleteEnabled, false, nullptr);
        state.setProperty(S::kClearEnabled, false, nullptr);
        state.setProperty(S::kMutateAllocationBlocked, false, nullptr);
        state.setProperty(S::kRetryAllocationBlocked, false, nullptr);
        state.setProperty(S::kMutateDefragRecovery, false, nullptr);
        state.setProperty(S::kRetryDefragRecovery, false, nullptr);
    }
} // namespace Core::MutatorSessionPersistence
