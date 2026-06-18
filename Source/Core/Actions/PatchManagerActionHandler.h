#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Actions/IActionHandler.h"
#include "Core/Services/DeviceMemoryLimits.h"

namespace Core
{

    class PatchManagerActionHandler final : public IActionHandler
    {
    public:
        using DeviceMemoryLimitsSupplier = std::function<DeviceMemoryLimits()>;

        PatchManagerActionHandler(juce::AudioProcessorValueTreeState& apvts,
                                  DeviceMemoryLimitsSupplier deviceMemoryLimits);

        void handleAction(const juce::String& propertyId, const juce::var& newValue) override;

    private:
        void applyPatchCoordinates(const PatchCoordinates& coordinates);
        int parseBankButtonIndex(const juce::String& propertyId) const;

        juce::AudioProcessorValueTreeState& apvts_;
        DeviceMemoryLimitsSupplier deviceMemoryLimits_;
    };

} // namespace Core
