#if defined(MATRIX_SKIN_EDITOR_RECEIVER)

#include "Standalone/SkinEditorReceiverStandalone.h"

#include "GUI/PluginEditor.h"
#include "GUI/Skins/Dev/DevSkinReceiverController.h"

namespace MatrixStandalone
{
    SkinEditorReceiverHost::SkinEditorReceiverHost() = default;

    SkinEditorReceiverHost::~SkinEditorReceiverHost()
    {
        reset();
    }

    void SkinEditorReceiverHost::beginAttach(juce::AudioProcessor* processor)
    {
        processor_ = processor;
        attempts_ = 0;
        tryAttachOnce();
        if (receiver_ == nullptr && processor_ != nullptr)
            startTimer(100);
    }

    void SkinEditorReceiverHost::reset()
    {
        stopTimer();
        receiver_.reset();
        processor_ = nullptr;
        attempts_ = 0;
    }

    void SkinEditorReceiverHost::timerCallback()
    {
        tryAttachOnce();
        if (receiver_ != nullptr || processor_ == nullptr || attempts_ >= kMaxAttempts)
            stopTimer();
    }

    void SkinEditorReceiverHost::tryAttachOnce()
    {
        if (receiver_ != nullptr || processor_ == nullptr)
            return;

        ++attempts_;
        auto* editor = dynamic_cast<PluginEditor*>(processor_->getActiveEditor());
        if (editor == nullptr)
            return;

        receiver_ = std::make_unique<TSS::DevSkin::ReceiverController>(*editor);
        if (! receiver_->start())
        {
            DBG("Matrix-Control Skin Editor receiver failed to bind localhost bridge");
            receiver_.reset();
        }
    }

    void attachSkinEditorReceiverIfNeeded(SkinEditorReceiverHost& host,
                                          juce::AudioProcessor* processor)
    {
        host.beginAttach(processor);
    }
}

#endif
