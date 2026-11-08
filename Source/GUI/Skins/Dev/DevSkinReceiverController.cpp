#if defined(MATRIX_SKIN_EDITOR_RECEIVER)

#include "GUI/Skins/Dev/DevSkinReceiverController.h"

#include "GUI/PluginEditor.h"

namespace TSS::DevSkin
{
    ReceiverController::ReceiverController(PluginEditor& editor)
        : editor_(editor)
    {
        server_.onPush = [this](const PushPayload& payload)
        {
            return handlePush(payload);
        };
        server_.onClientDisconnected = [this]()
        {
            editor_.clearDevSkinOverlay();
        };
    }

    ReceiverController::~ReceiverController()
    {
        stop();
    }

    bool ReceiverController::start()
    {
        return server_.start();
    }

    void ReceiverController::stop()
    {
        server_.stop();
        editor_.clearDevSkinOverlay();
    }

    ApplyResult ReceiverController::handlePush(const PushPayload& payload)
    {
        return editor_.applyDevSkinPush(payload);
    }
}

#endif
