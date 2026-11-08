#pragma once

#if defined(MATRIX_SKIN_EDITOR_RECEIVER)

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>

#include "GUI/Skins/Dev/DevSkinReceiverController.h"

class PluginEditor;

namespace MatrixStandalone
{
    /** Starts the Skin Editor thin-bridge receiver against the active PluginEditor. */
    class SkinEditorReceiverHost : private juce::Timer
    {
    public:
        SkinEditorReceiverHost();
        ~SkinEditorReceiverHost() override;

        void beginAttach(juce::AudioProcessor* processor);
        void reset();

    private:
        void timerCallback() override;
        void tryAttachOnce();

        juce::AudioProcessor* processor_ = nullptr;
        std::unique_ptr<TSS::DevSkin::ReceiverController> receiver_;
        int attempts_ = 0;
        static constexpr int kMaxAttempts = 50;
    };

    void attachSkinEditorReceiverIfNeeded(SkinEditorReceiverHost& host,
                                          juce::AudioProcessor* processor);
}

#endif
