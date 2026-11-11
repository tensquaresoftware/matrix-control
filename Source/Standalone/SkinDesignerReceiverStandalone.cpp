#if defined(MATRIX_SKIN_DESIGNER_RECEIVER)

#include "Standalone/SkinDesignerReceiverStandalone.h"

#include "GUI/PluginEditor.h"
#include "GUI/Skins/Dev/DevSkinReceiverController.h"

namespace MatrixStandalone
{
    SkinDesignerReceiverHost::SkinDesignerReceiverHost() = default;

    SkinDesignerReceiverHost::~SkinDesignerReceiverHost()
    {
        reset();
    }

    void SkinDesignerReceiverHost::beginAttach(juce::AudioProcessor* processor)
    {
        processor_ = processor;
        attempts_ = 0;
        tryAttachOnce();
        if (receiver_ == nullptr && processor_ != nullptr)
            startTimer(100);
    }

    void SkinDesignerReceiverHost::reset()
    {
        stopTimer();
        receiver_.reset();
        processor_ = nullptr;
        attempts_ = 0;
    }

    void SkinDesignerReceiverHost::timerCallback()
    {
        tryAttachOnce();
        if (receiver_ != nullptr || processor_ == nullptr || attempts_ >= kMaxAttempts)
            stopTimer();
    }

    void SkinDesignerReceiverHost::tryAttachOnce()
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
            DBG("Matrix-Control Skin Designer receiver failed to bind localhost bridge");
            receiver_.reset();
        }
    }

    void attachSkinDesignerReceiverIfNeeded(SkinDesignerReceiverHost& host,
                                          juce::AudioProcessor* processor)
    {
        host.beginAttach(processor);
    }
}

#endif
