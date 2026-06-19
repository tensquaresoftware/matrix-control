#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/Init/InitTemplateLoader.h"

namespace Core
{

    namespace InitTemplateFooter
    {
        inline void propagateMessage(juce::AudioProcessorValueTreeState& apvts,
                                     const InitTemplateLoadResult& result)
        {
            if (result.infoMessage.isEmpty())
            {
                ExceptionPropagator::clearMessage(apvts);
                return;
            }

            apvts.state.setProperty("uiMessageText", result.infoMessage, nullptr);

            const auto severity = (result.fallbackReason == InitTemplateFallbackReason::kFileInvalid)
                ? juce::String("warning")
                : juce::String("info");
            apvts.state.setProperty("uiMessageSeverity", severity, nullptr);
        }
    }

} // namespace Core
