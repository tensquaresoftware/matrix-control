#include "Core/Audio/StandaloneAudioInputRouter.h"

namespace Core::StandaloneAudioInputRouterDetail
{
    juce::StringArray getInputChannelNames();
    juce::StringArray getInputChannelIds();
    void applySourceId(const juce::String& sourceId);
}

namespace Core
{

    juce::StringArray StandaloneAudioInputRouter::getInputChannelNames()
    {
        return StandaloneAudioInputRouterDetail::getInputChannelNames();
    }

    juce::StringArray StandaloneAudioInputRouter::getInputChannelIds()
    {
        return StandaloneAudioInputRouterDetail::getInputChannelIds();
    }

    void StandaloneAudioInputRouter::applySourceId(const juce::String& sourceId)
    {
        StandaloneAudioInputRouterDetail::applySourceId(sourceId);
    }
}
