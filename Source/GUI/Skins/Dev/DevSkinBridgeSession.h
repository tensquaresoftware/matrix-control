#pragma once

#include <optional>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace TSS::DevSkin
{
    /**
     * Connection + pending-push policy for the Skin Editor thin bridge (no sockets).
     * BridgeClient drives this; unit tests cover offline queue and push-on-connect.
     */
    class BridgeSession
    {
    public:
        bool isConnected() const noexcept { return connected_; }
        bool hasPendingPush() const noexcept { return hasPending_; }
        const PushPayload* pendingPush() const noexcept
        {
            return hasPending_ ? &pending_ : nullptr;
        }

        void markConnected();
        void markDisconnected();

        /** Queue payload; returns it when already connected (caller should send). */
        std::optional<PushPayload> queuePush(const PushPayload& payload);

        /** On connect: return pending payload to send, then clear pending. */
        std::optional<PushPayload> takePushOnConnect();

        /** Re-queue after a failed send so reconnect/retry can push again. */
        void retainPending(const PushPayload& payload);

        /** Status text for the editor connection label (no product preview). */
        juce::String connectionStatusText() const;

    private:
        bool connected_ = false;
        bool hasPending_ = false;
        PushPayload pending_;
    };
}
