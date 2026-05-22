#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
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
    PatchEditBottomModulesPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~PatchEditBottomModulesPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setUiScale(float uiScale);
    
    Env1Panel* getEnv1Panel() { return env1Panel_.get(); }
    Env2Panel* getEnv2Panel() { return env2Panel_.get(); }
    Env3Panel* getEnv3Panel() { return env3Panel_.get(); }

private:
    int width_;
    int height_;
    int childModuleWidth_;
    int childModuleHeight_;
    int gap_;
    tss::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<Env1Panel> env1Panel_;
    std::unique_ptr<Env2Panel> env2Panel_;
    std::unique_ptr<Env3Panel> env3Panel_;
    std::unique_ptr<Lfo1Panel> lfo1Panel_;
    std::unique_ptr<Lfo2Panel> lfo2Panel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchEditBottomModulesPanel)
};

