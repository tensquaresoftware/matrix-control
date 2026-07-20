#pragma once

#include "Shared/Definitions/PluginIDs.h"

namespace Core
{
    namespace UnsavedEditWarning
    {
        // FR-51 Settings policy: warn when dirty unless the user chose never-warn.
        inline bool shouldPrompt(int policyId, bool isDirty) noexcept
        {
            using namespace PluginIDs::Settings::UnsavedEditWarningPolicy;

            if (! isDirty)
                return false;

            return policyId != kNeverWarn;
        }
    }
} // namespace Core
