#pragma once

namespace PluginAudioConstants
{
    /// Finite sentinel for header input gain minimum (−∞ display); maps to linear gain 0.0f.
    inline constexpr float kMinInputGainDb = -120.0f;
    inline constexpr float kMaxInputGainDb = 12.0f;
}
