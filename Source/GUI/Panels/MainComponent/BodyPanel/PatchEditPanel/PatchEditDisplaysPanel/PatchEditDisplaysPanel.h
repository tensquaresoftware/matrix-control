#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/EnvelopeDisplay.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "GUI/Widgets/TrackGeneratorDisplay.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace tss
{
    class ISkin;
}

class InteractiveDisplayApvtsSync;
class PatchEditBottomModulesPanel;
class PatchEditTopModulesPanel;

class PatchEditDisplaysPanel : public juce::Component
{
public:
    PatchEditDisplaysPanel(tss::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditDisplaysPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    void connectSliderFastPaths(PatchEditTopModulesPanel& topModulesPanel,
                                PatchEditBottomModulesPanel& bottomModulesPanel);

private:
    int width_;
    int height_;
    tss::ISkin* skin_;
    juce::AudioProcessorValueTreeState* apvts_ = nullptr;
    float uiScale_ = 1.0f;

    tss::EnvelopeDisplay envelope1Display_;
    tss::EnvelopeDisplay envelope2Display_;
    tss::EnvelopeDisplay envelope3Display_;
    tss::TrackGeneratorDisplay trackGeneratorDisplay_;
    tss::ModuleHeader patchNameModuleHeader_;
    tss::PatchNameDisplay patchNameDisplay_;
    std::unique_ptr<InteractiveDisplayApvtsSync> apvtsSync_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditDisplaysPanel)
};
