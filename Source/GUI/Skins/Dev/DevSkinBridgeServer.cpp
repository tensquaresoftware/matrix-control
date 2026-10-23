#include "GUI/Skins/Dev/DevSkinBridgeServer.h"

#include "GUI/Skins/Dev/DevSkinBridgeProtocol.h"
#include "GUI/Skins/Dev/DevSkinJson.h"

namespace TSS::DevSkin
{
    struct BridgeServer::LifetimeToken : public juce::ReferenceCountedObject
    {
        BridgeServer* server = nullptr;
    };

    class BridgeServer::Connection : public juce::InterprocessConnection
    {
    public:
        explicit Connection(BridgeServer& owner)
            : juce::InterprocessConnection(true, 0xf2c91a11),
              owner_(owner),
              lifetime_(owner.lifetime_)
        {
        }

        void connectionMade() override {}

        void connectionLost() override
        {
            auto token = lifetime_;
            auto* connection = this;
            juce::MessageManager::callAsync([token, connection]()
            {
                if (token == nullptr || token->server == nullptr)
                    return;

                auto& server = *token->server;
                if (server.activeConnection_.get() != connection)
                    return;

                server.activeConnection_.reset();
                if (server.onClientDisconnected)
                    server.onClientDisconnected();
            });
        }

        void messageReceived(const juce::MemoryBlock& message) override
        {
            juce::String json;
            if (! decodeMessage(message, json))
            {
                replyError("malformed", "Malformed bridge message", 0);
                return;
            }

            const auto parsed = juce::JSON::parse(json);
            PushPayload payload;
            juce::String error;
            if (! pushPayloadFromVar(parsed, payload, error))
            {
                replyError("malformed_payload", error, 0);
                return;
            }

            ApplyResult result;
            if (owner_.onPush)
                result = owner_.onPush(payload);
            else
            {
                result.ok = false;
                result.code = "no_handler";
                result.message = "Receiver not ready";
            }

            if (result.ok)
                replyAck(result);
            else
                replyError(result.code.isNotEmpty() ? result.code : "apply_failed",
                           result.message,
                           result.revision);
        }

    private:
        void replyAck(const ApplyResult& result)
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("type", "ack");
            obj->setProperty("skinId", result.appliedSkinId);
            obj->setProperty("revision", static_cast<int>(result.revision));
            sendMessage(encodeMessage(juce::JSON::toString(juce::var(obj))));
        }

        void replyError(const juce::String& code, const juce::String& message, juce::int64 revision)
        {
            auto* obj = new juce::DynamicObject();
            obj->setProperty("type", "error");
            obj->setProperty("code", code);
            obj->setProperty("message", message);
            obj->setProperty("revision", static_cast<int>(revision));
            sendMessage(encodeMessage(juce::JSON::toString(juce::var(obj))));
        }

        BridgeServer& owner_;
        juce::ReferenceCountedObjectPtr<LifetimeToken> lifetime_;
    };

    BridgeServer::BridgeServer()
        : lifetime_(new LifetimeToken())
    {
        lifetime_->server = this;
    }

    BridgeServer::~BridgeServer()
    {
        stop();
        if (lifetime_ != nullptr)
            lifetime_->server = nullptr;
    }

    bool BridgeServer::start()
    {
        if (listening_)
            return true;
        listening_ = beginWaitingForSocket(kBridgePort, "127.0.0.1");
        return listening_;
    }

    void BridgeServer::stop()
    {
        juce::InterprocessConnectionServer::stop();
        activeConnection_.reset();
        listening_ = false;
    }

    juce::InterprocessConnection* BridgeServer::createConnectionObject()
    {
        if (activeConnection_ != nullptr)
        {
            activeConnection_->disconnect();
            activeConnection_.reset();
            if (onClientDisconnected)
                onClientDisconnected();
        }

        activeConnection_ = std::make_unique<Connection>(*this);
        return activeConnection_.get();
    }
}
