#include "Core/Audio/StandaloneAudioInputRouter.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    __attribute__((weak)) juce::StringArray getInputChannelNames()
    {
        return {};
    }

    __attribute__((weak)) juce::StringArray getInputChannelIds()
    {
        return {};
    }

    __attribute__((weak)) void applySourceId(const juce::String& sourceId)
    {
        juce::ignoreUnused(sourceId);
    }
}
