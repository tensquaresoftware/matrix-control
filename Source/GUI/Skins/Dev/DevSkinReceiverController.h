#pragma once

#if defined(MATRIX_SKIN_EDITOR_RECEIVER)

#include <memory>

#include "GUI/Skins/Dev/DevSkinBridgeServer.h"
#include "GUI/Skins/Dev/DevSkinTypes.h"

class PluginEditor;

namespace TSS::DevSkin
{
    /** Owns the Standalone Dev bridge server and applies pushes to PluginEditor. */
    class ReceiverController
    {
    public:
        explicit ReceiverController(PluginEditor& editor);
        ~ReceiverController();

        bool start();
        void stop();

    private:
        ApplyResult handlePush(const PushPayload& payload);

        PluginEditor& editor_;
        BridgeServer server_;
    };
}

#endif
