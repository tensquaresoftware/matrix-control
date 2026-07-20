#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Widgets/EnvelopeDisplay.h"
#include "GUI/Widgets/TrackGeneratorDisplay.h"

namespace TSS
{
    class ISkin;
}

class EnvelopeDisplayApvtsBinding;
class PatchNameDisplayPanel;

class PatchEditDisplaysPanel : public juce::Component
{
public:
    PatchEditDisplaysPanel(TSS::ISkin& skin, const PatchEditDisplaysPanelDimensions& dims, juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditDisplaysPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

private:
    PatchEditDisplaysPanelDimensions dims_;
    TSS::ISkin* skin_;
    // Kept for Story 10.3 (Track Generator display ↔ APVTS wiring).
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    float uiScale_ = 1.0f;

    TSS::EnvelopeDisplay envelope1Display_;
    TSS::EnvelopeDisplay envelope2Display_;
    TSS::EnvelopeDisplay envelope3Display_;
    TSS::TrackGeneratorDisplay trackGeneratorDisplay_;
    std::unique_ptr<PatchNameDisplayPanel> patchNameDisplayPanel_;

    // Declared after displays so bindings are destroyed first (clear callbacks / listeners).
    std::unique_ptr<EnvelopeDisplayApvtsBinding> envelope1Binding_;
    std::unique_ptr<EnvelopeDisplayApvtsBinding> envelope2Binding_;
    std::unique_ptr<EnvelopeDisplayApvtsBinding> envelope3Binding_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditDisplaysPanel)
};
