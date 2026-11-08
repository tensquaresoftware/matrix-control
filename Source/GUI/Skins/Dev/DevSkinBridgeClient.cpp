#include "GUI/Skins/Dev/DevSkinBridgeClient.h"

#include "GUI/Skins/Dev/DevSkinBridgeProtocol.h"
#include "GUI/Skins/Dev/DevSkinJson.h"

namespace TSS::DevSkin
{
    BridgeClient::BridgeClient()
        : juce::InterprocessConnection(true, 0xf2c91a11)
    {
    }

    BridgeClient::~BridgeClient()
    {
        stop();
    }

    void BridgeClient::start()
    {
        if (started_)
            return;
        started_ = true;
        startTimerHz(2);
        tryConnect();
    }

    void BridgeClient::stop()
    {
        started_ = false;
        stopTimer();
        disconnect();
        session_.markDisconnected();
    }

    void BridgeClient::push(const PushPayload& payload)
    {
        if (const auto outbound = session_.queuePush(payload))
            sendPayload(*outbound);
    }

    void BridgeClient::connectionMade()
    {
        session_.markConnected();
        if (onConnectionChanged)
            onConnectionChanged(true);
        if (const auto outbound = session_.takePushOnConnect())
            sendPayload(*outbound);
    }

    void BridgeClient::connectionLost()
    {
        session_.markDisconnected();
        if (onConnectionChanged)
            onConnectionChanged(false);
    }

    void BridgeClient::messageReceived(const juce::MemoryBlock& message)
    {
        juce::String json;
        if (! decodeMessage(message, json))
            return;
        handleReply(json);
    }

    void BridgeClient::timerCallback()
    {
        if (! session_.isConnected())
            tryConnect();
    }

    void BridgeClient::tryConnect()
    {
        if (session_.isConnected())
            return;
        connectToSocket("127.0.0.1", kBridgePort, 200);
    }

    void BridgeClient::sendPayload(const PushPayload& payload)
    {
        const auto json = juce::JSON::toString(pushPayloadToVar(payload));
        if (! sendMessage(encodeMessage(json)))
        {
            session_.retainPending(payload);
            if (onApplyResult)
            {
                ApplyResult failure;
                failure.ok = false;
                failure.code = "send_failed";
                failure.message = "Bridge send failed; push kept pending";
                failure.revision = payload.revision;
                onApplyResult(failure);
            }
        }
    }

    void BridgeClient::handleReply(const juce::String& json)
    {
        const auto parsed = juce::JSON::parse(json);
        auto* obj = parsed.getDynamicObject();
        if (obj == nullptr)
            return;

        ApplyResult result;
        const auto type = obj->getProperty("type").toString();
        if (type == "ack")
        {
            result.ok = true;
            result.appliedSkinId = obj->getProperty("skinId").toString();
            result.revision = static_cast<juce::int64>(static_cast<int>(obj->getProperty("revision")));
            result.message = "Applied";
        }
        else if (type == "error")
        {
            result.ok = false;
            result.code = obj->getProperty("code").toString();
            result.message = obj->getProperty("message").toString();
            result.revision = static_cast<juce::int64>(static_cast<int>(obj->getProperty("revision")));
        }
        else
        {
            return;
        }

        if (onApplyResult)
            onApplyResult(result);
    }
}
