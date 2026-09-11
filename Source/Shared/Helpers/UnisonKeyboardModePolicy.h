#pragma once

#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace TSS::UnisonKeyboardModePolicy
{
    /** SysEx / UI index for Keyboard Mode = UNISON (0=REASGN, 1=ROTATE, 2=UNISON, 3=REAROB). */
    constexpr int kUnisonKeyboardModeIndex = 2;

    inline bool isUnisonKeyboardModeIndex(int keyboardModeIndex) noexcept
    {
        return keyboardModeIndex == kUnisonKeyboardModeIndex;
    }

    /** LEGATO PORTA is available only when Keyboard Mode is UNISON. */
    inline bool isLegatoPortaEnabled(int keyboardModeIndex) noexcept
    {
        return isUnisonKeyboardModeIndex(keyboardModeIndex);
    }

    /** STRIG is meaningful only when Keyboard Mode is UNISON. */
    inline bool isStrigAllowed(int keyboardModeIndex) noexcept
    {
        return isUnisonKeyboardModeIndex(keyboardModeIndex);
    }

    /**
     * Master-override badge: Matrix-1000 only, Master Unison ON, Keyboard Mode ≠ UNISON.
     */
    inline bool shouldShowMasterOverrideBadge(MatrixDeviceTypes::Type deviceType,
                                              bool masterUnisonOn,
                                              int keyboardModeIndex) noexcept
    {
        return deviceType == MatrixDeviceTypes::Type::kMatrix1000
            && masterUnisonOn
            && ! isUnisonKeyboardModeIndex(keyboardModeIndex);
    }

    inline int firstNonStrigChoiceIndex(int strigChoiceIndex, int numChoices) noexcept
    {
        for (int i = 0; i < numChoices; ++i)
        {
            if (i != strigChoiceIndex)
                return i;
        }

        return 0;
    }
}
