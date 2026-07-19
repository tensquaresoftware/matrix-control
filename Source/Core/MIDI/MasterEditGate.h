#pragma once

#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace Core
{
    /** MASTER EDIT outbound / INIT allowed only for Matrix-1000 while detected (FR-46). */
    inline bool isMasterEditAllowed(bool deviceDetected,
                                    MatrixDeviceTypes::Type deviceType) noexcept
    {
        return deviceDetected && deviceType == MatrixDeviceTypes::Type::kMatrix1000;
    }
}
