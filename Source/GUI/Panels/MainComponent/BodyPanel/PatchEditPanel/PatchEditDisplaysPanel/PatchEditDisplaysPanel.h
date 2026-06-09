#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/EnvelopeDisplay.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "GUI/Widgets/TrackGeneratorDisplay.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    class ISkin;
}

class InteractiveDisplayApvtsSync;
class PatchEditBottomModulesPanel;
class PatchEditTopModulesPanel;

class PatchEditDisplaysPanel : public juce::Component
{
public:
    PatchEditDisplaysPanel(TSS::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditDisplaysPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    void connectSliderFastPaths(PatchEditTopModulesPanel& topModulesPanel,
                                PatchEditBottomModulesPanel& bottomModulesPanel);

private:
    int width_;
    int height_;
    TSS::ISkin* skin_;
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    float uiScale_ = 1.0f;

    TSS::EnvelopeDisplay envelope1Display_;
    TSS::EnvelopeDisplay envelope2Display_;
    TSS::EnvelopeDisplay envelope3Display_;
    TSS::TrackGeneratorDisplay trackGeneratorDisplay_;
    TSS::ModuleHeader patchNameModuleHeader_;
    TSS::PatchNameDisplay patchNameDisplay_;
    std::unique_ptr<InteractiveDisplayApvtsSync> apvtsSync_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditDisplaysPanel)
};
