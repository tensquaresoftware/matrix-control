#include "HardwareLatency.h"

#include <cmath>

#include <juce_core/juce_core.h>

namespace Core
{
namespace HardwareLatency
{
float clampMs(float ms) noexcept
{
    if (!std::isfinite(ms))
        return kMinMs;

    return juce::jlimit(kMinMs, kMaxMs, ms);
}

float quantizeMs(float ms) noexcept
{
    const float clamped = clampMs(ms);
    return std::round(clamped / kStepMs) * kStepMs;
}

int msToSamples(float ms, double sampleRate) noexcept
{
    if (sampleRate <= 0.0)
        return 0;

    const float quantizedMs = quantizeMs(ms);

    if (quantizedMs <= kMinMs)
        return 0;

    return juce::roundToInt(static_cast<double>(quantizedMs) * sampleRate / 1000.0);
}
}
}
