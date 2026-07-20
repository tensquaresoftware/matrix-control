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
    // Kept for Stories 10.2 / 10.3 (direct display ↔ APVTS wiring).
    [[maybe_unused]] juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    float uiScale_ = 1.0f;

    TSS::EnvelopeDisplay envelope1Display_;
    TSS::EnvelopeDisplay envelope2Display_;
    TSS::EnvelopeDisplay envelope3Display_;
    TSS::TrackGeneratorDisplay trackGeneratorDisplay_;
    std::unique_ptr<PatchNameDisplayPanel> patchNameDisplayPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditDisplaysPanel)
};
