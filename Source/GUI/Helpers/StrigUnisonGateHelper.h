#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Shared/Helpers/UnisonKeyboardModePolicy.h"

namespace TSS
{
    class ComboBox;
}

namespace TSS::StrigUnisonGateHelper
{
    constexpr int kUnisonKeyboardModeIndex = UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex;

    bool isKeyboardModeUnison(const juce::AudioProcessorValueTreeState& apvts);

    struct TriggerRefreshRequest
    {
        juce::AudioProcessorValueTreeState& apvts;
        ComboBox* combo = nullptr;
        juce::String triggerParameterId;
        int strigChoiceIndex = 0;
        bool clearIfCurrentStrig = false;
    };

    /**
     * When Keyboard Mode ≠ UNISON: disable the STRIG popup item.
     * When clearIfCurrentStrig is true and the current value is STRIG, fall back to the
     * first non-STRIG choice and show the INFO footer (use when Keyboard Mode leaves UNISON).
     * When Keyboard Mode = UNISON: re-enable STRIG. Other choices stay editable either way.
     */
    void refreshTriggerParameter(const TriggerRefreshRequest& request);

    /**
     * Listens to Keyboard Mode. Callback receives true when the mode just left UNISON
     * (use that to clear STRIG); false for enablement-only refresh.
     */
    class KeyboardModeChangeListener : private juce::AudioProcessorValueTreeState::Listener
    {
    public:
        KeyboardModeChangeListener(juce::AudioProcessorValueTreeState& apvts,
                                   std::function<void(bool leftUnison)> onKeyboardModeChanged);
        ~KeyboardModeChangeListener() override;

        KeyboardModeChangeListener(const KeyboardModeChangeListener&) = delete;
        KeyboardModeChangeListener& operator=(const KeyboardModeChangeListener&) = delete;

    private:
        void parameterChanged(const juce::String& parameterID, float newValue) override;

        juce::AudioProcessorValueTreeState& apvts_;
        std::function<void(bool leftUnison)> onKeyboardModeChanged_;
        bool wasUnison_ = false;
    };
}
