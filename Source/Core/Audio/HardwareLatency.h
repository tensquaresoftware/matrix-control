#pragma once

namespace Core
{
namespace HardwareLatency
{
    inline constexpr float kMinMs = 0.0f;
    inline constexpr float kMaxMs = 100.0f;
    inline constexpr float kStepMs = 0.1f;

    float clampMs(float ms) noexcept;
    float quantizeMs(float ms) noexcept;
    int msToSamples(float ms, double sampleRate) noexcept;
}
}
