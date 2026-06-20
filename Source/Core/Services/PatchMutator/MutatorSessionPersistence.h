#pragma once

#include <juce_data_structures/juce_data_structures.h>

#include "Shared/Definitions/PluginIDs.h"

namespace Core::MutatorSessionPersistence
{
    inline void initializeRecipeState(juce::ValueTree& state)
    {
        namespace Mutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

        if (! state.hasProperty(Mutator::kAmount))
            state.setProperty(Mutator::kAmount, 0, nullptr);

        if (! state.hasProperty(Mutator::kRandom))
            state.setProperty(Mutator::kRandom, 0, nullptr);

        const char* toggleIds[] = {
            Mutator::kEnableDco1,
            Mutator::kEnableDco2,
            Mutator::kEnableVcfVca,
            Mutator::kEnableFmTrack,
            Mutator::kEnableRampPortamento,
            Mutator::kEnableEnvelope1,
            Mutator::kEnableEnvelope2,
            Mutator::kEnableEnvelope3,
            Mutator::kEnableLfo1,
            Mutator::kEnableLfo2
        };

        for (const auto* toggleId : toggleIds)
        {
            if (! state.hasProperty(toggleId))
                state.setProperty(toggleId, false, nullptr);
        }
    }

    inline void stripEphemeralStateForPersistence(juce::ValueTree& state)
    {
        namespace S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

        state.removeProperty(S::kHistoryMList, nullptr);
        state.removeProperty(S::kHistoryRList, nullptr);
        state.removeProperty(S::kSelectedM, nullptr);
        state.removeProperty(S::kSelectedR, nullptr);
        state.removeProperty(S::kCompareActive, nullptr);
        state.removeProperty(S::kMutateEnabled, nullptr);
        state.removeProperty(S::kRetryEnabled, nullptr);
        state.removeProperty(S::kExportEnabled, nullptr);
        state.removeProperty(S::kDeleteEnabled, nullptr);
        state.removeProperty(S::kClearEnabled, nullptr);
    }

    inline void resetEphemeralStateAfterSessionLoad(juce::ValueTree& state)
    {
        namespace S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

        state.setProperty(S::kHistoryMList, juce::String(), nullptr);
        state.setProperty(S::kHistoryRList, juce::String(), nullptr);
        state.setProperty(S::kSelectedM, -1, nullptr);
        state.setProperty(S::kSelectedR, S::kSelectedRRootOnly, nullptr);
        state.setProperty(S::kCompareActive, false, nullptr);
    }

    inline void setActionEnabledMirrorsForEmptyHistory(juce::ValueTree& state)
    {
        namespace S = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

        state.setProperty(S::kMutateEnabled, true, nullptr);
        state.setProperty(S::kRetryEnabled, false, nullptr);
        state.setProperty(S::kExportEnabled, false, nullptr);
        state.setProperty(S::kDeleteEnabled, false, nullptr);
        state.setProperty(S::kClearEnabled, false, nullptr);
    }
} // namespace Core::MutatorSessionPersistence
