#pragma once

#include <functional>

#include <juce_events/juce_events.h>

#include "GUI/Skins/Dev/DevSkinBridgeSession.h"
#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    class BridgeClient : private juce::InterprocessConnection,
                         private juce::Timer
    {
    public:
        BridgeClient();
        ~BridgeClient() override;

        void start();
        void stop();

        bool isConnected() const noexcept { return session_.isConnected(); }
        const BridgeSession& session() const noexcept { return session_; }

        /** Push active layer payload; queues until connected. */
        void push(const PushPayload& payload);

        std::function<void(bool connected)> onConnectionChanged;
        std::function<void(const ApplyResult& result)> onApplyResult;

    private:
        void connectionMade() override;
        void connectionLost() override;
        void messageReceived(const juce::MemoryBlock& message) override;
        void timerCallback() override;

        void tryConnect();
        void sendPayload(const PushPayload& payload);
        void handleReply(const juce::String& json);

        BridgeSession session_;
        bool started_ = false;
    };
}
