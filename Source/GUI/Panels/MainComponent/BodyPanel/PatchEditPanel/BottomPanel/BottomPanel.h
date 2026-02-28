#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace tss
{
    class Skin;
}

class WidgetFactory;
class Env1Panel;
class Env2Panel;
class Env3Panel;
class Lfo1Panel;
class Lfo2Panel;

class BottomPanel : public juce::Component
{
public:
    BottomPanel(tss::Skin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~BottomPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);
    
    Env1Panel* getEnv1Panel() { return env1Panel_.get(); }
    Env2Panel* getEnv2Panel() { return env2Panel_.get(); }
    Env3Panel* getEnv3Panel() { return env3Panel_.get(); }

private:
    int width_;
    int height_;
    int childModuleWidth_;
    int childModuleHeight_;
    int spacing_;
    tss::Skin* skin_;

    std::unique_ptr<Env1Panel> env1Panel_;
    std::unique_ptr<Env2Panel> env2Panel_;
    std::unique_ptr<Env3Panel> env3Panel_;
    std::unique_ptr<Lfo1Panel> lfo1Panel_;
    std::unique_ptr<Lfo2Panel> lfo2Panel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BottomPanel)
};

