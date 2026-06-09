#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;
class Env1Panel;
class Env2Panel;
class Env3Panel;
class Lfo1Panel;
class Lfo2Panel;

class PatchEditBottomModulesPanel : public juce::Component
{
public:
    PatchEditBottomModulesPanel(TSS::ISkin& skin,
                                const PatchEditModulesRowDimensions& rowDims,
                                int width,
                                int height,
                                const ParameterCellDimensions& parameterCellDims,
                                const ModuleHeaderDimensions& moduleHeaderDims,
                                WidgetFactory& widgetFactory,
                                juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditBottomModulesPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    
    Env1Panel* getEnv1Panel() { return env1Panel_.get(); }
    Env2Panel* getEnv2Panel() { return env2Panel_.get(); }
    Env3Panel* getEnv3Panel() { return env3Panel_.get(); }

private:
    PatchEditModulesRowDimensions rowDims_;
    int width_;
    int height_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<Env1Panel> env1Panel_;
    std::unique_ptr<Env2Panel> env2Panel_;
    std::unique_ptr<Env3Panel> env3Panel_;
    std::unique_ptr<Lfo1Panel> lfo1Panel_;
    std::unique_ptr<Lfo2Panel> lfo2Panel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditBottomModulesPanel)
};
