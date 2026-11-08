#pragma once

#include <functional>
#include <memory>

#include <juce_events/juce_events.h>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    class BridgeServer : private juce::InterprocessConnectionServer
    {
    public:
        BridgeServer();
        ~BridgeServer() override;

        bool start();
        void stop();

        /** Called on message thread after a push is decoded. Return ack/error. */
        std::function<ApplyResult(const PushPayload&)> onPush;

        /** Editor disconnected: Standalone should fall back to compiled Black/Cream. */
        std::function<void()> onClientDisconnected;

    private:
        juce::InterprocessConnection* createConnectionObject() override;

        class Connection;
        struct LifetimeToken;

        std::unique_ptr<Connection> activeConnection_;
        juce::ReferenceCountedObjectPtr<LifetimeToken> lifetime_;
        bool listening_ = false;
    };
}
