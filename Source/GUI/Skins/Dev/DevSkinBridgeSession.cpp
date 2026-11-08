#include "GUI/Skins/Dev/DevSkinBridgeSession.h"

namespace TSS::DevSkin
{
    void BridgeSession::markConnected()
    {
        connected_ = true;
    }

    void BridgeSession::markDisconnected()
    {
        connected_ = false;
    }

    std::optional<PushPayload> BridgeSession::queuePush(const PushPayload& payload)
    {
        pending_ = payload;
        hasPending_ = true;
        if (connected_)
            return payload;
        return std::nullopt;
    }

    std::optional<PushPayload> BridgeSession::takePushOnConnect()
    {
        if (! hasPending_)
            return std::nullopt;
        hasPending_ = false;
        return pending_;
    }

    void BridgeSession::retainPending(const PushPayload& payload)
    {
        pending_ = payload;
        hasPending_ = true;
    }

    juce::String BridgeSession::connectionStatusText() const
    {
        if (connected_)
            return "Connection: connected";
        return "Connection: disconnected";
    }
}
