#pragma once

#include <cstdint>

#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
// After Universal Device Inquiry times out: run Master-parameter fallback when the
// synth is still undetected, when a port-pair change forced a fresh identity attempt,
// or when presence heartbeat needs to re-prove an Inquiry-silent Matrix-6 family device.
// Matrix-1000 presence silence and outbound-busy deferral must not enter fallback —
// callers gate those paths before invoking this.
inline bool shouldRunMatrix6MasterFallbackOnInquiryTimeout(
    bool deviceDetected,
    bool forceMasterPullOnNextInquirySuccess,
    MatrixDeviceTypes::Type detectedType) noexcept
{
    if (! deviceDetected)
        return true;

    if (forceMasterPullOnNextInquirySuccess)
        return true;

    // Presence: Inquiry-silent Matrix-6 / 6R — re-check with Master before soft-abort.
    return MatrixDeviceTypes::isMatrix6Family(detectedType);
}

// Master fallback failed: soft-abort (keep detection) only for Matrix-6 family presence
// re-check. First-connect and port-pair force keep full clear-detect failure.
inline bool shouldSoftAbortOnMatrix6MasterFallbackFailure(
    bool deviceDetected,
    MatrixDeviceTypes::Type detectedType,
    bool forceMasterPullOnNextInquirySuccess) noexcept
{
    if (forceMasterPullOnNextInquirySuccess)
        return false;

    return deviceDetected && MatrixDeviceTypes::isMatrix6Family(detectedType);
}

// Master-fallback identity proof: shared Matrix-6 / 6R family, no invented firmware.
inline MatrixDeviceTypes::Type matrix6MasterFallbackDeviceType() noexcept
{
    return MatrixDeviceTypes::Type::kMatrix6;
}

inline bool matrix6MasterFallbackPreservesStoredEpromType() noexcept
{
    return true;
}

inline const char* matrix6MasterFallbackDeviceVersion() noexcept
{
    return "";
}

// Failure copy after Inquiry timeout then Master timeout (ASCII for logs / error state).
inline const char* matrix6MasterFallbackTimeoutFailureMessage() noexcept
{
    return "Timeout waiting for device identity (Device Inquiry, then Master parameters)";
}

// Stale async work must no-op when a newer inquiry replaced the token.
inline bool isMatrix6MasterFallbackTokenCurrent(std::uint64_t armedToken,
                                               std::uint64_t currentToken) noexcept
{
    return armedToken == currentToken;
}
} // namespace Core
