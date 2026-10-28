#pragma once

#include <cstdint>

#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
// After Universal Device Inquiry times out: run Master-parameter fallback when the
// synth is still undetected, or when a port-pair change forced a fresh identity attempt.
// Soft-abort presence (already detected, same pair) and outbound-busy deferral must not
// enter fallback — callers gate those paths before invoking this.
inline bool shouldRunMatrix6MasterFallbackOnInquiryTimeout(
    bool deviceDetected,
    bool forceMasterPullOnNextInquirySuccess) noexcept
{
    if (! deviceDetected)
        return true;

    return forceMasterPullOnNextInquirySuccess;
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
