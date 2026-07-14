#pragma once

#include <functional>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS::GrayedControlHelper
{
    void applyGrayedAppearance(juce::Component& component, bool grayed);

    void setFooterInfoMessage(juce::AudioProcessorValueTreeState& apvts, const juce::String& message);
    void setFooterWarningMessage(juce::AudioProcessorValueTreeState& apvts, const juce::String& message);

    void setGrayedClickHandler(juce::Component& component,
                               bool grayed,
                               std::function<void()> showFooter);

    void clearGrayedClickHandler(juce::Component& component);
}
