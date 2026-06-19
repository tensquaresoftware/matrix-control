#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Services/PatchFileService.h"

namespace Core
{

    namespace PatchFileServiceFooter
    {
        inline void propagateScanResult(juce::AudioProcessorValueTreeState& apvts,
                                        const PatchFolderScanResult& result)
        {
            if (result.footerMessage.isEmpty())
                return;

            apvts.state.setProperty("uiMessageText", result.footerMessage, nullptr);
            apvts.state.setProperty("uiMessageSeverity", result.footerSeverity, nullptr);
        }
    }

} // namespace Core
