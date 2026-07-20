#pragma once

#include <array>
#include <atomic>
#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class EnvelopeDisplay;
}

/**
 * Binds one EnvelopeDisplay to five APVTS int parameters (Delay…Release).
 * Writes via setValueNotifyingHost; refreshes the display from parameter listeners.
 * Does not touch sliders or Env panels.
 */
class EnvelopeDisplayApvtsBinding : private juce::AudioProcessorValueTreeState::Listener
{
public:
    static constexpr int kParameterCount {5};

    using ParameterIds = std::array<const char*, static_cast<size_t>(kParameterCount)>;

    EnvelopeDisplayApvtsBinding(juce::AudioProcessorValueTreeState& apvts,
                                TSS::EnvelopeDisplay& display,
                                const ParameterIds& parameterIds);
    ~EnvelopeDisplayApvtsBinding() override;

private:
    inline static constexpr int kIntParameterMax {63};

    juce::AudioProcessorValueTreeState& apvts_;
    TSS::EnvelopeDisplay& display_;
    ParameterIds parameterIds_;
    juce::AudioProcessorParameter* activeGestureParameter_ {nullptr};
    // Shared so queued callAsync lambdas can still skip mid-drag after this binding is destroyed.
    std::shared_ptr<std::atomic<juce::AudioProcessorParameter*>> activeGestureGate_ {
        std::make_shared<std::atomic<juce::AudioProcessorParameter*>>(nullptr)};

    void connectDisplayCallbacks();
    void syncAllFromApvts();

    int readIntParameter(const juce::String& parameterId) const;
    void writeIntParameter(const juce::String& parameterId, int value);
    void beginParameterGesture(const juce::String& parameterId);
    void endParameterGesture();

    int indexForParameterId(const juce::String& parameterId) const;
    void applyDisplayValue(int paramIndex, int value);

    void parameterChanged(const juce::String& parameterId, float newValue) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeDisplayApvtsBinding)
};
