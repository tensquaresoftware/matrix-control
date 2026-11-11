#pragma once

#if defined(MATRIX_SKIN_DESIGNER_RECEIVER)

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>

#include "GUI/Skins/Dev/DevSkinReceiverController.h"

class PluginEditor;

namespace MatrixStandalone
{
    /** Starts the Skin Designer thin-bridge receiver against the active PluginEditor. */
    class SkinDesignerReceiverHost : private juce::Timer
    {
    public:
        SkinDesignerReceiverHost();
        ~SkinDesignerReceiverHost() override;

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

    void attachSkinDesignerReceiverIfNeeded(SkinDesignerReceiverHost& host,
                                          juce::AudioProcessor* processor);
}

#endif
