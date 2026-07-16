#include "GUI/Helpers/CompareLockBinder.h"

#include "Shared/Definitions/PluginIDs.h"

namespace
{
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    constexpr float kLockedAlpha = 0.5f;
}

namespace TSS
{
    CompareLockBinder::CompareLockBinder(juce::AudioProcessorValueTreeState& apvts,
                                         std::vector<juce::Component*> lockedComponents)
        : apvts_(apvts)
    {
        locked_.reserve(lockedComponents.size());
        for (auto* component : lockedComponents)
            locked_.emplace_back(component);

        apvts_.state.addListener(this);
        apply();
    }

    CompareLockBinder::~CompareLockBinder()
    {
        apvts_.state.removeListener(this);
    }

    void CompareLockBinder::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
    {
        if (property.toString() == MutatorState::kCompareActive)
            apply();
    }

    void CompareLockBinder::valueTreeRedirected(juce::ValueTree&)
    {
        apply();
    }

    void CompareLockBinder::apply()
    {
        const bool compareActive = static_cast<bool>(
            apvts_.state.getProperty(MutatorState::kCompareActive, false));

        for (auto& safeComponent : locked_)
        {
            if (auto* component = safeComponent.getComponent())
            {
                // Intercept clicks on the container itself but block children while locked,
                // so every descendant (including interactive displays) stops editing.
                component->setInterceptsMouseClicks(true, ! compareActive);
                component->setAlpha(compareActive ? kLockedAlpha : 1.0f);

                if (compareActive)
                    component->giveAwayKeyboardFocus();
            }
        }
    }
}
